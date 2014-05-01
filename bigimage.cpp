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

#include "bigimage.h"

#include <cmath>
#include "tools.h"
#include <QDebug>
#include <QImageReader>
#include <QFileInfo>
#include <omp.h>
#include <cassert>

inline void fillRow(uchar *img_row, int out_width, uchar alfa){
    for (int x=0; x < out_width; ++x){
        int out_addr = x << 2;
        img_row[out_addr+3] = alfa;
    }
}


inline void fillLeftSide(uchar *img_row, int out_width, int frame_width){
    for (int x=0; x < frame_width; ++x){
        uchar alfa = ceil((float(x) / frame_width) * 255.0);
        int out_addr = x << 2;
        if (img_row[out_addr+3] > alfa)
            img_row[out_addr+3] = alfa;
    }
}


inline void fillRightSide(uchar *img_row, int out_width, int frame_width){
    for (int x=out_width-frame_width; x < out_width; ++x){
        uchar alfa = ceil((float((out_width-x)) / frame_width) * 255.0);
        int out_addr = x << 2;
        if (img_row[out_addr+3] > alfa)
            img_row[out_addr+3] = alfa;
    }
}

inline void create_frame(uchar *image, int in_width, int in_heigth, int frame_width, float angle){
    uchar *img = image;
    int img_bline = in_width * 4;
    for (int y = 0; y < frame_width; ++y){
        uchar alfa = ceil((float(y) / frame_width) * 255.0);
        int ny = y * img_bline;
        uchar *in_row = img + ny;
        fillRow(in_row, in_width, alfa);
    }
    for (int y = in_heigth-frame_width; y < in_heigth; ++y){
        uchar alfa = ceil((float(in_heigth-y) / frame_width) * 255.0);
        int ny = y * img_bline;
        uchar *in_row = img + ny;
        fillRow(in_row, in_width, alfa);
    }
    if (angle < 2*PI_F){
        for (int y = 0; y < in_heigth; ++y){
            int ny = y * img_bline;
            uchar *in_row = img + ny;
            fillLeftSide(in_row, in_width, frame_width);
            fillRightSide(in_row, in_width, frame_width);
        }
    }
}


inline void copyScaledRowSwith(uchar *out_row, uchar *in_row, int out_width, float scale_x){
    for (int x=0; x < out_width; ++x){
        int in_addr = (((int)floor(x * scale_x))) << 2;
        int out_addr = x << 2;
        out_row[out_addr] = in_row[in_addr+2];
        out_row[out_addr+1] = in_row[in_addr+1];
        out_row[out_addr+2] = in_row[in_addr];
        out_row[out_addr+3] = in_row[in_addr+3];
    }
}


inline void copyScaledRow(uchar *out_row, uchar *in_row, int out_width, float scale_x){
    for (int x=0; x < out_width; ++x){
        int in_addr = (((int)floor(x * scale_x))) << 2;
        int out_addr = x << 2;
        out_row[out_addr] = in_row[in_addr];
        out_row[out_addr+1] = in_row[in_addr+1];
        out_row[out_addr+2] = in_row[in_addr+2];
        out_row[out_addr+3] = in_row[in_addr+3];
    }
}


