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
#include "section.h"

#include "qstringplus.h"
#include "doubleplus.h"
#include "uintplus.h"
#include "intplus.h"
#include "boolplus.h"
#include "point2dplus.h"
#include "point2dmodel.h"
#include "sectionloadphasemodel.h"
#include "sectionloadphase.h"
#include "material.h"

#include <cmath>

#include <QXmlStreamAttributes>
#include <QString>
#include <QMap>
#include <QPolygonF>

Section::Section( UnitMeasure * ump, Material * mat, const QString & n, QObject * parent ):
    VarPlusContainerPlus( ump, "Section", n, parent ),
    m_material(  mat ),
    m_loadPhase(NULL){

    initVar();

    setMaterial( mat );
}

Section::Section(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent):
    VarPlusContainerPlus( ump, "Section", trUtf8("Sezione"), parent ),
    m_material( NULL ),
    m_loadPhase( NULL ){
    initVar();
    loadFromXML( attrs, mModel, lModel );
}

Section::~Section() {
    if( m_material != NULL ){
        m_material->removeIsUsedBy( this );
    }
    VarPlus::deleteVarPlus( typeName );
}

void Section::loadFromXML( const QXmlStreamAttributes &attrs, MaterialModel * mModel, SectionLoadPhaseModel * loadModel ){
    for ( int i=0; i<attrs.count(); ++i ) {
        for( QMap<QString, VarPlus *>::iterator iter = m_varContainer->begin(); iter != m_varContainer->end(); ++iter ){
            if ( attrs.at(i).name().toString().toUpper() == (*iter)->name().toUpper() ) {
                (*iter)->setValueNormal( attrs.at(i).value().toString(), false );
            } else if ( attrs.at(i).name().toString().toUpper() == "MATERIAL" ) {
                if( mModel != NULL ){
                    setMaterial( mModel->materialId( attrs.at(i).value().toString().toUInt() ), false );
                }
            } else if ( attrs.at(i).name().toString().toUpper() == "LOADPHASE" ) {
                if( loadModel != NULL ){
                    setLoadPhase( loadModel->loadPhaseId( attrs.at(i).value().toString().toUInt() ), false );
                }
            }
        }
    }
    updateReadOnlyVars();
}

Section & Section::operator=( Section & val){
    if( this != &val ){
        m_material = val.m_material;
        VarPlusContainer::operator=(val);
    }
    return *this;
}

void Section::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement(*m_typeNameInternal);
    for( QMap<QString, VarPlus *>::iterator i = m_varContainer->begin(); i != m_varContainer->end(); ++i) {
        writer->writeAttribute( (*i)->xmlAttribute() );
    }
    if( m_material ){
        writer->writeAttribute( "material", m_material->id->valueNormalStr() );
    }
    if( m_loadPhase ){
        writer->writeAttribute( "loadPhase", m_loadPhase->id->valueNormalStr() );
    }
    writer->writeEndElement();
}

void Section::readXml(QXmlStreamReader *reader) {
    Q_UNUSED(reader);
}

Material * Section::material(){
    return m_material;
}

void Section::setMaterial( Material * mat, bool emitAuto ){
    if( m_material != mat ){
        if( m_material ){
            disconnect( m_material, &Material::materialChanged, this, &Section::sectionChanged );
            disconnect( m_material, &Material::aboutToBeDeleted, this, &Section::setMaterialNULL );
            pW->removeConnectedVar( m_material->gammaW );
            m_material->removeIsUsedBy( this );
        }

        Material * oldMaterial = m_material;
        m_material = mat;

        if( m_material ){
            m_material->setIsUsedBy( this );
            pW->addConnectedVar( m_material->gammaW );
            connect( m_material, &Material::aboutToBeDeleted, this, &Section::setMaterialNULL );
            connect( m_material, &Material::materialChanged, this, &Section::sectionChanged );
        }
        setPW( emitAuto );

        if( emitAuto ){
            emit materialChanged( oldMaterial, m_material );
        }
        updateReadOnlyVars();
    }
}

SectionLoadPhase *Section::loadPhase() {
    return m_loadPhase;
}

void Section::setMaterialNULL(){
    Material * mat = dynamic_cast<Material *>( sender() );
    if( mat == m_material ){
        setMaterial( NULL, true );
    }
}

void Section::setLoadPhase( SectionLoadPhase * lp, bool emitAuto ){
    if( m_loadPhase != lp ) {
        if( m_loadPhase != NULL ){
            disconnect( m_loadPhase, &SectionLoadPhase::aboutToBeDeleted, this, &Section::setLoadPhaseNULL );
        }

        m_loadPhase = lp;

        if( m_loadPhase != NULL ){
            connect( m_loadPhase, &SectionLoadPhase::aboutToBeDeleted, this, &Section::setLoadPhaseNULL );
        }

        if( emitAuto ){
            emit loadPhaseChanged();
        }
    }
}

void Section::setLoadPhaseNULL(){
    SectionLoadPhase * lp = dynamic_cast<SectionLoadPhase *>( sender() );
    if( lp == m_loadPhase ){
        setLoadPhase( NULL, true );
    }
}

void Section::translate( Point2DPlus * p ){
    translateNormal( p->y->valueNormal(), p->z->valueNormal());
}

void Section::translateNormal(double yTrans, double zTrans, bool suspSignals){
    if( suspSignals ){
        suspendSignals( true );
    }

    double SyTmp = SyTransRotNormal( yTrans, zTrans, 0.0 );
    double SzTmp = SzTransRotNormal( yTrans, zTrans, 0.0 );
    double IyTmp = IyyTransRotNormal( yTrans, zTrans, 0.0 );
    double IzTmp = IzzTransRotNormal( yTrans, zTrans, 0.0 );
    double IyzTmp = IyzTransRotNormal( yTrans, zTrans, 0.0 );
    double Sy3DyDzTmp = Sy3DyDzTransRotNormal( yTrans, zTrans, 0.0 );
    double Sz3DyDzTmp = Sz3DyDzTransRotNormal( yTrans, zTrans, 0.0 );
    double Sy2zDyDzTmp = Sy2zDyDzTransRotNormal( yTrans, zTrans, 0.0 );
    double Syz2DyDzTmp = Syz2DyDzTransRotNormal( yTrans, zTrans, 0.0 );

    Sy->setValueNormal( SyTmp );
    Sz->setValueNormal( SzTmp );
    Iyy->setValueNormal( IyTmp );
    Izz->setValueNormal( IzTmp );
    Iyz->setValueNormal( IyzTmp );
    Sy3DyDz->setValueNormal( Sy3DyDzTmp );
    Sz3DyDz->setValueNormal( Sz3DyDzTmp );
    Sy2zDyDz->setValueNormal( Sy2zDyDzTmp );
    Syz2DyDz->setValueNormal( Syz2DyDzTmp );

    if( suspSignals ){
        suspendSignals( false );
    }
}

