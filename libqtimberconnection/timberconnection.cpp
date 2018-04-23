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
#include "timberconnection.h"

#include "timber.h"
#include "steel.h"
#include "timberconnector.h"
#include "doubleplus.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "lstype.h"

#include <QString>
#include <QMap>
#include <QXmlAttributes>
#include <QXmlStreamWriter>

#include <cmath>

class TimberConnectionPrivate{
public:
    TimberConnectionPrivate( UnitMeasure * ump,
                             ServiceClass * sc,
                             LoadDurationClass * ld,
                             LSType * ls ):
        timber1( NULL ),
        timber2( NULL ),
        steel( NULL ),
        connector( new TimberConnector(ump, 6.4e+8, 8.0e+8, 12e-3 ) ){
        if( sc == 0 ){
            sClass = new ServiceClass( ServiceClass::SC1, "Classe di servizio" );
            sClassWasCreated = true;
        } else {
            sClass = sc;
            sClassWasCreated = false;
        }
        if( ld == 0 ){
            ldClass = new LoadDurationClass( LoadDurationClass::permanent, "Classe di durata" );
            ldClassWasCreated = true;
        } else {
            ldClass = ld;
            ldClassWasCreated = false;
        }
        if( ls == 0 ){
            lsType = new LSType( LSType::ULSFundamental, "Tipologia Stato Limite" );
            lsTypeWasCreated = true;
        } else {
            lsType = ls;
            lsTypeWasCreated = false;
        }
    }
    ~TimberConnectionPrivate(){
        delete connector;

        if( sClassWasCreated ){
            VarPlus::deleteVarPlus( sClass );
        }
        if( ldClassWasCreated ){
            VarPlus::deleteVarPlus( ldClass );
        }
        if( lsTypeWasCreated ){
            VarPlus::deleteVarPlus( lsType );
        }
    }

    Timber * timber1;
    Timber * timber2;
    Steel * steel;
    TimberConnector * connector;
    ServiceClass * sClass;
    bool sClassWasCreated;
    LoadDurationClass * ldClass;
    bool ldClassWasCreated;
    LSType * lsType;
    bool lsTypeWasCreated;
};

TimberConnection::TimberConnection(UnitMeasure * ump,
                                   const QString &n,
                                   Timber *timb,
                                   ServiceClass * sc,
                                   LoadDurationClass * ld,
                                   LSType * ls,
                                   QObject * parent ):
    QObject( parent ),
    VarPlusContainer( ump, "TimberConnection", n ),
    m_d( new TimberConnectionPrivate(ump, sc, ld, ls) ){
    initVar();
    setTimber1( timb );
    setTimber2( timb );
}

TimberConnection::TimberConnection(UnitMeasure *ump,
                                   const QXmlStreamAttributes &attrs,
                                   MaterialModel *mModel,
                                   ServiceClass * sc,
                                   LoadDurationClass * ld,
                                   LSType * ls,
                                   QObject *parent):
    QObject( parent ),
    VarPlusContainer( ump, "TimberConnection", "" ),
    m_d( new TimberConnectionPrivate(ump, sc, ld, ls) ){
    initVar();
    loadFromXML( attrs, mModel );
}

TimberConnection::~TimberConnection(){
    delete m_d;
}

void TimberConnection::loadFromXML( const QXmlStreamAttributes &attrs, MaterialModel * mModel ){
    for ( int i=0; i<attrs.count(); ++i ) {
        for( QMap<QString, VarPlus *>::iterator iter = m_varContainer->begin(); iter != m_varContainer->end(); ++iter ){
            if ( attrs.at(i).name().toString().toUpper() == (*iter)->name().toUpper() ) {
                (*iter)->setValueNormal( attrs.at(i).value().toString() );
            } else if ( attrs.at(i).name().toString().toUpper() == "TIMBER1" ) {
                Timber * t = dynamic_cast<Timber *>(mModel->materialId( attrs.at(i).value().toString().toUInt() ) );
                if( t ){
                    setTimber1( t );
                }
            } else if ( attrs.at(i).name().toString().toUpper() == "TIMBER2" ) {
                Timber * t = dynamic_cast<Timber *>(mModel->materialId( attrs.at(i).value().toString().toUInt() ) );
                if( t ){
                    setTimber2( t );
                }
            } else if ( attrs.at(i).name().toString().toUpper() == "STEEL" ) {
                Steel * st = dynamic_cast<Steel *>(mModel->materialId( attrs.at(i).value().toString().toUInt() ) );
                if( st ){
                    setSteel( st );
                }
            }
        }
    }
    updateReadOnlyVars();
}

void TimberConnection::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement(*m_typeNameInternal);
    for( QMap<QString, VarPlus *>::iterator i = m_varContainer->begin(); i != m_varContainer->end(); ++i) {
        writer->writeAttribute( (*i)->xmlAttribute() );
    }
    if( m_d->timber1 ){
        writer->writeAttribute( "timber1", m_d->timber1->id->valueNormalStr() );
    }
    if( m_d->timber2 ){
        writer->writeAttribute( "timber2", m_d->timber2->id->valueNormalStr() );
    }
    if( m_d->steel ){
        writer->writeAttribute( "steel", m_d->steel->id->valueNormalStr() );
    }
    writer->writeEndElement();
}

TimberConnection & TimberConnection::operator=( TimberConnection & val){
    if( this != &val ){
        m_d->connector = val.m_d->connector;
        m_d->timber1 = val.m_d->timber1;
        m_d->timber2 = val.m_d->timber2;
        m_d->steel = val.m_d->steel;
        VarPlusContainer::operator=(val);
    }
    return *this;
}

ServiceClass *TimberConnection::sClass() {
    return m_d->sClass;
}

LoadDurationClass *TimberConnection::ldClass() {
    return m_d->ldClass;
}

LSType *TimberConnection::lsType() {
    return m_d->lsType;
}

TimberConnector * TimberConnection::connector(){ return m_d->connector; }

void TimberConnection::setConnector( TimberConnector * con ){ m_d->connector = con; }

Timber * TimberConnection::timber1(){ return m_d->timber1; }

void TimberConnection::setTimber1( Timber * timb ){
    if( timb != m_d->timber1 ){
        if( m_d->timber1 ){
            disconnect( m_d->timber1, SIGNAL(destroyed()), this, SLOT(setTimber1NULL()));
            disconnect( m_d->timber1->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFh0k1()));
            disconnect( m_d->timber1->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk1A()));
            disconnect( m_d->timber1->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2A()));
            disconnect( m_d->timber1->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2B()));
            disconnect( m_d->timber1, SIGNAL(materialChanged()), this, SLOT(setKmod()));
        }
        m_d->timber1 = timb;
        emit timber1Changed();
        if( m_d->timber1 ){
            connect( m_d->timber1, SIGNAL(destroyed()), this, SLOT(setTimber1NULL()));
            connect( m_d->timber1->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFh0k1()));
            setFh0k1();
            connect( m_d->timber1->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk1A()));
            setFAxk1A();
            connect( m_d->timber1->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2A()));
            setFAxk2A();
            connect( m_d->timber1->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2B()));
            setFAxk2B();
            connect( m_d->timber1, SIGNAL(materialChanged()), this, SLOT(setKmod()));
            setKmod();
        }
    }
}

void TimberConnection::setTimber1NULL(){
    setTimber1( NULL );
}

Timber * TimberConnection::timber2(){ return m_d->timber2; }

void TimberConnection::setTimber2( Timber * timb ){
    if( timb != m_d->timber2 ){
        if( m_d->timber2 ){
            disconnect( m_d->timber2, SIGNAL(destroyed()), this, SLOT(setTimber2NULL()));
            disconnect( m_d->timber2->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFh0k2()));
            disconnect( m_d->timber2->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2A()));
            disconnect( m_d->timber2->fc90k, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2A()));
            disconnect( m_d->timber2->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2B()));
            disconnect( m_d->timber2, SIGNAL(materialChanged()), this, SLOT(setKmod()));
        }
        m_d->timber2 = timb;
        emit timber2Changed();
        if( m_d->timber2 ){
            connect( m_d->timber2, SIGNAL(destroyed()), this, SLOT(setTimber2NULL()));
            connect( m_d->timber2->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFh0k2()));
            setFh0k2();
            connect( m_d->timber2->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2A()));
            connect( m_d->timber2->fc90k, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2A()));
            setFAxk2A();
            connect( m_d->timber2->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2B()));
            setFAxk2B();
            connect( m_d->timber2, SIGNAL(materialChanged()), this, SLOT(setKmod()));
            setKmod();
        }
    }
}

void TimberConnection::setTimber2NULL(){
    setTimber2( NULL );
}

Steel * TimberConnection::steel(){ return m_d->steel; }

void TimberConnection::setSteel( Steel * st ){
    if( st != m_d->steel ){
        m_d->steel = st;
        emit steelChanged();
        if( m_d->steel ){
            connect( m_d->steel, SIGNAL(destroyed()), this, SLOT(setSteelNULL()));
        }
    }
}

