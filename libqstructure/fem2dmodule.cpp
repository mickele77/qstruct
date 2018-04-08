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
#include "fem2dmodule.h"

#include "qfem2dgui.h"
#include "femstructure.h"
#include "hypothesis2d.h"
#include "vertexmodelgui.h"
#include "elementmodelgui.h"
#include "calcresultsgui.h"

#include <QGridLayout>
#include <QIcon>
#include <QList>
#include <QDockWidget>
#include <QShowEvent>

class Fem2DModulePrivate {
public:
    Fem2DModulePrivate( UnitMeasure * ump, SectionModel * sm, QWidget * parent ) {
        hypothesis = new Hypothesis2D();
        structure = new FEMStructure( sm, hypothesis, ump, parent );
        gui = new QFem2DGUI( structure, parent );
        vertexGUI = new VertexModelGUI( structure, parent );
        elementGUI = new ElementModelGUI( structure->elementModel(), structure->vertexModel(), parent );
        calcResultsGUI = new CalcResultsGUI( structure, parent );
    }
    ~Fem2DModulePrivate(){
        delete structure;
        delete hypothesis;
    }

    Hypothesis2D * hypothesis;
    FEMStructure * structure;
    QFem2DGUI * gui;
    VertexModelGUI * vertexGUI;
    ElementModelGUI * elementGUI;
    CalcResultsGUI * calcResultsGUI;
};


Fem2DModule::Fem2DModule( UnitMeasure * ump,
                          SectionModel * sm,
                          QWidget *parent) :
    StructModule( ump, parent ),
    m_d( new Fem2DModulePrivate( ump, sm, this) ){

    connect( m_d->calcResultsGUI, SIGNAL(resultsChanged()), m_d->elementGUI, SLOT(updateResults()));
    connect( m_d->calcResultsGUI, SIGNAL(addGLItem(GLItem*)), m_d->gui, SLOT(setExtraGLItem(GLItem*)));

    setLayout( new QGridLayout(this));
    layout()->addWidget( m_d->gui );

    QDockWidget * dockCalcResults = new QDockWidget(richName() + " - " + trUtf8("Calcolo"));
    dockCalcResults->setObjectName( "FEM2DCalcResults" );
    dockCalcResults->setWidget( m_d->calcResultsGUI );
    m_panels->append( dockCalcResults );

    QDockWidget * dockVertex = new QDockWidget(richName() + " - " + trUtf8("Vertici"));
    dockVertex->setObjectName( "FEM2DVertex");
    dockVertex->setWidget( m_d->vertexGUI );
    m_panels->append( dockVertex );

    QDockWidget * dockElement = new QDockWidget(richName() + " - " + trUtf8("Elementi"));
    dockElement->setObjectName( "FEM2DElement");
    dockElement->setWidget( m_d->elementGUI );
    m_panels->append( dockElement );

    connect( m_d->calcResultsGUI, SIGNAL(resultsChanged()), m_d->elementGUI, SLOT(updateResults()));
}

Fem2DModule::~Fem2DModule(){
    delete m_d;
}

QString Fem2DModule::name(){
    return QString("Fem2D");
}

QString Fem2DModule::richName(){
    return trUtf8("FEM 2D");
}

QIcon Fem2DModule::icon(){
    return QIcon(":/images/fem2d.svg");
}

void Fem2DModule::showEvent ( QShowEvent * event ){
    if( !event->spontaneous() ) {
        m_d->elementGUI->updateData();
    }
}
