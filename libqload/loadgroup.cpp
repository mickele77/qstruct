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
#include "loadgroup.h"

#include "loaddurationclass.h"

#include "tablemodelplusprivate.h"

#include "load.h"

#include <QString>
#include <QList>
#include <QObject>

class LoadGroupPrivate{
public:
    LoadGroupPrivate( const QString & nn, double mm ){
        name = nn;
        molt = mm;
        container = new QList<Load *>;
        durationClass = new LoadDurationClass( LoadDurationClass::instantaneous, QObject::trUtf8("Classe durata") );
    };
    QString name;
    double molt;
    QList<Load *> * container;
    LoadDurationClass * durationClass;
};

LoadGroup::LoadGroup( UnitMeasure * ump, const QString & nn,  double mm, QObject * parent ):
    TableModelPlus( "LoadGroup", ump, parent ),
    m_d( new LoadGroupPrivate(nn, mm ) ){
}