bool TimberConnection::verifyNormal(double FAxEd, double FVEd, double * imp, QString * messages) {
    *imp = 0.0;
    if( FAxRdTot->valueNormal() == 0.0 && FAxEd == 0.0) {
        if( FVRdTot->valueNormal() == 0.0 && FVEd == 0.0){
            *imp = 1.0;
            return true;
        } else if( FVRdTot->valueNormal() != 0.0 ){
            *imp = fabs( FVEd / FVRdTot->valueNormal() );
            messages->append( trUtf8("ImpV = Imp = ") + QString::number(*imp) + QString("\n"));
            if( *imp > 1.0 ){
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    }
    if (FVRdTot->valueNormal() == 0.0 && FVEd == 0.0) {
        if( FAxRdTot->valueNormal() != 0.0 ){
            *imp = fabs( FAxEd / FAxRdTot->valueNormal() );
            messages->append( trUtf8("ImpAx = Imp = ") + QString::number(*imp) + QString("\n"));
            if( *imp > 1.0 ){
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    }
    if (FAxRdTot->valueNormal() != 0.0 && FVRdTot->valueNormal() != 0.0){
        double impAx = fabs( FAxEd / FAxRdTot->valueNormal() );
        messages->append( trUtf8("ImpAx = ") + QString::number(impAx) + QString("\n"));
        double impV = fabs( FVEd / FVRdTot->valueNormal() );
        messages->append( trUtf8("ImpV = ") + QString::number(impV) + QString("\n"));
        if( m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothRoundNail ||
                m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothSquareNail ){
            *imp = impAx + impV;
        } else if( m_d->connector->connectorType->valueNormal() == TimberConnector::OtherRoundNail ||
                   m_d->connector->connectorType->valueNormal() == TimberConnector::OtherSquareNail ||
                   m_d->connector->connectorType->valueNormal() == TimberConnector::Screw ){
            *imp = pow( pow(impAx,2.0) + pow(impV,2.0) , 0.50 );
        } else if( m_d->connector->connectorType->valueNormal() == TimberConnector::Bolt ){
            *imp = impAx > impV? impAx: impV;
        }
        messages->append( trUtf8("Imp = ") + QString::number(*imp) + QString("\n"));
        if( *imp > 1.0 ){
            return false;
        } else {
            return true;
        }
    }
    return false;
}

bool TimberConnection::verify(DoublePlus *FAxEd, DoublePlus *FVEd, DoublePlus * res, QString * messages ) {
    double rr;
    bool ret = verifyNormal( FAxEd->valueNormal(), FVEd->valueNormal(), &rr, messages );
    res->setValueNormal( rr );
    return ret;
}

void TimberConnection::setSteelNULL(){
    setSteel( NULL );
}

void TimberConnection::initVar(){
    tTimber1 = new DoublePlus( 5.0e-2, "tTimber1", m_unitMeasure, UnitMeasure::sectL );
    tTimber1->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">T,1</span>") );
    tTimber1->setToolTip( trUtf8("Spessore legno 1"));
    tTimber1->setReadOnly( false );
    addVarToContainer( tTimber1 );

    tTimber2 = new DoublePlus( 5.0e-2, "tTimber2", m_unitMeasure, UnitMeasure::sectL );
    tTimber2->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">T,2</span>") );
    tTimber2->setToolTip( trUtf8("Spessore legno 2"));
    tTimber2->setReadOnly( false );
    addVarToContainer( tTimber2 );

    tSteel = new DoublePlus( 4.0e-3, "tSteel", m_unitMeasure, UnitMeasure::sectL );
    tSteel->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">S</span>") );
    tSteel->setToolTip( trUtf8("Spessore della piastra di acciaio"));
    tSteel->setReadOnly( false );
    addVarToContainer( tSteel );

    m_gammaM = new QList<double>;
    m_gammaM->append(1.5);
    m_gammaM->append(1.0);
    m_gammaM->append(1.0);

    gammaM = new DoublePlus( 1.0, "gammaM", m_unitMeasure, UnitMeasure::noDimension );
    gammaM->setRichName(trUtf8("γ<span style=\" vertical-align:sub;\">M</span>"));
    gammaM->setReadOnly( true );
    addVarToContainer( gammaM );
    connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(setGammaM()) );
    setGammaM();

    kmod = new DoublePlus( 0.0, "kmod", m_unitMeasure, UnitMeasure::noDimension );
    kmod->setRichName(trUtf8("k<span style=\" vertical-align:sub;\">mod</span>"));
    kmod->setReadOnly( true );
    addVarToContainer( kmod );
    connect( kmod, SIGNAL(readOnlyChanged(bool)), this, SLOT(setKmod()));
    connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(setKmod()));
    connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(setKmod()));

    connectionType = new TimberConnectionType( TimberConnection::TTSingleShear, "connectionType");
    connectionType->setRichName( trUtf8("Tipologia connessione"));
    connectionType->setReadOnly( false );
    addVarToContainer( connectionType );

    fAxk1A = new DoublePlus( 0.0, "fAxk1A", m_unitMeasure, UnitMeasure::tension );
    fAxk1A->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">Ax,k,1A</span>") );
    fAxk1A->setToolTip( trUtf8(""));
    fAxk1A->setReadOnly( true );
    addVarToContainer( fAxk1A );
    connect( m_d->connector->connectorType, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk1A()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk1A()));
    connect( fAxk1A, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFAxk1A()));
    setFAxk1A();

    fAxk1B = new DoublePlus( 0.0, "fAxk1B", m_unitMeasure, UnitMeasure::tension );
    fAxk1B->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">Ax,k,1B</span>") );
    fAxk1B->setToolTip( trUtf8(""));
    fAxk1B->setReadOnly( true );
    addVarToContainer( fAxk1B );
    connect( fAxk1A, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk1B()));
    connect( m_d->connector->alpha, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk1B()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk1B()));
    connect( fAxk1B, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFAxk1B()));
    setFAxk1B();

    fAxk2A = new DoublePlus( 0.0, "fAxk2A", m_unitMeasure, UnitMeasure::tension );
    fAxk2A->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">Ax,k,2A</span>") );
    fAxk2A->setToolTip( trUtf8(""));
    fAxk2A->setReadOnly( true );
    addVarToContainer( fAxk2A );
    connect( m_d->connector->connectorType, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2A()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2A()));
    connect( fAxk2A, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFAxk2A()));
    setFAxk2A();

    fAxk2B = new DoublePlus( 0.0, "fAxk2B", m_unitMeasure, UnitMeasure::tension );
    fAxk2B->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">Ax,k,2B</span>") );
    fAxk2B->setToolTip( trUtf8(""));
    fAxk2B->setReadOnly( true );
    addVarToContainer( fAxk2B );
    connect( m_d->connector->connectorType, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2B()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFAxk2B()));
    connect( fAxk2B, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFAxk2B()));
    setFAxk2B();

    FAxRk = new DoublePlus( 0.0, "FAxRk", m_unitMeasure, UnitMeasure::loadF );
    FAxRk->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">ax,Rk</span>"));
    FAxRk->setReadOnly( true );
    addVarToContainer( FAxRk );
    connect( tSteel, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( tTimber1, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( m_d->connector->lEf, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( m_d->connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( m_d->connector->dHead, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( m_d->connector->fyk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( fAxk1A, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( fAxk1B, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( fAxk2A, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( fAxk2B, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRk()));
    connect( FAxRk, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFAxRk()));
    setFAxRk();

    FAxRd = new DoublePlus( 0.0, "FAxRd", m_unitMeasure, UnitMeasure::loadF );
    FAxRd->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">ax,Rd</span>"));
    FAxRd->setReadOnly( true );
    addVarToContainer( FAxRd );
    connect( FAxRk, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRd()));
    connect( gammaM, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRd()));
    connect( kmod, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRd()));
    connect( FAxRd, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFAxRd()));

    connect( m_d->connector->connectorType, SIGNAL(valueChanged(QString)), this, SLOT(setVisibleVarConnector()));
    setVisibleVarConnector();

    fh0k1 = new DoublePlus( 0.0, "fh0k1", m_unitMeasure, UnitMeasure::tension );
    fh0k1->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">h,0,k,1</span>") );
    fh0k1->setToolTip( trUtf8("Valore caratteristico di rifollamento nel caso di chiodi inseriti con preforatura, lungo la direzione delle fibre in legno - Legno 1"));
    fh0k1->setReadOnly( true );
    addVarToContainer( fh0k1 );
    connect( fh0k1, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFh0k1()));
    connect( m_d->connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setFh0k1()));
    connect( this, SIGNAL(timber1Changed()), this, SLOT(setFh0k1()));
    setFh0k1();

    fhAlphak1 = new DoublePlus( 0.0, "fhAlphak1", m_unitMeasure, UnitMeasure::tension );
    fhAlphak1->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">h,α,k,1</span>") );
    fhAlphak1->setToolTip( trUtf8("Valore caratteristico di rifollamento nel caso di chiodi inseriti con preforatura, angolo α rispetto alle fibre in legno - Legno 1"));
    fhAlphak1->setReadOnly( true );
    addVarToContainer( fhAlphak1 );
    connect( fh0k1, SIGNAL(valueChanged(QString)), this, SLOT(setFhAlphak1()));
    connect( m_d->connector->alpha, SIGNAL(valueChanged(QString)), this, SLOT(setFhAlphak1()));
    connect( fhAlphak1, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFhAlphak1()));
    setFhAlphak1();

    fh0k2 = new DoublePlus( 0.0, "fh0k2", m_unitMeasure, UnitMeasure::tension );
    fh0k2->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">h,0,k,2</span>") );
    fh0k2->setToolTip(  trUtf8("Valore caratteristico di rifollamento nel caso di chiodi inseriti con preforatura, lungo la direzione delle fibre in legno - Legno 2"));
    fh0k2->setReadOnly( true );
    addVarToContainer( fh0k2 );
    connect( fh0k2, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFh0k2()));
    connect( this, SIGNAL(timber2Changed()), this, SLOT(setFh0k2()));
    connect( m_d->connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setFh0k2()));
    setFh0k2();

    fhAlphak2 = new DoublePlus( 0.0, "fhAlphak2", m_unitMeasure, UnitMeasure::tension );
    fhAlphak2->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">h,α,k,2</span>") );
    fhAlphak2->setToolTip( trUtf8("Valore caratteristico di rifollamento nel caso di chiodi inseriti con preforatura, angolo α rispetto alle fibre in legno - Legno 2"));
    fhAlphak2->setReadOnly( true );
    addVarToContainer( fhAlphak2 );
    connect( fh0k2, SIGNAL(valueChanged(QString)), this, SLOT(setFhAlphak2()));
    connect( m_d->connector->alpha, SIGNAL(valueChanged(QString)), this, SLOT(setFhAlphak2()));
    connect( fhAlphak2, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFhAlphak2()));
    setFhAlphak2();

    beta = new DoublePlus( 0.0, "beta", m_unitMeasure, UnitMeasure::noDimension );
    beta->setRichName( trUtf8("β") );
    beta->setToolTip( trUtf8("Rapporto tra le tensioni caratteristiche di rifollamento (f<span style=\" vertical-align:sub;\">h,2,k</span>/f<span style=\" vertical-align:sub;\">h,1,k</span>)"));
    beta->setReadOnly( true );
    addVarToContainer( beta );
    connect( fhAlphak1, SIGNAL(valueChanged(QString)), this, SLOT(setBeta()));
    connect( fhAlphak2, SIGNAL(valueChanged(QString)), this, SLOT(setBeta()));

    FVRk1A = new DoublePlus( 0.0, "FVRk1A", m_unitMeasure, UnitMeasure::loadF );
    FVRk1A->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">v,Rk,IA</span>"));
    FVRk1A->setReadOnly( true );
    addVarToContainer( FVRk1A );
    connect( fhAlphak1, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1A()));
    connect( tTimber1, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1A()));
    connect( m_d->connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1A()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1A()));
    connect( FVRk1A, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFVRk1A()));
    setFVRk1A();

    FVRk1B = new DoublePlus( 0.0, "FVRk1B", m_unitMeasure, UnitMeasure::loadF );
    FVRk1B->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">v,Rk,IB</span>"));
    FVRk1B->setReadOnly( true );
    addVarToContainer( FVRk1B );
    connect( fhAlphak1, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1B()));
    connect( tTimber1, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1B()));
    connect( fhAlphak2, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1B()));
    connect( tTimber2, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1B()));
    connect( m_d->connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1B()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1B()));
    connect( FVRk1B, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFVRk1B()));
    setFVRk1B();

    FVRk1C = new DoublePlus( 0.0, "FVRk1C", m_unitMeasure, UnitMeasure::loadF );
    FVRk1C->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">v,Rk,IC</span>"));
    FVRk1C->setReadOnly( true );
    addVarToContainer( FVRk1C );
    connect( fhAlphak1, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1C()));
    connect( beta, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1C()));
    connect( tTimber1, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1C()));
    connect( tTimber2, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1C()));
    connect( m_d->connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1C()));
    connect( FAxRk, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1C()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk1C()));
    connect( FVRk1C, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFVRk1C()));
    setFVRk1C();

    FVRk2A = new DoublePlus( 0.0, "FVRk2A", m_unitMeasure, UnitMeasure::loadF );
    FVRk2A->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">v,Rk,IIA</span>"));
    FVRk2A->setReadOnly( true );
    addVarToContainer( FVRk2A );
    connect( fhAlphak1, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2A()));
    connect( beta, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2A()));
    connect( tTimber1, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2A()));
    connect( m_d->connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2A()));
    connect( m_d->connector->MyRk, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2A()));
    connect( FAxRk, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2A()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2A()));
    connect( FVRk2A, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFVRk2A()));
    setFVRk2A();

    FVRk2B = new DoublePlus( 0.0, "FVRk2B", m_unitMeasure, UnitMeasure::loadF );
    FVRk2B->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">v,Rk,IIB</span>"));
    FVRk2B->setReadOnly( true );
    addVarToContainer( FVRk2B );
    connect( fhAlphak1, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2B()));
    connect( beta, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2B()));
    connect( tTimber2, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2B()));
    connect( m_d->connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2B()));
    connect( m_d->connector->MyRk, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2B()));
    connect( FAxRk, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2B()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk2B()));
    connect( FVRk2B, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFVRk2B()));
    setFVRk2B();

    FVRk3 = new DoublePlus( 0.0, "FVRk3", m_unitMeasure, UnitMeasure::loadF );
    FVRk3->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">v,Rk,III</span>"));
    FVRk3->setReadOnly( true );
    addVarToContainer( FVRk3 );
    connect( fhAlphak1, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk3()));
    connect( beta, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk3()));
    connect( m_d->connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk3()));
    connect( m_d->connector->MyRk, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk3()));
    connect( FAxRk, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk3()));
    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk3()));
    connect( FVRk3, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFVRk3()));
    setFVRk3();

    FVRk = new DoublePlus( 0.0, "FVRk", m_unitMeasure, UnitMeasure::loadF );
    FVRk->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">v,Rk</span>"));
    FVRk->setReadOnly( true );
    addVarToContainer( FVRk );
    connect( FVRk1A, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk()));
    connect( FVRk1B, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk()));
    connect( FVRk1C, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk()));
    connect( FVRk2A, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk()));
    connect( FVRk2B, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk()));
    connect( FVRk3, SIGNAL(valueChanged(QString)), this, SLOT(setFVRk()));
    setFVRk();

    FVRd = new DoublePlus( 0.0, "FVRd", m_unitMeasure, UnitMeasure::loadF );
    FVRd->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">v,Rd</span>"));
    FVRd->setReadOnly( true );
    addVarToContainer( FVRd );
    connect( FVRd, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFVRd()));
    connect( FVRk, SIGNAL(valueChanged(QString)), this, SLOT(setFVRd()));
    connect( gammaM, SIGNAL(valueChanged(QString)), this, SLOT(setFVRd()));
    connect( kmod, SIGNAL(valueChanged(QString)), this, SLOT(setFVRd()));

    connect( connectionType, SIGNAL(valueChanged(QString)), this, SLOT(setVisibleVar()));
    setVisibleVar();

    a1 = new DoublePlus( 0.0, "a1", m_unitMeasure, UnitMeasure::sectL );
    a1->setRichName( trUtf8("a<span style=\" vertical-align:sub;\">1</span>"));
    a1->setToolTip( trUtf8("Spaziatura dei connettori parallelamente alle fibre di legno"));
    a1->setReadOnly( false );
    addVarToContainer( a1 );

    nPar = new DoublePlus( 1.0, "nPar", m_unitMeasure, UnitMeasure::noDimension );
    nPar->setRichName( trUtf8("n<span style=\" vertical-align:sub;\">par</span>"));
    nPar->setToolTip( trUtf8("Numero effettivo dei connettori, parallelo alle fibre") );
    nPar->setReadOnly( false );
    addVarToContainer( nPar );

    nPerp = new DoublePlus( 1.0, "nPerp", m_unitMeasure, UnitMeasure::noDimension );
    nPerp->setRichName( trUtf8("n<span style=\" vertical-align:sub;\">perp</span>"));
    nPerp->setToolTip( trUtf8("Numero effettivo dei connettori, perpendicolare alle fibre") );
    nPerp->setReadOnly( false );
    addVarToContainer( nPerp );

    nVEf = new DoublePlus( 0.0, "nVEf", m_unitMeasure, UnitMeasure::noDimension );
    nVEf->setRichName( trUtf8("n<span style=\" vertical-align:sub;\">V,ef</span>"));
    nVEf->setToolTip( trUtf8("Numero efficace dei connettori per sforzo di taglio"));
    nVEf->setReadOnly( true );
    addVarToContainer( nVEf );
    connect( nPar, SIGNAL(valueChanged(QString)), this, SLOT(setNVEf()));
    connect( nPerp, SIGNAL(valueChanged(QString)), this, SLOT(setNVEf()));
    connect( a1, SIGNAL(valueChanged(QString)), this, SLOT(setNVEf()));
    connect( m_d->connector->alpha, SIGNAL(valueChanged(QString)), this, SLOT(setNVEf()));
    connect( nVEf, SIGNAL(readOnlyChanged(bool)), this, SLOT(setNVEf()));
    setNVEf();

    FVRdTot = new DoublePlus( 0.0, "FVRdTot", m_unitMeasure, UnitMeasure::loadF );
    FVRdTot->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">v,Rd,tot</span>"));
    FVRdTot->setReadOnly( true );
    FVRdTot->setToolTip( trUtf8("Resistenza di progetto complessiva del collegamento per carichi trasversali (resistenza del singolo connettore * numero efficace)"));
    addVarToContainer( FVRdTot );
    connect( FVRd, SIGNAL(valueChanged(QString)), this, SLOT(setFVRdTot()));
    connect( nVEf, SIGNAL(valueChanged(QString)), this, SLOT(setFVRdTot()));
    connect( FVRdTot, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFVRdTot()));

    nAxEf = new DoublePlus( 0.0, "nAxEf", m_unitMeasure, UnitMeasure::noDimension );
    nAxEf->setRichName( trUtf8("n<span style=\" vertical-align:sub;\">Ax,ef</span>"));
    nAxEf->setToolTip( trUtf8("Numero efficace dei connettori per sforzo di estrazione"));
    nAxEf->setReadOnly( true );
    addVarToContainer( nAxEf );
    connect( nPar, SIGNAL(valueChanged(QString)), this, SLOT(setNAxEf()));
    connect( nPerp, SIGNAL(valueChanged(QString)), this, SLOT(setNAxEf()));
    connect( nAxEf, SIGNAL(readOnlyChanged(bool)), this, SLOT(setNAxEf()));
    setNAxEf();

    FAxRdTot = new DoublePlus( 0.0, "FAxRdTot", m_unitMeasure, UnitMeasure::loadF );
    FAxRdTot->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">Ax,Rd,tot</span>"));
    FAxRdTot->setReadOnly( true );
    FAxRdTot->setToolTip( trUtf8("Resistenza di progetto complessiva del collegamento per carichi assiali (resistenza del singolo connettore * numero efficace)"));
    addVarToContainer( FAxRdTot );
    connect( FAxRd, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRdTot()));
    connect( nAxEf, SIGNAL(valueChanged(QString)), this, SLOT(setFAxRdTot()));
    connect( FAxRdTot, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFAxRdTot()));
}

