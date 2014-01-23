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

#include <QtOpenGL>
#include <QDebug>
#include <QAbstractVideoBuffer>

#include "paintCore.h"
#include "tools.h"



GLWidget::GLWidget(const QGLFormat fmt, QWidget *parent, QGLWidget *shareWidget)
    : QGLWidget(fmt, parent, shareWidget)
{
    clearColor = Qt::black;
#ifdef QT_OPENGL_ES_2
    program = 0;
#endif
    anim_timer = new QTimer(this);
    connect(anim_timer, SIGNAL(timeout()), this, SLOT(animate()));
    connect(&this->mouse_inactivity, SIGNAL(timeout()), this, SLOT(mouse_inactivity_Event()));
    this->mouse_inactivity.setSingleShot(true);
    this->anim_timer->setSingleShot(true);

    show_block = 0;
    show_wired = 0;
    geometry_type = GT_SQUEAR;
    mouse_pressed = 0;
    thumbnail_alfa = 0.0;
    x = 0;
    y = 0;
    max_step = 0;
    min_step = 0;
    hw = 1.0;
    wh = 1.0;
    basic_step = 0.33;
    last_diff = 0.0;
    anim_time = 0.0;
    aver_time = 0.0;
    animating = 0;
    show_t = false;
    this->new_image_loaded = 0;
    this->new_image_loaded_old = 0;
    this->clean_image_mux = 0;
    this->pos = 0;
    this->scroll_pos = 0;
    this->thumb_window_size = 7;
    this->big_image_bias_static = -0.5;
    this->big_image_bias_moving = -0.15;
    this->big_image_bias = this->big_image_bias_static;
    this->thumbnail_bias = -0.5;
    this->old_scroll_time = QTime::currentTime();
    this->setMouseTracking(true);
    QThread::currentThread()->setPriority(QThread::HighestPriority);
}

GLWidget::~GLWidget()
{
    for (int i = 0;i < this->active_imgs.size(); ++i){
        delete this->active_imgs[i];
    }
}

void GLWidget::setNewImageLoaded(int* _new_image_loaded, QMutex* _clean_image_mux){
    this->new_image_loaded = _new_image_loaded;
    this->clean_image_mux = _clean_image_mux;
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(700, 400);
}

void GLWidget::initGL(){
    this->initializeGL();
}

void GLWidget::initializeGL()
{
    //makeObject();
    this->makeCurrent();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glCullFace(GL_FRONT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, -0.5);
    GLint t_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &t_size);
    max_texture_size = (float)(t_size / 4);
    glEnable(GL_TEXTURE_2D);
    aver_time = 16;
}

void GLWidget::paintGL()
{
    this->blockSignals(true);
    this->parent()->blockSignals(true);
    QTime t = QTime::currentTime();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-this->wh * 0.1, this->wh * 0.1, -this->hw * 0.1, this->hw * 0.1, 0.05, 4.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, this->big_image_bias);
    for(int i = 0; i < active_imgs.size(); ++i){
        GLAnimatedImage *image = active_imgs[i];
        if (image->isLoaded()){
            glLoadIdentity();
            image->paintGLflustrum();
        }else{
            //qDebug() << "Not loaded";
        }
    }
    if (this->thumbnail_alfa > 0){
        glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, -0.5);
        //glDisable(GL_CULL_FACE);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-this->wh, this->wh , -this->hw, this->hw, 5.0, -5.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4f(1.0, 1.0, 1.0, this->thumbnail_alfa);

        for(unsigned int i = 0; i < this->thumb_window_size; ++i){
            GLAnimatedImage *image = loaded_imgs[i];
            if (image != 0 && image->isLoaded()){
                glLoadIdentity();

                glTranslatef(-this->wh + (i)*this->wh/3.5 + this->wh/this->thumb_window_size,
                             -this->hw + this->hw/this->thumb_window_size,
                             0);
                glScalef(image->img_zoom, image->img_zoom, 1.0);
                glScalef(1.0/this->thumb_window_size, 1.0/this->thumb_window_size, 1.0);
                image->paintGLortho();
            }else{
                //qDebug() << "Not loaded";
            }
        }
    }
    this->parent()->blockSignals(false);
    this->blockSignals(false);
    QTime drawing_time = QTime::currentTime();
    glFinish();
    //glFlush();
    QTime time = QTime::currentTime();
    if (this->show_t){
        if (this->last_time.msecsTo(time) > 17){
            qDebug() << "Paiting" << this->last_time.msecsTo(time) << t.msecsTo(QTime::currentTime()) << "Drawt" << t.msecsTo(drawing_time);
        }
    }
    check_other_threads();
    this->last_time = time;
    if(this->animating){
        this->anim_timer->stop();
        this->anim_timer->start(this->aver_time / 4);
    }
}

