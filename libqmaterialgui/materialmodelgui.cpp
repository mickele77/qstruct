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
#include "materialmodelgui.h"
#include "ui_materialmodelgui.h"

#include <QMessageBox>
#include <QMenu>
#include <QAction>

#include "qstringplus.h"
#include "material.h"

class MaterialModelGUIPrivate {
    Q_DECLARE_TR_FUNCTIONS(MaterialModelGUIPrivate)
public:
    MaterialModelGUIPrivate(MaterialModel * m):
        ui(new Ui::MaterialModelGUI),
        model(m){

    }
    ~MaterialModelGUIPrivate(){
        delete ui;
        for( QMap<QAction *, MaterialModel::MaterialType>::iterator i = matActions.begin(); i != matActions.end(); ++i ){
            delete i.key();
        }
    }
    void createButtons( MaterialModelGUI * parent ) {
        // Connects buttons
        QMenu * newMaterialMenu = new QMenu(parent);
        QAction * cncrAct = newMaterialMenu->addAction( trUtf8("Calcestruzzo"));
        matActions.insert( cncrAct, MaterialModel::ConcreteMaterial );
        QAction * steelCncrAct = newMaterialMenu->addAction( trUtf8("Acciaio per c.a."));
        matActions.insert( steelCncrAct, MaterialModel::SteelCncrMaterial );
        QAction * frpAct = newMaterialMenu->addAction( trUtf8("FRP"));
        matActions.insert( frpAct, MaterialModel::FRPMaterial );
        QAction * steelAct = newMaterialMenu->addAction( trUtf8("Acciaio"));
        matActions.insert( steelAct, MaterialModel::SteelMaterial );
        QAction * timberAct = newMaterialMenu->addAction( trUtf8("Legno"));
        matActions.insert( timberAct, MaterialModel::TimberMaterial );
        QAction * matAct = newMaterialMenu->addAction( trUtf8("Generico"));
        matActions.insert( matAct, MaterialModel::GenericMaterial );
        ui->newPButton->setMenu( newMaterialMenu );
    }
    void insertMaterial( QAction * act ){
        if( matActions.contains(act)){
            MaterialModel::MaterialType type = matActions.value(act);
            QModelIndexList rowListSelected = ui->tableView->selectionModel()->selectedRows();
            QList<int> rowList;
            for( int i=0; i < rowListSelected.size(); i++ ){
                if( !rowList.contains(rowListSelected.at(i).row()) ){
                    rowList.append( rowListSelected.at(i).row() );
                }
            }
            qSort( rowList.begin(), rowList.end() );

            if( rowList.size() > 0 ){
                model->insertRows( type, rowList.first(), rowList.size());
            } else {
                int row = ui->tableView->currentIndex().row();
                if( row < 0 || row > model->count() )
                    row = model->count() - 1 ;
                model->insertRows( type, row + 1 );
            }
        }
    }

    Ui::MaterialModelGUI * ui;
    MaterialModel * model;
    QMap<QAction *, MaterialModel::MaterialType> matActions;
};

MaterialModelGUI::MaterialModelGUI(MaterialModel * m, QWidget *parent) :
    QWidget(parent),
    m_d( new MaterialModelGUIPrivate(m) ) {
    // setup UI
    m_d->ui->setupUi(this);

    // Assignes model to tableView
    m_d->ui->tableView->setModel( m_d->model );

    // Connects buttons
    m_d->createButtons(this);
    QList<QAction *> keys = m_d->matActions.keys();
    for( QList<QAction *>::iterator i = keys.begin(); i != keys.end(); ++i ){
        connect( *i, &QAction::triggered, this, &MaterialModelGUI::insertMaterial );
    }

    connect( m_d->ui->delPButton, SIGNAL(clicked()), this, SLOT( removeMaterials()) );

    // Rifinisce l'aspetto di tableView
    m_d->ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_d->ui->tableView->hideColumn(0);

    connect( m_d->ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(setCurrentChanged()) );
}

MaterialModelGUI::~MaterialModelGUI(){
    delete m_d;
}

void MaterialModelGUI::insertMaterial(){
    m_d->insertMaterial( dynamic_cast<QAction *>(sender()) );
}

void MaterialModelGUI::removeMaterials(){
    QModelIndexList rowListSelected = m_d->ui->tableView->selectionModel()->selectedRows();
    QList<int> rowList;
    for( int i=0; i < rowListSelected.size(); i++ ){
        if( !rowList.contains(rowListSelected.at(i).row()) ){
            rowList.append( rowListSelected.at(i).row() );
        }
    }
    qSort( rowList.begin(), rowList.end() );

    for( int i=(rowList.size()-1); i >= 0; i-- ){
        int row = rowList.at(i);
        if( row >= 0 && row < m_d->model->count() ) {
            if( m_d->model->material( row )->isUsed() ){
                QString title = trUtf8("Materiale referenziato");
                QString message = trUtf8("Non si può rimuovere %1 perché è usato da una o più sezioni.").arg( m_d->model->material(row)->name->valueStr() );
                QMessageBox::critical(0, title, message, QMessageBox::Ok);
            } else {
                m_d->model->removeRows( row );
            }
        }
    }
}

void MaterialModelGUI::setCurrentChanged(){
    int row = m_d->ui->tableView->currentIndex().row();
    emit currentChanged( m_d->model->material( row ) );
}
