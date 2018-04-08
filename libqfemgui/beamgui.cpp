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
#include "beamgui.h"

#include "beamfdelegate.h"
#include "beamqdelegate.h"
#include "beamfmodel.h"
#include "beamqmodel.h"
#include "beam.h"
#include "section.h"

#include "sectionmodel.h"
#include "elementvertexmodel.h"
#include "elementvertexdelegate.h"
#include "beamstressgui.h"
#include "beamdeformationgui.h"

#include "qstringplus.h"
#include "uintplus.h"

#include "varplusguiutility.h"
#include "qlabelrichname.h"
#include "qcomboboxenum.h"

#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>
#include <QGroupBox>
#include <QTabWidget>
#include <QPushButton>
#include <QWidget>

class BeamGUIPrivate{
public:
    BeamGUIPrivate( VertexModel * vm, SectionModel * sm, Hypothesis * hyp, QWidget * parent ):
        beam( NULL ),
        vertexModel(vm),
        sectionModel( sm ),
        globalLayout( new QGridLayout() ),
        beamPropertiesGBox ( new QGroupBox(parent) ),
        beamPropertiesGBoxLayout( new QGridLayout() ),
        sectionLabel( new QLabel( QObject::trUtf8("Sezione"), parent ) ),
        sectionCB( new QComboBox( parent ) ),
        beamTypeLabel( new QLabelRichName( parent ) ),
        beamTypeCB( new QComboBoxEnum( parent ) ),
        vertexView(  new QTableView( parent ) ),
        loadTab( new QWidget(parent) ),
        addFLoadPButton( new QPushButton( QObject::trUtf8("+"), parent)),
        removeFLoadPButton( new QPushButton( QObject::trUtf8("-"), parent)),
        fLoadView(  new QTableView( parent ) ),
        qLoadTab( new QWidget(parent) ),
        addQLoadPButton( new QPushButton( QObject::trUtf8("+"), parent)),
        removeQLoadPButton( new QPushButton( QObject::trUtf8("-"), parent)),
        qLoadView(  new QTableView( parent ) ),
        stressTab( new QWidget(parent)),
        deformationTab( new QWidget(parent)),
        tab ( new QTabWidget(parent) ) {

        beamPropertiesGBox->setTitle( QObject::trUtf8("Proprietà") );
        beamPropertiesGBox->setLayout( beamPropertiesGBoxLayout );
        beamPropertiesGBoxLayout->addWidget( beamTypeLabel, 0,0 );
        beamPropertiesGBoxLayout->addWidget( beamTypeCB, 0,1 );
        beamPropertiesGBoxLayout->addWidget( sectionLabel, 1,0 );
        beamPropertiesGBoxLayout->addWidget( sectionCB, 1,1 );
        globalLayout->addWidget( beamPropertiesGBox, 0, 0 );

        tab->addTab( vertexView, QObject::trUtf8("Vertici") );
        vertexView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QGridLayout * loadTabLayout = new QGridLayout;

        QGroupBox * qLoadGBox = new QGroupBox();
        loadTabLayout->addWidget( qLoadGBox, 0,0 );
        QGridLayout * qLoadGBoxLayout = new QGridLayout;
        qLoadGBox->setTitle( QObject::trUtf8("Distribuiti"));
        qLoadGBoxLayout->addWidget( addQLoadPButton, 0, 0);
        qLoadGBoxLayout->addWidget( removeQLoadPButton, 0, 1);
        qLoadGBoxLayout->addWidget( qLoadView, 1, 0, 1, 2);
        qLoadGBox->setLayout( qLoadGBoxLayout );

        qLoadView->setItemDelegate( new BeamQDelegate( hyp, qLoadView ));
        qLoadView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

        QGroupBox * fLoadGBox = new QGroupBox;
        loadTabLayout->addWidget(fLoadGBox, 1,0 );
        QGridLayout * fLoadGBoxLayout = new QGridLayout;
        fLoadGBox->setTitle( QObject::trUtf8("Concentrati"));
        fLoadGBoxLayout->addWidget( addFLoadPButton, 0, 0);
        fLoadGBoxLayout->addWidget( removeFLoadPButton, 0, 1);
        fLoadGBoxLayout->addWidget( fLoadView, 1, 0, 1, 2);
        fLoadGBox->setLayout( fLoadGBoxLayout );

        fLoadView->setItemDelegate( new BeamFDelegate( hyp, fLoadView ));
        fLoadView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

        loadTab->setLayout( loadTabLayout  );
        tab->addTab( loadTab, QObject::trUtf8("Carichi") );

        QGridLayout * stressTabLayout = new QGridLayout;
        stressGUI = new BeamStressGUI( stressTab );
        stressTabLayout->addWidget( stressGUI, 0, 0 );
        tab->addTab( stressTab, QObject::trUtf8("Sollecitazioni interne") );
        stressTab->setLayout( stressTabLayout );

        QGridLayout * deformationTabLayout = new QGridLayout;
        deformationGUI = new BeamDeformationGUI( deformationTab );
        deformationTabLayout->addWidget( deformationGUI, 0, 0 );
        tab->addTab( deformationTab, QObject::trUtf8("Deformazioni") );
        deformationTab->setLayout( deformationTabLayout );

        globalLayout->addWidget( tab, 1, 0 );
    };
    Beam * beam;
    VertexModel * vertexModel;
    SectionModel * sectionModel;
    QGridLayout * globalLayout;
    QGroupBox * beamPropertiesGBox;
    QGridLayout * beamPropertiesGBoxLayout;
    QLabel * sectionLabel;
    QComboBox * sectionCB;
    QLabelRichName * beamTypeLabel;
    QComboBoxEnum * beamTypeCB;
    QTableView * vertexView;

