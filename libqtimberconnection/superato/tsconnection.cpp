#include "tsconnection.h"

#include "timberconnector.h"
#include "timber.h"
#include "steel.h"
#include "doubleplus.h"
#include <cmath>

TSConnection::TSConnection( UnitMeasure * ump,
                            Timber * timb,
                            Steel * st,
                            TimberConnector * con,
                            ServiceClass * sc,
                            LoadDurationClass * ld,
                            LSType * ls,
                            QObject * parent ):
    TimberConnection( ump, timb, con, sc, ld, ls, parent ),
    m_steel( NULL ){
    *m_connectionName = "TSConnection";
    initVar();
    setSteel( st );
}

void TSConnection::initVar(){
    shearPlane = new TSShearPlanePlus( TSConnection::SingleShear, "shearPlane", VarPlus::readWrite );
    shearPlane->setRichName( trUtf8("Piani di taglio") );
    shearPlane->setToolTip( trUtf8("Piani di taglio della connessione") );
    shearPlane->setAutomatic( false );
    addVarToContainer( shearPlane );

    tSteel = new DoublePlus( 4.0e-3, "tSteel", m_unitMeasure, UnitMeasure::sectLength, VarPlus::readWrite );
    tSteel->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">S</span>") );
    tSteel->setToolTip( trUtf8("Spessore della piastra di acciaio"));
    tSteel->setAutomatic( false );
    addVarToContainer( tSteel );

    Rk1A = new DoublePlus( 0.0, "Rk1A", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk1A->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,IA</span>"));
    Rk1A->setAutomatic( true );
    addVarToContainer( Rk1A );
    connect( fhk, SIGNAL(valueChanged(QString)), this, SLOT(setRk1A()));
    connect( tTimber, SIGNAL(valueChanged(QString)), this, SLOT(setRk1A()));
    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setRk1A()));
    connect( Rk1A, SIGNAL(automaticChanged(bool)), this, SLOT(setRk1A()));
    setRk1A();

    Rk1B = new DoublePlus( 0.0, "Rk1B", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk1B->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,IB</span>"));
    Rk1B->setAutomatic( true );
    addVarToContainer( Rk1B );
    connect( fhk, SIGNAL(valueChanged(QString)), this, SLOT(setRk1B()));
    connect( tTimber, SIGNAL(valueChanged(QString)), this, SLOT(setRk1B()));
    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setRk1B()));
    connect( shearPlane, SIGNAL(valueChanged(QString)), this, SLOT(setRk1B()));
    connect( Rk1B, SIGNAL(automaticChanged(bool)), this, SLOT(setRk1B()));
    setRk1B();

    Rk1 = new DoublePlus( 0.0, "Rk1", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk1->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,I</span>"));
    Rk1->setAutomatic( true );
    addVarToContainer( Rk1 );
    connect( Rk1A, SIGNAL(valueChanged(QString)), this, SLOT(setRk1()));
    connect( Rk1B, SIGNAL(valueChanged(QString)), this, SLOT(setRk1()));
    connect( shearPlane, SIGNAL(valueChanged(QString)), this, SLOT(setRk1()));
    connect( Rk1, SIGNAL(automaticChanged(bool)), this, SLOT(setRk1()));
    setRk1();

    Rk2A = new DoublePlus( 0.0, "Rk2A", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk2A->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,IIA</span>"));
    Rk2A->setAutomatic( true );
    addVarToContainer( Rk2A );
    connect( fhk, SIGNAL(valueChanged(QString)), this, SLOT(setRk2A()));
    connect( tTimber, SIGNAL(valueChanged(QString)), this, SLOT(setRk2A()));
    connect( m_connector->d, SIGNAL(valueChanged(QString)), this, SLOT(setRk2A()));
    connect( m_connector->Myk, SIGNAL(valueChanged(QString)), this, SLOT(setRk2A()));
    connect( Rk2A, SIGNAL(automaticChanged(bool)), this, SLOT(setRk2A()));
    setRk2A();

    Rk2B = new DoublePlus( 0.0, "Rk2B", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk2B->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,IIB</span>"));
    Rk2B->setAutomatic( true );
    addVarToContainer( Rk2B );
    connect( fhk, SIGNAL(valueChanged(QString)), this, SLOT(setRk2B()));
    connect( tTimber, SIGNAL(valueChanged(QString)), this, SLOT(setRk2B()));
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
    connect( shearPlane, SIGNAL(valueChanged(QString)), this, SLOT(setRk2()));
    connect( Rk2, SIGNAL(automaticChanged(bool)), this, SLOT(setRk2()));
    setRk2();

    Rk3 = new DoublePlus( 0.0, "Rk3", m_unitMeasure, UnitMeasure::force, VarPlus::readWrite );
    Rk3->setRichName( trUtf8("R<span style=\" vertical-align:sub;\">k,III</span>"));
    Rk3->setAutomatic( true );
    addVarToContainer( Rk3 );
    connect( fhk, SIGNAL(valueChanged(QString)), this, SLOT(setRk3()));
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

Steel * TSConnection::steel(){ return m_steel; }

void TSConnection::setSteel( Steel * st ){
    if( st != m_steel ){
        m_steel = st;
        emit steelChanged();
        if( m_steel ){
            connect( m_steel, SIGNAL(destroyed()), this, SLOT(setSteelNULL()));
        }
    }
}

void TSConnection::setSteelNULL(){
    setSteel( NULL );
}

double TSConnection::RkNormal(){
    double rk = Rk1->valueNormal();

    if( Rk2->valueNormal() < rk ){
        rk = Rk2->valueNormal();
    }
    if( shearPlane->valueNormal() != TSConnection::SingleShear ){
        if( Rk3->valueNormal() < rk ){
            rk = Rk3->valueNormal();
        }
    }

    return rk;
}

void TSConnection::setRk1A(){
    if( Rk1A->automatic() ){
        Rk1A->setValueNormal( 0.40 * fhk->valueNormal() * tTimber->valueNormal() * m_connector->d->valueNormal() );
    }
}

void TSConnection::setRk1B(){
    if( Rk1B->automatic() ){
        if( shearPlane->valueNormal() == TSConnection::DoubleShearOuterSteel ){
            Rk1B->setValueNormal( 0.50 * fhk->valueNormal() * tTimber->valueNormal() * m_connector->d->valueNormal() );
        } else {
            Rk1B->setValueNormal( fhk->valueNormal() * tTimber->valueNormal() * m_connector->d->valueNormal() );
        }
    }
}

void TSConnection::setRk1(){
    if( Rk1->automatic() ){
        if( shearPlane->valueNormal() == TSConnection::SingleShear ){
            if ( fabs( tSteel->valueNormal() ) < 0.50 * fabs( m_connector->d->valueNormal() ) ){
                Rk1->setValueNormal( Rk1A->valueNormal() );
            } else if ( fabs( tSteel->valueNormal() ) >= fabs( m_connector->d->valueNormal() ) ){
                Rk1->setValueNormal( Rk1B->valueNormal() );
            } else {
                if( m_connector->d->valueNormal() != 0.0 ){
                    Rk1->setValueNormal( Rk1A->valueNormal() + (Rk1B->valueNormal() - Rk1A->valueNormal()) / (0.50 * fabs(m_connector->d->valueNormal() )) * (fabs( tSteel->valueNormal() ) - 0.50 * fabs( m_connector->d->valueNormal() )));
                } else {
                    Rk1->setValueNormal( 0.0 );
                }
            }
        } else {
            Rk1->setValueNormal( Rk1B->valueNormal() );
        }
    }
}

void TSConnection::setRk2A(){
    if( Rk2A->automatic() ){
        Rk2A->setValueNormal( (1.15 * pow( 2.0 * m_connector->Myk->valueNormal() * fhk->valueNormal() * m_connector->d->valueNormal(),0.50 ) ));
    }
}

void TSConnection::setRk2B(){
    if( Rk2B->automatic() ){
        double den = fhk->valueNormal() * m_connector->d->valueNormal() * pow( tTimber->valueNormal(), 2.0 );
        if( den != 0.0 ){
            Rk2B->setValueNormal( fhk->valueNormal() * m_connector->d->valueNormal() * tTimber->valueNormal() * \
                                  (pow( 2.0 + 4.0 * m_connector->Myk->valueNormal() / den,0.50) - 1.0) );
        } else {
            Rk2B->setValueNormal( 0.0 );
        }
    }
}

void TSConnection::setRk2(){
    if( Rk2->automatic() ){
        if( shearPlane->valueNormal() == TSConnection::SingleShear ){
            if ( fabs( tSteel->valueNormal() ) < 0.50 * fabs( m_connector->d->valueNormal() ) ){
                Rk2->setValueNormal( Rk2A->valueNormal() );
            } else if ( fabs( tSteel->valueNormal() ) >= fabs( m_connector->d->valueNormal() ) ){
                Rk2->setValueNormal( Rk2B->valueNormal() );
            } else {
                if( m_connector->d->valueNormal() != 0.0 ){
                    Rk2->setValueNormal( Rk2A->valueNormal() + (Rk2B->valueNormal() - Rk2A->valueNormal()) / (0.50 * fabs(m_connector->d->valueNormal() )) * (fabs( tSteel->valueNormal() ) - 0.50 * fabs( m_connector->d->valueNormal() )));
                } else {
                    Rk2->setValueNormal( 0.0 );
                }
            }
        } else if( shearPlane->valueNormal() == TSConnection::DoubleShearCentralSteel ){
            Rk2->setValueNormal( Rk2B->valueNormal() );
        } else if( shearPlane->valueNormal() == TSConnection::DoubleShearOuterSteel ){
            Rk2->setValueNormal( Rk2A->valueNormal() );
        }
    }
}

void TSConnection::setRk3(){
    if( Rk3->automatic() ){
        Rk3->setValueNormal( (2.30 * pow( m_connector->Myk->valueNormal() * fhk->valueNormal() * m_connector->d->valueNormal(), 0.50) ));
    }
}

void TSConnection::setRkVisible(){
    if( shearPlane->valueNormal() != TSConnection::SingleShear ){
        Rk1A->setEnabled( true );
        Rk1B->setEnabled( true );
        Rk2A->setEnabled( true );
        Rk2B->setEnabled( true );
    } else if( shearPlane->valueNormal() != TSConnection::DoubleShearCentralSteel ){
        Rk1A->setEnabled( false );
        Rk1B->setEnabled( true );
        Rk2A->setEnabled( false );
        Rk2B->setEnabled( true );
    } else if( shearPlane->valueNormal() != TSConnection::DoubleShearOuterSteel ){
        Rk1A->setEnabled( false );
        Rk1B->setEnabled( true );
        Rk2A->setEnabled( true );
        Rk2B->setEnabled( false );
    }
}

#include <QComboBox>

TSShearPlanePlus::TSShearPlanePlus( TSConnection::ShearPlane tt,
                                    const QString & nn,
                                    VarPlus::EditMode em):
    VarPlusComboBox( nn, em),
    m_value(tt),
    m_shearPlaneContainer(new QList<TSConnection::ShearPlane> ){
    initDesc();
}

TSShearPlanePlus::~TSShearPlanePlus(){
    delete m_shearPlaneContainer;
}

void TSShearPlanePlus::initDesc(){
    m_shearPlaneContainer->append( TSConnection::SingleShear );
    m_descTypeContainer->append( new QString( trUtf8("Singolo") ) );

    m_shearPlaneContainer->append( TSConnection::DoubleShearCentralSteel );
    m_descTypeContainer->append( new QString( trUtf8("Doppio - Piastra interna") ) );

    m_shearPlaneContainer->append( TSConnection::DoubleShearOuterSteel );
    m_descTypeContainer->append( new QString( trUtf8("Doppio - Piastre esterne") ) );
}

int TSShearPlanePlus::currentIndex(){
    return (int) m_value;
}

QString TSShearPlanePlus::typeVar(){
    return QString("TTShearSide");
}

TSShearPlanePlus & TSShearPlanePlus::operator=( TSShearPlanePlus & val ){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_value );
    }
    return *this;
}

VarPlus & TSShearPlanePlus::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        TSShearPlanePlus * valTSShearPlanePlus = dynamic_cast<TSShearPlanePlus *>(&val);
        if(valTSShearPlanePlus)
        {
            setValueNormal( valTSShearPlanePlus->m_value );
        }
    }
    return *this;
}

TSConnection::ShearPlane TSShearPlanePlus::value(){
    return valueNormal();
}

TSConnection::ShearPlane TSShearPlanePlus::valueNormal(){
    return m_value;
}

QString TSShearPlanePlus::valueNormalStr(){
    return *(m_descTypeContainer->at( (int) m_value ));
}

void TSShearPlanePlus::setValue( TSConnection::ShearPlane val ){
    setValueNormal( val );
}

void TSShearPlanePlus::setValue( int val ){
    setValueNormal( val );
}

void TSShearPlanePlus::setValueNormal( TSConnection::ShearPlane val ){
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

void TSShearPlanePlus::setValueNormal( int i ){
    if( i >= 0 && i < m_shearPlaneContainer->size() ){
        setValueNormal( m_shearPlaneContainer->at(i) );
    }
}

void TSShearPlanePlus::setValueNormal( const QString & val ){
    for( int i=0; i < m_descTypeContainer->size(); i++ ){
        if( val.toUpper() == m_descTypeContainer->at(i)->toUpper() ){
            setValueNormal( i );
            return;
        }
    }
}
