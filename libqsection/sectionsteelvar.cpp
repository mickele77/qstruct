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
#include "sectionsteelvar.h"

#include <QApplication>

class SectionSteelManufactTypePrivate{
    Q_DECLARE_TR_FUNCTIONS(SectionSteelManufactTypePrivate)

    class enumVal{
    public:
        enumVal(SectionSteel::ManufactType v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        SectionSteel::ManufactType val;
        QString normalStr;
        QString str;
    };

public:
    SectionSteelManufactTypePrivate(SectionSteel::ManufactType v):
        value(v){
        enumList.append( enumVal( SectionSteel::sectionRolled, "sectionRolled", trUtf8("Laminata")) );
        enumList.append( enumVal( SectionSteel::sectionWelded, "sectionWelded", trUtf8("Composta saldata")) );
        enumList.append( enumVal( SectionSteel::sectionColdFormed, "sectionColdFormed", trUtf8("Piegata a freddo")) );
        enumList.append( enumVal( SectionSteel::sectionManufactTypeOther, "sectionManufactTypeOther", trUtf8("Altre sezioni")) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    SectionSteel::ManufactType value;
    QList<enumVal> enumList;
};

SectionSteelManufactType::SectionSteelManufactType( SectionSteel::ManufactType mt,
                                                    const QString & nn,
                                                    bool ro):
    EnumPlus( nn, ro),
    m_d( new SectionSteelManufactTypePrivate( mt) ) {
}

SectionSteelManufactType::~SectionSteelManufactType(){
}

SectionSteelManufactType & SectionSteelManufactType::operator=( SectionSteelManufactType & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & SectionSteelManufactType::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        SectionSteelManufactType * valCncrModelPlus = dynamic_cast<SectionSteelManufactType *>(&val);
        if(valCncrModelPlus)
        {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString SectionSteelManufactType::typeVar(){
    return QString("SectionSteelManufactType");
}

SectionSteel::ManufactType SectionSteelManufactType::value(){
    return m_d->value;
}

SectionSteel::ManufactType SectionSteelManufactType::valueNormal(){
    return m_d->value;
}

QString SectionSteelManufactType::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString SectionSteelManufactType::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void SectionSteelManufactType::setValueNormal( SectionSteel::ManufactType vv, bool emitAuto ){
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

void SectionSteelManufactType::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SectionSteelManufactType::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void SectionSteelManufactType::setValue( SectionSteel::ManufactType vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void SectionSteelManufactType::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int SectionSteelManufactType::enumCount() {
    return m_d->enumList.size();
}

QString SectionSteelManufactType::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int SectionSteelManufactType::valueNormalInt() {
    return m_d->valueIndex();
}

class SectionSteelShapePrivate{
    Q_DECLARE_TR_FUNCTIONS(SectionSteelShapePrivate)

    class enumVal{
    public:
        enumVal(SectionSteel::SectionShape v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        SectionSteel::SectionShape val;
        QString normalStr;
        QString str;
    };

public:
    SectionSteelShapePrivate(SectionSteel::SectionShape v):
        value(v){
        enumList.append( enumVal( SectionSteel::sectionI, "sectionI", trUtf8("Sezione ad I - H (ali uguali)")) );
        enumList.append( enumVal( SectionSteel::sectionL, "sectionL", trUtf8("Angolare")) );
        enumList.append( enumVal( SectionSteel::sectionCirc, "sectionCirc", trUtf8("Sezione circolare")) );
        enumList.append( enumVal( SectionSteel::sectionRect, "sectionRect", trUtf8("Sezione rettangolare")) );
        enumList.append( enumVal( SectionSteel::sectionOmega, "sectionOmega", trUtf8("Sezione ad Omega")) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    SectionSteel::SectionShape value;
    QList<enumVal> enumList;
};

SectionSteelShape::SectionSteelShape( SectionSteel::SectionShape tt,
                                      const QString & nn,
                                      bool ro):
    EnumPlus( nn, ro),
    m_d( new SectionSteelShapePrivate(tt)){
}

SectionSteelShape::~SectionSteelShape(){
}

SectionSteelShape & SectionSteelShape::operator=( SectionSteelShape & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & SectionSteelShape::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        SectionSteelShape * valCncrModelPlus = dynamic_cast<SectionSteelShape *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString SectionSteelShape::typeVar(){
    return QString("SectionSteelShape");
}

SectionSteel::SectionShape SectionSteelShape::value(){
    return m_d->value;
}

SectionSteel::SectionShape SectionSteelShape::valueNormal(){
    return m_d->value;
}

QString SectionSteelShape::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString SectionSteelShape::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void SectionSteelShape::setValueNormal( SectionSteel::SectionShape vv, bool emitAuto ){
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

void SectionSteelShape::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SectionSteelShape::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void SectionSteelShape::setValue( SectionSteel::SectionShape vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void SectionSteelShape::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int SectionSteelShape::enumCount() {
    return m_d->enumList.size();
}

QString SectionSteelShape::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int SectionSteelShape::valueNormalInt() {
    return m_d->valueIndex();
}

class SectionSteelMomentDistributionPrivate{
    Q_DECLARE_TR_FUNCTIONS(SectionSteelMomentDistributionPrivate)

    class enumVal{
    public:
        enumVal(SectionSteel::MomentDistribution v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        SectionSteel::MomentDistribution val;
        QString normalStr;
        QString str;
    };

public:
    SectionSteelMomentDistributionPrivate(SectionSteel::MomentDistribution v):
        value(v){
        enumList.append( enumVal( SectionSteel::momentLinear, "momentLinear", trUtf8("Lineare")) );
        enumList.append( enumVal( SectionSteel::momentBiLinear, "momentBiLinear", trUtf8("BiLineare")) );
        enumList.append( enumVal( SectionSteel::momentParabolic, "momentParabolic", trUtf8("Parabolico")) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    SectionSteel::MomentDistribution value;
    QList<enumVal> enumList;
};

SectionSteelMomentDistribution::SectionSteelMomentDistribution( SectionSteel::MomentDistribution md,
                                                                const QString & nn,
                                                                bool ro):
    EnumPlus( nn, ro),
    m_d( new SectionSteelMomentDistributionPrivate(md) ){
}

SectionSteelMomentDistribution::~SectionSteelMomentDistribution(){
}

SectionSteelMomentDistribution & SectionSteelMomentDistribution::operator=( SectionSteelMomentDistribution & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & SectionSteelMomentDistribution::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        SectionSteelMomentDistribution * valCncrModelPlus = dynamic_cast<SectionSteelMomentDistribution *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString SectionSteelMomentDistribution::typeVar(){
    return QString("SectionSteelMomentDistribution");
}

SectionSteel::MomentDistribution SectionSteelMomentDistribution::value(){
    return m_d->value;
}

SectionSteel::MomentDistribution SectionSteelMomentDistribution::valueNormal(){
    return m_d->value;
}

QString SectionSteelMomentDistribution::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString SectionSteelMomentDistribution::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void SectionSteelMomentDistribution::setValueNormal( SectionSteel::MomentDistribution vv, bool emitAuto ){
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

void SectionSteelMomentDistribution::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SectionSteelMomentDistribution::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void SectionSteelMomentDistribution::setValue( SectionSteel::MomentDistribution vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void SectionSteelMomentDistribution::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int SectionSteelMomentDistribution::enumCount() {
    return m_d->enumList.size();
}

QString SectionSteelMomentDistribution::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int SectionSteelMomentDistribution::valueNormalInt() {
    return m_d->valueIndex();
}

class SectionSteelTorsionalDeformabilityPrivate{
    Q_DECLARE_TR_FUNCTIONS(SectionSteelTorsionalDeformabilityPrivate)

    class enumVal{
    public:
        enumVal(SectionSteel::TorsionalDeformability v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        SectionSteel::TorsionalDeformability val;
        QString normalStr;
        QString str;
    };

public:
    SectionSteelTorsionalDeformabilityPrivate(SectionSteel::TorsionalDeformability v):
        value(v){
        enumList.append( enumVal( SectionSteel::torsDeformable, "torsDeformable", trUtf8("Deformabile")) );
        enumList.append( enumVal( SectionSteel::torsNotDeformable, "torsNotDeformable", trUtf8("Non deformabile")) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    SectionSteel::TorsionalDeformability value;
    QList<enumVal> enumList;
};

SectionSteelTorsionalDeformability::SectionSteelTorsionalDeformability( SectionSteel::TorsionalDeformability md,
                                                                        const QString & nn,
                                                                        bool ro):
    EnumPlus( nn, ro),
    m_d( new SectionSteelTorsionalDeformabilityPrivate(md) ){
}

SectionSteelTorsionalDeformability::~SectionSteelTorsionalDeformability(){
}

SectionSteelTorsionalDeformability & SectionSteelTorsionalDeformability::operator=( SectionSteelTorsionalDeformability & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & SectionSteelTorsionalDeformability::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        SectionSteelTorsionalDeformability * valCncrModelPlus = dynamic_cast<SectionSteelTorsionalDeformability *>(&val);
        if(valCncrModelPlus)
        {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString SectionSteelTorsionalDeformability::typeVar(){
    return QString("SectionSteelTorsionalDeformability");
}

SectionSteel::TorsionalDeformability SectionSteelTorsionalDeformability::value(){
    return m_d->value;
}

SectionSteel::TorsionalDeformability SectionSteelTorsionalDeformability::valueNormal(){
    return m_d->value;
}

QString SectionSteelTorsionalDeformability::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString SectionSteelTorsionalDeformability::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void SectionSteelTorsionalDeformability::setValueNormal( SectionSteel::TorsionalDeformability vv, bool emitAuto ){
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

void SectionSteelTorsionalDeformability::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SectionSteelTorsionalDeformability::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void SectionSteelTorsionalDeformability::setValue( SectionSteel::TorsionalDeformability vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void SectionSteelTorsionalDeformability::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int SectionSteelTorsionalDeformability::enumCount() {
    return m_d->enumList.size();
}

QString SectionSteelTorsionalDeformability::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int SectionSteelTorsionalDeformability::valueNormalInt() {
    return m_d->valueIndex();
}
