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

#include "frp.h"

#include "qstringplus.h"
#include "doubleplus.h"

#include <QCoreApplication>

class FRPExpositionVal{
public:
    FRPExpositionVal(FRP::Exposition v, const QString & ns, const QString & s):
        val(v),
        normalStr(ns),
        str(s){
    }
    FRP::Exposition val;
    QString normalStr;
    QString str;
};

class FRPExpositionPrivate {
    Q_DECLARE_TR_FUNCTIONS(FRPExpositionPrivate)
public:
    FRPExpositionPrivate( FRP::Exposition vv ):
        value(vv){
        enumList.append( FRPExpositionVal( FRP::internal, "Internal", trUtf8("Interno")) );
        enumList.append( FRPExpositionVal( FRP::external, "External", trUtf8("Esterno")) );
        enumList.append( FRPExpositionVal( FRP::aggressive, "Aggressive", trUtf8("Ambiente aggressivo")) );
    }
    ~FRPExpositionPrivate(){
    }

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }

    FRP::Exposition value;
    QList<FRPExpositionVal> enumList;
};

FRPExposition::FRPExposition( FRP::Exposition vv,
                              const QString & nn,
                              bool ro):
    EnumPlus( nn, ro ),
    m_d( new FRPExpositionPrivate(vv) ){
}

FRPExposition::FRPExposition( FRPExposition & val ):
    EnumPlus(val){
    m_d->value = val.m_d->value;
}

FRPExposition::~FRPExposition(){
    delete m_d;
}

QString FRPExposition::typeVar(){
    return QString("FRPExposition");
}

FRPExposition & FRPExposition::operator=( FRPExposition & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & FRPExposition::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        FRPExposition * valFRPExposition = dynamic_cast<FRPExposition *>(&val);
        if(valFRPExposition) {
            setValueNormal( valFRPExposition->m_d->value );
        }
    }
    return *this;
}

FRP::Exposition FRPExposition::value(){
    return m_d->value;
}

FRP::Exposition FRPExposition::valueNormal(){
    return m_d->value;
}

QString FRPExposition::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString FRPExposition::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void FRPExposition::setValueNormal(FRP::Exposition vv, bool emitAuto ){
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

void FRPExposition::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void FRPExposition::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void FRPExposition::setValue(FRP::Exposition vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void FRPExposition::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int FRPExposition::enumCount() {
    return m_d->enumList.size();
}

QString FRPExposition::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int FRPExposition::valueNormalInt() {
    return m_d->valueIndex();
}

class FRPFiberTypeVal{
public:
    FRPFiberTypeVal(FRP::FiberType v, const QString & ns, const QString & s):
        val(v),
        normalStr(ns),
        str(s){
    }
    FRP::FiberType val;
    QString normalStr;
    QString str;
};

class FRPFiberTypePrivate {
    Q_DECLARE_TR_FUNCTIONS(FRPFiberTypePrivate)
public:
    FRPFiberTypePrivate( FRP::FiberType vv ):
        value(vv){
        enumList.append( FRPFiberTypeVal( FRP::epoxyGlass, "EpoxyGlass", trUtf8("Vetro/Epossidica")) );
        enumList.append( FRPFiberTypeVal( FRP::epoxyAramid, "EpoxyAramid", trUtf8("Arammidica/Epossidica")) );
        enumList.append( FRPFiberTypeVal( FRP::epoxyCarbon, "EpoxyCarbon", trUtf8("Carbonio/Epossidica")) );
    }
    ~FRPFiberTypePrivate(){
    }

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }

    FRP::FiberType value;
    QList<FRPFiberTypeVal> enumList;
};

FRPFiberType::FRPFiberType( FRP::FiberType vv,
                            const QString & nn,
                            bool ro):
    EnumPlus( nn, ro ),
    m_d( new FRPFiberTypePrivate(vv) ){
}

FRPFiberType::FRPFiberType( FRPFiberType & val ):
    EnumPlus(val){
    m_d->value = val.m_d->value;
}

FRPFiberType::~FRPFiberType(){
    delete m_d;
}

QString FRPFiberType::typeVar(){
    return QString("FRPFiberType");
}

FRPFiberType & FRPFiberType::operator=( FRPFiberType & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & FRPFiberType::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        FRPFiberType * valFRPFiberType = dynamic_cast<FRPFiberType *>(&val);
        if(valFRPFiberType) {
            setValueNormal( valFRPFiberType->m_d->value );
        }
    }
    return *this;
}

