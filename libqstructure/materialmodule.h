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
#ifndef MATERIALMODULE_H
#define MATERIALMODULE_H

#include "structmodule.h"

class MaterialModulePrivate;

class MaterialModel;
class Material;

/**
* @class MaterialModule
*
* @brief Modulo MaterialModule dell'applicazione QStruct
*
* Modulo per la gestione dei materiali
*
* @author Michele Mocciola
*
*/

class MaterialModule : public StructModule
{
    Q_OBJECT

public:
    explicit MaterialModule(UnitMeasure * um,
                            MaterialModel * materialModel,
                            const QString & settingsFile,
                            QWidget *parent = 0 );
    QString name();
    QString richName();
    QIcon icon();

private:
    MaterialModulePrivate * m_d;

private slots:
    void setCurrentMaterial( Material * );
    void setCurrentMaterialNULL();
};

#endif // MATERIALMODULE_H