void Section::rotate( DoublePlus * rot ){
    rotateNormal( rot->valueNormal() );
}


void Section::rotateNormal( double rot, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }

    double SyTmp = SyTransRotNormal( 0.0, 0.0, rot );
    double SzTmp = SzTransRotNormal( 0.0, 0.0, rot );
    double IyTmp = IyyTransRotNormal( 0.0, 0.0, rot );
    double IzTmp = IzzTransRotNormal( 0.0, 0.0, rot );
    double IyzTmp = IyzTransRotNormal( 0.0, 0.0, rot );
    double Sy3DyDzTmp = Sy3DyDzTransRotNormal( 0.0, 0.0, rot );
    double Sz3DyDzTmp = Sz3DyDzTransRotNormal( 0.0, 0.0, rot );
    double Sy2zDyDzTmp = Sy2zDyDzTransRotNormal( 0.0, 0.0, rot );
    double Syz2DyDzTmp = Syz2DyDzTransRotNormal( 0.0, 0.0, rot );

    Sy->setValueNormal( SyTmp );
    Sz->setValueNormal( SzTmp );
    Iyy->setValueNormal( IyTmp );
    Izz->setValueNormal( IzTmp );
    Iyz->setValueNormal( IyzTmp );
    Sy3DyDz->setValueNormal( Sy3DyDzTmp );
    Sz3DyDz->setValueNormal( Sz3DyDzTmp );
    Sy2zDyDz->setValueNormal( Sy2zDyDzTmp );
    Syz2DyDz->setValueNormal( Syz2DyDzTmp );

    if( suspSignals ){
        suspendSignals( false );
    }
}

SectionSpace::SectionType Section::sectionType() {
    return SectionSpace::GenericSection;
}

