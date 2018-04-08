#include "ttconnection.h"

#include "timberconnector.h"
#include "timber.h"
#include "doubleplus.h"
#include <cmath>

TTConnection::TTConnection( UnitMeasure * ump,
                            Timber * timb1,
                            Timber * timb2,
                            TimberConnector * con,
                            ServiceClass * sc,
                            LoadDurationClass * ld,
                            LSType * ls,
                            QObject * parent ):
    TimberConnection( ump, timb1, con, sc, ld, ls, parent ),
    m_timber2( NULL ){
    *m_connectionName = "TTConnection";
    initVar();
    setTimber2( timb2 );
}

void TTConnection::initVar(){
    shearPlane = new TTShearPlanePlus( TTConnection::SingleShear, "shearPlane", VarPlus::readWrite );
    shearPlane->setRichName( trUtf8("Piani di taglio") );
    shearPlane->setToolTip( trUtf8("Piani di taglio della connessione") );
    shearPlane->setAutomatic( false );
    addVarToContainer( shearPlane );

    fh1k = fhk;
    fh1k->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">h,1,k</span>") );

    tTimber1 = tTimber;
    tTimber1->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">T,1</span>") );

    tTimber2 = new DoublePlus( 5.0e-2, "tTimber2", m_unitMeasure, UnitMeasure::sectLength, VarPlus::readWrite );
    tTimber2->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">T,2</span>") );
    tTimber2->setToolTip( trUtf8("Spessore legno 2"));
    tTimber2->setAutomatic( false );
    addVarToContainer( tTimber2 );

    fh2k = new DoublePlus( 0.0, "fh2k", m_unitMeasure, UnitMeasure::tension, VarPlus::readWrite );
    fh2k->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">h,2,k</span>") );
    fh2k->setToolTip( trUtf8("Valore caratteristico di rifollamento nel caso di chiodi inseriti con preforatura"));
    fh2k->setAutomatic( true );
    addVarToContainer( fh2k );
    connect( this, SIGNAL(timber2Changed()), this, SLOT(setFh2k()));
    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setFh2k()));
    setFh2k();

    /** Rapporto tra fh2k e fh1k */
    beta = new DoublePlus( 0.0, "beta", m_unitMeasure, UnitMeasure::noDimension, VarPlus::readWrite );
    beta->setRichName( trUtf8("β") );
    beta->setToolTip( trUtf8("Rapporto tra le tensioni caratteristiche di rifollamento (f<span style=\" vertical-align:sub;\">h,2,k</span>/f<span style=\" vertical-align:sub;\">h,1,k</span>)"));
    beta->setAutomatic( true );
    addVarToContainer( beta );
    connect( fh1k, SIGNAL(valueChanged(QString)), this, SLOT(setBeta()));
    connect( fh2k, SIGNAL(valueChanged(QString)), this, SLOT(setBeta()));

    Rk1A = new DoublePlus( 0.0, "Rk1A", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk1A->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,IA</span>"));
    Rk1A->setAutomatic( true );
    addVarToContainer( Rk1A );
    connect( fh1k, SIGNAL(valueChanged(QString)), this, SLOT(setRk1A()));
    connect( tTimber1, SIGNAL(valueChanged(QString)), this, SLOT(setRk1A()));
    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setRk1A()));
    connect( Rk1A, SIGNAL(automaticChanged(bool)), this, SLOT(setRk1A()));
    setRk1A();

    Rk1B = new DoublePlus( 0.0, "Rk1B", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk1B->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,IB</span>"));
    Rk1B->setAutomatic( true );
    addVarToContainer( Rk1B );
    connect( fh2k, SIGNAL(valueChanged(QString)), this, SLOT(setRk1B()));
    connect( tTimber2, SIGNAL(valueChanged(QString)), this, SLOT(setRk1B()));
    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setRk1B()));
    connect( shearPlane, SIGNAL(valueChanged(QString)), this, SLOT(setRk1B()));
    connect( Rk1B, SIGNAL(automaticChanged(bool)), this, SLOT(setRk1B()));
    setRk1B();

    Rk1C = new DoublePlus( 0.0, "Rk1C", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk1C->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,IC</span>"));
    Rk1C->setAutomatic( true );
    addVarToContainer( Rk1C );
    connect( fh1k, SIGNAL(valueChanged(QString)), this, SLOT(setRk1C()));
    connect( beta, SIGNAL(valueChanged(QString)), this, SLOT(setRk1C()));
    connect( tTimber1, SIGNAL(valueChanged(QString)), this, SLOT(setRk1C()));
    connect( tTimber2, SIGNAL(valueChanged(QString)), this, SLOT(setRk1C()));
    connect( Rk1C, SIGNAL(automaticChanged(bool)), this, SLOT(setRk1C()));
    setRk1C();

    Rk1 = new DoublePlus( 0.0, "Rk1", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk1->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,I</span>"));
    Rk1->setAutomatic( true );
    addVarToContainer( Rk1 );
    connect( Rk1A, SIGNAL(valueChanged(QString)), this, SLOT(setRk1()));
    connect( Rk1B, SIGNAL(valueChanged(QString)), this, SLOT(setRk1()));
    connect( Rk1C, SIGNAL(valueChanged(QString)), this, SLOT(setRk1()));
    connect( Rk1, SIGNAL(automaticChanged(bool)), this, SLOT(setRk1()));
    setRk1();

    Rk2A = new DoublePlus( 0.0, "Rk2A", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk2A->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,IIA</span>"));
    Rk2A->setAutomatic( true );
    addVarToContainer( Rk2A );
    connect( fh1k, SIGNAL(valueChanged(QString)), this, SLOT(setRk2A()));
    connect( beta, SIGNAL(valueChanged(QString)), this, SLOT(setRk2A()));
    connect( tTimber1, SIGNAL(valueChanged(QString)), this, SLOT(setRk2A()));
    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setRk2A()));
    connect( m_connector->Myk, SIGNAL(valueChanged(QString)), this, SLOT(setRk2A()));
    connect( Rk2A, SIGNAL(automaticChanged(bool)), this, SLOT(setRk2A()));
    setRk2A();

    Rk2B = new DoublePlus( 0.0, "Rk2B", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk2B->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,IIB</span>"));
    Rk2B->setAutomatic( true );
    addVarToContainer( Rk2B );
    connect( fh1k, SIGNAL(valueChanged(QString)), this, SLOT(setRk2B()));
    connect( beta, SIGNAL(valueChanged(QString)), this, SLOT(setRk2B()));
    connect( tTimber2, SIGNAL(valueChanged(QString)), this, SLOT(setRk2B()));
    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setRk2B()));
    connect( m_connector->Myk, SIGNAL(valueChanged(QString)), this, SLOT(setRk2B()));
    connect( Rk2B, SIGNAL(automaticChanged(bool)), this, SLOT(setRk2B()));
    setRk2B();

    Rk2 = new DoublePlus( 0.0, "Rk2", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk2->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,II</span>"));
    Rk2->setAutomatic( true );
    addVarToContainer( Rk2 );
    connect( Rk2A, SIGNAL(valueChanged(QString)), this, SLOT(setRk2()));
    connect( Rk2B, SIGNAL(valueChanged(QString)), this, SLOT(setRk2()));
    connect( Rk2, SIGNAL(automaticChanged(bool)), this, SLOT(setRk2()));
    setRk1();

    Rk3 = new DoublePlus( 0.0, "Rk3", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk3->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,III</span>"));
    Rk3->setAutomatic( true );
    addVarToContainer( Rk3 );
    connect( fh1k, SIGNAL(valueChanged(QString)), this, SLOT(setRk3()));
    connect( beta, SIGNAL(valueChanged(QString)), this, SLOT(setRk3()));
    connect( m_connector->Myk, SIGNAL(valueChanged(QString)), this, SLOT(setRk3()));
    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setRk3()));
    connect( Rk3, SIGNAL(automaticChanged(bool)), this, SLOT(setRk3()));
    setRk3();

    connect( Rk1, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    connect( Rk2, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    connect( Rk3, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    connect( shearPlane, SIGNAL(valueChanged(QString)), this, SLOT(setRk()));
    setRk();

    connect( shearPlane, SIGNAL(valueChanged(QString)), this, SLOT(setRkVisible()));
}

Timber * TTConnection::timber1(){ return timber(); }

void TTConnection::setTimber1( Timber * timb ){ setTimber( timb ); }

Timber * TTConnection::timber2(){ return m_timber2; }

void TTConnection::setTimber2( Timber * timb ){
    if( timb != m_timber2 ){
        m_timber2 = timb;
        emit timber2Changed();
        if( m_timber2 ){
            connect( m_timber2, SIGNAL(destroyed()), this, SLOT(setTimber2NULL()));
            connect( m_timber2->gammaWk, SIGNAL(valueChanged(QString)), this, SLOT(setFh2k()));
            setFh2k();
            connect( m_timber2, SIGNAL(materialChanged()), this, SLOT(setKmod()));
            setKmod();
        }
    }
}

void TTConnection::setTimber2NULL(){
    setTimber2( NULL );
}

void TTConnection::setFh2k(){
    if( fh2k->automatic() ){
        if( m_timber2 ){
            fh2k->setValueNormal( 1.0e+6 * 8.2e-3 * (1.0 - 10.0 * m_connector->d->valueNormal()) * m_timber2->gammaWk->valueNormal() );
        }
    }
}

void TTConnection::setBeta(){
    if( beta->automatic() ){
        if( fh1k->valueNormal() != 0.0 ){
            beta->setValueNormal( fh2k->valueNormal() / fh1k->valueNormal() );
        } else {
            beta->setValueNormal( 0.0 );
        }
    }
}

void TTConnection::setKmod(){
    if( kmod->automatic() ){
        if( m_timber != NULL && m_timber2 != NULL ){
            double k1 = m_timber->kmodNormal( m_sClass->valueNormal(), m_ldClass->valueNormal());
            double k2 = m_timber2->kmodNormal( m_sClass->valueNormal(), m_ldClass->valueNormal());
            kmod->setValueNormal( (k1<k2? k1: k2) );
        }
    }
}

double TTConnection::RkNormal(){
    double rk = Rk1->valueNormal();

    if( Rk2->valueNormal() < rk ){
        rk = Rk2->valueNormal();
    }
    if( Rk3->valueNormal() < rk ){
        rk = Rk3->valueNormal();
    }

    return rk;
}

void TTConnection::setRk1A(){
    if( Rk1A->automatic() ){
        Rk1A->setValueNormal( fh1k->valueNormal() * tTimber1->valueNormal() * m_connector->d->valueNormal() );
    }
}

void TTConnection::setRk1B(){
    if( Rk1B->automatic() ){
        if( shearPlane->valueNormal() == TTConnection::SingleShear ){
            Rk1B->setValueNormal( fh2k->valueNormal() * tTimber2->valueNormal() * m_connector->d->valueNormal() );
        } else {
            Rk1B->setValueNormal( 0.50 * fh2k->valueNormal() * tTimber2->valueNormal() * m_connector->d->valueNormal() );
        }
    }
}

void TTConnection::setRk1C(){
    if( Rk1C->automatic() ){
        double b = beta->valueNormal();
        if( (1.0 + b) != 0.0 && tTimber1->valueNormal() != 0.0 ){
            double r = tTimber2->valueNormal() / tTimber1->valueNormal();
            Rk1C->setValueNormal( fh1k->valueNormal() * m_connector->d->valueNormal() * tTimber1->valueNormal() / (1.0 + beta->valueNormal()) * \
                                  ( pow(b*(1.0 + 2.0 * b * (1.0 + r+ r*r) + b*b * r*r), 0.50) - b * (1.0 + r) ) );
        } else {
            Rk1C->setValueNormal( 0.0 );
        }
    }
}

void TTConnection::setRk1(){
    if( Rk1->automatic() ){
        double rk = Rk1A->valueNormal();
        if( rk > Rk1B->valueNormal() ){
            rk = Rk1B->valueNormal();
        }
        if( rk > Rk1C->valueNormal() && shearPlane->valueNormal() == TTConnection::SingleShear ){
            rk = Rk1C->valueNormal();
        }
        Rk1->setValueNormal( rk );
    }
}

void TTConnection::setRk2A(){
    if( Rk2A->automatic() ){
        double b = beta->valueNormal();
        double den = fh1k->valueNormal() * m_connector->d->valueNormal() * pow( tTimber1->valueNormal(), 2.0 );
        if( (2.0+b)!= 0.0 &&  den != 0.0 ){
            Rk2A->setValueNormal( 1.05 * fh1k->valueNormal() * m_connector->d->valueNormal() * tTimber1->valueNormal() / (2.0 + b) * \
                                  ( pow( (2.0*b*(1.0+b) + 4.0*b*(2.0+b)*m_connector->Myk->valueNormal() / den),0.50) - b) );
        } else {
            Rk2A->setValueNormal( 0.0 );
        }
    }
}

void TTConnection::setRk2B(){
    if( Rk2B->automatic() ){
        double b = beta->valueNormal();
        double den = fh1k->valueNormal() * m_connector->d->valueNormal() * pow( tTimber2->valueNormal(), 2.0 );
        if( (1.0+2.0*b)!= 0.0 &&  den != 0.0 ){
            Rk2B->setValueNormal( 1.05 * fh1k->valueNormal() * m_connector->d->valueNormal() * tTimber2->valueNormal() / (1.0 + 2.0*b) * \
                                  ( pow( (2.0*b*b*(1.0+b) + 4.0*b*(1.0+2.0*b)*m_connector->Myk->valueNormal() / den),0.50) - b) );
        } else {
            Rk2B->setValueNormal( 0.0 );
        }
    }
}

void TTConnection::setRk2(){
    if( Rk2->automatic() ){
        double rk = Rk2A->valueNormal();
        if( rk > Rk2B->valueNormal() && shearPlane->valueNormal() == TTConnection::SingleShear ){
            rk = Rk2B->valueNormal();
        }
        Rk2->setValueNormal( rk );
    }
}

void TTConnection::setRk3(){
    if( Rk3->automatic() ){
        double b = beta->valueNormal();
        if( (1.0+b)!= 0.0 ){
            Rk3->setValueNormal( 1.15 * pow( (4.0*b*m_connector->Myk->valueNormal() * fh1k->valueNormal() * m_connector->d->valueNormal() ) / (1.0+b),0.50 ));
        } else {
            Rk3->setValueNormal( 0.0 );
        }
    }
}

void TTConnection::setRkVisible(){
    if( shearPlane->valueNormal() == TTConnection::DoubleShear ){
        Rk1C ->setEnabled( false );
        Rk2B ->setEnabled( false );
    } else if( shearPlane->valueNormal() == TTConnection::SingleShear ){
        Rk1C ->setEnabled( true );
        Rk2B ->setEnabled( true );
    }

}

#include <QComboBox>

TTShearPlanePlus::TTShearPlanePlus( TTConnection::ShearPlane tt,
                                    const QString & nn,
                                    VarPlus::EditMode em):
    VarPlusComboBox( nn, em),
    m_value(tt),
    m_shearPlaneContainer(new QList<TTConnection::ShearPlane> ){
    initDesc();
}

TTShearPlanePlus::~TTShearPlanePlus(){
    delete m_shearPlaneContainer;
}

void TTShearPlanePlus::initDesc(){
    m_shearPlaneContainer->append( TTConnection::SingleShear );
    m_descTypeContainer->append( new QString( trUtf8("Singolo") ) );

    m_shearPlaneContainer->append( TTConnection::DoubleShear );
    m_descTypeContainer->append( new QString( trUtf8("Doppio") ) );
}

int TTShearPlanePlus::currentIndex(){
    return (int) m_value;
}

QString TTShearPlanePlus::typeVar(){
    return QString("TTShearSide");
}

TTShearPlanePlus & TTShearPlanePlus::operator=( TTShearPlanePlus & val ){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_value );
    }
    return *this;
}

