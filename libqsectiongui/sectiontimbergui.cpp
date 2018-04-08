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
#include "sectiontimbergui.h"
#include "ui_sectiontimbergui.h"

#include "varplusguiutility.h"
#include "sectiontimber.h"
#include "sectiontimberdatasetmodel.h"
#include "materialmodel.h"
#include "ssbeamtimber.h"
#include "loadcompltabledelegate.h"
#include "loadcompltablemodel.h"

#include "timber.h"
#include "material.h"

#include "point2dplus.h"
#include "doubleplus.h"
#include "qstringplus.h"
#include "uintplus.h"

#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QFileDialog>
#include <QMessageBox>

class SectionTimberGUIPrivate{
public:
    SectionTimberGUIPrivate():
        ui(new Ui::SectionTimberGUI),
        section(NULL){
    };
    ~SectionTimberGUIPrivate(){
        delete ui;
    };
    Ui::SectionTimberGUI * ui;
    SectionTimber * section;
    SectionTimberDataSetModel * dataSetModel;

    DoublePlus * beamL0y;
    DoublePlus * beamL0z;
    DoublePlus * beamL0LT;

    LSType * lsType;
    ServiceClass * serviceClass;
    LoadDurationClass * loadDurationClass;

    DoublePlus * NULSMinus;
    DoublePlus * NULSPlus;
    DoublePlus * VULS;
    DoublePlus * MULSX;
    DoublePlus * MULSY;
    DoublePlus * MULSZ;

    DoublePlus * beamlY;
    DoublePlus * beamlRelY;
    DoublePlus * kBucklY;
    DoublePlus * kCrBucklY;
    DoublePlus * NULSBucklY;
    DoublePlus * beamlZ;
    DoublePlus * beamlRelZ;
    DoublePlus * kBucklZ;
    DoublePlus * kCrBucklZ;
    DoublePlus * NULSBucklZ;
    DoublePlus * beamlRelLTY;
    DoublePlus * kCrBucklLTY;
    DoublePlus * MULSBucklLTY;
    DoublePlus * beamlRelLTZ;
    DoublePlus * kCrBucklLTZ;
    DoublePlus * MULSBucklLTZ;

    DoublePlus * NSd;
    DoublePlus * VySd;
    DoublePlus * VzSd;
    DoublePlus * VSd;
    DoublePlus * MxSd;
    Point2DPlus * cenMSd;
    DoublePlus * MySd;
    DoublePlus * MzSd;
    DoublePlus * resultSd;

    LSType * dataSetLSType;
    ServiceClass * dataSetServiceClass;
    LoadDurationClass * dataSetLoadDurationClass;

    LoadComplTableModel * SSBeamLoadModel;
    SSBeamTimber * ssBeamTimber;
};

SectionTimberGUI::SectionTimberGUI(UnitMeasure * ump, MaterialModel * matModel, QWidget *parent) :
    SectionGUI( ump, matModel, parent ),
    m_d( new SectionTimberGUIPrivate() ){

    m_d->ui->setupUi(this);

    initVar();

    m_materialComboBox = m_d->ui->timberComboBox;
    connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    refreshMaterialComboBox();
}

SectionTimberGUI::~SectionTimberGUI() {
    delete m_d;
}