QMatrix4x4 GLWidget::window_to_viewport_matrix(){
    return QMatrix4x4(2 * this->wh/this->width(), 0, 0, -this->wh,
                      0, 2 * this->hw/this->height(),0 , -this->hw,
                      0, 0, 1, 0,
                      0, 0, 0, 1);
}

QVector4D GLWidget::window_pos_to_ortomodel(QPoint win_pos){
    return window_to_viewport_matrix() * QVector4D(win_pos.rx(), win_pos.ry(), 0, 1);
}

void GLWidget::check_click_orto_position(QPoint window_position){
    QVector4D pos = window_pos_to_ortomodel(window_position);
    click_to_thumbnail(pos);
}

void GLWidget::check_move_orto_position(QPoint window_position){
    QVector4D pos = window_pos_to_ortomodel(window_position);
    if (pos.y() > this->hw - this->hw / 10){
        if (this->thumbnail_alfa != 1.0){
            this->thumbnail_alfa = 1.0;
            if (!this->animating){
                this->add_animator(0);
            }
        }
    }
    if (this->thumbnail_alfa > 0 && pos.y() < this->hw - this->hw / 3.5){
        this->thumbnail_alfa = 0.0;
        if (!this->animating){
            this->add_animator(0);
        }
    }
}

void GLWidget::click_to_thumbnail(QVector4D pos){
    if (thumbnail_alfa > 0 && pos.y() > this->hw - this->hw / 3.5){
        float part_size = this->wh / 3.5;
        int thumbimg_id = floor(((pos.x() + this->wh) / part_size));
        //qDebug() << "thumbnail Click:"  << thumbimg_id;
        GLAnimatedImage* img = this->loaded_imgs[thumbimg_id];
        if (img != 0){
            this->set_new_pos(img->image_id);
        }
    }
}

void GLWidget::show_times(){
    this->show_t = !this->show_t;
}

void GLWidget::cleanup_images(){
    if(active_imgs.size() > 2){
        for (int i = 2;i < active_imgs.size(); ++i){
            delete active_imgs[2];
            active_imgs.remove(2);
        }
    }
}

void GLWidget::check_other_threads(){
    if (this->new_image_loaded == 0)
        return;

    if (*this->new_image_loaded != this->new_image_loaded_old){
        this->new_image_loaded_old = *this->new_image_loaded;
        this->setCachedImages(this->cached_images, this->pos);
        if (!this->animating){
            this->add_animator(0);
        }
    }
}

void GLWidget::animate(){
    //qDebug() << "paint anim time:" << this->last_time.msecsTo(QTime::currentTime());
    QTime cur_time = QTime::currentTime();
    int animated = 0;
    int img_animated = 0;
    for (int j = 0; j < this->active_imgs.size(); ++j){
        img_animated = this->active_imgs[j]->animate(cur_time);
        if (this->active_imgs[j]->del_after_animation && img_animated == 0){
            delete this->active_imgs[j];
            this->active_imgs.remove(j);
        }else{
            animated += this->active_imgs[j]->animate(cur_time);
        }
    }
    if (animated == 0 && this->pos == this->scroll_pos && this->all_loaded){
        this->animating = 0;
        anim_timer->stop();
        //Make texture more detailed when there is no animation.
        this->big_image_bias = this->big_image_bias_static;
    }else{
        this->big_image_bias = this->big_image_bias_moving;
    }
    foreach (GLAnimatedImage * img, this->active_imgs){
        img->remove_animators_for_del();
    }
    updateGL();
}


void GLWidget::add_animator(AnimatorBase */*animator*/){
    //Animation event;;
    if (!this->animating){
        this->animating = 1;
        this->anim_timer->stop();
        this->anim_timer->start(this->aver_time / 4);
    }
}


