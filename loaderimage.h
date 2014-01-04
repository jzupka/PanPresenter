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

#ifndef LOADERIMAGE_H
#define LOADERIMAGE_H

#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

class LoaderImage
{
public:
    LoaderImage();
    bool is_loaded();
    void wait();
    void load();
    void unload();

protected:
    std::mutex loaded_mux;
public:
    std::condition_variable loaded;
    bool b_loaded;
};

#endif // LOADERIMAGE_H
