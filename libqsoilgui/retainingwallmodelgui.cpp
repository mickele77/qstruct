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
#include "retainingwallmodelgui.h"
#include "ui_retainingwallmodelgui.h"

#include "retainingwallmodel.h"
#include "retainingwall.h"
#include "qstringplus.h"

RetainingWallModelGUI::RetainingWallModelGUI(RetainingWallModel * m, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::RetainingWallModelGUI),
    m_model( m ){
    // setup UI
    m_ui->setupUi(this);

    // Assignes model to tableView
    m_ui->tableView->setModel( m_model );

    // Connects buttons
    connect( m_ui->newPButton, SIGNAL(clicked()), this, SLOT( insert() ) );
    connect( m_ui->delPButton, SIGNAL(clicked()), this, SLOT( remove()) );
    connect( m_ui->defaultPButton, SIGNAL(clicked()), this, SLOT(setDefault()) );

    // Rifinisce l'aspetto di tableView
    m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // m_ui->tableView->hideColumn(0);
    // m_ui->tableView->hideColumn(1);

    connect( m_ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(setCurrent()));
}

RetainingWallModelGUI::~RetainingWallModelGUI(){
    delete m_ui;
}

void RetainingWallModelGUI::insert(){
    QModelIndexList listRows = m_ui->tableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_model->insertRows( listRows.last().row()+1, listRows.size());
    } else {
        int row = m_ui->tableView->currentIndex().row();
        if( row < 0 || row > m_model->retainingWallCount() )
            row = m_model->retainingWallCount() - 1 ;
        m_model->insertRows( row + 1 );
    }
}

void RetainingWallModelGUI::remove(){
    QModelIndexList listRows = m_ui->tableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        for( int i=(listRows.size()-1); i >= 0; i-- ){
            int row = listRows.at(i).row();
            if( row >= 0 && row < m_model->retainingWallCount() ) {
                m_model->removeRows( row );
            }
        }
    }
}

void RetainingWallModelGUI::setDefault(){
    m_model->setDefaultRetainingWall( m_ui->tableView->currentIndex().row() );
}

void RetainingWallModelGUI::setCurrent(){
    int row = m_ui->tableView->currentIndex().row();
    if( row >= 0 && row < m_model->retainingWallCount() ){
        m_model->setCurrentRetainingWall( row );
    } else {
        m_model->setCurrentRetainingWall( -1 );
    }
}
