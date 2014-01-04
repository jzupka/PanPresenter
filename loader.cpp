/*
    This file is part of Presenter,
    copyright (c) 2013--2014 Jiří Župka.

    Presenter is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Presenter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Presenter.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "loader.h"
#include <QDebug>

Loader::Loader(int max_texture_size, int cache_size, QSize img_size,QObject * parent):
    QThread(parent)
{
    supported_formats = CacheImage::supported_formats;
    this->max_cache_size = cache_size;
    this->cache_size = cache_size;
    this->pos = 0;
    this->max_texture_size = max_texture_size;
    this->images_count = 0;
    this->load_pos_start = 0;
    this->load_pos_end = 0;
    this->quit = false;
    this->img_size = img_size;
}


Loader::~Loader(){
    this->quit = true;
    this->pos++;
    this->wait_set.wakeAll();
    this->wait(30);
    for (int i = 0; i < images.size(); ++i){
        delete this->images[i];
    }
    images.clear();
}

void Loader::start(){
    QThread::start(Priority::IdlePriority);
}

void Loader::run(){
    int _pos = this->pos;
    while (!quit){
        this->wait_set_mux.lock();
        if (_pos == this->pos){
            wait_set.wait(&this->wait_set_mux);
            _pos = this->pos;
        }else{
            _pos = this->pos;
        }
        this->wait_set_mux.unlock();
        if (quit){
            return;
        }
        this->update_range(_pos);

        this->clean_cache();

        this->images[_pos]->load();
        this->new_image_loaded++;

        int qd, qu;
        //Load Prevs
        for (int i = _pos-1,j = _pos+1; i >= this->load_pos_start && j <= this->load_pos_end; ++j, --i){
            qd = (i + this->images_count) % this->images_count;
            qu = (j + this->images_count) % this->images_count;
            if (_pos != this->pos) break;
            this->images[qu]->load();
            this->new_image_loaded++;
            if (_pos != this->pos) break;
            this->images[qd]->load();
            this->new_image_loaded++;
        }
    }
}


void Loader::clean_cache(){
    this->clean_image_mux.lock();
    QSet<int> to_delete;
    for (int i = 0; i < this->images_count; i++){
        to_delete.insert(i);
    }
    int q = 0;
    for (int i = this->load_pos_start; i <= this->load_pos_end; ++i){
        q = (i + this->images_count) % this->images_count;
        while (q < 0){
            q = (q + images_count) % images_count;
        }
        to_delete.remove(q);
    }
    foreach (int to_del, to_delete) {
        if (this->images[to_del]->is_loaded()){
            delete this->images[to_del];
            this->images[to_del] = 0;
            CacheImage* image;
            if (this->img_size.isValid()){
                image = new Thumbnail(this->files[to_del], this->max_texture_size, this->img_size);
            }else{
                image = new BigImage(this->files[to_del], this->max_texture_size);
            }
            this->images[to_del] = image;
        }
    }
    this->clean_image_mux.unlock();
}


void Loader::update_range(int pos){
    this->load_pos_end = pos + this->cache_size/2;
    this->load_pos_start = pos - this->cache_size/2;
    //qDebug("new range %d %d", this->load_pos_start, this->load_pos_end);
}


void Loader::set(int pos){
    //qDebug() << "update pos:" << pos;
    this->pos = pos;
    this->wait_set.wakeAll();
}

int* Loader::newImageLoaded(){
    return &this->new_image_loaded;
}

void Loader::setFiles(QStringList files){
    this->files = files;
    this->images_count = files.size();
    this->cache_size = this->images_count > this->max_cache_size ? this->max_cache_size : this->images_count;
    for (int i = 0; i < this->images_count; ++i){
        CacheImage* image;
        if (this->img_size.isValid()){
            image = new Thumbnail(files[i], this->max_texture_size, this->img_size);
        }else{
            image = new BigImage(files[i], this->max_texture_size);
        }
        images.append(image);
    }
}
