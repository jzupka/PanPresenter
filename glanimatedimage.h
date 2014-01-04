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

#ifndef GLANIMATEDIMAGE_H
#define GLANIMATEDIMAGE_H

#include "animatorbase.h"
#include "tools.h"
#include <QGLBuffer>


class GLImageGeom{
public:
    GLImageGeom();
    void update();
    GLImageGeom* clone();
    ~GLImageGeom();
    QVector< QGLBuffer* > vbo;
    QVector<float* > vbo_data;
    QGLBuffer* tex;
    float* tex_data;
    int count;
    float height;
    float width;
    GeometryType type;
};


class GLAnimatedImage
{
public:
    GLAnimatedImage(int loader_id=0);
    virtual ~GLAnimatedImage();
    void init();
    void add_animator(AnimatorBase *animator);
    void del_animator(AnimatorBase *animator);
    void remove_animators_for_del();
    void stop_animators();
    void stop_animators(QStringList name);
    void stop_animators(QString name);
    int animate(QTime cur_time);
    void reset();
    bool isLoaded(){return loaded;}
    void paintGLflustrum();
    void paintGLortho();

    GLWidget *painter;
    GLImageGeom cgeom;
    GLImageGeom sgeom;
    GLImageGeom *geom;
    GLuint *textures;
    int max_texture_size;
    int image_id;

    virtual void update_view() {}
    virtual void switch_geometry(char /*cylinder*/) {}
    virtual void create_geometry_squear() {}
    virtual void create_geometry_cylinder() {}
public:
    bool del_after_animation;
    bool loaded;

    float delta[3];
    float delta_backup[3];

    float rotate[3];
    float rotate_backup[3];

    float zoom;
    float img_zoom;
    float mouse_pos_constant;
    float alfa;
    float angle;
    bool no_slide;
    QVector< AnimatorBase* > animators;
    QVector< AnimatorBase* > animators_stack;
    QVector< AnimatorBase* > animators_for_delete;
};

#endif // GLANIMATEDIMAGE_H
