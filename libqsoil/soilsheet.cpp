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
#include "soilsheet.h"

#include "soilmodel.h"
#include "soil.h"
#include "sectionpoints.h"
#include "point2dplusmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "boolplus.h"
#include "qstringplus.h"
#include "uintplus.h"
#include "varpluscontainer.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QGraphicsWidget>

#include <cmath>
#include <algorithm>

class SoilSheetPrivate: public VarPlusContainer {
public:
    SoilSheetPrivate( UnitMeasure * ump):
        VarPlusContainer( ump, "SoilSheetPrivate", "SoilSheetPrivate" ){

        graphicsItem = new QGraphicsWidget();

        soilDown = NULL;
        soilUp = NULL;

    }

    /** Sezione di terreno verso monte */
    SectionPoints * sectionSoilUp;

    /** Terreno verso valle */
    Soil * soilDown;
    /** Terreno verso monte */
    Soil * soilUp;

    /** Oggetto grafico radice */
    QGraphicsWidget *graphicsItem;
};

SoilSheet::SoilSheet( UnitMeasure * ump, const QString & n, Soil * sDown, Soil * sUp, QObject * parent ):
    QObject( parent ),
    VarPlusContainer( ump, "SoilSheet", n ),
    m_d( new SoilSheetPrivate(ump) ){
    setSoilDown( sDown );
    setSoilUp( sUp );
    nextId();
    initVar();
}

SoilSheet::SoilSheet( UnitMeasure * ump, const QXmlStreamAttributes &attrs, SoilModel * sModel, QObject * parent ):
    QObject( parent ),
    VarPlusContainer( ump, "SoilSheet", "" ),
    m_d( new SoilSheetPrivate(ump) ){
    initVar();
    loadFromXML( attrs, sModel );
}

void SoilSheet::writeXml( QXmlStreamWriter * writer ){
    writer->writeStartElement(*m_typeNameInternal);
    for( QMap<QString, VarPlus *>::iterator i = m_varContainer->begin(); i != m_varContainer->end(); ++i) {
        writer->writeAttribute( (*i)->xmlAttribute() );
    }
    if( m_d->soilDown ){
        writer->writeAttribute( "soilDown", m_d->soilDown->id->valueStr() );
    }
    if( m_d->soilUp ){
        writer->writeAttribute( "soilUp", m_d->soilUp->id->valueStr() );
    }
    writer->writeEndElement();
}

void SoilSheet::loadFromXML( const QXmlStreamAttributes &attrs, SoilModel * sModel ){
    for ( int i=0; i<attrs.count(); ++i ) {
        for( QMap<QString, VarPlus *>::iterator iter = m_varContainer->begin(); iter != m_varContainer->end(); ++iter ){
            if ( attrs.at(i).name().toString().toUpper() == (*iter)->name().toUpper() ) {
                (*iter)->setValueNormal( attrs.at(i).value().toString() );
            } else if ( attrs.at(i).name().toString().toUpper() == "SOILDOWN" ) {
                setSoilDown( sModel->soilId( attrs.at(i).value().toString().toUInt() ) );
            } else if ( attrs.at(i).name().toString().toUpper() == "SOILUP" ) {
                setSoilUp( sModel->soilId( attrs.at(i).value().toString().toUInt() ) );
            }
        }
    }
    updateReadOnlyVars();
}

