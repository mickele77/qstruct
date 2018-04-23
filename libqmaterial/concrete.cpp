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

#include "concrete.h"

#include <cmath>
#include <QString>
#include <QXmlAttributes>
#include <QMap>
#include "doubleplus.h"
#include "boolplus.h"
#include "qstringplus.h"

#include <QList>
#include <QCoreApplication>

class ConcreteModelfEpsSectionPrivate{
    Q_DECLARE_TR_FUNCTIONS(ConcreteModelfEpsSectionPrivate)

    class enumVal{
    public:
        enumVal(Concrete::ModelfEpsSection v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        }
        Concrete::ModelfEpsSection val;
        QString normalStr;
        QString str;
    };

public:
    ConcreteModelfEpsSectionPrivate(Concrete::ModelfEpsSection v):
        value(v){
        enumList.append( enumVal( Concrete::SectionParabolaRectangle, "SectionParabolaRectangle", trUtf8("Parabola-rettangolo")) );
        enumList.append( enumVal( Concrete::SectionBiLinear, "SectionBiLinear", trUtf8("Triangolo-rettangolo")) );
        enumList.append( enumVal( Concrete::SectionStressBlock, "SectionStressBlock", trUtf8("Rettangolo")) );
    }
    ~ConcreteModelfEpsSectionPrivate(){
    }

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    Concrete::ModelfEpsSection value;
    QList<enumVal> enumList;
};

ConcreteModelfEpsSection::ConcreteModelfEpsSection( Concrete::ModelfEpsSection vv,
                                                    const QString & nn,
                                                    bool ro):
    EnumPlus( nn, ro  ),
    m_d( new ConcreteModelfEpsSectionPrivate(vv) ) {
}

ConcreteModelfEpsSection::~ConcreteModelfEpsSection(){
    delete m_d;
}

