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

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

#include "glwidget.h"
#include "loader.h"
#include <QTimer>
#include <QTime>

class GLWidget;

class Pos{
public:
    Pos(int max_size);
    Pos(const Pos &pos);
    void next();
    void prev();
    int get();
    void set(int pos);
private:
    int pos;
    int max_size;
};


class Presenter : public QMainWindow
{
    Q_OBJECT

public:
    Presenter(QMainWindow *parent = 0);
    ~Presenter();
    void setPath(QStringList path);
private:
    char set_media(int pos, QString path);

private slots:
    void keyPressEvent(QKeyEvent *);
    void update_pos();
    void set_next(QMediaPlayer::State state);
    void set_new_pos(int pos);
    void scroll_new_pos(int pos);

private:
    Loader *loader;
    Loader *tloader;
    GLWidget *gl_widget;
    QStringList files;
    Pos pos;
    QTime image_change_old_time;
    int painter_pos;
    QTimer image_change;

public:
    QStringList Image_Ext;
    QStringList Video_Ext;
};

#endif