void Section::initVar(){
    connect( this, &Section::materialChanged, this, &Section::sectionChanged );
    connect( this, &Section::containerChanged, this, &Section::sectionChanged );

    typeName = new QStringPlus( trUtf8("Sezione"), "typeName", true );
    typeName->setRichName( trUtf8("Tipo") );

    applyRoundOff = new BoolPlus( true, "applyRoundOff" );
    applyRoundOff->setToolTip( trUtf8("Applica l'arrotondamento"));
    applyRoundOff->setRichName( trUtf8("Arrotondamento"));
    addVarToContainer( applyRoundOff );

    A = new DoublePlus(0.0, "A", m_unitMeasure, UnitMeasure::sectL2 );
    A->setToolTip( trUtf8("Area della sezione"));
    A->setRichName( trUtf8("A"));
    addVarToContainer( A );
    A->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setA) );

    pW = new DoublePlus(0.0, "pW", m_unitMeasure, UnitMeasure::loadFLine, true );
    pW->setToolTip( trUtf8("Peso unitario travi"));
    pW->setRichName( trUtf8("p<span style=\"vertical-align:sub;\">W</span>"));
    addVarToContainer( pW );
    pW->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setPW) );
    pW->addConnectedVar( A );

    Sy = new DoublePlus( 0.0, "Sy", m_unitMeasure, UnitMeasure::sectL3 );
    Sy->setToolTip( trUtf8("Momento statico rispetto all'asse y"));
    Sy->setRichName( trUtf8("S<span style=\"vertical-align:sub;\">y</span>") );
    addVarToContainer( Sy );
    Sy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setSy) );

    Sz = new DoublePlus( 0.0, "Sz", m_unitMeasure, UnitMeasure::sectL3 );
    Sz->setToolTip( trUtf8("Momento statico rispetto all'asse z"));
    Sz->setRichName( trUtf8("S<span style=\"vertical-align:sub;\">z</span>") );
    addVarToContainer( Sz );
    Sz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setSz) );

    Iyy = new DoublePlus( 0.0, "Iyy", m_unitMeasure, UnitMeasure::sectL4 );
    Iyy->setToolTip( trUtf8("Momento di inerzia rispetto all'asse y"));
    Iyy->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">y</span>") );
    addVarToContainer( Iyy );
    Iyy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setIyy) );

    Izz = new DoublePlus( 0.0, "Izz", m_unitMeasure, UnitMeasure::sectL4 );
    Izz->setToolTip( trUtf8("Momento di inerzia rispetto all'asse z"));
    Izz->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">z</span>") );
    addVarToContainer( Izz );
    Izz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setIzz) );

    Ixx = new DoublePlus( 0.0, "Ixx", m_unitMeasure, UnitMeasure::sectL4, true );
    Ixx->setToolTip( trUtf8("Momento di inerzia rispetto all'asse x (e.g. momento di inerzia polare rispetto all'origine')"));
    Ixx->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">x</span>") );
    addVarToContainer( Ixx );
    Ixx->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setIxx) );
    Ixx->addConnectedVars( 2, Iyy, Izz );

    Iyz = new DoublePlus( 0.0, "Iyz", m_unitMeasure, UnitMeasure::sectL4 );
    Iyz->setToolTip( trUtf8("Momento centrifugo rispetto agli assi y e z"));
    Iyz->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">yz</span>") );
    addVarToContainer( Iyz );
    Iyz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setIyz) );

    Sy3DyDz = new DoublePlus( 0.0, "Sy3DyDz", m_unitMeasure, UnitMeasure::sectL5 );
    Sy3DyDz->setToolTip( trUtf8("Integrale di y^3 sulla superficie della sezione"));
    Sy3DyDz->setRichName( trUtf8("∬ y<span style=\" vertical-align:super;\">3</span> dA") );
    addVarToContainer( Sy3DyDz );
    Sy3DyDz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setSy3DyDz) );

    Sz3DyDz = new DoublePlus( 0.0, "Sz3DyDz", m_unitMeasure, UnitMeasure::sectL5 );
    Sz3DyDz->setToolTip( trUtf8("Integrale di z^3 sulla superficie della sezione"));
    Sz3DyDz->setRichName( trUtf8("∬ z<span style=\" vertical-align:super;\">3</span> dA") );
    addVarToContainer( Sz3DyDz );
    Sz3DyDz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setSz3DyDz) );

    Sy2zDyDz = new DoublePlus( 0.0, "Sy2zDyDz", m_unitMeasure, UnitMeasure::sectL5 );
    Sy2zDyDz->setToolTip( trUtf8("Integrale di y^2*z sulla superficie della sezione"));
    Sy2zDyDz->setRichName( trUtf8("∬ y<span style=\" vertical-align:super;\">2</span>z dA") );
    addVarToContainer( Sy2zDyDz );
    Sy2zDyDz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setSy2zDyDz) );

    Syz2DyDz = new DoublePlus( 0.0, "Syz2DyDz", m_unitMeasure, UnitMeasure::sectL5 );
    Syz2DyDz->setToolTip( trUtf8("Integrale di y*z^2 sulla superficie della sezione"));
    Syz2DyDz->setRichName( trUtf8("∬ yz<span style=\" vertical-align:super;\">2</span> dA") );
    addVarToContainer( Syz2DyDz );
    Syz2DyDz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setSyz2DyDz) );

    G = new Point2DPlus("G", "yG", 0.0, "zG", 0.0, m_unitMeasure, UnitMeasure::sectL, true );
    G->setRichName(trUtf8("Coordinate del baricentro della sezione"));
    G->y->setToolTip( trUtf8("Ascissa del baricentro della sezione"));
    G->z->setToolTip( trUtf8("Ordinata del baricentro della sezione"));
    G->setRichName( "G");
    G->y->setRichName( trUtf8("y<span style=\"vertical-align:sub;\">G</span>") );
    G->z->setRichName( trUtf8("z<span style=\"vertical-align:sub;\">G</span>") );
    addVarToContainer( G );
    G->y->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setGy) );
    G->y->addConnectedVars( 2, A, Sz );
    G->z->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setGz) );
    G->z->addConnectedVars( 2, A, Sy );

    IGyy = new DoublePlus( 0.0, "IGy", m_unitMeasure, UnitMeasure::sectL4, true );
    IGyy->setToolTip( trUtf8("Momento di inerzia rispetto all'asse y, nel baricentro"));
    IGyy->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">G,yy</span>") );
    addVarToContainer( IGyy );
    IGyy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setIGyy) );
    IGyy->addConnectedVars(6, A, Sy, Sz, Iyy, Izz, Iyz );

    IGzz = new DoublePlus( 0.0, "IGzz", m_unitMeasure, UnitMeasure::sectL4, true );
    IGzz->setToolTip( trUtf8("Momento di inerzia rispetto all'asse z nel baricentro"));
    IGzz->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">G,zz</span>") );
    addVarToContainer( IGzz );
    IGzz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setIGzz) );
    IGzz->addConnectedVars(6, A, Sy, Sz, Iyy, Izz, Iyz );

    IGyz = new DoublePlus( 0.0, "IGyz", m_unitMeasure, UnitMeasure::sectL4, true );
    IGyz->setToolTip( trUtf8("Momento di inerzia centrifugo rispetto agli assi y e z, nel baricentro"));
    IGyz->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">G,yz</span>") );
    addVarToContainer( IGyz );
    IGyz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setIGyz) );
    IGyz->addConnectedVars(6, A, Sy, Sz, Iyy, Izz, Iyz );

    IGxx = new DoublePlus( 0.0, "IGxx", m_unitMeasure, UnitMeasure::sectL4, true );
    IGxx->setToolTip( trUtf8("Momento di inerzia baricentrico rispetto all'asse x (e.g. momento di inerzia polare rispetto al baricentro)"));
    IGxx->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">G,xx</span>") );
    addVarToContainer( IGxx );
    IGxx->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setIGxx) );
    IGxx->addConnectedVars(2, IGyy, IGzz );

    thetaC = new DoublePlus( 0.0, "thetaC", m_unitMeasure, UnitMeasure::angle, true );
    thetaC->setToolTip( trUtf8("Angolo di inclinazione dell'asse centrale di inerzia Y rispetto all'asse y"));
    thetaC->setRichName( trUtf8("θ<span style=\" vertical-align:sub;\">G,Y</span>"));
    addVarToContainer( thetaC );
    thetaC->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setThetaC) );
    thetaC->addConnectedVars(3, IGyy, IGzz, IGyz );

    ICyy = new DoublePlus( 0.0, "ICyy", m_unitMeasure, UnitMeasure::sectL4, true );
    ICyy->setToolTip( trUtf8("Momento centrale di inerzia - asse Y"));
    ICyy->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">C,YY</span>") );
    addVarToContainer( ICyy );
    ICyy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setICyy) );
    ICyy->addConnectedVars(3, IGyy, IGzz, IGyz );

    ICzz = new DoublePlus( 0.0, "ICzz", m_unitMeasure, UnitMeasure::sectL4, true );
    ICzz->setToolTip( trUtf8("Momento centrale di inerzia - asse Z"));
    ICzz->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">C,ZZ</span>") );
    addVarToContainer( ICzz );
    ICzz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setICzz) );
    ICzz->addConnectedVars(3, IGyy, IGzz, IGyz );

    ICyz = new DoublePlus( 0.0, "ICyz", m_unitMeasure, UnitMeasure::sectL4, true );
    ICyz->setToolTip( trUtf8("Momento centrifugo rispetto agli assi centrali di inerzia (dovrebbe essere sempre nullo, lo calcoliamo per controllo)"));
    ICyz->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">C,YZ</span>") );
    addVarToContainer( ICyz );
    ICyz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setICyz) );
    ICyz->addConnectedVars( 7, G, A, Sy, Sz, Iyy, Izz, Iyz );

    iy = new DoublePlus(0.0, "iy", m_unitMeasure, UnitMeasure::sectL, true );
    iy->setToolTip( trUtf8("Raggio di inerzia, asse y"));
    iy->setRichName( trUtf8("i<span style=\" vertical-align:sub;\">y</span>"));
    addVarToContainer( iy );
    iy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setiy) );
    iy->addConnectedVars( 2, A, Iyy );

    iz = new DoublePlus(0.0, "iz", m_unitMeasure, UnitMeasure::sectL, true );
    iz->setToolTip( trUtf8("Raggio di inerzia, asse z"));
    iz->setRichName( trUtf8("i<span style=\" vertical-align:sub;\">z</span>"));
    addVarToContainer( iz );
    iz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setiz) );
    iz->addConnectedVars( 2, A, Izz );

    ty = new DoublePlus(1.2, "ty", m_unitMeasure, UnitMeasure::noDimension, false );
    ty->setToolTip( trUtf8("Fattore di taglio, asse y"));
    ty->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">y</span>"));
    addVarToContainer( ty );
    ty->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setty) );

    tz = new DoublePlus(1.2, "tz", m_unitMeasure, UnitMeasure::noDimension, false );
    tz->setToolTip( trUtf8("Fattore di taglio, asse z"));
    tz->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">z</span>"));
    addVarToContainer( tz );
    tz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::settz) );

    tyz = new DoublePlus(0.0, "tyz", m_unitMeasure, UnitMeasure::noDimension, false );
    tyz->setToolTip( trUtf8("Fattore di taglio misto, assi y e z"));
    tyz->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">yz</span>"));
    addVarToContainer( tyz );
    tyz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::settyz) );

    IT = new DoublePlus(0.0, "IT", m_unitMeasure, UnitMeasure::sectL4, false );
    IT->setToolTip( trUtf8("Fattore di rigidezza torsionale"));
    IT->setRichName( trUtf8("I<span style=\" vertical-align:sub;\">T</span>"));
    addVarToContainer( IT );
    IT->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Section::setIT) );

    updateReadOnlyVars();
    updateQGraphics();
}