    QWidget * loadTab;
    QPushButton * addFLoadPButton;
    QPushButton * removeFLoadPButton;
    QTableView * fLoadView;

    QWidget * qLoadTab;
    QPushButton * addQLoadPButton;
    QPushButton * removeQLoadPButton;
    QTableView * qLoadView;

    QWidget * stressTab;
    BeamStressGUI * stressGUI;

    QWidget * deformationTab;
    BeamDeformationGUI * deformationGUI;

    QTabWidget * tab;
};

BeamGUI::BeamGUI(Beam * b, VertexModel * vm, SectionModel * sm, QWidget *parent) :
    ElementGUI( parent),
    m_d( new BeamGUIPrivate( vm, sm, b->hypothesis(), this ) ){

    setLayout( m_d->globalLayout );

    setElement(b);
    updateData();

    connect( m_d->sectionCB, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentSection(int)));

    connect( m_d->removeFLoadPButton, SIGNAL(clicked()), this, SLOT(removeFLoad()) );
    connect( m_d->addFLoadPButton, SIGNAL(clicked()), this, SLOT(addFLoad()) );

    connect( m_d->removeQLoadPButton, SIGNAL(clicked()), this, SLOT(removeQLoad()) );
    connect( m_d->addQLoadPButton, SIGNAL(clicked()), this, SLOT(addQLoad()) );
}

QString BeamGUI::typeElement(){
    return QString("beam");
}

void BeamGUI::setElement( Element * b ){
    if( b->typeElement().toUpper() == "BEAM"){
        m_d->beam = (Beam *) b;

        m_d->beam->createVertexModel( m_d->vertexModel );
        m_d->vertexView->setModel( m_d->beam->vertexModel() );
        QAbstractItemDelegate * oldDelegate = m_d->vertexView->itemDelegate();
        m_d->vertexView->setItemDelegate( new ElementVertexDelegate( m_d->vertexModel, m_d->beam->vertexModel()) );
        delete oldDelegate;

        VarPlusGUI::connectEnumVar( m_d->beam->bType,  m_d->beamTypeLabel, m_d->beamTypeCB );

        m_d->fLoadView->setModel( m_d->beam->fAppModel() );

        m_d->qLoadView->setModel( m_d->beam->qAppModel() );

        m_d->stressGUI->setBeam( m_d->beam );
        m_d->deformationGUI->setBeam( m_d->beam );
    }
}

void BeamGUI::updateData(){
    updateSectionCB();
}

void BeamGUI::updateResults(){
    m_d->stressGUI->updateData();
    m_d->deformationGUI->updateData();
}

void BeamGUI::updateSectionCB(){
    QList<Section *> sectList = m_d->sectionModel->sectionsList( SectionSpace::GenericSection );

    m_d->sectionCB->clear();

    for( QList<Section *>::iterator i=sectList.begin(); i!=sectList.end(); ++i ){
        m_d->sectionCB->addItem( (*i)->name->valueStr(), QVariant((*i)->id->valueNormal()) );
    }
    if( m_d->beam ){
        if( m_d->beam->section() ){
            m_d->sectionCB->setCurrentIndex( m_d->sectionCB->findData( QVariant( m_d->beam->section()->id->valueNormal())) );
        }
    }
}

void BeamGUI::setSection( int ns ){
    m_d->beam->setSection( m_d->sectionModel->section(ns) );
}

void BeamGUI::addFLoad(){
    QModelIndexList listRows = m_d->fLoadView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->beam->fAppModel()->insert( listRows.at(0).row() , listRows.size() );
    } else {
        m_d->beam->fAppModel()->append();
    }
}

void BeamGUI::removeFLoad(){
    QModelIndexList listRows = m_d->fLoadView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->beam->fAppModel()->remove( listRows.at(0).row(), listRows.size() );
    }
}

void BeamGUI::addQLoad(){
    QModelIndexList listRows = m_d->qLoadView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->beam->qAppModel()->insert( listRows.at(0).row() , listRows.size() );
    } else {
        m_d->beam->qAppModel()->append();
    }
}

void BeamGUI::removeQLoad(){
    QModelIndexList listRows = m_d->qLoadView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->beam->qAppModel()->remove( listRows.at(0).row(), listRows.size() );
    }
}

void BeamGUI::showEvent ( QShowEvent *){
    updateData();
    connect( m_d->sectionCB, SIGNAL(currentIndexChanged(int)), this,SLOT(setSection(int)));
}

void BeamGUI::hideEvent ( QHideEvent *){
    disconnect( m_d->sectionCB, SIGNAL(currentIndexChanged(int)), this,SLOT(setSection(int)));
}