ConcreteModelfEpsSection & ConcreteModelfEpsSection::operator=( ConcreteModelfEpsSection & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & ConcreteModelfEpsSection::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        ConcreteModelfEpsSection * valCncrModelPlus = dynamic_cast<ConcreteModelfEpsSection *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString ConcreteModelfEpsSection::typeVar(){
    return QString("ConcreteModelfEpsSection");
}

Concrete::ModelfEpsSection ConcreteModelfEpsSection::value(){
    return m_d->value;
}

Concrete::ModelfEpsSection ConcreteModelfEpsSection::valueNormal(){
    return m_d->value;
}

QString ConcreteModelfEpsSection::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString ConcreteModelfEpsSection::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void ConcreteModelfEpsSection::setValueNormal( Concrete::ModelfEpsSection vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void ConcreteModelfEpsSection::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void ConcreteModelfEpsSection::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void ConcreteModelfEpsSection::setValue( Concrete::ModelfEpsSection vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void ConcreteModelfEpsSection::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int ConcreteModelfEpsSection::enumCount() {
    return m_d->enumList.size();
}

QString ConcreteModelfEpsSection::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int ConcreteModelfEpsSection::valueNormalInt() {
    return m_d->valueIndex();
}

Concrete::Concrete( UnitMeasure * ump, const QString & n, QObject * parent ):
    Material(ump, n, parent )  {
    initVar();

    Rck->setValueNormal( 3.0e+7 );
    if( n.isEmpty() ){
        name->setValue( "Rck" + Rck->valueStr() );
    } else {
        name->setValue( n );
    }
    gammaW->setValueNormal( 2.5e+4 );
}

Concrete::Concrete( UnitMeasure * ump, double rck, const QString &n, QObject * parent ):
    Material(ump, n, parent) {
    initVar();

    Rck->setValue( rck );
    if( n.isEmpty() ){
        name->setValue( "Rck" + Rck->valueStr() );
    } else {
        name->setValue( n );
    }
    gammaW->setValueNormal( 2.5e+4 );
}

Concrete::Concrete( Concrete & val):
    Material( val ) {
    *this = val;
}

Concrete::Concrete( Concrete * val): Material( val ) {
    *this = *val;
}

Concrete::Concrete(UnitMeasure * ump, const QXmlStreamAttributes &attrs, QObject *parent):
    Material(ump, "", parent ){
    initVar();
    loadFromXML( attrs );
}

MaterialModel::MaterialType Concrete::materialType() {
    return MaterialModel::ConcreteMaterial;
}

void Concrete::initVar(){
    *m_typeNameInternal = "Concrete";
    typeName->setValue( trUtf8("Calcestruzzo"));
    alpha->setValueNormal( 1.0e-5, false );
    nu->setValueNormal( 0.30, false );
    gammaW->setValueNormal( 2.5e+4, false );
    E->setReadOnly( true, false );
    G->setReadOnly( true, false );

    Rck = new DoublePlus(0.00, "Rck", m_unitMeasure, UnitMeasure::tension );
    Rck->setRichName( trUtf8("R<span style=\"vertical-align:sub;\">ck</span>") );
    Rck->setReadOnly( false );
    Rck->setToolTip(trUtf8("Valore caratteristico della resistenza a compressione cubica"));
    addVarToContainer( Rck );

    fck = new DoublePlus(0.00, "fck", m_unitMeasure, UnitMeasure::tension, true );
    fck->setToolTip(trUtf8("Valore caratteristico della resistenza a compressione cilindrica"));
    fck->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">ck</span>") );
    addVarToContainer( fck );

    fcm = new DoublePlus(0.0, "fcm", m_unitMeasure, UnitMeasure::tension );
    fcm->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">cm</span>") );
    fcm->setReadOnly( true );
    fcm->setToolTip(trUtf8("Valore medio della resistenza a compressione"));
    addVarToContainer( fcm );

    fctm = new DoublePlus(0.0, "fctm", m_unitMeasure, UnitMeasure::tension );
    fctm->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">ctm</span>") );
    fctm->setReadOnly( true );
    fctm->setToolTip(trUtf8("Valore medio della resistenza a trazione assiale"));
    addVarToContainer( fctm );
    fctm->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfctm) );
    fctm->addConnectedVars( 3, Rck, fck, fcm);

    fctk = new DoublePlus(0.0, "fctk", m_unitMeasure, UnitMeasure::tension );
    fctk->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">ctk</span>") );
    fctk->setReadOnly( true );
    fctk->setToolTip(trUtf8("Valore caratteristico della resistenza a trazione assiale, frattile 5%"));
    addVarToContainer( fctk);
    fctk->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfctk) );
    fctk->addConnectedVars( 1, fctm);

    fctk95 = new DoublePlus(0.0, "fctk95", m_unitMeasure, UnitMeasure::tension );
    fctk95->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">ctk,95</span>") );
    fctk95->setToolTip(trUtf8("Valore caratteristico della resistenza a trazione assiale, frattile 95%"));
    fctk95->setReadOnly( true );
    addVarToContainer( fctk95);
    fctk95->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfctk95) );
    fctk95->addConnectedVars( 1, fctm );

    fcfm = new DoublePlus(0.0, "fcfm", m_unitMeasure, UnitMeasure::tension );
    fcfm->setToolTip( trUtf8("Valore medio della resistenza a trazione per flessione"));
    fcfm->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">cfm</span>") );
    fcfm->setReadOnly( true );
    addVarToContainer( fcfm );
    fcfm->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfcfm) );
    fcfm->addConnectedVars( 1, fctm );

    fcRare = new DoublePlus(0.0, "fcRara", m_unitMeasure, UnitMeasure::tension, true );
    fcRare->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">rara</span>") );
    fcRare->setReadOnly( true );
    fcRare->setToolTip(trUtf8("Valore limite secondo NTC08 della resistenza a compressione nella combinazione rara"));
    addVarToContainer( fcRare);
    fcRare->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfcRare) );
    fcRare->addConnectedVars( 1, fck );

    fcRareRed = new DoublePlus(0.0, "fcRaraRed", m_unitMeasure, UnitMeasure::tension, true );
    fcRareRed->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">rara,red</span>") );
    fcRareRed->setReadOnly( true );
    fcRareRed->setToolTip(trUtf8("Valore limite secondo NTC08 della resistenza a compressione nella combinazione rara, per sezioni con h < 5 cm"));
    addVarToContainer( fcRareRed);
    fcRareRed->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfcRareRed) );
    fcRareRed->addConnectedVars( 1, fcRare );

    fcPermanent = new DoublePlus(0.0, "fcRaraRed", m_unitMeasure, UnitMeasure::tension, true );
    fcPermanent->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">perm</span>") );
    fcPermanent->setReadOnly( true );
    fcPermanent->setToolTip(trUtf8("Valore limite secondo NTC08 della resistenza a compressione nella combinazione permanente"));
    addVarToContainer( fcPermanent );
    fcPermanent->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfcPermanent) );
    fcPermanent->addConnectedVars( 1, fck );

    fcPermanentRed = new DoublePlus(0.0, "fcPermanentRed", m_unitMeasure, UnitMeasure::tension, true );
    fcPermanentRed->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">perm,red</span>") );
    fcPermanentRed->setReadOnly( true );
    fcPermanentRed->setToolTip(trUtf8("Valore limite secondo NTC08 della resistenza a compressione nella combinazione permanente, per sezioni con h < 5 cm"));
    addVarToContainer( fcPermanentRed );
    fcPermanentRed->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfcPermanentRed) );
    fcPermanentRed->addConnectedVars( 1, fcPermanent );

    Ecm = new DoublePlus(0.0, "Ecm", m_unitMeasure, UnitMeasure::tension, true );
    Ecm->setRichName( trUtf8("E<span style=\"vertical-align:sub;\">cm</span>") );
    Ecm->setReadOnly( true );
    Ecm->setToolTip( trUtf8("Modulo di elasticità normale secante"));
    addVarToContainer( Ecm );
    Ecm->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setEcm) );
    Ecm->addConnectedVars( 1, fcm );

    ratioEEcm = new DoublePlus(1.0, "ratioEEcm", m_unitMeasure, UnitMeasure::noDimension );
    ratioEEcm->setRichName( trUtf8("E/E<span style=\"vertical-align:sub;\">cm</span>") );
    ratioEEcm->setReadOnly( false );
    ratioEEcm->setToolTip( trUtf8("Rapporto tra il modulo di elasticità normale assunto per il calcolo elastico e quello secante da NTC08 ed EC2"));
    addVarToContainer( ratioEEcm );

    E->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setE) );
    E->addConnectedVars( 2, Ecm, ratioEEcm );

    epsC1 = new DoublePlus(0.0, "epsC1", m_unitMeasure, UnitMeasure::percent );
    epsC1->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">c1</span>") );
    epsC1->setReadOnly( true );
    epsC1->setToolTip(trUtf8("Deformazione di contrazione alla tensione di picco"));
    addVarToContainer( epsC1 );
    epsC1->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setEpsC1) );
    epsC1->addConnectedVars( 1, fcm );

    epsC2 = new DoublePlus(0.0, "epsC2", m_unitMeasure, UnitMeasure::percent );
    epsC2->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">c2</span>") );
    epsC2->setReadOnly( true );
    addVarToContainer( epsC2 );
    epsC2->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setEpsC2) );
    epsC2->addConnectedVars( 1, fck );

    epsC3 = new DoublePlus(0.0, "epsC3", m_unitMeasure, UnitMeasure::percent );
    epsC3->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">c3</span>") );
    epsC3->setReadOnly( true );
    addVarToContainer( epsC3 );
    epsC3->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setEpsC3) );
    epsC3->addConnectedVars( 1, fck );

    epsCU1 = new DoublePlus(0.0, "epsCU1", m_unitMeasure, UnitMeasure::percent );
    epsCU1->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">cu1</span>") );
    epsCU1->setReadOnly( true );
    epsCU1->setToolTip(trUtf8("Deformazione ultima di contrazione"));
    addVarToContainer( epsCU1 );
    epsCU1->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setEpsCU1) );
    epsCU1->addConnectedVars( 2, fcm, fck );

    epsCU2 = new DoublePlus(0.0, "epsCU2", m_unitMeasure, UnitMeasure::percent );
    epsCU2->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">cu2</span>") );
    epsCU2->setReadOnly( true );
    epsCU2->setToolTip(trUtf8("Deformazione ultima di contrazione - diagramma parabola-rettangolo"));
    addVarToContainer( epsCU2 );
    epsCU2->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setEpsCU2) );
    epsCU2->addConnectedVars( 1, fck );

    epsC4 = new DoublePlus(0.0, "epsC4", m_unitMeasure, UnitMeasure::percent );
    epsC4->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">c4</span>") );
    epsC4->setReadOnly( true );
    addVarToContainer( epsC4 );
    epsC4->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setEpsC4) );
    epsC4->addConnectedVars( 1, epsCU2 );

    epsCU3 = new DoublePlus(0.0, "epsCU3", m_unitMeasure, UnitMeasure::percent );
    epsCU3->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">cu3</span>") );
    epsCU3->setReadOnly( true );
    epsCU3->setToolTip(trUtf8("Deformazione ultima di contrazione - diagramma bilineare"));
    addVarToContainer( epsCU3 );
    epsCU3->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setEpsCU3) );
    epsCU3->addConnectedVars( 1, fck );

    epsCU4 = new DoublePlus(0.0, "epsCU4", m_unitMeasure, UnitMeasure::percent );
    epsCU4->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">cu4</span>") );
    epsCU4->setReadOnly( true );
    epsCU4->setToolTip(trUtf8("Deformazione ultima di contrazione - diagramma costante (stress-block) "));
    addVarToContainer( epsCU4 );
    epsCU4->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setEpsCU4) );
    epsCU4->addConnectedVars( 1, fck );

    alphaCC = new DoublePlus(0.85, "alphaCC", m_unitMeasure, UnitMeasure::noDimension );
    alphaCC->setRichName( trUtf8("α<span style=\" vertical-align:sub;\">cc</span>") );
    alphaCC->setReadOnly( false );
    alphaCC->setToolTip( trUtf8("Coefficiente effetti a lungo termine e modalità  di applicazione del carico [0.8 - 1.0]"));
    addVarToContainer( alphaCC );

    gammaC = new DoublePlus(1.50, "gammaC", m_unitMeasure, UnitMeasure::noDimension );
    gammaC->setRichName( trUtf8("γ<span style=\" vertical-align:sub;\">c</span>") );
    gammaC->setToolTip(trUtf8("Coefficiente di sicurezza parziale"));
    gammaC->setReadOnly( false );
    addVarToContainer( gammaC );

    fcd = new DoublePlus(0.0, "fcd", m_unitMeasure, UnitMeasure::tension );
    fcd->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">cd</span>") );
    fcd->setReadOnly( true );
    fcd->setToolTip(trUtf8("Valore di progetto della resistenza a compressione"));
    addVarToContainer( fcd);

    fcdRed = new DoublePlus(0.0, "fcdRed", m_unitMeasure, UnitMeasure::tension );
    fcdRed->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">cd,red</span>") );
    fcdRed->setReadOnly( true );
    fcdRed->setToolTip(trUtf8("Valore di progetto della resistenza a compressione, per sezioni con h < 5 cm"));
    addVarToContainer( fcdRed);
    fcdRed->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfcdRed) );
    fcdRed->addConnectedVars( 1, fcd);

    fctd = new DoublePlus(0.0, "fctd", m_unitMeasure, UnitMeasure::tension, true );
    fctd->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">ctd</span>") );
    fctd->setReadOnly( true );
    fctd->setToolTip(trUtf8("Valore di progetto della resistenza a trazione assiale"));
    addVarToContainer( fctd );
    fctd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfctd) );
    fctd->addConnectedVars( 2, fctk, gammaC );

    fctdRed = new DoublePlus(0.0, "fctdRed", m_unitMeasure, UnitMeasure::tension, true );
    fctdRed->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">ctd,red</span>") );
    fctdRed->setReadOnly( true );
    fctdRed->setToolTip(trUtf8("Valore di progetto della resistenza a trazione assiale, per sezioni con h < 5 cm"));
    addVarToContainer( fctdRed );
    fctdRed->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfctdRed) );
    fctdRed->addConnectedVars( 1, fctd );

    modelfEpsSection = new ConcreteModelfEpsSection( Concrete::SectionParabolaRectangle,"modelfEpsSection" );
    modelfEpsSection->setReadOnly( false );
    addVarToContainer( modelfEpsSection );

    setExisting( isExisting->valueNormal() );
    connect( isExisting, &BoolPlus::valueChangedBool, this, &Concrete::setExisting );
}