void Section::updateQGraphics(){
}

int Section::internalLoadPhase() {
    if( m_loadPhase != NULL ) {
        return m_loadPhase->phaseNum();
    }
    return -1;
}

void Section::NMSLSNormal( double * NRet, double *MyRet, double * MzRet,
                          double l, double my, double mz, QList<Point2DModel *> * sects ){
    Q_UNUSED(sects);
    *NRet = 0.0;
    *MyRet = 0.0;
    *MzRet = 0.0;
    if( m_material ){
        *NRet = m_material->E->valueNormal() * ( l* A->valueNormal() + my * Sy->valueNormal() + mz * Sz->valueNormal());
        *MyRet = m_material->E->valueNormal() * (l * Sy->valueNormal() + my * Iyy->valueNormal() + mz * Iyz->valueNormal());
        *MzRet =  - (m_material->E->valueNormal() * (l * Sz->valueNormal() + my * Iyz->valueNormal() + mz * Izz->valueNormal()));
    }
}

double Section::NULSNormal( double l, double my, double mz, QList<Point2DModel *> * sects ){
    Q_UNUSED(sects);
    if( m_material ){
        return m_material->E->valueNormal() * ( l* A->valueNormal() + my * Sy->valueNormal() + mz * Sz->valueNormal());
    }
    return 0.0;
}

void Section::MULSNormal( double *yRet, double * zRet,
                          double l, double my, double mz, QList<Point2DModel *> * sects ){
    Q_UNUSED(sects);
    *yRet = 0.0;
    *zRet = 0.0;
    if( m_material ){
        *yRet = m_material->E->valueNormal() * (l * Sy->valueNormal() + my * Iyy->valueNormal() + mz * Iyz->valueNormal());
        *zRet =  - (m_material->E->valueNormal() * (l * Sz->valueNormal() + my * Iyz->valueNormal() + mz * Izz->valueNormal()));
    }
}

QPolygonF Section::MULSyMULSz( DoublePlus * NSd, Point2DPlus * cen, int propNP){
    Q_UNUSED(NSd);
    Q_UNUSED(cen);
    Q_UNUSED(propNP);
    return QPolygonF();
}

QPolygonF Section::MULSN( Point2DPlus * cen, DoublePlus * rot, int propNP){
    Q_UNUSED(cen);
    Q_UNUSED(rot);
    Q_UNUSED(propNP);
    return QPolygonF();
}

QGraphicsItem * Section::qGraphicsItem(){
    return NULL;
}

double Section::SyTransRotNormal( double yTrans, double zTrans, double a){
    double SyPrime = Sy->valueNormal() - zTrans * A->valueNormal();
    double SzPrime = Sz->valueNormal() - yTrans * A->valueNormal();
    double cosa = cos(a);
    double sina = sin(a);

    double ret = cosa * SyPrime - sina * SzPrime;
    if( applyRoundOff->valueNormal() ){
        ret = MRoundOff( ret, 3.0 );
    }
    return ret;
}

double Section::SzTransRotNormal( double yTrans, double zTrans, double a){
    double SyPrime = Sy->valueNormal() - zTrans * A->valueNormal();
    double SzPrime = Sz->valueNormal() - yTrans * A->valueNormal();
    double cosa = cos(a);
    double sina = sin(a);

    double ret = sina* SyPrime + cosa * SzPrime;

    if( applyRoundOff->valueNormal() ){
        ret = MRoundOff( ret, 3.0 );
    }
    return ret;
}

double Section::IyyTransRotNormal( double yTrans, double zTrans, double rot){
    double IyPrime = Iyy->valueNormal() - 2.0 * zTrans * Sy->valueNormal() + pow(zTrans, 2.0) * A->valueNormal();
    double IzPrime = Izz->valueNormal() - 2.0 * yTrans * Sz->valueNormal() + pow(yTrans, 2.0) * A->valueNormal();
    double IyzPrime = Iyz->valueNormal() - zTrans * Sz->valueNormal() - yTrans * Sy->valueNormal() + yTrans * zTrans * A->valueNormal();
    double cos2a = cos(2.0*rot);
    double sin2a = sin(2.0*rot);

    double ret = 0.50*(IyPrime+IzPrime) + cos2a * 0.50*(IyPrime-IzPrime) - sin2a * IyzPrime;

    if( applyRoundOff->valueNormal() ){
        ret = MRoundOff( ret, 4.0 );
    }
    return ret;
}


