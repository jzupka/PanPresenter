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

#ifndef CACHEIMAGE_H
#define CACHEIMAGE_H

#include <QString>
#include <QImage>
#include <QGLBuffer>
#include <QStringList>
#include <QSize>
#include "loaderimage.h"


class GLBufferImage: public QGLBuffer{
public:
    GLBufferImage(QGLBuffer::Type type, QSize size);
    GLBufferImage();
    int width();
    int height();
private:
    QSize size;
};

class CacheImage: public LoaderImage
{
public:
    CacheImage(QString path, uint max_texture_size);
    CacheImage(const CacheImage &image);
    virtual ~CacheImage();
    int width();
    int height();

    virtual void load(){}

    QSize size();
    QString path;
    unsigned int max_texture_size;
    QVector< QVector<QImage> > images;
    float angle;
    float img_hw;
    bool no_slide;

    static QStringList supported_formats;
protected:
    QSize sz;

private:
    void loadImage(QString path);
};

#endif // CACHEIMAGE_H
