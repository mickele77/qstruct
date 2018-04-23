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
#include "sectionsteel.h"
#include "sectionsteelvar.h"

#include "sectionloadphasemodel.h"
#include "sectionsteeli.h"
#include "sectionsteell.h"
#include "sectionsteelcirc.h"
#include "sectionsteelrect.h"
#include "sectionsteelomega.h"

#include "steel.h"

#include "point2dplusmodel.h"
#include "point2dmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "qstringplus.h"
#include "boolplus.h"
#include "structmath.h"

#include <QXmlStreamAttributes>
#include <QGraphicsItemGroup>
#include <QList>

#include <cmath>

// numero massimo parametri associati a sezione
#define N_PAR 5
// massimo numero di iterazioni in operazioni complesse
#define MAX_ITER_SECTSTEEL 500

class SectionSteelPrivate{
public:
    SectionSteelPrivate():
        sectionSteelProfile( NULL ),
        qGraphicsItem(NULL),
        sectionPoints(NULL) {
    }

    ~SectionSteelPrivate() {
        if( sectionSteelProfile ){
            delete sectionSteelProfile;
        }
        if( sectionPoints ){
            delete sectionPoints;
        }
        if( qGraphicsItem ){
            delete qGraphicsItem;
        }
    }


    /** Numero dei parametri dell'array m_p effettivamente utilizzati dal tipo di sezione impiegato */
    int np;
    /** Array contenente i parametri necessari per caratterizzare la geometria della sezione */
    DoublePlus * p[N_PAR];

    /** SectionSteelShape associata alla sezione */
    SectionSteelProfile * sectionSteelProfile;

    /** Diagramma delle classi */
    QList<QPolygonF *> * classDiagram;

    /** Oggetto grafico contenente la rappresentazione grafica della sezione */
    QGraphicsItemGroup * qGraphicsItem;

    Point2DModel * sectionPoints;
};

int SectionSteel::npMax(){
    return N_PAR;
}

SectionSteel::SectionSteel( UnitMeasure * ump, Steel * s, SectionSteel::ManufactType manType, SectionSteel::SectionShape sectType, const QString & n, QObject * parent ):
    Section( ump, s, n, parent ),
    m_d( new SectionSteelPrivate() ){
    initVar( manType, sectType);
}

SectionSteel::SectionSteel(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel *lModel, QObject *parent) :
    Section( ump, NULL, "", parent ),
    m_d( new SectionSteelPrivate() ) {
    initVar();
    loadFromXML( attrs, mModel, lModel );
}

SectionSteel::~SectionSteel(){
    delete m_d;
}

void SectionSteel::suspendSignals(bool susp) {
    VarPlusContainer::suspendSignals( susp );
    if( m_d->sectionSteelProfile ) {
        m_d->sectionSteelProfile->suspendSignals( susp );
    }
}