double Concrete::fSLSNormal( double e ){
    double ret = 0.0;
    if( e < 0.0 ){
        ret = E->valueNormal() * e;
    }
    return ret;
}

double Concrete::fULSNormal( double e ){
    double ret = 0.0;
    if( modelfEpsSection->valueNormal() == Concrete::SectionParabolaRectangle ){
        if( e < 0.0 ){
            if( e > (- epsC2->valueNormal()) ){
                // tratto parabolico f = - (fcd / epsC2^2) *  e + (2 * fcd / epsC2) * e
                ret = ( fcd->valueNormal() / epsC2->valueNormal() ) * ( - e / epsC2->valueNormal() + 2.0 ) * e;
            } else if( e >= (- epsCU2->valueNormal()) ){
                ret = -fcd->valueNormal();
            }
        }
    } else if( modelfEpsSection->valueNormal() == Concrete::SectionBiLinear ){
        if( e < 0.0 ){
            if( e > -epsC3->valueNormal()){
                ret = -fcd->valueNormal() * e / epsC3->valueNormal();
            } else if( e >= -epsCU3->valueNormal() ){
                ret = -fcd->valueNormal();
            }
        }
    } else if( modelfEpsSection->valueNormal() == SectionStressBlock ){
        if( e <= -epsC4->valueNormal() && e >= - epsCU4->valueNormal() ){
            ret = -fcd->valueNormal();
        }
    }
    return ret;
}