double Section::IzzTransRotNormal( double yTrans, double zTrans, double rot){
    double IyPrime = Iyy->valueNormal() - 2.0 * zTrans * Sy->valueNormal() + pow(zTrans, 2.0) * A->valueNormal();
    double IzPrime = Izz->valueNormal() - 2.0 * yTrans * Sz->valueNormal() + pow(yTrans, 2.0) * A->valueNormal();
    double IyzPrime = Iyz->valueNormal() - zTrans * Sz->valueNormal() - yTrans * Sy->valueNormal() + yTrans * zTrans * A->valueNormal();
    double cos2a = cos(2.0*rot);
    double sin2a = sin(2.0*rot);

    double ret = 0.50*(IyPrime+IzPrime) - 0.50*cos2a*(IyPrime-IzPrime) + sin2a * IyzPrime;

    if( applyRoundOff->valueNormal() ){
        ret = MRoundOff( ret, 4.0 );
    }
    return ret;
}

double Section::IyzTransRotNormal( double yTrans, double zTrans, double rot){
    double IyPrime = Iyy->valueNormal() - 2.0 * zTrans * Sy->valueNormal() + pow(zTrans, 2.0) * A->valueNormal();
    double IzPrime = Izz->valueNormal() - 2.0 * yTrans * Sz->valueNormal() + pow(yTrans, 2.0) * A->valueNormal();
    double IyzPrime = Iyz->valueNormal() - zTrans * Sz->valueNormal() - yTrans * Sy->valueNormal() + yTrans * zTrans * A->valueNormal();
    double cos2a = cos(2.0*rot);
    double sin2a = sin(2.0*rot);

    double ret = 0.50 * sin2a * (IyPrime-IzPrime) + cos2a * IyzPrime;

    if( applyRoundOff->valueNormal() ){
        ret = MRoundOff( ret, 4.0 );
    }
    return ret;
}

double Section::Sz3DyDzTransRotNormal( double yTrans, double zTrans, double rot){
    double Sy3PrimeDyDz = Sy3DyDz->valueNormal() - 3.0 * yTrans * Izz->valueNormal() + 3.0 * pow(yTrans, 2.0) * Sz->valueNormal() - pow(yTrans, 3.0) * A->valueNormal();
    double Sz3PrimeDyDz = Sz3DyDz->valueNormal() - 3.0 * zTrans * Iyy->valueNormal() + 3.0 * pow(zTrans, 2.0) * Sy->valueNormal() - pow(zTrans, 3.0) * A->valueNormal();
    double Sy2zPrimeDyDz = Sy2zDyDz->valueNormal() - zTrans * Izz->valueNormal() - 2.0 * yTrans * Iyy->valueNormal() + 2.0 * yTrans * zTrans * Sz->valueNormal() + pow(yTrans, 2.0) * Sy->valueNormal() - pow(yTrans, 2.0) * zTrans * A->valueNormal();
    double Syz2PrimeDyDz = Syz2DyDz->valueNormal() - yTrans * Iyy->valueNormal() - 2.0 * zTrans * Iyy->valueNormal() + 2.0 * yTrans * zTrans * Sy->valueNormal() + pow(zTrans, 2.0) * Sz->valueNormal() - yTrans * pow(zTrans, 2.0) * A->valueNormal();

    double cosa = cos( rot );
    double sina = sin( rot );

    double ret = - pow(sina, 3.0) * Sy3PrimeDyDz + 3.0 * pow(sina, 2.0) * cosa * Sy2zPrimeDyDz - \
            3.0 * sina * pow(cosa, 2.0) * Syz2PrimeDyDz + pow(cosa, 3.0) * Sz3PrimeDyDz;

    if( applyRoundOff->valueNormal() ){
        ret = MRoundOff( ret, 5.0 );
    }
    return ret;
}

double Section::Sy3DyDzTransRotNormal( double yTrans, double zTrans, double rot){
    double Sy3PrimeDyDz = Sy3DyDz->valueNormal() - 3.0 * yTrans * Izz->valueNormal() + 3.0 * pow(yTrans, 2.0) * Sz->valueNormal() - pow(yTrans, 3.0) * A->valueNormal();
    double Sz3PrimeDyDz = Sz3DyDz->valueNormal() - 3.0 * zTrans * Iyy->valueNormal() + 3.0 * pow(zTrans, 2.0) * Sy->valueNormal() - pow(zTrans, 3.0) * A->valueNormal();
    double Sy2zPrimeDyDz = Sy2zDyDz->valueNormal() - zTrans * Izz->valueNormal() - 2.0 * yTrans * Iyy->valueNormal() + 2.0 * yTrans * zTrans * Sz->valueNormal() + pow(yTrans, 2.0) * Sy->valueNormal() - pow(yTrans, 2.0) * zTrans * A->valueNormal();
    double Syz2PrimeDyDz = Syz2DyDz->valueNormal() - yTrans * Iyy->valueNormal() - 2.0 * zTrans * Iyy->valueNormal() + 2.0 * yTrans * zTrans * Sy->valueNormal() + pow(zTrans, 2.0) * Sz->valueNormal() - yTrans * pow(zTrans, 2.0) * A->valueNormal();

    double cosa = cos( rot );
    double sina = sin( rot );

    double ret = pow(cosa, 3.0) * Sy3PrimeDyDz + 3.0 * pow(cosa, 2.0) * sina * Sy2zPrimeDyDz + \
            3.0 * cosa * pow(sina, 2.0) * Syz2PrimeDyDz + pow(sina, 3.0) * Sz3PrimeDyDz;

    if( applyRoundOff->valueNormal() ){
        ret = MRoundOff( ret, 5.0 );
    }
    return ret;
}

double Section::Syz2DyDzTransRotNormal( double yTrans, double zTrans, double rot){
    double Sy3PrimeDyDz = Sy3DyDz->valueNormal() - 3.0 * yTrans * Izz->valueNormal() + 3.0 * pow(yTrans, 2.0) * Sz->valueNormal() - pow(yTrans, 3.0) * A->valueNormal();
    double Sz3PrimeDyDz = Sz3DyDz->valueNormal() - 3.0 * zTrans * Iyy->valueNormal() + 3.0 * pow(zTrans, 2.0) * Sy->valueNormal() - pow(zTrans, 3.0) * A->valueNormal();
    double Sy2zPrimeDyDz = Sy2zDyDz->valueNormal() - zTrans * Izz->valueNormal() - 2.0 * yTrans * Iyy->valueNormal() + 2.0 * yTrans * zTrans * Sz->valueNormal() + pow(yTrans, 2.0) * Sy->valueNormal() - pow(yTrans, 2.0) * zTrans * A->valueNormal();
    double Syz2PrimeDyDz = Syz2DyDz->valueNormal() - yTrans * Iyy->valueNormal() - 2.0 * zTrans * Iyy->valueNormal() + 2.0 * yTrans * zTrans * Sy->valueNormal() + pow(zTrans, 2.0) * Sz->valueNormal() - yTrans * pow(zTrans, 2.0) * A->valueNormal();

    double cosa = cos( rot );
    double sina = sin( rot );

    double ret = pow(sina, 2.0) * cosa * Sy3PrimeDyDz + ( pow(sina, 3.0) - 2.0 * sina * pow(cosa, 2.0)) * Sy2zPrimeDyDz + \
            ( pow(cosa, 3.0) - 2.0 * pow(sina, 2.0)) * cosa * Syz2PrimeDyDz + sina * pow(cosa, 2.0) * Sz3PrimeDyDz;

    if( applyRoundOff->valueNormal() ){
        ret = MRoundOff( ret, 5.0 );
    }
    return ret;
}

