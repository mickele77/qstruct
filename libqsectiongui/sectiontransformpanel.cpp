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
#include "sectiontransformpanel.h"
#include "ui_sectiontransformpanel.h"

#include "section.h"

#include "varplusguiutility.h"
#include "varpluscontainer.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "boolplus.h"

class SectionTransformPanelPrivate: public VarPlusContainer {
public:
    SectionTransformPanelPrivate( UnitMeasure * ump):
        VarPlusContainer( ump, "", "" ){
        ui = new Ui::SectionTransformPanel;
        section = NULL;

        rot = new DoublePlus( 0.0, "rot", m_unitMeasure, UnitMeasure::angle );
        rot->setRichName( QObject::trUtf8("α" ));
        rot->setToolTip( QObject::trUtf8("Angolo di rotazione"));
        rot->setReadOnly( false );
        addVarToContainer(rot);

        trans = new Point2DPlus( "trans", "transY", 0.0, "transZ", 0.0, m_unitMeasure, UnitMeasure::sectL );
        trans->y->setRichName(QObject::trUtf8("Δy"));
        trans->y->setToolTip(QObject::trUtf8("Componente y del vettore traslazione"));
        trans->y->setReadOnly( false );
        trans->z->setRichName(QObject::trUtf8("Δz"));
        trans->z->setToolTip(QObject::trUtf8("Componente z del vettore traslazione"));
        trans->z->setReadOnly( false );
        addVarToContainer(trans);

    };
    ~SectionTransformPanelPrivate(){
        delete ui;
    };
    void connectUI(){
        VarPlusGUI::connectVar( rot,  ui->rotateAngleLabel, ui->rotateAngleLEdit, ui->rotateAngleUMLabel );
        VarPlusGUI::connectVar( trans->y, ui->translateDyLabel, ui->translateDyLEdit, ui->translateDyUMLabel );
        VarPlusGUI::connectVar( trans->z, ui->translateDzLabel, ui->translateDzLEdit, ui->translateDzUMLabel );
    };

    Section * section;
    DoublePlus * rot;
    Point2DPlus * trans;
    Ui::SectionTransformPanel *ui;
};

SectionTransformPanel::SectionTransformPanel(UnitMeasure * ump, QWidget *parent) :
    QWidget( parent),
    m_d( new SectionTransformPanelPrivate( ump ) ){
    m_d->ui->setupUi(this);

    m_d->connectUI();
    connect( m_d->ui->translatePButton, SIGNAL(clicked()), this, SLOT(translate()));
    connect( m_d->ui->rotateAnglePButton, SIGNAL(clicked()), this, SLOT(rotate()));
    connect( m_d->ui->GTranslatePButton, SIGNAL(clicked()), this, SLOT(GTranslate()));
    connect( m_d->ui->PTransRotPButton, SIGNAL(clicked()), this, SLOT(PTransRot()));
}

SectionTransformPanel::~SectionTransformPanel() {
    delete m_d;
}

Section * SectionTransformPanel::section(){
    return m_d->section;
}

void SectionTransformPanel::setSection(Section * s){
    m_d->section = s;
}

void SectionTransformPanel::rotate(){
    if( m_d->section ){
        m_d->section->rotate( m_d->rot );
    }
}

void SectionTransformPanel::translate(){
    if( m_d->section ){
        m_d->section->translate( m_d->trans );
    }
}

void SectionTransformPanel::GTranslate(){
    if( m_d->section ){
        m_d->section->GTranslate();
    }
}

void SectionTransformPanel::PTransRot(){
    if( m_d->section ){
        m_d->section->PTransRot();
    }
}