void Concrete::fbk( DoublePlus * ret, DoublePlus * d ){
    ret->setValueNormal( fbkNormal(d->valueNormal() ));
}

double Concrete::fbkNormal( double d ){
    double eta = 1.0;
    if( d > 0.032 )
        eta = (0.132 - d)/ 0.100;
    return ( 2.50 * eta * (fctk->valueNormal() ) );
}

void Concrete::fbd( DoublePlus * ret, DoublePlus * e ){
    ret->setValueNormal( fbdNormal(e->valueNormal() ));
}


double Concrete::fbdNormal( double d ){
    return ( fbkNormal( d ) / gammaC->valueNormal() );
}

/*void Concrete::updateReadOnlyVars( bool emitAuto ) {
    fck->updateValue( emitAuto );
    fcm->updateValue( emitAuto );
    fctm->updateValue( emitAuto );
    fctk->updateValue( emitAuto );
    fctk95->updateValue( emitAuto );
    fcfm->updateValue( emitAuto );
    fcd->updateValue( emitAuto );
    fcdRed->updateValue( emitAuto );
    fctd->updateValue( emitAuto );
    fctdRed->updateValue( emitAuto );
    fcRare->updateValue( emitAuto );
    fcRareRed->updateValue( emitAuto );
    fcPermanent->updateValue( emitAuto );
    fcPermanentRed->updateValue( emitAuto );
    Ecm->updateValue( emitAuto );
    E->updateValue( emitAuto );
    G->updateValue( emitAuto );
    epsC1->updateValue( emitAuto );
    epsC2->updateValue( emitAuto );
    epsC3->updateValue( emitAuto );
    epsC4->updateValue( emitAuto );
    epsCU1->updateValue( emitAuto );
    epsCU2->updateValue( emitAuto );
    epsCU3->updateValue( emitAuto );
    epsCU4->updateValue( emitAuto );
}*/