double Section::Sy2zDyDzTransRotNormal( double yTrans, double zTrans, double rot){
    double Sy3PrimeDyDz = Sy3DyDz->valueNormal() - 3.0 * yTrans * Izz->valueNormal() + 3.0 * pow(yTrans, 2.0) * Sz->valueNormal() - pow(yTrans, 3.0) * A->valueNormal();
    double Sz3PrimeDyDz = Sz3DyDz->valueNormal() - 3.0 * zTrans * Iyy->valueNormal() + 3.0 * pow(zTrans, 2.0) * Sy->valueNormal() - pow(zTrans, 3.0) * A->valueNormal();
    double Sy2zPrimeDyDz = Sy2zDyDz->valueNormal() - zTrans * Izz->valueNormal() - 2.0 * yTrans * Iyy->valueNormal() + 2.0 * yTrans * zTrans * Sz->valueNormal() + pow(yTrans, 2.0) * Sy->valueNormal() - pow(yTrans, 2.0) * zTrans * A->valueNormal();
    double Syz2PrimeDyDz = Syz2DyDz->valueNormal() - yTrans * Iyy->valueNormal() - 2.0 * zTrans * Iyy->valueNormal() + 2.0 * yTrans * zTrans * Sy->valueNormal() + pow(zTrans, 2.0) * Sz->valueNormal() - yTrans * pow(zTrans, 2.0) * A->valueNormal();

    double cosa = cos( rot );
    double sina = sin( rot );

    double ret = - sina * pow(cosa, 2.0) * Sy3PrimeDyDz + ( pow(cosa, 3.0) - 2.0 * pow(sina,2.0) * cosa) * Sy2zPrimeDyDz - \
            ( pow(sina, 3.0) - 2.0 * sina * pow(cosa, 2.0)) * Syz2PrimeDyDz + pow(sina,2.0) * cosa * Sz3PrimeDyDz;

    if( applyRoundOff->valueNormal() ){
        ret = MRoundOff( ret, 5.0 );
    }
    return ret;
}

double Section::beamlBucklNormal( double Ly, double Lz ){
    double ly = 0.0, lz = 0.0;
    if( iy->valueNormal() != 0.0 ){
        ly = fabs( Ly / iy->valueNormal() );
    }
    if( Lz != 0.0 ){
        lz = fabs( Lz / iz->valueNormal() );
    }
    return ly>lz? ly : lz;
}

double Section::beamNCrNormal( double Ly, double Lz ){
    if( Ly != 0.0 && Lz != 0.0 ){
        double Ny = pow(M_PI, 2.0) * m_material->E->valueNormal() * Iyy->valueNormal() / pow( Ly, 2.0);
        double Nz = pow(M_PI, 2.0) * m_material->E->valueNormal() * Izz->valueNormal() / pow( Lz, 2.0);
        return Ny<Nz? Ny : Nz;
    } else if( Ly != 0.0 ){ // Lz == 0.0
        return pow(M_PI, 2.0) * m_material->E->valueNormal() * Iyy->valueNormal() / pow( Ly, 2.0);
    } else if( Lz != 0.0 ){ // Ly = 0.0
        return pow(M_PI, 2.0) * m_material->E->valueNormal() * Izz->valueNormal() / pow( Lz, 2.0);
    } else {
        return 0.0;
    }
}

void Section::SyTransRot(DoublePlus * ret, Point2DPlus * trans, DoublePlus * a , bool emitAuto){
    double yTrans = 0.0, zTrans = 0.0;
    double rot = 0.0;
    if( trans ){
        yTrans = trans->y->valueNormal();
        zTrans = trans->z->valueNormal();
    }
    if( a ){
        rot = a->valueNormal();
    }
    ret->setValueNormal( SyTransRotNormal(yTrans, zTrans, rot ), emitAuto );
}

void Section::SzTransRot(DoublePlus * ret, Point2DPlus * trans, DoublePlus * a , bool emitAuto){
    double yTrans = 0.0, zTrans = 0.0;
    double rot = 0.0;
    if( trans ){
        yTrans = trans->y->valueNormal();
        zTrans = trans->z->valueNormal();
    }
    if( a ){
        rot = a->valueNormal();
    }
    ret->setValueNormal( SzTransRotNormal(yTrans, zTrans, rot ), emitAuto );
}

void Section::IyyTransRot( DoublePlus * ret, Point2DPlus * trans, DoublePlus * a, bool emitAuto ){
    double yTrans = 0.0, zTrans = 0.0;
    double rot = 0.0;
    if( trans ){
        yTrans = trans->y->valueNormal();
        zTrans = trans->z->valueNormal();
    }
    if( a ){
        rot = a->valueNormal();
    }
    ret->setValueNormal( IyyTransRotNormal(yTrans, zTrans, rot ), emitAuto );
}

void Section::IzzTransRot( DoublePlus * ret, Point2DPlus * trans, DoublePlus * a, bool emitAuto ){
    double yTrans = 0.0, zTrans = 0.0;
    double rot = 0.0;
    if( trans ){
        yTrans = trans->y->valueNormal();
        zTrans = trans->z->valueNormal();
    }
    if( a ){
        rot = a->valueNormal();
    }
    ret->setValueNormal( IzzTransRotNormal(yTrans, zTrans, rot), emitAuto );
}

