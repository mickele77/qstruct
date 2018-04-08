/*
            Copyright (C) 2014-2017 Michele Mocciola

            This file is part of QStruct.

            QStruct is free software: you can redistribute it and/or modify
            it under the terms of the GNU General Public License as published by
            the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.

            QStruct is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
            GNU General Public License for more details.

            You should have received a copy of the GNU General Public License
            along with QStruct.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QApplication>

#include "qstructuregui.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    #ifdef _WIN32
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES) ;
    #endif

/*    QFont font;
    font.setFamily(font.defaultFamily());
    app.setFont(font);*/

    QStructureGUI window;
    window.show();
    return app.exec();
}
