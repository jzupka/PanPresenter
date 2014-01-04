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

#ifndef LOADER_H
#define LOADER_H

#include "paintCore.h"
#include "bigimage.h"
#include "thumbnail.h"
#include <QVector>

class Loader : public QThread
{
    Q_OBJECT
public:
    Loader(int max_texture_size, int cache_size=5, QSize img_size=QSize(), QObject * parent=0);
    ~Loader();
    void run();
    void start();
    void set(int pos);
    void setFiles(QStringList files);
    int* newImageLoaded();

    QVector< CacheImage* > images;
    QStringList supported_formats;
    QMutex clean_image_mux;
private:
    void clean_cache();
    void update_range(int pos);
private:
    QStringList files;
    int cache_size;
    int max_cache_size;
    int pos;
    int images_count;
    int max_texture_size;
    QSize img_size;
    bool quit;
    int new_image_loaded;
    QMutex wait_set_mux;
    QWaitCondition wait_set;
    int load_pos_start;
    int load_pos_end;
    QSet<int> loaded;
};

#endif // LOADER_H
