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
#include "soilsheetmodule.h"

#include "soilsheetmodel.h"
#include "soilsheetmodelgui.h"
#include "soilsheetgui.h"
#include "soilsheet.h"
#include "qgraphicsitemviewer.h"

#include <QGridLayout>
#include <QIcon>
#include <QDockWidget>

class SoilSheetModulePrivate{
public:
    SoilSheetModulePrivate( UnitMeasure * ump,
                                SoilSheetModel * soilSheetModel,
                                SoilModel * soilModel,
                                QWidget * parent ){
        model = soilSheetModel;
        GUI = new SoilSheetGUI( ump, soilModel, parent );
        modelGUI = new SoilSheetModelGUI( soilSheetModel, parent );
        graphicsItemViewer = new QGraphicsItemViewer ( parent );
    }

    SoilSheetModel * model;
    SoilSheetGUI * GUI;
    SoilSheetModelGUI * modelGUI;
    QGraphicsItemViewer * graphicsItemViewer;
};

SoilSheetModule::SoilSheetModule(UnitMeasure * ump,
                                         SoilSheetModel * soilSheetModel,
                                         SoilModel * soilModel,
                                         QWidget *parent ) :
    StructModule(ump, parent ),
    m_d( new SoilSheetModulePrivate( ump, soilSheetModel, soilModel, this ) ){

    setLayout( new QGridLayout );

    layout()->addWidget( m_d->GUI );

    QDockWidget * modelDock = new QDockWidget(richName() + " - " + trUtf8("Lista"));
    modelDock->setObjectName( "SoilSheetModel" );
    modelDock->setWidget( m_d->modelGUI );
    m_panels->append(  modelDock );

    QDockWidget * viewerDock = new QDockWidget(richName() + " - " + trUtf8("Disegno"));
    viewerDock->setObjectName( "SoilSheetViewer" );
    viewerDock->setWidget( m_d->graphicsItemViewer );
    m_panels->append(  viewerDock );

    connect( soilSheetModel, SIGNAL(currentChanged(SoilSheet*)), this, SLOT(setCurrentSoilSheet(SoilSheet*)));
}

QString SoilSheetModule::name(){
    return QString("SoilSheet");
}

QString SoilSheetModule::richName(){
    return trUtf8("Paratie");
}

QIcon SoilSheetModule::icon(){
    return QIcon(":/images/soilsheet.svg");
}

void SoilSheetModule::setCurrentSoilSheet( SoilSheet * soilSheet ){

    m_d->graphicsItemViewer->removeAllQGraphicsItems();

    m_d->GUI->setSoilSheet( soilSheet );

    m_d->graphicsItemViewer->addQGraphicsItem( soilSheet->qGraphicsItem() );
}