void SectionTimberGUI::initVar(){
    m_d->beamL0y = new DoublePlus(0.0, "beamL0y", m_unitMeasure, UnitMeasure::length, false );
    m_d->beamL0y->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">0,y</span>") );
    VarPlusGUI::connectVar( m_d->beamL0y, m_d->ui->beamL0yLabel, m_d->ui->beamL0yLEdit, m_d->ui->beamL0yUMLabel );

    m_d->beamL0z = new DoublePlus(0.0, "beamL0z", m_unitMeasure, UnitMeasure::length );
    m_d->beamL0z->setReadOnly( true );
    m_d->beamL0z->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">0,z</span>") );
    VarPlusGUI::connectVar( m_d->beamL0z, m_d->ui->beamL0zLabel, m_d->ui->beamL0zLEdit, m_d->ui->beamL0zUMLabel, m_d->ui->beamL0ZCheckBox );
    connect( m_d->beamL0z, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamL0z()) );
    connect( m_d->beamL0y, SIGNAL(valueChanged(QString)), this, SLOT(setBeamL0z()) );

    m_d->beamL0LT = new DoublePlus(0.0, "beamL0LT", m_unitMeasure, UnitMeasure::length );
    m_d->beamL0LT->setReadOnly( true );
    m_d->beamL0LT->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">0,LT</span>") );
    VarPlusGUI::connectVar( m_d->beamL0LT, m_d->ui->beamL0LTLabel, m_d->ui->beamL0LTLEdit, m_d->ui->beamL0LTUMLabel, m_d->ui->beamL0LTCheckBox );
    connect( m_d->beamL0LT, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamL0LT()) );
    connect( m_d->beamL0y, SIGNAL(valueChanged(QString)), this, SLOT(setBeamL0LT()) );

    m_d->lsType = new LSType( LSType::ULSFundamental, "lsType" );
    m_d->lsType->setRichName( trUtf8("Combinazione"));
    VarPlusGUI::connectEnumVar( m_d->lsType, m_d->ui->LSTypeLabel, m_d->ui->LSTypeComboBox );

    m_d->serviceClass = new ServiceClass(ServiceClass::SC1, "serviceClass" );
    m_d->serviceClass->setRichName( trUtf8("Classe di servizio"));
    VarPlusGUI::connectEnumVar( m_d->serviceClass, m_d->ui->serviceClassLabel, m_d->ui->serviceClassComboBox );
    connect( m_d->serviceClass, SIGNAL(valueChanged(QString)), this, SLOT(setRd()) );

    m_d->loadDurationClass = new LoadDurationClass( LoadDurationClass::permanent, "loadDurationClass" );
    m_d->loadDurationClass->setRichName(trUtf8("Classe di durata"));
    VarPlusGUI::connectEnumVar( m_d->loadDurationClass,  m_d->ui->durationClassLabel, m_d->ui->durationClassComboBox);
    connect( m_d->loadDurationClass, SIGNAL(valueChanged(QString)), this, SLOT(setRd()) );

    m_d->NULSMinus = new DoublePlus(0.0, "NULSMinus", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->NULSMinus->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">c,Rd</span>"));
    VarPlusGUI::connectVar( m_d->NULSMinus, m_d->ui->NULSMinusLabel, m_d->ui->NULSMinusLEdit, m_d->ui->NULSMinusUMLabel);

    m_d->NULSPlus = new DoublePlus(0.0, "NULSPlus", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->NULSPlus->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">t,Rd</span>"));
    m_d->NULSPlus->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->NULSPlus, m_d->ui->NULSPlusLabel, m_d->ui->NULSPlusLEdit, m_d->ui->NULSPlusUMLabel);

    m_d->VULS = new DoublePlus(0.0, "VULS", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->VULS->setRichName( trUtf8("V<span style=\" vertical-align:sub;\">Rd</span>"));
    VarPlusGUI::connectVar( m_d->VULS, m_d->ui->VULSLabel, m_d->ui->VULSLEdit, m_d->ui->VULSUMLabel);

    m_d->MULSX = new DoublePlus(0.0, "MULSX", m_unitMeasure, UnitMeasure::loadM, true );
    m_d->MULSX->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">T,Rd</span>"));
    VarPlusGUI::connectVar( m_d->MULSX, m_d->ui->MULSXLabel, m_d->ui->MULSXLEdit, m_d->ui->MULSXUMLabel );

    m_d->MULSY = new DoublePlus(0.0, "MULSY", m_unitMeasure, UnitMeasure::loadM, true );
    m_d->MULSY->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">y,Rd</span>"));
    VarPlusGUI::connectVar( m_d->MULSY, m_d->ui->MULSYLabel, m_d->ui->MULSYLEdit, m_d->ui->MULSYUMLabel );

    m_d->MULSZ = new DoublePlus(0.0, "MULSZ", m_unitMeasure, UnitMeasure::loadM, true );
    m_d->MULSZ->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">z,Rd</span>"));
    VarPlusGUI::connectVar( m_d->MULSZ, m_d->ui->MULSZLabel, m_d->ui->MULSZLEdit, m_d->ui->MULSZUMLabel );

    m_d->beamlY = new DoublePlus(0.0, "beamlY", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->beamlY->setRichName( trUtf8("λ<span style=\" vertical-align:sub;\">y</span>"));
    m_d->beamlY->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->beamlY, m_d->ui->beamlYLabel, m_d->ui->beamlYLEdit, m_d->ui->beamlYUMLabel,  m_d->ui->beamlYCheckBox );
    connect( m_d->beamlY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamlY()));
    connect( m_d->beamL0y, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlY()));

    m_d->beamlRelY = new DoublePlus(0.0, "beamlRelY", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->beamlRelY->setRichName( trUtf8("<span style=\" text-decoration: overline;\">λ</span><span style=\" vertical-align:sub;\">y</span>"));
    m_d->beamlRelY->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->beamlRelY, m_d->ui->beamlRelYLabel, m_d->ui->beamlRelYLEdit, m_d->ui->beamlRelYUMLabel, m_d->ui->beamlRelYCheckBox);
    connect( m_d->beamlRelY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamlRelY()) );
    connect( m_d->beamlY, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlRelY()));

    m_d->kBucklY = new DoublePlus(1.0, "kBucklY", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->kBucklY->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">y</span>"));
    m_d->kBucklY->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->kBucklY, m_d->ui->beamkYLabel,  m_d->ui->beamkYLEdit, m_d->ui->beamkYUMLabel, m_d->ui->beamkYCheckBox );
    connect( m_d->kBucklY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setKBucklY()));
    connect( m_d->beamlRelY, SIGNAL(valueChanged(QString)), this, SLOT(setKBucklY()));

    m_d->kCrBucklY = new DoublePlus(1.0, "kCrBucklY", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->kCrBucklY->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">cr,y</span>"));
    m_d->kCrBucklY->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->kCrBucklY, m_d->ui->beamkCritYLabel, m_d->ui->beamkCritYLEdit, m_d->ui->beamkCritYUMLabel, m_d->ui->beamkCritYCheckBox );
    connect( m_d->kCrBucklY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setKCrBucklY()));
    connect( m_d->kBucklY, SIGNAL(valueChanged(QString)), this, SLOT(setKCrBucklY()));
    connect( m_d->beamlRelY, SIGNAL(valueChanged(QString)), this, SLOT(setKCrBucklY()));

    m_d->NULSBucklY = new DoublePlus(0.0, "NULSBucklY", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->NULSBucklY->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">b,Rd,y</span>"));
    m_d->NULSBucklY->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->NULSBucklY, m_d->ui->beamNULSBucklYLabel, m_d->ui->beamNULSBucklYLEdit, m_d->ui->beamNULSBucklYUMLabel );
    connect( m_d->NULSBucklY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setNULSBucklY()) );
    connect( m_d->kCrBucklY, SIGNAL(valueChanged(QString)), this, SLOT(setNULSBucklY()) );

    m_d->beamlZ  = new DoublePlus(0.0, "beamlZ", m_unitMeasure, UnitMeasure::noDimension );
    m_d->beamlZ->setRichName( trUtf8("λ<span style=\" vertical-align:sub;\">z</span>"));
    m_d->beamlZ->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->beamlZ, m_d->ui->beamlZLabel, m_d->ui->beamlZLEdit, m_d->ui->beamlZUMLabel, m_d->ui->beamlZCheckBox);
    connect( m_d->beamlZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamlZ()) );
    connect( m_d->beamL0z, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlZ()));

    m_d->beamlRelZ  = new DoublePlus(0.0, "beamlRelZ", m_unitMeasure, UnitMeasure::noDimension );
    m_d->beamlRelZ->setRichName( trUtf8("<span style=\" text-decoration: overline;\">λ</span><span style=\" vertical-align:sub;\">z</span>"));
    m_d->beamlRelZ->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->beamlRelZ, m_d->ui->beamlRelZLabel, m_d->ui->beamlRelZLEdit, m_d->ui->beamlRelZUMLabel, m_d->ui->beamlRelZCheckBox );
    connect( m_d->beamlRelZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamlRelZ()) );
    connect( m_d->beamlZ, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlRelZ()));

    m_d->kBucklZ = new DoublePlus(1.0, "kBucklZ", m_unitMeasure, UnitMeasure::noDimension );
    m_d->kBucklZ->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">z</span>"));
    m_d->kBucklZ->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->kBucklZ, m_d->ui->beamkZLabel, m_d->ui->beamkZLEdit, m_d->ui->beamkZUMLabel,m_d->ui->beamkZCheckBox );
    connect( m_d->kBucklZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setKBucklZ()));
    connect( m_d->beamlRelZ, SIGNAL(valueChanged(QString)), this, SLOT(setKBucklZ()));

    m_d->kCrBucklZ = new DoublePlus(1.0, "kCrBucklZ", m_unitMeasure, UnitMeasure::noDimension );
    m_d->kCrBucklZ->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">cr,z</span>"));
    m_d->kCrBucklZ->setReadOnly( true );
    VarPlusGUI::connectVar(m_d->kCrBucklZ, m_d->ui->beamkCritZLabel, m_d->ui->beamkCritZLEdit,  m_d->ui->beamkCritZUMLabel, m_d->ui->beamkCritZCheckBox);
    connect( m_d->kCrBucklZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setKCrBucklZ()));
    connect( m_d->kBucklZ, SIGNAL(valueChanged(QString)), this, SLOT(setKCrBucklZ()));
    connect( m_d->beamlRelZ, SIGNAL(valueChanged(QString)), this, SLOT(setKCrBucklZ()));

    m_d->NULSBucklZ = new DoublePlus(0.0, "NULSBucklZ", m_unitMeasure, UnitMeasure::loadF );
    m_d->NULSBucklZ->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">b,Rd,z</span>"));
    m_d->NULSBucklZ->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->NULSBucklZ, m_d->ui->beamNULSBucklZLabel, m_d->ui->beamNULSBucklZLEdit, m_d->ui->beamNULSBucklZUMLabel);
    connect( m_d->kCrBucklZ, SIGNAL(valueChanged(QString)), this, SLOT(setNULSBucklZ()) );

    m_d->beamlRelLTY  = new DoublePlus(0.0, "beamlRelLTY", m_unitMeasure, UnitMeasure::noDimension );
    m_d->beamlRelLTY->setRichName( trUtf8("<span style=\" text-decoration: overline;\">λ</span><span style=\" vertical-align:sub;\">LT,y</span>"));
    m_d->beamlRelLTY->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->beamlRelLTY, m_d->ui->beamlRelLTYLabel, m_d->ui->beamlRelLTYLEdit, m_d->ui->beamlRelLTYUMLabel, m_d->ui->beamlRelLTYCheckBox);
    connect( m_d->beamlRelLTY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamlRelLTY()));
    connect( m_d->beamL0LT, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlRelLTY()));

    m_d->kCrBucklLTY = new DoublePlus(1.0, "kCrBucklLTY", m_unitMeasure, UnitMeasure::noDimension );
    m_d->kCrBucklLTY->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">cr,LT,y</span>"));
    m_d->kCrBucklLTY->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->kCrBucklLTY, m_d->ui->beamkCritLTYLabel, m_d->ui->beamkCritLTYLEdit, m_d->ui->beamkCritLTYUMLabel, m_d->ui->beamkCritLTYCheckBox);
    connect( m_d->kCrBucklLTY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setKCrBucklLTY()));
    connect( m_d->beamlRelLTY, SIGNAL(valueChanged(QString)), this, SLOT(setKCrBucklLTY()));

    m_d->MULSBucklLTY = new DoublePlus(0.0, "MULSBucklLTY", m_unitMeasure, UnitMeasure::loadM );
    m_d->MULSBucklLTY->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">b,Rd,LT,y</span>"));
    m_d->MULSBucklLTY->setReadOnly( true );
    VarPlusGUI::connectVar(m_d->MULSBucklLTY,  m_d->ui->beamMULSBucklLTYLabel, m_d->ui->beamMULSBucklLTYLEdit, m_d->ui->beamMULSBucklLTYUMLabel);
    connect( m_d->MULSBucklLTY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setMULSBucklLTY()));
    connect( m_d->kCrBucklLTY, SIGNAL(valueChanged(QString)), this, SLOT(setMULSBucklLTY()));

    m_d->beamlRelLTZ  = new DoublePlus(0.0, "beamlRelLTZ", m_unitMeasure, UnitMeasure::noDimension );
    m_d->beamlRelLTZ->setRichName( trUtf8("<span style=\" text-decoration: overline;\">λ</span><span style=\" vertical-align:sub;\">LT,z</span>"));
    m_d->beamlRelLTZ->setReadOnly( true );
    VarPlusGUI::connectVar(m_d->beamlRelLTZ, m_d->ui->beamlRelLTZLabel, m_d->ui->beamlRelLTZLEdit, m_d->ui->beamlRelLTZUMLabel, m_d->ui->beamlRelLTZCheckBox);
    connect( m_d->beamlRelLTZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamlRelLTZ()));
    connect( m_d->beamL0LT, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlRelLTZ()));

    m_d->kCrBucklLTZ = new DoublePlus(1.0, "kCrBucklLTZ", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->kCrBucklLTZ->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">cr,LT,z</span>"));
    m_d->kCrBucklLTZ->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->kCrBucklLTZ, m_d->ui->beamkCritLTZLabel, m_d->ui->beamkCritLTZLEdit, m_d->ui->beamkCritLTZUMLabel, m_d->ui->beamkCritLTZCheckBox);
    connect( m_d->kCrBucklLTZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setKCrBucklLTZ()));
    connect( m_d->beamlRelLTZ, SIGNAL(valueChanged(QString)), this, SLOT(setKCrBucklLTZ()));

    m_d->MULSBucklLTZ = new DoublePlus(0.0, "MULSBucklLTZ", m_unitMeasure, UnitMeasure::loadM, true );
    m_d->MULSBucklLTZ->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">b,Rd,LT,z</span>"));
    m_d->MULSBucklLTZ->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->MULSBucklLTZ, m_d->ui->beamMULSBucklLTZLabel, m_d->ui->beamMULSBucklLTZLEdit, m_d->ui->beamMULSBucklLTZUMLabel );
    connect( m_d->MULSBucklLTZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setMULSBucklLTZ()));
    connect( m_d->kCrBucklLTZ, SIGNAL(valueChanged(QString)), this, SLOT(setMULSBucklLTZ()));

    m_d->NSd = new DoublePlus(0.0, "NSd", m_unitMeasure, UnitMeasure::loadF );
    m_d->NSd->setRichName( trUtf8("N"));
    m_d->NSd->setReadOnly( false );
    VarPlusGUI::connectVar(m_d->NSd, m_d->ui->NSdLabel, m_d->ui->NSdLEdit, m_d->ui->NSdUMLabel);

    m_d->VySd = new DoublePlus(0.0, "VySd", m_unitMeasure, UnitMeasure::loadF );
    m_d->VySd->setReadOnly( false );
    m_d->VySd->setRichName( trUtf8("V<span style=\" vertical-align:sub;\">y</span>"));
    VarPlusGUI::connectVar( m_d->VySd, m_d->ui->VySdLabel, m_d->ui->VySdLEdit, m_d->ui->VySdUMLabel );

    m_d->VzSd = new DoublePlus(0.0, "VzSd", m_unitMeasure, UnitMeasure::loadF );
    m_d->VzSd->setReadOnly( false );
    m_d->VzSd->setRichName( trUtf8("V<span style=\" vertical-align:sub;\">z</span>"));
    VarPlusGUI::connectVar( m_d->VzSd, m_d->ui->VzSdLabel, m_d->ui->VzSdLEdit, m_d->ui->VzSdUMLabel );

    m_d->cenMSd = new Point2DPlus( "cenMSd", "yMSd", 0.0, "zMSd", 0.0, m_unitMeasure, UnitMeasure::sectL);
    m_d->cenMSd->y->setRichName( trUtf8("y<span style=\" vertical-align:sub;\">M</span>"));
    m_d->cenMSd->y->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->cenMSd->y, m_d->ui->yCenMSdLabel, m_d->ui->yCenMSdLEdit, m_d->ui->yCenMSdUMLabel, m_d->ui->yCenMSdCheckBox );
    m_d->cenMSd->z->setRichName( trUtf8("z<span style=\" vertical-align:sub;\">M</span>"));
    m_d->cenMSd->z->setReadOnly( true );
    VarPlusGUI::connectVar( m_d->cenMSd->z, m_d->ui->zCenMSdLabel, m_d->ui->zCenMSdLEdit, m_d->ui->zCenMSdUMLabel, m_d->ui->zCenMSdCheckBox );

    m_d->MxSd = new DoublePlus(0.0, "MxSd", m_unitMeasure, UnitMeasure::loadM );
    m_d->MxSd->setReadOnly( false );
    m_d->MxSd->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">T</span>"));
    VarPlusGUI::connectVar( m_d->MxSd, m_d->ui->MxSdLabel, m_d->ui->MxSdLEdit, m_d->ui->MxSdUMLabel );

    m_d->MySd = new DoublePlus(0.0, "MySd", m_unitMeasure, UnitMeasure::loadM );
    m_d->MySd->setReadOnly( false );
    m_d->MySd->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">y</span>"));
    VarPlusGUI::connectVar( m_d->MySd, m_d->ui->MySdLabel, m_d->ui->MySdLEdit, m_d->ui->MySdUMLabel );

    m_d->MzSd = new DoublePlus(0.0, "MzSd", m_unitMeasure, UnitMeasure::loadM );
    m_d->MzSd->setReadOnly( false );
    m_d->MzSd->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">z</span>"));
    VarPlusGUI::connectVar( m_d->MzSd, m_d->ui->MzSdLabel, m_d->ui->MzSdLEdit, m_d->ui->MzSdUMLabel );

    m_d->resultSd = new DoublePlus(0.0, "resultSd", m_unitMeasure, UnitMeasure::noDimension );
    m_d->resultSd->setRichName( trUtf8("Risultato"));
    m_d->resultSd->setReadOnly( false );
    VarPlusGUI::connectVar( m_d->resultSd, m_d->ui->resultSdLabel, m_d->ui->resultSdLEdit );

    connect( m_d->ui->ULSVerifyPButton, SIGNAL(clicked()), this, SLOT(verifyULS()));

    // DataSet

    m_d->dataSetLSType = new LSType( LSType::ULSFundamental, "dataSetLSType" );
    m_d->dataSetLSType->setRichName( trUtf8("Combinazione"));
    VarPlusGUI::connectEnumVar( m_d->dataSetLSType, m_d->ui->dataSetLSTypeLabel, m_d->ui->dataSetLSTypeComboBox );

    m_d->dataSetServiceClass = new ServiceClass(ServiceClass::SC1, "dataSetServiceClass" );
    m_d->dataSetServiceClass->setRichName( trUtf8("Classe di servizio"));
    VarPlusGUI::connectEnumVar( m_d->dataSetServiceClass, m_d->ui->dataSetServiceClassLabel, m_d->ui->dataSetServiceClassComboBox );

    m_d->dataSetLoadDurationClass = new LoadDurationClass( LoadDurationClass::permanent, "dataSetLoadDurationClass" );
    m_d->dataSetLoadDurationClass->setRichName(trUtf8("Classe di durata"));
    VarPlusGUI::connectEnumVar( m_d->dataSetLoadDurationClass, m_d->ui->dataSetDurationClassLabel, m_d->ui->dataSetDurationClassComboBox );

    m_d->dataSetModel = new SectionTimberDataSetModel( m_unitMeasure, m_d->dataSetLSType, m_d->dataSetServiceClass, m_d->dataSetLoadDurationClass, m_d->kCrBucklY, m_d->kCrBucklZ, m_d->kCrBucklLTY, m_d->kCrBucklLTZ );
    m_d->ui->dataSetTableView->setModel( m_d->dataSetModel );
    connect( m_d->ui->dataSetClearPButton, SIGNAL(clicked()), this, SLOT(dataSetClear()));
    connect( m_d->ui->dataSetLoadFromFilePButton, SIGNAL(clicked()), this, SLOT(dataSetLoadFromFile()));
    connect( m_d->ui->dataSetVerifyULSAllPButton, SIGNAL(clicked()), this, SLOT(dataSetVerifyULSAll()));
    connect( m_d->ui->dataSetFromFormToTablePButton, SIGNAL(clicked()), this, SLOT(dataSetFromGUIToTable()));
    connect( m_d->ui->dataSetFromTableToFormPButton, SIGNAL(clicked()), this, SLOT(dataSetFromTableToGUI()));

    // SSBeam
    m_d->SSBeamLoadModel = new LoadComplTableModel( m_unitMeasure );
    m_d->ui->SSBeamLoadTableView->setItemDelegate( new LoadComplTableDelegate(m_d->ui->SSBeamLoadTableView));
    m_d->ui->SSBeamLoadTableView->setModel( m_d->SSBeamLoadModel );
    connect( m_d->ui->SSBeamLoadAddPButton, SIGNAL(clicked()), this, SLOT(SSBeamAddLoad()));
    connect( m_d->ui->SSBeamLoadRemovePButton, SIGNAL(clicked()), this, SLOT(SSBeamRemoveLoad()));

    m_d->ssBeamTimber = new SSBeamTimber( m_unitMeasure, m_d->section );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->alpha, m_d->ui->SSBeamAlphaLabel, m_d->ui->SSBeamAlphaLEdit, m_d->ui->SSBeamAlphaUMLabel, m_d->ui->SSBeamAlphaCheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->L, m_d->ui->SSBeamLLabel, m_d->ui->SSBeamLLEdit, m_d->ui->SSBeamLUMLabel,  m_d->ui->SSBeamLCheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->LHor, m_d->ui->SSBeamLHorLabel, m_d->ui->SSBeamLHorLEdit, m_d->ui->SSBeamLHorUMLabel, m_d->ui->SSBeamLHorCheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->LVer, m_d->ui->SSBeamLVerLabel, m_d->ui->SSBeamLVerLEdit, m_d->ui->SSBeamLVerUMLabel, m_d->ui->SSBeamLVerCheckBox );

    VarPlusGUI::connectVar( m_d->ssBeamTimber->alpha1, m_d->ui->SSBeamAlpha1Label, m_d->ui->SSBeamAlpha1LEdit, m_d->ui->SSBeamAlpha1UMLabel, m_d->ui->SSBeamAlpha1CheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->L1, m_d->ui->SSBeamL1Label, m_d->ui->SSBeamL1LEdit, m_d->ui->SSBeamL1UMLabel,  m_d->ui->SSBeamL1CheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->LHor1, m_d->ui->SSBeamLHor1Label, m_d->ui->SSBeamLHor1LEdit, m_d->ui->SSBeamLHor1UMLabel, m_d->ui->SSBeamLHor1CheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->LVer1, m_d->ui->SSBeamLVer1Label, m_d->ui->SSBeamLVer1LEdit, m_d->ui->SSBeamLVer1UMLabel, m_d->ui->SSBeamLVer1CheckBox );

    VarPlusGUI::connectVar( m_d->ssBeamTimber->alpha2, m_d->ui->SSBeamAlpha2Label, m_d->ui->SSBeamAlpha2LEdit, m_d->ui->SSBeamAlpha2UMLabel,  m_d->ui->SSBeamAlpha2CheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->L2, m_d->ui->SSBeamL2Label, m_d->ui->SSBeamL2LEdit, m_d->ui->SSBeamL2UMLabel,  m_d->ui->SSBeamL2CheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->LHor2, m_d->ui->SSBeamLHor2Label, m_d->ui->SSBeamLHor2LEdit, m_d->ui->SSBeamLHor2UMLabel, m_d->ui->SSBeamLHor2CheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->LVer2, m_d->ui->SSBeamLVer2Label, m_d->ui->SSBeamLVer2LEdit, m_d->ui->SSBeamLVer2UMLabel, m_d->ui->SSBeamLVer2CheckBox );

    VarPlusGUI::connectVar( m_d->ssBeamTimber->L0LT, m_d->ui->SSBeamL0LTLabel, m_d->ui->SSBeamL0LTLEdit, m_d->ui->SSBeamL0LTUMLabel, m_d->ui->SSBeamL0LTCheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->lRelLT, m_d->ui->SSBeamlRelLTLabel, m_d->ui->SSBeamlRelLTLEdit, m_d->ui->SSBeamlRelLTUMLabel, m_d->ui->SSBeamlRelLTCheckBox );
    VarPlusGUI::connectVar( m_d->ssBeamTimber->kCrLT, m_d->ui->SSBeamKCrLTLabel, m_d->ui->SSBeamKCrLTLEdit, m_d->ui->SSBeamKCrLTUMLabel, m_d->ui->SSBeamKCrLTCheckBox );

    VarPlusGUI::connectEnumVar( m_d->ssBeamTimber->serviceClass, m_d->ui->SSBeamServiceClassLabel, m_d->ui->SSBeamServiceClassComboBox );
}