void SectionSteel::loadFromXML( const QXmlStreamAttributes &attrs, MaterialModel * mModel, SectionLoadPhaseModel * loadModel ){
    for ( int i=0; i<attrs.count(); ++i ) {
        for( QMap<QString, VarPlus *>::iterator iter = m_varContainer->begin(); iter != m_varContainer->end(); ++iter ){
            if ( attrs.at(i).name().toString().toUpper() == (*iter)->name().toUpper() ) {
                (*iter)->setValueNormal( attrs.at(i).value().toString() );
            } else if ( attrs.at(i).name().toString().toUpper() == "MATERIAL" ) {
                Steel * st = dynamic_cast<Steel *>(mModel->materialId( attrs.at(i).value().toString().toUInt() ));
                if( st == NULL ){
                    st = dynamic_cast<Steel *>(mModel->firstMaterial( MaterialModel::SteelMaterial ) );
                }
                if( st != NULL ){
                    setMaterial( st, false );
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

QString SectionSteel::typeSection(){
    return trUtf8("Sezione in Acciaio");
}

Steel * SectionSteel::steel(){
    return dynamic_cast<Steel *>(m_material);
}

void SectionSteel::setMaterial( Material * mat, bool emitAuto ){
    setSteel( dynamic_cast<Steel *>(mat), emitAuto );
}

void SectionSteel::setSteel( Steel * s, bool emitAuto ){
    Steel * oldSteel = dynamic_cast<Steel *>(m_material);
    if( oldSteel ){
        NULSMax->removeConnectedVarContainer( oldSteel );
        WEffMyMax->removeConnectedVarContainer( oldSteel );
        WEffMyMin->removeConnectedVarContainer( oldSteel );
        WEffMzMax->removeConnectedVarContainer( oldSteel );
        WEffMzMin->removeConnectedVarContainer( oldSteel );
        NClass->removeConnectedVarContainer( oldSteel );
        MULSyMax->removeConnectedVarContainer( oldSteel );
        MULSyMin->removeConnectedVarContainer( oldSteel );
        MULSzMax->removeConnectedVarContainer( oldSteel );
        MULSzMin->removeConnectedVarContainer( oldSteel );
        MyMaxClass->removeConnectedVarContainer( oldSteel );
        MyMinClass->removeConnectedVarContainer( oldSteel );
        MyClassLimit->removeConnectedVarContainer( oldSteel );
        MzMaxClass->removeConnectedVarContainer( oldSteel );
        MzMinClass->removeConnectedVarContainer( oldSteel );
        MzClassLimit->removeConnectedVarContainer( oldSteel );
    }

    Section::setMaterial( s, emitAuto );
    if( m_d->sectionSteelProfile ){
        m_d->sectionSteelProfile->setSteel( s );
    }

    Steel * newSteel = dynamic_cast<Steel *>(m_material);
    if( newSteel ){
        NULSMax->addConnectedVarContainer( newSteel );
        WEffMyMax->addConnectedVarContainer( newSteel );
        WEffMyMin->addConnectedVarContainer( newSteel );
        WEffMzMax->addConnectedVarContainer( newSteel );
        WEffMzMin->addConnectedVarContainer( newSteel );
        NClass->addConnectedVarContainer( newSteel );
        MULSyMax->addConnectedVarContainer( newSteel );
        MULSyMin->addConnectedVarContainer( newSteel );
        MULSzMax->addConnectedVarContainer( newSteel );
        MULSzMin->addConnectedVarContainer( newSteel );
        MyMaxClass->addConnectedVarContainer( newSteel );
        MyMinClass->addConnectedVarContainer( newSteel );
        MyClassLimit->addConnectedVarContainer( newSteel );
        MzMaxClass->addConnectedVarContainer( newSteel );
        MzMinClass->addConnectedVarContainer( newSteel );
        MzClassLimit->addConnectedVarContainer( newSteel );
    }
}

int SectionSteel::np(){
    return m_d->sectionSteelProfile->np();
}

DoublePlus * SectionSteel::p( int i ){
    if( i >= 0 && i < npMax() ){
        return m_d->p[i];
    } else {
        return NULL;
    }
}

void SectionSteel::updateSectionPoints() {
    if( m_d->sectionPoints == NULL ){
        m_d->sectionPoints = new Point2DModel();
    }

    if( m_d->sectionSteelProfile != NULL ){
        *(m_d->sectionPoints) = *(m_d->sectionSteelProfile->sectionPoints() );

        m_d->sectionPoints->rotateNormal( rotationO->valueNormal() );
        m_d->sectionPoints->translateNormal( translationO->y->valueNormal(),translationO->z->valueNormal() );
    }
}

void SectionSteel::updateSectionSteelProfile(){
    bool okUpdate = false;

    if( m_d->sectionSteelProfile != NULL ){
        if( m_d->sectionSteelProfile->sectionShape() != sectionShape->valueNormal() ){
            disconnect( m_d->sectionSteelProfile, &SectionSteelProfile::sectionSteelProfileChanged, this, &SectionSteel::updateReadOnlyVars );
            delete m_d->sectionSteelProfile;
            m_d->sectionSteelProfile = NULL;
            okUpdate = true;
        }
    } else {
        okUpdate = true;
    }

    if( okUpdate ){
        suspendSignals( true );

        switch( sectionShape->valueNormal() ){
        case sectionI: {
            m_d->sectionSteelProfile = new SectionSteelI( m_d->p, steel(), m_unitMeasure );
            break;
        }
        case sectionL: {
            m_d->sectionSteelProfile = new SectionSteelL( m_d->p, steel(), m_unitMeasure );
            break;
        }
        case sectionCirc:{
            m_d->sectionSteelProfile = new SectionSteelCirc( m_d->p, steel(), m_unitMeasure );
            break;
        }
        case sectionRect:{
            m_d->sectionSteelProfile = new SectionSteelRect( m_d->p, steel(), m_unitMeasure );
            break;
        }
        case sectionOmega:{
            m_d->sectionSteelProfile = new SectionSteelOmega( m_d->p, steel(), m_unitMeasure );
            break;
        }
        default:{
            m_d->sectionSteelProfile = NULL;
            break;
        }
        }

        if( m_d->qGraphicsItem && m_d->sectionSteelProfile ){
            m_d->qGraphicsItem->resetTransform();
            m_d->qGraphicsItem->addToGroup( m_d->sectionSteelProfile->qGraphicsItem() );
        }

        connect( m_d->sectionSteelProfile, &SectionSteelProfile::sectionSteelProfileChanged, this, &SectionSteel::updateReadOnlyVars );

        updateReadOnlyVars();
        updateQGraphics();

        suspendSignals( false );
    }
}

void SectionSteel::initVar( SectionSteel::ManufactType manType, SectionSteel::SectionShape sectType ){
    *m_typeNameInternal = "SectionSteel";
    typeName->setValue( trUtf8("Acciaio"));

    manufactType = new SectionSteelManufactType( manType, "manufactType" );
    manufactType->setRichName( trUtf8("Realizzazione"));
    addVarToContainer( manufactType );

    sectionShape = new SectionSteelShape( sectType, "sectionShape" );
    sectionShape->setRichName( trUtf8("Forma"));
    addVarToContainer( sectionShape );

    connect( sectionShape, &SectionSteelShape::valueChanged, this, &SectionSteel::updateSectionSteelProfile );

    A->setReadOnly( true );
    Sy->setReadOnly( true );
    Sz->setReadOnly( true );
    Iyy->setReadOnly( true );
    Izz->setReadOnly( true );
    Iyz->setReadOnly( true );
    Sy3DyDz->setReadOnly( true );
    Sz3DyDz->setReadOnly( true );
    Sy2zDyDz->setReadOnly( true );
    Syz2DyDz->setReadOnly( true );

    translationO = new Point2DPlus( "translationO", "translateOy", 0.0, "translateOz", 0.0, m_unitMeasure, UnitMeasure::sectL );
    translationO->y->setRichName( trUtf8("Δy<span style=\" vertical-align:sub;\">O</span>"));
    translationO->z->setRichName( trUtf8("Δz<span style=\" vertical-align:sub;\">O</span>"));
    addVarToContainer( translationO );
    connect( translationO, &Point2DPlus::valueChanged, this, &Section::updateQGraphics );

    rotationO = new DoublePlus( 0.0, "rotationO", m_unitMeasure, UnitMeasure::angle );
    rotationO->setRichName( trUtf8("α<span style=\" vertical-align:sub;\">O</span>"));
    addVarToContainer( rotationO );
    connect( rotationO, &DoublePlus::valueChanged, this, &Section::updateQGraphics );

    Iw = new DoublePlus(0.0, "Iw", m_unitMeasure, UnitMeasure::sectL6, true );
    Iw->setToolTip( trUtf8("Costante di ingobbamento"));
    Iw->setRichName( trUtf8("I<span style=\" vertical-align:sub;\">w</span>"));
    addVarToContainer( Iw );
    Iw->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setIw) );

    WElMy = new DoublePlus(0.0, "WElMy", m_unitMeasure, UnitMeasure::sectL3 );
    WElMy->setReadOnly( true );
    WElMy->setToolTip( trUtf8("Modulo elastico di resistenza flessionale, asse y-y"));
    WElMy->setRichName( trUtf8("W<span style=\" vertical-align:sub;\">el,y</span>"));
    addVarToContainer( WElMy );
    WElMy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setWElMy) );

    WElMz = new DoublePlus(0.0, "WElMz", m_unitMeasure, UnitMeasure::sectL3 );
    WElMz->setReadOnly( true );
    WElMz->setToolTip( trUtf8("Modulo elastico di resistenza flessionale, asse z-z"));
    WElMz->setRichName( trUtf8("W<span style=\" vertical-align:sub;\">el,z</span>"));
    addVarToContainer( WElMz );
    WElMz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setWElMz) );

    WPlMy = new DoublePlus(0.0, "WPlMy", m_unitMeasure, UnitMeasure::sectL3 );
    WPlMy->setReadOnly( true );
    WPlMy->setToolTip( trUtf8("Modulo plastico di resistenza flessionale, asse y-y"));
    WPlMy->setRichName( trUtf8("W<span style=\" vertical-align:sub;\">pl,y</span>"));
    addVarToContainer( WPlMy );
    WPlMy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setWPlMy) );

    WPlMz = new DoublePlus(0.0, "WPlMz", m_unitMeasure, UnitMeasure::sectL3 );
    WPlMz->setReadOnly( true );
    WPlMz->setToolTip( trUtf8("Modulo plastico di resistenza flessionale, asse z-z"));
    WPlMz->setRichName( trUtf8("W<span style=\" vertical-align:sub;\">pl,z</span>"));
    addVarToContainer( WPlMz );
    WPlMz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setWPlMz) );

    AEffN = new DoublePlus(0.0, "AEffN", m_unitMeasure, UnitMeasure::sectL2 );
    AEffN->setReadOnly( true );
    AEffN->setToolTip( trUtf8("Area efficace"));
    AEffN->setRichName( trUtf8("A<span style=\" vertical-align:sub;\">eff</span>"));
    addVarToContainer( AEffN );
    AEffN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setEffN) );

    GEffN = new Point2DPlus("GEffN", "yGEffNMinus", 0.0, "zGEffNMinus", 0.0, m_unitMeasure, UnitMeasure::sectL );
    addVarToContainer( GEffN );
    GEffN->y->setReadOnly( true );
    GEffN->y->setToolTip(trUtf8("Ascissa del baricentro della sezione efficace in presenza di solo sforzo di compressione"));
    GEffN->y->setRichName( trUtf8("y<span style=\" vertical-align:sub;\">G,eff</span>"));
    GEffN->z->setReadOnly( true );
    GEffN->z->setToolTip(trUtf8("Ordinata del baricentro della sezione efficace in presenza di solo sforzo di compressione"));
    GEffN->z->setRichName( trUtf8("z<span style=\" vertical-align:sub;\">G,eff</span>"));
    GEffN->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setEffN) );

    WEffMyMax = new DoublePlus(0.0, "WEffMyPlus", m_unitMeasure, UnitMeasure::sectL3 );
    WEffMyMax->setReadOnly( true );
    WEffMyMax->setToolTip( trUtf8("Massimo modulo di resistenza flessionale della sezione efficace, asse y-y"));
    WEffMyMax->setRichName( trUtf8("W<span style=\" vertical-align:sub;\">y,eff,max</span>"));
    addVarToContainer( WEffMyMax );
    WEffMyMax->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setWEffMyMax) );

    WEffMyMin = new DoublePlus(0.0, "WEffMyMinus", m_unitMeasure, UnitMeasure::sectL3 );
    WEffMyMin->setReadOnly( true );
    WEffMyMin->setToolTip( trUtf8("Minimo modulo di resistenza felssionale della sezione efficace, asse y-y"));
    WEffMyMin->setRichName( trUtf8("W<span style=\" vertical-align:sub;\">y,eff,min</span>"));
    addVarToContainer( WEffMyMin );
    WEffMyMin->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setWEffMyMin) );

    WEffMzMax = new DoublePlus(0.0, "WEffMzPlus", m_unitMeasure, UnitMeasure::sectL3 );
    WEffMzMax->setReadOnly( true );
    WEffMzMax->setToolTip( trUtf8("Massimo modulo di resistenza flessionale della sezione efficace, asse z-z"));
    WEffMzMax->setRichName( trUtf8("W<span style=\" vertical-align:sub;\">z,eff,max</span>"));
    addVarToContainer( WEffMzMax );
    WEffMzMax->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setWEffMzMax) );

    WEffMzMin = new DoublePlus(0.0, "WEffMzMinus", m_unitMeasure, UnitMeasure::sectL3 );
    WEffMzMin->setReadOnly( true );
    WEffMzMin->setToolTip( trUtf8("Minimo modulo di resistenza flessionale della sezione efficace, asse z-z"));
    WEffMzMin->setRichName( trUtf8("W<span style=\" vertical-align:sub;\">z,eff,min</span>"));
    addVarToContainer( WEffMzMin );
    WEffMzMin->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setWEffMzMin) );

    NClass = new QStringPlus("0", "NClass" );
    NClass->setReadOnly( true );
    NClass->setToolTip( trUtf8("Classe della sezione per compressione semplice"));
    NClass->setRichName( trUtf8("Classe N"));
    addVarToContainer( NClass );
    NClass->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setNClass) );

    MyMaxClass = new QStringPlus("0", "MyMaxClass" );
    MyMaxClass->setReadOnly( true );
    MyMaxClass->setToolTip( trUtf8("Classe della sezione per flessione semplice massima lungo asse y-y"));
    MyMaxClass->setRichName( trUtf8("Classe My+"));
    addVarToContainer( MyMaxClass );
    MyMaxClass->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setMyMaxClass) );

    MyMinClass = new QStringPlus("0", "MyMinClass" );
    MyMinClass->setReadOnly( true );
    MyMinClass->setToolTip( trUtf8("Classe della sezione per flessione semplice minima lungo l'asse y-y"));
    MyMinClass->setRichName( trUtf8("Classe My-"));
    addVarToContainer( MyMinClass );
    MyMinClass->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setMyMinClass) );

    MyClassLimit = new DoublePlus(0.0, "MyClassLimit", m_unitMeasure, UnitMeasure::loadF );
    MyClassLimit->setReadOnly( true );
    MyClassLimit->setToolTip( trUtf8("Carico limite per applicazione classe flessione pura y-y (6.2.9.1 (4) dell'EC3)"));
    MyClassLimit->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">lim,y</span>"));
    addVarToContainer( MyClassLimit );
    MyClassLimit->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setMyClassLimit) );

    MzMaxClass = new QStringPlus("0", "MzMaxClass" );
    MzMaxClass->setReadOnly( true );
    MzMaxClass->setToolTip( trUtf8("Classe della sezione per flessione semplice massimo lungo l'asse z-z "));
    MzMaxClass->setRichName( trUtf8("Classe Mz+"));
    addVarToContainer( MzMaxClass );
    MzMaxClass->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setMzMaxClass) );

    MzMinClass = new QStringPlus("0", "MzMinClass" );
    MzMinClass->setReadOnly( true );
    MzMinClass->setToolTip( trUtf8("Classe della sezione per flessione semplice minima lungo l'asse z-z"));
    MzMinClass->setRichName( trUtf8("Classe Mz-"));
    addVarToContainer( MzMinClass );
    MzMinClass->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setMzMinClass) );

    MzClassLimit = new DoublePlus(0.0, "MzClassLimit", m_unitMeasure, UnitMeasure::loadF );
    MzClassLimit->setReadOnly( true );
    MzClassLimit->setToolTip( trUtf8("Carico limite per applicazione classe flessione pura z-z (6.2.9.1 (4) dell'EC3)"));
    MzClassLimit->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">lim,z</span>"));
    addVarToContainer( MzClassLimit );
    MzClassLimit->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setMzClassLimit) );

    NULSMax = new DoublePlus(0.0, "NULSPlus", m_unitMeasure, UnitMeasure::loadF );
    NULSMax->setReadOnly( true );
    NULSMax->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">t,Rd</span>") );
    NULSMax->setToolTip( trUtf8("Resistenza di calcolo a trazione della sezione lorda"));
    addVarToContainer( NULSMax );
    NULSMax->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setNULSMax) );
    NULSMax->addConnectedVar( A );

    NULSMin = new DoublePlus(0.0, "NULSMinus", m_unitMeasure, UnitMeasure::loadF );
    NULSMin->setReadOnly( true );
    NULSMin->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">c,Rd</span>") );
    NULSMin->setToolTip( trUtf8("Resistenza di calcolo a compressione della sezione lorda"));
    addVarToContainer( NULSMin );
    NULSMin->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setNULSMin) );
    NULSMax->addConnectedVars( 2, NClass, A );

    MULSyMax = new DoublePlus(0.0, "MULSyMax", m_unitMeasure, UnitMeasure::loadM );
    MULSyMax->setReadOnly( true );
    MULSyMax->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">c,Rd+,y</span>") );
    MULSyMax->setToolTip( trUtf8("Momento resistente massimo nel piano perpendicolare all'asse y"));
    addVarToContainer( MULSyMax );
    MULSyMax->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setMULSyMax) );
    MULSyMax->addConnectedVars( 4, MyMaxClass, WPlMy, WElMy, WEffMyMax );

    MULSyMin = new DoublePlus(0.0, "MULSyMin", m_unitMeasure, UnitMeasure::loadM );
    MULSyMin->setReadOnly( true );
    MULSyMin->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">c,Rd-,y</span>") );
    MULSyMin->setToolTip( trUtf8("Momento resistente minimo nel piano perpendicolare all'asse y"));
    addVarToContainer( MULSyMin );
    MULSyMin->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setMULSyMin) );
    MULSyMin->addConnectedVars( 4, MyMinClass, WPlMy, WElMy, WEffMyMin );

    MULSzMax = new DoublePlus(0.0, "MULSzMax", m_unitMeasure, UnitMeasure::loadM );
    MULSzMax->setReadOnly( true );
    MULSzMax->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">c,Rd+,z</span>") );
    MULSzMax->setToolTip( trUtf8("Momento resistente nel piano perpendicolare all'asse z"));
    addVarToContainer( MULSzMax );
    MULSzMax->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setMULSzMax) );
    MULSzMax->addConnectedVars( 4, MzMaxClass, WPlMz, WElMz, WEffMzMax );

    MULSzMin = new DoublePlus(0.0, "MULSzMin", m_unitMeasure, UnitMeasure::loadM );
    MULSzMin->setReadOnly( true );
    MULSzMin->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">c,Rd-,z</span>") );
    MULSzMin->setToolTip( trUtf8("Momento resistente nel piano perpendicolare all'asse z"));
    addVarToContainer( MULSzMin );
    MULSzMin->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setMULSzMin) );
    MULSzMin->addConnectedVars( 4, MzMinClass, WPlMz, WElMz, WEffMzMin );

    AVy = new DoublePlus(0.0, "AVy", m_unitMeasure, UnitMeasure::sectL2 );
    AVy->setReadOnly( true );
    AVy->setRichName( trUtf8("A<span style=\" vertical-align:sub;\">V,y</span>") );
    AVy->setToolTip( trUtf8("Area resistente per taglio agente lungo asse y"));
    addVarToContainer( AVy );
    AVy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setAVy) );

    VULSy = new DoublePlus(0.0, "VULSy", m_unitMeasure, UnitMeasure::loadF );
    VULSy->setReadOnly( true );
    VULSy->setRichName( trUtf8("V<span style=\" vertical-align:sub;\">c,Rd,y</span>") );
    VULSy->setToolTip( trUtf8("Resistenza di calcolo per taglio agente lungo asse y"));
    addVarToContainer( VULSy );
    VULSy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setVULSy) );
    VULSy->addConnectedVar( AVy );

    AVz = new DoublePlus(0.0, "AVz", m_unitMeasure, UnitMeasure::sectL2 );
    AVz->setReadOnly( true );
    AVz->setRichName( trUtf8("A<span style=\" vertical-align:sub;\">V,z</span>") );
    AVz->setToolTip( trUtf8("Area resistente per taglio agente lungo asse z"));
    addVarToContainer( AVz );
    AVz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setAVz) );

    VULSz = new DoublePlus(0.0, "VULSz", m_unitMeasure, UnitMeasure::loadF );
    VULSz->setReadOnly( true );
    VULSz->setRichName( trUtf8("V<span style=\" vertical-align:sub;\">c,Rd,z</span>") );
    VULSz->setToolTip( trUtf8("Resistenza di calcolo per taglio agente lungo asse z"));
    addVarToContainer( VULSz );
    VULSz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionSteel::setVULSz) );
    VULSz->addConnectedVar( AVz );

    m_d->classDiagram = new QList<QPolygonF *>;
    for( int i=0; i < 4; i++ ){
        m_d->classDiagram->append( new QPolygonF );
    }

    for( int i=0; i < npMax(); i++ ){
        m_d->p[i] = new DoublePlus(0.0, "p" + QString::number(i), m_unitMeasure, UnitMeasure::noDimension );
        m_d->p[i]->setRichName( "" );
        m_d->p[i]->setReadOnly( false );
        addVarToContainer( m_d->p[i] );
    }
    Sy->addConnectedVars( 2, translationO, rotationO );
    Sz->addConnectedVars( 2, translationO, rotationO );
    Iyy->addConnectedVars( 2, translationO, rotationO );
    Izz->addConnectedVars( 2, translationO, rotationO );
    Iyz->addConnectedVars( 2, translationO, rotationO );
    Sy3DyDz->addConnectedVars( 2, translationO, rotationO );
    Sz3DyDz->addConnectedVars( 2, translationO, rotationO );
    Sy2zDyDz->addConnectedVars( 2, translationO, rotationO );
    Syz2DyDz->addConnectedVars( 2, translationO, rotationO );

    updateSectionSteelProfile();
}

double SectionSteel::fykNormal(){
    if( steel() && m_d->sectionSteelProfile ){
        if( m_d->sectionSteelProfile->maxTNormal() > 0.04 ){
            return steel()->fyk40->valueNormal();
        } else {
            return steel()->fyk->valueNormal();
        }
    }
    return 0.0;
}

double SectionSteel::fydNormal( int i ){
    if( i >= 0 && i < 3 && steel() && m_d->sectionSteelProfile ){
        if( m_d->sectionSteelProfile->maxTNormal() > 0.04 ){
            return steel()->fyd40(i)->valueNormal();
        } else {
            return steel()->fyd(i)->valueNormal();
        }
    }
    return 0.0;
}

double SectionSteel::ITNormal(){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->ITNormal();
    }
    return ret;
}

void SectionSteel::setIw( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->IwNormal();
    }
    Iw->setValueNormal( ret, emitAuto );
}

double SectionSteel::ANormal(){
    if( m_d->sectionSteelProfile ){
        return m_d->sectionSteelProfile->ANormal();
    } else {
        return 0.0;
    }
}

double SectionSteel::SyNormal(){
    if( m_d->sectionSteelProfile ){
        double zTrans = - translationO->z->valueNormal();
        double rot = - rotationO->valueNormal();
        double cosa = cos(rot);
        double sina = sin(rot);
        double Ap = m_d->sectionSteelProfile->ANormal();
        double Syp = m_d->sectionSteelProfile->SyNormal();
        double Szp = m_d->sectionSteelProfile->SzNormal();


        Syp = -Szp * sina + Syp * cosa;
        double ret = Syp - Ap * zTrans;
        if( applyRoundOff->valueNormal() ){
            ret = MRoundOff( ret, 3.0 );
        }
        return ret;
    } else {
        return 0.0;
    }
}

double SectionSteel::SzNormal(){
    if( m_d->sectionSteelProfile ){
        double yTrans = - translationO->y->valueNormal();
        double rot = - rotationO->valueNormal();
        double cosa = cos(rot);
        double sina = sin(rot);
        double Ap = m_d->sectionSteelProfile->ANormal();
        double Syp = m_d->sectionSteelProfile->SyNormal();
        double Szp = m_d->sectionSteelProfile->SzNormal();

        Szp = Szp * cosa + Syp * sina;
        double ret = Szp - Ap * yTrans;
        if( applyRoundOff->valueNormal() ){
            ret = MRoundOff( ret, 3.0 );
        }
        return ret;
    } else {
        return 0.0;
    }
}