void TimberConnection::setGammaM(){
    if( gammaM->readOnly() ){
        if( m_d->lsType->valueNormal() < m_gammaM->size() )
            gammaM->setValueNormal( m_gammaM->at( (int) (m_d->lsType->valueNormal()) ) );
        else if ( m_gammaM->size() > 1 )
            gammaM->setValueNormal( m_gammaM->at( 0 ) );
        else
            gammaM->setValueNormal( 1.0 );
    }
}

void TimberConnection::setKmod(){
    if( kmod->readOnly() ){
        if( m_d->timber1 ){
            double k1 = m_d->timber1->kmodNormal( m_d->sClass->valueNormal(), m_d->ldClass->valueNormal());
            if( connectionType->valueNormal() == TimberConnection::TTDoubleShear || connectionType->valueNormal() == TimberConnection::TTSingleShear ) {
                if( m_d->timber2 ){
                    double k2 = m_d->timber2->kmodNormal( m_d->sClass->valueNormal(), m_d->ldClass->valueNormal());
                    kmod->setValueNormal( pow( k1 * k2, 0.5) ); // EC5 (2.6)
                    return;
                } else {
                    kmod->setValueNormal( 0.0 );
                    return;
                }
            } else {
                kmod->setValueNormal( k1 );
                return;
            }
        } else {
            kmod->setValueNormal( 0.0 );
            return;
        }
    }
}

