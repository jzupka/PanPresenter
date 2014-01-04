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

#ifndef GLIMAGEVIDEO_H
#define GLIMAGEVIDEO_H

#include <QMediaPlayer>
#include <QVideoFrame>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include "paintCore.h"

class GLWidgetVideo;

class GLImageVideo : public GLAnimatedImage
{
public:
    GLImageVideo(GLWidget *painter, QString path, int max_texture_size);
    virtual ~GLImageVideo();
    void init();
    void reset();
    void update_view();
    void switch_geometry(char cylinder);
    void create_geometry_squear();
    void create_geometry_cylinder();
    void setVideoSize(QSize size);
    void newVideoFrame(const QVideoFrame &frame);

    QSize size;

    QString path;
    QUrl url;
    QMediaPlayer *media_player;
    GLWidgetVideo *asurface;
};


class GLWidgetVideo : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    GLWidgetVideo(GLImageVideo *gl_imagevideo, GLWidget *parent = 0);
    ~GLWidgetVideo() {}

    QSize nativeResolution() const;
    bool isFormatSupported(const QVideoSurfaceFormat &format) const;
    QVideoSurfaceFormat nearestFormat(const QVideoSurfaceFormat &format) const;
    bool present(const QVideoFrame &frame);
    bool start(const QVideoSurfaceFormat &format);
    void stop();
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
private:
    GLImageVideo* gl_imagevideo;
    GLWidget* gl_widget;
};

#endif // GLIMAGEVIDEO_H