void SectionTimberGUI::setBeamL0z(){
    if( m_d->beamL0z->readOnly() ){
        m_d->beamL0z->setValueNormal( m_d->beamL0y->valueNormal() );
    }
}

void SectionTimberGUI::setBeamL0LT(){
    if( m_d->beamL0LT->readOnly() ){
        m_d->beamL0LT->setValueNormal( m_d->beamL0y->valueNormal() );
    }
}

void SectionTimberGUI::setNULSMinus(){
    if( m_d->NULSMinus->readOnly() && m_d->section != NULL ){
        m_d->section->NULSMinus( m_d->NULSMinus, m_d->lsType, m_d->serviceClass, m_d->loadDurationClass );
    }
}

void SectionTimberGUI::setNULSPlus(){
    if( m_d->NULSPlus->readOnly() && m_d->section != NULL ){
        m_d->section->NULSPlus( m_d->NULSPlus, m_d->lsType, m_d->serviceClass, m_d->loadDurationClass );
    }
}

void SectionTimberGUI::setVULS(){
    if( m_d->VULS->readOnly() && m_d->section != NULL ){
        m_d->section->VULS( m_d->VULS, m_d->lsType, m_d->serviceClass, m_d->loadDurationClass );
    }
}

void SectionTimberGUI::setMULSX(){
    if( m_d->MULSX->readOnly() && m_d->section != NULL ){
        m_d->section->MULSX( m_d->MULSX, m_d->lsType, m_d->serviceClass, m_d->loadDurationClass );
    }
}

