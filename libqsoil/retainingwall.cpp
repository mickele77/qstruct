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
#include "retainingwall.h"

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

class RetainingWallPrivate: public VarPlusContainer {
public:
    RetainingWallPrivate( UnitMeasure * ump):
        VarPlusContainer( ump, "RetainingWallPrivate", "RetainingWallPrivate" ){

        graphicsItem = new QGraphicsWidget();

        soilDown = NULL;
        soilUp = NULL;

        toe = new SectionPoints( ump, NULL, "toe" );
        toe->pointsModel()->appendRows( 4 );
        toe->qGraphicsItem()->setParentItem( graphicsItem );

        heel = new SectionPoints( ump, NULL, "heel" );
        heel->pointsModel()->appendRows( 4 );
        heel->qGraphicsItem()->setParentItem( graphicsItem );

        key = new SectionPoints( ump, NULL, "key" );
        key->pointsModel()->appendRows( 4 );
        key->qGraphicsItem()->setParentItem( graphicsItem );

        stem = new SectionPoints( ump, NULL, "stem" );
        stem->pointsModel()->appendRows( 6 );
        stem->qGraphicsItem()->setParentItem( graphicsItem );

        counterfort = new SectionPoints( ump, NULL, "counterfort" );
        counterfort->pointsModel()->appendRows( 4 );
        counterfort->qGraphicsItem()->setParentItem( graphicsItem );

        sectionSoilUp = new SectionPoints( ump, NULL, "sectionSoilUp" );
        sectionSoilUp->pointsModel()->appendRows( 4 );
        sectionSoilUp->qGraphicsItem()->setParentItem( graphicsItem );
    };

    /** Mensola di valle */
    SectionPoints * toe;
    /** Mensola di monte */
    SectionPoints * heel;
    /** Dente */
    SectionPoints * key;
    /** Muro */
    SectionPoints * stem;
    /** Contrafforte */
    SectionPoints * counterfort;

    /** Sezione di terreno verso monte */
    SectionPoints * sectionSoilUp;

    /** Terreno verso valle */
    Soil * soilDown;
    /** Terreno verso monte */
    Soil * soilUp;

    /** Oggetto grafico radice */
    QGraphicsWidget *graphicsItem;
};

RetainingWall::RetainingWall( UnitMeasure * ump, const QString & n, Soil * sDown, Soil * sUp, QObject * parent ):
    QObject( parent ),
    VarPlusContainer( ump, "RetainingWall", n ),
    m_d( new RetainingWallPrivate(ump) ){
    setSoilDown( sDown );
    setSoilUp( sUp );
    nextId();
    initVar();
}

RetainingWall::RetainingWall( UnitMeasure * ump, const QXmlStreamAttributes &attrs, SoilModel * sModel, QObject * parent ):
    QObject( parent ),
    VarPlusContainer( ump, "RetainingWall", "" ),
    m_d( new RetainingWallPrivate(ump) ){
    initVar();
    loadFromXML( attrs, sModel );
}