double SectionSteel::IyyNormal(){
    if( m_d->sectionSteelProfile ){
        double yTrans = - translationO->y->valueNormal();
        double zTrans = - translationO->z->valueNormal();
        double rot = - rotationO->valueNormal();
        double Ap = m_d->sectionSteelProfile->ANormal();
        double Syp = m_d->sectionSteelProfile->SyNormal();
        double Szp = m_d->sectionSteelProfile->SzNormal();
        double Iyyp = m_d->sectionSteelProfile->IyyNormal();
        double Izzp = m_d->sectionSteelProfile->IzzNormal();
        double Iyzp = m_d->sectionSteelProfile->IyzNormal();

        double IyPrime = Iyyp - 2.0 * zTrans * Syp + pow(zTrans, 2.0) * Ap;
        double IzPrime = Izzp - 2.0 * yTrans * Szp + pow(yTrans, 2.0) * Ap;
        double IyzPrime = Iyzp - zTrans * Szp - yTrans * Syp + yTrans * zTrans * Ap;
        double cos2a = cos(2.0*rot);
        double sin2a = sin(2.0*rot);

        double ret = 0.50*(IyPrime+IzPrime) + cos2a * 0.50*(IyPrime-IzPrime) - sin2a * IyzPrime;

        if( applyRoundOff->valueNormal() ){
            ret = MRoundOff( ret, 4.0 );
        }
        return ret;
    } else {
        return 0.0;
    }
}

double SectionSteel::IzzNormal(){
    if( m_d->sectionSteelProfile ){
        double yTrans = - translationO->y->valueNormal();
        double zTrans = - translationO->z->valueNormal();
        double rot = - rotationO->valueNormal();
        double Ap = m_d->sectionSteelProfile->ANormal();
        double Syp = m_d->sectionSteelProfile->SyNormal();
        double Szp = m_d->sectionSteelProfile->SzNormal();
        double Iyyp = m_d->sectionSteelProfile->IyyNormal();
        double Izzp = m_d->sectionSteelProfile->IzzNormal();
        double Iyzp = m_d->sectionSteelProfile->IyzNormal();

        double IyPrime = Iyyp - 2.0 * zTrans * Syp + pow(zTrans, 2.0) * Ap;
        double IzPrime = Izzp - 2.0 * yTrans * Szp + pow(yTrans, 2.0) * Ap;
        double IyzPrime = Iyzp - zTrans * Szp - yTrans * Sy->valueNormal() + yTrans * zTrans * Ap;
        double cos2a = cos(2.0*rot);
        double sin2a = sin(2.0*rot);

        double ret = 0.50*(IyPrime+IzPrime) - 0.50*cos2a*(IyPrime-IzPrime) + sin2a * IyzPrime;

        if( applyRoundOff->valueNormal() ){
            ret = MRoundOff( ret, 4.0 );
        }
        return ret;
    } else {
        return 0.0;
    }
}

double SectionSteel::IyzNormal(){
    if( m_d->sectionSteelProfile ){
        double yTrans = - translationO->y->valueNormal();
        double zTrans = - translationO->z->valueNormal();
        double rot = - rotationO->valueNormal();
        double Ap = m_d->sectionSteelProfile->ANormal();
        double Syp = m_d->sectionSteelProfile->SyNormal();
        double Szp = m_d->sectionSteelProfile->SzNormal();
        double Iyyp = m_d->sectionSteelProfile->IyyNormal();
        double Izzp = m_d->sectionSteelProfile->IzzNormal();
        double Iyzp = m_d->sectionSteelProfile->IyzNormal();

        double IyPrime = Iyyp - 2.0 * zTrans * Syp + pow(zTrans, 2.0) * Ap;
        double IzPrime = Izzp - 2.0 * yTrans * Szp + pow(yTrans, 2.0) * Ap;
        double IyzPrime = Iyzp - zTrans * Szp - yTrans * Syp + yTrans * zTrans * Ap;
        double cos2a = cos(2.0*rot);
        double sin2a = sin(2.0*rot);

        double ret = 0.50 * sin2a * (IyPrime-IzPrime) + cos2a * IyzPrime;

        if( applyRoundOff->valueNormal() ){
            ret = MRoundOff( ret, 4.0 );
        }
        return ret;
    } else {
        return 0.0;
    }
}

double SectionSteel::Sz3DyDzNormal(){
    if( m_d->sectionSteelProfile ){
        double yTrans = - translationO->y->valueNormal();
        double zTrans = - translationO->z->valueNormal();
        double rot = - rotationO->valueNormal();
        double Ap = m_d->sectionSteelProfile->ANormal();
        double Syp = m_d->sectionSteelProfile->SyNormal();
        double Szp = m_d->sectionSteelProfile->SzNormal();
        double Iyyp = m_d->sectionSteelProfile->IyyNormal();
        double Izzp = m_d->sectionSteelProfile->IzzNormal();
        double Sy3DyDzp = m_d->sectionSteelProfile->Sy3DyDzNormal();
        double Sz3DyDzp = m_d->sectionSteelProfile->Sz3DyDzNormal();
        double Sy2zDyDzp = m_d->sectionSteelProfile->Sy2zDyDzNormal();
        double Syz2DyDzp = m_d->sectionSteelProfile->Syz2DyDzNormal();

        double Sy3PrimeDyDz = Sy3DyDzp - 3.0 * yTrans * Izzp + 3.0 * pow(yTrans, 2.0) * Szp - pow(yTrans, 3.0) * Ap;
        double Sz3PrimeDyDz = Sz3DyDzp - 3.0 * zTrans * Iyyp + 3.0 * pow(zTrans, 2.0) * Syp - pow(zTrans, 3.0) * Ap;
        double Sy2zPrimeDyDz = Sy2zDyDzp - zTrans * Izzp - 2.0 * yTrans * Iyyp + 2.0 * yTrans * zTrans * Szp + pow(yTrans, 2.0) * Syp - pow(yTrans, 2.0) * zTrans * Ap;
        double Syz2PrimeDyDz = Syz2DyDzp - yTrans * Iyyp - 2.0 * zTrans * Iyyp + 2.0 * yTrans * zTrans * Syp + pow(zTrans, 2.0) * Szp - yTrans * pow(zTrans, 2.0) * Ap;

        double cosa = cos( rot );
        double sina = sin( rot );

        double ret = - pow(sina, 3.0) * Sy3PrimeDyDz + 3.0 * pow(sina, 2.0) * cosa * Sy2zPrimeDyDz - \
                3.0 * sina * pow(cosa, 2.0) * Syz2PrimeDyDz + pow(cosa, 3.0) * Sz3PrimeDyDz;

        if( applyRoundOff->valueNormal() ){
            ret = MRoundOff( ret, 5.0 );
        }
        return ret;
    } else {
        return 0.0;
    }
}

double SectionSteel::Sy3DyDzNormal(){
    if( m_d->sectionSteelProfile ){
        double yTrans = - translationO->y->valueNormal();
        double zTrans = - translationO->z->valueNormal();
        double rot = - rotationO->valueNormal();
        double Ap = m_d->sectionSteelProfile->ANormal();
        double Syp = m_d->sectionSteelProfile->SyNormal();
        double Szp = m_d->sectionSteelProfile->SzNormal();
        double Iyyp = m_d->sectionSteelProfile->IyyNormal();
        double Izzp = m_d->sectionSteelProfile->IzzNormal();
        double Sy3DyDzp = m_d->sectionSteelProfile->Sy3DyDzNormal();
        double Sz3DyDzp = m_d->sectionSteelProfile->Sz3DyDzNormal();
        double Sy2zDyDzp = m_d->sectionSteelProfile->Sy2zDyDzNormal();
        double Syz2DyDzp = m_d->sectionSteelProfile->Syz2DyDzNormal();

        double Sy3PrimeDyDz = Sy3DyDzp - 3.0 * yTrans * Izzp + 3.0 * pow(yTrans, 2.0) * Szp - pow(yTrans, 3.0) * Ap;
        double Sz3PrimeDyDz = Sz3DyDzp - 3.0 * zTrans * Iyyp + 3.0 * pow(zTrans, 2.0) * Syp - pow(zTrans, 3.0) * Ap;
        double Sy2zPrimeDyDz = Sy2zDyDzp - zTrans * Izzp - 2.0 * yTrans * Iyyp + 2.0 * yTrans * zTrans * Szp + pow(yTrans, 2.0) * Syp - pow(yTrans, 2.0) * zTrans * Ap;
        double Syz2PrimeDyDz = Syz2DyDzp - yTrans * Iyyp - 2.0 * zTrans * Iyyp + 2.0 * yTrans * zTrans * Syp + pow(zTrans, 2.0) * Szp - yTrans * pow(zTrans, 2.0) * Ap;

        double cosa = cos( rot );
        double sina = sin( rot );

        double ret = pow(cosa, 3.0) * Sy3PrimeDyDz + 3.0 * pow(cosa, 2.0) * sina * Sy2zPrimeDyDz + \
                3.0 * cosa * pow(sina, 2.0) * Syz2PrimeDyDz + pow(sina, 3.0) * Sz3PrimeDyDz;

        if( applyRoundOff->valueNormal() ){
            ret = MRoundOff( ret, 5.0 );
        }
        return ret;
    } else {
        return 0.0;
    }
}

double SectionSteel::Syz2DyDzNormal(){
    if( m_d->sectionSteelProfile ){
        double yTrans = - translationO->y->valueNormal();
        double zTrans = - translationO->z->valueNormal();
        double rot = - rotationO->valueNormal();
        double Ap = m_d->sectionSteelProfile->ANormal();
        double Syp = m_d->sectionSteelProfile->SyNormal();
        double Szp = m_d->sectionSteelProfile->SzNormal();
        double Iyyp = m_d->sectionSteelProfile->IyyNormal();
        double Izzp = m_d->sectionSteelProfile->IzzNormal();
        double Sy3DyDzp = m_d->sectionSteelProfile->Sy3DyDzNormal();
        double Sz3DyDzp = m_d->sectionSteelProfile->Sz3DyDzNormal();
        double Sy2zDyDzp = m_d->sectionSteelProfile->Sy2zDyDzNormal();
        double Syz2DyDzp = m_d->sectionSteelProfile->Syz2DyDzNormal();

        double Sy3PrimeDyDz = Sy3DyDzp - 3.0 * yTrans * Izzp + 3.0 * pow(yTrans, 2.0) * Szp - pow(yTrans, 3.0) * Ap;
        double Sz3PrimeDyDz = Sz3DyDzp - 3.0 * zTrans * Iyyp + 3.0 * pow(zTrans, 2.0) * Syp - pow(zTrans, 3.0) * Ap;
        double Sy2zPrimeDyDz = Sy2zDyDzp - zTrans * Izzp - 2.0 * yTrans * Iyyp + 2.0 * yTrans * zTrans * Szp + pow(yTrans, 2.0) * Syp - pow(yTrans, 2.0) * zTrans * Ap;
        double Syz2PrimeDyDz = Syz2DyDzp - yTrans * Iyyp - 2.0 * zTrans * Iyyp + 2.0 * yTrans * zTrans * Syp + pow(zTrans, 2.0) * Szp - yTrans * pow(zTrans, 2.0) * Ap;

        double cosa = cos( rot );
        double sina = sin( rot );

        double ret = pow(sina, 2.0) * cosa * Sy3PrimeDyDz + ( pow(sina, 3.0) - 2.0 * sina * pow(cosa, 2.0)) * Sy2zPrimeDyDz + \
                ( pow(cosa, 3.0) - 2.0 * pow(sina, 2.0)) * cosa * Syz2PrimeDyDz + sina * pow(cosa, 2.0) * Sz3PrimeDyDz;

        if( applyRoundOff->valueNormal() ){
            ret = MRoundOff( ret, 5.0 );
        }
        return ret;
    } else {
        return 0.0;
    }
}

double SectionSteel::Sy2zDyDzNormal(){
    if( m_d->sectionSteelProfile ){
        double yTrans = - translationO->y->valueNormal();
        double zTrans = - translationO->z->valueNormal();
        double rot = - rotationO->valueNormal();
        double Ap = m_d->sectionSteelProfile->ANormal();
        double Syp = m_d->sectionSteelProfile->SyNormal();
        double Szp = m_d->sectionSteelProfile->SzNormal();
        double Iyyp = m_d->sectionSteelProfile->IyyNormal();
        double Izzp = m_d->sectionSteelProfile->IzzNormal();
        double Sy3DyDzp = m_d->sectionSteelProfile->Sy3DyDzNormal();
        double Sz3DyDzp = m_d->sectionSteelProfile->Sz3DyDzNormal();
        double Sy2zDyDzp = m_d->sectionSteelProfile->Sy2zDyDzNormal();
        double Syz2DyDzp = m_d->sectionSteelProfile->Syz2DyDzNormal();

        double Sy3PrimeDyDz = Sy3DyDzp - 3.0 * yTrans * Izzp + 3.0 * pow(yTrans, 2.0) * Szp - pow(yTrans, 3.0) * Ap;
        double Sz3PrimeDyDz = Sz3DyDzp - 3.0 * zTrans * Iyyp + 3.0 * pow(zTrans, 2.0) * Syp - pow(zTrans, 3.0) * Ap;
        double Sy2zPrimeDyDz = Sy2zDyDzp - zTrans * Izzp - 2.0 * yTrans * Iyyp + 2.0 * yTrans * zTrans * Szp + pow(yTrans, 2.0) * Syp - pow(yTrans, 2.0) * zTrans * Ap;
        double Syz2PrimeDyDz = Syz2DyDzp - yTrans * Iyyp - 2.0 * zTrans * Iyyp + 2.0 * yTrans * zTrans * Syp + pow(zTrans, 2.0) * Szp - yTrans * pow(zTrans, 2.0) * Ap;

        double cosa = cos( rot );
        double sina = sin( rot );

        double ret = - sina * pow(cosa, 2.0) * Sy3PrimeDyDz + ( pow(cosa, 3.0) - 2.0 * pow(sina,2.0) * cosa) * Sy2zPrimeDyDz - \
                ( pow(sina, 3.0) - 2.0 * sina * pow(cosa, 2.0)) * Syz2PrimeDyDz + pow(sina,2.0) * cosa * Sz3PrimeDyDz;

        if( applyRoundOff->valueNormal() ){
            ret = MRoundOff( ret, 5.0 );
        }
        return ret;
    } else {
        return 0.0;
    }
}

