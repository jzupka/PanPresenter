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

#include "tools.h"

void NormalizeAngle(float *rotate){
    if(rotate[0] > 180){
        rotate[0] -= 360;
    }else if (rotate[0] < -180){
        rotate[0] += 360;
    }
    if (rotate[1] > 180){
        rotate[1] -= 360;
    } else if(rotate[1] < -180){
        rotate[1] += 360;
    }
}