FRP::FiberType FRPFiberType::value(){
    return m_d->value;
}

FRP::FiberType FRPFiberType::valueNormal(){
    return m_d->value;
}

QString FRPFiberType::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString FRPFiberType::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void FRPFiberType::setValueNormal(FRP::FiberType vv, bool emitAuto ){
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

void FRPFiberType::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void FRPFiberType::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void FRPFiberType::setValue(FRP::FiberType vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void FRPFiberType::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int FRPFiberType::enumCount() {
    return m_d->enumList.size();
}

QString FRPFiberType::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int FRPFiberType::valueNormalInt() {
    return m_d->valueIndex();
}

class FRPApplicationTypeVal{
public:
    FRPApplicationTypeVal(FRP::ApplicationType v, const QString & ns, const QString & s):
        val(v),
        normalStr(ns),
        str(s){
    }
    FRP::ApplicationType val;
    QString normalStr;
    QString str;
};

class FRPApplicationTypePrivate {
    Q_DECLARE_TR_FUNCTIONS(FRPApplicationTypePrivate)
public:
    FRPApplicationTypePrivate( FRP::ApplicationType vv ):
        value(vv){
        enumList.append( FRPApplicationTypeVal( FRP::A, "A", trUtf8("A")) );
        enumList.append( FRPApplicationTypeVal( FRP::B, "A", trUtf8("B")) );
    }
    ~FRPApplicationTypePrivate(){
    }

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }

    FRP::ApplicationType value;
    QList<FRPApplicationTypeVal> enumList;
};

FRPApplicationType::FRPApplicationType( FRP::ApplicationType vv,
                                        const QString & nn,
                                        bool ro):
    EnumPlus( nn, ro ),
    m_d( new FRPApplicationTypePrivate(vv) ){
}

FRPApplicationType::FRPApplicationType( FRPApplicationType & val ):
    EnumPlus(val){
    m_d->value = val.m_d->value;
}

FRPApplicationType::~FRPApplicationType(){
    delete m_d;
}

QString FRPApplicationType::typeVar(){
    return QString("FRPApplicationType");
}

FRPApplicationType & FRPApplicationType::operator=( FRPApplicationType & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & FRPApplicationType::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        FRPApplicationType * valFRPApplicationType = dynamic_cast<FRPApplicationType *>(&val);
        if(valFRPApplicationType) {
            setValueNormal( valFRPApplicationType->m_d->value );
        }
    }
    return *this;
}

FRP::ApplicationType FRPApplicationType::value(){
    return m_d->value;
}

FRP::ApplicationType FRPApplicationType::valueNormal(){
    return m_d->value;
}