void Concrete::setFckFromRck( bool emitAuto ){
    fck->setValueNormal( 0.83 * Rck->valueNormal(), emitAuto );
}

void Concrete::setFckFromFcm(bool emitAuto) {
    fck->setValueNormal( fcm->valueNormal() - 8.0e+6, emitAuto );
}

void Concrete::setFcmFromFck( bool emitAuto ){
    fcm->setValueNormal( fck->valueNormal() + 8.0e+6, emitAuto );
}

void Concrete::setfctm( bool emitAuto ){
    if ( ( Rck->valueNormal() * 1.0e-6) > 60.0 )
        fctm->setValueNormal( 2.12 * log( 1.0 + fcm->valueNormal() * 1.0e-7 ) * 1.0e+6, emitAuto );
    else
        fctm->setValueNormal( 0.30 * pow( fck->valueNormal() * 1.0e-6 , (2.0/3.0) ) * 1.0e+6, emitAuto );
}

void Concrete::setfctk( bool emitAuto ){
    fctk->setValueNormal( 0.70 * fctm->valueNormal(), emitAuto );
}

void Concrete::setfctk95( bool emitAuto ){
    fctk95->setValueNormal( 1.30 * fctm->valueNormal(), emitAuto );
}

void Concrete::setfcfm( bool emitAuto ){
    fcfm->setValueNormal( 1.20 * fctm->valueNormal(), emitAuto );
}