void SoilSheet::initVar() {
    soilDownZ  = new DoublePlus( 0.8, "soilDownZ", m_unitMeasure, UnitMeasure::length );
    soilDownZ->setToolTip( trUtf8("Z del terreno verso valle"));
    soilDownZ->setRichName( trUtf8("z"));
    addVarToContainer( soilDownZ );

    soilDownDelta = new DoublePlus( 0.0, "soilDownDelta", m_unitMeasure, UnitMeasure::angle );
    soilDownDelta->setToolTip( trUtf8("Coefficiente di attrito muro-terrano verso valle"));
    soilDownDelta->setRichName( trUtf8("δ"));
    soilDownDelta->setReadOnly( true );
    addVarToContainer( soilDownDelta );

    soilUpZ  = new DoublePlus( 6.8, "soilUpZ", m_unitMeasure, UnitMeasure::length );
    soilUpZ->setToolTip( trUtf8("Z del terreno verso monte"));
    soilUpZ->setRichName( trUtf8("z"));
    addVarToContainer( soilUpZ );

    soilUpDelta = new DoublePlus( 0.0, "soilUpDelta", m_unitMeasure, UnitMeasure::angle );
    soilUpDelta->setToolTip( trUtf8("Coefficiente di attrito muro-terrano verso monte"));
    soilUpDelta->setRichName( trUtf8("δ"));
    soilUpDelta->setReadOnly( false );
    addVarToContainer( soilUpDelta );
    connect( soilDownDelta, SIGNAL(readOnlyChanged(bool)), this, SLOT(setSoilDelta()));
    connect( soilUpDelta, SIGNAL(readOnlyChanged(bool)), this, SLOT(setSoilDelta()));

    setSoilDelta();
}

QGraphicsItem * SoilSheet::qGraphicsItem() {
    return m_d->graphicsItem;
}

Soil * SoilSheet::soilDown(){
    return m_d->soilDown;
}

void SoilSheet::setSoilDown( Soil * s ){
    if( m_d->soilDown != s){
        Soil * oldSoil = m_d->soilDown;
        if( m_d->soilDown ){
            m_d->soilDown->removeIsUsedBy( this );
            disconnect( m_d->soilDown->phiPrimeK, SIGNAL(valueChanged(QString)), this, SLOT(setSoilDelta()));
        }
        m_d->soilDown = s;
        if( m_d->soilDown ){
            m_d->soilDown->setIsUsedBy( this );
            connect( m_d->soilDown->phiPrimeK, SIGNAL(valueChanged(QString)), this, SLOT(setSoilDelta()));
        }
        emit soilDownChanged( oldSoil, m_d->soilDown );
    }
}

Soil * SoilSheet::soilUp(){
    return m_d->soilUp;
}

void SoilSheet::setSoilUp( Soil * s ){
    if( m_d->soilUp != s){
        Soil * oldSoil = m_d->soilUp;
        if( m_d->soilUp ){
            m_d->soilUp->removeIsUsedBy( this );
            disconnect( m_d->soilUp->phiPrimeK, SIGNAL(valueChanged(QString)), this, SLOT(setSoilDelta()));
        }
        m_d->soilUp = s;
        if( m_d->soilUp ){
            m_d->soilUp->setIsUsedBy( this );
            connect( m_d->soilUp->phiPrimeK, SIGNAL(valueChanged(QString)), this, SLOT(setSoilDelta()));
        }
        emit soilUpChanged( oldSoil, m_d->soilUp );
    }
}

void SoilSheet::setSoilDelta(){
    if( soilUpDelta->readOnly() && (m_d->soilUp != NULL) ){
        soilUpDelta->setValueNormal( m_d->soilUp->phiPrimeK->valueNormal() );
    }
    if( soilDownDelta->readOnly() && (m_d->soilDown != NULL) ){
        soilDownDelta->setValueNormal( m_d->soilDown->phiPrimeK->valueNormal() );
    }
}