QString FRPApplicationType::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString FRPApplicationType::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void FRPApplicationType::setValueNormal(FRP::ApplicationType vv, bool emitAuto ){
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

void FRPApplicationType::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void FRPApplicationType::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void FRPApplicationType::setValue(FRP::ApplicationType vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void FRPApplicationType::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int FRPApplicationType::enumCount() {
    return m_d->enumList.size();
}

QString FRPApplicationType::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int FRPApplicationType::valueNormalInt() {
    return m_d->valueIndex();
}

class FRPLoadModeVal{
public:
    FRPLoadModeVal(FRP::LoadMode v, const QString & ns, const QString & s):
        val(v),
        normalStr(ns),
        str(s){
    }
    FRP::LoadMode val;
    QString normalStr;
    QString str;
};

class FRPLoadModePrivate {
    Q_DECLARE_TR_FUNCTIONS(FRPLoadModePrivate)
public:
    FRPLoadModePrivate( FRP::LoadMode vv ):
        value(vv){
        enumList.append( FRPLoadModeVal( FRP::persistent, "persistent", trUtf8("Persistente")) );
        enumList.append( FRPLoadModeVal( FRP::cyclic, "cyclic", trUtf8("Ciclico")) );
    }
    ~FRPLoadModePrivate(){
    }

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }

    FRP::LoadMode value;
    QList<FRPLoadModeVal> enumList;
};

FRPLoadMode::FRPLoadMode( FRP::LoadMode vv,
                                        const QString & nn,
                                        bool ro):
    EnumPlus( nn, ro ),
    m_d( new FRPLoadModePrivate(vv) ){
}

FRPLoadMode::FRPLoadMode( FRPLoadMode & val ):
    EnumPlus(val){
    m_d->value = val.m_d->value;
}

FRPLoadMode::~FRPLoadMode(){
    delete m_d;
}

QString FRPLoadMode::typeVar(){
    return QString("FRPLoadMode");
}

FRPLoadMode & FRPLoadMode::operator=( FRPLoadMode & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & FRPLoadMode::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        FRPLoadMode * valFRPLoadMode = dynamic_cast<FRPLoadMode *>(&val);
        if(valFRPLoadMode) {
            setValueNormal( valFRPLoadMode->m_d->value );
        }
    }
    return *this;
}

FRP::LoadMode FRPLoadMode::value(){
    return m_d->value;
}

FRP::LoadMode FRPLoadMode::valueNormal(){
    return m_d->value;
}

QString FRPLoadMode::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString FRPLoadMode::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void FRPLoadMode::setValueNormal(FRP::LoadMode vv, bool emitAuto ){
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

void FRPLoadMode::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void FRPLoadMode::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void FRPLoadMode::setValue(FRP::LoadMode vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void FRPLoadMode::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int FRPLoadMode::enumCount() {
    return m_d->enumList.size();
}

QString FRPLoadMode::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int FRPLoadMode::valueNormalInt() {
    return m_d->valueIndex();
}


class FRPResistenceModelVal{
public:
    FRPResistenceModelVal(FRP::ResistenceModel v, const QString & ns, const QString & s):
        val(v),
        normalStr(ns),
        str(s){
    }
    FRP::ResistenceModel val;
    QString normalStr;
    QString str;
};

class FRPResistenceModelPrivate {
    Q_DECLARE_TR_FUNCTIONS(FRPResistenceModelPrivate)
public:
    FRPResistenceModelPrivate( FRP::ResistenceModel vv ):
        value(vv){
        enumList.append( FRPResistenceModelVal( FRP::normalBending, "normalBending", trUtf8("Flessione/Pressoflessione")) );
        enumList.append( FRPResistenceModelVal( FRP::shearTorsion, "shearTorsion", trUtf8("Taglio/Torsione")) );
        enumList.append( FRPResistenceModelVal( FRP::confinement, "confinement", trUtf8("Confinamento")) );
    }
    ~FRPResistenceModelPrivate(){
    }

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }

    FRP::ResistenceModel value;
    QList<FRPResistenceModelVal> enumList;
};

FRPResistenceModel::FRPResistenceModel( FRP::ResistenceModel vv,
                                        const QString & nn,
                                        bool ro):
    EnumPlus( nn, ro ),
    m_d( new FRPResistenceModelPrivate(vv) ){
}

FRPResistenceModel::FRPResistenceModel( FRPResistenceModel & val ):
    EnumPlus(val){
    m_d->value = val.m_d->value;
}

FRPResistenceModel::~FRPResistenceModel(){
    delete m_d;
}

QString FRPResistenceModel::typeVar(){
    return QString("FRPResistenceModel");
}

FRPResistenceModel & FRPResistenceModel::operator=( FRPResistenceModel & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & FRPResistenceModel::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        FRPResistenceModel * valFRPResistenceModel = dynamic_cast<FRPResistenceModel *>(&val);
        if(valFRPResistenceModel) {
            setValueNormal( valFRPResistenceModel->m_d->value );
        }
    }
    return *this;
}