void SectionTimberGUI::setMULSY(){
    if( m_d->MULSY->readOnly() && m_d->section != NULL ){
        m_d->section->MULSY( m_d->MULSY, m_d->lsType, m_d->serviceClass, m_d->loadDurationClass );
    }
}

void SectionTimberGUI::setMULSZ(){
    if( m_d->MULSZ->readOnly() && m_d->section != NULL ){
        m_d->section->MULSZ( m_d->MULSZ, m_d->lsType, m_d->serviceClass, m_d->loadDurationClass );
    }
}

void SectionTimberGUI::setBeamlY(){
    if( m_d->beamlY->readOnly() && m_d->section != NULL ){
        m_d->section->beamlBuckl( m_d->beamlY, m_d->beamL0y, NULL );
    }
}

void SectionTimberGUI::setBeamlRelY(){
    if( m_d->beamlRelY->readOnly() && m_d->section != NULL ){
        m_d->section->beamlRelBuckl( m_d->beamlRelY, m_d->beamlY );
    }
}

void SectionTimberGUI::setKBucklY(){
    if( m_d->kBucklY->readOnly() && m_d->section != NULL ){
        m_d->section->beamkBuckl( m_d->kBucklY, m_d->beamlRelY );
    }
}

void SectionTimberGUI::setKCrBucklY(){
    if( m_d->kCrBucklY->readOnly() && m_d->section != NULL ){
        m_d->section->beamkCrBuckl( m_d->kCrBucklY, m_d->kBucklY, m_d->beamlRelY );
    }
}