void Section::IyzTransRot( DoublePlus * ret, Point2DPlus * trans, DoublePlus * a, bool emitAuto ){
    double yTrans = 0.0, zTrans = 0.0;
    double rot = 0.0;
    if( trans ){
        yTrans = trans->y->valueNormal();
        zTrans = trans->z->valueNormal();
    }
    if( a ){
        rot = a->valueNormal();
    }
    ret->setValueNormal( IyzTransRotNormal(yTrans, zTrans, rot ), emitAuto );
}

void Section::Sz3DyDzTransRot( DoublePlus * ret, Point2DPlus * trans, DoublePlus * a, bool emitAuto  ){
    double yTrans = 0.0, zTrans = 0.0;
    double rot = 0.0;
    if( trans ){
        yTrans = trans->y->valueNormal();
        zTrans = trans->z->valueNormal();
    }
    if( a ){
        rot = a->valueNormal();
    }
    ret->setValueNormal( Sz3DyDzTransRotNormal( yTrans, zTrans, rot ), emitAuto );
}

void Section::Sy3DyDzTransRot( DoublePlus * ret, Point2DPlus * trans, DoublePlus * a, bool emitAuto  ){
    double yTrans = 0.0, zTrans = 0.0;
    double rot = 0.0;
    if( trans ){
        yTrans = trans->y->valueNormal();
        zTrans = trans->z->valueNormal();
    }
    if( a ){
        rot = a->valueNormal();
    }
    ret->setValueNormal( Sy3DyDzTransRotNormal( yTrans, zTrans, rot ), emitAuto );
}

void Section::Sy2zDyDzTransRot( DoublePlus * ret, Point2DPlus * trans, DoublePlus * a, bool emitAuto  ){
    double yTrans = 0.0, zTrans = 0.0;
    double rot = 0.0;
    if( trans ){
        yTrans = trans->y->valueNormal();
        zTrans = trans->z->valueNormal();
    }
    if( a ){
        rot = a->valueNormal();
    }
    ret->setValueNormal( Sy2zDyDzTransRotNormal( yTrans, zTrans, rot ), emitAuto );
}

void Section::Syz2DyDzTransRot( DoublePlus * ret, Point2DPlus * trans, DoublePlus * a, bool emitAuto  ){
    double yTrans = 0.0, zTrans = 0.0;
    double rot = 0.0;
    if( trans ){
        yTrans = trans->y->valueNormal();
        zTrans = trans->z->valueNormal();
    }
    if( a ){
        rot = a->valueNormal();
    }
    ret->setValueNormal( Syz2DyDzTransRotNormal( yTrans, zTrans, rot ), emitAuto );
}

void Section::beamlBuckl( DoublePlus * ret, DoublePlus * Ly, DoublePlus * Lz ){
    if( Ly != NULL && Lz != NULL ){
        ret->setValueNormal( beamlBucklNormal( Ly->valueNormal(), Lz->valueNormal() ) );
    } else if( Ly != NULL ){ // Lz == NULL
        ret->setValueNormal( beamlBucklNormal( Ly->valueNormal(), 0.0 ) );
    } else if( Lz != NULL ){ // Ly == NULL
        ret->setValueNormal( beamlBucklNormal( 0.0, Lz->valueNormal() ) );
    } else {
        ret->setValueNormal( 0.0 );
    }
}

void Section::beamNCr( DoublePlus * ret, DoublePlus * Ly, DoublePlus * Lz ){
    if( Ly != NULL && Lz != NULL ){
        ret->setValueNormal( beamNCrNormal( Ly->valueNormal(), Lz->valueNormal() ) );
    } else if( Ly != NULL ){ // Lz == NULL
        ret->setValueNormal( beamNCrNormal( Ly->valueNormal(), 0.0 ) );
    } else if( Lz != NULL ){ // Ly == NULL
        ret->setValueNormal( beamNCrNormal( 0.0, Lz->valueNormal() ) );
    } else {
        ret->setValueNormal( 0.0 );
    }
}

double Section::ANormal(){
    return 0.0;
}

double Section::SyNormal(){
    return 0.0;
}

double Section::SzNormal(){
    return 0.0;
}

double Section::IyyNormal(){
    return 0.0;
}

double Section::IzzNormal(){
    return 0.0;
}

double Section::IyzNormal(){
    return 0.0;
}

double Section::IxxNormal() {
    return Iyy->valueNormal() + Izz->valueNormal();
}

double Section::Sy3DyDzNormal(){
    return 0.0;
}

double Section::Sz3DyDzNormal(){
    return 0.0;
}

double Section::Sy2zDyDzNormal(){
    return 0.0;
}

double Section::Syz2DyDzNormal(){
    return 0.0;
}

double Section::IGyyNormal() {
    double rot = 0.0;
    double yTrans = G->y->valueNormal();
    double zTrans = G->z->valueNormal();
    return IyyTransRotNormal(yTrans, zTrans, rot );
}

double Section::IGzzNormal() {
    double rot = 0.0;
    double    yTrans = G->y->valueNormal();
    double  zTrans = G->z->valueNormal();
    return IzzTransRotNormal(yTrans, zTrans, rot);
}

double Section::IGyzNormal() {
    double rot = 0.0;
    double yTrans = G->y->valueNormal();
    double zTrans = G->z->valueNormal();
    return IyzTransRotNormal(yTrans, zTrans, rot );
}

double Section::IGxxNormal() {
    return IGyy->valueNormal() + IGzz->valueNormal();
}

double Section::GyNormal(){
    if( A->valueNormal() != 0 ){
        return MRoundOff( Sz->valueNormal()/A->valueNormal(), 1.0 );
    }
    return 0.0;
}

double Section::GzNormal(){
    if( A->valueNormal() != 0 ){
        return MRoundOff( Sy->valueNormal()/A->valueNormal(), 1.0);
    }
    return 0.0;
}

double Section::thetaCNormal(){
    if( IGyz->valueNormal() != 0.0 && (IGyy->valueNormal() - IGzz->valueNormal()) != 0.0 ){
        return 0.50 * atan2( 2.0 * IGyz->valueNormal(), (IGzz->valueNormal() - IGyy->valueNormal()));
    }
    return 0.0;
}

double Section::ICyyNormal(){
    if( IGyy->valueNormal() > IGzz->valueNormal() ){
        return 0.50 * ( (IGyy->valueNormal() + IGzz->valueNormal()) +
                        sqrt( pow(IGyy->valueNormal()-IGzz->valueNormal(), 2.0) + 4.0 * pow(IGyz->valueNormal(), 2.0) ) );
    } else {
        return 0.50 * ( (IGyy->valueNormal() + IGzz->valueNormal()) -
                        sqrt( pow(IGyy->valueNormal()-IGzz->valueNormal(), 2.0) + 4.0 * pow(IGyz->valueNormal(), 2.0) ) );
    }
}

