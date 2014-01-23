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

#include <QApplication>
#include <QFileDialog>
#include <QIcon>
#include "presenter.h"

#ifdef __linux__
   #include <X11/Xlib.h>
#endif


int main(int argc, char *argv[])
{
    #ifdef __linux__
       XInitThreads();
    #endif

    QApplication app(argc, argv);
    Q_INIT_RESOURCE(Presenter);
    Presenter window;
    QStringList path;
    if(argc == 1){
        path.append(QFileDialog::getExistingDirectory(0, QString("Open Image Directory"),
                                                 QDir::homePath(), QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks));
        if (path[0] == ""){
            app.exit();
            exit(0);
        }
    }else if(argc == 2){
        path.append(argv[1]);
    }else if(argc > 2){
        for(int i = 1;i < argc; ++i){
            path.append(argv[i]);
        }
    }
    app.setApplicationName("Presenter");
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    window.setPath(path);
    window.show();
    return app.exec();
}
