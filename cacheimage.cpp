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

#include "cacheimage.h"

GLBufferImage::GLBufferImage(QGLBuffer::Type type, QSize size):
    QGLBuffer(type)
{
    this->size = size;
}

GLBufferImage::GLBufferImage():
    QGLBuffer()
{
}

int GLBufferImage::width(){
    return this->size.width();
}

int GLBufferImage::height(){
    return this->size.height();
}


CacheImage::CacheImage(QString path, unsigned int max_texture_size)
{
    this->path = path;
    this->max_texture_size = max_texture_size;
    this->b_loaded = false;
    this->no_slide = false;
}

CacheImage::CacheImage(const CacheImage &image){
    this->sz = image.sz;
    this->max_texture_size = image.max_texture_size;
    this->angle = image.angle;
    this->no_slide = image.no_slide;
    this->images = image.images; //Possible problems.
}


QStringList CacheImage::supported_formats = QStringList() << "*.PNG" << "*.JPG" << "*.RAW" << "*.NEF" << "*.TIF";


CacheImage::~CacheImage()
{
    for (int i = 0; i < this->images.size(); i++){
        this->images[i].clear();
    }
    this->images.clear();
}

int CacheImage::width()
{
    return this->sz.width();
}

int CacheImage::height()
{
    return this->sz.height();
}

QSize CacheImage::size()
{
    return this->sz;
}
