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

#ifndef GLIMAGE_H
#define GLIMAGE_H

#include "paintCore.h"
#include "cacheimage.h"
#include "thumbnail.h"



class GLImage: public GLAnimatedImage
{
public:
    GLImage(CacheImage *img, GLWidget *painter, GLuint *textures, int max_texture_size, int loader_id=0);
    GLImage(const GLImage& image);
    virtual ~GLImage();
    void init();
    void reset();
    void update_view();
    void switch_geometry(char cylinder);
    void create_geometry_squear();
    void create_geometry_cylinder();


    CacheImage *img;
};

#endif // GLIMAGE_H
