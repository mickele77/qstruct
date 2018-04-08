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
#ifndef FEM2DMODULE_H
#define FEM2DMODULE_H

#include "structmodule.h"

class Fem2DModulePrivate;

class SectionModel;

/**
* @class Fem2DModule
*
* @brief Modulo Fem2DModule dell'applicazione QStruct
*
* Modulo per il calcolo FEM di strutture 2D
*
* @author Michele Mocciola
*
*/

class Fem2DModule : public StructModule
{
    Q_OBJECT

public:
    explicit Fem2DModule( UnitMeasure * ump,
                          SectionModel * sectionModel,
                          QWidget *parent = 0 );
    ~Fem2DModule();

    QString name();
    QString richName();
    QIcon icon();

    void showEvent ( QShowEvent * event );

signals:

private:
    Fem2DModulePrivate * m_d;
};

#endif // FEM2DMODULE_H