void SectionTimberGUI::setNULSBucklY(){
    if( m_d->NULSBucklY->readOnly() && m_d->section !=  NULL ){
        m_d->section->beamNULSBuckl( m_d->NULSBucklY, m_d->kCrBucklY, m_d->lsType, m_d->serviceClass, m_d->loadDurationClass );
    }
}

void SectionTimberGUI::setBeamlZ(){
    if( m_d->beamlZ->readOnly() && m_d->section != NULL ){
        m_d->section->beamlBuckl( m_d->beamlZ, NULL, m_d->beamL0z );
    }
}

void SectionTimberGUI::setBeamlRelZ(){
    if( m_d->beamlRelZ->readOnly() && m_d->section != NULL ){
        m_d->section->beamlRelBuckl( m_d->beamlRelZ, m_d->beamlZ );
    }
}

void SectionTimberGUI::setKBucklZ(){
    if( m_d->kBucklZ->readOnly() && m_d->section != NULL ){
        m_d->section->beamkBuckl( m_d->kBucklZ, m_d->beamlRelZ );
    }
}

void SectionTimberGUI::setKCrBucklZ(){
    if( m_d->kCrBucklZ->readOnly() && m_d->section != NULL ){
        m_d->section->beamkCrBuckl( m_d->kCrBucklZ, m_d->kBucklZ, m_d->beamlRelZ );
    }
}