FRP::ResistenceModel FRPResistenceModel::value(){
    return m_d->value;
}

FRP::ResistenceModel FRPResistenceModel::valueNormal(){
    return m_d->value;
}

QString FRPResistenceModel::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString FRPResistenceModel::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void FRPResistenceModel::setValueNormal(FRP::ResistenceModel vv, bool emitAuto ){
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

void FRPResistenceModel::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void FRPResistenceModel::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void FRPResistenceModel::setValue(FRP::ResistenceModel vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void FRPResistenceModel::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int FRPResistenceModel::enumCount() {
    return m_d->enumList.size();
}

QString FRPResistenceModel::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int FRPResistenceModel::valueNormalInt() {
    return m_d->valueIndex();
}




FRP::FRP( UnitMeasure * ump, const QString &n, QObject * parent ):
    Material(ump, n, parent)  {
    initVar();
    if( n.isEmpty() ){
        name->setValue( "FRP" );
    } else {
        name->setValue( n );
    }
    updateReadOnlyVars();
}

FRP::FRP(UnitMeasure *ump, const QXmlStreamAttributes &attrs, QObject * parent):
    Material(ump, "", parent ){
    initVar();
    loadFromXML( attrs );
}

MaterialModel::MaterialType FRP::materialType() {
    return MaterialModel::FRPMaterial;
}

double FRP::gammaRd(FRP::ResistenceModel resModel) {
    if( resModel == FRP::normalBending ){
        return 1.00;
    } else if( resModel == FRP::shearTorsion ){
        return 1.20;
    } else if( resModel == FRP::confinement ){
        return 1.10;
    }
    return 0.0;
}

double FRP::etal(LoadMode lMode) {
    if( lMode == FRP::persistent ){
        if( fiberType->valueNormal() == FRP::epoxyGlass ){
            return 0.30;
        } else if( fiberType->valueNormal() == FRP::epoxyAramid ){
            return 0.50;
        } else if( fiberType->valueNormal() == FRP::epoxyCarbon ){
            return 0.80;
        }
    } else if( lMode == FRP::cyclic ){
        return 0.50;
    }
    return 0.0;
}

void FRP::initVar(){
    *m_typeNameInternal = "FRP";
    typeName->setValue( trUtf8("FRP"));

    E->setValueNormal( 2.0e+11 );
    E->setRichName( trUtf8("E<span style=\" vertical-align:sub;\">f</span>") );
    E->setToolTip( trUtf8("Modulo di elasticità normale nella direzione della forza"));

    exposition = new FRPExposition( FRP::internal, "exposition" );
    exposition->setRichName( trUtf8("Esposizione") );
    exposition->setToolTip( trUtf8("Condizione di esposizione") );
    addVarToContainer( exposition );

    fiberType = new FRPFiberType( FRP::epoxyCarbon, "fiberType" );
    fiberType->setRichName( trUtf8("Tipo") );
    fiberType->setToolTip( trUtf8("Tipo di fibra/resina") );
    addVarToContainer( fiberType );

    applicationType = new FRPApplicationType( FRP::B, "applicationType" );
    applicationType->setRichName( trUtf8("Tipo applicazione") );
    applicationType->setToolTip( trUtf8("Tipo di applicazione (A/B)") );
    addVarToContainer( applicationType );

    etaa = new DoublePlus( 0.0, "etaa", m_unitMeasure, UnitMeasure::noDimension, true );
    etaa->setRichName( trUtf8("η<span style=\" vertical-align:sub;\">a</span>") );
    etaa->setToolTip( trUtf8("Fattore di conversione ambientale"));
    addVarToContainer( etaa );
    etaa->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&FRP::setEtaa) );
    etaa->addConnectedVars( 2, exposition, fiberType );

    gammaf = new DoublePlus( 0.0, "gammaf", m_unitMeasure, UnitMeasure::noDimension, true );
    gammaf->setRichName( trUtf8("γ<span style=\" vertical-align:sub;\">f</span>") );
    gammaf->setToolTip( trUtf8("Coefficiente parziale a rottura per i materiali"));
    addVarToContainer( gammaf );
    gammaf->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&FRP::setGammaf) );
    gammaf->addConnectedVars( 1, applicationType );

    gammafd = new DoublePlus( 0.0, "gammafd", m_unitMeasure, UnitMeasure::noDimension, true );
    gammafd->setRichName( trUtf8("γ<span style=\" vertical-align:sub;\">f,d</span>") );
    gammafd->setToolTip( trUtf8("Coefficiente parziale a distacco per i materiali"));
    addVarToContainer( gammafd );
    gammafd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&FRP::setGammafd) );
    gammafd->addConnectedVars( 1, applicationType );

    ffk = new DoublePlus( 1.8e+9 , "ffk", m_unitMeasure, UnitMeasure::tension );
    ffk->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">fk</span>") );
    ffk->setToolTip(trUtf8("Tensione caratteristica a rottura"));
    addVarToContainer( ffk );

    epsfk = new DoublePlus(0.0, "epsfk", m_unitMeasure, UnitMeasure::percent, true );
    epsfk->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">fk</span>") );
    epsfk->setToolTip(trUtf8("Deformazione caratteristica a rottura"));
    addVarToContainer( epsfk );
    epsfk->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&FRP::setEpsfk) );
    epsfk->addConnectedVars( 2, E, ffk );
}