void GLWidget::rectangle(){
    this->active_imgs.first()->switch_geometry(GT_SQUEAR);
    for (int i = 0;i < active_imgs.size(); ++i){
        active_imgs[i]->stop_animators();
        active_imgs[i]->reset();
    }
    this->geometry_type = GT_SQUEAR;
    this->updateGL();
}

void GLWidget::cylindrical(){
    this->active_imgs.first()->switch_geometry(GT_CYLINDER);
    for (int i = 0;i < active_imgs.size(); ++i){
        active_imgs[i]->stop_animators();
        active_imgs[i]->reset();
    }
    this->geometry_type = GT_CYLINDER;
    this->updateGL();
}

void GLWidget::anim(float anim_time){

    active_imgs[0]->stop_animators(moveable_animators);
    if(this->geometry_type == GT_SQUEAR){
        anim_time *= 1.0 / (active_imgs[0]->sgeom.height * 3);
        AnimatorBase* anim = new SlideBeginAnim(active_imgs[0], this, anim_time / 20,
                       anim_vec() << new SlideAnim(active_imgs[0], this, anim_time / 20 * 18,
                          anim_vec() << new SlideEndAnim(active_imgs[0], this, anim_time / 20)));
        anim->start();
    }else if (this->geometry_type == GT_CYLINDER){
        anim_time *= 1.0 / (active_imgs[0]->sgeom.height * 3);
        AnimatorBase* anim = new RotateBeginAnim(active_imgs[0], this, anim_time / 20,
                       anim_vec() << new RotateAnim(active_imgs[0], this, anim_time / 20 * 18,
                          anim_vec() << new RotateEndAnim(active_imgs[0], this, anim_time / 20)));
        anim->start();
    }
}


void GLWidget::anim_transf(float anim_time){
    active_imgs[0]->stop_animators(moveable_animators);
    anim_time *= 1.0 / (active_imgs[0]->sgeom.height * 3);
    AnimatorBase* animt = new TransformAnimRC(active_imgs[0], this, anim_time / 20);
    AnimatorBase* anim = new RotateBeginAnim(active_imgs[0], this, anim_time / 20,
                   anim_vec() << new RotateAnim(active_imgs[0], this, anim_time / 20 * 18,
                      anim_vec() << new RotateEndAnim(active_imgs[0], this, anim_time / 20)
                                 << new TransformAnimCR(active_imgs[0], this, anim_time / 20)));
    animt->start();
    anim->start();
}

void GLWidget::anim_transf_no_end(float anim_time)
{
    active_imgs[0]->stop_animators();
    anim_time *= 1.0 / (active_imgs[0]->sgeom.height * 3);
    AnimatorBase* animt = new TransformAnimRC(active_imgs[0], this, anim_time / 20);
    AnimatorBase* anim = new RotateBeginAnim(active_imgs[0], this, anim_time / 20,
                   anim_vec() << new RotateAnim(active_imgs[0], this, anim_time / 20 * 18));
    animt->start();
    anim->start();
}

void GLWidget::anim_back_from(float anim_time)
{
    active_imgs[0]->stop_animators();
    AnimatorBase* anim = new TransformAnimCR(active_imgs[0], this, anim_time / 20);
    anim->start();
    anim = new RotateEndAnim(active_imgs[0], this, anim_time / 20);
    anim->start();
    anim = new SlideEndAnim(active_imgs[0], this, anim_time / 20);
    anim->start();
}


void GLWidget::transfRC(float anim_time){
    for (int i = 0;i < active_imgs.size(); ++i){
        active_imgs[i]->stop_animators();
        AnimatorBase* animt = new TransformAnimRC(active_imgs[i], this, anim_time);
        animt->start();
        this->geometry_type = GT_CYLINDER;
    }
}


void GLWidget::transfCR(float anim_time){
    for (int i = 0;i < active_imgs.size(); ++i){
        active_imgs[i]->stop_animators();
        AnimatorBase* animt = new TransformAnimCR(active_imgs[i], this, anim_time);
        animt->start();
        this->geometry_type = GT_SQUEAR;
    }
}


void GLWidget::anim_show(float anim_time){
    for (int i = 0;i < active_imgs.size(); ++i){
        active_imgs[i]->stop_animators();
        AnimatorBase* animt = new ShowAmin(active_imgs[i], this, anim_time);
        animt->start();
    }
}