void SectionTimberGUI::setNULSBucklZ(){
    if( m_d->NULSBucklZ->readOnly() && m_d->section !=  NULL ){
        m_d->section->beamNULSBuckl( m_d->NULSBucklZ, m_d->kCrBucklZ, m_d->lsType, m_d->serviceClass, m_d->loadDurationClass );
    }
}

void SectionTimberGUI::setBeamlRelLTY(){
    if( m_d->beamlRelLTY->readOnly() && m_d->section != NULL ){
        m_d->section->beamlRelBucklLTY( m_d->beamlRelLTY, m_d->beamL0LT );
    }
}

void SectionTimberGUI::setKCrBucklLTY(){
    if( m_d->kCrBucklLTY->readOnly() && m_d->section != NULL ){
        m_d->section->beamkCrBucklLT( m_d->kCrBucklLTY, m_d->beamlRelLTY );
    }
}

void SectionTimberGUI::setMULSBucklLTY(){
    if( m_d->MULSBucklLTY->readOnly() && m_d->section != NULL ){
        m_d->section->beamMULSBucklLTY( m_d->MULSBucklLTY, m_d->kCrBucklLTY, m_d->lsType, m_d->serviceClass, m_d->loadDurationClass );
    }
}

void SectionTimberGUI::setBeamlRelLTZ(){
    if( m_d->beamlRelLTZ->readOnly() && m_d->section != NULL ){
        m_d->section->beamlRelBucklLTZ( m_d->beamlRelLTZ, m_d->beamL0LT );
    }
}

void SectionTimberGUI::setKCrBucklLTZ(){
    if( m_d->kCrBucklLTZ->readOnly() && m_d->section != NULL ){
        m_d->section->beamkCrBucklLT( m_d->kCrBucklLTZ, m_d->beamlRelLTZ );
    }
}

void SectionTimberGUI::setMULSBucklLTZ(){
    if( m_d->MULSBucklLTZ->readOnly() && m_d->section != NULL ){
        m_d->section->beamMULSBucklLTZ( m_d->MULSBucklLTZ, m_d->kCrBucklLTZ, m_d->lsType, m_d->serviceClass, m_d->loadDurationClass );
    }
}

void SectionTimberGUI::setRd(){
    setBeamL0z();
    setBeamL0LT();
    setNULSMinus();
    setNULSPlus();
    setVULS();
    setMULSX();
    setMULSY();
    setMULSZ();

    setBeamlY();
    setBeamlRelY();
    setKBucklY();
    setKCrBucklY();
    setNULSBucklY();

    setBeamlZ();
    setBeamlRelZ();
    setKBucklZ();
    setKCrBucklZ();
    setNULSBucklZ();

    setBeamlRelLTY();
    setKCrBucklLTY();
    setMULSBucklLTY();
    setBeamlRelLTZ();
    setKCrBucklLTZ();
    setMULSBucklLTZ();
}