void SoilSheet::calculateGammaWD( DoublePlus * soilDownGammaW, DoublePlus * soilUpGammaW, DoublePlus * gamma){
    if( soilDownGammaW ){
        if(  soilDownGammaW->readOnly() ){
            if( m_d->soilDown ){
                if( gamma ){
                    m_d->soilDown->gammaWD( soilDownGammaW, gamma );
                } else {
                    soilDownGammaW->setValueNormal( m_d->soilDown->gammaW->valueNormal() );
                }
            } else {
                soilDownGammaW->setValueNormal(0.0);
            }
        }
    }
    if( soilUpGammaW ){
        if(  soilUpGammaW->readOnly() ){
            if( m_d->soilUp ){
                if( gamma ){
                    m_d->soilUp->gammaWD( soilUpGammaW, gamma );
                } else {
                    soilUpGammaW->setValueNormal( m_d->soilUp->gammaW->valueNormal() );
                }
            } else {
                soilUpGammaW->setValueNormal(0.0);
            }
        }
    }
}

void SoilSheet::calculatePhiPrimeD( DoublePlus * soilDownPhiPrimeD, DoublePlus * soilUpPhiPrimeD, DoublePlus * gammaTanPhi ){
    if( soilDownPhiPrimeD->readOnly() ){
        if( m_d->soilDown ){
            m_d->soilDown->phiPrimeD( soilDownPhiPrimeD, gammaTanPhi );
        } else {
            soilDownPhiPrimeD->setValueNormal( 0.0 );
        }
    }
    if( soilUpPhiPrimeD->readOnly() ){
        if( m_d->soilUp ){
            m_d->soilUp->phiPrimeD( soilUpPhiPrimeD, gammaTanPhi );
        } else {
            soilUpPhiPrimeD->setValueNormal( 0.0 );
        }
    }
}

void SoilSheet::calculateCPrimeD( DoublePlus * soilDownCPrimeD, DoublePlus * soilUpCPrimeD, DoublePlus * gammaCPrime ){
    if( soilDownCPrimeD->readOnly() ){
        if( m_d->soilDown ){
            m_d->soilDown->cPrimeD( soilDownCPrimeD, gammaCPrime );
        } else {
            soilDownCPrimeD->setValueNormal( 0.0 );
        }
    }
    if( soilUpCPrimeD->readOnly() ){
        if( m_d->soilUp ){
            m_d->soilUp->cPrimeD( soilUpCPrimeD, gammaCPrime );
        } else {
            soilUpCPrimeD->setValueNormal( 0.0 );
        }
    }
}

void SoilSheet::calculateDeltaD( DoublePlus * soilDownDeltaD, DoublePlus * soilUpDeltaD, DoublePlus * gammaTanPhi ){
    if( soilDownDeltaD->readOnly() ){
        if( gammaTanPhi->valueNormal() != 0.0 ){
            soilDownDeltaD->setValueNormal( atan( tan(soilDownDelta->valueNormal() ) / gammaTanPhi->valueNormal() ) );
        } else {
            soilDownDeltaD->setValueNormal( 0.0 );
        }
    }
    if( soilUpDeltaD->readOnly() ){
        if( gammaTanPhi->valueNormal() != 0.0 ){
            soilUpDeltaD->setValueNormal( atan( tan(soilUpDelta->valueNormal() ) / gammaTanPhi->valueNormal() ) );
        } else {
            soilUpDeltaD->setValueNormal( 0.0 );
        }
    }
}

void SoilSheet::calculateK( DoublePlus * soilUpPhiPrimeD, DoublePlus * soilUpDeltaD, DoublePlus * Ka,
                                DoublePlus * soilDownPhiPrimeD, DoublePlus * soilDownDeltaD, DoublePlus * Kp, DoublePlus *chiKp, DoublePlus *KpRed ){
    if( Ka->readOnly() ){
        if( m_d->soilUp ){
            m_d->soilUp->Ka( Ka, soilUpPhiPrimeD, NULL, NULL, soilUpDeltaD);
        }
    }

    if( Kp->readOnly() ){
        if( m_d->soilDown ){
            m_d->soilDown->Kp( Kp, soilDownPhiPrimeD, NULL, NULL, soilDownDeltaD);
        }
    }

    if( KpRed->readOnly() ){
        KpRed->setValueNormal( chiKp->valueNormal() * Kp->valueNormal() );
    }
}
