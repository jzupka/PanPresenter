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

#include "thumbnail.h"

#include <cmath>
#include "tools.h"
#include <QDebug>
#include <QImageReader>
#include <QFileInfo>
#include <omp.h>
#include <cassert>

Thumbnail::Thumbnail(QString path, unsigned int max_texture_size, QSize isize):
    CacheImage(path, max_texture_size)
{
    this->isize = isize;
}

Thumbnail::Thumbnail(const Thumbnail &image):
    CacheImage(image)
{
    this->isize = image.isize;
}


void Thumbnail::load(){
    if (this->is_loaded()){
        //qDebug() << "!!!!!!!!!!!!! Preloaded";
        return;
    }
    QFileInfo media_info(path);
    QString suffix = "*." + media_info.suffix();
    suffix = suffix.toUpper();
    if (this->supported_formats.indexOf(suffix) < 0){
        this->b_loaded = true;
        return;
    }
    QImageReader img_read(path);
    QSize image_size = img_read.size();

    float scale = 0.0;
    float hw_isize = ((float)this->isize.height()) / this->isize.width();
    float hw_image_size = ((float)image_size.height()) / image_size.width();
    if (hw_image_size <= hw_isize){
        scale = (float)this->isize.width() / image_size.width();
    }else{
        scale = (float)this->isize.height() / image_size.height();
    }

    //img_read.setQuality(100.0 / scale_y);
    img_read.setScaledSize(QSize(((int(image_size.width() * scale)>>4)<<4),
                                 ((int(image_size.height() * scale)>>4)<<4)));
    img_read.setQuality(93.0);
    this->images.append(QVector <QImage>());
    this->images[0].append(img_read.read());
    //qDebug() << "Image Size image not loaded." << images[0][0].size();
    this->sz = QSize(this->images[0][0].size());
    this->img_hw = float(this->sz.height()) / this->sz.width();
    this->angle = PI;

    LoaderImage::load();
}


Thumbnail::~Thumbnail()
{
}