void SectionTimberGUI::verifyULS(){
    if( m_d->section ){
        QString mes;

        if( m_d->section->verifyULS( m_d->lsType,
                                     m_d->serviceClass,
                                     m_d->loadDurationClass,
                                     m_d->kCrBucklY,
                                     m_d->kCrBucklZ,
                                     m_d->NSd,
                                     m_d->cenMSd,
                                     m_d->MxSd,
                                     m_d->kCrBucklLTY,
                                     m_d->MySd,
                                     m_d->kCrBucklLTZ,
                                     m_d->MzSd,
                                     m_d->VySd,
                                     m_d->VzSd,
                                     m_d->resultSd,
                                     &mes) ) {
            m_d->ui->resultSdLEdit->setStyleSheet( "* {background-color: rgb(0, 255, 0)}" );
        } else {
            m_d->ui->resultSdLEdit->setStyleSheet( "* {background-color: rgb(255, 0, 0)}" );
        }
        m_d->ui->messagesSdTBrowser->clear();
        m_d->ui->messagesSdTBrowser->append( mes );
    }
}

void SectionTimberGUI::dataSetClear(){
    m_d->dataSetModel->clear();
}

void SectionTimberGUI::dataSetLoadFromFile(){
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning( this, trUtf8("QStruct"), trUtf8("Non riesco ad aprire il file file %1:\n%2.").arg(fileName).arg(file.errorString()));
            return;
        }
        m_d->dataSetModel->clear();
        m_d->dataSetModel->loadFromFile( file );
        file.close();
    }
}

void SectionTimberGUI::dataSetVerifyULSAll(){
    m_d->dataSetModel->verifyULSAll();
}

void SectionTimberGUI::dataSetFromTableToGUI(){
    int currRow = m_d->ui->dataSetTableView->selectionModel()->currentIndex().row();
    if( currRow >= 0 && currRow < m_d->dataSetModel->rowCount() ){
        SectionTimberData * currData = m_d->dataSetModel->data( currRow );

        *m_d->NSd = *(currData->N);
        *m_d->MySd = *(currData->My);
        *m_d->MzSd = *(currData->Mz);
        *m_d->cenMSd = *(currData->cenM);
        *m_d->VzSd = *(currData->Vz);
        *m_d->VySd = *(currData->Vy);

        m_d->lsType->setValueNormal( m_d->dataSetLSType->valueNormal() );
        m_d->serviceClass->setValueNormal( m_d->dataSetServiceClass->valueNormal() );
        m_d->loadDurationClass->setValueNormal( m_d->dataSetLoadDurationClass->valueNormal() );
    }
}

void SectionTimberGUI::dataSetFromGUIToTable(){
    int currRow = m_d->ui->dataSetTableView->selectionModel()->currentIndex().row();
    if( currRow >= 0 && currRow < m_d->dataSetModel->rowCount() ){
        SectionTimberData * currData = m_d->dataSetModel->data( currRow );

        *(currData->N) = *m_d->NSd;
        *(currData->Mx) = *m_d->MxSd;
        *(currData->My) = *m_d->MySd;
        *(currData->Mz) = *m_d->MzSd;
        *(currData->cenM) = *m_d->cenMSd;
        *(currData->Vz) = *m_d->VzSd;
        *(currData->Vy) = *m_d->VySd;

        m_d->dataSetLSType->setValueNormal( m_d->lsType->valueNormal() );
        m_d->dataSetServiceClass->setValueNormal( m_d->serviceClass->valueNormal() );
        m_d->dataSetLoadDurationClass->setValueNormal( m_d->loadDurationClass->valueNormal() );
    }
}

void SectionTimberGUI::SSBeamAddLoad(){
    QModelIndexList listRows = m_d->ui->SSBeamLoadTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->SSBeamLoadModel->insertRows( LoadType::loadLine, listRows.last().row()+1, listRows.size());
    } else {
        int row = m_d->ui->SSBeamLoadTableView->currentIndex().row();
        if( row < 0 || row > m_d->SSBeamLoadModel->count() )
            row = m_d->SSBeamLoadModel->count() - 1 ;
        m_d->SSBeamLoadModel->insertRows( LoadType::loadArea, row + 1 );
    }
}

void SectionTimberGUI::SSBeamRemoveLoad(){
    QModelIndexList listRows = m_d->ui->SSBeamLoadTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        for( int i=(listRows.size()-1); i >= 0; i-- ){
            int row = listRows.at(i).row();
            if( row >= 0 && row < m_d->SSBeamLoadModel->count() ) {
                m_d->SSBeamLoadModel->removeRows( row );
            }
        }
    }
}

