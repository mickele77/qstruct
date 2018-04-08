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
#include "timberconnectionmodule.h"

#include "timberconnectionmodel.h"
#include "timberconnectionmodelgui.h"
#include "timberconnectiongui.h"

#include <QGridLayout>
#include <QIcon>
#include <QList>
#include <QDockWidget>

class TimberConnectionModulePrivate{
public:
    TimberConnectionModulePrivate( TimberConnectionModel * tbConModel ):
        model( tbConModel ),
        GUI(NULL){
    }
    TimberConnectionModel * model;
    TimberConnectionGUI * GUI;
};

TimberConnectionModule::TimberConnectionModule(UnitMeasure * um,
                                               MaterialModel * materialModel,
                                               TimberConnectionModel * timberConnectionModel,
                                               QWidget *parent) :
    StructModule(um, parent ),
    m_d( new TimberConnectionModulePrivate(timberConnectionModel )){
    setLayout( new QGridLayout );

    // Aggiungere qui le nuove GUI
    m_d->GUI = new TimberConnectionGUI( um, materialModel, this );
    layout()->addWidget( m_d->GUI );

    QDockWidget * dock = new QDockWidget(richName() + " - " + trUtf8("Lista"));
    dock->setObjectName( "TimberConnectionDock" );
    dock->setWidget( new TimberConnectionModelGUI( timberConnectionModel, this ) );
    m_panels->append(  dock );

    connect( m_d->model, SIGNAL(currentChanged(TimberConnection*)), this, SLOT(setCurrentConnection(TimberConnection*)));
}

QString TimberConnectionModule::name(){
    return QString("TimberConnection");
}

QString TimberConnectionModule::richName(){
    return trUtf8("CollegaLegno");
}

QIcon TimberConnectionModule::icon(){
    return QIcon(":/images/timberconnection.svg");
}

void TimberConnectionModule::setCurrentConnection( TimberConnection * con ){
    if( m_d->GUI != NULL ) {
        m_d->GUI->setConnection( con );
    }
}
