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
#ifndef STRUCTMODULE_H
#define STRUCTMODULE_H

#include <QWidget>

class UnitMeasure;

template <typename T> class QList;

class QXmlStreamReader;
class QFile;
class QDockWidget;
class QIcon;

/**
* @class StructModule
*
* @brief Modulo di QStruct
*
* Classe virtuale necessaria per definire i moduli di QStruct
*
* @author Michele Mocciola
*
*/

class StructModule : public QWidget
{
protected:
    UnitMeasure * m_unitMeasure;
    QList<QDockWidget *> * m_panels;
    QByteArray m_state;
public:
    StructModule( UnitMeasure *um, QWidget * parent );

    QList<QDockWidget *> panels();

    virtual QString name() = 0;
    virtual QString richName() = 0;
    virtual QIcon icon() = 0;

    void setState( const QByteArray & byte );
    QByteArray state();
};

#endif // STRUCTMODULE_H