void TimberConnection::setFAxk1A(){
    double ret = 0.0;
    if( m_d->connector->connectorType->valueNormal() == TimberConnector::Screw ){
        if( m_d->timber1 ){
            ret = ( 3.6 * 1.0e-3 * pow( 1.0e-1 * m_d->timber1->gammaWk->valueNormal(), 1.5) ) * 1.0e+6;
        }
    } else if ( m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothRoundNail ||
                m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothSquareNail ||
                m_d->connector->connectorType->valueNormal() == TimberConnector::OtherRoundNail ||
                m_d->connector->connectorType->valueNormal() == TimberConnector::OtherSquareNail ){
        if( m_d->timber1 ){
            ret = (2.0e+1 * 1.0e-6 * pow( m_d->timber1->gammaWk->valueNormal() * 1.0e-1, 2.0 )) * 1.0e+6;
        }
    } else if( m_d->connector->connectorType->valueNormal() == TimberConnector::Bolt ){
        if( m_d->timber1 ){
            ret = 3.0  * m_d->timber1->fc90k->valueNormal();
        }
    }
    fAxk1A->setValueNormal( ret );
}

void TimberConnection::setFAxk1B(){
    double ret = 0.0;
    if( m_d->connector->connectorType->valueNormal() == TimberConnector::Screw ){
        ret = fAxk1A->valueNormal() / (pow(sin( m_d->connector->alpha->valueNormal()), 2.0) + 1.50 * pow(cos( m_d->connector->alpha->valueNormal()), 2.0));
    }
    fAxk1B->setValueNormal( ret );
}

void TimberConnection::setFAxk2A(){
    double ret = 0.0;
    if ( m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothRoundNail ||
         m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothSquareNail ||
         m_d->connector->connectorType->valueNormal() == TimberConnector::OtherRoundNail ||
         m_d->connector->connectorType->valueNormal() == TimberConnector::OtherSquareNail ){
        if( m_d->timber2 ){
            ret = (2.0e+1 * 1.0e-6 * pow( m_d->timber2->gammaWk->valueNormal() * 1.0e-1, 2.0 )) * 1.0e+6;
        }
    } else if( m_d->connector->connectorType->valueNormal() == TimberConnector::Bolt ){
        if( m_d->timber2 ){
            ret = 3.0  * m_d->timber2->fc90k->valueNormal();
        }
    }
    fAxk2A->setValueNormal( ret );
}

void TimberConnection::setFAxk2B(){
    double ret = 0.0;
    if ( m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothRoundNail ||
         m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothSquareNail ||
         m_d->connector->connectorType->valueNormal() == TimberConnector::OtherRoundNail ||
         m_d->connector->connectorType->valueNormal() == TimberConnector::OtherSquareNail ){
        if( m_d->timber2 ){
            ret = (7.0e+1 * 1.0e-6 * pow( m_d->timber2->gammaWk->valueNormal() * 1.0e-1, 2.0 )) * 1.0e+6;
        }
    }
    fAxk2B->setValueNormal( ret );
}

void TimberConnection::setFAxRk(){
    if( FAxRk->readOnly() ){
        double ret = 0.0;
        if( m_d->connector->connectorType->valueNormal() == TimberConnector::Screw ){
            double l = m_d->connector->lEf->valueNormal();
            if( l > (tTimber1->valueNormal() - m_d->connector->d->valueNormal()) )
                l = tTimber1->valueNormal() - m_d->connector->d->valueNormal();
            ret =  pow( M_PI * m_d->connector->d->valueNormal() * 1.0e+3 * l * 1.0e+3, 0.8) * fAxk1B->valueNormal() * 1.0e-6;
        } else if ( m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothRoundNail ||
                    m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothSquareNail) {
            if( connectionType->valueNormal() == TimberConnection::TTDoubleShear ||
                    connectionType->valueNormal() == TimberConnection::TTSingleShear  ){
                ret = tTimber1->valueNormal() * fAxk1A->valueNormal() * m_d->connector->d->valueNormal();
                double retAlt = tTimber2->valueNormal() * fAxk2A->valueNormal() * m_d->connector->d->valueNormal() + fAxk2B->valueNormal() * pow(m_d->connector->dHead->valueNormal(), 2.0);
                if( retAlt < ret ){
                    ret = retAlt;
                }
            } else {
                ret = tTimber1->valueNormal() * fAxk1A->valueNormal() * m_d->connector->d->valueNormal();
            }
        } else if ( m_d->connector->connectorType->valueNormal() == TimberConnector::OtherRoundNail ||
                    m_d->connector->connectorType->valueNormal() == TimberConnector::OtherSquareNail ){
            if( connectionType->valueNormal() == TimberConnection::TTDoubleShear ||
                    connectionType->valueNormal() == TimberConnection::TTSingleShear  ){
                ret = tTimber1->valueNormal() * fAxk1A->valueNormal() * m_d->connector->d->valueNormal();
                double retAlt = fAxk2B->valueNormal() * pow(m_d->connector->dHead->valueNormal(), 2.0);
                if( retAlt < ret ){
                    ret = retAlt;
                }
            } else {
                ret = tTimber1->valueNormal() * fAxk1A->valueNormal() * m_d->connector->d->valueNormal();
            }
        } else if( m_d->connector->connectorType->valueNormal() == TimberConnector::Bolt ){
            if( connectionType->valueNormal() == TimberConnection::TSDoubleShearCentralSteel ||
                    connectionType->valueNormal() == TimberConnection::TSDoubleShearOuterSteel ||
                    connectionType->valueNormal() == TimberConnection::TSSingleShear ){
                double dH = m_d->connector->dHead->valueNormal();
                double dPlate = 12.0 * tSteel->valueNormal();
                if( (4.0 * m_d->connector->d->valueNormal()) < dPlate ){
                    dPlate = (4.0 * m_d->connector->d->valueNormal());
                }
                if( dPlate > dH ){
                    dH = dPlate;
                }
                ret = fAxk1A->valueNormal() * M_PI * (pow(dH/2.0, 2.0) - pow(m_d->connector->d->valueNormal()/2.0, 2.0));
                double FConnector = M_PI * pow( m_d->connector->d->valueNormal()/2.0, 2.0) * m_d->connector->fyk->valueNormal();
                if( ret > FConnector ){
                    ret = FConnector;
                }
            } else if( connectionType->valueNormal() == TimberConnection::TTDoubleShear ||
                       connectionType->valueNormal() == TimberConnection::TTSingleShear ){
                double fAxk = fAxk1A->valueNormal() < fAxk2A->valueNormal()? fAxk1A->valueNormal(): fAxk2A->valueNormal();
                ret = fAxk * M_PI * (pow(m_d->connector->dHead->valueNormal()/2.0, 2.0) - pow(m_d->connector->d->valueNormal()/2.0, 2.0));
            }
        }
        FAxRk->setValueNormal( ret );
    }
}