double SectionSteel::tyNormal(){
    if( m_d->sectionSteelProfile ){
        return m_d->sectionSteelProfile->tyNormal();
    } else {
        return 1.2;
    }
}

double SectionSteel::tzNormal(){
    if( m_d->sectionSteelProfile ){
        return m_d->sectionSteelProfile->tzNormal();
    } else {
        return 1.2;
    }
}

double SectionSteel::tyzNormal(){
    if( m_d->sectionSteelProfile ){
        return m_d->sectionSteelProfile->tyzNormal();
    } else {
        return 0.0;
    }
}

void SectionSteel::setWElMy( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->WElMyNormal();
    }
    WElMy->setValueNormal( ret, emitAuto );
}

void SectionSteel::setWElMz( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->WElMzNormal();
    }
    WElMz->setValueNormal( ret, emitAuto );
}

void SectionSteel::setWPlMy( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->WPlMyNormal();
    }
    WPlMy->setValueNormal( ret, emitAuto);
}

void SectionSteel::setWPlMz( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->WPlMzNormal();
    }
    WPlMz->setValueNormal( ret, emitAuto );
}

void SectionSteel::setEffN( bool emitAuto ){
    double AEff = 0.0, yGEff = 0.0, zGEff = 0.0;
    if( m_d->sectionSteelProfile ){
        m_d->sectionSteelProfile->EffNMinusNormal( &AEff, &yGEff, &zGEff );
    }
    if( GEffN->y->readOnly() ){
        GEffN->y->setValueNormal( yGEff, emitAuto );
    }
    if( GEffN->z->readOnly() ){
        GEffN->z->setValueNormal( zGEff, emitAuto );
    }
    if( AEffN->readOnly() ){
        AEffN->setValueNormal( AEff, emitAuto );
    }
}

void SectionSteel::setWEffMyMax( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->WEffMyMaxNormal();
    }
    WEffMyMax->setValueNormal( ret, emitAuto );
}

void SectionSteel::setWEffMyMin( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->WEffMyMinNormal();
    }
    WEffMyMin->setValueNormal( ret, emitAuto );
}

void SectionSteel::setWEffMzMax( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->WEffMzMaxNormal();
    }
    WEffMzMax->setValueNormal( ret, emitAuto );
}

void SectionSteel::setWEffMzMin( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->WEffMzMinNormal();
    }
    WEffMzMin->setValueNormal( ret, emitAuto );
}

void SectionSteel::setAVy( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->AVyNormal();
    }
    AVy->setValueNormal( ret, emitAuto );
}

void SectionSteel::setAVz( bool emitAuto ){
    double ret = 0.0;
    if( m_d->sectionSteelProfile ){
        ret = m_d->sectionSteelProfile->AVzNormal();
    }
    AVz->setValueNormal( ret, emitAuto );
}

void SectionSteel::setNClass( bool emitAuto ){
    if( steel() && m_d->sectionSteelProfile ){
        NClass->setValueNormal( m_d->sectionSteelProfile->NClass( ), emitAuto );
    }
}

void SectionSteel::setMyMaxClass( bool emitAuto ){
    if( steel() && m_d->sectionSteelProfile ){
        MyMaxClass->setValueNormal( m_d->sectionSteelProfile->MyMaxClass(), emitAuto );
    }
}

void SectionSteel::setMyMinClass( bool emitAuto ){
    if( steel() && m_d->sectionSteelProfile ){
        MyMinClass->setValueNormal( m_d->sectionSteelProfile->MyMinClass(), emitAuto );
    }
}

void SectionSteel::setMyClassLimit( bool emitAuto ){
    if( steel() && m_d->sectionSteelProfile ){
        MyClassLimit->setValueNormal( m_d->sectionSteelProfile->MyClassLimitNormal(), emitAuto );
    }
}

void SectionSteel::setMzMaxClass( bool emitAuto ){
    if( MzMaxClass->readOnly() ){
        if( steel() && m_d->sectionSteelProfile ){
            MzMaxClass->setValueNormal( m_d->sectionSteelProfile->MzMaxClass(),emitAuto );
        }
    }
}

void SectionSteel::setMzMinClass( bool emitAuto ){
    if( MzMinClass->readOnly() ){
        if( steel() && m_d->sectionSteelProfile ){
            MzMinClass->setValueNormal( m_d->sectionSteelProfile->MzMinClass(), emitAuto );
        }
    }
}

void SectionSteel::setMzClassLimit( bool emitAuto ){
    if( MzClassLimit->readOnly() ){
        if( steel() && m_d->sectionSteelProfile ){
            MzClassLimit->setValueNormal( m_d->sectionSteelProfile->MzClassLimitNormal(), emitAuto );
        }
    }
}

void SectionSteel::setNULSMax( bool emitAuto ){
    if( NULSMax->readOnly() ){
        NULSMax->setValueNormal( fydNormal(0) * A->valueNormal(), emitAuto );
    }
}

void SectionSteel::setNULSMin( bool emitAuto ){
    if( NULSMin->readOnly() ){
        if( NClass->valueStr().startsWith('1') || NClass->valueStr().startsWith('2') || NClass->valueStr().startsWith('3') ){
            NULSMin->setValueNormal( - fydNormal(0) * A->valueNormal(), emitAuto );
        } else if( NClass->valueStr().startsWith('4') ){
            NULSMin->setValueNormal( - fydNormal(0) * AEffN->valueNormal(), emitAuto );
        } else {
            NULSMin->setValueNormal( 0.0, emitAuto );
        }
    }
}

void SectionSteel::setMULSyMax( bool emitAuto ){
    if( MULSyMax->readOnly() ){
        if( MyMaxClass->valueStr().startsWith('1') || MyMaxClass->valueStr().startsWith('2') ){
            MULSyMax->setValueNormal( fydNormal(0) * WPlMy->valueNormal(), emitAuto );
        } else if( MyMaxClass->valueStr().startsWith('3') ) {
            MULSyMax->setValueNormal( fydNormal(0) * WElMy->valueNormal(), emitAuto );
        } else if( MyMaxClass->valueStr().startsWith('4') ){
            MULSyMax->setValueNormal( fydNormal(0) * WEffMyMax->valueNormal(), emitAuto );
        } else {
            MULSyMax->setValueNormal( 0.0, emitAuto );
        }
    }
}

void SectionSteel::setMULSyMin( bool emitAuto ){
    if( MULSyMin->readOnly() ){
        if( MyMinClass->valueStr().startsWith('1') || MyMinClass->valueStr().startsWith('2') ){
            MULSyMin->setValueNormal( - fydNormal(0) * WPlMy->valueNormal(), emitAuto);
        } else if( MyMinClass->valueStr().startsWith('3') ) {
            MULSyMin->setValueNormal( - fydNormal(0) * WElMy->valueNormal(), emitAuto );
        } else if( MyMinClass->valueStr().startsWith('4') ){
            MULSyMin->setValueNormal( fydNormal(0) * WEffMyMin->valueNormal(), emitAuto );
        } else {
            MULSyMin->setValueNormal( 0.0, emitAuto );
        }
    }
}

void SectionSteel::setMULSzMax( bool emitAuto ){
    if( MULSzMax->readOnly() ){
        if( MzMaxClass->valueStr().startsWith('1') || MzMaxClass->valueStr().startsWith('2') ){
            MULSzMax->setValueNormal( fydNormal(0) * WPlMz->valueNormal(), emitAuto );
        } else if( MzMaxClass->valueStr().startsWith('3') ) {
            MULSzMax->setValueNormal( fydNormal(0) * WElMz->valueNormal(), emitAuto );
        } else if( MzMaxClass->valueStr().startsWith('4') ){
            MULSzMax->setValueNormal( fydNormal(0) * WEffMzMax->valueNormal(), emitAuto );
        } else {
            MULSzMax->setValueNormal( 0.0, emitAuto );
        }
    }
}

void SectionSteel::setMULSzMin( bool emitAuto ){
    if( MULSzMin->readOnly() ){
        if( MzMinClass->valueStr().startsWith('1') || MzMinClass->valueStr().startsWith('2') ){
            MULSzMin->setValueNormal( - fydNormal(0) * WPlMz->valueNormal(), emitAuto );
        } else if( MzMinClass->valueStr().startsWith('3') ) {
            MULSzMin->setValueNormal( - fydNormal(0) * WElMz->valueNormal(), emitAuto );
        } else if( MzMinClass->valueStr().startsWith('4') ){
            MULSzMin->setValueNormal( fydNormal(0) * WEffMzMin->valueNormal(), emitAuto );
        } else {
            MULSzMin->setValueNormal( 0.0, emitAuto );
        }
    }
}

void SectionSteel::setVULSy( bool emitAuto ){
    if( VULSy->readOnly() ){
        if( steel() ){
            VULSy->setValueNormal( AVy->valueNormal() * steel()->fyd(0)->valueNormal() / pow(3, 0.5), emitAuto );
        } else {
            VULSy->setValueNormal( 0.0, emitAuto );
        }
    }
}

void SectionSteel::setVULSz( bool emitAuto ){
    if( VULSz->readOnly() ){
        if( steel()){
            VULSz->setValueNormal(AVz->valueNormal() * steel()->fyd(0)->valueNormal() / pow(3, 0.5), emitAuto );
        } else {
            VULSz->setValueNormal( 0.0, emitAuto );
        }
    }
}

double SectionSteel::yMaxNormal() {
    if( m_d->sectionPoints == NULL ){
        updateSectionPoints();
    }
    return m_d->sectionPoints->yMaxNormal();
}

double SectionSteel::yMinNormal() {
    if( m_d->sectionPoints == NULL ){
        updateSectionPoints();
    }
    return m_d->sectionPoints->yMinNormal();
}

double SectionSteel::zMinNormal() {
    if( m_d->sectionPoints == NULL ){
        updateSectionPoints();
    }
    return m_d->sectionPoints->zMinNormal();
}

double SectionSteel::zMaxNormal() {
    if( m_d->sectionPoints == NULL ){
        updateSectionPoints();
    }
    return m_d->sectionPoints->zMaxNormal();
}

bool SectionSteel::sMinNormal( double *ret, double my, double mz ) {
    if( m_d->sectionPoints == NULL ){
        updateSectionPoints();
    }
    return m_d->sectionPoints->sMinNormal(ret, my, mz );
}

bool SectionSteel::sMaxNormal( double *ret, double my, double mz ) {
    if( m_d->sectionPoints == NULL ){
        updateSectionPoints();
    }
    return m_d->sectionPoints->sMaxNormal(ret, my, mz );
}

double SectionSteel::NNetULSMaxNormal( double a ){
    if( steel() ){
        double aNet = 0.0;
        if( a == 0.0 ){
            aNet = A->valueNormal();
        } else {
            aNet = a;
        }
        return (0.90 * steel()->fyd(2)->valueNormal() * aNet);
    } else {
        return 0.0;
    }
}

double SectionSteel::beamlRelBuckl0Normal(){
    return 0.20;
}

double SectionSteel::beamlRelBucklNormal( double NCrInput ){
    if( NCrInput != 0.0 ){
        return pow( A->valueNormal() * fykNormal() / NCrInput ,0.50);
    } else {
        return 0.0;
    }
}

double SectionSteel::alphaBucklYNormal(){
    double ret = 0.49;
    if( sectionShape->valueNormal() == SectionSteel::sectionI ){
        if( manufactType->valueNormal() == SectionSteel::sectionRolled ){
            if( m_d->p[0]->valueNormal() / m_d->p[1]->valueNormal() > 1.2 ){
                if( m_d->p[3]->valueNormal() <= 0.04 ){
                    if( steel()->fyk->valueNormal() >= 460.0e+6 ){
                        ret = 0.13;
                    } else {
                        ret = 0.21;
                    }
                } else if( m_d->p[3]->valueNormal() <= 0.10 ){
                    if( steel()->fyk->valueNormal() >= 460.0e+6 ){
                        ret = 0.21;
                    } else {
                        ret = 0.34;
                    }
                }
            } else {
                if( m_d->p[3]->valueNormal() <= 0.10 ){
                    if( steel()->fyk->valueNormal() >= 460.0e+6 ){
                        ret = 0.21;
                    } else {
                        ret = 0.34;
                    }
                } else {
                    if( steel()->fyk->valueNormal() >= 460.0e+6 ){
                        ret = 0.49;
                    } else {
                        ret = 0.76;
                    }
                }
            }
        } else if( manufactType->valueNormal() == SectionSteel::sectionWelded ){
            if( m_d->p[3]->valueNormal() <= 0.04 ){
                ret = 0.34;
            } else {
                ret = 0.49;
            }
        }
    } else if( sectionShape->valueNormal() == SectionSteel::sectionCirc ){
        if( manufactType->valueNormal() == SectionSteel::sectionRolled ){
            if( steel()->fyk->valueNormal() >= 460.0e+6 ){
                ret = 0.13;
            } else {
                ret = 0.21;
            }
        } else if( manufactType->valueNormal() == SectionSteel::sectionColdFormed ){
            ret = 0.49;
        }
    }
    return ret;
}

