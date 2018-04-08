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
#include "multisection.h"

#include "multisectionmodelbase.h"

MultiSection::MultiSection(UnitMeasure * ump, Material * mat, const QString & nn, QObject *parent) :
    Section( ump, mat, nn, parent ),
    m_model(NULL){
}

MultiSection & MultiSection::operator=( MultiSection & val ){
    if( this != &val ){
        this->Section::operator=( val );

        if( m_model != NULL && val.m_model != NULL ){
            *m_model = *(val.m_model);
        }
    }
    return *this;
}

Section & MultiSection::operator=( Section & val ){
    if( this != &val ){
        MultiSection * mSection = dynamic_cast<MultiSection *>(&val);
        if( mSection ){
            *this = *mSection;
        }
        else {
            this->Section::operator=( val );
        }
    }
    return *this;
}

MultiSectionModelBase * MultiSection::model(){
    return m_model;
}

void MultiSection::suspendSignals(bool susp) {
    if( susp ){
        m_model->suspendSignals( true );
        VarPlusContainer::suspendSignals( true );
    } else {
        VarPlusContainer::suspendSignals( false );
        m_model->suspendSignals( false );
    }
}

void MultiSection::translateNormal( double yTrans, double zTrans, bool suspSignals ){
    if( m_model ){
        if( suspSignals ) {
            suspendSignals( true );
        }

        m_model->translateNormal( yTrans, zTrans);

        if( suspSignals ) {
            suspendSignals( false );
        }
    }
}

void MultiSection::rotateNormal( double rot, bool suspSignals ){
    if( m_model ){
        if( suspSignals ) {
            suspendSignals( true );
        }

        m_model->rotateNormal( rot );

        if( suspSignals ){
            suspendSignals( false );
        }
    }
}

Section * MultiSection::section( int i ){
    if( m_model ){
        return m_model->section(i);
    } else {
        return NULL;
    }
}

Section * MultiSection::sectionId( unsigned int i){
    if( m_model ){
        return m_model->sectionId( i );
    } else {
        return NULL;
    }
}

int MultiSection::nSection(){
    if( m_model ){
        return m_model->count();
    } else {
        return 0;
    }
}

double MultiSection::ANormal(){
    if( m_model ){
        return m_model->ANormal();
    } else {
        return 0.0;
    }
}

double MultiSection::SyNormal(){
    if( m_model ){
        return m_model->SyNormal();
    } else {
        return 0.0;
    }
}

double MultiSection::SzNormal(){
    if( m_model ){
        return m_model->SzNormal();
    } else {
        return 0.0;
    }
}

double MultiSection::IyyNormal(){
    if( m_model ){
        return m_model->IyyNormal();
    } else {
        return 0.0;
    }
}

double MultiSection::IzzNormal(){
    if( m_model ){
        return m_model->IzzNormal();
    } else {
        return 0.0;
    }
}

double MultiSection::IyzNormal(){
    if( m_model ){
        return m_model->IyzNormal();
    } else {
        return 0.0;
    }
}

double MultiSection::Sy3DyDzNormal(){
    if( m_model ){
        return m_model->Sy3DyDzNormal();
    } else {
        return 0.0;
    }
}

double MultiSection::Sz3DyDzNormal(){
    if( m_model ){
        return m_model->Sz3DyDzNormal();
    } else {
        return 0.0;
    }
}

double MultiSection::Sy2zDyDzNormal(){
    if( m_model ){
        return m_model->Sy2zDyDzNormal();
    } else {
        return 0.0;
    }
}

double MultiSection::Syz2DyDzNormal(){
    if( m_model ){
        return m_model->Syz2DyDzNormal();
    } else {
        return 0.0;
    }
}

double MultiSection::pWNormal(){
    if( m_model ){
        return m_model->pWNormal();
    } else {
        return 0.0;
    }
}

double MultiSection::NULSNormal(double l, double m, QList<Point2DModel *> *sects ){
    if( m_model ){
        return m_model->NULSNormal( l, m, sects );
    } else {
        return 0.0;
    }
}

void MultiSection::MULSNormal( double *yRet, double * zRet, double l, double m, QList<Point2DModel *> * sects ){
    if( m_model ){
        m_model->MULSNormal(yRet, zRet, l, m, sects );
    }
}

QGraphicsItem * MultiSection::qGraphicsItem(){
    if( m_model ){
        return m_model->qGraphicsItem();
    } else {
        return Section::qGraphicsItem();
    }
}
