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
#ifndef SOILMODULE_H
#define SOILMODULE_H

#include "structmodule.h"

class SoilModel;
class Soil;
class SoilModulePrivate;

/**
* @class SoilModule
*
* @brief Modulo SoilModule dell'applicazione QStruct
*
* Modulo per la gestione dei terreni
*
* @author Michele Mocciola
*
*/

class SoilModule : public StructModule
{
    Q_OBJECT

public:
    explicit SoilModule(UnitMeasure * um,
                        SoilModel * soilModel,
                        QWidget *parent = 0 );
    QString name();
    QString richName();
    QIcon icon();

signals:

private:
    SoilModulePrivate * m_d;

private slots:
    void setCurrentSoil( Soil * );
};

#endif // SOILMODULE_H