double SectionSteel::alphaBucklZNormal(){
    double ret = 0.49;
    if( sectionShape->valueNormal() == SectionSteel::sectionI ){
        if( manufactType->valueNormal() == SectionSteel::sectionRolled ){
            if( m_d->p[0]->valueNormal() / m_d->p[1]->valueNormal() > 1.2 ){
                if( m_d->p[3]->valueNormal() <= 0.04 ){
                    if( steel()->fyk->valueNormal() >= 460.0e+6 ){
                        ret = 0.13;
                    } else {
                        ret = 0.34;
                    }
                } else if( m_d->p[3]->valueNormal() <= 0.10 ){
                    if( steel()->fyk->valueNormal() >= 460.0e+6 ){
                        ret = 0.21;
                    } else {
                        ret = 0.49;
                    }
                }
            } else {
                if( m_d->p[3]->valueNormal() <= 0.10 ){
                    if( steel()->fyk->valueNormal() >= 460.0e+6 ){
                        ret = 0.21;
                    } else {
                        ret = 0.49;
                    }
                } else {
                    if( steel()->fyk->valueNormal() >= 460.0e+6 ){
                        ret = 0.49;
                    } else {
                        ret = 0.76;
                    }
                }
            }
        } else if( manufactType->valueNormal() == SectionSteel::sectionWelded ){
            if( m_d->p[3]->valueNormal() <= 0.04 ){
                ret = 0.49;
            } else {
                ret = 0.76;
            }
        }
    } else if( sectionShape->valueNormal() == SectionSteel::sectionCirc ){
        if( manufactType->valueNormal() == SectionSteel::sectionRolled ){
            if( steel()->fyk->valueNormal() >= 460.0e+6 ){
                ret = 0.13;
            } else {
                ret = 0.21;
            }
        } else if( manufactType->valueNormal() == SectionSteel::sectionColdFormed ){
            ret = 0.49;
        }
    }
    return ret;
}

double SectionSteel::beamChiBucklYNormal( double lRelY ){
    double ret = 1.0;
    if( lRelY >= beamlRelBuckl0Normal() ){
        double phi = 0.50 * (1.0 + alphaBucklYNormal() * (lRelY - 0.20) + pow(lRelY, 2.0));
        double D = (phi + pow( pow(phi, 2.0) - pow(lRelY, 2.0), 0.50 ));
        if( D != 0.0 ){
            ret = 1.0 / D;
        }
        if( ret > 1.0 ){
            ret = 1.0;
        }
    }
    return ret;
}

double SectionSteel::beamChiBucklZNormal( double lRelZ ){
    double ret = 1.0 ;
    if( lRelZ >= beamlRelBuckl0Normal() ){
        double phi = 0.50 * (1.0 + alphaBucklZNormal() * (lRelZ - 0.20) + pow(lRelZ, 2.0));
        double D = (phi + pow( pow(phi, 2.0) - pow(lRelZ, 2.0), 0.50 ));
        if( D != 0.0 ){
            ret = 1.0 / D;
        }
        if( ret > 1.0 ){
            ret = 1.0;
        }
    }
    return ret;
}

double SectionSteel::beamNULSBucklNormal( double chi ){
    return (- fydNormal(1) * A->valueNormal() * chi);
}

void SectionSteel::beamlRelBuckl( DoublePlus * ret, DoublePlus * NCrInput ){
    ret->setValueNormal( beamlRelBucklNormal( NCrInput->valueNormal()) );
}

void SectionSteel::beamChiBucklY( DoublePlus * ret, DoublePlus * lRelY ){
    ret->setValueNormal( beamChiBucklYNormal( lRelY->valueNormal() ));
}

void SectionSteel::beamChiBucklZ(  DoublePlus * ret, DoublePlus * lRelZ ){
    ret->setValueNormal( beamChiBucklZNormal( lRelZ->valueNormal() ));
}

void SectionSteel::beamNULSBuckl( DoublePlus * ret, DoublePlus * chi ){
    ret->setValueNormal( beamNULSBucklNormal( chi->valueNormal() ));
}

double SectionSteel::alphaBucklLTNormal(){
    double ret = 0.76;
    if( sectionShape->valueNormal() == SectionSteel::sectionI ){
        if( manufactType->valueNormal() == SectionSteel::sectionRolled ){
            if( m_d->p[0]->valueNormal() / m_d->p[1]->valueNormal() <= 2.0 ){
                ret = 0.34;
            } else {
                ret = 0.49;
            }
        } else if( manufactType->valueNormal() == SectionSteel::sectionWelded ){
            if( m_d->p[0]->valueNormal() / m_d->p[1]->valueNormal() <= 2.0 ){
                ret = 0.49;
            } else {
                ret = 0.76;
            }
        }
    }
    return ret;
}

double SectionSteel::beamMCrLTNormal( double Ly, double MH1, double MH2 ){
    if( Ly != 0.0 && steel() != NULL && IT->valueNormal() != 0.0 ){
        if( steel()->G->valueNormal() != 0.0 ){
            double ratioM = 0.0;
            if( MH1 == MH2 ){
                ratioM = 1.0;
            } else if( MH2 != 0.0){
                ratioM = MH1 /MH2;
            }
            if( fabs(ratioM) > 1.0 ){
                ratioM = 1 / ratioM;
            }
            double phi = 1.75 - 1.05 * ratioM + 0.30 * pow(ratioM, 2.0);
            double M = phi * M_PI / Ly * \
                    pow( steel()->E->valueNormal() * Izz->valueNormal() * steel()->G->valueNormal() * IT->valueNormal(), 0.50) * \
                    pow( 1.0 + pow(M_PI/Ly, 2.0) * steel()->E->valueNormal() * Iw->valueNormal() / (steel()->G->valueNormal() * IT->valueNormal()), 0.50);
            return M;
        }
    }
    return 0.0;
}

double SectionSteel::beamlRelBucklLT0Normal(){
    double l0 = 0.0;
    switch( manufactType->valueNormal() ){
    case SectionSteel::sectionRolled: {
        l0 = 0.40;
        break;}
    case SectionSteel::sectionWelded: {
        l0 = 0.40;
        break;}
    default:{
        l0 = 0.20;
        break;}
    }
    return l0;
}

double SectionSteel::beamlRelBucklLTNormal( double MCrInput ){
    if( MCrInput != 0.0 ){
        double W = 0.0;
        if( MyMaxClass->valueStr().startsWith("1") || MyMaxClass->valueStr().startsWith("2")){
            W = WPlMy->valueNormal();
        }else if( MyMaxClass->valueStr().startsWith("3") ){
            W = WElMy->valueNormal();
        }
        return pow( W * fykNormal() / MCrInput ,0.50);
    }
    return 0.0;
}

double SectionSteel::beamChiBucklLTNormal( double MH1, double MH2,  SectionSteel::MomentDistribution md , double lRelLT ){
    double ret = 1.0;
    if ( lRelLT > beamlRelBucklLT0Normal() ){
        double kc =0.77;
        switch( md ){
        case SectionSteel::momentLinear : {
            if( MH1 == 0.0 && MH2 == 0.0){
                kc = 1.0;
            } else {
                double phi = 1.0;
                if( MH1 != 0.0 ){
                    phi = MH2 / MH1;
                } else {
                    phi = 0.0;
                }
                if( fabs(phi) > 1.0 ){
                    phi = 1.0 / phi;
                }
                kc = 1.0 / (1.33 * 0.33 * phi);
            }
            break; }
        case SectionSteel::momentBiLinear : {
            if( MH1 * MH2 != 0.0 ){
                kc = 0.77;
            } else if( MH1 == 0.0 && MH2 == 0.0 ){
                kc = 0.86;
            } else {
                kc = 0.82;
            }
            break; }
        case SectionSteel::momentParabolic : {
            if( MH1 * MH2 != 0.0 ){
                kc = 0.90;
            } else if( MH1 == 0.0 && MH2 == 0.0 ){
                kc = 0.94;
            } else {
                kc = 0.91;
            }
            break; }
        }

        double f = 1.0 - 0.50 * (1.0 - kc) * (1.0 - 2.0 * pow(lRelLT - 0.80, 2.0));

        double beta = 1.0;
        switch( manufactType->valueNormal() ){
        case SectionSteel::sectionRolled: {
            beta = 0.75;
            break;}
        case SectionSteel::sectionWelded: {
            beta = 0.75;
            break;}
        default:{
            beta = 1.0;
            break;}
        }

        double phi = 0.50 * ( 1.0 + alphaBucklLTNormal() * (lRelLT - beamlRelBucklLT0Normal() )+ beta * pow(lRelLT, 2.0) );
        ret = 1.0 / ( f * ( phi + pow(pow(phi, 2.0) - beta * pow(lRelLT, 2.0), 0.50)) );
        if( ret > 1.0 )
            ret = 1.0;
        if( ret > (1.0 / (f * pow(lRelLT, 2.0))) )
            ret = (1.0 / (f * pow(lRelLT, 2.0)));
    }
    return ret;
}

double SectionSteel::beamMULSBucklLTMaxNormal( double chiY ){
    double W = 0.0;
    if( MyMaxClass->valueStr().startsWith("1") || MyMaxClass->valueStr().startsWith("2")){
        W = WPlMy->valueNormal();
    } else if( MyMaxClass->valueStr().startsWith("3") ){
        W = WElMy->valueNormal();
    } else if( MyMaxClass->valueStr().startsWith("4") ){
        W = WEffMyMax->valueNormal();
    }
    return (W * fydNormal(1) * chiY);
}

double SectionSteel::beamMULSBucklLTMinNormal( double chiY ){
    double W = 0.0;
    if( MyMinClass->valueStr().startsWith("1") || MyMinClass->valueStr().startsWith("2")){
        W = - WPlMy->valueNormal();
    } else if( MyMinClass->valueStr().startsWith("3") ){
        W = - WElMy->valueNormal();
    } else if( MyMinClass->valueStr().startsWith("4") ){
        W = WEffMyMin->valueNormal();
    }
    return (W * fydNormal(1) * chiY);
}

void SectionSteel::beamlRelBucklLT( DoublePlus *ret, DoublePlus * MCrInput ){
    ret->setValueNormal( beamlRelBucklLTNormal(MCrInput->valueNormal()));
}

void SectionSteel::beamlRelBucklLT0( DoublePlus *ret ){
    ret->setValueNormal( beamlRelBucklLT0Normal() );
}

void SectionSteel::beamChiBucklLT( DoublePlus * ret, DoublePlus * MH1, DoublePlus * MH2, SectionSteelMomentDistribution * md , DoublePlus * lRelY ){
    ret->setValueNormal( beamChiBucklLTNormal(MH1->valueNormal(), MH2->valueNormal(), md->valueNormal(), lRelY->valueNormal()));
}

void SectionSteel::beamMCrLT( DoublePlus *ret, DoublePlus * Ly, DoublePlus * MH1, DoublePlus * MH2 ){
    ret->setValueNormal( beamMCrLTNormal( Ly->valueNormal(), MH1->valueNormal(), MH2->valueNormal()));
}


void SectionSteel::beamMULSBucklLTMax( DoublePlus * ret, DoublePlus * chiY ){
    ret->setValueNormal( beamMULSBucklLTMaxNormal( chiY->valueNormal() ));
}

void SectionSteel::beamMULSBucklLTMin( DoublePlus * ret, DoublePlus * chiY ){
    ret->setValueNormal( beamMULSBucklLTMinNormal( chiY->valueNormal() ));
}

double SectionSteel::fElNormal( double N, double My, double Mz){
    double ret = 0.0;
    if( A->valueNormal() != 0 && WElMy->valueNormal() != 0 && WElMz->valueNormal() != 0){
        ret = fabs(N) / A->valueNormal() - fabs(My) / WElMy->valueNormal() - fabs(Mz) / WElMz->valueNormal();
    }
    if( N < 0.0){
        ret = -ret;
    }
    return ret;
}

void SectionSteel::NMULSPlastNormal( double *NRet, double *MyRet, double * MzRet,
                                     double l, double my, double mz,
                                     double yCen, double zCen ){
    if( m_d->sectionSteelProfile ){
        m_d->sectionSteelProfile->NMULSPlastNormal( NRet, MyRet, MzRet, l, my, mz, yCen, zCen);
    } else {
        *NRet = 0.0;
        *MyRet = 0.0;
        *MzRet = 0.0;
    }
}

void SectionSteel::NMULSElastPlastNormal( double * NRet, double *MyRet, double * MzRet,
                                          double l, double my, double mz,
                                          double yCen, double zCen ){
    if( m_d->sectionSteelProfile ){
        m_d->sectionSteelProfile->NMULSElastPlastNormal( NRet, MyRet, MzRet, l, my, mz, yCen, zCen);
    } else {
        *NRet = 0.0;
        *MyRet = 0.0;
        *MzRet = 0.0;
    }
}

void SectionSteel::MULSNormal( double * yRet, double *zRet,
                               double NSd,
                               double yCen, double zCen,
                               double rot,
                               double accuracy){
    updateSectionPoints();
    if( m_d->sectionSteelProfile ){
        if( NSd >= NULSMax->valueNormal() ){
            *yRet = 0.0;
            *zRet = 0.0;
            return;
        }
        if( NSd <= NULSMin->valueNormal() ){
            *yRet = 0.0;
            *zRet = 0.0;
            return;
        }
        if( m_d->sectionSteelProfile->nPoints() > 0 ){
            double mz = sin( rot );
            double my = cos( rot );
            double y, z;
            m_d->sectionSteelProfile->point(0, &y, &z);
            double lMin = -(mz * y + my * z);
            double lMax = lMin;
            for( int i=1; i < m_d->sectionSteelProfile->nPoints(); i++){
                m_d->sectionSteelProfile->point(i, &y, &z);
                double l = -( mz * y + my * z );
                if( l > lMax ){
                    lMax = l;
                }
                if( l < lMin ){
                    lMin = l;
                }
            }
            if( lMin == lMax ){
                *yRet = 0.0;
                *zRet = 0.0;
                return;
            }
            double NMin = 0.0;
            NMULSPlastNormal( &NMin, NULL, NULL, lMin, my, mz, 0.0, 0.0 );
            double NMax = 0.0;
            NMULSPlastNormal( &NMax, NULL, NULL, lMax, my, mz, 0.0, 0.0 );
            if( NMin > NMax ){
                double Nc = NMin;
                double lc = lMin;
                NMin = NMax;
                lMin = lMax;
                NMax = Nc;
                lMax = lc;
            }
            if( fabs((NMin - NSd) / NULSMax->valueNormal()) < accuracy ){
                NMULSPlastNormal( NULL, yRet, zRet, lMin, my, mz, yCen, zCen );
                return;
            }
            if( fabs((NMax - NSd) / NULSMax->valueNormal()) < accuracy ){
                NMULSPlastNormal( NULL, yRet, zRet, lMax, my, mz, yCen, zCen );
                return;
            }
            double lm = (lMin + lMax) * 0.50;
            double Nm = 0.0;
            NMULSPlastNormal( &Nm, NULL, NULL, lm, my, mz, 0.0, 0.0 );
            double Dn = fabs((Nm - NSd) / NULSMax->valueNormal());
            int i = 0; // contatore per verificare numero max iterazioni
            while( Dn > accuracy && i < MAX_ITER_SECTSTEEL ){
                lm = (lMin + lMax) * 0.50;
                NMULSPlastNormal( &Nm, NULL, NULL, lm, my, mz, 0.0, 0.0 );
                if( Nm < NSd ){
                    NMin = Nm;
                    lMin = lm;
                } else {
                    NMax = Nm;
                    lMax = lm;
                }
                Dn = fabs((Nm - NSd) / NULSMax->valueNormal());
                i++;
            }
            if( Dn > accuracy ){
                *yRet = 0.0;
                *zRet = 0.0;
                return;
            } else {
                m_d->sectionSteelProfile->NMULSPlastNormal( NULL, yRet, zRet, lm, my, mz, yCen, zCen );
                return;
            }
        }
    }
}

