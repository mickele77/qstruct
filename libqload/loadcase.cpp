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
#include "loadcase.h"
#include "tablemodelplusprivate.h"

#include "qstringplus.h"
#include "loadgroup.h"

// #include <QMap>
#include <QList>

class LoadGroupEntry{
public:
    LoadGroupEntry(LoadGroup * lg, double m):
        loadGroup(lg),
        molt(m){
    };
    LoadGroup * loadGroup;
    double molt;
};

class LoadCasePrivate{
public:
    LoadCasePrivate(){

    };
    void insertLoadGroup( LoadGroup * lg, double m, int p ){
        // l'id deve essere unico
        /*while( sectionMap.contains(s->id->valueNormal()) ){
            s->nextId();
        }*/
        sectionContainer.insert( p, new LoadGroupEntry(lg, m) );
        // sectionMap.insert( s->id->valueNormal(), s );
    };
    void removeLoadGroup( int p ){
        // sectionMap.remove( sectionContainer.at(p)->id->valueNormal() );
        delete sectionContainer.at(p);
        sectionContainer.removeAt(p);
    };

    QList<LoadGroupEntry *> sectionContainer;
    // QMap<unsigned int, LoadGroupEntry *> sectionMap;
};

LoadCase::LoadCase( UnitMeasure * ump, LSType::Type lst, const QString & nn, QObject * parent ):
    TableModelPlus( "LoadCase", ump, parent ),
    lsType(new LSType(lst, "lsType")),
    name( new QStringPlus( nn, "name")),
    m_dd( new LoadCasePrivate() ) {

    QList<QString> headerList;
    headerList << trUtf8("Nome")
               << trUtf8("Tipo")
               << trUtf8("Durata");
    m_d->setHeaders( headerList );
}
