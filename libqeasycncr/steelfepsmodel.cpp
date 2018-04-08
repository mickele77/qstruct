#include "steelfepsmodel.h"


class SteelFEpsModelPrivate{
    class enumVal{
    public:
        enumVal(SimpleSectionRCncrComp::ModelSteelFEps v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        SimpleSectionRCncrComp::ModelSteelFEps val;
        QString normalStr;
        QString str;
    };

public:
    SteelFEpsModelPrivate(SimpleSectionRCncrComp::ModelSteelFEps v):
        value(v){
        enumList.append( enumVal( SimpleSectionRCncrComp::inclinedTopBranch, "inclinedTopBranch", QObject::trUtf8("E.P. incrudente")) );
        enumList.append( enumVal( SimpleSectionRCncrComp::horizontalTopBranch, "horizontalTopBranch", QObject::trUtf8("El. inf. plastico")) );
    }
    ~SteelFEpsModelPrivate(){
    }

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    SimpleSectionRCncrComp::ModelSteelFEps value;
    QList<enumVal> enumList;
};

SteelFEpsModel::SteelFEpsModel( SimpleSectionRCncrComp::ModelSteelFEps m,
                                const QString & nn,
                                bool ro):
    EnumPlus( nn, ro),
    m_d( new SteelFEpsModelPrivate(m )){
}

SteelFEpsModel::~SteelFEpsModel(){
    delete m_d;
}

SteelFEpsModel & SteelFEpsModel::operator=( SteelFEpsModel & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & SteelFEpsModel::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        SteelFEpsModel * valCncrModelPlus = dynamic_cast<SteelFEpsModel *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString SteelFEpsModel::typeVar(){
    return QString("SteelFEpsModel");
}

SimpleSectionRCncrComp::ModelSteelFEps SteelFEpsModel::value(){
    return m_d->value;
}

SimpleSectionRCncrComp::ModelSteelFEps SteelFEpsModel::valueNormal(){
    return m_d->value;
}

QString SteelFEpsModel::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString SteelFEpsModel::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void SteelFEpsModel::setValueNormal( SimpleSectionRCncrComp::ModelSteelFEps vv, bool emitAuto ){
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

void SteelFEpsModel::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SteelFEpsModel::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void SteelFEpsModel::setValue( SimpleSectionRCncrComp::ModelSteelFEps vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void SteelFEpsModel::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int SteelFEpsModel::enumCount() {
    return m_d->enumList.size();
}

QString SteelFEpsModel::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int SteelFEpsModel::valueNormalInt() {
    return m_d->valueIndex();
}

