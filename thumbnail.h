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

#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QString>
#include <QImage>
#include <QStringList>
#include <QSize>
#include "cacheimage.h"

class Thumbnail: public CacheImage
{
public:
    Thumbnail(QString path, unsigned int max_textures_size, QSize isize);
    Thumbnail(const Thumbnail &image);
    virtual ~Thumbnail();

    void load();
private:
    QSize isize;
};

#endif // THUMBNAIL_H
