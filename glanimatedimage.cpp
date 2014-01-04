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
#include "tools.h"

GLImageGeom::GLImageGeom()
{
    this->tex = 0;
    this->tex_data = 0;
    this->height = 0;
    this->width = 0;
    this->count = 0;
    this->type = GT_NONE;
}

void GLImageGeom::update()
{
    assert (this->count > 0);
    if(this->count > 0){
        if (this->vbo.size() > 0){
            for (int i=0; i < this->vbo.size(); ++i){
                delete this->vbo[i];
            }
            this->vbo.clear();
            delete this->tex;
        }
        //qDebug() << "Geometry size "<< this->count;
        foreach (const float* data, this->vbo_data){
            QGLBuffer *vbo = new QGLBuffer(QGLBuffer::VertexBuffer);
            vbo->create();
            vbo->bind();
            vbo->allocate(data, sizeof(float) * this->count * 3);
            this->vbo.append(vbo);
        }
        QGLBuffer *vbo = new QGLBuffer(QGLBuffer::VertexBuffer);
        vbo->create();
        vbo->bind();
        vbo->allocate(this->tex_data, sizeof(float)  * this->count * 2);
        this->tex = vbo;
    }
}

GLImageGeom *GLImageGeom::clone()
{
    //qDebug() << "Clone" <<  this->vbo_data.size();
    GLImageGeom* geom = new GLImageGeom();
    geom->height = this->height;
    geom->width = this->width;
    geom->type = this->type;
    geom->count = this->count;
    for (int i = 0; i < this->vbo_data.size(); ++i){
        int size = sizeof(float) * this->count * 3;
        float *data = static_cast<float*> (::operator new (size));
        memcpy(data, this->vbo_data[i], size);

        geom->vbo_data.append(data);
    }
    int size = sizeof(float)  * this->count * 2;
    float *data = static_cast<float*> (::operator new (size));
    memcpy(data, this->tex_data, size);
    geom->tex_data = data;
    geom->update();
    return geom;
}

GLImageGeom::~GLImageGeom()
{
    for(int i = 0; i < this->vbo_data.size(); ++i){
        delete this->vbo_data[i];
    }
    for(int i = 0; i < this->vbo.size(); ++i){
        delete this->vbo[i];
    }
    if(this->tex_data != 0){
        delete this->tex_data;
        delete this->tex;
    }
}


GLAnimatedImage::GLAnimatedImage(int loader_id)
{
    std::fill_n(delta, 3, 0.0);
    std::fill_n(delta_backup, 3, 0.0);
    std::fill_n(rotate, 3, 0.0);
    std::fill_n(rotate_backup, 3, 0.0);
    this->alfa = 0.0;
    this->del_after_animation = false;
    this->loaded = false;
    this->image_id = loader_id;
}


GLAnimatedImage::~GLAnimatedImage(){
    this->stop_animators();
    this->remove_animators_for_del();
}


void GLAnimatedImage::init(){
    this->reset();
}


void GLAnimatedImage::paintGLflustrum(){
    this->update_view();

    glScalef(this->img_zoom, this->img_zoom, 1.0);
    glScalef(this->zoom, this->zoom, 1.0);

    glTranslatef(this->delta[0],
                 this->delta[1],
                 this->delta[2]);

    glRotatef(this->rotate[1], 1, 0, 0);
    glRotatef(this->rotate[0], 0, 1, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    for(int j=0; j < this->geom->vbo.size(); ++j){
        glColor4f(1.0, 1.0, 1.0, this->alfa);
        glBindTexture(GL_TEXTURE_2D, this->textures[j]);
        /*QString message;
        message.sprintf("Texture set %d %g", image->textures[j], image->alfa);
        qDebug() << message;*/
        this->geom->vbo[j]->bind();
        glVertexPointer(3, GL_FLOAT, 0, 0);

        this->geom->tex->bind();
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, this->geom->count);
    }
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void GLAnimatedImage::paintGLortho(){
    this->update_view();

    glScalef(1.0/3, 1.0/3, 1.0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    for(int j=0; j < this->sgeom.vbo.size(); ++j){
        glBindTexture(GL_TEXTURE_2D, this->textures[j]);
        /*QString message;
        message.sprintf("Texture set %d %g", image->textures[j], image->alfa);
        qDebug() << message;*/
        this->sgeom.vbo[j]->bind();
        glVertexPointer(3, GL_FLOAT, 0, 0);

        this->sgeom.tex->bind();
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, this->sgeom.count);
    }
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void GLAnimatedImage::stop_animators()
{
    //qDebug() << "Kill him";
    for (int i = 0;i < this->animators_stack.size(); ++i){
        this->animators_for_delete.append(this->animators_stack[i]);
    }
}


void GLAnimatedImage::stop_animators(QStringList names)
{
    //qDebug() << "Kill him";
    for (int i = 0;i < this->animators_stack.size(); ++i){
        if (names.contains(this->animators_stack[i]->name)){
            this->animators_for_delete.append(this->animators_stack[i]);
        }
    }
}


void GLAnimatedImage::stop_animators(QString name)
{
    //qDebug() << "Kill him";
    for (int i = 0;i < this->animators_stack.size(); ++i){
        if (this->animators_stack[i]->name == name){
            this->animators_for_delete.append(this->animators_stack[i]);
        }
    }
}


int GLAnimatedImage::animate(QTime cur_time){
    int animated = 0;
    QVector <AnimatorBase *> ended_anim;
    for (int j = 0; j < this->animators.size(); ++j){
        ended_anim << this->animators[j]->call_animate(cur_time);
        animated++;
    }
    foreach (AnimatorBase * ended, ended_anim){
        if (this->animators_stack.indexOf(ended) >= 0){
            ended->stop();
            this->animators_for_delete.append(ended);
        }
    }
    return animated;
}


void GLAnimatedImage::add_animator(AnimatorBase *animator){
    //Animation event;
    this->animators.append(animator);
}


void GLAnimatedImage::del_animator(AnimatorBase *animator){
    //Animation event;
    if (this->animators.indexOf(animator) >= 0){
        this->animators.remove(this->animators.indexOf(animator));
    }
}


void GLAnimatedImage::remove_animators_for_del(){
    foreach (AnimatorBase * anim, this->animators_for_delete){
        if (this->animators_stack.indexOf(anim) >= 0){
            delete this->animators_stack[this->animators_stack.indexOf(anim)];
            this->animators_stack.remove(this->animators_stack.indexOf(anim));
        }
    }
    this->animators_for_delete.clear();
}


void GLAnimatedImage::reset()
{
    zoom = 1.0;
    delta[0] = 0.0;
    delta[1] = 0.0;
    delta[2] = -0.5;
    delta_backup[0] = 0.0;
    delta_backup[1] = 0.0;
    delta_backup[2] = -0.5;

    rotate[0] = 0.0;
    rotate[1] = 0.0;
    rotate[2] = 0.0;
    rotate_backup[0] = 0.0;
    rotate_backup[1] = 0.0;
    rotate_backup[2] = 0.0;
}
