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
#ifndef TIMBERCONNECTIONMODULE_H
#define TIMBERCONNECTIONMODULE_H

#include "structmodule.h"

class MaterialModel;
class TimberConnectionModulePrivate;
class TimberConnectionModel;
class TimberConnection;

/**
* @class TimberConnectionModule
*
* @brief Modulo TimberConnectionModule dell'applicazione QStruct
*
* Modulo per la gestione dei materiali
*
* @author Michele Mocciola
*
*/

class TimberConnectionModule : public StructModule
{
    Q_OBJECT

public:
    explicit TimberConnectionModule( UnitMeasure * um,
                                     MaterialModel * materialModel,
                                     TimberConnectionModel * timberConnectionModel,
                                     QWidget *parent = 0 );
    QString name();
    QString richName();
    QIcon icon();

private:
    TimberConnectionModulePrivate * m_d;

private slots:
    void setCurrentConnection( TimberConnection * );
};

#endif // TIMBERCONNECTIONMODULE_H
