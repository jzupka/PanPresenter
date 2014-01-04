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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui>
#include <QGLWidget>
#include <QDesktopWidget>
#include <QAtomicInt>
#include <QMatrix4x4>
#include <QVector4D>
#include "glimage.h"


class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(const QGLFormat fmt, QWidget *parent = 0, QGLWidget *shareWidget = 0);
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void rotateBy(int xAngle, int yAngle, int zAngle);
    void setClearColor(const QColor &color);
    void set(CacheImage* image);
    void setVideoImage(QString path);
    void setCachedImages(QVector<CacheImage* > *image, int pos);
    void initGL();
    void add_animator(AnimatorBase *animator);
    void del_animator(AnimatorBase *animator);
    void anim(float anim_time);
    void anim_transf(float anim_time);
    void anim_transf_no_end(float anim_time);
    void anim_back_from(float anim_time);
    void transfRC(float anim_time);
    void transfCR(float anim_time);
    void anim_show(float anim_time);
    void anim_hide(float anim_time);
    void rectangle();
    void cylindrical();
    void show_times();
    void newVideoFrame(GLImageVideo *gl_imagevideo, QVideoFrame &frame);
    void setAnimations(GLAnimatedImage* image);

    void setNewImageLoaded(int* _new_image_loaded, QMutex* _clean_image_mux);


    float d_width;
    float d_height;
    int max_texture_size;
    float hw;
    float wh;

public slots:
    void animate();
    void mouse_inactivity_Event();

signals:
    void set_new_pos(int pos);
    void scroll_new_pos(int pos);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void updateView();
    void updateView(int width, int height);
    void cleanup_images();

private:
    void makeObject();
    void check_other_threads();
    void check_press_activity(QPoint pos);
    void check_click_orto_position(QPoint win_pos);
    void check_move_orto_position(QPoint win_pos);
    QVector4D window_pos_to_ortomodel(QPoint win_pos);
    QMatrix4x4 window_to_viewport_matrix();
    void click_to_thumbnail(QVector4D pos);

    GLuint video_frame;
    int *new_image_loaded;
    QMutex *clean_image_mux;
    int new_image_loaded_old;
    int pos;
    int scroll_pos;
    bool all_loaded;
    unsigned int thumb_window_size;
    float thumbnail_alfa;
    QVector <CacheImage* > *cached_images;

    QColor clearColor;
    QPoint lastPos;
    int animators;
    QVector<GLAnimatedImage* > active_imgs;
    QVector<GLAnimatedImage* > loaded_imgs;
    QMap<int, QVector<GLuint> > images;
    QTimer *anim_timer;
    char show_block;
    char show_wired;
    char geometry_type;
    char mouse_pressed;
    char animating;
    int x;
    int y;
    int max_step;
    int min_step;
    float basic_step;
    bool show_t;
    QTime last_time;
    float last_diff;
    float anim_time;
    float aver_time;
    QDesktopWidget desktop;
    QMutex animate_event;
    QTimer mouse_inactivity;
    QTime old_scroll_time;


#ifdef QT_OPENGL_ES_2
    QGLShaderProgram *program;
#endif
};


#endif
