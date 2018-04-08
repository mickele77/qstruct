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
#include "elementmodelgui.h"
#include "ui_elementmodelgui.h"

#include "beamgui.h"

#include <QMenu>
#include <QAction>

#include "qstringplus.h"
#include "vertexmodel.h"

ElementModelGUI::ElementModelGUI(ElementModel * m, VertexModel * vm, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ElementModelGUI),
    m_model( m ),
    m_vertexModel( vm ){
    // setup UI
    m_ui->setupUi(this);

    // Assignes model to tableView
    m_ui->tableView->setModel( m_model );

    // Collega i pulsanti
    QMenu * newElementMenu = new QMenu( this );
    QAction * beamAct = newElementMenu->addAction( trUtf8("Trave"));
    connect( beamAct, SIGNAL(triggered()), this, SLOT(insertBeam()));
    m_ui->newPButton->setMenu( newElementMenu );

    connect( m_ui->delPButton, SIGNAL(clicked()), this, SLOT( remove()) );

    // Columns automatically resize
    m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect( m_ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(setCurrent()));

    m_currentElementGUI = NULL;
}

ElementModelGUI::~ElementModelGUI(){
    delete m_ui;
}

void ElementModelGUI::insertElement( ElementModel::ElementType type ){
    QModelIndexList listRows = m_ui->tableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_model->insert( type, listRows.last().row()+1, listRows.size());
    } else {
        int row = m_ui->tableView->currentIndex().row();
        if( row < 0 || row > m_model->nElements() )
            row = m_model->nElements() - 1 ;
        m_model->insert( type, row + 1 );
    }
}

void ElementModelGUI::insertBeam(){
    insertElement( ElementModel::elementBeam );
}

void ElementModelGUI::remove(){
    QModelIndexList listRows = m_ui->tableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        for( int i=(listRows.size()-1); i >= 0; i-- ){
            int row = listRows.at(i).row();
            if( row >= 0 && row < m_model->nElements() ) {
                m_model->remove( row );
            }
        }
    }
}

void ElementModelGUI::setCurrent(){
    int row = m_ui->tableView->currentIndex().row();
    if( row >= 0 && row < m_model->nElements() ){
        if( m_currentElementGUI != NULL ){
            if( m_currentElementGUI->typeElement().toUpper() != m_model->element(row)->typeElement().toUpper() ){
                delete m_currentElementGUI;
                m_currentElementGUI = NULL;
            }
        }
        if( m_currentElementGUI == NULL ){
            if( m_model->element(row)->typeElement().toUpper() == "BEAM" ){
                m_currentElementGUI = new BeamGUI( (Beam *) m_model->element(row), m_vertexModel, m_model->sectionModel() );
                m_ui->splitter->addWidget( m_currentElementGUI );
            }
        } else {
            m_currentElementGUI->setElement( m_model->element(row) );
        }
    }
}

void ElementModelGUI::updateData(){
    if( m_currentElementGUI ){
        m_currentElementGUI->updateData();
    }
}

void ElementModelGUI::updateResults(){
    if( m_currentElementGUI ){
        m_currentElementGUI->updateResults();
    }
}