void TimberConnection::setFAxRd(){
    if( FAxRd->readOnly() ){
        if( gammaM->valueNormal() != 0.0 ){
            FAxRd->setValueNormal( FAxRk->valueNormal() * kmod->valueNormal() / gammaM->valueNormal() );
        } else {
            FAxRd->setValueNormal( 0.0 );
        }
    }
}

double TimberConnection::maxVAxApport(){
    double perc = 0.0;
    switch( m_d->connector->connectorType->valueNormal() ){
    case TimberConnector::Bolt : {
        perc = 0.25;
        break;
    }
    case TimberConnector::Screw : {
        perc = 1.0;
        break;
    }
    case TimberConnector::SmoothRoundNail : {
        perc = 0.15;
        break;
    }
    case TimberConnector::SmoothSquareNail : {
        perc = 0.25;
        break;
    }
    case TimberConnector::OtherRoundNail : {
        perc = 0.15;
        break;
    }
    case TimberConnector::OtherSquareNail : {
        perc = 0.15;
        break;
    }
    }
    return perc;
}

double TimberConnection::fh0kNormal( Timber * timb ){
    if( timb ){
        if( (m_d->connector->connectorType->valueNormal() == TimberConnector::Bolt) ||  \
                (m_d->connector->connectorType->valueNormal() == TimberConnector::Screw && m_d->connector->d->valueNormal() > 6.0e-3 ) ){
            return ( 1.0e+6 * 8.2e-2 * (1.0 - 10.0 * m_d->connector->d->valueNormal()) * 1.0e-1 * timb->gammaWk->valueNormal() );
        } else if( m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothRoundNail ||
                   m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothSquareNail ||
                   m_d->connector->connectorType->valueNormal() == TimberConnector::OtherRoundNail ||
                   m_d->connector->connectorType->valueNormal() == TimberConnector::OtherSquareNail ||  \
                   (m_d->connector->connectorType->valueNormal() == TimberConnector::Screw && m_d->connector->d->valueNormal() <= 6.0e-3 ) ) {
            if( m_d->connector->d->valueNormal() <= 8.0e-3 ){
                if( m_d->connector->predrilledHole->valueNormal() == TimberConnector::WithPredrilledHole ){
                    return ( 1.0e+6 * 8.2e-2 * (1.0 - 10.0 * m_d->connector->d->valueNormal()) * 1.0e-1 * timb->gammaWk->valueNormal() );
                } else {
                    return ( 1.0e+6 * 8.2e-2 * (1.0e-1 * timb->gammaWk->valueNormal()) * pow(m_d->connector->d->valueNormal() * 1.0e+3, -0.3));
                }
            }
        }
    }
    return 0.0;
}

double TimberConnection::fhAlphakNormal( Timber * timb ){
    if( timb ){
        if( (m_d->connector->connectorType->valueNormal() == TimberConnector::Bolt) ||  \
                (m_d->connector->connectorType->valueNormal() == TimberConnector::Screw && m_d->connector->d->valueNormal() > 6.0e-3 ) ){
            double k90 = 1.0;
            if( m_d->timber1->timberType->valueNormal() == Timber::LVL ){
                k90 = 1.30 + 0.015 * m_d->connector->d->valueNormal() * 1e+3;
            } else {
                if( m_d->timber1->woodEssence->valueNormal() == Timber::hardwoods ) {
                    k90 = 0.90 + 0.015 * m_d->connector->d->valueNormal() * 1e+3;
                } else {
                    k90 = 1.35 + 0.015 * m_d->connector->d->valueNormal() * 1e+3;
                }
            }
            double a = M_PI/2.0 - m_d->connector->alpha->valueNormal();
            double val = fh0k1->valueNormal() / (k90 * pow(sin(a), 2.0) + pow(cos(a), 2.0));
            return val;
        } else if( m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothRoundNail ||
                   m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothSquareNail ||
                   m_d->connector->connectorType->valueNormal() == TimberConnector::OtherRoundNail ||
                   m_d->connector->connectorType->valueNormal() == TimberConnector::OtherSquareNail ||
                   (m_d->connector->connectorType->valueNormal() == TimberConnector::Screw && m_d->connector->d->valueNormal() <= 6.0e-3 ) ) {
            return ( fh0k1->valueNormal() );
        }
    }
    return 0.0;
}


void TimberConnection::setFh0k1(){
    if( fh0k1->readOnly() ){
        fh0k1->setValueNormal( fh0kNormal( m_d->timber1 ) );
    }
}

void TimberConnection::setFhAlphak1(){
    if( fhAlphak1->readOnly() ){
        fhAlphak1->setValueNormal( fhAlphakNormal( m_d->timber1 ) );
    }
}

void TimberConnection::setFh0k2(){
    if( fh0k2->readOnly() ){
        fh0k2->setValueNormal( fh0kNormal( m_d->timber2 ) );
    }
}

void TimberConnection::setFhAlphak2(){
    if( fhAlphak2->readOnly() ){
        fhAlphak2->setValueNormal( fhAlphakNormal( m_d->timber2 ) );
    }
}

void TimberConnection::setBeta(){
    if( beta->readOnly() ){
        if( fhAlphak1->valueNormal() != 0.0 ){
            beta->setValueNormal( fhAlphak2->valueNormal() / fhAlphak1->valueNormal() );
        } else {
            beta->setValueNormal( 0.0 );
        }
    }
}

void TimberConnection::setFVRk1A(){
    if( FVRk1A->readOnly() ){
        double ret = 0.0;
        if( (connectionType->valueNormal() == TimberConnection::TTSingleShear) || (connectionType->valueNormal() == TimberConnection::TTDoubleShear)){
            ret = fhAlphak1->valueNormal() * tTimber1->valueNormal() * m_d->connector->d->valueNormal();
        }
        if( (connectionType->valueNormal() == TimberConnection::TSSingleShear) ){
            ret = 0.40 * fhAlphak1->valueNormal() * tTimber1->valueNormal() * m_d->connector->d->valueNormal();
        }
        FVRk1A->setValueNormal( ret );
    }
}

void TimberConnection::setFVRk1B(){
    if( FVRk1B->readOnly() ){
        double ret = 0.0;
        if( connectionType->valueNormal() == TimberConnection::TTSingleShear ){
            ret = fhAlphak2->valueNormal() * tTimber2->valueNormal() * m_d->connector->d->valueNormal();
        }
        if( connectionType->valueNormal() == TimberConnection::TTDoubleShear ){
            ret = 0.50 * fhAlphak2->valueNormal() * tTimber2->valueNormal() * m_d->connector->d->valueNormal();
        }
        if( connectionType->valueNormal() == TimberConnection::TSDoubleShearOuterSteel ){
            ret = 0.50 * fhAlphak1->valueNormal() * tTimber1->valueNormal() * m_d->connector->d->valueNormal();
        }
        if( (connectionType->valueNormal() == TimberConnection::TSSingleShear) ||
                (connectionType->valueNormal() == TimberConnection::TSDoubleShearCentralSteel) ){
            ret = fhAlphak1->valueNormal() * tTimber1->valueNormal() * m_d->connector->d->valueNormal();
        }
        FVRk1B->setValueNormal( ret );
    }
}

