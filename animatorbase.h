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

#ifndef ANIMATORBASE_H
#define ANIMATORBASE_H

#include <QTime>
#include <QVector>

#include "paintCore.h"

typedef QVector<AnimatorBase* > anim_vec;

class AnimatorBase
{
public:
    AnimatorBase(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim=anim_vec());
    virtual AnimatorBase* call_animate(QTime /*cur_time*/);
    virtual void animate(QTime cur_time);
    virtual void start();
    virtual void stop();
    virtual ~AnimatorBase();

    QString name;
protected:
    GLAnimatedImage *animated;
    GLWidget *painter;
    float anim_time;
    anim_vec next_anim;
    QTime end_time;
    QTime start_time;
};

#endif // ANIMATORBASE_H