void SectionSteel::MULS( DoublePlus * yRet, DoublePlus *zRet,
                         DoublePlus * NSd,
                         Point2DPlus * cen,
                         DoublePlus * rot,
                         DoublePlus * accuracy){
    double yM = 0.0, zM = 0.0;
    double acc = 1.0e-6;
    if( accuracy ){ // accuracy != NULL
        acc = accuracy->valueNormal();
    }
    double yCen = 0.0, zCen = 0.0;
    if( cen ){
        yCen = cen->y->valueNormal();
        zCen = cen->z->valueNormal();
    }
    double r = 0.0;
    if( rot ){
        r = rot->valueNormal();
    }
    MULSNormal( &yM, &zM, NSd->valueNormal(), yCen, zCen, r, acc);
    yRet->setValueNormal( yM );
    zRet->setValueNormal( zM );
}

QPolygonF SectionSteel::MULSyMULSzPlastNormal( double NSd, double yCen, double zCen, int propNP){
    int nP = propNP > 4 ? propNP: 4;
    double dRot = 2.0 * M_PI / nP;

    double yRet = 0.0, zRet = 0.0;
    double rot = 0.0;

    QPolygonF polygon;
    for( int i=0; i < nP; i++){
        MULSNormal( &yRet, &zRet, NSd, yCen, zCen, rot, 1.0e-6 );
        polygon << QPointF(yRet, zRet);
        rot += dRot;
    }
    polygon << polygon.at(0);

    return polygon;
}


QPolygonF SectionSteel::MULSyMULSzPlast( DoublePlus * NSd, Point2DPlus * cen, int propNP){
    QPolygonF polygon = MULSyMULSzPlastNormal( NSd->valueNormal(), cen->y->valueNormal(), cen->z->valueNormal(), propNP );
    for( int i=0; i < polygon.size(); i++ ){
        polygon[i] *= m_unitMeasure->scale(UnitMeasure::loadM);
    }
    return polygon;
}

QPolygonF SectionSteel::MULSyMULSzElastNormal( double NSd, double yCen, double zCen, int propNP){
    int nP = propNP > 4 ? propNP: 4;
    double dRot = 2.0 * M_PI / nP;

    double rot = 0.0;

    QPolygonF polygon;
    for( int i=0; i < nP; i++){
        double M = (fydNormal(0) - fabs(NSd / A->valueNormal())) / ( fabs(cos(rot) / WElMy->valueNormal()) + fabs(sin(rot)/WElMz->valueNormal()));
        double My = fabs(M) * cos(rot) - NSd * zCen;
        double Mz = fabs(M) * sin(rot) + NSd * yCen;
        polygon << QPointF( My, Mz );
        rot += dRot;
    }
    polygon << polygon.at(0);
    return polygon;
}

QPolygonF SectionSteel::MULSyMULSzElast( DoublePlus * NSd, Point2DPlus * cen, int propNP){
    QPolygonF polygon = MULSyMULSzElastNormal( NSd->valueNormal(), cen->y->valueNormal(), cen->z->valueNormal(), propNP );
    for( int i=0; i < polygon.size(); i++ ){
        polygon[i] *= m_unitMeasure->scale(UnitMeasure::loadM);
    }
    return polygon;
}

QPolygonF SectionSteel::MULSyMULSz( DoublePlus * NSd, Point2DPlus * cen, int propNP){
    int cl;
    if( NSd->valueNormal() > MyClassLimit ->valueNormal() && NSd->valueNormal() > MzClassLimit ->valueNormal() ){
        cl = QString( MyMaxClass->valueStr().at(0) ).toInt();
        if( QString( MzMaxClass->valueStr().at(0) ).toInt() > cl ){
            cl = QString( MzMaxClass->valueStr().at(0)).toInt();
        }
    } else {
        cl = QString( NClass->valueStr().at(0) ).toInt();
    }
    if( cl <= 2 ){
        return MULSyMULSzPlast( NSd, cen, propNP);
    } else if( cl == 3 ){
        return MULSyMULSzElast( NSd, cen, propNP);
    } else {
        return QPolygonF();
    }
}

QPolygonF SectionSteel::MULSN( Point2DPlus * cen, DoublePlus * rot, int propNP){
    int nP = propNP > 4 ? propNP: 4;

    double yRet = 0.0, zRet = 0.0;
    double dN = (NULSMax->valueNormal() - NULSMin->valueNormal()) / nP;
    double N = NULSMin->valueNormal();

    QPolygonF polygon;
    while( N <= NULSMax->valueNormal() ){
        MULSNormal( &yRet, &zRet, N, cen->y->valueNormal(), cen->z->valueNormal(), rot->valueNormal() );
        polygon << QPointF( N * m_unitMeasure->scale(UnitMeasure::loadF), (yRet * cos(rot->valueNormal()) + zRet * sin(rot->valueNormal()) ) * m_unitMeasure->scale(UnitMeasure::loadM) );
        N += dN;
    }

    N = NULSMax->valueNormal();
    while( N >= NULSMin->valueNormal() ){
        MULSNormal( &yRet, &zRet, N, cen->y->valueNormal(), cen->z->valueNormal(), (rot->valueNormal()+M_PI) );
        polygon << QPointF( N * m_unitMeasure->scale(UnitMeasure::loadF), (yRet * cos(rot->valueNormal()) + zRet * sin(rot->valueNormal())) * m_unitMeasure->scale(UnitMeasure::loadM) );
        N -= dN;
    }
    polygon << polygon.at(0);

    return polygon;
}