void TimberConnection::setFVRk1C(){
    if( FVRk1C->readOnly() ){
        double ret = 0.0;
        if( connectionType->valueNormal() == TimberConnection::TTSingleShear ){
            double b = beta->valueNormal();
            if( (1.0 + b) != 0.0 && tTimber1->valueNormal() != 0.0 ){
                double r = tTimber2->valueNormal() / tTimber1->valueNormal();
                double johanPart = fhAlphak1->valueNormal() * tTimber1->valueNormal() * m_d->connector->d->valueNormal() / (1.0 + beta->valueNormal()) * \
                        ( pow(b*(1.0 + 2.0 * b * (1.0 + r+ r*r) + (b*b) * (r*r)), 0.50) - b * (1.0 + r) );
                double axPart = FAxRk->valueNormal() / 4.0;
                double perc = maxVAxApport();
                if( perc * johanPart < axPart ){
                    axPart = perc * johanPart;
                }
                ret = johanPart + axPart;
            }
        }
        FVRk1C->setValueNormal( ret );
    }
}

void TimberConnection::setFVRk2A(){
    if( FVRk2A->readOnly() ){
        double ret = 0.0;
        if((connectionType->valueNormal() == TimberConnection::TTSingleShear) || \
                (connectionType->valueNormal() == TimberConnection::TTDoubleShear)) {
            double b = beta->valueNormal();
            double den = fhAlphak1->valueNormal() * m_d->connector->d->valueNormal() * pow( tTimber1->valueNormal(), 2.0 );
            if( (2.0+b)!= 0.0 &&  den != 0.0 ){
                double johanPart = 1.05 * fhAlphak1->valueNormal() * tTimber1->valueNormal() * m_d->connector->d->valueNormal() / (2.0 + b) * \
                        ( pow( (2.0*b*(1.0+b) + 4.0*b*(2.0+b)*m_d->connector->MyRk->valueNormal() / den),0.50) - b);
                double axPart = FAxRk->valueNormal() / 4.0;
                double perc = maxVAxApport();
                if( perc * johanPart < axPart ){
                    axPart = perc * johanPart;
                }
                ret = axPart + johanPart;
            }
        }
        if( (connectionType->valueNormal() == TimberConnection::TSSingleShear) || \
                (connectionType->valueNormal() == TimberConnection::TSDoubleShearOuterSteel) ){
            double johanPart = (1.15 * pow( 2.0 * m_d->connector->MyRk->valueNormal() * fhAlphak1->valueNormal() * m_d->connector->d->valueNormal(),0.50 ) );
            double axPart = FAxRk->valueNormal() / 4.0;
            double perc = maxVAxApport();
            if( perc * johanPart < axPart ){
                axPart = perc * johanPart;
            }
            ret = axPart + johanPart;
        }

        FVRk2A->setValueNormal( ret );
    }
}

void TimberConnection::setFVRk2B(){
    if( FVRk2B->readOnly() ){
        double ret = 0.0;
        if( connectionType->valueNormal() == TimberConnection::TTSingleShear) {
            double b = beta->valueNormal();
            double den = fhAlphak1->valueNormal() * m_d->connector->d->valueNormal() * pow( tTimber2->valueNormal(), 2.0 );
            if( (1.0+2.0*b)!= 0.0 &&  den != 0.0 ){
                double johanPart = 1.05 * fhAlphak1->valueNormal() * m_d->connector->d->valueNormal() * tTimber2->valueNormal() / (1.0 + 2.0*b) * \
                        ( pow( (2.0*b*b*(1.0+b) + 4.0*b*(1.0+2.0*b)*m_d->connector->MyRk->valueNormal() / den),0.50) - b);
                double axPart = FAxRk->valueNormal() / 4.0;
                double perc = maxVAxApport();
                if( perc * johanPart < axPart ){
                    axPart = perc * johanPart;
                }
                ret = axPart + johanPart;
            }
        }

        if( (connectionType->valueNormal() == TimberConnection::TSSingleShear) || \
                (connectionType->valueNormal() == TimberConnection::TSDoubleShearCentralSteel) || \
                (connectionType->valueNormal() == TimberConnection::TSDoubleShearOuterSteel) ){
            double den = fhAlphak1->valueNormal() * m_d->connector->d->valueNormal() * pow( tTimber1->valueNormal(), 2.0 );
            if( den != 0.0 ){
                double johanPart = fhAlphak1->valueNormal() * tTimber1->valueNormal() * m_d->connector->d->valueNormal() * \
                        (pow( 2.0 + 4.0 * m_d->connector->MyRk->valueNormal() / den,0.50) - 1.0);
                double axPart = FAxRk->valueNormal() / 4.0;
                double perc = maxVAxApport();
                if( perc * johanPart < axPart ){
                    axPart = perc * johanPart;
                }
                ret = axPart + johanPart;
            }
        }
        FVRk2B->setValueNormal( ret );
    }
}

void TimberConnection::setFVRk3(){
    if( FVRk3->readOnly() ){
        double ret = 0.0;
        if((connectionType->valueNormal() == TimberConnection::TTSingleShear) || \
                (connectionType->valueNormal() == TimberConnection::TTDoubleShear)) {
            double b = beta->valueNormal();
            if( (1.0+b)!= 0.0 ){
                double johanPart = 1.15 * pow( (4.0*b*m_d->connector->MyRk->valueNormal() * fhAlphak1->valueNormal() * m_d->connector->d->valueNormal() ) / (1.0+b),0.50 );
                double axPart = FAxRk->valueNormal() / 4.0;
                double perc = maxVAxApport();
                if( perc * johanPart < axPart ){
                    axPart = perc * johanPart;
                }
                ret = axPart + johanPart;
            }
        } else if( (connectionType->valueNormal() == TimberConnection::TSSingleShear) || \
                   (connectionType->valueNormal() == TimberConnection::TSDoubleShearCentralSteel) || \
                   (connectionType->valueNormal() == TimberConnection::TSDoubleShearOuterSteel) ) {
            ret = (2.30 * pow( m_d->connector->MyRk->valueNormal() * fhAlphak1->valueNormal() * m_d->connector->d->valueNormal(), 0.50) );
        }
        FVRk3->setValueNormal( ret );
    }
}

void TimberConnection::setFVRk(){
    if( FVRk->readOnly() ){
        double ret = 0.0;
        switch( connectionType->valueNormal() ){
        case TimberConnection::TTSingleShear : {
            ret = FVRk1A->valueNormal();
            if( ret > FVRk1B->valueNormal() ){
                ret = FVRk1B->valueNormal();
            }
            if( ret > FVRk1C->valueNormal() ){
                ret = FVRk1C->valueNormal();
            }
            if( ret > FVRk2A->valueNormal() ){
                ret = FVRk2A->valueNormal();
            }
            if( ret > FVRk2B->valueNormal() ){
                ret = FVRk2B->valueNormal();
            }
            if( ret > FVRk3->valueNormal() ){
                ret = FVRk3->valueNormal();
            }
            break;
        }
        case TimberConnection::TTDoubleShear: {
            ret = FVRk1A->valueNormal();
            if( ret > FVRk1B->valueNormal() ){
                ret = FVRk1B->valueNormal();
            }
            if( ret > FVRk2A->valueNormal() ){
                ret = FVRk2A->valueNormal();
            }
            if( ret > FVRk3->valueNormal() ){
                ret = FVRk3->valueNormal();
            }
            break;        }
        case TimberConnection::TSSingleShear :{
            if( m_d->connector->d->valueNormal() > 0.0 && tSteel->valueNormal() > 0.0 ){
                if( tSteel->valueNormal() <= 0.5 * m_d->connector->d->valueNormal() ){
                    ret = FVRk1A->valueNormal();
                    if( ret > FVRk2A->valueNormal() ){
                        ret = FVRk2A->valueNormal();
                    }
                } else if( tSteel->valueNormal() >= m_d->connector->d->valueNormal() ){
                    ret = FVRk1B->valueNormal();
                    if( ret > FVRk2B->valueNormal() ){
                        ret = FVRk2B->valueNormal();
                    }
                    if( ret > FVRk3->valueNormal() ){
                        ret = FVRk3->valueNormal();
                    }
                } else {
                    double retThin = FVRk1A->valueNormal();
                    if( retThin > FVRk2A->valueNormal() ){
                        retThin = FVRk2A->valueNormal();
                    }
                    double retThick = FVRk1B->valueNormal();
                    if( retThick > FVRk2B->valueNormal() ){
                        retThick = FVRk2B->valueNormal();
                    }
                    if( retThick > FVRk3->valueNormal() ){
                        retThick = FVRk3->valueNormal();
                    }
                    ret = retThin + (retThick - retThin) / (0.5 * m_d->connector->d->valueNormal()) * (tSteel->valueNormal() - 0.5 * m_d->connector->d->valueNormal());
                }
            }
            break;
        }
        case TimberConnection::TSDoubleShearCentralSteel :{
            ret = FVRk1B->valueNormal();
            if( ret > FVRk2B->valueNormal() ){
                ret = FVRk2B->valueNormal();
            }
            if( ret > FVRk3->valueNormal() ){
                ret = FVRk3->valueNormal();
            }
            break;
        }
        case TimberConnection::TSDoubleShearOuterSteel :{
            if( m_d->connector->d->valueNormal() > 0.0 && tSteel->valueNormal() > 0.0 ){
                if( tSteel->valueNormal() <= 0.5 * m_d->connector->d->valueNormal() ){
                    ret = FVRk1B->valueNormal();
                    if( ret > FVRk2A->valueNormal() ){
                        ret = FVRk2A->valueNormal();
                    }
                } else if( tSteel->valueNormal() >= m_d->connector->d->valueNormal() ){
                    ret = FVRk1B->valueNormal();
                    if( ret > FVRk3->valueNormal() ){
                        ret = FVRk3->valueNormal();
                    }
                } else {
                    double retThin = FVRk1B->valueNormal();
                    if( retThin > FVRk2A->valueNormal() ){
                        retThin = FVRk2A->valueNormal();
                    }
                    double retThick = FVRk1B->valueNormal();
                    if( retThick > FVRk3->valueNormal() ){
                        retThick = FVRk3->valueNormal();
                    }
                    ret = retThin + (retThick - retThin) / (0.5 * m_d->connector->d->valueNormal()) * (tSteel->valueNormal() - 0.5 * m_d->connector->d->valueNormal());
                }
            }
            break;
        }
        }

        FVRk->setValueNormal( ret );
    }
}

