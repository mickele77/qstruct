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
#ifndef RETAININGWALLMODULE_H
#define RETAININGWALLMODULE_H

#include "structmodule.h"

class RetainingWallModel;
class RetainingWall;
class SoilModel;
class RetainingWallModulePrivate;

/**
* @class RetainingWallModule
*
* @brief Modulo RetainingWallModule dell'applicazione QStruct
*
* Modulo per la gestione dei muri controterra
*
* @author Michele Mocciola
*
*/

class RetainingWallModule : public StructModule
{
    Q_OBJECT

public:
    explicit RetainingWallModule( UnitMeasure * um,
                                  RetainingWallModel * retainingWallModel,
                                  SoilModel * soilModel,
                                  QWidget *parent = 0 );
    QString name();
    QString richName();
    QIcon icon();

signals:

private:
    RetainingWallModulePrivate * m_d;

private slots:
    void setCurrentRetainingWall( RetainingWall * );
};

#endif // RETAININGWALLMODULE_H