bool SectionSteel::verifyULSNormal( double lRelY,
                                    double chiY,
                                    double lRelZ,
                                    double chiZ,
                                    double N,
                                    SectionSteel::TorsionalDeformability torsDef,
                                    double chiLT,
                                    SectionSteel::MomentDistribution myd,
                                    double MyH1,
                                    double MyS,
                                    double MyH2,
                                    SectionSteel::MomentDistribution mzd,
                                    double MzH1,
                                    double MzS,
                                    double MzH2,
                                    SectionSteel::MomentDistribution mltd,
                                    double MltH1,
                                    double MltS,
                                    double MltH2,
                                    double yM,
                                    double zM,
                                    double MyInput,
                                    double MzInput,
                                    double Vy,
                                    double Vz,
                                    double * res,
                                    QString * clString,
                                    QString * messages,
                                    bool calcDomMyMz ){
    // Azzera la variabile contenente i messaggi
    if( messages ){
        messages->clear();
    }

    // Riferisce i momenti agenti all'orogine del sistema di riferimento
    double My = MyInput - N * yM;
    double Mz = MzInput + N * zM;


    // Controlla se i valori di N, M e T sono oltre i valori di resistenza nel caso di sollecitazione semplice
    if( N > NULSMax->valueNormal() ){
        if( messages ){
            messages->append( trUtf8("Sforzo normale maggiore della resistenza di calcolo a trazione.\n"));
        }
    }
    if( N < NULSMin->valueNormal() ){
        if( messages ){
            messages->append( trUtf8("Sforzo normale minore della resistenza di calcolo a compressione.\n"));
        }
    }
    if( My > MULSyMax->valueNormal() ){
        if( messages ){
            messages->append( trUtf8("Momento y-y maggiore del momento resistente massimo y-y.\n"));
        }
    }
    if( My < MULSyMin->valueNormal() ){
        if( messages ){
            messages->append( trUtf8("Momento y-y minore del momento resistente minimo y-y.\n"));
        }
    }
    if( Mz > MULSzMax->valueNormal() ){
        if( messages ){
            messages->append( trUtf8("Momento z-z maggiore del momento resistente massimo z-z.\n"));
        }
    }
    if( Mz < MULSzMin->valueNormal() ){
        if( messages ){
            messages->append( trUtf8("Momento z-z minore del momento resistente minimo z-z.\n"));
        }
    }
    if( fabs(Vz) > VULSz->valueNormal() ){
        if( messages ){
            messages->append( trUtf8("Taglio z-z maggiore del taglio resistente z-z.\n"));
        }
    }
    if( fabs(Vy) > VULSy->valueNormal() ){
        if( messages ){
            messages->append( trUtf8("Taglio y-y maggiore del taglio resistente y-y.\n"));
        }
    }

    // Calcola la class di appartenenza della sezione
    int cl = classNormal( N, My, Mz );
    if( clString ){
        clString->setNum( cl );
    }

    // EC3 - 6.2.8 -Influenza del taglio
    double rhoV = 0.0;
    if( fabs(Vy) > (fabs(VULSy->valueNormal()) / 2.0) || fabs(Vz) > (fabs(VULSz->valueNormal()) / 2.0)){
        // Risultante dello sfrozo di taglio agente
        double V = length2D( 0.0, 0.0,  Vy, Vz);
        // Versore dello sforzo di algio
        double vy = Vy / V;
        double vz = Vz / V;
        // Taglio resistente lungo la direzione del taglio agente
        double VULS = length2D( 0.0, 0.0, VULSy->valueNormal() * vy, VULSy->valueNormal() * vz );
        // vedi EC3 6.2.8
        rhoV = pow( (2.0 * V / VULS - 1.0) , 2.0);
    }

    if( (chiY == 1.0 && chiZ == 1.0 && chiLT == 1.0) || (N >= 0.0 && chiLT == 1.0) ){
        // non siamo in presenza di fenomeni di instabilitÃ
        if( cl == 1 || cl == 2 ){
            if( calcDomMyMz ){
                int np = 16;
                QPolygonF intDiagram = MULSyMULSzPlastNormal( N, 0.0, 0.0, np);

                bool ret = intDiagram.containsPoint(QPointF(My, Mz), Qt::OddEvenFill);
                if( ret ){
                    *res = 0.0;
                } else {
                    *res = 100.0;
                }
                return ret;
            } else {
                double n = 0.0;
                if( N * NULSMax->valueNormal() > 0.0 ){
                    n = N / NULSMax->valueNormal();
                } else if( N * NULSMin->valueNormal() > 0.0 ){
                    n = N / NULSMin->valueNormal();
                }

                double MNyRd, MNzRd;

                if( sectionShape->valueNormal() == SectionSteel::sectionI ){
                    double a = 0.0;
                    if( A->valueNormal() != 0.0 ){
                        a = (A->valueNormal() - 2.0 * m_d->p[1]->valueNormal() * m_d->p[3]->valueNormal()) / A->valueNormal();
                        if( a > 0.50 ){
                            a = 0.50;
                        }
                    }
                    double n = 0.0;
                    if( NULSMax->valueNormal() != 0.0 ){
                        n = N / NULSMax->valueNormal();
                    }
                    MNyRd = MULSyMax->valueNormal() * (1.0 - n) / (1.0 - 0.50 *a);
                    if( MNyRd > MULSyMax->valueNormal() ){
                        MNyRd = MULSyMax->valueNormal();
                    }
                    if( n <= a ){
                        MNzRd = MULSzMax->valueNormal();
                    } else {
                        MNzRd = MULSzMax->valueNormal() * (1.0 - pow((n-a)/(1.0-a),2.0));
                    }
                } else {
                    double tmp;
                    MULSNormal( &MNyRd, &tmp, N, 0.0, 0.0, 0.0, 1.0e-6 );
                    MULSNormal( &tmp, &MNzRd, N, 0.0, 0.0, -M_PI/2.0, 1.0e-6 );
                }

                double a = 1.0, b = 1.0;
                if( sectionShape->valueNormal() == SectionSteel::sectionI ){
                    if( n >= 0.2 ){
                        a = 2.0;
                        b = 5.0 * n;
                    } else {
                        // EC 3
                        a = 2.0;
                        b = 1.0;
                    }
                }
                if( sectionShape->valueNormal() == SectionSteel::sectionCirc ){
                    a = 2.0;
                    b = 2.0;
                }

                double check = 1.0;

                *res = 100.0;
                if( MNyRd != 0.0 && MNzRd != 0.0){
                    check = pow(fabs(My / MNyRd), a) + pow(fabs(Mz / MNzRd), b );
                    if( (1.0 - rhoV ) != 0.0 )
                        *res = check / (1.0 - rhoV );
                }
                if( check < (1.0 - rhoV )){
                    return true;
                } else {
                    return false;
                }
            }
        } else if( cl == 3 ){
            double s = pow( fabs(My) / WElMy->valueNormal() + fabs(Mz) / WElMz->valueNormal() + fabs(N) / A->valueNormal(), 2.0) + 3.0 * ( pow(Vy/AVy->valueNormal(),2.0) + pow(Vz/AVz->valueNormal(), 2.0) );
            s = sqrt( s );
            if( fydNormal(0) != 0.0 ){
                *res = s / fydNormal(0);
            }
            if( s <= fydNormal(0)){
                return true;
            } else {
                return false;
            }
        } else if( cl == 4 ){
            if( messages ){
                messages->append( trUtf8("Classe 4: non ancora implementata (calcolata come classe 3)\n") );
            }
            double s = pow( fabs(My) / WElMy->valueNormal() + fabs(Mz) / WElMz->valueNormal() + fabs(N) / A->valueNormal(), 2.0) + 3.0 * ( pow(Vy/AVy->valueNormal(),2.0) + pow(Vz/AVz->valueNormal(), 2.0) );
            s = sqrt( s );
            if( fydNormal(0) != 0.0 ){
                *res = s / fydNormal(0);
            }
            if( s <= fydNormal(0)){
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        // siamo in presenza di fenomeni di instabilitÃ
        double NRd = 0.0, MRdY = 0.0, MRdZ = 0.0;
        if( cl == 1 || cl == 2 ){
            NRd = fydNormal(1) * A->valueNormal();
            MRdY = fydNormal(1) * WPlMy->valueNormal();
            MRdZ = fydNormal(1) * WPlMz->valueNormal();
        } else if( cl == 3 ) {
            NRd = fydNormal(1) * A->valueNormal();
            MRdY = fydNormal(1) * WElMy->valueNormal();
            MRdZ = fydNormal(1) * WElMz->valueNormal();
        } else if( cl == 4 ) {
            // TODO
            if(messages ){
                messages->append( trUtf8("Classe 4: non ancora implementata (calcolata come classe 3)\n") );
            }
            NRd = fydNormal(1) * A->valueNormal();
            MRdY = fydNormal(1) * WElMy->valueNormal();
            MRdZ = fydNormal(1) * WElMz->valueNormal();
        }
        double DNY = NRd * chiY;
        double DNZ = NRd * chiZ;
        double DM = chiLT * MRdY;
        if( (DNY * DNZ * DM * MRdZ) != 0.0 ){
            double amy = Cm( myd, MyH1, MyS, MyH2), amz = Cm(mzd, MzH1, MzS, MzH2), amlt = Cm(mltd, MltH1, MltS, MltH2);
            double kyy = 1.0, kyz = 1.0, kzy = 1.0, kzz = 1.0;
            // kyy
            if( cl == 1 || cl == 2){
                double ly = lRelY < 1.0? lRelY: 1.0;
                kyy = amy * (1.0 + (ly - 0.20) * N / DNY );
            } else {// cl == 3 || cl == 4
                double ly = lRelY < 1.0? lRelY: 1.0;
                kyy = amy * (1.0 + 0.6 * ly * N / DNY );
            }
            // kzz
            if( cl == 1 || cl == 2){
                double lz = lRelZ < 1.0? lRelZ: 1.0;
                if( sectionShape->valueNormal() == SectionSteel::sectionI ){
                    kzz = amz * (1.0 + (2.0*lz-0.60) * N / DNZ );
                } else {
                    kzz = amz * (1.0 + (lz-0.20) * N / DNZ );
                }
            } else {// cl == 3 || cl == 4
                double lz = lRelZ < 1.0? lRelZ: 1.0;
                kzz = amz * (1.0 + 0.6 * lz * N / DNY );
            }
            // kyz
            if( cl == 1 || cl == 2){
                kyz = 0.60 * kzz;
            } else {// cl == 3 || cl == 4
                kyz = kzz;
            }
            //kzy
            if( torsDef == SectionSteel::torsNotDeformable || sectionShape->valueNormal() == SectionSteel::sectionCirc ){
                if( cl == 1 || cl == 2)
                    kzy = 0.60 * kyy;
                else // cl == 3 || cl == 4
                    kzy = 0.80 * kyy;
            } else { // torsDef == SectionSteel::torsDeformable
                if( cl == 1 || cl == 2 ){
                    if( lRelZ < 0.40 ){
                        double ret1 = 1.0 - 0.10 * lRelZ * N / ((amlt - 0.25)*DNZ);
                        double ret2 = 0.60 + lRelZ;
                        if( ret2 <= ret1 )
                            kzy = ret2;
                        else
                            kzy = ret1;
                    } else { // lRelZ >= 0.40
                        double l = lRelZ > 1.0? lRelZ: 1.0;
                        kzy = 1.0 - 0.10 * l * N / ((amlt - 0.25)*DNZ);
                    }
                } else { // cl == 3 || cl == 4
                    double l = lRelZ > 1.0? lRelZ: 1.0;
                    kzy = 1.0 - 0.05 * l * N / ((amlt - 0.25)*DNZ);
                }
            }
            double check = fabs(N) / DNY  + kyy * fabs(My) / DM + kyz * fabs(Mz) / MRdZ;
            double tmp = fabs(N) / DNZ + kzy * fabs(My) / DM + kzz * fabs(Mz) / MRdZ;
            if( tmp > check ){
                check = tmp;
            }
            if( (1.0 - rhoV) != 0.0 ){
                *res = check / (1.0 - rhoV);
            }
            if( check > (1.0 - rhoV) ){
                return false;
            } else {
                return true;
            }
        } else {
            *res = 0.0;
            return false;
        }
    }
}

double SectionSteel::Cm( SectionSteel::MomentDistribution md, double MH1, double MS, double MH2 ){
    if( MH1 == 0.0 && MH2 == 0.0 ){
        if( md == SectionSteel::momentLinear ){
            return 1.0;
        } else if( md == SectionSteel::momentBiLinear ){
            return 0.90;
        } else { // md == SectionSteel::momentParabolic
            return 0.95;
        }
    }

    double MH, phi;
    if( fabs( MH1 ) > fabs( MH2) ){
        MH = MH1;
        phi = MH2 / MH1;
    } else {
        MH = MH2;
        phi = MH1 / MH2;
    }

    if( md == SectionSteel::momentLinear ){
        double ret = 0.60 + 0.40 * phi;
        if( ret < 0.40 ){
            ret = 0.40;
        }
        return ret;
    } else {
        if( fabs(MH) > fabs(MS) ){
            double alpha = MS / MH;
            if( alpha >= 0.0 ){
                double ret = 0.20 + 0.80 * alpha;
                if( ret < 0.40 ){
                    ret = 0.40;
                }
                return ret;
            } else { // alpha < 0.0
                if( phi >= 0.0){
                    if( md == SectionSteel::momentParabolic ){
                        double ret = 0.10 - 0.80 * alpha;
                        if( ret < 0.40 ){
                            ret = 0.40;
                        }
                        return ret;
                    } else { // md == SectionSteel::momentBiLinear
                        double ret = 0.80 * alpha;
                        if( ret < 0.40 ){
                            ret = 0.40;
                        }
                        return ret;
                    }
                } else { // phi < 0.0
                    if( md == SectionSteel::momentParabolic ){
                        double ret = 0.10 * (1.0 - phi) - 0.80 * alpha;
                        if( ret < 0.40 ){
                            ret = 0.40;
                        }
                        return ret;
                    } else { // md == SectionSteel::momentBiLinear
                        double ret = -0.20 * phi - 0.80 * alpha;
                        if( ret < 0.40 ){
                            ret = 0.40;
                        }
                        return ret;
                    }
                }
            }
        } else {
            double alpha = MH / MS;
            if( alpha >= 0.0 ){
                if( md == SectionSteel::momentParabolic ){
                    return (0.95 + 0.05 * alpha );
                } else{ //  md == SectionSteel::momentBilinear
                    return (0.9 + 0.1 * alpha);
                }
            } else { // alpha < 0.0
                if( phi >= 0.0){
                    if( md == SectionSteel::momentParabolic ){
                        return (0.95 + 0.05 * alpha );
                    } else{ //  md == SectionSteel::momentBilinear
                        return (0.9 + 0.1 * alpha);
                    }
                } else { // phi < 0.0
                    if( md == SectionSteel::momentParabolic ){
                        return (0.95 + 0.05 * alpha * (1.0 + 2.0 * phi));
                    } else{ //  md == SectionSteel::momentBilinear
                        return (0.9 - 0.1 * alpha * (1.0 + 2.0 * alpha));
                    }
                }
            }
        }
    }
}

void SectionSteel::updateReadOnlyVars() {
    updateSectionPoints();

    VarPlusContainer::updateReadOnlyVars();

    updateClassDiagram();
}

void SectionSteel::translateNormal(double yTrans, double zTrans, bool suspSignals) {
    if( suspSignals ){
        suspendSignals( true );
    }
    translationO->translateNormal( yTrans, zTrans );

    if( suspSignals ){
        updateReadOnlyVars();
        updateQGraphics();
        suspendSignals( false);
    }
}

void SectionSteel::rotateNormal(double rot, bool suspSignals ) {
    if( suspSignals ){
        suspendSignals( true );
    }
    rotationO->setValueNormal( rotationO->valueNormal() + rot );
    translationO->rotateNormal( rot );
    updateReadOnlyVars();
    updateQGraphics();
    if( suspSignals ){
        suspendSignals( false );
    }
}

bool SectionSteel::verifyULS( DoublePlus * lRelY,
                              DoublePlus * chiY,
                              DoublePlus * lRelZ,
                              DoublePlus * chiZ,
                              DoublePlus * N,
                              SectionSteelTorsionalDeformability * torsDef,
                              DoublePlus * chiLT,
                              SectionSteelMomentDistribution * myd,
                              DoublePlus * MyH1,
                              DoublePlus * MyS,
                              DoublePlus * MyH2,
                              SectionSteelMomentDistribution * mzd,
                              DoublePlus * MzH1,
                              DoublePlus * MzS,
                              DoublePlus * MzH2,
                              SectionSteelMomentDistribution * mltd,
                              DoublePlus * MltH1,
                              DoublePlus * MltS,
                              DoublePlus * MltH2,
                              Point2DPlus * cen,
                              DoublePlus * My,
                              DoublePlus * Mz,
                              DoublePlus * Vy,
                              DoublePlus * Vz,
                              DoublePlus * res,
                              QString * clString,
                              QString * messages ){

    double r;
    bool ret = verifyULSNormal( lRelY->valueNormal(),
                                chiY->valueNormal(),
                                lRelZ->valueNormal(),
                                chiZ->valueNormal(),
                                N->valueNormal(),
                                torsDef->valueNormal(),
                                chiLT->valueNormal(),
                                myd->valueNormal(),
                                MyH1->valueNormal(),
                                MyS->valueNormal(),
                                MyH2->valueNormal(),
                                mzd->valueNormal(),
                                MzH1->valueNormal(),
                                MzS->valueNormal(),
                                MzH2->valueNormal(),
                                mltd->valueNormal(),
                                MltH1->valueNormal(),
                                MltS->valueNormal(),
                                MltH2->valueNormal(),
                                cen->y->valueNormal(),
                                cen->z->valueNormal(),
                                My->valueNormal(),
                                Mz->valueNormal(),
                                Vy->valueNormal(),
                                Vz->valueNormal(),
                                &r,
                                clString,
                                messages );
    res->setValueNormal( r );

    return ret;
}

QGraphicsItem * SectionSteel::qGraphicsItem(){
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsItemGroup();
        if( m_d->sectionSteelProfile != NULL ){
            m_d->qGraphicsItem->addToGroup( m_d->sectionSteelProfile->qGraphicsItem() );
        }
        updateQGraphics();
    }
    return m_d->qGraphicsItem;
}

void SectionSteel::updateQGraphics(){
    if( m_d->qGraphicsItem ){
        QTransform rotTrans;
        rotTrans.translate( translationO->y->value(), translationO->z->value() );
        rotTrans.rotateRadians( rotationO->valueNormal() );
        m_d->qGraphicsItem->setTransform( rotTrans );
    }
}

double SectionSteel::Theta( double N, double My, double Mz){
    double n = 0.0;
    if( NULSMax->valueNormal() != 0.0 ){
        n = N * NULSMax->valueNormal();
    }
    double my = 0.0;
    if( MULSyMax->valueNormal() != 0.0 ){
        my = My / MULSyMax->valueNormal();
    }
    double mz = 0.0;
    if( MULSzMax->valueNormal() != 0.0 ){
        mz = Mz / MULSzMax->valueNormal();
    }
    double d = sqrt( pow(my,2.0) + pow(mz,2.0) );
    double ret = atan2( n, d );
    if( std::isnan( ret ) ){
        return 0.0;
    } else {
        return ret;
    }
}

double SectionSteel::xi( double My, double Mz){
    double my = 0.0;
    if( MULSyMax->valueNormal() != 0.0 ){
        my = My / MULSyMax->valueNormal();
    }
    double mz = 0.0;
    if( MULSzMax->valueNormal() != 0.0 ){
        mz = Mz / MULSzMax->valueNormal();
    }
    double ret = fabs( atan2(mz, my ) );
    if( std::isnan(ret) ){
        return 0.0;
    } else {
        return ret;
    }
}

void SectionSteel::updateClassDiagram(){
    if( m_d->sectionSteelProfile ){
        m_d->sectionSteelProfile->classDiagram( m_d->classDiagram, NULSMax->valueNormal(), MULSyMax->valueNormal(), MULSzMax->valueNormal() );
    }
}

QList<QPolygonF *> * SectionSteel::classDiagram(){
    return m_d->classDiagram;
}

int SectionSteel::classNormal( double N, double MyInp, double MzInp, double yCen, double zCen, bool simpleCalc ){
    double My = MyInp + N * zCen;
    double Mz = MzInp - N * yCen;
    if( N >= 0.0 && My == 0.0 && Mz == 0.0 ){
        return 1;
    }
    int vN = QString(NClass->valueStr().at(0)).toInt();
    int vMy = QString(MyMaxClass->valueStr().at(0)).toInt();
    int vMz = QString(MzMaxClass->valueStr().at(0)).toInt();

    if( vN == vMy && vN == vMz ){
        return vN;
    }
    if( N != 0.0 && My == 0.0 && Mz == 0.0 )
        return vN;
    else if( N == 0.0 && My != 0.0 && Mz == 0.0 )
        return vMy;
    else if( N == 0.0 && My == 0.0 && Mz != 0.0 )
        return vMz;

    if( simpleCalc ){
        int ret = 0;
        if( My != 0.0 ){
            if( N < MyClassLimit->valueNormal() ){
                return vN;
            } else {
                if( vMy > ret ) {
                    ret = vMy;
                }
            }
        }
        if( Mz != 0.0 ){
            if( N < MzClassLimit->valueNormal() ){
                return vN;
            }
        } else {
            if( vMz > ret ) {
                ret = vMz;
            }
        }
        return ret;
    }

    if( steel() && \
            (A->valueNormal() != 0.0) && \
            (Iyy->valueNormal() != 0.0) && \
            (Izz->valueNormal() != 0.0) && \
            (MULSyMin->valueNormal() != 0.0) && \
            (MULSzMax->valueNormal() != 0.0)){
        QPointF P( xi(My, Mz), Theta(N, My, Mz) );
        for( int i=(m_d->classDiagram->size()-1); i >= 0 ;i--){
            if( m_d->classDiagram->at(i)->containsPoint(P, Qt::OddEvenFill) ){
                return (i+1);
            }
        }
        return 4;
    }

    return 0;
}

void SectionSteel::MULSNormal( double *yRet,
                               double *zRet,
                               double l,
                               double my,
                               QList<Point2DModel * > * sects){
    *yRet = 0.0;
    *zRet = 0.0;
    if( steel() ){
        if( m_d->sectionPoints->nPoints() > 2 ) {
            if( my == 0.0 ){
                double s = steel()->fULSNormal( l );
                *yRet = s * Sy->valueNormal();
                *zRet = - (s * Sz->valueNormal());
            } else {
                Point2DModel * sectPlus = new Point2DModel();
                Point2DModel * sectEl = new Point2DModel();
                Point2DModel * sectMinus = new Point2DModel();

                double epsYd = steel()->fyd(0)->valueNormal() / steel()->E->valueNormal();
                calculateSects( l, my, epsYd, - epsYd, sectPlus, sectEl, sectMinus );

                double yLin = steel()->E->valueNormal() * (l * sectEl->SyNormal() + my  * sectEl->IyyNormal());
                double yConst = - steel()->fyd(0)->valueNormal() * sectMinus->SyNormal();
                yConst += steel()->fyd(0)->valueNormal() * sectPlus->SyNormal();
                *yRet = yLin + yConst;

                double zLin = steel()->E->valueNormal() * (l * sectEl->SzNormal() + my  * sectEl->IyzNormal());
                double zConst = - steel()->fyd(0)->valueNormal() * sectMinus->SzNormal();
                zConst += steel()->fyd(0)->valueNormal() * sectPlus->SzNormal();
                // invertiamo il segno del momento rispetto ad asse z
                *zRet = - (zLin + zConst);

                if( sects ){
                    sects->append( sectPlus );
                    sects->append( sectEl );
                    sects->append( sectMinus );
                } else {
                    delete sectPlus;
                    delete sectEl;
                    delete sectMinus;
                }
            }
        }
    }
}

double SectionSteel::NULSNormal(double l, double my, QList<Point2DModel *> * sects){
    updateSectionPoints();
    double NRet = 0.0;
    if( steel() ){
        if( steel()->E->valueNormal() != 0.0 ){
            if( m_d->sectionPoints->nPoints() > 2 ){
                if( my == 0.0 ){
                    NRet = steel()->fULSNormal(l) * A->valueNormal();
                } else {
                    Point2DModel * sectPlus = new Point2DModel();
                    Point2DModel * sectEl = new Point2DModel();
                    Point2DModel * sectMinus = new Point2DModel();

                    double epsYd = steel()->fyd(0)->valueNormal() / steel()->E->valueNormal();
                    calculateSects( l, my, epsYd, - epsYd, sectPlus, sectEl, sectMinus );
                    NRet = steel()->E->valueNormal() * ( l * sectEl->ANormal() + my * sectEl->SyNormal() );
                    NRet -= steel()->fyd(0)->valueNormal() * sectMinus->ANormal();
                    NRet += steel()->fyd(0)->valueNormal() * sectPlus->ANormal();

                    if( sects ){
                        sects->append( sectPlus );
                        sects->append( sectEl );
                        sects->append( sectMinus );
                    } else {
                        delete sectPlus;
                        delete sectEl;
                        delete sectMinus;
                    }
                }
            }
        }
    }
    return NRet;
}

void SectionSteel::calculateSects(const double & l,
                                  const double & my,
                                  const double & epsPlus,
                                  const double & epsMinus,
                                  Point2DModel * sectPlus,
                                  Point2DModel * sectEl,
                                  Point2DModel * sectMinus){
    if( m_d->sectionPoints->nPoints() > 0 ){
        double zPlus = (epsPlus - l)/my;
        double zMinus = (epsMinus - l)/my;
        int d1 = where( m_d->sectionPoints->zNormal(0), zPlus, zMinus );
        int d2;
        if( d1 < 0 ){
            sectMinus->appendPoint( m_d->sectionPoints->yNormal(0), m_d->sectionPoints->zNormal(0) );
        } else if( d1 > 0 ){
            sectPlus->appendPoint( m_d->sectionPoints->yNormal(0), m_d->sectionPoints->zNormal(0) );
        } else {
            sectEl->appendPoint( m_d->sectionPoints->yNormal(0), m_d->sectionPoints->zNormal(0) );
        }
        for( int j = 0; j < m_d->sectionPoints->nPoints(); j++){
            int i = j+1;
            if( i == m_d->sectionPoints->nPoints() )
                i = 0;
            d2 = where( m_d->sectionPoints->zNormal(i), zPlus, zMinus );
            if( d1 < 0 ){
                if( d2 == 0 ){
                    double y = m_d->sectionPoints->yNormal(j) + \
                            (m_d->sectionPoints->yNormal(i) - m_d->sectionPoints->yNormal(j)) / (m_d->sectionPoints->zNormal(i) - m_d->sectionPoints->zNormal(j)) * \
                            (zMinus - m_d->sectionPoints->zNormal(j));
                    sectMinus->appendPoint( y, zMinus );
                    sectEl->appendPoint( y, zMinus );
                    sectEl->appendPoint( m_d->sectionPoints->yNormal(i), m_d->sectionPoints->zNormal(i) );
                } else if( d2 > 0 ){
                    double y = m_d->sectionPoints->yNormal(j) + \
                            (m_d->sectionPoints->yNormal(i) - m_d->sectionPoints->yNormal(j)) / (m_d->sectionPoints->zNormal(i) - m_d->sectionPoints->zNormal(j)) * \
                            (zMinus - m_d->sectionPoints->zNormal(j));
                    sectMinus->appendPoint( y, zMinus );
                    sectEl->appendPoint( y, zMinus );
                    y = m_d->sectionPoints->yNormal(j) + \
                            (m_d->sectionPoints->yNormal(i) - m_d->sectionPoints->yNormal(j)) / (m_d->sectionPoints->zNormal(i) - m_d->sectionPoints->zNormal(j)) * \
                            (zPlus - m_d->sectionPoints->zNormal(j));
                    sectEl->appendPoint( y, zPlus );
                    sectPlus->appendPoint( y, zPlus );
                    sectPlus->appendPoint( m_d->sectionPoints->yNormal(i), m_d->sectionPoints->zNormal(i) );
                } else { // d1 < 0
                    sectMinus->appendPoint( m_d->sectionPoints->yNormal(i), m_d->sectionPoints->zNormal(i) );
                }
            } else if( d1 == 0 ){
                if( d2 < 0 ){
                    double y = m_d->sectionPoints->yNormal(j) + \
                            (m_d->sectionPoints->yNormal(i) - m_d->sectionPoints->yNormal(j)) / (m_d->sectionPoints->zNormal(i) - m_d->sectionPoints->zNormal(j)) * \
                            (zMinus - m_d->sectionPoints->zNormal(j));
                    sectEl->appendPoint( y, zMinus );
                    sectMinus->appendPoint( y, zMinus );
                    sectMinus->appendPoint( m_d->sectionPoints->yNormal(i), m_d->sectionPoints->zNormal(i) );
                } else if( d2 > 0 ){
                    double y = m_d->sectionPoints->yNormal(j) + \
                            (m_d->sectionPoints->yNormal(i) - m_d->sectionPoints->yNormal(j)) / (m_d->sectionPoints->zNormal(i) - m_d->sectionPoints->zNormal(j)) * \
                            (zPlus - m_d->sectionPoints->zNormal(j));
                    sectEl->appendPoint( y, zPlus );
                    sectPlus->appendPoint( y, zPlus );
                    sectPlus->appendPoint( m_d->sectionPoints->yNormal(i), m_d->sectionPoints->zNormal(i) );
                } else { // d2 == 0
                    sectEl->appendPoint( m_d->sectionPoints->yNormal(i), m_d->sectionPoints->zNormal(i) );
                }
            } else if( d1 > 0 ){
                if( d2 == 0 ){
                    double y = m_d->sectionPoints->yNormal(j) + \
                            (m_d->sectionPoints->yNormal(i) - m_d->sectionPoints->yNormal(j)) / (m_d->sectionPoints->zNormal(i) - m_d->sectionPoints->zNormal(j)) * \
                            (zPlus - m_d->sectionPoints->zNormal(j));
                    sectPlus->appendPoint( y, zPlus );
                    sectEl->appendPoint( y, zPlus );
                    sectEl->appendPoint( m_d->sectionPoints->yNormal(i), m_d->sectionPoints->zNormal(i) );
                } else if( d2 < 0 ){
                    double y = m_d->sectionPoints->yNormal(j) + \
                            (m_d->sectionPoints->yNormal(i) - m_d->sectionPoints->yNormal(j)) / (m_d->sectionPoints->zNormal(i) - m_d->sectionPoints->zNormal(j)) * \
                            (zPlus - m_d->sectionPoints->zNormal(j));
                    sectPlus->appendPoint( y, zPlus );
                    sectEl->appendPoint( y, zPlus );
                    y = m_d->sectionPoints->yNormal(j) + \
                            (m_d->sectionPoints->yNormal(i) - m_d->sectionPoints->yNormal(j)) / (m_d->sectionPoints->zNormal(i) - m_d->sectionPoints->zNormal(j)) * \
                            (zMinus - m_d->sectionPoints->zNormal(j));
                    sectEl->appendPoint( y, zMinus );
                    sectMinus->appendPoint( y, zMinus );
                    sectMinus->appendPoint( m_d->sectionPoints->yNormal(i), m_d->sectionPoints->zNormal(i) );
                } else { // d2 > 0
                    sectPlus->appendPoint( m_d->sectionPoints->yNormal(i), m_d->sectionPoints->zNormal(i) );
                }
            }
            d1 = d2;
        }
    }
}

int SectionSteel::where( double z, double zPlus, double zMinus ){
    if( z < zMinus ) {
        return -1;
    } else if( z > zPlus ) {
        return 1;
    } else {
        return 0;
    }
}

void SectionSteel::MULSNormal( double *yRet,
                               double *zRet,
                               double l,
                               double my,
                               double mz,
                               QList<Point2DModel * > * sects){
    *yRet = 0.0;
    *zRet = 0.0;
    if( steel() ){
        if( m_d->sectionPoints->nPoints() > 2 ) {
            if( my == 0.0 && mz == 0.0 ){
                double s = steel()->fULSNormal( l );
                *yRet = s * Sy->valueNormal();
                *zRet = - (s * Sz->valueNormal());
            } else {
                Point2DModel * sectPlus = new Point2DModel();
                Point2DModel * sectEl = new Point2DModel();
                Point2DModel * sectMinus = new Point2DModel();
                QList<Point2DModel *> sectsVal;
                sectsVal << sectMinus << sectEl << sectPlus;

                double epsYd = steel()->fyd(0)->valueNormal() / steel()->E->valueNormal();
                QList<double> eVal;
                eVal << -epsYd << epsYd;

                m_d->sectionPoints->calculateSects( l, my, mz, eVal, sectsVal );

                double yLin = steel()->E->valueNormal() * (l * sectEl->SyNormal() + my  * sectEl->IyyNormal() + mz * sectEl->IyzNormal() );
                double yConst = - steel()->fyd(0)->valueNormal() * sectMinus->SyNormal();
                yConst += steel()->fyd(0)->valueNormal() * sectPlus->SyNormal();
                *yRet = yLin + yConst;

                double zLin = steel()->E->valueNormal() * (l * sectEl->SzNormal() + my  * sectEl->IyzNormal() + mz * sectEl->IzzNormal() );
                double zConst = - steel()->fyd(0)->valueNormal() * sectMinus->SzNormal();
                zConst += steel()->fyd(0)->valueNormal() * sectPlus->SzNormal();
                // invertiamo il segno del momento rispetto ad asse z
                *zRet = - (zLin + zConst);

                if( sects ){
                    sects->append( sectPlus );
                    //sects->append( sectEl );
                    //sects->append( sectMinus );
                } else {
                    delete sectPlus;
                    delete sectEl;
                    delete sectMinus;
                }
            }
        }
    }
}

double SectionSteel::NULSNormal( double l, double my, double mz, QList<Point2DModel *> * sects){
    updateSectionPoints();
    double NRet = 0.0;
    if( steel() ){
        if( steel()->E->valueNormal() != 0.0 ){
            if( m_d->sectionPoints->nPoints() > 2 ){
                if( (my == 0.0) & (mz == 0.0) ){
                    NRet = steel()->fULSNormal(l) * A->valueNormal();
                } else {
                    Point2DModel * sectPlus = new Point2DModel();
                    Point2DModel * sectEl = new Point2DModel();
                    Point2DModel * sectMinus = new Point2DModel();
                    QList<Point2DModel *> sectsVal;
                    sectsVal << sectMinus << sectEl << sectPlus;

                    double epsYd = steel()->fyd(0)->valueNormal() / steel()->E->valueNormal();
                    QList<double> epsVal;
                    epsVal << -epsYd << epsYd;

                    m_d->sectionPoints->calculateSects( l, my, mz, epsVal, sectsVal );
                    NRet = steel()->E->valueNormal() * ( l * sectEl->ANormal() + my * sectEl->SyNormal() + mz * sectEl->SzNormal() );
                    NRet += steel()->fyd(0)->valueNormal() * sectPlus->ANormal();
                    NRet -= steel()->fyd(0)->valueNormal() * sectMinus->ANormal();

                    if( sects ){
                        sects->append( sectPlus );
                        sects->append( sectEl );
                        sects->append( sectMinus );
                    } else {
                        delete sectPlus;
                        delete sectEl;
                        delete sectMinus;
                    }
                }
            }
        }
    }
    return NRet;
}