void FRP::setEtaa(bool emitAuto) {
    double val = 0.0;
    if( exposition->valueNormal() == FRP::internal ){
        if( fiberType->valueNormal() == FRP::epoxyGlass ){
            val = 0.75;
        } else if( fiberType->valueNormal() == FRP::epoxyAramid ){
            val = 0.85;
        } else if( fiberType->valueNormal() == FRP::epoxyCarbon ){
            val = 0.95;
        }
    } else if( exposition->valueNormal() == FRP::external ){
        if( fiberType->valueNormal() == FRP::epoxyGlass ){
            val = 0.65;
        } else if( fiberType->valueNormal() == FRP::epoxyAramid ){
            val = 0.75;
        } else if( fiberType->valueNormal() == FRP::epoxyCarbon ){
            val = 0.85;
        }
    } else if( exposition->valueNormal() == FRP::aggressive ){
        if( fiberType->valueNormal() == FRP::epoxyGlass ){
            val = 0.50;
        } else if( fiberType->valueNormal() == FRP::epoxyAramid ){
            val = 0.70;
        } else if( fiberType->valueNormal() == FRP::epoxyCarbon ){
            val = 0.85;
        }
    }
    etaa->setValueNormal( val, emitAuto );
}

void FRP::setGammaf(bool emitAuto) {
    double val = 0.0;
    if( applicationType->valueNormal() == FRP::A ){
        val = 1.10;
    } else if( applicationType->valueNormal() == FRP::B ){
        val = 1.25;
    }
    gammaf->setValueNormal( val, emitAuto );
}

void FRP::setGammafd(bool emitAuto) {
    double val = 0.0;
    if( applicationType->valueNormal() == FRP::A ){
        val = 1.20;
    } else if( applicationType->valueNormal() == FRP::B ){
        val = 1.50;
    }
    gammafd->setValueNormal( val, emitAuto );
}

void FRP::setEpsfk(bool emitAuto) {
    double val = 0.0;
    if( E->valueNormal() != 0.0 ){
        val = ffk->valueNormal() / E->valueNormal();
    }
    epsfk->setValueNormal( val, emitAuto );
}