double Section::ICzzNormal(){
    if( IGyy->valueNormal() > IGzz->valueNormal() ){
        return 0.50 * ( (IGyy->valueNormal() + IGzz->valueNormal()) -
                        sqrt(( pow(IGyy->valueNormal()-IGzz->valueNormal(), 2.0) + 4.0 * pow(IGyz->valueNormal(), 2.0) )));
    } else {
        return 0.50 * ( (IGyy->valueNormal() + IGzz->valueNormal()) +
                        sqrt(( pow(IGyy->valueNormal()-IGzz->valueNormal(), 2.0) + 4.0 * pow(IGyz->valueNormal(), 2.0) )));
    }
}

double Section::ICyzNormal(){
    double rot = thetaC->valueNormal();
    double yTrans = G->y->valueNormal();
    double zTrans = G->z->valueNormal();
    return IyzTransRotNormal(yTrans, zTrans, rot );
}

double Section::ICxxNormal() {
    return ICyy->valueNormal() + ICzz->valueNormal();
}

double Section::iyNormal(){
    if( A->valueNormal() != 0.0 ){
        return pow( Iyy->valueNormal() / A->valueNormal() ,0.50);
    }
    return 0.0;
}

double Section::izNormal(){
    if( A->valueNormal() != 0.0 ){
        return pow( Izz->valueNormal() / A->valueNormal() ,0.50);
    }
    return 0.0;
}

double Section::tyNormal(){
    return 1.2;
}

double Section::tzNormal(){
    return 1.2;
}

double Section::tyzNormal(){
    return 0.0;
}

double Section::ITNormal() {
    return 0.0;
}

double Section::pWNormal(){
    if( m_material ){
        return (m_material->gammaW->valueNormal() * A->valueNormal());
    } else {
        return 0.0;
    }
}

void Section::setA( bool emitAuto ){
    A->setValueNormal( ANormal(), emitAuto );
}

void Section::setSy( bool emitAuto ){
    Sy->setValueNormal( SyNormal(), emitAuto );
}

void Section::setSz( bool emitAuto ){
    Sz->setValueNormal( SzNormal(), emitAuto );
}

void Section::setIyy( bool emitAuto ){
    Iyy->setValueNormal( IyyNormal(), emitAuto );
}

void Section::setIzz( bool emitAuto ){
    Izz->setValueNormal( IzzNormal(), emitAuto );
}

void Section::setIyz( bool emitAuto ){
    Iyz->setValueNormal( IyzNormal(), emitAuto );
}

void Section::setSy3DyDz( bool emitAuto ){
    Sy3DyDz->setValueNormal( Sy3DyDzNormal(), emitAuto );
}

void Section::Section::setSz3DyDz( bool emitAuto ){
    Sz3DyDz->setValueNormal( Sz3DyDzNormal(), emitAuto );
}

void Section::setSy2zDyDz( bool emitAuto ){
    Sy2zDyDz->setValueNormal( Sy2zDyDzNormal(), emitAuto );
}

void Section::setSyz2DyDz( bool emitAuto ){
    Syz2DyDz->setValueNormal( Syz2DyDzNormal(), emitAuto );
}

void Section::setPW( bool emitAuto ){
    pW->setValueNormal( pWNormal(), emitAuto );
}

void Section::setIxx( bool emitAuto ){
    Ixx->setValueNormal( IxxNormal(), emitAuto);
}

void Section::setGy( bool emitAuto){
    G->y->setValueNormal( GyNormal(), emitAuto );
}

void Section::setGz( bool emitAuto){
    G->z->setValueNormal( GzNormal(), emitAuto );
}

void Section::setIGyy( bool emitAuto ){
    IGyy->setValueNormal( IGyyNormal(), emitAuto );
}

void Section::setIGzz( bool emitAuto ){
    IGzz->setValueNormal( IGzzNormal(), emitAuto );
}

void Section::setIGyz( bool emitAuto ){
    IGyz->setValueNormal( IGyzNormal(), emitAuto );
}

void Section::setIGxx( bool emitAuto ){
    IGxx->setValueNormal( IGxxNormal(), emitAuto );
}

void Section::setThetaC( bool emitAuto ){
    thetaC->setValueNormal( thetaCNormal(), emitAuto );
}

void Section::setICyy( bool emitAuto ){
    ICyy->setValueNormal( ICyyNormal(), emitAuto );
}

void Section::setICzz( bool emitAuto ){
    ICzz->setValueNormal( ICzzNormal(), emitAuto );
}

void Section::setICyz( bool emitAuto ){
    ICyz->setValueNormal( ICyzNormal(), emitAuto );
}

void Section::setiy( bool emitAuto ){
    iy->setValueNormal( iyNormal(), emitAuto );
}

void Section::setiz( bool emitAuto ){
    iz->setValueNormal( izNormal(), emitAuto );
}

void Section::settz( bool emitAuto ){
    tz->setValueNormal( tzNormal(), emitAuto );
}

void Section::setty( bool emitAuto ){
    ty->setValueNormal( tyNormal(), emitAuto );
}

void Section::settyz( bool emitAuto ){
    tyz->setValueNormal( tyzNormal(), emitAuto );
}

void Section::setIT( bool emitAuto ){
    IT->setValueNormal( ITNormal(), emitAuto );
}

void Section::GTranslate(){
    double yTrans = - G->y->valueNormal();
    double zTrans = - G->z->valueNormal();
    translateNormal( yTrans, zTrans );
}

void Section::PTransRot(){
    double yTrans = - G->y->valueNormal();
    double zTrans = - G->z->valueNormal();
    double aP = - thetaC->valueNormal();
    translateNormal( yTrans, zTrans );
    rotateNormal( aP );
}

double Section::MRoundOff( double val , double n ){
    if( A->valueNormal() != 0.0 && applyRoundOff->valueNormal() ){
        double prec = pow(10, trunc(log10(fabs(A->valueNormal()))) * 2.0 ) * pow(1.0e-4, n );
        return round( val / prec  ) * prec;
    } else {
        return val;
    }
}
