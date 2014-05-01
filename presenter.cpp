/*
    This file is part of Presenter,
    copyright (c) 2012--2013 Jiří Župka.

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


#include <QtGui>
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>
#include "cacheimage.h"
#include "presenter.h"

Pos::Pos(int max_size)
{
    this->max_size = max_size;
    this->pos = 0;
}

Pos::Pos(const Pos &pos){
    this->max_size = pos.max_size;
    this->pos = 0;
}

void Pos::next(){
    this->pos++;
    this->pos %= max_size;
}

void Pos::prev(){
    this->pos--;
    if (this->pos < 0){
        this->pos += this->max_size;
    }
    this->pos %= max_size;
}

int Pos::get(){
    return this->pos;
}

void Pos::set(int pos)
{
    this->pos = pos % max_size;
}

Presenter::Presenter(QMainWindow *parent):
    QMainWindow(parent), pos(0), painter_pos(0)
{
    QGLFormat fmt = QGLFormat::defaultFormat();
    this->image_change_old_time = QTime::currentTime();
    fmt.setDoubleBuffer(true);
    fmt.setSwapInterval(1);
    gl_widget = new GLWidget(fmt, this);
    gl_widget->show();
    gl_widget->makeCurrent();
    gl_widget->initGL();
    this->loader = new Loader(gl_widget->max_texture_size, 5);
    this->loader->start();
    this->tloader = new Loader(gl_widget->max_texture_size, 50, QSize(200, 200));
    this->tloader->start();
    gl_widget->setNewImageLoaded(this->tloader->newImageLoaded(),
                                 &this->tloader->clean_image_mux);
    this->setCentralWidget(gl_widget);
    this->Image_Ext = loader->supported_formats;
    this->Video_Ext << "*.AVI" << "*.WEBM" << "*.MOV" << "*.M4V" << "*.OGV" << "*.MP4";

    setWindowTitle(tr("Textures"));
    connect(&this->image_change, SIGNAL(timeout()), this, SLOT(update_pos()));
    connect(this->gl_widget, SIGNAL(set_new_pos(int)), this, SLOT(set_new_pos(int)));
    connect(this->gl_widget, SIGNAL(scroll_new_pos(int)), this, SLOT(scroll_new_pos(int)));
    this->image_change.start(100);
}

Presenter::~Presenter()
{
    delete gl_widget;
    delete loader;
    delete tloader;
}

void Presenter::update_pos(){
    if (this->painter_pos != this->pos.get()){
        this->painter_pos = this->pos.get();
        this->set_media(this->pos.get(), this->files[this->pos.get()]);
    }
}


void Presenter::setPath(QStringList path)
{
    QString first_img;
    if(path.length() < 2){
        QDir dir;
        if (QFileInfo(path[0]).isFile()){
            first_img = path[0];
            dir = QFileInfo(path[0]).absoluteDir();
        }else{
            dir = path[0];
        }
        QStringList files = dir.entryList(this->Image_Ext + this->Video_Ext, QDir::Files);
        foreach(const QString file, files){
            this->files.append(dir.absoluteFilePath(file));
        }
    }else{
        this->files = path;
    }
    this->pos = Pos(this->files.size());
    if (first_img != ""){
        this->pos.set(this->files.indexOf(first_img));
    }
    this->loader->setFiles(this->files);
    this->tloader->setFiles(this->files);
    this->set_media(this->pos.get(), this->files[this->pos.get()]);
    this->setWindowTitle(this->files[this->pos.get()]);
}


void Presenter::set_next(QMediaPlayer::State state){
    if (state == QMediaPlayer::StoppedState){
        this->pos.next();
        //qDebug() << "Pressed" << QTime::currentTime();
        this->setWindowTitle(this->files[this->pos.get()]);
        //QTime end = QTime::currentTime();
        //qDebug() << "Pressed end" << QTime::currentTime();
    }
}

void Presenter::set_new_pos(int pos){
    this->pos.set(pos);
}

void Presenter::scroll_new_pos(int pos){
    tloader->set(pos);
}

void Presenter::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Up || event->key() == Qt::Key_Right ||
        event->key() == Qt::Key_Down || event->key() == Qt::Key_Left){
        QTime time = QTime::currentTime();
        if (this->image_change_old_time.msecsTo(time) > 750){
            this->image_change_old_time = time;
        }else{
            return;
        }
    }
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Up || event->key() == Qt::Key_Right){
        this->pos.next();
        this->setWindowTitle(this->files[this->pos.get()]);
    } else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Left){
        this->pos.prev();
        this->setWindowTitle(this->files[this->pos.get()]);
    } else if (event->key() == Qt::Key_Escape){
        QApplication::closeAllWindows();
        QApplication::exit();
        QCoreApplication::instance()->quit();
    } else if (event->key() == Qt::Key_A){
        this->gl_widget->anim(15.0);
    } else if (event->key() == Qt::Key_F){
        this->setWindowState(this->windowState() ^ Qt::WindowFullScreen);
        this->show();
    } else if (event->key() == Qt::Key_C){
        gl_widget->cylindrical();
    } else if (event->key() == Qt::Key_R){
        gl_widget->rectangle();
    } else if (event->key() == Qt::Key_D){
        gl_widget->anim_transf(15.0);
    } else if (event->key() == Qt::Key_S){
        gl_widget->anim_transf_no_end(13.0);
    } else if (event->key() == Qt::Key_W){
        gl_widget->anim_back_from(15.0);
    } else if (event->key() == Qt::Key_T){
        gl_widget->transfRC(10.0);
    } else if (event->key() == Qt::Key_U){
        gl_widget->transfCR(10.0);
    } else if (event->key() == Qt::Key_H){
        gl_widget->anim_show(10.0);
    } else if (event->key() == Qt::Key_J){
        gl_widget->anim_hide(10.0);
    } else if (event->key() == Qt::Key_P){
        gl_widget->show_times();
    }
}

char Presenter::set_media(int pos, QString path){
    QFileInfo media_info(path);
    QString suffix = "*." + media_info.suffix();
    suffix = suffix.toUpper();
    if(this->Image_Ext.indexOf(suffix) >= 0){
        this->loader->set(pos);
        this->tloader->set(pos);
        CacheImage * image = this->loader->images[pos];
        //qDebug() << "Waiting start gui" << QTime::currentTime();
        image->wait();
        this->gl_widget->set(image);
        this->gl_widget->setCachedImages(&this->tloader->images, pos);

        //qDebug() << "Waiting end gui" << QTime::currentTime();
    }else if(this->Video_Ext.indexOf(suffix) >= 0){
        this->loader->set(pos);
        this->tloader->set(pos);
        this->gl_widget->setVideoImage(path);
        this->gl_widget->setCachedImages(&this->tloader->images, pos);
        //qDebug() << "Waiting end gui" << QTime::currentTime();
    }else
        return 0;
    return 1;
}