void TimberConnection::setFVRd(){
    if( FVRd->readOnly() ){
        if( gammaM->valueNormal() != 0.0 ){
            FVRd->setValueNormal( FVRk->valueNormal() * kmod->valueNormal() / gammaM->valueNormal() );
        } else {
            FVRd->setValueNormal( 0.0 );
        }
    }
}

void TimberConnection::setVisibleVar(){
    switch( connectionType->valueNormal() ){
    case TimberConnection::TTSingleShear:{
        tTimber2->setEnabled( true );
        tSteel->setEnabled( false );
        fhAlphak2->setEnabled( true );
        beta->setEnabled( true );
        FVRk1A->setEnabled( true );
        FVRk1A->setToolTip( trUtf8("EC5 (8.6a)"));
        FVRk1B->setEnabled( true );
        FVRk1B->setToolTip( trUtf8("EC5 (8.6b)"));
        FVRk1C->setEnabled( true );
        FVRk1C->setToolTip( trUtf8("EC5 (8.6c)"));
        FVRk2A->setEnabled( true );
        FVRk2A->setToolTip( trUtf8("EC5 (8.6d)"));
        FVRk2B->setEnabled( true );
        FVRk2B->setToolTip( trUtf8("EC5 (8.6e)"));
        FVRk3->setEnabled( true );
        FVRk3->setToolTip( trUtf8("EC5 (8.6f)"));
        break;
    }
    case TimberConnection::TTDoubleShear:{
        tTimber2->setEnabled( true );
        fhAlphak2->setEnabled( true );
        beta->setEnabled( true );
        tSteel->setEnabled( false );
        FVRk1A->setEnabled( true );
        FVRk1A->setToolTip( trUtf8("EC5 (8.7g)"));
        FVRk1B->setEnabled( true );
        FVRk1B->setToolTip( trUtf8("EC5 (8.7h)"));
        FVRk1C->setEnabled( false );
        FVRk1C->setToolTip( "" );
        FVRk2A->setEnabled( true );
        FVRk2A->setToolTip( trUtf8("EC5 (8.7j)"));
        FVRk2B->setEnabled( false);
        FVRk2B->setToolTip( "" );
        FVRk3->setEnabled( true );
        FVRk3->setToolTip( trUtf8("EC5 (8.7k)"));
        break;
    }
    case TimberConnection::TSSingleShear:{
        tTimber2->setEnabled( false );
        fhAlphak2->setEnabled( false );
        beta->setEnabled( false );
        tSteel->setEnabled( true );
        FVRk1A->setEnabled( true );
        FVRk1A->setToolTip( trUtf8("EC5 (8.9a)"));
        FVRk1B->setEnabled( true );
        FVRk1B->setToolTip( trUtf8("EC5 (8.10e)"));
        FVRk1C->setEnabled( false );
        FVRk1C->setToolTip( "" );
        FVRk2A->setEnabled( true );
        FVRk2A->setToolTip( "EC5 - (8.9b)" );
        FVRk2B->setEnabled( true );
        FVRk2B->setToolTip( "EC5 - (8.10c)" );
        FVRk3->setEnabled( true );
        FVRk3->setToolTip( "EC5 - (8.10d)" );
        break;
    }
    case TimberConnection::TSDoubleShearCentralSteel:{
        tTimber2->setEnabled( false );
        fhAlphak2->setEnabled( false );
        beta->setEnabled( false );
        FVRk1A->setEnabled( false );
        FVRk1A->setToolTip( trUtf8("EC5 (8.9a)"));
        FVRk1B->setEnabled( true );
        FVRk1B->setToolTip( trUtf8("EC5 (8.11f)"));
        FVRk1C->setEnabled( false );
        FVRk1C->setToolTip( "" );
        FVRk2A->setEnabled( false );
        FVRk2A->setToolTip( "" );
        FVRk2B->setEnabled( true );
        FVRk2B->setToolTip( trUtf8("EC5 (8.11g)"));
        FVRk3->setEnabled( true );
        FVRk3->setToolTip( trUtf8("EC5 (8.11h)"));
        break;
    }
    case TimberConnection::TSDoubleShearOuterSteel:{
        tTimber2->setEnabled( false );
        fhAlphak2->setEnabled( false );
        beta->setEnabled( false );
        FVRk1A->setEnabled( false );
        FVRk1A->setToolTip( trUtf8("EC5 (8.9a)"));
        FVRk1B->setEnabled( true );
        FVRk1B->setToolTip( trUtf8("EC5 (8.12j) e (8.13l)") );
        FVRk1C->setEnabled( false );
        FVRk1C->setToolTip( "" );
        FVRk2A->setEnabled( true );
        FVRk2A->setToolTip( trUtf8("EC5 (8.12k)") );
        FVRk2B->setEnabled( false );
        FVRk2B->setToolTip( "" );
        FVRk3->setEnabled( true );
        FVRk3->setToolTip( trUtf8("EC5 (8.13m)" ) );
        break;
    }
    default: {
        tTimber2->setEnabled( false );
        fhAlphak2->setEnabled( false );
        beta->setEnabled( false );
        FVRk1A->setEnabled( false );
        FVRk1A->setToolTip( "" );
        FVRk1B->setEnabled( false );
        FVRk1B->setToolTip( "" );
        FVRk1C->setEnabled( false );
        FVRk1C->setToolTip( "" );
        FVRk2A->setEnabled( false );
        FVRk2A->setToolTip( "" );
        FVRk2B->setEnabled( false );
        FVRk2B->setToolTip( "" );
        FVRk3->setEnabled( false );
        FVRk3->setToolTip( "" );
        break;
    }
    }
}

void TimberConnection::setVisibleVarConnector(){
    if( m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothRoundNail ||
            m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothSquareNail ||
            m_d->connector->connectorType->valueNormal() == TimberConnector::OtherRoundNail ||
            m_d->connector->connectorType->valueNormal() == TimberConnector::OtherSquareNail ){
        fAxk1A->setEnabled( true );
        fAxk1A->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">Ax,k,1</span>") );
        fAxk1A->setToolTip( trUtf8("Valore caratteristico della tensione di estrazione, legno 1 (lato infissione)"));
        fAxk1B->setEnabled( false );
        fAxk1B->setRichName( "" );
        fAxk1B->setToolTip( "" );
        fAxk2A->setEnabled( false );
        fAxk2A->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">Ax,k,2</span>") );
        fAxk2A->setToolTip( trUtf8("Valore caratteristico della tensione di estrazione, legno 2 (lato testa del chiodo)"));
        fAxk2B->setEnabled( false );
        fAxk2B->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">head,k,2</span>") );
        fAxk2B->setToolTip( trUtf8("Valore caratteristico della tensione di estrazione sotto la testa del chiodo, legno 2 (lato testa del chiodo)"));
    } else if( m_d->connector->connectorType->valueNormal() == TimberConnector::Bolt ){
        fAxk1A->setEnabled( true );
        fAxk1A->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">washer,k,1</span>") );
        fAxk1A->setToolTip( trUtf8("Valore caratteristico della tensione resistente massima sotto la rondella, legno 1"));
        fAxk1B->setEnabled( false );
        fAxk1B->setRichName( "" );
        fAxk1B->setToolTip( "" );
        fAxk2A->setEnabled( true );
        fAxk2A->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">washer,k,2</span>") );
        fAxk2A->setToolTip( trUtf8("Valore caratteristico della tensione resistente massima sotto la rondella, legno 2"));
        fAxk2B->setEnabled( false );
        fAxk2B->setRichName( "" );
        fAxk2B->setToolTip( "" );
    } else if( m_d->connector->connectorType->valueNormal() == TimberConnector::Screw ){
        fAxk1A->setEnabled( true );
        fAxk1A->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">Ax,k,0</span>") );
        fAxk1A->setToolTip( trUtf8("Valore caratteristico della tensione di estrazione, secondo direzione perpendicolare alle fibre di legno"));
        fAxk1B->setEnabled( true );
        fAxk1B->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">Ax,α,k</span>") );
        fAxk1B->setToolTip( trUtf8("Valore caratteristico della tensione di estrazione, secondo direzione inclinata di alpha rispetto alle fibre di legno"));
        fAxk2A->setEnabled( false );
        fAxk2A->setValueNormal( 0.0 );
        fAxk2A->setRichName( "" );
        fAxk2A->setToolTip( "" );
        fAxk2B->setEnabled( false );
        fAxk2B->setValueNormal( 0.0 );
        fAxk2B->setRichName( "" );
        fAxk2B->setToolTip( "" );
    }
}