void RetainingWall::writeXml( QXmlStreamWriter * writer ){
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

void RetainingWall::loadFromXML( const QXmlStreamAttributes &attrs, SoilModel * sModel ){
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

void RetainingWall::initVar() {
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
    connect( soilUpZ, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( soilUpZ );

    soilUpI = new DoublePlus( 0.0, "soilUpI", m_unitMeasure, UnitMeasure::angle );
    soilUpI->setToolTip( trUtf8("Inclinazione del piano di campagna verso monte"));
    soilUpI->setRichName( trUtf8("i"));
    addVarToContainer( soilUpI );

    soilUpDelta = new DoublePlus( 0.0, "soilUpDelta", m_unitMeasure, UnitMeasure::angle );
    soilUpDelta->setToolTip( trUtf8("Coefficiente di attrito muro-terrano verso monte"));
    soilUpDelta->setRichName( trUtf8("δ"));
    soilUpDelta->setReadOnly( false );
    addVarToContainer( soilUpDelta );
    connect( soilDownDelta, SIGNAL(readOnlyChanged(bool)), this, SLOT(setSoilDelta()));
    connect( soilUpDelta, SIGNAL(readOnlyChanged(bool)), this, SLOT(setSoilDelta()));

    baseAlpha = new DoublePlus( 0.0, "baseAlpha", m_unitMeasure, UnitMeasure::angle );
    baseAlpha->setToolTip( trUtf8("Inclinazione della fondazione rispetto al piano orizzontale"));
    baseAlpha->setRichName( trUtf8("α"));
    connect( baseAlpha, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()) );
    addVarToContainer( baseAlpha );

    toeGammaW = new DoublePlus( 2.4e+4, "toeGammaW", m_unitMeasure, UnitMeasure::loadFVolume );;
    toeGammaW->setToolTip( trUtf8("Peso specifico del materiale della mensola di valle"));
    toeGammaW->setRichName( trUtf8("γ"));
    toeGammaW->setReadOnly( true );
    addVarToContainer( toeGammaW );

    toeHLeft = new DoublePlus( 0.8, "toeHLeft", m_unitMeasure, UnitMeasure::length );
    toeHLeft->setToolTip( trUtf8("Spessore della mensola di valle, verso valle (misurata in verticale)"));
    toeHLeft->setRichName( trUtf8("H<span style=\" vertical-align:sub;\">l</span>"));
    addVarToContainer( toeHLeft );

    toeHRight = new DoublePlus( 0.8, "toeHRight", m_unitMeasure, UnitMeasure::length );
    toeHRight->setToolTip( trUtf8("Spessore della mensola di valle, verso monte (misurata in verticale)"));
    toeHRight->setRichName( trUtf8("H<span style=\" vertical-align:sub;\">r</span>"));
    toeHRight->setReadOnly( true );
    connect( toeHRight, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    connect( toeHLeft, SIGNAL(valueChanged(QString)), this, SLOT(setToeHRight()));
    connect( toeHRight, SIGNAL(readOnlyChanged(bool)), this, SLOT(setToeHRight()));
    addVarToContainer( toeHRight );

    toeB = new DoublePlus( 0.8, "toeB", m_unitMeasure, UnitMeasure::length );
    toeB->setToolTip( trUtf8("Lunghezza della mensola di valle (misurata in orizzontale)"));
    toeB->setRichName( trUtf8("B"));
    connect( toeB, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( toeB );

    toeD = new DoublePlus( 1.0, "toeD", m_unitMeasure, UnitMeasure::length );
    toeD->setToolTip( trUtf8("Profondit�   della mensola di valle"));
    toeD->setRichName( trUtf8("d"));
    toeD->setReadOnly( true );
    addVarToContainer( toeD );

    heelGammaW = new DoublePlus( 2.4e+4, "heelGammaW", m_unitMeasure, UnitMeasure::loadFVolume );;
    heelGammaW->setToolTip( trUtf8("Peso specifico del materiale della mensola di monte"));
    heelGammaW->setRichName( trUtf8("γ"));
    heelGammaW->setReadOnly( true );
    addVarToContainer( heelGammaW );

    heelHLeft = new DoublePlus( 0.8, "heelHLeft", m_unitMeasure, UnitMeasure::length );
    heelHLeft->setToolTip( trUtf8("Spessore della mensola di monte, verso valle (misurata in verticale)"));
    heelHLeft->setRichName( trUtf8("H<span style=\" vertical-align:sub;\">l</span>"));
    addVarToContainer( heelHLeft );

    heelHRight = new DoublePlus( 0.8, "heelHRight", m_unitMeasure, UnitMeasure::length );
    heelHRight->setToolTip( trUtf8("Spessore della mensola di monte, verso monte (misurata in verticale)"));
    heelHRight->setRichName( trUtf8("H<span style=\" vertical-align:sub;\">r</span>"));
    heelHRight->setReadOnly( true );
    connect( heelHRight, SIGNAL(valueChanged(QString)),this, SLOT(updateSections()));
    connect( heelHLeft, SIGNAL(valueChanged(QString)), this, SLOT(setHeelHRight()));
    connect( heelHRight, SIGNAL(readOnlyChanged(bool)),this, SLOT(setHeelHRight()));
    addVarToContainer( heelHRight );

    heelB = new DoublePlus( 3.0, "heelB", m_unitMeasure, UnitMeasure::length );
    heelB->setToolTip( trUtf8("Lunghezza della mensola di monte (misurata in orizzontale)"));
    heelB->setRichName( trUtf8("B"));
    connect( heelB, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( heelB );

    heelD = new DoublePlus( 1.0, "heelD", m_unitMeasure, UnitMeasure::length );
    heelD->setToolTip( trUtf8("Profondità   della mensola di monte"));
    heelD->setRichName( trUtf8("d"));
    heelD->setReadOnly( true );
    addVarToContainer( heelD );

    keyGammaW = new DoublePlus( 2.4e+4, "keyGammaW", m_unitMeasure, UnitMeasure::loadFVolume );;
    keyGammaW->setToolTip( trUtf8("Peso specifico del materiale del dente"));
    keyGammaW->setRichName( trUtf8("γ"));
    keyGammaW->setReadOnly( true );
    addVarToContainer( keyGammaW );

    keyY = new DoublePlus( 0.0, "keyY", m_unitMeasure, UnitMeasure::length );;
    keyY->setToolTip( trUtf8("Ascissa del primo punto del dente"));
    keyY->setRichName( trUtf8("y"));
    connect( keyY, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( keyY );

    keyH = new DoublePlus( 0.0, "keyH", m_unitMeasure, UnitMeasure::length );
    keyH->setToolTip( trUtf8("Altezza del dente"));
    keyH->setRichName( trUtf8("H"));
    connect( keyH, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( keyH );

    keyB = new DoublePlus( 0.0, "keyB", m_unitMeasure, UnitMeasure::length );
    keyB->setToolTip( trUtf8("Spessore del dente"));
    keyB->setRichName( trUtf8("B"));
    connect( keyB, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( keyB );

    keyD = new DoublePlus( 1.0, "keyD", m_unitMeasure, UnitMeasure::length );
    keyD->setToolTip( trUtf8("Profondit�  del dente"));
    keyD->setRichName( trUtf8("d"));
    keyD->setReadOnly( true );
    addVarToContainer( keyD );

    counterfortGammaW = new DoublePlus( 2.4e+4, "counterfortGammaW", m_unitMeasure, UnitMeasure::loadFVolume );;
    counterfortGammaW->setToolTip( trUtf8("Peso specifico del materiale costituente il muro"));
    counterfortGammaW->setRichName( trUtf8("γ"));
    counterfortGammaW->setReadOnly( true );
    addVarToContainer( counterfortGammaW );

    counterfortBBottom = new DoublePlus( 0.0, "counterfortBBottom", m_unitMeasure, UnitMeasure::length );
    counterfortBBottom->setToolTip( trUtf8("Larghezza dello sperone in basso"));
    counterfortBBottom->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">bot</span>"));
    connect( counterfortBBottom, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( counterfortBBottom );

    counterfortBTop = new DoublePlus( 0.0, "counterfortBTop", m_unitMeasure, UnitMeasure::length );
    counterfortBTop->setToolTip( trUtf8("Larghezza dello sperone in alto"));
    counterfortBTop->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">top</span>"));
    connect( counterfortBTop, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( counterfortBTop );

    counterfortH = new DoublePlus( 2.5, "counterfortH", m_unitMeasure, UnitMeasure::length );
    counterfortH->setToolTip( trUtf8("Altezza dello sperone"));
    counterfortH->setRichName( trUtf8("H"));
    counterfortH->setReadOnly( true );
    connect( counterfortH, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( counterfortH );

    counterfortT = new DoublePlus( 0.3, "counterfortT", m_unitMeasure, UnitMeasure::length );
    counterfortT->setToolTip( trUtf8("Spessore dello sperone"));
    counterfortT->setRichName( trUtf8("t"));
    addVarToContainer( counterfortT );

    stemGammaW = new DoublePlus( 2.4e+4, "stemGammaW", m_unitMeasure, UnitMeasure::loadFVolume );;
    stemGammaW->setToolTip( trUtf8("Peso specifico del materiale costituente il muro"));
    stemGammaW->setRichName( trUtf8("γ"));
    addVarToContainer( stemGammaW );
    connect( stemGammaW, SIGNAL(valueChanged(QString)), this, SLOT(setGammaW()));
    connect( heelGammaW, SIGNAL(readOnlyChanged(bool)), this, SLOT(setGammaW()));
    connect( toeGammaW, SIGNAL(readOnlyChanged(bool)), this, SLOT(setGammaW()));
    connect( keyGammaW, SIGNAL(readOnlyChanged(bool)), this, SLOT(setGammaW()));
    connect( counterfortGammaW, SIGNAL(readOnlyChanged(bool)), this, SLOT(setGammaW()));

    stemBeta = new DoublePlus( 0.0, "stemB", m_unitMeasure, UnitMeasure::angle );
    stemBeta->setToolTip( trUtf8("Angolo rispetto alla verticale del paramento del muro verso monte"));
    stemBeta->setRichName( trUtf8("β"));
    connect( stemBeta, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( stemBeta );

    stemTBottom = new DoublePlus( 0.8, "stemTBottom", m_unitMeasure, UnitMeasure::length );
    stemTBottom->setToolTip( trUtf8("Spessore del muro in basso"));
    stemTBottom->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">bot</span>"));
    connect( stemTBottom, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( stemTBottom );

    stemTTop = new DoublePlus( 0.4, "stemTTop", m_unitMeasure, UnitMeasure::length );
    stemTTop->setToolTip( trUtf8("Spessore del muro in alto"));
    stemTTop->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">top</span>"));
    connect( stemTTop, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( stemTTop );

    stemH = new DoublePlus( 6.8, "stemH", m_unitMeasure, UnitMeasure::length );
    stemH->setToolTip( trUtf8("Altezza del muro"));
    stemH->setRichName( trUtf8("H"));
    connect( stemH, SIGNAL(valueChanged(QString)), this, SLOT(updateSections()));
    addVarToContainer( stemH );

    stemD = new DoublePlus( 1.0, "stemD", m_unitMeasure, UnitMeasure::length );
    stemD->setToolTip( trUtf8("Profondit�  del muro"));
    stemD->setRichName( trUtf8("d"));
    connect( stemD, SIGNAL(valueChanged(QString)), this, SLOT(setD()) );
    addVarToContainer( stemD );
    connect( stemD, SIGNAL(valueChanged(QString)), this, SLOT(setD()));
    connect( heelD, SIGNAL(readOnlyChanged(bool)), this, SLOT(setD()));
    connect( toeD, SIGNAL(readOnlyChanged(bool)), this, SLOT(setD()));
    connect( keyD, SIGNAL(readOnlyChanged(bool)), this, SLOT(setD()));

    connect( stemH, SIGNAL(valueChanged(QString)), this, SLOT(setCounterfortH()));
    connect( toeHRight, SIGNAL(valueChanged(QString)), this, SLOT(setCounterfortH()));
    connect( baseAlpha, SIGNAL(valueChanged(QString)), this, SLOT(setCounterfortH()));
    connect( counterfortH, SIGNAL(readOnlyChanged(bool)), this, SLOT(setCounterfortH()));

    setHeelHRight();
    setToeHRight();
    setGammaW();
    setD();
    setCounterfortH();
    setSoilDelta();
    updateSections();
}

QGraphicsItem * RetainingWall::qGraphicsItem() {
    return m_d->graphicsItem;
}

Soil * RetainingWall::soilDown(){
    return m_d->soilDown;
}

void RetainingWall::setSoilDown( Soil * s ){
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

Soil * RetainingWall::soilUp(){
    return m_d->soilUp;
}

void RetainingWall::setSoilUp( Soil * s ){
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

void RetainingWall::updateSections() {
    m_d->stem->pointsModel()->P(0)->setValueNormal( 0.0, 0.0 );
    m_d->stem->pointsModel()->P(1)->setValueNormal( 0.0, heelHLeft->valueNormal() );
    m_d->stem->pointsModel()->P(2)->setValueNormal( - (stemH->valueNormal()-heelHLeft->valueNormal()) *  tan(stemBeta->valueNormal()), stemH->valueNormal() );
    m_d->stem->pointsModel()->P(3)->setValueNormal( - (stemH->valueNormal()- heelHLeft->valueNormal()) *  tan(stemBeta->valueNormal()) - stemTTop->valueNormal(), stemH->valueNormal() );
    m_d->stem->pointsModel()->P(4)->setValueNormal( - stemTBottom->valueNormal(), - stemTBottom->valueNormal() * tan(baseAlpha->valueNormal()) + toeHRight->valueNormal() );
    m_d->stem->pointsModel()->P(5)->setValueNormal( - stemTBottom->valueNormal(), - stemTBottom->valueNormal() * tan(baseAlpha->valueNormal()) );

    m_d->heel->pointsModel()->P(0)->setValueNormal( 0.0, 0.0 );
    m_d->heel->pointsModel()->P(1)->setValueNormal( heelB->valueNormal(), heelB->valueNormal() * tan(baseAlpha->valueNormal()));
    m_d->heel->pointsModel()->P(2)->setValueNormal( heelB->valueNormal(), heelB->valueNormal() * tan(baseAlpha->valueNormal()) + heelHRight->valueNormal() );
    m_d->heel->pointsModel()->P(3)->setValueNormal( 0.0, heelHLeft->valueNormal());

    m_d->toe->pointsModel()->P(0)->setValueNormal( -stemTBottom->valueNormal(), - stemTBottom->valueNormal() * tan(baseAlpha->valueNormal()) );
    m_d->toe->pointsModel()->P(1)->setValueNormal( -stemTBottom->valueNormal(), - stemTBottom->valueNormal() * tan(baseAlpha->valueNormal()) + toeHRight->valueNormal() );
    m_d->toe->pointsModel()->P(2)->setValueNormal( - (stemTBottom->valueNormal() + toeB->valueNormal()), - (stemTBottom->valueNormal() + toeB->valueNormal()) * tan(baseAlpha->valueNormal()) + toeHLeft->valueNormal() );
    m_d->toe->pointsModel()->P(3)->setValueNormal( - (stemTBottom->valueNormal() + toeB->valueNormal()), - (stemTBottom->valueNormal() + toeB->valueNormal()) * tan(baseAlpha->valueNormal()) );

    double gamma = atan2( - stemTBottom->valueNormal() * tan(baseAlpha->valueNormal()) + toeHRight->valueNormal() - (-(stemTBottom->valueNormal()+toeB->valueNormal()) * tan(baseAlpha->valueNormal())  + toeHLeft->valueNormal()),
                          - stemTBottom->valueNormal() - (- stemTBottom->valueNormal() - toeB->valueNormal()) );
    double delta = atan2( - (stemH->valueNormal()-heelHLeft->valueNormal()) * tan(stemBeta->valueNormal()) - stemTTop->valueNormal() + stemTBottom->valueNormal(), stemH->valueNormal() - (- stemTBottom->valueNormal() * tan(baseAlpha->valueNormal())  + toeHRight->valueNormal()) );
    m_d->counterfort->pointsModel()->P(0)->setValueNormal(- stemTBottom->valueNormal(), - stemTBottom->valueNormal() * tan(baseAlpha->valueNormal())  + toeHRight->valueNormal());
    m_d->counterfort->pointsModel()->P(1)->setValueNormal(- stemTBottom->valueNormal() + counterfortH->valueNormal() * tan(delta), - stemTBottom->valueNormal() * tan(baseAlpha->valueNormal())  + toeHRight->valueNormal() + counterfortH->valueNormal());
    m_d->counterfort->pointsModel()->P(2)->setValueNormal(- stemTBottom->valueNormal() + counterfortH->valueNormal() * tan(delta) - counterfortBTop->valueNormal(), - stemTBottom->valueNormal() * tan(baseAlpha->valueNormal())  + toeHRight->valueNormal() + counterfortH->valueNormal());
    m_d->counterfort->pointsModel()->P(3)->setValueNormal(- stemTBottom->valueNormal() - counterfortBBottom->valueNormal(), - stemTBottom->valueNormal() * tan(baseAlpha->valueNormal())  + toeHRight->valueNormal() - counterfortBBottom->valueNormal() * tan(gamma) );

    m_d->key->pointsModel()->P(0)->setValueNormal(keyY->valueNormal(),  keyY->valueNormal() * tan(baseAlpha->valueNormal()) );
    m_d->key->pointsModel()->P(1)->setValueNormal(keyY->valueNormal(), keyY->valueNormal() * tan(baseAlpha->valueNormal()) - keyH->valueNormal() );
    m_d->key->pointsModel()->P(2)->setValueNormal(keyY->valueNormal() + keyB->valueNormal(), keyY->valueNormal() * tan(baseAlpha->valueNormal()) - keyH->valueNormal() );
    m_d->key->pointsModel()->P(3)->setValueNormal(keyY->valueNormal() + keyB->valueNormal(), (keyY->valueNormal() + keyB->valueNormal()) * tan(baseAlpha->valueNormal()) );

    m_d->sectionSoilUp->pointsModel()->P(0)->setValueNormal( 0.0, heelHLeft->valueNormal());
    m_d->sectionSoilUp->pointsModel()->P(1)->setValueNormal( heelB->valueNormal(), heelB->valueNormal() * tan(baseAlpha->valueNormal()) + heelHRight->valueNormal() );
    m_d->sectionSoilUp->pointsModel()->P(2)->setValueNormal( heelB->valueNormal(), soilUpZ->valueNormal() + ((soilUpZ->valueNormal()-heelHLeft->valueNormal()) * tan(stemBeta->valueNormal()) + heelB->valueNormal()) * tan(soilUpI->valueNormal()));
    m_d->sectionSoilUp->pointsModel()->P(3)->setValueNormal( -(soilUpZ->valueNormal()-heelHLeft->valueNormal()) * tan(stemBeta->valueNormal()), soilUpZ->valueNormal());
}

void RetainingWall::setGammaW(){
    if( toeGammaW->readOnly() ){
        toeGammaW->setValueNormal( stemGammaW->valueNormal() );
    }
    if( heelGammaW->readOnly() ){
        heelGammaW->setValueNormal( stemGammaW->valueNormal() );
    }
    if( keyGammaW->readOnly() ){
        keyGammaW->setValueNormal( stemGammaW->valueNormal() );
    }
    if( counterfortGammaW->readOnly() ){
        counterfortGammaW->setValueNormal( stemGammaW->valueNormal() );
    }
}

void RetainingWall::setHeelHRight(){
    if( heelHRight->readOnly() ){
        heelHRight->setValueNormal( heelHLeft->valueNormal() );
    }
}

void RetainingWall::setToeHRight(){
    if( toeHRight->readOnly() ){
        toeHRight->setValueNormal( toeHLeft->valueNormal() );
    }
}

void RetainingWall::setD(){
    if( heelD->readOnly() ){
        heelD->setValueNormal( stemD->valueNormal() );
    }
    if( toeD->readOnly()){
        toeD->setValueNormal( stemD->valueNormal() );
    }
    if( keyD->readOnly() ){
        keyD->setValueNormal( stemD->valueNormal() );
    }
}

void RetainingWall::setCounterfortH(){
    if( counterfortH->readOnly() ){
        counterfortH->setValueNormal( stemH->valueNormal() + stemTBottom->valueNormal() * tan(baseAlpha->valueNormal()) - toeHRight->valueNormal() );
    }
}

void RetainingWall::setSoilDelta(){
    if( soilUpDelta->readOnly() && (m_d->soilUp != NULL) ){
        soilUpDelta->setValueNormal( m_d->soilUp->phiPrimeK->valueNormal() );
    }
    if( soilDownDelta->readOnly() && (m_d->soilDown != NULL) ){
        soilDownDelta->setValueNormal( m_d->soilDown->phiPrimeK->valueNormal() );
    }
}

void RetainingWall::calculateGammaWD( DoublePlus * soilDownGammaW, DoublePlus * soilUpGammaW, DoublePlus * gamma){
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

void RetainingWall::calculatePhiPrimeD( DoublePlus * soilDownPhiPrimeD, DoublePlus * soilUpPhiPrimeD, DoublePlus * gammaTanPhi ){
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

void RetainingWall::calculateCPrimeD( DoublePlus * soilDownCPrimeD, DoublePlus * soilUpCPrimeD, DoublePlus * gammaCPrime ){
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

void RetainingWall::calculateDeltaD( DoublePlus * soilDownDeltaD, DoublePlus * soilUpDeltaD, DoublePlus * gammaTanPhi ){
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

void RetainingWall::calculateK( DoublePlus * soilUpPhiPrimeD, DoublePlus * soilUpDeltaD, DoublePlus * Ka,
                                DoublePlus * soilDownPhiPrimeD, DoublePlus * soilDownDeltaD, DoublePlus * Kp, DoublePlus *chiKp, DoublePlus *KpRed ){
    if( Ka->readOnly() ){
        if( m_d->soilUp ){
            m_d->soilUp->Ka( Ka, soilUpPhiPrimeD, soilUpI, stemBeta, soilUpDeltaD);
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

void RetainingWall::calculateEQTheta( DoublePlus * ret, DoublePlus * kh, DoublePlus * kv, RWEarthQuakeDirection * direction ){
    if( direction->valueNormal() == EQup ){
        ret->setValueNormal( atan2( kh->valueNormal() , (1.0 - kv->valueNormal() )) );
    } else if( direction->valueNormal() == EQdown ){
        ret->setValueNormal( atan2( kh->valueNormal() , (1.0 + kv->valueNormal() )) );
    }
}

void RetainingWall::calculateEQK( DoublePlus * theta,
                                  DoublePlus * soilUpPhiPrimeD, DoublePlus * soilUpDeltaD, DoublePlus * Ka,
                                  DoublePlus * soilDownPhiPrimeD, DoublePlus * soilDownDeltaD,
                                  DoublePlus * Kp, DoublePlus * chiKp, DoublePlus * KpRed){
    if( Ka->readOnly() ){
        if( m_d->soilUp ){
            m_d->soilUp->Kae( Ka, theta, soilUpPhiPrimeD, soilUpI, stemBeta, soilUpDeltaD);
        }
    }

    if( Kp->readOnly() ){
        if( m_d->soilUp ){
            m_d->soilDown->Kpe( Kp, theta, soilDownPhiPrimeD, NULL, NULL, soilDownDeltaD);
        }
    }

    if( KpRed->readOnly() ){
        KpRed->setValueNormal( chiKp->valueNormal() * Kp->valueNormal() );
    }
}

void RetainingWall::calculateOverTurning( Point2DPlus * MdC, DoublePlus * MEd, DoublePlus * MRd,
                                          DoublePlus * soilDownCPrimeD, DoublePlus * soilUpCPrimeD,
                                          DoublePlus * Ka,
                                          BoolPlus * applyKp, DoublePlus * Kp,
                                          DoublePlus * Gk2, DoublePlus * Qk,
                                          DoublePlus * gammaG1MinEQU, DoublePlus * gammaG1MaxEQU, DoublePlus * , DoublePlus * gammaG2MaxEQU, DoublePlus * , DoublePlus * gammaQMaxEQU){

    // ********** Calcola il polo rispetto a cui si calcolano i momenti *************
    if( MdC != NULL ){
        if( MdC->y->readOnly() ){
            MdC->y->setValueNormal( m_d->toe->pointsModel()->P(3)->y->valueNormal() );
        }
        if( MdC->z->readOnly() ){
            // se il dente è disposto a sinistra, effettua la verifica rispetto al punto inferiore sinistro del dente
            // altrimenti effettua la verifica rispetto al punto inferiore sisnitro della mensola di valle
            if( MdC->y->valueNormal() == m_d->key->pointsModel()->P(1)->y->valueNormal() ){
                MdC->z->setValueNormal( m_d->key->pointsModel()->P(1)->z->valueNormal() );
            } else {
                MdC->z->setValueNormal( m_d->toe->pointsModel()->P(3)->z->valueNormal() );
            }
        }
    }

    // ******** Calcola il momento resistente ***********
    if( MRd != NULL && MdC != NULL ){
        if( MRd->readOnly() ){
            // MRd - Peso proprio muro di sostegno
            double Mrd = 0.0;
            Mrd += toeGammaW->valueNormal() * ( m_d->toe->Sz->valueNormal() + m_d->toe->A->valueNormal() * ( - MdC->y->valueNormal()) ) * toeD->valueNormal();
            Mrd += heelGammaW->valueNormal() * ( m_d->heel->Sz->valueNormal() + m_d->heel->A->valueNormal() * ( - MdC->y->valueNormal()) ) * heelD->valueNormal();
            Mrd += stemGammaW->valueNormal() * ( m_d->stem->Sz->valueNormal() + m_d->stem->A->valueNormal() * ( - MdC->y->valueNormal()) ) * stemD->valueNormal();
            Mrd += keyGammaW->valueNormal() * ( m_d->key->Sz->valueNormal() + m_d->key->A->valueNormal() * ( - MdC->y->valueNormal()) ) * keyD->valueNormal();
            Mrd += counterfortGammaW->valueNormal() * ( m_d->counterfort->Sz->valueNormal() + m_d->counterfort->A->valueNormal() * ( - MdC->y->valueNormal()) ) * counterfortT->valueNormal();
            // MRd - Peso proprio terreno sopra mensola di monte
            // si assume la larghezza del paramento di monte
            if( m_d->soilUp != NULL ){
                Mrd += m_d->soilUp->gammaW->valueNormal() * ( m_d->sectionSoilUp->Sz->valueNormal()+ m_d->sectionSoilUp->A->valueNormal() * ( - MdC->y->valueNormal()) ) * stemD->valueNormal();
            }
            Mrd *= gammaG1MinEQU->valueNormal();

            if( applyKp->valueNormal() && m_d->soilDown != NULL ){
                // MRd - Spinta passiva su mensola di valle
                double zDown = m_d->toe->pointsModel()->P(3)->z->valueNormal();
                if( zDown > soilDownZ->valueNormal() ){
                    zDown = soilDownZ->valueNormal();
                }
                if( zDown > m_d->key->pointsModel()->P(1)->z->valueNormal() ){
                    zDown = m_d->key->pointsModel()->P(1)->z->valueNormal();
                }
                double h = soilDownZ->valueNormal() - zDown;
                Mrd += keyD->valueNormal() * \
                        0.50 * gammaG1MinEQU->valueNormal() *  \
                        Kp->valueNormal() * m_d->soilDown->gammaW->valueNormal() * \
                        pow(h, 2.0) * (zDown + h / 3.0 - MdC->z->valueNormal());
                Mrd += keyD->valueNormal() * \
                        soilDownCPrimeD->valueNormal() * sqrt(fabs(Kp->valueNormal())) * \
                        h * (zDown + h / 2.0 - MdC->z->valueNormal());
            }

            // Passiamo il valore calcolato nelle variabile DoublePlus
            MRd->setValueNormal( Mrd );
        }
    }


    // ******** Momento destabilizzante **********

    if( MEd != NULL ) {
        if( MEd->readOnly() ){
            double Med = 0.0;
            // parametri geometrici per calcolo spinta
            double zUp = m_d->sectionSoilUp->pointsModel()->P(2)->z->valueNormal();
            double zDown =  m_d->heel->pointsModel()->P(1)->z->valueNormal();
            if( zDown >  m_d->key->pointsModel()->P(2)->z->valueNormal() ){
                zDown =  m_d->key->pointsModel()->P(2)->z->valueNormal();
            }

            // MEd - Spinta terreno
            double h = zUp - zDown;
            if( m_d->soilUp ){
                // contributo angolo attrito interno
                Med += stemD->valueNormal() * \
                        0.50 * gammaG1MaxEQU->valueNormal() * Ka->valueNormal() * m_d->soilUp->gammaW->valueNormal() * \
                        pow(h,2.0) * ( (zDown + h / 3.0) - MdC->z->valueNormal());
                // contributo coesione
                Med -= stemD->valueNormal() * \
                        soilUpCPrimeD->valueNormal() * sqrt(fabs(Ka->valueNormal()) ) * h * ( (zDown + h / 2.0) - MdC->z->valueNormal());
            }
            // MEd - Spinta carichi esterni
            Med += gammaG2MaxEQU->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Gk2->valueNormal() * h * ((zDown + h/2.0) - MdC->z->valueNormal());
            Med += gammaQMaxEQU->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Qk->valueNormal() * h * ((zDown + h/2.0) - MdC->z->valueNormal());

            // passiamo il valore calcolato
            MEd->setValueNormal( Med );
        }
    }
}

void RetainingWall::calculateEQOverTurning( Point2DPlus * MdC, DoublePlus * MEd, DoublePlus * MRd,
                                            DoublePlus * soilDownCPrimeD, DoublePlus * soilUpCPrimeD,
                                            DoublePlus * Ka,
                                            BoolPlus * applyKp, DoublePlus * Kp,
                                            RWEarthQuakeDirection * EQDir, DoublePlus * kv, DoublePlus *kh,
                                            DoublePlus * Gk2, DoublePlus * Qk,
                                            DoublePlus * psi2Q ){
    // ********** Calcola il polo rispetto a cui si calcolano i momenti *************
    if( MdC != NULL ){
        if( MdC->y->readOnly() ){
            MdC->y->setValueNormal( m_d->toe->pointsModel()->P(3)->y->valueNormal() );
        }
        if( MdC->z->readOnly() ){
            // se il dente è disposto a sinistra, effettua la verifica rispetto al punto inferiore sinistro del dente
            // altrimenti effettua la verifica rispetto al punto inferiore sisnitro della mensola di valle
            if( MdC->y->valueNormal() == m_d->key->pointsModel()->P(1)->y->valueNormal() ){
                MdC->z->setValueNormal( m_d->key->pointsModel()->P(1)->z->valueNormal() );
            } else {
                MdC->z->setValueNormal( m_d->toe->pointsModel()->P(3)->z->valueNormal() );
            }
        }
    }

    // ********* Momento stabilizzante ***********

    // Azione inerziale su muro di sostegno e terreno e monte
    double moltIner = 1.0;
    if( EQDir->valueNormal() == EQup ){
        moltIner = 1.0 - kv->valueNormal();
    } else if( EQDir->valueNormal() == EQdown ){
        moltIner = 1.0 + kv->valueNormal();
    }

    // ******** Calcola il momento resistente ***********
    if( MRd != NULL && MdC != NULL ){
        if( MRd->readOnly() ){

            // MRd - Peso proprio muro di sostegno
            double Mrd = 0.0;
            Mrd += toeGammaW->valueNormal() * ( m_d->toe->Sz->valueNormal() + m_d->toe->A->valueNormal() * ( - MdC->y->valueNormal()) ) * toeD->valueNormal();
            Mrd += heelGammaW->valueNormal() * ( m_d->heel->Sz->valueNormal() + m_d->heel->A->valueNormal() * ( - MdC->y->valueNormal()) ) * heelD->valueNormal();
            Mrd += stemGammaW->valueNormal() * ( m_d->stem->Sz->valueNormal() + m_d->stem->A->valueNormal() * ( - MdC->y->valueNormal()) ) * stemD->valueNormal();
            Mrd += keyGammaW->valueNormal() * ( m_d->key->Sz->valueNormal() + m_d->key->A->valueNormal() * ( - MdC->y->valueNormal()) ) * keyD->valueNormal();
            Mrd += counterfortGammaW->valueNormal() * ( m_d->counterfort->Sz->valueNormal() + m_d->counterfort->A->valueNormal() * ( - MdC->y->valueNormal()) ) * counterfortT->valueNormal();
            // MRd - Peso proprio terreno sopra mensola di monte
            // si assume la larghezza del paramento di monte
            if( m_d->soilUp != NULL ){
                Mrd += m_d->soilUp->gammaW->valueNormal() * ( m_d->sectionSoilUp->Sz->valueNormal()+ m_d->sectionSoilUp->A->valueNormal() * ( - MdC->y->valueNormal()) ) * stemD->valueNormal();
            }
            Mrd *= moltIner;

            if( applyKp->valueNormal() && m_d->soilDown != NULL ){
                // MRd - Spinta passiva su mensola di valle
                double zDown = m_d->toe->pointsModel()->P(3)->z->valueNormal();
                if( zDown > soilDownZ->valueNormal() ){
                    zDown = soilDownZ->valueNormal();
                }
                if( zDown > m_d->key->pointsModel()->P(1)->z->valueNormal() ){
                    zDown = m_d->key->pointsModel()->P(1)->z->valueNormal();
                }
                double h = soilDownZ->valueNormal() - zDown;
                Mrd += keyD->valueNormal() * \
                        0.50 *  \
                        Kp->valueNormal() * m_d->soilDown->gammaW->valueNormal() * moltIner * \
                        pow(h, 2.0) * (zDown + h / 3.0 - MdC->z->valueNormal());
                Mrd += keyD->valueNormal() * \
                        soilDownCPrimeD->valueNormal() * sqrt(fabs(Kp->valueNormal())) * \
                        h * (zDown + h / 2.0 - MdC->z->valueNormal());
            }

            // Passiamo il valore calcolato nelle variabile DoublePlus
            MRd->setValueNormal( Mrd );
        }
    }

    // ******** Momento destabilizzante **********

    if( MEd != NULL && MdC != NULL ) {
        if( MEd->readOnly() ){

            // Azione sismica su peso proprio muro di sostegno
            double Med = toeGammaW->valueNormal() * ( m_d->toe->Sy->valueNormal() + m_d->toe->A->valueNormal() * ( - MdC->z->valueNormal()) ) * toeD->valueNormal();
            Med += heelGammaW->valueNormal() * ( m_d->heel->Sy->valueNormal() + m_d->heel->A->valueNormal() * ( - MdC->z->valueNormal()) ) * heelD->valueNormal();
            Med += stemGammaW->valueNormal() * ( m_d->stem->Sy->valueNormal() + m_d->stem->A->valueNormal() * ( - MdC->z->valueNormal()) ) * stemD->valueNormal();
            Med += keyGammaW->valueNormal() * ( m_d->key->Sy->valueNormal() + m_d->key->A->valueNormal() * ( - MdC->z->valueNormal()) ) * keyD->valueNormal();
            Med += counterfortGammaW->valueNormal() * ( m_d->counterfort->Sy->valueNormal() + m_d->counterfort->A->valueNormal() * ( - MdC->z->valueNormal()) ) * counterfortT->valueNormal();
            // MRd - Peso proprio terreno sopra mensola di monte
            // si assume la larghezza del paramento di monte
            if( m_d->soilUp != NULL ){
                Med += m_d->soilUp->gammaW->valueNormal() * ( m_d->sectionSoilUp->Sy->valueNormal()+ m_d->sectionSoilUp->A->valueNormal() * ( - MdC->z->valueNormal()) ) * stemD->valueNormal();
            }
            Med *= kh->valueNormal();

            // parametri geometrici per calcolo spinta
            double zUp = m_d->sectionSoilUp->pointsModel()->P(2)->z->valueNormal();
            double zDown =  m_d->heel->pointsModel()->P(1)->z->valueNormal();
            if( zDown >  m_d->key->pointsModel()->P(2)->z->valueNormal() ){
                zDown =  m_d->key->pointsModel()->P(2)->z->valueNormal();
            }
            // Spinta terreno
            double h = zUp - zDown;
            if( m_d->soilUp ){
                // contributo angolo attrito interno
                Med += stemD->valueNormal() * \
                        0.50 * Ka->valueNormal() * m_d->soilUp->gammaW->valueNormal() * moltIner * \
                        pow(h,2.0) * ( (zDown + h / 3.0) - MdC->z->valueNormal());
                // contributo coesione
                Med -= stemD->valueNormal() * \
                        soilUpCPrimeD->valueNormal() * sqrt(fabs(Ka->valueNormal()) ) * h * ( (zDown + h / 2.0) - MdC->z->valueNormal());
            }
            // MEd - Spinta carichi esterni
            Med += Ka->valueNormal() * stemD->valueNormal() * Gk2->valueNormal() * h * ((zDown + h/2.0) - MdC->z->valueNormal());
            Med += psi2Q->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Qk->valueNormal() * h * ((zDown + h/2.0) - MdC->z->valueNormal());

            // passiamo il valore calcolato
            MEd->setValueNormal( Med );
        }
    }
}

void RetainingWall::calculateSlidingVH( DoublePlus * VEd,
                                        DoublePlus * HEd,
                                        DoublePlus * HRdP,
                                        DoublePlus * soilDownCPrimeD,
                                        DoublePlus * soilUpCPrimeD,
                                        DoublePlus * Ka,
                                        DoublePlus * Kp,
                                        DoublePlus * Gk2, DoublePlus * Qk,
                                        DoublePlus * gammaG1Min,
                                        DoublePlus * gammaG1Max,
                                        DoublePlus * ,
                                        DoublePlus * gammaG2Max,
                                        DoublePlus * ,
                                        DoublePlus * gammaQMax ){

    // calcolo peso del muro controterra
    if( VEd ){
        if( VEd->readOnly() ){
            double V = 0.0;
            double d = stemD->valueNormal();
            V += toeGammaW->valueNormal() * m_d->toe->A->valueNormal() * toeD->valueNormal();
            V += heelGammaW->valueNormal() * m_d->heel->A->valueNormal() * heelD->valueNormal();
            V += stemGammaW->valueNormal() * m_d->stem->A->valueNormal() * stemD->valueNormal();
            V += keyGammaW->valueNormal() * m_d->key->A->valueNormal() * keyD->valueNormal();
            V += counterfortGammaW->valueNormal() * m_d->counterfort->A->valueNormal() * counterfortT->valueNormal();
            if( m_d->soilUp ){
                V += m_d->soilUp->gammaW->valueNormal() * m_d->sectionSoilUp->A->valueNormal() * d;
            }
            V *= gammaG1Min->valueNormal();
            VEd->setValueNormal( V );
        }
    }

    // Calcolo spinta passiva
    if( HRdP ){
        if( HRdP->readOnly() ){
            double H = 0.0;
            if( m_d->soilDown ){
                // Spinta passiva
                double zDown = m_d->toe->pointsModel()->P(3)->z->valueNormal();
                if( zDown > soilDownZ->valueNormal() ){
                    zDown = soilDownZ->valueNormal();
                }
                if( zDown > m_d->key->pointsModel()->P(1)->z->valueNormal() ){
                    zDown = m_d->key->pointsModel()->P(1)->z->valueNormal();
                }
                double h = soilDownZ->valueNormal() - zDown;
                // contributo angolo attrito interno
                H += keyD->valueNormal() * \
                        0.50 * gammaG1Min->valueNormal() *  \
                        Kp->valueNormal() * m_d->soilDown->gammaW->valueNormal() * pow(h, 2.0);
                // contributo coesione
                H += keyD->valueNormal() * \
                        soilDownCPrimeD->valueNormal() * sqrt( fabs(Kp->valueNormal())) * h;
            }
            HRdP->setValueNormal(H);
        }
    }

    // Calcolo spinta attiva
    if( HEd ){
        if( HEd->readOnly() ){
            double H = 0.0;
            // parametri geometrici per calcolo spinta
            double zUp = m_d->sectionSoilUp->pointsModel()->P(2)->z->valueNormal();
            double zDown =  m_d->heel->pointsModel()->P(1)->z->valueNormal();
            if( zDown >  m_d->key->pointsModel()->P(2)->z->valueNormal() ){
                zDown =  m_d->key->pointsModel()->P(2)->z->valueNormal();
            }

            // Spinta attiva terreno
            double h = zUp - zDown;
            if( m_d->soilUp ){
                // contributo angolo attrito interno
                H += stemD->valueNormal() * \
                        0.50 * gammaG1Max->valueNormal() * Ka->valueNormal() * \
                        m_d->soilUp->gammaW->valueNormal() * pow(h,2.0);
                // contributo coesione
                H -= stemD->valueNormal() * \
                        soilUpCPrimeD->valueNormal() * sqrt(fabs(Ka->valueNormal()) ) * h;
            }
            // Spinta carichi esterni
            H += gammaG2Max->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Gk2->valueNormal() * h;
            H += gammaQMax->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Qk->valueNormal() * h;

            HEd->setValueNormal( H );
        }
    }
}

void RetainingWall::calculateSliding( DoublePlus * HRd,
                                      BoolPlus * applyKp,
                                      DoublePlus * HRdP,
                                      DoublePlus * HEd,
                                      DoublePlus * VEd,
                                      DoublePlus * soilDownDeltaD,
                                      DoublePlus * gammaR) {
    double alpha = baseAlpha->valueNormal();

    // se il dente è verso valle, si assume la superficie di scorrimento formata dalla mensola di valle
    // e dal dente
    if( m_d->key->pointsModel()->P(2)->y->valueNormal() == m_d->heel->pointsModel()->P(1)->y->valueNormal() ){
        double a = std::min( atan2( m_d->heel->pointsModel()->P(1)->z->valueNormal() - m_d->toe->pointsModel()->P(3)->z->valueNormal(), m_d->key->pointsModel()->P(1)->y->valueNormal() - m_d->toe->pointsModel()->P(3)->y->valueNormal() ),
                             atan2( m_d->heel->pointsModel()->P(2)->z->valueNormal() - m_d->toe->pointsModel()->P(3)->z->valueNormal(), m_d->key->pointsModel()->P(2)->y->valueNormal() - m_d->toe->pointsModel()->P(3)->y->valueNormal() ) );
        alpha = std::min( alpha, a );
    }
    // scomponiamo HEd e VEd lungo le due componenti, normale e tangenziale alla superficie di
    // scorrimento
    double N = VEd->valueNormal() * cos(alpha) - HEd->valueNormal() * sin(alpha);
    // coefficiente di sicurezza
    double gR = gammaR != NULL ? gammaR->valueNormal(): 1.0;
    if( gR != 0.0 ){
        double TMax = ( N * tan( soilDownDeltaD->valueNormal() ) - N * sin(alpha) )/ gR;
        double HMax = TMax * cos(alpha);
        if(applyKp->valueNormal()){
            HMax += HRdP->valueNormal();
        }
        HRd->setValueNormal( HMax );
    } else {
        HRd->setValueNormal( 0.0 ) ;
    }
}

void RetainingWall::calculateQLimVHM(DoublePlus * VEd,
                                     DoublePlus * HEd,
                                     Point2DPlus * MdC,
                                     DoublePlus * MEd,
                                     DoublePlus * soilDownCPrimeD,
                                     DoublePlus * soilUpCPrimeD,
                                     DoublePlus * Ka,
                                     BoolPlus * applyKp,
                                     DoublePlus * Kp,
                                     DoublePlus *HRdPMax,
                                     DoublePlus *HRdP,
                                     DoublePlus * Gk2, DoublePlus * Qk,
                                     DoublePlus * gammaG1Min,
                                     DoublePlus * gammaG1Max,
                                     DoublePlus * ,
                                     DoublePlus * gammaG2Max,
                                     DoublePlus * ,
                                     DoublePlus * gammaQMax ){

    if( VEd != NULL ){
        if( VEd->readOnly() ){
            double V = 0.0;
            double d = stemD->valueNormal();
            V += toeGammaW->valueNormal() * m_d->toe->A->valueNormal() * toeD->valueNormal();
            V += heelGammaW->valueNormal() * m_d->heel->A->valueNormal() * heelD->valueNormal();
            V += stemGammaW->valueNormal() * m_d->stem->A->valueNormal() * stemD->valueNormal();
            V += keyGammaW->valueNormal() * m_d->key->A->valueNormal() * keyD->valueNormal();
            V += counterfortGammaW->valueNormal() * m_d->counterfort->A->valueNormal() * counterfortT->valueNormal();
            if( m_d->soilUp ){
                V += m_d->soilUp->gammaW->valueNormal() * m_d->sectionSoilUp->A->valueNormal() * d;
            }
            V *= gammaG1Max->valueNormal();
            VEd->setValueNormal( V );
        }
    }

    // Calcola la spinta passiva massima mobilitabile
    if( HRdPMax ){
        if( HRdPMax->readOnly() && m_d->soilDown != NULL ){
            double H = 0.0;

            // Spinta passiva
            double zDown = m_d->toe->pointsModel()->P(3)->z->valueNormal();
            if( zDown > soilDownZ->valueNormal() ){
                zDown = soilDownZ->valueNormal();
            }
            if( zDown > m_d->key->pointsModel()->P(1)->z->valueNormal() ){
                zDown = m_d->key->pointsModel()->P(1)->z->valueNormal();
            }
            double h = soilDownZ->valueNormal() - zDown;
            // contributo angolo attrito interno
            H += keyD->valueNormal() * \
                    0.50 * gammaG1Min->valueNormal() *  \
                    Kp->valueNormal() * m_d->soilDown->gammaW->valueNormal() * pow(h, 2.0);
            // contributo coesione
            H += keyD->valueNormal() * \
                    soilDownCPrimeD->valueNormal() * sqrt( fabs(Kp->valueNormal())) * h;
            HRdPMax->setValueNormal( H );
        }
    }

    if( HEd != NULL ){
        if( HEd->readOnly() ){
            double H = 0.0;
            // parametri geometrici per calcolo spinta
            double zUp = m_d->sectionSoilUp->pointsModel()->P(2)->z->valueNormal();
            double zDown =  m_d->heel->pointsModel()->P(1)->z->valueNormal();
            if( zDown >  m_d->key->pointsModel()->P(2)->z->valueNormal() ){
                zDown =  m_d->key->pointsModel()->P(2)->z->valueNormal();
            }

            // Spinta attiva terreno
            double h = zUp - zDown;
            if( m_d->soilUp ){
                // contributo angolo attrito interno
                H += stemD->valueNormal() * \
                        0.50 * gammaG1Max->valueNormal() * Ka->valueNormal() * \
                        m_d->soilUp->gammaW->valueNormal() * pow(h,2.0);
                // contributo coesione
                H -= stemD->valueNormal() * \
                        soilUpCPrimeD->valueNormal() * sqrt(fabs(Ka->valueNormal()) ) * h;
            }

            // Spinta carichi esterni
            H += gammaG2Max->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Gk2->valueNormal() * h;
            H += gammaQMax->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Qk->valueNormal() * h;

            // Spinta passiva terreno
            // se la spinta passiva massima disponibile è maggiore della spinta agente
            // si assume come spinta passiva il valore della spinta attiva
            if( HRdPMax != NULL && HRdP != NULL ){
                if( H < HRdPMax->valueNormal() ){
                    HRdP->setValueNormal( H );
                } else {
                    HRdP->setValueNormal( HRdPMax->valueNormal() );
                }
            }
            if( applyKp->valueNormal() && HRdP != NULL ){
                H -= HRdP->valueNormal();
            }

            HEd->setValueNormal( H );
        }
    }

    if( MdC != NULL ){
        if( MdC->y->readOnly() ){
            MdC->y->setValueNormal( 0.50 * (m_d->heel->pointsModel()->P(1)->y->valueNormal() + m_d->toe->pointsModel()->P(3)->y->valueNormal()) );
        }
        if( MdC->z->readOnly() ){
            MdC->z->setValueNormal( 0.50 * (m_d->heel->pointsModel()->P(1)->z->valueNormal() + m_d->toe->pointsModel()->P(3)->z->valueNormal()) );
        }
    }

    if( MEd != NULL && MdC != NULL ){
        if( MEd->readOnly() ){
            double Mrd = 0.0;
            Mrd += toeGammaW->valueNormal() * ( m_d->toe->Sz->valueNormal() + m_d->toe->A->valueNormal() * ( - MdC->y->valueNormal()) ) * toeD->valueNormal();
            Mrd += heelGammaW->valueNormal() * ( m_d->heel->Sz->valueNormal() + m_d->heel->A->valueNormal() * ( - MdC->y->valueNormal()) ) * heelD->valueNormal();
            Mrd += stemGammaW->valueNormal() * ( m_d->stem->Sz->valueNormal() + m_d->stem->A->valueNormal() * ( - MdC->y->valueNormal()) ) * stemD->valueNormal();
            Mrd += keyGammaW->valueNormal() * ( m_d->key->Sz->valueNormal() + m_d->key->A->valueNormal() * ( - MdC->y->valueNormal()) ) * keyD->valueNormal();
            Mrd += counterfortGammaW->valueNormal() * ( m_d->counterfort->Sz->valueNormal() + m_d->counterfort->A->valueNormal() * ( - MdC->y->valueNormal()) ) * counterfortT->valueNormal();
            // MRd - Peso proprio terreno sopra mensola di monte
            // si assume la larghezza del paramento di monte
            if( m_d->soilUp != NULL ){
                Mrd += m_d->soilUp->gammaW->valueNormal() * ( m_d->sectionSoilUp->Sz->valueNormal()+ m_d->sectionSoilUp->A->valueNormal() * ( - MdC->y->valueNormal()) ) * stemD->valueNormal();
            }
            Mrd *= gammaG1Max->valueNormal();

            double MRdPCalc = 0.0;
            double HRdPCalc = 0.0;
            if( m_d->soilDown != NULL ){
                // MRd - Spinta passiva su muro
                double zDown = m_d->toe->pointsModel()->P(3)->z->valueNormal();
                if( zDown > soilDownZ->valueNormal() ){
                    zDown = soilDownZ->valueNormal();
                }
                if( zDown > m_d->key->pointsModel()->P(1)->z->valueNormal() ){
                    zDown = m_d->key->pointsModel()->P(1)->z->valueNormal();
                }
                double h = soilDownZ->valueNormal() - zDown;
                MRdPCalc += keyD->valueNormal() * \
                        0.50 * gammaG1Max->valueNormal() *  \
                        Kp->valueNormal() * m_d->soilDown->gammaW->valueNormal() * \
                        pow(h, 2.0) * (zDown + h / 3.0 - MdC->z->valueNormal());
                HRdPCalc += keyD->valueNormal() * \
                        0.50 * gammaG1Max->valueNormal() *  \
                        Kp->valueNormal() * m_d->soilDown->gammaW->valueNormal() * \
                        pow(h, 2.0);
                MRdPCalc += keyD->valueNormal() * \
                        soilDownCPrimeD->valueNormal() * sqrt( fabs(Kp->valueNormal())) * \
                        h * (zDown + h / 2.0 - MdC->z->valueNormal());
                HRdPCalc += keyD->valueNormal() * \
                        soilDownCPrimeD->valueNormal() * sqrt( fabs(Kp->valueNormal())) * \
                        h;
            }
            if( applyKp->valueNormal() && HRdP != NULL && HRdPCalc != 0.0 ){
                if( HRdP->valueNormal() < HRdPCalc ){
                    // riduciamo proporzionalmente il momento dovuto alla spinta passiva
                    Mrd += MRdPCalc * HRdP->valueNormal() / HRdPCalc;
                } else {
                    Mrd += MRdPCalc;
                }
            }

            double Med = 0.0;
            // parametri geometrici per calcolo spinta
            double zUp = m_d->sectionSoilUp->pointsModel()->P(2)->z->valueNormal();
            double zDown =  m_d->heel->pointsModel()->P(1)->z->valueNormal();
            if( zDown >  m_d->key->pointsModel()->P(2)->z->valueNormal() ){
                zDown =  m_d->key->pointsModel()->P(2)->z->valueNormal();
            }

            // MEd - Spinta terreno
            double h = zUp - zDown;
            if( m_d->soilUp ){
                // contributo angolo attrito interno
                Med += stemD->valueNormal() * \
                        0.50 * gammaG1Max->valueNormal() * Ka->valueNormal() * m_d->soilUp->gammaW->valueNormal() * \
                        pow(h,2.0) * ( (zDown + h / 3.0) - MdC->z->valueNormal());
                // contributo coesione
                Med -= stemD->valueNormal() * \
                        soilUpCPrimeD->valueNormal() * sqrt(fabs(Ka->valueNormal()) ) * h * ( (zDown + h / 2.0) - MdC->z->valueNormal());
            }
            // MEd - Spinta carichi esterni
            Med += gammaG2Max->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Gk2->valueNormal() * h * ((zDown + h/2.0) - MdC->z->valueNormal());
            Med += gammaQMax->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Qk->valueNormal() * h * ((zDown + h/2.0) - MdC->z->valueNormal());

            MEd->setValueNormal( Med - Mrd );
        }
    }
}

void RetainingWall::calculateEQSlidingVH( DoublePlus * VEd,
                                          DoublePlus * HEd,
                                          DoublePlus * HRdP,
                                          DoublePlus * soilDownCPrimeD,
                                          DoublePlus * soilUpCPrimeD,
                                          DoublePlus * Ka,
                                          DoublePlus * Kp,
                                          RWEarthQuakeDirection * EQDir,
                                          DoublePlus * kv,
                                          DoublePlus * kh,
                                          DoublePlus * Gk2,
                                          DoublePlus * Qk,
                                          DoublePlus * psi2Q ){
    // Azione inerziale su muro di sostegno e terreno e monte
    double moltIner = 1.0;
    if( EQDir->valueNormal() == EQup ){
        moltIner = 1.0 - kv->valueNormal();
    } else if( EQDir->valueNormal() == EQdown ){
        moltIner = 1.0 + kv->valueNormal();
    }

    // Calcola componente verticale forza agente
    if( VEd != NULL ){
        if( VEd->readOnly() ){
            double V = 0.0;
            V += toeGammaW->valueNormal() * m_d->toe->A->valueNormal() * toeD->valueNormal();
            V += heelGammaW->valueNormal() * m_d->heel->A->valueNormal() * heelD->valueNormal();
            V += stemGammaW->valueNormal() * m_d->stem->A->valueNormal() * stemD->valueNormal();
            V += keyGammaW->valueNormal() * m_d->key->A->valueNormal() * keyD->valueNormal();
            V += counterfortGammaW->valueNormal() * m_d->counterfort->A->valueNormal() * counterfortT->valueNormal();
            // Peso proprio terreno sopra mensola di monte
            // si assume la larghezza del paramento di monte
            if( m_d->soilUp != NULL ){
                V += m_d->soilUp->gammaW->valueNormal() * m_d->sectionSoilUp->A->valueNormal() * stemD->valueNormal();
            }
            V *= moltIner;

            VEd->setValueNormal( V );
        }
    }


    // Spinta passiva terreno
    if( HRdP ){
        if( HRdP->readOnly() ){
            double H = 0.0;
            if( m_d->soilDown != NULL ){
                double zDown = m_d->toe->pointsModel()->P(3)->z->valueNormal();
                if( zDown > soilDownZ->valueNormal() ){
                    zDown = soilDownZ->valueNormal();
                }
                if( zDown > m_d->key->pointsModel()->P(1)->z->valueNormal() ){
                    zDown = m_d->key->pointsModel()->P(1)->z->valueNormal();
                }
                double h = soilDownZ->valueNormal() - zDown;
                H += keyD->valueNormal() * \
                        0.50 *  \
                        Kp->valueNormal() * m_d->soilDown->gammaW->valueNormal() * moltIner * \
                        pow(h, 2.0);
                H += keyD->valueNormal() * \
                        soilDownCPrimeD->valueNormal() * sqrt(fabs(Kp->valueNormal())) * \
                        h;
            }
            HRdP->setValueNormal(H);
        }
    }

    // Calcola spinta attiva
    if( HEd != NULL ){
        if( HEd->readOnly() ){

            // Azione sismica su peso proprio muro di sostegno
            double H = toeGammaW->valueNormal() * m_d->toe->A->valueNormal() * toeD->valueNormal();
            H += heelGammaW->valueNormal() * m_d->heel->A->valueNormal() * heelD->valueNormal();
            H += stemGammaW->valueNormal() * m_d->stem->A->valueNormal() * stemD->valueNormal();
            H += keyGammaW->valueNormal() * m_d->key->A->valueNormal() * keyD->valueNormal();
            H += counterfortGammaW->valueNormal() * m_d->counterfort->A->valueNormal() * counterfortT->valueNormal();
            // MRd - Peso proprio terreno sopra mensola di monte
            // si assume la larghezza del paramento di monte
            if( m_d->soilUp != NULL ){
                H += m_d->soilUp->gammaW->valueNormal() * m_d->sectionSoilUp->A->valueNormal() * stemD->valueNormal();
            }
            H *= kh->valueNormal();

            // parametri geometrici per calcolo spinta
            double zUp = m_d->sectionSoilUp->pointsModel()->P(2)->z->valueNormal();
            double zDown =  m_d->heel->pointsModel()->P(1)->z->valueNormal();
            if( zDown >  m_d->key->pointsModel()->P(2)->z->valueNormal() ){
                zDown =  m_d->key->pointsModel()->P(2)->z->valueNormal();
            }
            // Spinta attiva terreno
            double h = zUp - zDown;
            if( m_d->soilUp ){
                // contributo angolo attrito interno
                H += stemD->valueNormal() * \
                        0.50 * Ka->valueNormal() * m_d->soilUp->gammaW->valueNormal() * moltIner * \
                        pow(h,2.0);
                // contributo coesione
                H -= stemD->valueNormal() * \
                        soilUpCPrimeD->valueNormal() * sqrt(fabs(Ka->valueNormal()) ) * h;
            }
            // Spinta carichi esterni
            H += Ka->valueNormal() * stemD->valueNormal() * Gk2->valueNormal() * h;
            H += psi2Q->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Qk->valueNormal() * h;

            HEd->setValueNormal( H );
        }
    }
}

void RetainingWall::calculateEQQLimVHM( DoublePlus * VEd,
                                        DoublePlus * HEd,
                                        Point2DPlus * MdC,
                                        DoublePlus * MEd,
                                        DoublePlus * soilDownCPrimeD,
                                        DoublePlus * soilUpCPrimeD,
                                        DoublePlus * Ka,
                                        BoolPlus * applyKp,
                                        DoublePlus * Kp,
                                        DoublePlus * HRdPMax,
                                        DoublePlus * HRdP,
                                        RWEarthQuakeDirection * EQDir,
                                        DoublePlus * kv,
                                        DoublePlus * kh,
                                        DoublePlus * Gk2,
                                        DoublePlus * Qk,
                                        DoublePlus * psi2Q ){
    // Azione inerziale su muro di sostegno e terreno e monte
    double moltIner = 1.0;
    if( EQDir->valueNormal() == EQup ){
        moltIner = 1.0 - kv->valueNormal();
    } else if( EQDir->valueNormal() == EQdown ){
        moltIner = 1.0 + kv->valueNormal();
    }

    // Calcola componente verticale forza agente
    if( VEd != NULL ){
        if( VEd->readOnly() ){
            double V = 0.0;
            V += toeGammaW->valueNormal() * m_d->toe->A->valueNormal() * toeD->valueNormal();
            V += heelGammaW->valueNormal() * m_d->heel->A->valueNormal() * heelD->valueNormal();
            V += stemGammaW->valueNormal() * m_d->stem->A->valueNormal() * stemD->valueNormal();
            V += keyGammaW->valueNormal() * m_d->key->A->valueNormal() * keyD->valueNormal();
            V += counterfortGammaW->valueNormal() * m_d->counterfort->A->valueNormal() * counterfortT->valueNormal();
            // Peso proprio terreno sopra mensola di monte
            // si assume la larghezza del paramento di monte
            if( m_d->soilUp != NULL ){
                V += m_d->soilUp->gammaW->valueNormal() * m_d->sectionSoilUp->A->valueNormal() * stemD->valueNormal();
            }
            V *= moltIner;

            VEd->setValueNormal( V );
        }
    }

    if( HRdPMax ){
        if( HRdPMax->readOnly() && m_d->soilDown != NULL ){
            double H = 0.0;
            double zDown = m_d->toe->pointsModel()->P(3)->z->valueNormal();
            if( zDown > soilDownZ->valueNormal() ){
                zDown = soilDownZ->valueNormal();
            }
            if( zDown > m_d->key->pointsModel()->P(1)->z->valueNormal() ){
                zDown = m_d->key->pointsModel()->P(1)->z->valueNormal();
            }
            double h = soilDownZ->valueNormal() - zDown;
            H += keyD->valueNormal() * \
                    0.50 *  \
                    Kp->valueNormal() * m_d->soilDown->gammaW->valueNormal() * moltIner * \
                    pow(h, 2.0);
            H += keyD->valueNormal() * \
                    soilDownCPrimeD->valueNormal() * sqrt(fabs(Kp->valueNormal())) * \
                    h;
            HRdPMax->setValueNormal( H );
        }
    }

    // Calcola spinta attiva
    if( HEd != NULL ){
        if( HEd->readOnly() ){

            // Azione sismica su peso proprio muro di sostegno
            double H = toeGammaW->valueNormal() * m_d->toe->A->valueNormal() * toeD->valueNormal();
            H += heelGammaW->valueNormal() * m_d->heel->A->valueNormal() * heelD->valueNormal();
            H += stemGammaW->valueNormal() * m_d->stem->A->valueNormal() * stemD->valueNormal();
            H += keyGammaW->valueNormal() * m_d->key->A->valueNormal() * keyD->valueNormal();
            H += counterfortGammaW->valueNormal() * m_d->counterfort->A->valueNormal() * counterfortT->valueNormal();
            // MRd - Peso proprio terreno sopra mensola di monte
            // si assume la larghezza del paramento di monte
            if( m_d->soilUp != NULL ){
                H += m_d->soilUp->gammaW->valueNormal() * m_d->sectionSoilUp->A->valueNormal() * stemD->valueNormal();
            }
            H *= kh->valueNormal();

            // parametri geometrici per calcolo spinta
            double zUp = m_d->sectionSoilUp->pointsModel()->P(2)->z->valueNormal();
            double zDown =  m_d->heel->pointsModel()->P(1)->z->valueNormal();
            if( zDown >  m_d->key->pointsModel()->P(2)->z->valueNormal() ){
                zDown =  m_d->key->pointsModel()->P(2)->z->valueNormal();
            }
            // Spinta attiva terreno
            double h = zUp - zDown;
            if( m_d->soilUp ){
                // contributo angolo attrito interno
                H += stemD->valueNormal() * \
                        0.50 * Ka->valueNormal() * m_d->soilUp->gammaW->valueNormal() * moltIner * \
                        pow(h,2.0);
                // contributo coesione
                H -= stemD->valueNormal() * \
                        soilUpCPrimeD->valueNormal() * sqrt(fabs(Ka->valueNormal()) ) * h;
            }
            // Spinta carichi esterni
            H += Ka->valueNormal() * stemD->valueNormal() * Gk2->valueNormal() * h;
            H += psi2Q->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Qk->valueNormal() * h;

            // Spinta passiva terreno
            if( HRdPMax != NULL && HRdP != NULL ){
                if( H < HRdPMax->valueNormal() ){
                    HRdP->setValueNormal( H );
                } else {
                    HRdP->setValueNormal( HRdPMax->valueNormal() );
                }
            }
            if( applyKp->valueNormal() && HRdP != NULL ){
                H -= HRdP->valueNormal();
            }

            HEd->setValueNormal( H );
        }
    }

    // ********** Calcola il polo rispetto a cui si calcolano i momenti *************
    if( MdC != NULL ){
        if( MdC->y->readOnly() ){
            MdC->y->setValueNormal( 0.50 * (m_d->heel->pointsModel()->P(1)->y->valueNormal() + m_d->toe->pointsModel()->P(3)->y->valueNormal()) );
        }
        if( MdC->z->readOnly() ){
            MdC->z->setValueNormal( 0.50 * (m_d->heel->pointsModel()->P(1)->z->valueNormal() + m_d->toe->pointsModel()->P(3)->z->valueNormal()) );
        }
    }

    // ********* Momento stabilizzante ***********

    if( MEd != NULL && MdC != NULL ){
        if( MEd->readOnly() ){
            // ******** Calcola il momento resistente ***********

            // MRd - Peso proprio muro di sostegno
            double Mrd = 0.0;
            Mrd += toeGammaW->valueNormal() * ( m_d->toe->Sz->valueNormal() + m_d->toe->A->valueNormal() * ( - MdC->y->valueNormal()) ) * toeD->valueNormal();
            Mrd += heelGammaW->valueNormal() * ( m_d->heel->Sz->valueNormal() + m_d->heel->A->valueNormal() * ( - MdC->y->valueNormal()) ) * heelD->valueNormal();
            Mrd += stemGammaW->valueNormal() * ( m_d->stem->Sz->valueNormal() + m_d->stem->A->valueNormal() * ( - MdC->y->valueNormal()) ) * stemD->valueNormal();
            Mrd += keyGammaW->valueNormal() * ( m_d->key->Sz->valueNormal() + m_d->key->A->valueNormal() * ( - MdC->y->valueNormal()) ) * keyD->valueNormal();
            Mrd += counterfortGammaW->valueNormal() * ( m_d->counterfort->Sz->valueNormal() + m_d->counterfort->A->valueNormal() * ( - MdC->y->valueNormal()) ) * counterfortT->valueNormal();
            // MRd - Peso proprio terreno sopra mensola di monte
            // si assume la larghezza del paramento di monte
            if( m_d->soilUp != NULL ){
                Mrd += m_d->soilUp->gammaW->valueNormal() * ( m_d->sectionSoilUp->Sz->valueNormal()+ m_d->sectionSoilUp->A->valueNormal() * ( - MdC->y->valueNormal()) ) * stemD->valueNormal();
            }
            Mrd *= moltIner;

            double MRdPCalc = 0.0;
            double HRdPCalc = 0.0;
            if( m_d->soilDown != NULL ){
                // MRd - Spinta passiva su muro
                double zDown = m_d->toe->pointsModel()->P(3)->z->valueNormal();
                if( zDown > soilDownZ->valueNormal() ){
                    zDown = soilDownZ->valueNormal();
                }
                if( zDown > m_d->key->pointsModel()->P(1)->z->valueNormal() ){
                    zDown = m_d->key->pointsModel()->P(1)->z->valueNormal();
                }
                double h = soilDownZ->valueNormal() - zDown;
                MRdPCalc += keyD->valueNormal() * \
                        0.50 *  \
                        Kp->valueNormal() * m_d->soilDown->gammaW->valueNormal() * moltIner * \
                        pow(h, 2.0) * (zDown + h / 3.0 - MdC->z->valueNormal());
                HRdPCalc += keyD->valueNormal() * \
                        0.50 *  \
                        Kp->valueNormal() * m_d->soilDown->gammaW->valueNormal() * moltIner * \
                        pow(h, 2.0);

                MRdPCalc += keyD->valueNormal() * \
                        soilDownCPrimeD->valueNormal() * sqrt(fabs(Kp->valueNormal())) * \
                        h * (zDown + h / 2.0 - MdC->z->valueNormal());
                HRdPCalc += keyD->valueNormal() * \
                        soilDownCPrimeD->valueNormal() * sqrt(fabs(Kp->valueNormal())) * \
                        h;
            }
            if( applyKp->valueNormal() && HRdP != NULL && HRdPCalc != 0.0 ){
                if( HRdP->valueNormal() < HRdPCalc ){
                    // riduciamo proporzionalmente il momento dovuto alla spinta passiva
                    Mrd += MRdPCalc * HRdP->valueNormal() / HRdPCalc;
                } else {
                    Mrd += MRdPCalc;
                }
            }

            // ******** Momento destabilizzante **********

            // Azione sismica su peso proprio muro di sostegno
            double Med = toeGammaW->valueNormal() * ( m_d->toe->Sy->valueNormal() + m_d->toe->A->valueNormal() * ( - MdC->z->valueNormal()) ) * toeD->valueNormal();
            Med += heelGammaW->valueNormal() * ( m_d->heel->Sy->valueNormal() + m_d->heel->A->valueNormal() * ( - MdC->z->valueNormal()) ) * heelD->valueNormal();
            Med += stemGammaW->valueNormal() * ( m_d->stem->Sy->valueNormal() + m_d->stem->A->valueNormal() * ( - MdC->z->valueNormal()) ) * stemD->valueNormal();
            Med += keyGammaW->valueNormal() * ( m_d->key->Sy->valueNormal() + m_d->key->A->valueNormal() * ( - MdC->z->valueNormal()) ) * keyD->valueNormal();
            Med += counterfortGammaW->valueNormal() * ( m_d->counterfort->Sy->valueNormal() + m_d->counterfort->A->valueNormal() * ( - MdC->z->valueNormal()) ) * counterfortT->valueNormal();
            // MRd - Peso proprio terreno sopra mensola di monte
            // si assume la larghezza del paramento di monte
            if( m_d->soilUp != NULL ){
                Med += m_d->soilUp->gammaW->valueNormal() * ( m_d->sectionSoilUp->Sy->valueNormal()+ m_d->sectionSoilUp->A->valueNormal() * ( - MdC->z->valueNormal()) ) * stemD->valueNormal();
            }
            Med *= kh->valueNormal();

            // parametri geometrici per calcolo spinta
            double zUp = m_d->sectionSoilUp->pointsModel()->P(2)->z->valueNormal();
            double zDown =  m_d->heel->pointsModel()->P(1)->z->valueNormal();
            if( zDown >  m_d->key->pointsModel()->P(2)->z->valueNormal() ){
                zDown =  m_d->key->pointsModel()->P(2)->z->valueNormal();
            }
            // Spinta terreno
            double h = zUp - zDown;
            if( m_d->soilUp ){
                // contributo angolo attrito interno
                Med += stemD->valueNormal() * \
                        0.50 * Ka->valueNormal() * m_d->soilUp->gammaW->valueNormal() * moltIner * \
                        pow(h,2.0) * ( (zDown + h / 3.0) - MdC->z->valueNormal());
                // contributo coesione
                Med -= stemD->valueNormal() * \
                        soilUpCPrimeD->valueNormal() * sqrt(fabs(Ka->valueNormal()) ) * h * ( (zDown + h / 2.0) - MdC->z->valueNormal());
            }
            // MEd - Spinta carichi esterni
            Med += Ka->valueNormal() * stemD->valueNormal() * Gk2->valueNormal() * h * ((zDown + h/2.0) - MdC->z->valueNormal());
            Med += psi2Q->valueNormal() * Ka->valueNormal() * stemD->valueNormal() * Qk->valueNormal() * h * ((zDown + h/2.0) - MdC->z->valueNormal());

            // passiamo il valore calcolato
            MEd->setValueNormal( Med - Mrd );
        }
    }
}

void RetainingWall::calculateQLim( DoublePlus * qLim, DoublePlus * VRd,
                                   DoublePlus * phiD, DoublePlus * cd, DoublePlus * gammaW, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite, DoublePlus * D,
                                   DoublePlus * H, DoublePlus * V, DoublePlus * M, DoublePlus * q,
                                   DoublePlus * NGammaW, DoublePlus * sGammaW, DoublePlus * iGammaW, DoublePlus * bGammaW, DoublePlus * gGammaW,
                                   DoublePlus * Nc, DoublePlus * sc, DoublePlus * ic, DoublePlus * bc, DoublePlus * gc, DoublePlus * dc,
                                   DoublePlus * Nq, DoublePlus * sq, DoublePlus * iq, DoublePlus * bq, DoublePlus * gq, DoublePlus * dq,
                                   DoublePlus * gammaQLim ){
    if( m_d->soilDown ){
        if( B->readOnly() ){
            double Bd = toeB->valueNormal() + heelB->valueNormal() + stemTBottom->valueNormal();
            B->setValueNormal( m_d->soilDown->LPrimeNormal( Bd, M->valueNormal(), V->valueNormal() ) );
        }
        if( L->readOnly() ){
            double val = stemD->valueNormal();
            if( val < heelD->valueNormal() )
                val = heelD->valueNormal();
            if( val < toeD->valueNormal() )
                val = toeD->valueNormal();
            if( val < keyD->valueNormal() )
                val = keyD->valueNormal();
            L->setValueNormal( val );
        }
        if( D->readOnly() ){
            D->setValueNormal( soilDownZ->valueNormal() - 0.50 * toeB->valueNormal() * tan(baseAlpha->valueNormal() ));
        }
        if( NGammaW->readOnly() ){
            m_d->soilDown->NGammaWPrime( NGammaW, phiD );
        }
        if( sGammaW->readOnly() ){
            m_d->soilDown->sGammaWPrime( sGammaW, phiD, B, L, LInfinite );
        }
        if( iGammaW->readOnly() ){
            m_d->soilDown->iGammaWPrime( iGammaW, phiD, cd, V, H, B, L, LInfinite );
        }
        if( bGammaW->readOnly() ){
            m_d->soilDown->bGammaWPrime( bGammaW, phiD, baseAlpha );
        }
        if( gGammaW->readOnly() ){
            gGammaW->setValueNormal(1.0);
        }

        if( Nc->readOnly() ){
            m_d->soilDown->NcPrime( Nc, phiD );
        }
        if( sc->readOnly() ){
            m_d->soilDown->scPrime( sc, phiD, B, L, LInfinite );
        }
        if( ic->readOnly() ){
            m_d->soilDown->icPrime( ic, phiD, cd, V, H, B, L, LInfinite );
        }
        if( bc->readOnly() ){
            m_d->soilDown->bcPrime( bc, phiD, baseAlpha );
        }
        if( gc->readOnly() ){
            gc->setValueNormal(1.0);
        }
        if( dc->readOnly() ){
            m_d->soilDown->dcPrime( dc, phiD, D, B, L, LInfinite );
        }

        if( Nq->readOnly() ){
            m_d->soilDown->NqPrime( Nq, phiD );
        }
        if( sq->readOnly() ){
            m_d->soilDown->sqPrime( sq, phiD, B, L, LInfinite );
        }
        if( iq->readOnly() ){
            m_d->soilDown->iqPrime( iq, phiD, cd, V, H, B, L, LInfinite );
        }
        if( bq->readOnly() ){
            m_d->soilDown->bqPrime( bq, phiD, baseAlpha );
        }
        if( gq->readOnly() ){
            gq->setValueNormal( 1.0 );
        }
        if( dq->readOnly() ){
            m_d->soilDown->dcPrime( dq, phiD, D, B, L, LInfinite );
        }

        m_d->soilDown->qLimPrime( qLim, gammaW, cd, q, B, L, LInfinite,
                                  NGammaW, iGammaW, sGammaW, bGammaW, gGammaW,
                                  Nc, ic, sc, bc, gc, dc,
                                  Nq, iq, sq, bq, gq, dq );
        double gR = gammaQLim != NULL ? gammaQLim->valueNormal(): 1.0;
        if( gR != 0.0 ){
            VRd->setValueNormal( m_d->soilDown->QLimVRdPrimeNormal( qLim->valueNormal(), B->valueNormal(), L->valueNormal() ) / gR );
        } else {
            VRd->setValueNormal( 0.0 );
        }
    }
}

#include <QApplication>

class RWEarthQuakeDirectionPrivate{
    Q_DECLARE_TR_FUNCTIONS(RWEarthQuakeDirectionPrivate)

    class enumVal{
    public:
        enumVal(RetainingWall::EarthQuakeDirection v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        RetainingWall::EarthQuakeDirection val;
        QString normalStr;
        QString str;
    };

public:
    RWEarthQuakeDirectionPrivate(RetainingWall::EarthQuakeDirection v):
        value(v){
        enumList.append( enumVal( RetainingWall::EQup, "EQup", trUtf8("In alto")) );
        enumList.append( enumVal( RetainingWall::EQdown, "EQdown", trUtf8("In basso")) );
    };
    ~RWEarthQuakeDirectionPrivate(){
    };

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    RetainingWall::EarthQuakeDirection value;
    QList<enumVal> enumList;
};

RWEarthQuakeDirection::RWEarthQuakeDirection( RetainingWall::EarthQuakeDirection tt,
                                              const QString & nn,
                                              bool ro ):
    EnumPlus( nn, ro),
    m_d( new RWEarthQuakeDirectionPrivate(tt) ){
}

RWEarthQuakeDirection::~RWEarthQuakeDirection(){
    delete m_d;
}

RWEarthQuakeDirection & RWEarthQuakeDirection::operator=( RWEarthQuakeDirection & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & RWEarthQuakeDirection::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        RWEarthQuakeDirection * valCncrModelPlus = dynamic_cast<RWEarthQuakeDirection *>(&val);
        if(valCncrModelPlus)
        {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString RWEarthQuakeDirection::typeVar(){
    return QString("RWEarthQuakeDirection");
}

RetainingWall::EarthQuakeDirection RWEarthQuakeDirection::value(){
    return m_d->value;
}

RetainingWall::EarthQuakeDirection RWEarthQuakeDirection::valueNormal(){
    return m_d->value;
}

QString RWEarthQuakeDirection::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString RWEarthQuakeDirection::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void RWEarthQuakeDirection::setValueNormal( RetainingWall::EarthQuakeDirection vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void RWEarthQuakeDirection::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void RWEarthQuakeDirection::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void RWEarthQuakeDirection::setValue( RetainingWall::EarthQuakeDirection vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void RWEarthQuakeDirection::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int RWEarthQuakeDirection::enumCount() {
    return m_d->enumList.size();
}

QString RWEarthQuakeDirection::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int RWEarthQuakeDirection::valueNormalInt() {
    return m_d->valueIndex();
}
