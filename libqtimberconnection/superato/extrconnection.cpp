#include "extrconnection.h"

#include "timberconnector.h"
#include "timber.h"
#include "doubleplus.h"
#include <cmath>

ExtrConnection::ExtrConnection( UnitMeasure * ump,
                                Timber * timb,
                                TimberConnector * con,
                                ServiceClass * sc,
                                LoadDurationClass * ld,
                                LSType * ls,
                                QObject * parent ):
    TimberConnection( ump, timb, con, sc, ld, ls, parent ) {
    *m_connectionName = "ExtrConnection";
    initVar();
}

void ExtrConnection::initVar(){
    tTimber = new DoublePlus( 0.0, "tTimber", m_unitMeasure, UnitMeasure::sectLength, VarPlus::readWrite );
    tTimber->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">T</span>"));
    tTimber->setToolTip( trUtf8("Spessore elemento ligneo attraversato"));
    tTimber->setAutomatic( false );
    addVarToContainer( tTimber );

    f1k = new DoublePlus( 0.0, "f1k", m_unitMeasure, UnitMeasure::tension, VarPlus::readWrite );
    f1k->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">1,k</span>"));
    f1k->setToolTip( trUtf8("Parametro di resistenza ad estrazione della punta del chiodo"));
    f1k->setAutomatic( true );
    addVarToContainer( f1k );
    connect( f1k, SIGNAL(automaticChanged(bool)), this, SLOT(setf1k()));
    setf1k();

    f2k = new DoublePlus( 0.0, "f2k", m_unitMeasure, UnitMeasure::tension, VarPlus::readWrite );
    f2k->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">2,k</span>"));
    f2k->setToolTip( trUtf8("Parametro di resistenza ad estrazione della testa del chiodo"));
    f2k->setAutomatic( true );
    addVarToContainer( f2k );
    connect( f2k, SIGNAL(automaticChanged(bool)), this, SLOT(setf2k()));
    setf2k();

    f3k = new DoublePlus( 0.0, "f3k", m_unitMeasure, UnitMeasure::forcePerLength, VarPlus::readWrite );
    f3k->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">3,k</span>"));
    f3k->setAutomatic( true );
    addVarToContainer( f3k );
    connect( f3k, SIGNAL(automaticChanged(bool)), this, SLOT(setf3k()));
    setf3k();

    connect( this, SIGNAL(timberChanged()), this, SLOT(setf1k()));
    connect( this, SIGNAL(timberChanged()), this, SLOT(setf2k()));

    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setf3k()));
    connect( this, SIGNAL(timberChanged()), this, SLOT(setf3k()));

    connect( f1k, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    connect( f2k, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    connect( f3k, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    connect( tTimber, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    connect( m_connector->dh, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    connect( m_connector->l, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    connect( m_connector->lEff, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));

    connect( m_connector->connectorType, SIGNAL(valueChanged(QString)), this, SLOT(setfkVisible()));
    setfkVisible();
}

double ExtrConnection::RkNormal(){
    double rk = 0.0;

    if( m_connector->connectorType->valueNormal() == TimberConnector::SmoothNail || m_connector->connectorType->valueNormal() == TimberConnector::OtherNail ){
        double tmp = f1k->valueNormal() * m_connector->d->valueNormal() * m_connector->l->valueNormal();
        if( tmp < rk ){
            rk = tmp;
        }
        if( m_connector->connectorType->valueNormal() == TimberConnector::SmoothNail ) {
            tmp = f1k->valueNormal() * m_connector->d->valueNormal() * tTimber->valueNormal() + f2k->valueNormal() * pow( m_connector->dh->valueNormal(), 2.0);
        } else {
            tmp = f2k->valueNormal() * pow( m_connector->dh->valueNormal(), 2.0);
        }
        if( tmp < rk ){
            rk = tmp;
        }
    } else if( m_connector->connectorType->valueNormal() == TimberConnector::Screw ){
        rk = f3k->valueNormal() * (m_connector->lEff->valueNormal() - m_connector->d->valueNormal() );
    }
    return rk;
}

void ExtrConnection::setf1k(){
    if( f1k->automatic() ){
        if( m_timber ){
            f1k->setValueNormal( 2.0e-5 * pow( (m_timber->gammaWk->valueNormal() * 1.0e-1), 2.0) * 1.0e+6);
        } else {
            f1k->setValueNormal( 0.0 );
        }
    }
}

void ExtrConnection::setf2k(){
    if( f2k->automatic() ){
        if( m_timber ){
            f2k->setValueNormal( 7.0e-5 * pow( (m_timber->gammaWk->valueNormal() * 1.0e-1), 2.0) * 1.0e+6);
        } else {
            f2k->setValueNormal( 0.0 );
        }
    }
}

void ExtrConnection::setf3k(){
    if( f3k->automatic() ){
        if( m_timber ){
            f3k->setValueNormal( (1.50 + 0.60 * m_connector->d->valueNormal() * 1.0e+3) * pow( m_timber->gammaWk->valueNormal() * 1.0e-1, 0.50  ) * 1.0e+3);
        } else {
            f3k->setValueNormal( 0.0 );
        }
    }
}

void ExtrConnection::setfkVisible(){
    if( m_connector->connectorType->valueNormal() != TimberConnector::SmoothNail || m_connector->connectorType->valueNormal() != TimberConnector::OtherNail ){
        f1k->setEnabled( true );
        f2k->setEnabled( true );
        f3k->setEnabled( false );
    } else if( m_connector->connectorType->valueNormal() != TimberConnector::Screw ){
        f1k->setEnabled( false );
        f2k->setEnabled( false );
        f3k->setEnabled( true );
    } else if( m_connector->connectorType->valueNormal() != TimberConnector::Bolt ){
        f1k->setEnabled( false );
        f2k->setEnabled( false );
        f3k->setEnabled( false );
    }
}
