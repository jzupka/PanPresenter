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

#ifndef BIGIMAGE_H
#define BIGIMAGE_H

#include "cacheimage.h"

class BigImage: public CacheImage
{
public:
    BigImage(QString path, uint max_texture_size);
    BigImage(const BigImage &image);
    ~BigImage();

    void load();
};

#endif // BIGIMAGE_H
