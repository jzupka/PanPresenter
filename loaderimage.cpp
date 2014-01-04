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

#include "loaderimage.h"

LoaderImage::LoaderImage()
{
    this->b_loaded = false;
}


void LoaderImage::wait(){
    //qDebug() << "************Start try load";
    std::unique_lock<std::mutex> lk(this->loaded_mux);
    bool *p = &this->b_loaded;
    this->loaded.wait(lk, [p]{return *p;});
}


void LoaderImage::unload(){
    this->b_loaded = false;
}

void LoaderImage::load(){
    std::lock_guard<std::mutex> lk(this->loaded_mux);
    this->b_loaded = true;
    this->loaded.notify_all();
}

bool LoaderImage::is_loaded(){
    return this->b_loaded;
}
