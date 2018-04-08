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
#include "soilmodelgui.h"
#include "ui_soilmodelgui.h"

#include <QMessageBox>
#include <QMenu>
#include <QAction>

#include "qstringplus.h"
#include "soil.h"

SoilModelGUI::SoilModelGUI(SoilModel * m, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::SoilModelGUI),
    m_model( m ){
    // setup UI
    m_ui->setupUi(this);

    // Assignes model to tableView
    m_ui->tableView->setModel( m_model );

    // Connects buttons
    QMenu * newSoilMenu = new QMenu(this);
    QAction * clayAct = newSoilMenu->addAction( trUtf8("Argilla"));
    connect( clayAct, SIGNAL(triggered()), this, SLOT(insertClay()));
    QAction * sandAct = newSoilMenu->addAction( trUtf8("Sabbia"));
    connect( sandAct, SIGNAL(triggered()), this, SLOT(insertSand()));
    m_ui->newPButton->setMenu( newSoilMenu );

    connect( m_ui->delPButton, SIGNAL(clicked()), this, SLOT( remove()) );
    connect( m_ui->defaultPButton, SIGNAL(clicked()), this, SLOT(setDefault()) );

    // Rifinisce l'aspetto di tableView
    m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // m_ui->tableView->hideColumn(0);
    // m_ui->tableView->hideColumn(1);

    connect( m_ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(setCurrent()));
}

SoilModelGUI::~SoilModelGUI(){
    delete m_ui;
}

void SoilModelGUI::insertSoil( SoilModel::SoilType type ){
    QModelIndexList listRows = m_ui->tableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_model->insertRows( type, listRows.last().row()+1, listRows.size());
    } else {
        int row = m_ui->tableView->currentIndex().row();
        if( row < 0 || row > m_model->soilCount() )
            row = m_model->soilCount() - 1 ;
        m_model->insertRows( type, row + 1 );
    }
}

void SoilModelGUI::insertClay(){
    insertSoil( SoilModel::clay );
}

void SoilModelGUI::insertSand(){
    insertSoil( SoilModel::sand);
}

void SoilModelGUI::remove(){
    QModelIndexList listRows = m_ui->tableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        for( int i=(listRows.size()-1); i >= 0; i-- ){
            int row = listRows.at(i).row();
            if( row >= 0 && row < m_model->soilCount() ) {
                if( m_model->soil( row )->isUsed() ){
                    QString title = m_model->soil(row)->name->valueStr() + trUtf8(" già in uso");
                    QString message = trUtf8("Non si può rimuovere ") + m_model->soil(row)->name->valueStr() + trUtf8(" poiché è usato da una o più sezioni");
                    QMessageBox::critical(0, title, message, QMessageBox::Ok);
                } else {
                    m_model->removeRows( row );
                }
            }
        }
    }
}

void SoilModelGUI::setDefault(){
    m_model->setDefaultSoil( m_ui->tableView->currentIndex().row() );
}

void SoilModelGUI::setCurrent(){
    int row = m_ui->tableView->currentIndex().row();
    if( row >= 0 && row < m_model->soilCount() ){
        m_model->setCurrentSoil( row );
    } else {
        m_model->setCurrentSoil( -1 );
    }
}
