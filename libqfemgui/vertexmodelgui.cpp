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
#include "vertexmodelgui.h"
#include "ui_vertexmodelgui.h"


#include "femstructure.h"
#include "vertexmodel.h"
#include "vertex.h"

#include <QMessageBox>


VertexModelGUI::VertexModelGUI( FEMStructure * str, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::VertexModelGUI ),
    m_structure( str ){
    // setup UI
    m_ui->setupUi(this);

    // Assignes model to tableView
    m_ui->tableView->setModel( m_structure->vertexModel() );

    // Columns automatically resize
    m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Buttons' connections
    connect( m_ui->newPButton, SIGNAL(clicked()), this, SLOT( insert()) );
    connect( m_ui->delPButton, SIGNAL(clicked()), this, SLOT( remove()) );
}

VertexModelGUI::~VertexModelGUI(){
    delete m_ui;
}

void VertexModelGUI::insert( ){
    QModelIndexList listRows = m_ui->tableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_structure->vertexModel()->insertRows( listRows.last().row()+1, listRows.size());
    } else {
        int row = m_ui->tableView->currentIndex().row();
        if( row < 0 || row > m_structure->vertexModel()->vertexCount() )
            row = m_structure->vertexModel()->vertexCount() - 1 ;
        m_structure->vertexModel()->insertRows( row + 1 );
    }
}

void VertexModelGUI::remove(){
    QModelIndexList listRows = m_ui->tableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        for( int i=(listRows.size()-1); i >= 0; i-- ){
            int row = listRows.at(i).row();
            if( row >= 0 && row < m_structure->vertexModel()->vertexCount() ) {
                if( m_structure->isVertexUsed( m_structure->vertexModel()->vertex( row ) ) ){
                    QString title = trUtf8("Vertice già in uso");
                    QString message = trUtf8("Non è possibile rimuoverlo");
                    QMessageBox::critical(0, title, message, QMessageBox::Ok);
                } else {
                    m_structure->vertexModel()->removeRows( row );
                }
            }
        }
    }
}