void GLWidget::anim_hide(float anim_time){
    for (int i = 0;i < active_imgs.size(); ++i){
        active_imgs[i]->stop_animators();
        AnimatorBase* animt = new HideAmin(active_imgs[i], this, anim_time);
        animt->start();
    }
}


void GLWidget::newVideoFrame(GLImageVideo *gl_imagevideo, QVideoFrame &r_frame){
    if (gl_imagevideo->textures[0]){
        glDeleteTextures(1, &gl_imagevideo->textures[0]);
        gl_imagevideo->textures[0] = (GLuint)0;
    }
    //QVideoFrame *r_frame = (QVideoFrame)frame;
    if (r_frame.map(QAbstractVideoBuffer::ReadOnly)){
        glGenTextures(1, &gl_imagevideo->textures[0]);
        glBindTexture(GL_TEXTURE_2D, gl_imagevideo->textures[0]);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        /*QString message;
        message.sprintf("Texture set %d size %d,%d", gl_imagevideo->textures[0],
                                                     gl_imagevideo->size.width(),
                                                     gl_imagevideo->size.height());
        qDebug() << message;*/
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     gl_imagevideo->size.width(),
                     gl_imagevideo->size.height(),
                     0, GL_BGRA, GL_UNSIGNED_BYTE,
                     r_frame.bits());
        r_frame.unmap();
        //glFlush();
    }
}

void GLWidget::setVideoImage(QString path){
    this->setAnimations(new GLImageVideo(this, path, this->max_texture_size));
}

void GLWidget::set(CacheImage* image){
    //QString message;
    int image_count = image->images.size();
    GLuint *textures = new GLuint[image_count];
    glGenTextures(image_count, &textures[0]);
    //QTime t_load = QTime::currentTime();
    for (int i = 0;i < image_count; ++i){
        //QTime start = QTime::currentTime();
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, image->images[i].size()-1);
        for (int j = 0; j < image->images[i].size(); j++){
            QVector<QImage> images = image->images[i];

            glTexImage2D(GL_TEXTURE_2D, j, GL_RGBA, images[j].width(), images[j].height(), 0,
                         GL_BGRA, GL_UNSIGNED_BYTE, images[j].bits());


            GLint error = glGetError();
            //qDebug() << error;
            assert(error == 0);
            //QTime end = QTime::currentTime();
            //message.sprintf("Texture set %d size %d,%d time %d", textures[0], images[j].width(),
            //                                                     images[j].height(), start.msecsTo(end));
            //qDebug() << message;
        }
    }
    //qDebug() << "End load texture." << t_load.msecsTo(QTime::currentTime());
    //glFlush();
    this->setAnimations(new GLImage(image,
                                    this,
                                    textures,
                                    this->max_texture_size));
}

void GLWidget::setAnimations(GLAnimatedImage* image){
    active_imgs.insert(0, image);
    if(active_imgs.size() == 1){
        GLAnimatedImage *active = this->active_imgs[0];
        active->alfa = 0.3;
        AnimatorBase *a = new ShowAmin(active, this, 1.0);
        a->start();
        if (!active->no_slide){
            a = new SlideSlowAnim(active, this, 20.0);
            a->start();
        }

    } else if(active_imgs.size() >= 2){
        GLAnimatedImage *active = this->active_imgs[0];
        active->alfa = 0.3;
        AnimatorBase *a = new ShowAmin(active, this, 1.0);
        a->start();
        if (!active->no_slide){
            a = new SlideSlowAnim(active, this, 20.0);
            a->start();
        }
        active = this->active_imgs[1];
        active->stop_animators();
        active->del_after_animation = true;
        a = new HideAmin(active, this, 1.0);
        a->start();
    }
    this->cleanup_images();
}


