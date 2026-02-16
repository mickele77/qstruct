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
#include "soilmodule.h"

#include "soilmodel.h"
#include "soilmodelgui.h"
#include "soilgui.h"
#include "soil.h"

#include <QGridLayout>
#include <QIcon>
#include <QList>
#include <QDockWidget>

class SoilModulePrivate{
public:
    SoilModulePrivate( SoilModel * sModel ):
        model( sModel ){
    };
    SoilModel * model;
    SoilGUI * GUI;
};

SoilModule::SoilModule(UnitMeasure * um,
                       SoilModel * soilModel,
                       QWidget *parent) :
    StructModule(um, parent ),
    m_d( new SoilModulePrivate( soilModel )){
    setLayout( new QGridLayout );

    // Aggiungere qui le nuove GUI
    m_d->GUI = new SoilGUI( um, this );
    layout()->addWidget( m_d->GUI );

    QDockWidget * dock = new QDockWidget(richName() + " - " + tr("Lista"));
    dock->setObjectName( "SoilDock" );
    dock->setWidget( new SoilModelGUI( soilModel, this ) );
    m_panels->append(  dock );

    connect( soilModel, &SoilModel::currentChanged, this, &SoilModule::setCurrentSoil );
}

QString SoilModule::name(){
    return QString("Soil");
}

QString SoilModule::richName(){
    return tr("Terreno");
}

QIcon SoilModule::icon(){
    return QIcon(":/images/soil.svg");
}

void SoilModule::setCurrentSoil( Soil * soil ){
    m_d->GUI->setSoil( soil );
}