void TimberConnection::setNVEf(){
    if( m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothRoundNail ||
            m_d->connector->connectorType->valueNormal() == TimberConnector::SmoothSquareNail ||
            m_d->connector->connectorType->valueNormal() == TimberConnector::OtherRoundNail ||
            m_d->connector->connectorType->valueNormal() == TimberConnector::OtherSquareNail ||
            ((m_d->connector->connectorType->valueNormal() == TimberConnector::Screw) && (m_d->connector->d->valueNormal() <= 6.0e-3) )){
        double kef = 0.0;
        if( m_d->connector->predrilledHole->valueNormal() == TimberConnector::WithoutPredrilledHole ){
            if( a1->valueNormal() < (7.0 * m_d->connector->d->valueNormal()) ){
                nVEf->setValueNormal( 0.0 );
                return;
            } else if( a1->valueNormal() >= (7.0 * m_d->connector->d->valueNormal()) && a1->valueNormal() < (10.0 * m_d->connector->d->valueNormal())){
                kef = 0.7 + (0.85-0.7) * (a1->valueNormal() - (7.0 * m_d->connector->d->valueNormal())) / (3.0 * m_d->connector->d->valueNormal());
            } else if( a1->valueNormal() >= (10.0 * m_d->connector->d->valueNormal()) && a1->valueNormal() < (14.0 * m_d->connector->d->valueNormal())){
                kef = 0.85 + (1.0 - 0.85) * (a1->valueNormal() - (10.0 * m_d->connector->d->valueNormal())) / (4.0 * m_d->connector->d->valueNormal());
            } else {
                kef = 1.0;
            }
        } else if( m_d->connector->predrilledHole->valueNormal() == TimberConnector::WithPredrilledHole ){
            if( a1->valueNormal() < (4.0 * m_d->connector->d->valueNormal()) ){
                nVEf->setValueNormal( 0.0 );
                return;
            } else if( a1->valueNormal() >= (4.0 * m_d->connector->d->valueNormal()) && a1->valueNormal() < (7.0 * m_d->connector->d->valueNormal())){
                kef = 0.5 + (0.7-0.5) * (a1->valueNormal() - (4.0 * m_d->connector->d->valueNormal())) / (3.0 * m_d->connector->d->valueNormal());
            } else if( a1->valueNormal() >= (7.0 * m_d->connector->d->valueNormal()) && a1->valueNormal() < (10.0 * m_d->connector->d->valueNormal())){
                kef = 0.7 + (0.85-0.7) * (a1->valueNormal() - (7.0 * m_d->connector->d->valueNormal())) / (3.0 * m_d->connector->d->valueNormal());
            } else if( a1->valueNormal() >= (10.0 * m_d->connector->d->valueNormal()) && a1->valueNormal() < (14.0 * m_d->connector->d->valueNormal())){
                kef = 0.85 + (1.0 - 0.85) * (a1->valueNormal() - (10.0 * m_d->connector->d->valueNormal())) / (4.0 * m_d->connector->d->valueNormal());
            } else {
                kef = 1.0;
            }
        }
        double n0Ef = pow(nPar->valueNormal(), kef);
        double nParV = n0Ef + (nPar->valueNormal()-n0Ef) * m_d->connector->alpha->valueNormal() / (M_PI / 2.0);
        if( nParV < 1 ){
            nParV = 1;
        }
        nVEf->setValueNormal( nParV * nPerp->valueNormal()  );
        return;
    } else if( m_d->connector->connectorType->valueNormal() == TimberConnector::Bolt  ||
               ((m_d->connector->connectorType->valueNormal() == TimberConnector::Screw) && (m_d->connector->d->valueNormal() > 6.0e-3) )){
        double n0Ef = pow(nPar->valueNormal(), 0.9) * pow( a1->valueNormal() / (13.0 * m_d->connector->d->valueNormal()),0.25);
        if( n0Ef > nPar->valueNormal() ){
            n0Ef = nPar->valueNormal();
        }
        double nParV = n0Ef + (nPar->valueNormal()-n0Ef) * m_d->connector->alpha->valueNormal() / (M_PI / 2.0);
        if( nParV < 1 ){
            nParV = 1;
        }
        nVEf->setValueNormal( nParV * nPerp->valueNormal() );
        return;
    }
}

void TimberConnection::setFVRdTot() {
    if( FVRdTot->readOnly() ){
        FVRdTot->setValueNormal( nVEf->valueNormal() * FVRd->valueNormal() );
    }
}

void TimberConnection::setNAxEf(){
    if( m_d->connector->connectorType->valueNormal() == TimberConnector::Screw ){
        nAxEf->setValueNormal( pow(nPar->valueNormal() * nPerp->valueNormal(),0.9) );
    } else {
        nAxEf->setValueNormal( nPar->valueNormal() * nPerp->valueNormal() );
    }
}

void TimberConnection::setFAxRdTot() {
    if( FAxRdTot->readOnly() ){
        FAxRdTot->setValueNormal( nAxEf->valueNormal() * FAxRd->valueNormal() );
    }
}

#include <QCoreApplication>

class TimberConnectionTypePrivate{
    Q_DECLARE_TR_FUNCTIONS(TimberConnectionTypePrivate)

    class enumVal{
    public:
        enumVal(TimberConnection::ConnectionType v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        TimberConnection::ConnectionType val;
        QString normalStr;
        QString str;
    };

public:
    TimberConnectionTypePrivate(TimberConnection::ConnectionType v):
        value(v){
        enumList.append( enumVal( TimberConnection::TTSingleShear, "TTSingleShear", trUtf8("Legno-Legno - Piano di taglio singolo")) );
        enumList.append( enumVal( TimberConnection::TTDoubleShear, "TTDoubleShear", trUtf8("Legno-Legno - Piano di taglio doppio")) );
        enumList.append( enumVal( TimberConnection::TSSingleShear, "TSSingleShear", trUtf8("Legno-Acciaio - Piano di taglio singolo")) );
        enumList.append( enumVal( TimberConnection::TSDoubleShearCentralSteel, "TSDoubleShearCentralSteel", trUtf8("Legno-Acciaio - Piano di taglio doppio - Piastra interna")) );
        enumList.append( enumVal( TimberConnection::TSDoubleShearOuterSteel, "TSDoubleShearOuterSteel", trUtf8("Legno-Acciaio - Piano di taglio doppio - Piastre esterne")) );
    };
    ~TimberConnectionTypePrivate(){
    };

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    TimberConnection::ConnectionType value;
    QList<enumVal> enumList;
};

TimberConnectionType::TimberConnectionType( TimberConnection::ConnectionType tt,
                                            const QString & nn,
                                            bool ro):
    EnumPlus( nn, ro),
    m_d( new TimberConnectionTypePrivate(tt) ){
}

TimberConnectionType::~TimberConnectionType(){
    delete m_d;
}

TimberConnectionType & TimberConnectionType::operator=( TimberConnectionType & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & TimberConnectionType::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        TimberConnectionType * valCncrModelPlus = dynamic_cast<TimberConnectionType *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString TimberConnectionType::typeVar(){
    return QString("TimberConnectionType");
}

TimberConnection::ConnectionType TimberConnectionType::value(){
    return m_d->value;
}

TimberConnection::ConnectionType TimberConnectionType::valueNormal(){
    return m_d->value;
}

QString TimberConnectionType::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString TimberConnectionType::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void TimberConnectionType::setValueNormal( TimberConnection::ConnectionType vv, bool emitAuto ){
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

void TimberConnectionType::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void TimberConnectionType::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void TimberConnectionType::setValue( TimberConnection::ConnectionType vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void TimberConnectionType::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int TimberConnectionType::enumCount() {
    return m_d->enumList.size();
}

QString TimberConnectionType::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int TimberConnectionType::valueNormalInt() {
    return m_d->valueIndex();
}
