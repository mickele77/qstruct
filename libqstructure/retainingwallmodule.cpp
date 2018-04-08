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
#include "retainingwallmodule.h"

#include "retainingwallmodel.h"
#include "retainingwallmodelgui.h"
#include "retainingwallgui.h"
#include "retainingwall.h"
#include "qgraphicsitemviewer.h"

#include <QGridLayout>
#include <QIcon>
#include <QDockWidget>

class RetainingWallModulePrivate{
public:
    RetainingWallModulePrivate( UnitMeasure * ump,
                                RetainingWallModel * retainingWallModel,
                                SoilModel * soilModel,
                                QWidget * parent ){
        model = retainingWallModel;
        GUI = new RetainingWallGUI( ump, soilModel, parent );
        modelGUI = new RetainingWallModelGUI( retainingWallModel, parent );
        graphicsItemViewer = new QGraphicsItemViewer ( parent );
    }

    RetainingWallModel * model;
    RetainingWallGUI * GUI;
    RetainingWallModelGUI * modelGUI;
    QGraphicsItemViewer * graphicsItemViewer;
};

RetainingWallModule::RetainingWallModule(UnitMeasure * ump,
                                         RetainingWallModel * retainingWallModel,
                                         SoilModel * soilModel,
                                         QWidget *parent ) :
    StructModule(ump, parent ),
    m_d( new RetainingWallModulePrivate( ump, retainingWallModel, soilModel, this ) ){

    setLayout( new QGridLayout );

    layout()->addWidget( m_d->GUI );

    QDockWidget * modelDock = new QDockWidget(richName() + " - " + trUtf8("Lista"));
    modelDock->setObjectName( "RetainingWallModel" );
    modelDock->setWidget( m_d->modelGUI );
    m_panels->append(  modelDock );

    QDockWidget * viewerDock = new QDockWidget(richName() + " - " + trUtf8("Disegno"));
    viewerDock->setObjectName( "RetainingWallViewer" );
    viewerDock->setWidget( m_d->graphicsItemViewer );
    m_panels->append(  viewerDock );

    connect( retainingWallModel, SIGNAL(currentChanged(RetainingWall*)), this, SLOT(setCurrentRetainingWall(RetainingWall*)));
}

QString RetainingWallModule::name(){
    return QString("RetainingWall");
}

QString RetainingWallModule::richName(){
    return trUtf8("SostegnoMuro");
}

QIcon RetainingWallModule::icon(){
    return QIcon(":/images/retainingwall.svg");
}

void RetainingWallModule::setCurrentRetainingWall( RetainingWall * retainingWall ){

    m_d->graphicsItemViewer->removeAllQGraphicsItems();

    m_d->GUI->setRetainingWall( retainingWall );

    m_d->graphicsItemViewer->addQGraphicsItem( retainingWall->qGraphicsItem() );
}
