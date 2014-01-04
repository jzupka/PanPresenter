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

#include "paintCore.h"

AnimatorBase::AnimatorBase(GLAnimatedImage *animated, GLWidget *painter, float anim_time, anim_vec next_anim)
{
    this->animated = animated;
    this->painter = painter;
    this->anim_time = anim_time * 1000;
    this->next_anim = next_anim;
    this->end_time = QTime();
    this->start_time = QTime();
    this->animated->animators_stack.append(this);
}

AnimatorBase::~AnimatorBase(){
    this->stop();
}

AnimatorBase* AnimatorBase::call_animate(QTime cur_time){
    if(cur_time >= this->end_time){
        //qDebug() << "Stop Animator";
        this->animate(cur_time);
        for (int i = 0;i < this->next_anim.size(); ++i){
            this->next_anim[i]->start();
        }
        return this;
    }
    //QString mess;
    //mess.sprintf("Anim %s curt, %s , %s", cur_time.toString().toAscii().data(), this->start_time.toString().toAscii().data(), this->end_time.toString().toAscii().data());
    //qDebug() << mess;
    this->animate(cur_time);
    return 0;
}

void AnimatorBase::animate(QTime /*cur_time*/){
}

void AnimatorBase::start(){
    this->start_time = QTime::currentTime();
    this->end_time = this->start_time.addMSecs(ceil(this->anim_time));
    //qDebug() << "Start"<< this->name << this->start_time << this->end_time;
    this->animated->add_animator(this);
    this->painter->add_animator(this);
}

void AnimatorBase::stop(){
    //qDebug() << "Stop"<< this->name;
    this->animated->del_animator(this);
}