void copyScaledRect(uchar *out_img, uchar *in_img, int in_width,QRect copy_place, float scale_x, float scale_y, bool smoth, bool swith_color){
    //qDebug() << "*********************Start pos: " << copy_place.top() << "end pos: " << copy_place.top() + copy_place.height() << "scale: scale_y" << scale_y << "Top: " << copy_place.top() * scale_y;
    int in_bline = in_width * 4;
    int out_bline = copy_place.width() * 4;
    uchar *in = in_img;
    uchar *out = out_img;
    // Limit number of working threads without that there could be problem with rendering thread.
    int cpus = omp_get_max_threads()-2;
    if (cpus < 1){
        cpus = 1;
    }
    omp_set_num_threads(cpus);
    #pragma omp parallel
    if (!smoth){
        if (swith_color){
            #pragma omp parallel for
            for (int y=0; y < copy_place.height(); ++y){
                int ny = (int)floor((y + copy_place.top()) * scale_y) * in_bline;
                int nyx = ((int)(floor(copy_place.left() * scale_x))) << 2;
                uchar *in_row = in + ny + nyx;
                uchar *out_row = out + (y * out_bline);
                copyScaledRowSwith(out_row, in_row, copy_place.width(), scale_x);
            }
        }else{
            #pragma omp parallel for
            for (int y=0; y < copy_place.height(); ++y){
                int ny = (int)floor((y + copy_place.top()) * scale_y + 0.5) * in_bline;
                int nyx = ((int)(floor(copy_place.left() * scale_x))) << 2;
                uchar *in_row = in + ny + nyx;
                uchar *out_row = out + (y * out_bline);
                copyScaledRow(out_row, in_row, copy_place.width(), scale_x);
            }
        }
    }else{
        if (!swith_color){
            #pragma omp parallel for
            for (int y=0; y < copy_place.height(); ++y){
                int ny = (int)floor((y + copy_place.top()) * scale_y) * in_bline;
                int nyx = ((int)(floor(copy_place.left() * scale_x))) << 2;
                uchar *in_row = in + ny + nyx;
                uchar *out_row = out + (y * out_bline);

                if ((y == 0) || (y == copy_place.height()-1)){
                    copyScaledRow(out_row, in_row, copy_place.width(), scale_x);
                }else{
                    int in_addr = (((int)floor(0 * scale_x))) << 2;
                    int out_addr = 0 << 2;
                    out_row[out_addr] = in_row[in_addr];
                    out_row[out_addr+1] = in_row[in_addr+1];
                    out_row[out_addr+2] = in_row[in_addr+2];
                    out_row[out_addr+3] = in_row[in_addr+3];
                    for (int x=1; x < copy_place.width()-1; ++x){
                        int in_addr1 = ((((int)floor(x * scale_x)) -1) << 2) - in_bline;
                        int in_addr2 = in_addr1 + (1 << 2);
                        int in_addr3 = in_addr2 + (1 << 2);
                        int in_addr4 = in_addr1 + in_bline;
                        int in_addr5 = in_addr2 + in_bline;
                        int in_addr6 = in_addr3 + in_bline;
                        int in_addr7 = in_addr4 + in_bline;
                        int in_addr8 = in_addr5 + in_bline;
                        int in_addr9 = in_addr6 + in_bline;
                        int out_addr = x << 2;
                        out_row[out_addr] = (in_row[in_addr1] + in_row[in_addr2] + in_row[in_addr3] + in_row[in_addr4] + in_row[in_addr5] + in_row[in_addr6] + in_row[in_addr7] + in_row[in_addr8] + in_row[in_addr9]) / 9;
                        out_row[out_addr+1] = (in_row[in_addr1 + 1] + in_row[in_addr2 + 1] + in_row[in_addr3 + 1] + in_row[in_addr4 + 1] + in_row[in_addr5 + 1] + in_row[in_addr6 + 1] + in_row[in_addr7 + 1] + in_row[in_addr8 + 1] + in_row[in_addr9 + 1]) / 9;
                        out_row[out_addr+2] = (in_row[in_addr1 + 2] + in_row[in_addr2 + 2] + in_row[in_addr3 + 2] + in_row[in_addr4 + 2] + in_row[in_addr5 + 2] + in_row[in_addr6 + 2] + in_row[in_addr7 + 2] + in_row[in_addr8 + 2] + in_row[in_addr9 + 2]) / 9;
                        out_row[out_addr+3] = (in_row[in_addr1 + 3] + in_row[in_addr2 + 3] + in_row[in_addr3 + 3] + in_row[in_addr4 + 3] + in_row[in_addr5 + 3] + in_row[in_addr6 + 3] + in_row[in_addr7 + 3] + in_row[in_addr8 + 3] + in_row[in_addr9 + 3]) / 9;
                    }
                    in_addr = (((int)floor((copy_place.width()-1) * scale_x))) << 2;
                    out_addr = (copy_place.width() - 1) << 2;
                    out_row[out_addr] = in_row[in_addr];
                    out_row[out_addr+1] = in_row[in_addr+1];
                    out_row[out_addr+2] = in_row[in_addr+2];
                    out_row[out_addr+3] = in_row[in_addr+3];
                }
            }
        }else{
            #pragma omp parallel for
            for (int y=0; y < copy_place.height(); ++y){
                int ny = (int)floor((y + copy_place.top()) * scale_y) * in_bline;
                int nyx = ((int)(floor(copy_place.left() * scale_x))) << 2;
                uchar *in_row = in + ny + nyx;
                uchar *out_row = out + (y * out_bline);

                if ((y == 0) || (y == copy_place.height()-1)){
                    copyScaledRowSwith(out_row, in_row, copy_place.width(), scale_x);
                }else{
                    int in_addr = (((int)floor(0 * scale_x))) << 2;
                    int out_addr = 0 << 2;
                    out_row[out_addr] = in_row[in_addr+2];
                    out_row[out_addr+1] = in_row[in_addr+1];
                    out_row[out_addr+2] = in_row[in_addr];
                    out_row[out_addr+3] = in_row[in_addr+3];
                    for (int x=1; x < copy_place.width()-1; ++x){
                        int in_addr1 = ((((int)floor(x * scale_x)) -1) << 2) - in_bline;
                        int in_addr2 = in_addr1 + (1 << 2);
                        int in_addr3 = in_addr2 + (1 << 2);
                        int in_addr4 = in_addr1 + in_bline;
                        int in_addr5 = in_addr2 + in_bline;
                        int in_addr6 = in_addr3 + in_bline;
                        int in_addr7 = in_addr4 + in_bline;
                        int in_addr8 = in_addr5 + in_bline;
                        int in_addr9 = in_addr6 + in_bline;
                        int out_addr = x << 2;
                        out_row[out_addr] = (in_row[in_addr1 + 2] + in_row[in_addr2 + 2] + in_row[in_addr3 + 2] + in_row[in_addr4 + 2] + in_row[in_addr5 + 2] + in_row[in_addr6 + 2] + in_row[in_addr7 + 2] + in_row[in_addr8 + 2] + in_row[in_addr9 + 2]) / 9;
                        out_row[out_addr+1] = (in_row[in_addr1 + 1] + in_row[in_addr2 + 1] + in_row[in_addr3 + 1] + in_row[in_addr4 + 1] + in_row[in_addr5 + 1] + in_row[in_addr6 + 1] + in_row[in_addr7 + 1] + in_row[in_addr8 + 1] + in_row[in_addr9 + 1]) / 9;
                        out_row[out_addr+2] = (in_row[in_addr1] + in_row[in_addr2] + in_row[in_addr3] + in_row[in_addr4] + in_row[in_addr5] + in_row[in_addr6] + in_row[in_addr7] + in_row[in_addr8] + in_row[in_addr9]) / 9;
                        out_row[out_addr+3] = (in_row[in_addr1 + 3] + in_row[in_addr2 + 3] + in_row[in_addr3 + 3] + in_row[in_addr4 + 3] + in_row[in_addr5 + 3] + in_row[in_addr6 + 3] + in_row[in_addr7 + 3] + in_row[in_addr8 + 3] + in_row[in_addr9 + 3]) / 9;
                    }
                    in_addr = (((int)floor((copy_place.width()-1) * scale_x))) << 2;
                    out_addr = (copy_place.width() - 1) << 2;
                    out_row[out_addr] = in_row[in_addr+2];
                    out_row[out_addr+1] = in_row[in_addr+1];
                    out_row[out_addr+2] = in_row[in_addr];
                    out_row[out_addr+3] = in_row[in_addr+3];
                }
            }
        }
    }
}


