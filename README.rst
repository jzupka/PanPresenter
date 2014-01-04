==================
Panorama Presenter
==================

Panorama Presenter could be used for presenting images and videos.
There is possible present panorama reasonable way without delays.

Dependences
===========

- qt5
- opengl
- gstreamer or alternative for qt

Install
=======
- git clone https://github.com/jzupka/PanPresenter.git
- cd PanResenter
- qmake-qt5
- make
- sudo make install


Run
===

Presenter
  Show window for selecting directory with images.
Presenter 


Control
=======

Keys:
-----

::

 esc                   exit program
 space, right, down    next_image
 left, up              prev_image
 a                     present panorama like moveable rectagnle
 d                     present panorama like cylinder
 s                     present panorama like cylinder with stop on end of panorama
 w                     return to init image position and view
 r                     change to rectangle form
 c                     change to cylindrical form
 t                     transform to cylindrical form
 u                     transform to rectangle form

Mouse control:
--------------
Image moving mouse click and drag.

Image zoom scroll by mouse.
    
Image name format for panorma.
==============================

::

  name#angle.jpg
     angle in degree
     if angle is sub zero no slide animation is used when image is switched.
   
examples:
---------

::

  name#360.jpg      Show image in full cylinder and slide when image is switched.
  name#-60.jpg      Show image on 60° without slide.
    
When the cursor is in bottom side of window then miniature of images are displayed
and could be used for faster moving through images.