void Concrete::setfcd( bool emitAuto ){
    double val = 0.0;
    if( gammaC->valueNormal() != 0.0 ){
        val = alphaCC->valueNormal() * fck->valueNormal() / gammaC->valueNormal();
    }
    fcd->setValueNormal( val, emitAuto );
}

void Concrete::setfcdExisting( bool emitAuto ){
    double val = 0.0;
    if( FC->valueNormal() != 0.0 ){
        val = fcm->valueNormal() / FC->valueNormal();
    }
    fcd->setValueNormal( val, emitAuto );
}

void Concrete::setfcdRed( bool emitAuto ){
    fcdRed->setValueNormal( 0.80 * fcd->valueNormal(), emitAuto );
}

void Concrete::setfctd( bool emitAuto ){
    fctd->setValueNormal( fctk->valueNormal() / gammaC->valueNormal(), emitAuto );
}

void Concrete::setfctdRed( bool emitAuto ){
    fctdRed->setValueNormal( 0.80 * fctd->valueNormal(), emitAuto );
}

void Concrete::setfcRare( bool emitAuto ){
    fcRare->setValueNormal( 0.60 * fck->valueNormal(), emitAuto );
}

void Concrete::setfcRareRed( bool emitAuto ){
    fcRareRed->setValueNormal( 0.80 * fcRare->valueNormal(), emitAuto );
}

void Concrete::setfcPermanent( bool emitAuto ){
    fcPermanent->setValueNormal( 0.45 * fck->valueNormal(), emitAuto );
}

void Concrete::setfcPermanentRed( bool emitAuto ){
    fcPermanentRed->setValueNormal( 0.80 * fcPermanent->valueNormal(), emitAuto );
}

void Concrete::setEcm( bool emitAuto ){
    Ecm->setValueNormal( 22000.0 * pow( fcm->valueNormal() * 1.0e-7, 0.3 ) * 1.0e+6, emitAuto );
}

void Concrete::setE( bool emitAuto ){
    E->setValueNormal( Ecm->valueNormal() * ratioEEcm->valueNormal(), emitAuto );
}

void Concrete::setEpsC1( bool emitAuto ){
    double val = 0.70 * pow( fcm->valueNormal() * 1.0e-6, 0.31 ) * 1.0e-3;
    if( val > 2.80e-3 ){
        val = 2.80e-3;
    }
    epsC1->setValueNormal( val, emitAuto );
}

void Concrete::setEpsC2( bool emitAuto ){
    if( (fck->valueNormal() * 1.0e-6) > 50.0 ){
        epsC2->setValueNormal( 0.002 + 0.000085 * pow( (fck->valueNormal() * 1.0e-6 - 50.0), 0.53), emitAuto );
    } else {
        epsC2->setValueNormal( 0.002, emitAuto );
    }
}