void GLWidget::setCachedImages(QVector<CacheImage* > *images, int pos){
    //QString message;
    this->cached_images = images;
    this->pos = (pos + images->count()) % images->count();
    this->scroll_pos = this->pos;
    if (images->count() == 0){
        return;
    }

    foreach (GLAnimatedImage * image, this->loaded_imgs){
        delete image;
    }
    this->loaded_imgs.clear();

    int qq = 0;
    int not_loaded = 0;
    for (int q = pos-3; q <= pos+3; q++){
        qq = (q + images->count()) % images->count();
        while (qq < 0){
            qq = (qq + images->count()) % images->count();
        }
        this->clean_image_mux->lock();
        CacheImage * image = (*images)[qq];
        if (!image->is_loaded()){
            this->loaded_imgs.append(0);
            not_loaded++;
            this->clean_image_mux->unlock();
            continue;
        }
        int image_count = image->images.count();
        GLuint *textures = new GLuint[image_count];
        glGenTextures(image_count, &textures[0]);
        for (int i = 0;i < image_count; ++i){
            //QTime start = QTime::currentTime();
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            int j = image->images[i].size()-1;

            QVector<QImage> images = image->images[i];

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, images[j].width(), images[j].height(), 0,
                         GL_BGRA, GL_UNSIGNED_BYTE, images[j].bits());

            GLint error = glGetError();
            //qDebug() << error;
            assert(error == 0);
            //QTime end = QTime::currentTime();
            //message.sprintf("Texture set %d size %d,%d time %d", textures[0], images[j].width(),
            //                                                     images[j].height(), start.msecsTo(end));
            //qDebug() << message;
        }
        //qDebug() << "End load texture." << t_load.msecsTo(QTime::currentTime());
        this->loaded_imgs.append(new GLImage(image,
                                 this,
                                 textures,
                                 this->max_texture_size, qq));
        this->clean_image_mux->unlock();
    }
    if (not_loaded){
        this->all_loaded = false;
    }else{
        this->all_loaded = true;
    }
    //glFlush();
}


void GLWidget::updateView(int width, int height){
    glViewport(0, 0, width, height);
    this->wh = 1.0;
    this->hw = ((float)height) / width;
}


void GLWidget::updateView(){
    this->updateView(this->width(), this->height());
}

void GLWidget::check_press_activity(QPoint pos){
    check_click_orto_position(pos);
}

void GLWidget::resizeGL(int width, int height)
{
    QSize size = this->desktop.screenGeometry(this->desktop.screenNumber(this)).size();
    this->d_width = size.width();
    this->d_height = size.height();
    this->updateView(width, height);
    QString message;
    message.sprintf("d_width = %f, d_heigth = %f", d_width, d_height);
    //qDebug() << message;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "Mouse Press";
    for(int i = 0; i < this->active_imgs.size(); i++){
        memcpy(this->active_imgs[i]->delta, this->active_imgs[i]->delta_backup, sizeof(float) * 3);
        memcpy(this->active_imgs[i]->rotate, this->active_imgs[i]->rotate_backup, sizeof(float) * 3);
    }
    lastPos = event->pos();
    this->check_press_activity(this->lastPos);
    this->mouse_pressed = 1;
}

void GLWidget::mouseReleaseEvent(QMouseEvent * event)
{
    for(int i = 0; i < this->active_imgs.size(); i++){
        memcpy(this->active_imgs[i]->delta_backup, this->active_imgs[i]->delta, sizeof(float) * 3);
        memcpy(this->active_imgs[i]->rotate_backup, this->active_imgs[i]->rotate, sizeof(float) * 3);
    }
    lastPos = event->pos();
    this->mouse_pressed = 0;
}


void GLWidget::mouse_inactivity_Event(){

    QApplication::setOverrideCursor(Qt::BlankCursor);
    this->mouse_inactivity.stop();
    //qDebug() << "Hide mouse";
}


void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    this->mouse_inactivity.stop();
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    this->mouse_inactivity.start(5000);
    if (this->mouse_pressed){
        int dx = event->x() - lastPos.x();
        int dy = event->y() - lastPos.y();
        for(int i = 0; i < this->active_imgs.size(); i++){
            GLAnimatedImage* image = this->active_imgs[i];
            image->stop_animators();
            if(image->geom->type == GT_SQUEAR){
                image->delta[0] = image->delta_backup[0] + ((2 * image->mouse_pos_constant * dx) / (image->zoom * this->width()));
                image->delta[1] = image->delta_backup[1] - ((2 * image->mouse_pos_constant * this->hw * dy) / (image->zoom * this->height()));
            }else if (image->geom->type == GT_CYLINDER){
                image->rotate[0] = image->rotate_backup[0] - dx / (10.0 * image->zoom);
                image->rotate[1] = image->rotate_backup[1] - dy / (10.0 * image->zoom);
                NormalizeAngle(image->rotate);
            }
        }
        if (!this->animating){
            this->add_animator(0);
        }
    }else{
        lastPos = event->pos();
        check_move_orto_position(lastPos);
    }
}


