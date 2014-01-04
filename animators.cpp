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

#include "animators.h"
#include "tools.h"

HideAmin::HideAmin(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    AnimatorBase(animated, painter, anim_time, next_anim)
{
    this->name = "HideAnim";
    this->alfa_start = 0.0;
    this->alfa_end = 0.0;
}

void HideAmin::animate(QTime cur_time){
    AnimatorBase::animate(cur_time);
    float time_part = start_time.msecsTo(cur_time) / anim_time;
    if(time_part > 1){
        time_part = 1.0;
    }
    this->animated->alfa = alfa_start + ((alfa_end - alfa_start) * time_part);
}

void HideAmin::start(){
    this->alfa_start = this->animated->alfa;
    AnimatorBase::start();
}


ShowAmin::ShowAmin(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    HideAmin(animated, painter, anim_time, next_anim)
{
    this->name = "ShowAnim";
    this->alfa_end = 1.0;
}



FuncAnim::FuncAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, void (*func)(const void*), void *args_struct, anim_vec next_anim):
    AnimatorBase(animated, painter, anim_time, next_anim)
{
    this->name = "FuncAnim";
    this->func = func;
    this->args_struct = args_struct;
}

void FuncAnim::start(){
    (*this->func)(args_struct);
}


SlideBeginAnim::SlideBeginAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    AnimatorBase(animated, painter, anim_time, next_anim)
{
    this->name = "SlideBeginAnim";
    this->zoom_start = 0.0;
    this->zoom_end = 0.0;
}

void SlideBeginAnim::animate(QTime cur_time){
    AnimatorBase::animate(cur_time);
    float time_part = this->start_time.msecsTo(cur_time) / anim_time;
    if(time_part > 1){
        time_part = 1.0;
    }
    this->animated->delta[0] = this->delta_start[0] + ((this->delta_end[0] - this->delta_start[0]) * time_part);
    this->animated->delta[1] = this->delta_start[1] + ((this->delta_end[1] - this->delta_start[1]) * time_part);
    memcpy(this->animated->delta_backup, this->animated->delta, sizeof(float) * 3);

    this->animated->zoom = this->zoom_start + ((this->zoom_end - this->zoom_start) * time_part);
}


void SlideBeginAnim::start(){
    this->zoom_start = this->animated->zoom;
    this->zoom_end = painter->hw / animated->geom->height * 0.8;

    memcpy(this->delta_start, this->animated->delta, sizeof(float) * 3);
    this->delta_end[0] = 1.0 - 1.0 / this->zoom_end;
    this->delta_end[1] = 0.0;

    AnimatorBase::start();
}


SlideAnim::SlideAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    SlideBeginAnim(animated, painter, anim_time, next_anim)
{
    this->name = "SlideAnim";
    this->zoom_start = 0.0;
    this->zoom_end = 0.0;
}

void SlideAnim::start(){
    this->zoom_start = this->animated->zoom;
    this->zoom_end = painter->hw / animated->geom->height * 0.8;

    memcpy(this->delta_start, this->animated->delta, sizeof(float) * 3);
    this->delta_end[0] = -1.0 + 1.0 / this->zoom_end;
    this->delta_end[1] = 0.0;

    AnimatorBase::start();
}



SlideEndAnim::SlideEndAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    SlideBeginAnim(animated, painter, anim_time, next_anim)
{
    this->name = "SlideEndAnim";
    this->zoom_start = 0.0;
    this->zoom_end = 0.0;
}

void SlideEndAnim::start(){
    this->zoom_start = this->animated->zoom;
    this->zoom_end = 1.0;

    memcpy(this->delta_start, this->animated->delta, sizeof(float) * 3);
    this->delta_end[0] = 0.0;
    this->delta_end[1] = 0.0;

    AnimatorBase::start();
}


SlideSlowAnim::SlideSlowAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    SlideBeginAnim(animated, painter, anim_time, next_anim)
{
    this->name = "SlideSlowAnim";
    this->zoom_start = 0.0;
    this->zoom_end = 0.0;
}

void SlideSlowAnim::start(){
    this->zoom_start = this->animated->zoom;
    this->zoom_end = this->animated->zoom;

    this->delta_start[0] = -0.03;
    this->delta_start[1] = -0.01;
    this->delta_end[0] = 0.01;
    this->delta_end[1] = 0.01;

    AnimatorBase::start();
}


RotateBeginAnim::RotateBeginAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    AnimatorBase(animated, painter, anim_time, next_anim)
{
    this->name = "RotateBeginAnim";
    this->zoom_start = 0.0;
    this->zoom_end = 0.0;
}


void RotateBeginAnim::animate(QTime cur_time){
    AnimatorBase::animate(cur_time);
    float time_part = this->start_time.msecsTo(cur_time) / anim_time;
    if(time_part > 1){
        time_part = 1.0;
    }
    this->animated->rotate[0] = this->rotate_start[0] + ((this->rotate_end[0] - this->rotate_start[0]) * time_part);
    this->animated->rotate[1] = this->rotate_start[1] + ((this->rotate_end[1] - this->rotate_start[1]) * time_part);
    NormalizeAngle(this->animated->rotate);
    memcpy(this->animated->rotate_backup, this->animated->rotate, sizeof(float) * 3);

    this->animated->zoom = this->zoom_start + ((this->zoom_end - this->zoom_start) * time_part);
}


void RotateBeginAnim::start(){
    this->zoom_start = this->animated->zoom;
    this->zoom_end = 1.0;

    memcpy(this->rotate_start, this->animated->rotate, sizeof(float) * 3);
    if (this->animated->angle == 2 * PI_F) {
        this->rotate_end[0] = 0.0;
        this->rotate_end[1] = 0.0;
    }else{
        float image_zoom = 2.0 * this->animated->painter->hw / this->animated->cgeom.height;
        this->rotate_end[0] = (-this->animated->angle / 2 + 2.0 / image_zoom) * 180.0 / PI_F;
        this->rotate_end[1] = 0.0;
    }
    memcpy(this->animated->rotate_backup, this->animated->rotate, sizeof(float) * 3);

    AnimatorBase::start();
}


RotateAnim::RotateAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    RotateBeginAnim(animated, painter, anim_time, next_anim)
{
    this->name = "RotateAnim";
    this->zoom_start = 0.0;
    this->zoom_end = 0.0;
}

void RotateAnim::start(){
    this->zoom_start = this->animated->zoom;
    this->zoom_end = 1.0;

    memcpy(this->rotate_start, this->animated->rotate, sizeof(float) * 3);
    if (this->animated->angle == 2 * PI_F) {
        this->rotate_end[0] = 360.0;
        this->rotate_end[1] = 0.0;
    }else{
        this->rotate_end[0] = (this->animated->angle / 2 - 2.5 / this->animated->img_zoom) * 180.0 / PI_F;
        this->rotate_end[1] = 0.0;
    }
    memcpy(this->animated->rotate_backup, this->animated->rotate, sizeof(float) * 3);

    AnimatorBase::start();
}



RotateEndAnim::RotateEndAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    RotateBeginAnim(animated, painter, anim_time, next_anim)
{
    this->name = "RotateEndAnim";
    this->zoom_start = 0.0;
    this->zoom_end = 0.0;
}

void RotateEndAnim::start(){
    this->zoom_start = this->animated->zoom;
    this->zoom_end = 1.0;

    memcpy(this->rotate_start, this->animated->rotate, sizeof(float) * 3);
    this->rotate_end[0] = 0.0;
    this->rotate_end[1] = 0.0;
    memcpy(this->animated->rotate_backup, this->animated->rotate, sizeof(float) * 3);

    AnimatorBase::start();
}


TransformAnimRC::TransformAnimRC(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    AnimatorBase(animated, painter, anim_time, next_anim)
{
    this->name = "TransformAnimRC";
    this->zoom_start = 0.0;
    this->zoom_end = 0.0;
}


void TransformAnimRC::animate(QTime cur_time){
    AnimatorBase::animate(cur_time);
    float time_part = this->start_time.msecsTo(cur_time) / anim_time;
    if(time_part > 1){
        time_part = 1.0;
    }

    for(int i = 0; i < this->geom_start->vbo_data.size(); ++i){
        for (int j = 0; j < this->geom_start->count*3; ++j){
            this->animated->geom->vbo_data[i][j] = this->geom_start->vbo_data[i][j] + ((this->geom_end->vbo_data[i][j] - this->geom_start->vbo_data[i][j]) * time_part);
        }
    }

    this->animated->geom->height = this->geom_start->height + ((this->geom_end->height - this->geom_start->height) * time_part);
    this->animated->geom->width = this->geom_start->width + ((this->geom_end->width - this->geom_start->width) * time_part);

    this->animated->geom->update();

    this->animated->delta[0] = this->delta_start[0] + ((this->delta_end[0] - this->delta_start[0]) * time_part);
    this->animated->delta[1] = this->delta_start[1] + ((this->delta_end[1] - this->delta_start[1]) * time_part);
    memcpy(this->animated->delta_backup, this->animated->delta, sizeof(float) * 3);

    this->animated->img_zoom = this->img_zoom_start + ((this->img_zoom_end - this->img_zoom_start) * time_part);
}


void TransformAnimRC::start(){
    this->animated->geom->type = GT_NONE;
    this->geom_start = this->animated->geom->clone();
    this->geom_end = this->animated->cgeom.clone();

    this->img_zoom_start = this->animated->img_zoom;
    this->img_zoom_end = 2.5 * this->animated->painter->hw / this->geom_end->height;

    this->zoom_start = this->animated->zoom;
    this->zoom_end = 1.0;

    memcpy(this->delta_start, this->animated->delta, sizeof(float) * 3);
    this->delta_end[0] = 0.0;
    this->delta_end[1] = 0.0;
    this->delta_end[2] = -0.5;

    AnimatorBase::start();
}

void TransformAnimRC::stop(){
    this->animated->switch_geometry(GT_CYLINDER);
    AnimatorBase::stop();
}


TransformAnimCR::TransformAnimCR(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim):
    TransformAnimRC(animated, painter, anim_time, next_anim)
{
    this->name = "TransformAnimCR";
    this->zoom_start = 0.0;
    this->zoom_end = 0.0;
}


void TransformAnimCR::start(){
    this->animated->geom->type = GT_NONE;
    this->geom_start = this->animated->geom->clone();
    this->geom_end = this->animated->sgeom.clone();

    this->img_zoom_start = this->animated->img_zoom;
    if(this->animated->painter->hw < this->geom_end->height){
        this->img_zoom_end = 3.0 * this->animated->painter->hw / this->geom_end->height;
    }else{
        this->img_zoom_end = 3.0;
    }

    this->zoom_start = this->animated->zoom;
    this->zoom_end = 1.0;

    memcpy(this->delta_start, this->animated->delta, sizeof(float) * 3);
    this->delta_end[0] = 0.0;
    this->delta_end[1] = 0.0;
    this->delta_end[2] = -0.5;

    AnimatorBase::start();
}

void TransformAnimCR::stop(){
    this->animated->switch_geometry(GT_SQUEAR);
    AnimatorBase::stop();
}