VarPlus & TTShearPlanePlus::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        TTShearPlanePlus * valTTShearPlanePlus = dynamic_cast<TTShearPlanePlus *>(&val);
        if(valTTShearPlanePlus)
        {
            setValueNormal( valTTShearPlanePlus->m_value );
        }
    }
    return *this;
}

TTConnection::ShearPlane TTShearPlanePlus::value(){
    return valueNormal();
}

TTConnection::ShearPlane TTShearPlanePlus::valueNormal(){
    return m_value;
}

QString TTShearPlanePlus::valueNormalStr(){
    return *(m_descTypeContainer->at( (int) m_value ));
}

void TTShearPlanePlus::setValue( TTConnection::ShearPlane val ){
    setValueNormal( val );
}

void TTShearPlanePlus::setValue( int val ){
    setValueNormal( val );
}

void TTShearPlanePlus::setValueNormal( TTConnection::ShearPlane val ){
    if( val != m_value ){
        m_value = val;
        emit valueChanged( valueStr() );
        QList<QComboBox *>::iterator iter = m_comboBoxContainer->begin();
        while( iter != m_comboBoxContainer->end()){
            (*iter)->setCurrentIndex( (int) m_value );
            iter++;
        }
    }
}

void TTShearPlanePlus::setValueNormal( int i ){
    if( i >= 0 && i < m_shearPlaneContainer->size() ){
        setValueNormal( m_shearPlaneContainer->at(i) );
    }
}

void TTShearPlanePlus::setValueNormal( const QString & val ){
    for( int i=0; i < m_descTypeContainer->size(); i++ ){
        if( val.toUpper() == m_descTypeContainer->at(i)->toUpper() ){
            setValueNormal( i );
            return;
        }
    }
}