void Concrete::setEpsC3( bool emitAuto ){
    if( (fck->valueNormal() * 1.0e-6) > 50.0 ){
        epsC3->setValueNormal( 0.00175 + 0.00055 * (fck->valueNormal() * 1.0e-6 - 50.0) / 40.0, emitAuto );
    } else {
        epsC3->setValueNormal( 0.00175, emitAuto );
    }
}

void Concrete::setEpsC4(bool emitAuto ){
    epsC4->setValueNormal( 0.2 * epsCU2->valueNormal(), emitAuto );
}

void Concrete::setEpsCU1( bool emitAuto ){
    if( (fck->valueNormal() * 1.0e-6) >= 50.0 ){
        epsCU1->setValueNormal( 0.0028 + 0.027 * pow( ((98.0 - fcm->valueNormal() * 1.0e-6 ) / 100.0), 4.0 ), emitAuto );
    } else {
        epsCU1->setValueNormal( 0.0035, emitAuto );
    }
}

void Concrete::setEpsCU2( bool emitAuto ){
    if( (fck->valueNormal() * 1.0e-6) >= 50.0 ){
        epsCU2->setValueNormal( 0.0026 + 0.035 * pow( ((90.0 - fck->valueNormal() * 1.0e-6 ) /100.0), 4.0 ), emitAuto );
    } else {
        epsCU2->setValueNormal( 0.0035, emitAuto );
    }
}

void Concrete::setEpsCU3( bool emitAuto ){
    if( (fck->valueNormal() * 1.0e-6) >= 50.0 ){
        epsCU3->setValueNormal( 0.0026 + 0.035 * pow( ((90.0 - fck->valueNormal() * 1.0e-6 ) /100.0), 4.0 ), emitAuto );
    } else {
        epsCU3->setValueNormal( 0.0035, emitAuto );
    }
}

void Concrete::setEpsCU4( bool emitAuto ){
    if( (fck->valueNormal() * 1.0e-6) >= 50.0 ){
        epsCU4->setValueNormal( 0.0026 + 0.035 * pow( ((90.0 - fck->valueNormal() * 1.0e-6 ) /100.0), 4.0 ), emitAuto );
    } else {
        epsCU4->setValueNormal( 0.0035, emitAuto );
    }
}

double Concrete::epsCUNormal(){
    switch( modelfEpsSection->valueNormal() ){
    case Concrete::SectionBiLinear:
        return epsCU3->valueNormal();
    case Concrete::SectionStressBlock:
        return epsCU4->valueNormal();
    default:
        return epsCU2->valueNormal();
    }
}

double Concrete::epsCNormal(){
    switch( modelfEpsSection->valueNormal() ){
    case Concrete::SectionBiLinear:
        return epsC3->valueNormal();
    case Concrete::SectionStressBlock:
        return epsC4->valueNormal();
    default:
        return epsC2->valueNormal();
    }
}

void Concrete::setExisting( bool exist ) {
    if( exist ){
        FC->setEnabled( true );
        Rck->setEnabled( false );

        fck->setReadOnly( true );
        fck->clearConnectedVars();
        fck->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setFckFromFcm) );
        setFckFromFcm( true );

        fcm->setReadOnly( false );
        fcm->clearConnectedVars();
        fcm->setUpdateValueMethod( this, NULL );

        fcd->setReadOnly( true );
        fcd->clearConnectedVars();
        fcd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfcdExisting ) );
        fcd->addConnectedVars( 2, FC, fcm );
        setfcdExisting( true );
    } else {
        FC->setEnabled( false );
        Rck->setEnabled( true );

        fck->setReadOnly( true );
        fck->clearConnectedVars();
        fck->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setFckFromRck) );
        fck->addConnectedVars( 1, Rck );
        setFckFromRck( true );

        fcm->setReadOnly( true );
        fcm->clearConnectedVars();
        fcm->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setFcmFromFck) );
        fcm->addConnectedVars( 1, fck);
        setFcmFromFck( true );

        fcd->setReadOnly( true );
        fcd->clearConnectedVars();
        fcd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&Concrete::setfcd ) );
        fcd->addConnectedVars( 3, alphaCC, gammaC, fck );
        setfcd( true );
    }
}
