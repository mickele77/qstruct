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
#include "soilsheetmodelgui.h"
#include "ui_soilsheetmodelgui.h"

#include "soilsheetmodel.h"
#include "soilsheet.h"
#include "qstringplus.h"

SoilSheetModelGUI::SoilSheetModelGUI(SoilSheetModel * m, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::SoilSheetModelGUI),
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

SoilSheetModelGUI::~SoilSheetModelGUI(){
    delete m_ui;
}

void SoilSheetModelGUI::insert(){
    QModelIndexList listRows = m_ui->tableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_model->insertRows( listRows.last().row()+1, listRows.size());
    } else {
        int row = m_ui->tableView->currentIndex().row();
        if( row < 0 || row > m_model->soilSheetCount() )
            row = m_model->soilSheetCount() - 1 ;
        m_model->insertRows( row + 1 );
    }
}

void SoilSheetModelGUI::remove(){
    QModelIndexList listRows = m_ui->tableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        for( int i=(listRows.size()-1); i >= 0; i-- ){
            int row = listRows.at(i).row();
            if( row >= 0 && row < m_model->soilSheetCount() ) {
                m_model->removeRows( row );
            }
        }
    }
}

void SoilSheetModelGUI::setDefault(){
    m_model->setDefaultSoilSheet( m_ui->tableView->currentIndex().row() );
}

void SoilSheetModelGUI::setCurrent(){
    int row = m_ui->tableView->currentIndex().row();
    if( row >= 0 && row < m_model->soilSheetCount() ){
        m_model->setCurrentSoilSheet( row );
    } else {
        m_model->setCurrentSoilSheet( -1 );
    }
}