void SectionTimberGUI::setSection(Section * s){
    if( m_d->section ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );
        VarPlusGUI::disconnectEnumVar( m_d->ui->shapeLabel, m_d->ui->shapeComboBox );
        if( m_d->section->p(0) ){
            VarPlusGUI::disconnectVar( m_d->ui->p0Label, m_d->ui->p0LEdit, m_d->ui->p0UMLabel );
        }
        if( m_d->section->p(1) ){
            VarPlusGUI::disconnectVar( m_d->ui->p1Label, m_d->ui->p1LEdit, m_d->ui->p1UMLabel );
        }
        VarPlusGUI::disconnectVar( m_d->ui->ALabel, m_d->ui->ALEdit, m_d->ui->AUMLabel, m_d->ui->ACheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->IyLabel, m_d->ui->IyLEdit, m_d->ui->IyUMLabel, m_d->ui->IyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->IzLabel, m_d->ui->IzLEdit, m_d->ui->IzUMLabel, m_d->ui->IzCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->IyLabel, m_d->ui->WyLEdit, m_d->ui->WyUMLabel, m_d->ui->WyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->IzLabel, m_d->ui->WzLEdit, m_d->ui->WzUMLabel, m_d->ui->WzCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->iyLabel, m_d->ui->iyLEdit, m_d->ui->iyUMLabel, m_d->ui->iyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->izLabel, m_d->ui->izLEdit, m_d->ui->izUMLabel, m_d->ui->izCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->ITLabel, m_d->ui->ITLEdit, m_d->ui->ITUMLabel, m_d->ui->ITCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->khLabel, m_d->ui->khLEdit, m_d->ui->khUMLabel, m_d->ui->khCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->khyLabel, m_d->ui->khyLEdit, m_d->ui->khyUMLabel, m_d->ui->khyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->khzLabel, m_d->ui->khzLEdit, m_d->ui->khzUMLabel, m_d->ui->khzCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->kshLabel, m_d->ui->kshLEdit, m_d->ui->kshUMLabel, m_d->ui->kshCheckBox );

        disconnect( m_d->section->G->y, SIGNAL(valueChanged(QString)), this, SLOT(setYCenMSd()));
        disconnect( m_d->section->G->z, SIGNAL(valueChanged(QString)), this, SLOT(setZCenMSd()));

        disconnect(m_d->section, SIGNAL(sectionChanged()), this, SLOT(setRd()) );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        m_materialComboBox->setCurrentIndex( -1 );
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    }

    SectionTimber * sectTimber = dynamic_cast<SectionTimber *>(s);
    m_d->section = sectTimber;
    m_d->ssBeamTimber->setSection( sectTimber );

    if( m_d->section ){
        VarPlusGUI::connectVar( m_d->section->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar( m_d->section->pW, m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );
        VarPlusGUI::connectEnumVar( m_d->section->sectionShape, m_d->ui->shapeLabel, m_d->ui->shapeComboBox );
        if( m_d->section->p(0) ){
            VarPlusGUI::connectVar( m_d->section->p(0), m_d->ui->p0Label, m_d->ui->p0LEdit, m_d->ui->p0UMLabel );
        }
        if( m_d->section->p(1) ){
            VarPlusGUI::connectVar( m_d->section->p(1), m_d->ui->p1Label, m_d->ui->p1LEdit, m_d->ui->p1UMLabel );
        }
        VarPlusGUI::connectVar( m_d->section->A, m_d->ui->ALabel, m_d->ui->ALEdit, m_d->ui->AUMLabel, m_d->ui->ACheckBox );
        VarPlusGUI::connectVar( m_d->section->Iyy, m_d->ui->IyLabel, m_d->ui->IyLEdit, m_d->ui->IyUMLabel, m_d->ui->IyCheckBox );
        VarPlusGUI::connectVar( m_d->section->Izz, m_d->ui->IzLabel, m_d->ui->IzLEdit, m_d->ui->IzUMLabel, m_d->ui->IzCheckBox );
        VarPlusGUI::connectVar( m_d->section->Wy, m_d->ui->IyLabel, m_d->ui->WyLEdit, m_d->ui->WyUMLabel, m_d->ui->WyCheckBox );
        VarPlusGUI::connectVar( m_d->section->Wz, m_d->ui->IzLabel, m_d->ui->WzLEdit, m_d->ui->WzUMLabel, m_d->ui->WzCheckBox );
        VarPlusGUI::connectVar( m_d->section->iy, m_d->ui->iyLabel, m_d->ui->iyLEdit, m_d->ui->iyUMLabel, m_d->ui->iyCheckBox );
        VarPlusGUI::connectVar( m_d->section->iz, m_d->ui->izLabel, m_d->ui->izLEdit, m_d->ui->izUMLabel, m_d->ui->izCheckBox );
        VarPlusGUI::connectVar( m_d->section->IT, m_d->ui->ITLabel, m_d->ui->ITLEdit, m_d->ui->ITUMLabel, m_d->ui->ITCheckBox );
        VarPlusGUI::connectVar( m_d->section->kh, m_d->ui->khLabel, m_d->ui->khLEdit, m_d->ui->khUMLabel, m_d->ui->khCheckBox );
        VarPlusGUI::connectVar( m_d->section->khy, m_d->ui->khyLabel, m_d->ui->khyLEdit, m_d->ui->khyUMLabel, m_d->ui->khyCheckBox );
        VarPlusGUI::connectVar( m_d->section->khz, m_d->ui->khzLabel, m_d->ui->khzLEdit, m_d->ui->khzUMLabel, m_d->ui->khzCheckBox );
        VarPlusGUI::connectVar( m_d->section->ksh, m_d->ui->kshLabel, m_d->ui->kshLEdit, m_d->ui->kshUMLabel, m_d->ui->kshCheckBox );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        if( m_d->section->timber() != NULL ){
            m_materialComboBox->setCurrentIndex( m_materialComboBox->findData( QVariant(m_d->section->timber()->id->valueNormal()) ) );
        } else {
            m_materialComboBox->setCurrentIndex( -1 );
        }
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );

        connect( m_d->section->G->y, SIGNAL(valueChanged(QString)), this, SLOT(setYCenMSd()));
        connect( m_d->section->G->z, SIGNAL(valueChanged(QString)), this, SLOT(setZCenMSd()));

        connect(m_d->section, SIGNAL(sectionChanged()), this, SLOT(setRd()) );

        setRd();
    }
}

void SectionTimberGUI::setYCenMSd(){
    if( m_d->section ){
        if( m_d->cenMSd->y->readOnly() ){
            *(m_d->cenMSd->y) = *(m_d->section->G->y);
        }
    }
}

void SectionTimberGUI::setZCenMSd(){
    if( m_d->section ){
        if( m_d->cenMSd->z->readOnly() ){
            *(m_d->cenMSd->z) = *(m_d->section->G->z);
        }
    }
}

Material * SectionTimberGUI::material(){
    if( m_d->section != NULL ) {
        return m_d->section->material();
    }
    return NULL;
}

void SectionTimberGUI::setMaterialFromComboBox(){
    if( m_d->section != NULL && m_materialModel != NULL && m_materialComboBox != NULL ){
        if(  m_materialComboBox->currentIndex() == -1 ){
            m_d->section->setTimber( NULL );
        } else {
            Timber * timber = dynamic_cast<Timber *>(m_materialModel->materialId( m_materialComboBox->currentData().toUInt()));
            if( timber != NULL ){
                m_d->section->setTimber( timber );
            }
        }
    }
}

QList< Material * > SectionTimberGUI::materialsList(){
    return m_materialModel->materialsList( MaterialModel::TimberMaterial );
}
