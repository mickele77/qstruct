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
#include "sectionmodelgui.h"
#include "ui_sectionmodelgui.h"

#include "sectionmaterialdelegate.h"
#include "qstringplus.h"
#include "section.h"

#include <QMessageBox>
#include <QMenu>
#include <QAction>

SectionModelGUI::SectionModelGUI(SectionModel * sectModel, MaterialModel * matModel, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::SectionModelGUI),
    m_model( sectModel ){
    // setup UI
    m_ui->setupUi(this);

    m_ui->tableView->setItemDelegate( new SectionMaterialDelegate(matModel, MaterialModel::GenericMaterial, sectModel->materialColumn(), -1 )  );

    // Assegna il modello a tableView
    m_ui->tableView->setModel( m_model );

    // Collega i pulsanti
    QMenu * newSectionMenu = new QMenu( this );
    QAction * steelHAct = newSectionMenu->addAction( trUtf8("Sezione in Acciaio"));
    connect( steelHAct, &QAction::triggered, this, &SectionModelGUI::insertSectionSteelH );
    QAction * rCncrAct = newSectionMenu->addAction( trUtf8("Sezione in C.A."));
    connect( rCncrAct, &QAction::triggered, this, &SectionModelGUI::insertSectionRCncr );
    QAction * timberAct = newSectionMenu->addAction( trUtf8("Sezione in Legno"));
    connect( timberAct, &QAction::triggered, this, &SectionModelGUI::insertSectionTimber );
    QAction * sectionThinContSteelAct = newSectionMenu->addAction( trUtf8("Sezione piegata a freddo"));
    connect( sectionThinContSteelAct, &QAction::triggered, this, &SectionModelGUI::insertSectionThinContSteel );
    newSectionMenu->addSeparator();
    QAction * pointsAct = newSectionMenu->addAction( trUtf8("Sezione per punti"));
    connect( pointsAct, &QAction::triggered, this, &SectionModelGUI::insertSectionPoints );
    QAction * sectThinContAct = newSectionMenu->addAction( trUtf8("Sezione sottile continua"));
    connect( sectThinContAct, &QAction::triggered, this, &SectionModelGUI::insertSectionThinCont );
    QAction * sectThinAct = newSectionMenu->addAction( trUtf8("Sezione sottile"));
    connect( sectThinAct, &QAction::triggered, this, &SectionModelGUI::insertSectionThin );
    QAction * sectProfileAct = newSectionMenu->addAction( trUtf8("Sezione profilo"));
    connect( sectProfileAct, &QAction::triggered, this, &SectionModelGUI::insertSectionProfile );
    QAction * sectAct = newSectionMenu->addAction( trUtf8("Sezione generica"));
    connect( sectAct, &QAction::triggered, this, &SectionModelGUI::insertSectionGeneric );
    m_ui->newPButton->setMenu( newSectionMenu );

    connect( m_ui->delPButton, SIGNAL(clicked()), this, SLOT( remove()) );

    // Le colonne hanno dimensione fissa ed uguale per tutte
    m_ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect( m_ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(setCurrentChanged()));
}

SectionModelGUI::~SectionModelGUI(){
    delete m_ui;
}

void SectionModelGUI::insertSection( SectionSpace::SectionType type ){
    QModelIndexList rowListSelected = m_ui->tableView->selectionModel()->selectedRows();
    QList<int> rowList;
    for( int i=0; i < rowListSelected.size(); i++ ){
        if( !rowList.contains(rowListSelected.at(i).row()) ){
            rowList.append( rowListSelected.at(i).row() );
        }
    }
    qSort( rowList.begin(), rowList.end() );

    if( rowList.size() > 0 ){
        m_model->insertRows(type, rowList.first(), rowList.size() );
    } else {
        int row = m_ui->tableView->currentIndex().row();
        if( row < 0 || row > m_model->count() )
            row = m_model->count() - 1 ;
        m_model->insertRows( type, row + 1, 1 );
    }
}

void SectionModelGUI::insertSectionGeneric(){
    insertSection( SectionSpace::GenericSection );
}

void SectionModelGUI::insertSectionProfile(){
    insertSection( SectionSpace::ProfileSection );
}

void SectionModelGUI::insertSectionPoints(){
    insertSection( SectionSpace::PointsSection );
}

void SectionModelGUI::insertSectionRCncr(){
    insertSection( SectionSpace::RCncrSection );
}

void SectionModelGUI::insertSectionSteelH(){
    insertSection( SectionSpace::SteelSection );
}

void SectionModelGUI::insertSectionTimber(){
    insertSection( SectionSpace::TimberSection );
}

void SectionModelGUI::insertSectionThin(){
    insertSection( SectionSpace::ThinSection );
}

void SectionModelGUI::insertSectionThinCont(){
    insertSection( SectionSpace::ThinSectionCont );
}

void SectionModelGUI::insertSectionThinContSteel(){
    insertSection( SectionSpace::ThinSectionContSteel );
}

void SectionModelGUI::remove(){
    QModelIndexList rowListSelected = m_ui->tableView->selectionModel()->selectedRows();
    QList<int> rowList;
    for( int i=0; i < rowListSelected.size(); i++ ){
        if( !rowList.contains(rowListSelected.at(i).row()) ){
            rowList.append( rowListSelected.at(i).row() );
        }
    }
    qSort( rowList.begin(), rowList.end() );

    for( int i=(rowList.size()-1); i >= 0; i-- ){
        int row = rowList.at(i);
        if( row >= 0 && row < m_model->count() ) {
            if( m_model->section( row )->isUsed() ){
                QString title = trUtf8("Sezione referenziata");
                QString message = trUtf8("Non si può rimuovere %1 perché è usata da una o più travi.").arg( m_model->section(row)->name->valueStr() );
                QMessageBox::critical(0, title, message, QMessageBox::Ok);
            } else {
                m_model->removeRows( row );
            }
        }
    }
}

void SectionModelGUI::setCurrentChanged() {
    if( m_ui->tableView->currentIndex().isValid() ){
        int row = m_ui->tableView->currentIndex().row();
        emit currentChanged( m_model->section(row) );
    }
}
