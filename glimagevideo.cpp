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

GLWidgetVideo::GLWidgetVideo(GLImageVideo *gl_imagevideo, GLWidget *parent):
    QAbstractVideoSurface(parent->parent())
{
    this->gl_widget = parent;
    this->gl_imagevideo = gl_imagevideo;
}

QSize GLWidgetVideo::nativeResolution() const{
    return QSize(720, 400);
}

bool GLWidgetVideo::isFormatSupported(const QVideoSurfaceFormat &format) const
{
    (void)format;
    //QSize s = format.frameSize();
    //qreal r = format.frameRate();
    return true;
}

QVideoSurfaceFormat GLWidgetVideo::nearestFormat(const QVideoSurfaceFormat &format) const
{
    return format;
}

bool GLWidgetVideo::present(const QVideoFrame &frame)
{
    this->gl_imagevideo->newVideoFrame(frame);
    return 0;
}

bool GLWidgetVideo::start(const QVideoSurfaceFormat &format)
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        this->gl_imagevideo->setVideoSize(format.frameSize());
        return true;
    }else{
        return false;
    }
}

void GLWidgetVideo::stop()
{
    QAbstractVideoSurface::stop();
    //this->gl_imagevideo->VideoStop();
}

QList<QVideoFrame::PixelFormat> GLWidgetVideo::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
         return QList<QVideoFrame::PixelFormat>()
                 << QVideoFrame::Format_RGB32
                 << QVideoFrame::Format_ARGB32
                 << QVideoFrame::Format_ARGB32_Premultiplied;
     } else {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied;
     }
}


GLImageVideo::GLImageVideo(GLWidget *painter, QString path, int max_texture_size)
{
    this->painter = painter;
    this->path = path;
    this->max_texture_size = max_texture_size;
    this->media_player = new QMediaPlayer(painter);
    this->asurface = new GLWidgetVideo(this, painter);
    this->url = QUrl::fromLocalFile(path);
    this->media_player->setVideoOutput(this->asurface);
    this->media_player->setMedia(this->url);
    this->media_player->play();
    this->textures = new GLuint[1];
    this->geom = 0;
    this->angle = PI_F;
    GLAnimatedImage::init();
}


GLImageVideo::~GLImageVideo()
{
    glDeleteTextures(this->sgeom.vbo_data.size(), this->textures);
    delete[] this->textures;
    this->media_player->stop();
    delete this->media_player;
    delete this->asurface;
}

void GLImageVideo::setVideoSize(QSize size){
    if (!this->size.isValid()){
        this->size = size;
        this->init();
    }
}

void GLImageVideo::init(){
    this->create_geometry_squear();
    this->create_geometry_cylinder();
    this->geom = 0;
    this->switch_geometry(GT_SQUEAR);
    GLAnimatedImage::init();
}

void GLImageVideo::newVideoFrame(const QVideoFrame &frame){
    this->loaded = false;
    this->painter->newVideoFrame(this, (QVideoFrame&)frame);
    this->loaded = true;
    if (!this->painter->animating){
        this->painter->add_animator(0);
    }
}

void GLImageVideo::update_view(){
    if (this->size.isValid()){
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
}

void GLImageVideo::create_geometry_squear()
{
    //qDebug() << (float)this->img->width() << this->img->height();
    this->sgeom.type = GT_SQUEAR;
    this->sgeom.height = ((float)this->size.height()) / this->size.width();
    this->sgeom.width = 1.0;

    int texture_block_x = int(ceil(((float)this->size.width()) / this->max_texture_size));
    int texture_block_y = int(ceil(((float)this->size.height()) / this->max_texture_size));

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

void GLImageVideo::create_geometry_cylinder()
{
    //qDebug() << (float)this->img->width() << this->img->height();
    this->cgeom.type = GT_CYLINDER;
    float img_hw = ((float)this->size.height()) / this->size.width();
    this->cgeom.width = 1.0;

    int texture_block_x = int(ceil(((float)this->size.width()) / this->max_texture_size));
    int texture_block_y = int(ceil(((float)this->size.height()) / this->max_texture_size));

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
 }

void GLImageVideo::switch_geometry(char cylinder)
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

void GLImageVideo::reset()
{
    GLAnimatedImage::reset();
    this->update_view();
}
