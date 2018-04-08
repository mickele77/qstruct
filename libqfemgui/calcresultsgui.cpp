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
#include "calcresultsgui.h"

#include "ui_calcresultsgui.h"

#include "femstructure.h"
#include "beam.h"
#include "hypothesis.h"

class CalcResultsGUIPrivate{
public:
    CalcResultsGUIPrivate( FEMStructure * str ){
        ui = new Ui::CalcResultsGUI;
        structure = str;
        beamFPLocalScale = NULL;
    };
    ~CalcResultsGUIPrivate(){
        delete ui;
        if( beamFPLocalScale ){
            delete [] beamFPLocalScale;
        }
    };
    void connectBeamFPLocalTypeComboBox(){
        QVector<QString> FPNames = Beam::FPLocalNames( structure->hypothesis() );
        for( QVector<QString>::iterator iter = FPNames.begin(); iter != FPNames.end(); ++iter){
            ui->beamFPLocalTypeComboBox->addItem( *iter );
        }
        beamFPLocalScale = new double[structure->hypothesis()->nDOFVert() ];
        // scala per forze
        for( int i=0; i < structure->hypothesis()->spaceDim(); i++  ){
            beamFPLocalScale[i] = 1.0;
        }
        // scala per coppie
        for( int i=structure->hypothesis()->spaceDim(); i < structure->hypothesis()->nDOFVert(); i++  ){
            beamFPLocalScale[i] = 1.0;
        }

        currentBeamFPLocalScale = 0;
        ui->beamFPLocalTypeComboBox->setCurrentIndex(currentBeamFPLocalScale);
        ui->beamFPLocalScaleDSBox->setValue( beamFPLocalScale[currentBeamFPLocalScale] );
    }

    Ui::CalcResultsGUI *ui;

    FEMStructure * structure;

    double * beamFPLocalScale;
    int currentBeamFPLocalScale;
};

CalcResultsGUI::CalcResultsGUI( FEMStructure * str, QWidget *parent) :
    QWidget(parent),
    m_d( new CalcResultsGUIPrivate( str ) ) {
    m_d->ui->setupUi(this);
    m_d->ui->deformedScaleDSBox->setValue( 20.0 );
    connect( m_d->ui->calculatePButton, SIGNAL(clicked()), this, SLOT(calculate()));

    connect( m_d->ui->deformedPButton, SIGNAL(clicked()), this, SLOT(addGLItemDeformed()));

    m_d->connectBeamFPLocalTypeComboBox();
    connect( m_d->ui->beamFPLocalTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setFPLocalScale(int)) );
    connect( m_d->ui->beamFPLocalPButton, SIGNAL(clicked()), this, SLOT(addGLItemBeamFPLocal()) );

    connect( m_d->ui->clearPostProPButton, SIGNAL(clicked()), this, SLOT(clearGLItem()) );
}

CalcResultsGUI::~CalcResultsGUI() {
    delete m_d;
}

void CalcResultsGUI::calculate() {
    m_d->structure->setProblem();
    m_d->structure->solveProblem();

    emit resultsChanged();
}

void CalcResultsGUI::addGLItemDeformed(){
    emit addGLItem( m_d->structure->glItemDeformed( m_d->ui->deformedScaleDSBox->value() ) );
}

void CalcResultsGUI::addGLItemBeamFPLocal(){
    emit addGLItem( m_d->structure->glItemBeamFPLocal( m_d->ui->beamFPLocalTypeComboBox->currentIndex(), m_d->ui->beamFPLocalScaleDSBox->value() ) );
}

void CalcResultsGUI::clearGLItem(){
    emit addGLItem( NULL );
}

void CalcResultsGUI::setFPLocalScale( int i ){
    if( i != m_d->currentBeamFPLocalScale) {
        m_d->beamFPLocalScale[m_d->currentBeamFPLocalScale] = m_d->ui->beamFPLocalScaleDSBox->value();
        m_d->currentBeamFPLocalScale = i;
        m_d->ui->beamFPLocalScaleDSBox->setValue( m_d->beamFPLocalScale[m_d->currentBeamFPLocalScale] );
    }
}