BigImage::BigImage(QString path, unsigned int max_texture_size):
    CacheImage(path, max_texture_size)
{
}

BigImage::BigImage(const BigImage &image):
    CacheImage(image)
{
}

void BigImage::load(){
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

    float scale = image_size.height() * 0.5 / 1050;
    if(scale < 1){
        scale = 1.0;
    }
    int tex_count_x = ceil(image_size.width() / (scale * this->max_texture_size));
    int tex_count_y = ceil(image_size.height() / (scale * this->max_texture_size));

    int new_size_x = floor((1.0 * image_size.width()) / (32 * scale * tex_count_x));
    new_size_x *= 32 * tex_count_x;
    int new_size_y = floor((1.0 * image_size.height()) / (32 * scale * tex_count_y));
    new_size_y *= 32 * tex_count_y;


    //qDebug("IMAGE size OLD %d,%d   NEW: %d,%d path:%s", image_size.width(),
    //       image_size.height(),
    //       new_size_x,
    //       new_size_y,
    //       path.toLatin1().data());

    QFileInfo image_info(path);
    QString base_name = image_info.baseName();
    QStringList s_angle = base_name.split("#");

    if (s_angle.size() == 1){
        this->angle = 180;
    }else{
        bool ok;
        this->angle = s_angle[1].toInt(&ok);
        if (ok){
            if (this->angle < 0){
                this->angle = -this->angle;
                this->no_slide = true;
            }
        }else{
            this->angle = 180;
            this->no_slide = true;
        }
    }

    if(this->angle == 0){
        this->angle = PI_F;
    }
    if(this->angle < 360){
        this->angle = (fmod(this->angle, 360.0) / 180.0) * PI_F;
    }else if (angle >= 360){
        this->angle = 2 * PI_F;
    }

    float scale_x = ((float)image_size.width()) / new_size_x;
    float scale_y = ((float)image_size.height()) / new_size_y;

    img_read.setQuality(100.0);
    QImage image = img_read.read();
    // qDebug() << image.size();

    create_frame(image.bits(), image.width(), image.height(), image.height()/75.0, this->angle);

    this->sz = QSize(new_size_x, new_size_y);
    this->img_hw = float(this->sz.height()) / this->sz.width();

    int h_parts = ceil((float)this->width() / max_texture_size);
    int w_parts = ceil((float)this->height() / max_texture_size);
    float p_size_x = (((float)this->width())/h_parts);
    int psize_x = ceil(p_size_x);
    float p_size_y = (((float)this->height())/w_parts);
    int psize_y = ceil(p_size_y);

    for(int i = 0; i < h_parts; i++){
        for (int j = 0; j < w_parts; j++){
            this->images.append(QVector <QImage>());
            int g_index = this->images.size()-1;

            this->images[g_index].append(QImage(QSize(psize_x, psize_y), QImage::Format_ARGB32));
            copyScaledRect(this->images[g_index][0].bits(), image.bits(), image.width(),
                           QRect(i * psize_x, j * psize_y, psize_x, psize_y),
                           scale_x, scale_y, scale_x > 1.5, false);

            this->images[g_index].append(QImage(QSize(psize_x / 2, psize_y / 2), QImage::Format_ARGB32));
            copyScaledRect(this->images[g_index][1].bits(), this->images[g_index][0].bits(), this->images[g_index][0].width(),
                           QRect(0, 0, psize_x/2, psize_y/2),
                           2.0, 2.0, true, false);

            this->images[g_index].append(QImage(QSize(psize_x / 4, psize_y / 4), QImage::Format_ARGB32));
            copyScaledRect(this->images[g_index][2].bits(), this->images[g_index][1].bits(), this->images[g_index][1].width(),
                           QRect(0, 0, psize_x / 4, psize_y / 4),
                           2.0, 2.0, true, false);

            this->images[g_index].append(QImage(QSize(psize_x / 8, psize_y / 8), QImage::Format_ARGB32));
            copyScaledRect(this->images[g_index][3].bits(), this->images[g_index][2].bits(), this->images[g_index][2].width(),
                           QRect(0, 0, psize_x / 8, psize_y / 8),
                           2.0, 2.0, true, false);

            this->images[g_index].append(QImage(QSize(psize_x / 16, psize_y / 16), QImage::Format_ARGB32));
            copyScaledRect(this->images[g_index][4].bits(), this->images[g_index][3].bits(), this->images[g_index][3].width(),
                           QRect(0, 0, psize_x / 16, psize_y / 16),
                           2.0, 2.0, true, false);

            this->images[g_index].append(QImage(QSize(psize_x / 32, psize_y / 32), QImage::Format_ARGB32));
            copyScaledRect(this->images[g_index][5].bits(), this->images[g_index][4].bits(), this->images[g_index][4].width(),
                           QRect(0, 0, psize_x / 32, psize_y / 32),
                           2.0, 2.0, true, false);
        }
    }
    LoaderImage::load();
}

BigImage::~BigImage()
{
}
