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

#ifndef ANIMATORS_H
#define ANIMATORS_H

#include "animatorbase.h"
#include "paintCore.h"
#include <QStringList>

const QStringList moveable_animators = (const QStringList)(QStringList() << "TransformAnimCR" << "TransformAnimRC" << "RotateEndAnim"
                  << "RotateAnim" << "RotateBeginAnim" << "SlideSlowAnim" << "SlideEndAnim"
                  << "SlideAnim" << "SlideBeginAnim");

class HideAmin: public AnimatorBase
{
public:
    HideAmin(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual void animate(QTime cur_time);
    virtual void start();
    float alfa_start;
    float alfa_end;
};


class ShowAmin: public HideAmin
{
public:
    ShowAmin(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
};


class FuncAnim: public AnimatorBase
{
public:
    FuncAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, void (*func)(const void*), void *args_struct, anim_vec next_anim=anim_vec());
    virtual void start();
protected:
    void (*func)(const void*);
    void *args_struct;
};


class SlideBeginAnim: public AnimatorBase
{
public:
    SlideBeginAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual void animate(QTime cur_time);
    virtual void start();
protected:
    float zoom_start;
    float zoom_end;
    float delta_start[3];
    float delta_end[3];
};


class SlideAnim: public SlideBeginAnim
{
public:
    SlideAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual void start();
};


class SlideEndAnim: public SlideBeginAnim
{
public:
    SlideEndAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual void start();
};


class SlideSlowAnim: public SlideBeginAnim
{
public:
    SlideSlowAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual void start();
};


class RotateBeginAnim: public AnimatorBase
{
public:
    RotateBeginAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual void animate(QTime cur_time);
    virtual void start();
protected:
    float zoom_start;
    float zoom_end;
    float rotate_start[3];
    float rotate_end[3];
};


class RotateAnim: public RotateBeginAnim
{
public:
    RotateAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual void start();
};


class RotateEndAnim: public RotateBeginAnim
{
public:
    RotateEndAnim(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual void start();
};

class TransformAnimRC: public AnimatorBase
{
public:
    TransformAnimRC(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual void animate(QTime cur_time);
    virtual void start();
    virtual void stop();
protected:
    float zoom_start;
    float zoom_end;
    float img_zoom_start;
    float img_zoom_end;
    float rotate_start[3];
    float rotate_end[3];
    float delta_start[3];
    float delta_end[3];
    GLImageGeom *geom_start;
    GLImageGeom *geom_end;
};


class TransformAnimCR: public TransformAnimRC
{
public:
    TransformAnimCR(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual void start();
    virtual void stop();
};


#endif // ANIMATORS_H