void GLWidget::wheelEvent(QWheelEvent *event){
    if (thumbnail_alfa == 0.0){
        for(int i = 0; i < this->active_imgs.size(); i++){
            GLAnimatedImage* image = this->active_imgs[i];
            image->stop_animators();

            float multipler = 0.85;
            if (event->delta() > 0){
                multipler = 1.25;
            }
            float zoom = image->zoom * multipler;

            if (zoom > 0.5 && zoom < 40){
                float dxc = 0;
                float dyc = 0;
                float rx = 0.0;
                float ry = 0.0;
                if(image->geom->type == GT_SQUEAR){
                    this->lastPos = event->pos();
                    float zoom_t_x = (2 * image->mouse_pos_constant / (image->zoom * this->width()));
                    float zoom_t_y = (2 * image->mouse_pos_constant * this->hw / (image->zoom * this->height()));
                    if (multipler > 1.0){
                        dxc = ((this->lastPos.x() - this->width() / 2) * (1.0 - 1.0 / multipler));
                        dyc = ((this->lastPos.y() - this->height() / 2) * (1.0 - 1.0 / multipler));
                    }else{
                        dxc = ((image->delta[0] / zoom_t_x)) / (image->zoom * image->zoom);
                        dyc = -((image->delta[1] / zoom_t_y)) / (image->zoom * image->zoom);
                    }
                    image->delta[0] += zoom_t_x * (-dxc);
                    image->delta[1] -= zoom_t_y * (-dyc);
                    //qDebug() << image->delta[0] << image->delta[1] << zoom_t_x;
                    memcpy(this->active_imgs[i]->delta_backup, this->active_imgs[i]->delta, sizeof(float) * 3);
                }
                else if (image->geom->type == GT_CYLINDER){
                    this->lastPos = event->pos();
                    if (multipler > 1.0){
                        dxc = 2.5 * image->mouse_pos_constant * ((float)(this->lastPos.x() - this->width() / 2)) / (this->width() * image->zoom);
                        dyc = this->hw * 2.5 * image->mouse_pos_constant * ((float)(this->lastPos.y() - this->height() / 2)) / (this->height() * image->zoom);
                        rx = atan(dxc) * 180.0 / PI_F  * (1.0 - 1.0 / multipler);
                        ry = atan(dyc) * 180.0 / PI_F * (1.0 - 1.0 / multipler);
                    }else{
                        rx = 0.0;
                        ry = -(image->rotate[1] / (image->zoom * image->zoom));
                    }
                    //qDebug() << rx << ry;
                    image->rotate[0] += (rx);
                    image->rotate[1] += (ry);
                    memcpy(this->active_imgs[i]->rotate_backup, this->active_imgs[i]->rotate, sizeof(float) * 3);
                    NormalizeAngle(image->rotate);
                }
            }

            image->zoom = zoom;
            if (image->zoom < 0.5){
                image->zoom = 0.5;
            }else if (image->zoom > 40){
                image->zoom /= 1.25;
            }else{
                //image->delta[0] *= multipler;
                //image->delta[1] *= multipler;
                //memcpy(this->active_imgs[i]->delta_backup, this->active_imgs[i]->delta, sizeof(float) * 3);
            }
            //qDebug() << "Image zoom: " << image->zoom;
        }
        this->updateGL();
    }else{
        QTime time = QTime::currentTime();
        if (this->old_scroll_time.msecsTo(time) > 20){
            this->old_scroll_time = time;
            if (event->delta() > 0){
                this->scroll_pos--;
            }else{
                this->scroll_pos++;
            }
            int qq = (scroll_pos + cached_images->count()) % cached_images->count();
            while (qq < 0){
                qq = (qq + cached_images->count()) % cached_images->count();
            }
            this->scroll_new_pos(qq);
            this->setCachedImages(this->cached_images, qq);
            if (!this->animating){
                this->add_animator(0);
            }
        }
    }
}
