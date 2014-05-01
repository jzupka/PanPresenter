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
    along with darktable.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "paintCore.h"
#include "QDebug"
#include "tools.h"
#include <cmath>


GLImage::GLImage(CacheImage *img, GLWidget *painter, GLuint *textures, int max_texture_size, int loader_id):
    GLAnimatedImage(loader_id)
{
    this->img = img;
    this->angle = img->angle;
    this->no_slide = img->no_slide;
    this->textures = textures;
    this->max_texture_size = max_texture_size;
    this->painter = painter;
    this->loaded = true;
    //this->mouse_pos_constant = this->painter->hw / this->geom->height / 1.5;
    this->init();
}

GLImage::GLImage(const GLImage& image):
    GLAnimatedImage(image.image_id)
{
    this->img = image.img;
    this->textures = image.textures;
    this->max_texture_size = image.max_texture_size;
}

GLImage::~GLImage()
{
    glDeleteTextures(this->sgeom.vbo_data.size(), this->textures);
    delete[] this->textures;
    delete geom;
}

void GLImage::init(){
    this->create_geometry_squear();
    this->create_geometry_cylinder();
    this->geom = 0;
    this->switch_geometry(GT_SQUEAR);
    GLAnimatedImage::init();
}

void GLImage::update_view(){
    if (this->geom->type == GT_SQUEAR){
        if(this->painter->hw < this->geom->height){
            this->img_zoom = 3.0 * this->painter->hw / this->geom->height;
            this->mouse_pos_constant = this->geom->height / this->painter->hw;
        }else{
            this->img_zoom = 3.0;
            this->mouse_pos_constant = 1.0;
        }
    }else if (this->geom->type == GT_CYLINDER){
        this->img_zoom = 2.5 * this->painter->hw / this->geom->height;
        this->mouse_pos_constant = this->geom->height / (this->painter->hw);
    }
}


void GLImage::create_geometry_squear()
{
    //qDebug() << (float)this->img->width() << this->img->height();
    this->sgeom.type = GT_SQUEAR;
    this->sgeom.height = ((float)this->img->height()) / this->img->width();
    this->sgeom.width = 1.0;

    int texture_block_x = int(ceil(((float)this->img->width()) / this->max_texture_size));
    int texture_block_y = int(ceil(((float)this->img->height()) / this->max_texture_size));

    int corners = 20;

    float st_x = 2 * this->sgeom.width / texture_block_x;
    float st_y = 2 * this->sgeom.height / texture_block_y;
    int q = 0;
    float stb = st_x / corners;
    for(int x = 0; x < texture_block_x; ++x){
        for(int y = 0; y < texture_block_y; ++y){
            float *data = static_cast<float*> (::operator new (sizeof(float) * (corners + 1)*6));
            q = 0;
            for(int i = 0; i <= (corners)*6;i+=6){
                data[i] = -this->sgeom.width + q * stb + x * st_x;
                data[i+1] = this->sgeom.height - (y + 1) * st_y;
                data[i+2] = -1.0;
                data[i+3] = -this->sgeom.width + q * stb + x * st_x;
                data[i+4] = this->sgeom.height - y * st_y;
                data[i+5] = -1.0;
                q++;
            }
            this->sgeom.vbo_data.append(data);
        }
    }

    st_x = 1.0 / corners;
    q = 0;
    this->sgeom.tex_data = static_cast<float*> (::operator new (sizeof(float) * (corners + 1)*4));
    for(int i = 0; i < (corners+1)*4;i+=4){
        this->sgeom.tex_data[i] = q * st_x;
        this->sgeom.tex_data[i+1] = 1.0;
        this->sgeom.tex_data[i+2] = q * st_x;
        this->sgeom.tex_data[i+3] = 0.0;
        q++;
    }
    /*for(int i=0; i < (corners+1)*4; ++i){
        qDebug() << this->sgeom.tex_data[i];
    }*/

    this->sgeom.count = (corners+1) * 2;
    this->sgeom.update();
}

void GLImage::create_geometry_cylinder()
{
    //qDebug() << (float)this->img->width() << this->img->height();
    this->cgeom.type = GT_CYLINDER;
    float img_hw = ((float)this->img->height()) / this->img->width();
    this->cgeom.width = 1.0;

    int texture_block_x = int(ceil(((float)this->img->width()) / this->max_texture_size));
    int texture_block_y = int(ceil(((float)this->img->height()) / this->max_texture_size));

    int corners = 20;

    float start_angle = -this->angle / 2;
    float circuid = (this->angle) / texture_block_x;
    float height = (img_hw * circuid * texture_block_x) * 0.5;


    this->cgeom.height = height;
    this->cgeom.width = circuid * texture_block_x;

    float c_alfa = circuid / corners;

    int q = 0;
    float stb = (2.0 * (float)height) / texture_block_y;
    for(int x = 0; x < texture_block_x; ++x){
        for(int y = 0; y < texture_block_y; ++y){
            float *data = static_cast<float*> (::operator new (sizeof(float) * (corners + 1)*6));
            q = 0;
            for(int i = 0; i <= (corners)*6;i+=6){
                data[i] = sin(start_angle + c_alfa * q + x * circuid);
                data[i+1] = height - (y + 1) * stb;
                data[i+2] = -cos(start_angle + c_alfa * q + x * circuid);
                data[i+3] = sin(start_angle + c_alfa * q + x * circuid);
                data[i+4] = height - y * stb;
                data[i+5] = -cos(start_angle + c_alfa * q + x * circuid);
                q++;
            }
            this->cgeom.vbo_data.append(data);
        }
    }

    float st_x = 1.0 / corners;
    q = 0;
    this->cgeom.tex_data = static_cast<float*> (::operator new (sizeof(float) * (corners + 1)*4));
    for(int i = 0; i < (corners+1)*4;i+=4){
        this->cgeom.tex_data[i] = q * st_x;
        this->cgeom.tex_data[i+1] = 1.0;
        this->cgeom.tex_data[i+2] = q * st_x;
        this->cgeom.tex_data[i+3] = 0.0;
        q++;
    }
    /*for(int i=0; i < (corners+1)*4; ++i){
        qDebug() << this->cgeom.tex_data[i];
    }*/

    this->cgeom.count = (corners+1) * 2;
    this->cgeom.update();
}

void GLImage::switch_geometry(char cylinder)
{
    if(this->geom){
        delete geom;
    }
    if (cylinder == GT_SQUEAR){
        this->geom = this->sgeom.clone();
    }else if (cylinder == GT_CYLINDER){
        this->geom = this->cgeom.clone();
    }
    this->geom->update();
    this->update_view();
}

void GLImage::reset()
{
    GLAnimatedImage::reset();
    this->update_view();
}
