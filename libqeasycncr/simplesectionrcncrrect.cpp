#include "simplesectionrcncrrect.h"

#include "tablemodelplusprivate.h"
#include "simplesectionrcncrcomp.h"
#include "steelamodel.h"
#include "slssteelsigmamodel.h"
#include "steelfepsmodel.h"
#include "doubleplus.h"
#include "uintplus.h"
#include "unitmeasure.h"
#include "structmath.h"

#include <QVector>
#include <QPointF>

#include <cmath>

#include <QDebug>

class SimpleSectionRCncrRectPrivate{
public:
    SimpleSectionRCncrRectPrivate( UnitMeasure * ump ):
        steelAModel( new SteelAModel( ump )){

    }
    SteelAModel * steelAModel;
};

SimpleSectionRCncrRect::SimpleSectionRCncrRect(UnitMeasure *ump, const QString &n, QObject *parent) :
    VarPlusContainerPlus(ump, "SimpleSectionRCncrRect", n, parent ),
    m_d( new SimpleSectionRCncrRectPrivate(ump)){
    connect( m_d->steelAModel, SIGNAL(modelChanged()), this, SIGNAL(sectionChanged()) );
    initVar();
}

SteelAModel *SimpleSectionRCncrRect::steelAModel() {
    return m_d->steelAModel;
}

void SimpleSectionRCncrRect::initVar() {
    connect( this, &SimpleSectionRCncrRect::containerChanged, this, &SimpleSectionRCncrRect::sectionChanged );

    b = new DoublePlus( 0.300, "b", m_unitMeasure, UnitMeasure::sectL );
    b->setRichName(trUtf8("b"));
    b->setToolTip( trUtf8("Base della sezione rettangolare"));
    addVarToContainer(b);

    h = new DoublePlus( 0.400, "h", m_unitMeasure, UnitMeasure::sectL );
    h->setRichName(trUtf8("h"));
    h->setToolTip( trUtf8("Altezza della sezione rettangolare"));
    addVarToContainer(h);

    dMax = new DoublePlus( 0.0, "dMax", m_unitMeasure, UnitMeasure::sectL, true );
    dMax->setRichName(trUtf8("d<span style=\" vertical-align:sub;\">max</span>"));
    dMax->setToolTip( trUtf8("Altezza utile sezione rettangolare"));
    dMax->setReadOnly( true );
    connect( m_d->steelAModel, SIGNAL(dMaxChanged(double)), dMax, SLOT(setValueNormal(double)) );
    addVarToContainer(dMax);

    dMin = new DoublePlus( 0.0, "dMin", m_unitMeasure, UnitMeasure::sectL, true );
    dMin->setRichName(trUtf8("d<span style=\" vertical-align:sub;\">min</span>"));
    dMin->setToolTip( trUtf8("Altezza utile minima"));
    dMin->setReadOnly( true );
    connect( m_d->steelAModel, SIGNAL(dMinChanged(double)), dMin, SLOT(setValueNormal(double)) );
    addVarToContainer(dMin);

    fck = new DoublePlus( 25.0e+6, "fck", m_unitMeasure, UnitMeasure::tension );
    fck->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">ck</span>"));
    fck->setToolTip( trUtf8("Resistenza caratteristica su provino cilindrico del cls"));
    addVarToContainer(fck);

    alphaCC = new DoublePlus( 0.85, "alphaCC", m_unitMeasure, UnitMeasure::noDimension );
    alphaCC->setRichName(trUtf8("α<span style=\" vertical-align:sub;\">cc</span>"));
    alphaCC->setToolTip( trUtf8("Coefficiente effetti a lungo termine e modalità  di applicazione del carico [0.8 - 1.0]"));
    addVarToContainer( alphaCC );

    gammaC = new DoublePlus( 1.5, "gammaC", m_unitMeasure, UnitMeasure::noDimension );
    gammaC->setRichName(trUtf8("γ<span style=\" vertical-align:sub;\">c</span>"));
    gammaC->setToolTip(trUtf8("Coefficiente di sicurezza parziale calcestruzzo"));
    addVarToContainer( gammaC );

    fcd = new DoublePlus( 20.0e+6, "fcd", m_unitMeasure, UnitMeasure::tension, true );
    fcd->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">cd</span>"));
    fcd->setToolTip( trUtf8("Resistenza di progetto del cls"));
    fcd->setReadOnly( true );
    addVarToContainer(fcd);
    connect( alphaCC, SIGNAL(valueChanged(QString)), this, SLOT(setFcd()));
    connect( fck, SIGNAL(valueChanged(QString)), this, SLOT(setFcd()));
    connect( gammaC, SIGNAL(valueChanged(QString)), this, SLOT(setFcd()));
    connect( fcd, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFcd()));
    setFcd();

    epsC2 = new DoublePlus(0.0, "epsC2", m_unitMeasure, UnitMeasure::deformation );
    epsC2->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">c2</span>") );
    epsC2->setReadOnly( true );
    addVarToContainer( epsC2 );
    connect( fck, SIGNAL(valueChanged(QString)), this, SLOT(setEpsC2()) );
    connect( epsC2, SIGNAL(readOnlyChanged(bool)), this, SLOT(setEpsC2()) );
    setEpsC2();

    epsCU2 = new DoublePlus(0.0, "epsCU2", m_unitMeasure, UnitMeasure::deformation );
    epsCU2->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">cu2</span>") );
    epsCU2->setReadOnly( true );
    epsCU2->setToolTip(trUtf8("Deformazione ultima di contrazione - diagramma parabola-rettangolo"));
    addVarToContainer( epsCU2 );
    connect( fck, SIGNAL(valueChanged(QString)), this, SLOT(setEpsCU2()) );
    connect( epsCU2, SIGNAL(readOnlyChanged(bool)), this, SLOT(setEpsCU2()) );
    setEpsCU2();

    nC = new DoublePlus( 2.0, "nC", m_unitMeasure, UnitMeasure::noDimension );
    nC->setRichName(trUtf8("n<span style=\" vertical-align:sub;\">c</span>"));
    nC->setToolTip(trUtf8("Esponente delle deformazioni nella relazione deformazione/tensione del cls a SLU"));
    nC->setReadOnly( true );
    addVarToContainer( nC );
    connect( fck, SIGNAL(valueChanged(QString)), this, SLOT(setNC()) );
    connect( nC, SIGNAL(readOnlyChanged(bool)), this, SLOT(setNC()) );
    setNC();

    Es = new DoublePlus( 2.0e+11, "Es", m_unitMeasure, UnitMeasure::tension );
    Es->setRichName(trUtf8("E<span style=\" vertical-align:sub;\">s</span>"));
    Es->setToolTip( trUtf8("Modulo di elasticità normale dell'acciaio"));
    addVarToContainer(Es);

    epsSUk = new DoublePlus(0.075, "epsSUk", m_unitMeasure, UnitMeasure::deformation );
    epsSUk->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">s,uk</span>") );
    epsSUk->setToolTip(trUtf8("Deformazione caratteristica di rottura dell'acciaio"));
    addVarToContainer( epsSUk );

    epsSUd = new DoublePlus(0.0, "epsSUd", m_unitMeasure, UnitMeasure::deformation );
    epsSUd->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">s,ud</span>") );
    epsSUd->setReadOnly( true );
    epsSUd->setToolTip(trUtf8("Deformazione ultima di progetto dell'acciaio"));
    addVarToContainer( epsSUd );
    connect( epsSUk, SIGNAL(valueChanged(QString)), this, SLOT(setEpsSUd()) );
    connect( epsSUd, SIGNAL(readOnlyChanged(bool)), this, SLOT(setEpsSUd()) );
    setEpsSUd();

    fSyk = new DoublePlus( 450.0e+6, "fSyk", m_unitMeasure, UnitMeasure::tension );
    fSyk->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">s,yk</span>"));
    fSyk->setToolTip( trUtf8("Tensione caratteristica di snervamento dell'acciaio"));
    addVarToContainer(fSyk);

    gammaS = new DoublePlus( 1.15, "gammaS", m_unitMeasure, UnitMeasure::noDimension );
    gammaS->setRichName(trUtf8("γ<span style=\" vertical-align:sub;\">s</span>"));
    gammaS->setToolTip(trUtf8("Coefficiente di sicurezza parziale acciaio"));
    addVarToContainer( gammaS );

    fSyd = new DoublePlus( 0.0, "fSyd", m_unitMeasure, UnitMeasure::tension, true );
    fSyd->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">s,yd</span>"));
    fSyd->setToolTip( trUtf8("Tensione di progetto dell'acciaio"));
    fSyd->setReadOnly( true );
    addVarToContainer(fSyd);
    connect( fSyk, SIGNAL(valueChanged(QString)), this, SLOT(setFSyd()));
    connect( gammaS, SIGNAL(valueChanged(QString)), this, SLOT(setFSyd()));
    connect( fSyd, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFSyd()));
    setFSyd();

    kS = new DoublePlus( 1.15, "kS", m_unitMeasure, UnitMeasure::noDimension );
    kS->setRichName(trUtf8("(f<span style=\" vertical-align:sub;\">t</span>/f<span style=\" vertical-align:sub;\">y</span>)<span style=\" vertical-align:sub;\">k</span>"));
    kS->setToolTip(trUtf8("Rapporto caratteristico tra tensione di snervamento e tensione di rottura"));
    addVarToContainer( kS );

    fSud = new DoublePlus( 0.0, "fSud", m_unitMeasure, UnitMeasure::tension, true );
    fSud->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">s,ud</span>"));
    fSud->setToolTip( trUtf8("Tensione ultima di progetto dell'acciaio"));
    fSud->setReadOnly( true );
    addVarToContainer(fSud);
    connect( fSyd, SIGNAL(valueChanged(QString)), this, SLOT(setFSud()));
    connect( kS, SIGNAL(valueChanged(QString)), this, SLOT(setFSud()));
    connect( Es, SIGNAL(valueChanged(QString)), this, SLOT(setFSud()));
    connect( epsSUd, SIGNAL(valueChanged(QString)), this, SLOT(setFSud()));
    connect( epsSUk, SIGNAL(valueChanged(QString)), this, SLOT(setFSud()));
    connect( fSud, SIGNAL(readOnlyChanged(bool)), this, SLOT(setFSud()));
    setFSud();

    steelFEpsModel = new SteelFEpsModel( SimpleSectionRCncrComp::inclinedTopBranch, "steelFEpsModel", false );
    steelFEpsModel->setRichName( trUtf8("Legge σ-ε"));
    steelFEpsModel->setToolTip( trUtf8("Legge costitutiva dell'acciaio a SLU"));
    addVarToContainer( steelFEpsModel );

    NRdMin = new DoublePlus( 0.0, "NRdMin", m_unitMeasure, UnitMeasure::loadF );
    NRdMin->setRichName(trUtf8("N<span style=\" vertical-align:sub;\">Rd,min</span>"));
    NRdMin->setToolTip(trUtf8("Sforzo normale resistente minimo"));
    NRdMin->setReadOnly( true );
    addVarToContainer( NRdMin );
    connect( b, SIGNAL(valueChanged(QString)), this, SLOT(setNRdMin()) );
    connect( h, SIGNAL(valueChanged(QString)), this, SLOT(setNRdMin()) );
    connect( epsCU2, SIGNAL(valueChanged(QString)), this, SLOT(setNRdMin()) );
    connect( fcd, SIGNAL(valueChanged(QString)), this, SLOT(setNRdMin()) );
    connect( fSyd, SIGNAL(valueChanged(QString)), this, SLOT(setNRdMin()) );
    connect( fSud, SIGNAL(valueChanged(QString)), this, SLOT(setNRdMin()) );
    connect( steelFEpsModel, SIGNAL(valueChanged(QString)), this, SLOT(setNRdMin()) );
    connect( m_d->steelAModel, SIGNAL(modelChanged()), this, SLOT(setNRdMin()) );
    setNRdMin();

    NRdMax = new DoublePlus( 0.0, "NRdMax", m_unitMeasure, UnitMeasure::loadF );
    NRdMax->setRichName(trUtf8("N<span style=\" vertical-align:sub;\">Rd,max</span>"));
    NRdMax->setToolTip(trUtf8("Sforzo normale resistente massimo"));
    NRdMax->setReadOnly( true );
    addVarToContainer( NRdMax );
    connect( fSyd, SIGNAL(valueChanged(QString)), this, SLOT(setNRdMax()) );
    connect( fSud, SIGNAL(valueChanged(QString)), this, SLOT(setNRdMax()) );
    connect( steelFEpsModel, SIGNAL(valueChanged(QString)), this, SLOT(setNRdMax()) );
    connect( m_d->steelAModel, SIGNAL(modelChanged()), this, SLOT(setNRdMax()) );
    setNRdMax();
}

void SimpleSectionRCncrRect::setFcd() {
    if( fcd->readOnly() ){
        double ret = 0.0;
        if( gammaC->valueNormal() != 0.0 ){
            ret = fck->valueNormal() * alphaCC->valueNormal() / gammaC->valueNormal();
        }
        fcd->setValueNormal( ret );
    }
}

void SimpleSectionRCncrRect::setFSyd() {
    if( fSyd->readOnly() ){
        double ret = 0.0;
        if( gammaS->valueNormal() != 0.0 ){
            ret = fSyk->valueNormal() / gammaS->valueNormal();
        }
        fSyd->setValueNormal( ret );
    }
}

void SimpleSectionRCncrRect::setFSud() {
    if( fSud->readOnly() ){
        double ret = 0.0;
        if( Es->valueNormal() != 0.0 ){
            double epsYd = fSyd->valueNormal() / Es->valueNormal();
            double D = epsSUk->valueNormal() - epsYd;
            if( D != 0.0 ){
                ret = fSyd->valueNormal() + (kS->valueNormal() - 1.0 ) * fSyd->valueNormal() * (epsSUd->valueNormal() - epsYd) / D;
            }
        }
        fSud->setValueNormal( ret );
    }
}

void SimpleSectionRCncrRect::setNRdMin() {
    double N = - b->valueNormal() * h->valueNormal() * fcd->valueNormal();
    for( int i=0; i < m_d->steelAModel->count(); ++i ) {
        N += m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( -epsCU2->valueNormal());
    }
    NRdMin->setValueNormal( N );
}

void SimpleSectionRCncrRect::setNRdMax() {
    double N = 0.0;
    for( int i=0; i < m_d->steelAModel->count(); ++i ) {
        if( steelFEpsModel->valueNormal() == SimpleSectionRCncrComp::inclinedTopBranch ){
            N += m_d->steelAModel->A(i)->valueNormal() * fSud->valueNormal();
        } else {
            N += m_d->steelAModel->A(i)->valueNormal() * fSyd->valueNormal();
        }
    }
    NRdMax->setValueNormal( N );
}

void SimpleSectionRCncrRect::setEpsSUd() {
    if( epsSUd->readOnly() ){
        double ret = 0.9 * epsSUk->valueNormal();
        epsSUd->setValueNormal( ret );
    }
}

double SimpleSectionRCncrRect::beta1Normal( double eC ){
    double eC2 = epsC2->valueNormal();
    double eCU2 = epsCU2->valueNormal();
    double n = nC->valueNormal();

    if( eC2 > 0 &&  eCU2 > 0 && eC < 0.0){
        double eta = - eC / eC2;

        if( eta > 0 && eta <= eCU2 / eC2 ){
            double D = (n+1.0) * eta;
            if( D != 0.0 ){
                if ( eta < 1.0 )
                    return (1.0 - (1.0 - pow((1.0-eta),(n+1)) ) / D);
                else
                    return (1.0 - 1.0 / D);
            }
        }
    }
    return 0.0;
}

double SimpleSectionRCncrRect::beta2Normal( double eC ){
    double eC2 = epsC2->valueNormal();
    double eCU2 = epsCU2->valueNormal();
    double n = nC->valueNormal();

    if( eC2 > 0.0 &&  eCU2 > 0.0 && eC < 0.0){
        double eta = - eC / eC2;

        if( eta > 0.0 && eta <= eCU2 / eC2 ){
            if( eta <= 1.0 ){
                double D = (2.0 * (n+2.0) * ( (n+1.0) * eta - 1.0 + pow((1.0-eta),(n+1.0)) ) * eta );
                if( D != 0.0 ){
                    return ((n+1.0) * (n+2.0) * pow(eta,2.0) - 2.0 * (n+2.0) * eta + 2.0 - 2.0 * pow((1.0 - eta),(n+2.0)) ) / D;
                }
            } else {
                double D = (2.0 * (n+2.0) * ((n+1.0) * eta -1.0) * eta);
                if( D != 0.0 ){
                    return ((n+1.0) * (n+2.0) * pow(eta,2.0) - 2.0 * (n+2.0) * eta + 2.0) / D;
                }
            }
        }
    }
    return 0.0;
}

void SimpleSectionRCncrRect::setEpsC2( void ){
    if( epsC2->readOnly() ){
        if( (fck->valueNormal() * 1.0e-6) > 50.0 ){
            epsC2->setValueNormal( 0.002 + 0.000085 * pow( (fck->valueNormal() * 1.0e-6 - 50.0), 0.53) );
        } else {
            epsC2->setValueNormal( 0.002 );
        }
    }
}

void SimpleSectionRCncrRect::setEpsCU2( void ){
    if( epsCU2->readOnly() ){
        if( (fck->valueNormal() * 1.0e-6) >= 50.0 ){
            epsCU2->setValueNormal( 0.0026 + 0.035 * pow( ((90.0 - fck->valueNormal() * 1.0e-6 ) /100.0), 4.0 ) );
        } else {
            epsCU2->setValueNormal( 0.0035 );
        }
    }
}

void SimpleSectionRCncrRect::setNC( void ){
    if( nC->readOnly() ){
        if( (fck->valueNormal() * 1.0e-6) >= 50.0 ){
            nC->setValueNormal( 1.4 + 23.4 * pow( (90-fck->valueNormal()*1.0e-6)/100.0, 4.0) );
        } else {
            nC->setValueNormal(2.0);
        }
    }
}

double SimpleSectionRCncrRect::fCncrNormal( double eC ){
    if( eC < 0.0 && eC >= -epsCU2->valueNormal() ){
        if( eC <= -epsC2 ->valueNormal() ){
            return - fcd->valueNormal();
        } else if( epsC2->valueNormal() != 0.0){
            return - fcd->valueNormal() * ( 1.0 - pow( (1.0 + eC / epsC2->valueNormal()), nC->valueNormal()) );
        }
    }
    return 0.0;
}

double SimpleSectionRCncrRect::fSteelNormal( double eS ){
    if( Es->valueNormal() != 0.0 ){
        double eSYdVal = fSyd->valueNormal() / Es->valueNormal();
        if( fabs(eS) <= eSYdVal ){
            return Es->valueNormal() * eS;
        }
        double eSSgn = 1.0;
        if( eS < 0.0 ){
            eSSgn = -1.0;
        }
        if( steelFEpsModel->value() == SimpleSectionRCncrComp::inclinedTopBranch ){
            if( fabs(eS) <= epsSUd->valueNormal() ){
                double D = epsSUd->valueNormal() - eSYdVal;
                if( D != 0.0 ){
                    double fSYdVal = fSyd->valueNormal();
                    double Ep = (fSud->valueNormal() - fSYdVal) / D;
                    if( eS < 0.0 ){
                        eSYdVal *= -1.0;
                        fSYdVal *= -1.0;
                    }
                    return fSYdVal + Ep * (eS - eSYdVal );
                } else {
                    return (eSSgn * fSyd->valueNormal());
                }
            }
        } else { // steelFEpsModel->valueNormal() == horizontalTopBranch
            return (eSSgn * fSyd->valueNormal());
        }
    }
    return 0.0;
}

double SimpleSectionRCncrRect::NNormal( double eS, double eC, bool inverted ){
    double ret = 0.0;

    if( eS == eC ){
        // risultante cls
        ret += b->valueNormal() * h->valueNormal() * fCncrNormal( eC );
        // risultante armature acciaio
        for( int i=0; i < m_d->steelAModel->count(); ++i ) {
            ret += m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eS );
        }
    } else {
        // altezza utile sezione
        double dVal = 0.0;
        if( inverted ){
            dVal = h->valueNormal() - dMin->valueNormal();
        } else {
            dVal = dMax->valueNormal();
        }

        // distanza asse neutro da borso superiore compresso
        double x = dVal * (-eC) / (eS - eC);

        double RCncr = 0.0;
        if( x > 0.0 ){
            if( x > h->valueNormal() ){
                // deformazione bordo inferiore compresso
                double eCP = (1 - h->valueNormal() / x ) * eC;
                RCncr = fcd->valueNormal() * b->valueNormal() * (x * beta1Normal(eC) - (x - h->valueNormal() ) * beta1Normal(eCP));
            } else {
                RCncr = fcd->valueNormal() * b->valueNormal() * x * beta1Normal(eC);
            }
        }

        ret -= RCncr;

        double RSteel = 0.0;

        if( dVal != 0.0 ){
            for( int i=0; i < m_d->steelAModel->count(); ++i ) {
                // Altezza utile armatura
                double di = 0.0;
                if( inverted ){
                    di = h->valueNormal() - m_d->steelAModel->d(i)->valueNormal();
                } else {
                    di = m_d->steelAModel->d(i)->valueNormal();
                }
                /* deformazione armatura
                   trick necessario per evitare che eSi diventi maggiore di eSud per
                   problemi di approssimazione */
                double eSi = eS;
                if( di < dVal ) {
                    eSi = eC + (eS - eC ) * di / dVal;
                } else if( di > dVal ){
                    eSi = 0.0;
                }
                // risultante armatura
                RSteel += m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eSi );
            }
        }

        ret += RSteel;
    }
    return ret;
}

double SimpleSectionRCncrRect::MNormal( double eS, double eC, bool inverted ){
    double ret = 0.0;

    if( eS == eC ){
        // momento nullo della sezione in cls rispetto al suo baricentro

        // momento delle armature
        for( int i=0; i < m_d->steelAModel->count(); ++i ) {
            // altezza utile armatura
            double di = 0.0;
            if( inverted ){
                di = h->valueNormal() - m_d->steelAModel->d(i)->valueNormal();
                ret -= m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eS ) * (di - h->valueNormal() * 0.50 ) ;
            } else {
                di = m_d->steelAModel->d(i)->valueNormal();
                ret += m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eS ) * (di - h->valueNormal() * 0.50 ) ;
            }
        }
    } else {
        // altezza utile massima
        double dVal = 0.0;
        if( inverted ){
            dVal = h->valueNormal() - dMin->valueNormal();
        } else {
            dVal = dMax->valueNormal();
        }

        // distanza dall'asse neutro del bordo superiore compresso
        double x = dVal * (-eC) / (eS - eC);

        if( x > 0.0 ){
            // momento del cls
            if( x > h->valueNormal() ){
                // deformazione lembo inferiore del cls
                double eCP = (1 - h->valueNormal() / x ) * eC;
                if( inverted ){
                    ret -= fcd->valueNormal() * b->valueNormal() *
                            ( x * beta1Normal(eC) * (h->valueNormal() * 0.50  - x * beta2Normal(eC) )
                              + (x - h->valueNormal()) * beta1Normal(eCP) * (h->valueNormal() * 0.50 + (x-h->valueNormal()) * beta2Normal(eCP) ) );
                } else {
                    ret += fcd->valueNormal() * b->valueNormal() *
                            ( x * beta1Normal(eC) * (h->valueNormal() * 0.50  - x * beta2Normal(eC) )
                              + (x-h->valueNormal()) * beta1Normal(eCP) * (h->valueNormal() * 0.50 + (x-h->valueNormal()) * beta2Normal(eCP) ) );
                }
            } else {
                if( inverted ){
                    ret -= fcd->valueNormal() * b->valueNormal() * x * beta1Normal(eC) * (h->valueNormal() * 0.50  - x * beta2Normal(eC) );
                } else {
                    ret += fcd->valueNormal() * b->valueNormal() * x * beta1Normal(eC) * (h->valueNormal() * 0.50  - x * beta2Normal(eC) );
                }
            }
        }

        // momento armature
        if( dVal != 0.0 ){
            for( int i=0; i < m_d->steelAModel->count(); ++i ) {
                // altezza utile armatura
                double di = 0.0;
                if( inverted ){
                    di = h->valueNormal() - m_d->steelAModel->d(i)->valueNormal();
                } else {
                    di = m_d->steelAModel->d(i)->valueNormal();
                }
                // deformazione armatura
                /* trick necessario per evitare che eSi diventi maggiore di eSud per
                       problemi di approssimazione */
                double eSi = eS;
                if( di < dVal ) {
                    eSi = eC + (eS - eC ) * di / dVal;
                } else if( di > dVal ){
                    eSi = 0.0;
                }
                if( inverted ){
                    ret -= m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eSi ) * (di - h->valueNormal() * 0.50 );
                } else {
                    ret += m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eSi ) * (di - h->valueNormal() * 0.50 );
                }
            }
        }
    }

    return ret;
}

QVector<QPointF> SimpleSectionRCncrRect::NEpsConfigurations1(int nP ) {
    QVector<QPointF> conf;
    if(nP > 0 ){
        double eS = epsSUd->valueNormal();
        double de = (epsSUd->valueNormal() + epsCU2->valueNormal()) / double(nP);
        double eC = epsSUd->valueNormal();
        bool eCPositive = true;
        for( int i=0; i < nP; ++i ){
            conf << QPointF( eC * m_unitMeasure->scale( UnitMeasure::deformation ), NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ) );
            eC -= de;
            if( eC < 0.0 && eCPositive ){
                conf << QPointF( 0.0, NNormal(eS, 0.0) * m_unitMeasure->scale( UnitMeasure::loadF ) );
                eCPositive = false;
            }
        }
        eC = -epsCU2->valueNormal();
        conf << QPointF( eC * m_unitMeasure->scale( UnitMeasure::deformation ), NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ) );
    }
    return conf;
}

QVector<QPointF> SimpleSectionRCncrRect::NEpsConfigurations23( int nP) {
    QVector<QPointF> conf;
    if(nP > 0 ){
        double eS = epsSUd->valueNormal();
        double eC = -epsCU2->valueNormal();

        double eS3 = 0.0;
        if( h->valueNormal() != 0.0 ){
            eS3 = - epsCU2->valueNormal() * (1.0 - dMax->valueNormal()  / h->valueNormal() );
        }
        double de = (eS - eS3)  / double(nP);

        for( int i=0; i < nP; ++i ){
            conf << QPointF( eS * m_unitMeasure->scale( UnitMeasure::deformation ), NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ) );
            eS -= de;
        }
        eS = eS3;
        conf << QPointF( eS * m_unitMeasure->scale( UnitMeasure::deformation ), NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ) );
    }
    return conf;
}

QVector<QPointF> SimpleSectionRCncrRect::NEpsConfigurations4( int nP) {
    QVector<QPointF> conf;

    if( epsCU2->valueNormal() > epsC2->valueNormal() ){
        double eC = -epsCU2->valueNormal();
        double eS3 = 0.0;
        if( h->valueNormal() != 0.0 ){
            eS3 = - epsCU2->valueNormal() * (1.0 - dMax->valueNormal()  / h->valueNormal() );
        }
        double eS = eS3;
        double de = (eS3 + epsC2->valueNormal()) / double(nP);

        if( epsCU2->valueNormal() != 0.0 ){
            double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * h->valueNormal();
            double xS = dMax->valueNormal() - xC;

            if( xS != 0.0 ){
                for( int i=0; i < nP; ++i){
                    conf << QPointF( eS * m_unitMeasure->scale( UnitMeasure::deformation ), NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ) );
                    eS -= de;
                    eC = - ( epsC2->valueNormal() + xC / xS * ( epsC2->valueNormal() + eS) );
                }
                eS = - epsC2->valueNormal();
                eC = - epsC2->valueNormal();
                conf << QPointF( eS * m_unitMeasure->scale( UnitMeasure::deformation ), NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ) );
            }
        }
    }
    return conf;
}

QVector<QPointF> SimpleSectionRCncrRect::MChiDiagram( DoublePlus * NEdPlus, int nP, double prec, int maxIter ) {
    double NEd = NEdPlus->valueNormal();
    QVector<QPointF> ret;
    if( dMax->valueNormal() != 0.0 ){
        double chi = 0.0;
        double chiMax = (epsSUd->valueNormal() + epsCU2->valueNormal()) / dMax->valueNormal();
        double DChi = chiMax / nP;
        int i=-1;
        while( i < nP ){
            // e = e0 + chi * (d - d0)
            double d0 = 0.0;
            double e0Min = -epsCU2->valueNormal();
            double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * h->valueNormal();
            if( e0Min + chi * ( xC - d0) < (-epsC2->valueNormal()) ){
                d0 = xC;
                e0Min = -epsC2->valueNormal();
            }

            double e0Max = epsSUd->valueNormal() - chi * (dMax->valueNormal() - d0);

            double NRdMin = NNormal( e0Min + chi * ( dMax->valueNormal() - d0), e0Min - chi * d0 );
            double NRdMax = NNormal( e0Max + chi * ( dMax->valueNormal() - d0), e0Max - chi * d0 );

            if( NRdMin != 0.0 ){
                if( NEd == NRdMin ){
                    ret << QPointF( chi * m_unitMeasure->scale(UnitMeasure::curvature), MNormal(e0Min + chi * ( dMax->valueNormal() - d0), e0Min - chi * d0 ) * m_unitMeasure->scale(UnitMeasure::loadM));
                } else if( NEd == NRdMax ){
                    ret << QPointF( chi * m_unitMeasure->scale(UnitMeasure::curvature), MNormal(e0Max + chi * ( dMax->valueNormal() - d0), e0Max - chi * d0 ) * m_unitMeasure->scale(UnitMeasure::loadM) );
                } else if( NEd > NRdMin && NEd < NRdMax ){
                    double e0 = e0Min + (NEd - NRdMin) * (e0Max - e0Min) / (NRdMax - NRdMin);
                    double NRd = NNormal(e0 + chi * ( dMax->valueNormal() - d0), e0 - chi * d0 );
                    double DN = fabs( (NRd - NEd) / NRdMin );
                    int i=0;
                    while( (DN > prec) && (i < maxIter)){
                        if( NRdMin != NRdMax ){
                            e0 = e0Min + (NEd - NRdMin) * (e0Max - e0Min) / (NRdMax - NRdMin);
                        } else {
                            i = maxIter;
                        }
                        double NRd = NNormal(e0 + chi * ( dMax->valueNormal() - d0), e0 - chi * d0 );
                        if( NEd == NRd ){
                            DN = 0.0;
                        } else {
                            if( NEd > NRd ){
                                e0Min = e0;
                                NRdMin = NRd;
                            } else {
                                e0Max = e0;
                                NRdMax = NRd;
                            }
                            DN = fabs( (NRd - NEd) / NRdMin );
                        }
                        ++i;
                    }
                    if( DN < prec ){
                        ret << QPointF( chi * m_unitMeasure->scale( UnitMeasure::curvature ), MNormal(e0 + chi * ( dMax->valueNormal() - d0), e0 - chi * d0 ) * m_unitMeasure->scale( UnitMeasure::loadM ));
                    }
                }
            }
            ++i;
            if( i < nP-1){
                chi += DChi;
            } else {
                chi = chiMax;
            }
        }
    }
    return ret;
}

QVector<QPointF> SimpleSectionRCncrRect::NRdMRd(int nP) {
    QVector<QPointF> conf;

    if(nP > 0 ){
        if( steelFEpsModel->valueNormal() == SimpleSectionRCncrComp::inclinedTopBranch ){
            // campo 1
            double eC = epsSUd->valueNormal();
            double eS = epsSUd->valueNormal();
            double de = (epsSUd->valueNormal() + epsCU2->valueNormal()) / double(nP);
            for( int i=0; i < nP; ++i ){
                conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                eC -= de;
            }
            eC = -epsCU2->valueNormal();
            conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );

            // campi 2-3
            eS = epsSUd->valueNormal();
            eC = -epsCU2->valueNormal();
            double eS3 = 0.0;
            if( h->valueNormal() != 0.0 ){
                eS3 = - epsCU2->valueNormal() * (1.0 - dMax->valueNormal()  / h->valueNormal() );
            }
            de = (eS - eS3)  / double(nP);
            for( int i=0; i < nP; ++i ){
                conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                eS -= de;
            }
            eS = eS3;
            conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );

            if( epsCU2->valueNormal() != 0.0 ){
                // campo 4
                eC = -epsCU2->valueNormal();
                eS = eS3;
                de = (eS3 + epsC2->valueNormal()) / double(nP);
                double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * h->valueNormal();
                double xS = dMax->valueNormal() - xC;
                if( xS != 0.0 ){
                    for( int i=0; i < nP; ++i){
                        conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                        eS -= de;
                        eC = - ( epsC2->valueNormal() + xC / xS * ( epsC2->valueNormal() + eS) );
                    }
                    eS = - epsC2->valueNormal();
                    eC = - epsC2->valueNormal();
                    conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                }
            }

            eS3 = 0.0;
            if( h->valueNormal() != 0.0 ){
                eS3 = - epsCU2->valueNormal() * (1.0 - (h->valueNormal() - dMin->valueNormal())  / h->valueNormal() );
            }

            // campo 4 al contrario
            if( epsCU2->valueNormal() != 0.0 ){
                eS = - epsC2->valueNormal();
                eC = - epsC2->valueNormal();
                de = (eS3 + epsC2->valueNormal()) / double(nP);
                double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * h->valueNormal();
                double xS = (h->valueNormal() - dMin->valueNormal()) - xC;
                if( xS != 0.0 ){
                    for( int i=0; i < nP; ++i){
                        conf << QPointF( NNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                        eS += de;
                        eC = - ( epsC2->valueNormal() + xC / xS * ( epsC2->valueNormal() + eS) );
                    }
                    eC = -epsCU2->valueNormal();
                    eS = eS3;
                    conf << QPointF( NNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                }
            }

            // campi 2-3 al contrario
            eC = -epsCU2->valueNormal();
            eS = eS3;
            de = (epsSUd->valueNormal() - eS3)  / double(nP);
            for( int i=0; i < nP; ++i ){
                conf << QPointF( NNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                eS += de;
            }
            eS = epsSUd->valueNormal();
            conf << QPointF( NNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadM ) );

            // campo 1 al contrario
            eS = epsSUd->valueNormal();
            eC = -epsCU2->valueNormal();
            de = (epsSUd->valueNormal() + epsCU2->valueNormal()) / double(nP);
            for( int i=0; i < nP; ++i ){
                conf << QPointF( NNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                eC += de;
            }
            eC = epsSUd->valueNormal();
            conf << QPointF( NNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadM ) );
        } else { // if( steelFEpsModel->valueNormal() == horizontalTopBranch )
            double epsSMax = 2.0 * epsSUd->valueNormal();

            // campo 1
            double eC = epsSMax;
            double eS = epsSMax;
            conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );

            // campi 2-3
            eS = epsSMax;
            eC = -epsCU2->valueNormal();
            double eS3 = 0.0;
            if( h->valueNormal() != 0.0 ){
                eS3 = - epsCU2->valueNormal() * (1.0 - dMax->valueNormal()  / h->valueNormal() );
            }
            double de = (eS - eS3)  / double(nP);
            for( int i=0; i < nP; ++i ){
                conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                eS -= de;
            }
            eS = eS3;
            conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );

            if( epsCU2->valueNormal() != 0.0 ){
                // campo 4
                eC = -epsCU2->valueNormal();
                eS = eS3;
                de = (eS3 + epsC2->valueNormal()) / double(nP);
                double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * h->valueNormal();
                double xS = dMax->valueNormal() - xC;
                if( xS != 0.0 ){
                    for( int i=0; i < nP; ++i){
                        conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                        eS -= de;
                        eC = - ( epsC2->valueNormal() + xC / xS * ( epsC2->valueNormal() + eS) );
                    }
                    eS = - epsC2->valueNormal();
                    eC = - epsC2->valueNormal();
                    conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                }
            }

            eS3 = 0.0;
            if( h->valueNormal() != 0.0 ){
                eS3 = - epsCU2->valueNormal() * (1.0 - (h->valueNormal() - dMin->valueNormal())  / h->valueNormal() );
            }

            // campo 4 al contrario
            if( epsCU2->valueNormal() != 0.0 ){
                eS = - epsC2->valueNormal();
                eC = - epsC2->valueNormal();
                de = (eS3 + epsC2->valueNormal()) / double(nP);
                double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * h->valueNormal();
                double xS = (h->valueNormal() - dMin->valueNormal()) - xC;
                if( xS != 0.0 ){
                    for( int i=0; i < nP; ++i){
                        conf << QPointF( NNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                        eS += de;
                        eC = - ( epsC2->valueNormal() + xC / xS * ( epsC2->valueNormal() + eS) );
                    }
                    eC = -epsCU2->valueNormal();
                    eS = eS3;
                    conf << QPointF( NNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                }
            }

            // campi 2-3 al contrario
            eC = -epsCU2->valueNormal();
            eS = eS3;
            de = (epsSMax - eS3)  / double(nP);
            for( int i=0; i < nP; ++i ){
                conf << QPointF( NNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadM ) );
                eS += de;
            }
            eS = epsSMax;
            conf << QPointF( NNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC, true) * m_unitMeasure->scale( UnitMeasure::loadM ) );

            // campo 1 al contrario
            eC = epsSMax;
            eS = epsSMax;
            conf << QPointF( NNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadF ), MNormal(eS, eC) * m_unitMeasure->scale( UnitMeasure::loadM ) );
        }
    }
    return conf;
}

void SimpleSectionRCncrRect::ULSTMax( DoublePlus *TRd, DoublePlus *TRdc, DoublePlus *TRds,
                                      DoublePlus *Asw, DoublePlus *s, DoublePlus *theta) {
    double t, TRdcVal, TRdsVal;
    TRd->setValueNormal( ULSTMaxNormal( &TRdcVal, &TRdsVal,
                                        Asw->valueNormal(), s->valueNormal(), &t ) );
    theta->setValueNormal(t);
    TRdc->setValueNormal( TRdcVal );
    TRds->setValueNormal( TRdsVal );
}

double SimpleSectionRCncrRect::ULSTMaxNormal( double *TRdc, double *TRds,
                                              double Asw, double s, double *theta ) {
    *TRdc = 0.0;
    *TRds = 0.0;
    *theta = M_PI / 4.0;

    if( fSyd->valueNormal() != 0.0 && Asw != 0.0 ) {
        double A = b->valueNormal() * h->valueNormal();
        // perimetro esterno della sezione
        double u = (b->valueNormal() + h->valueNormal()) * 2.0;
        double tEff = A / u;
        double Ak = (b->valueNormal() - tEff) * (h->valueNormal() - tEff);
        double alphaCW = 1.0;
        double nu1 = 0.60 * (1.0 - fck->valueNormal() * 1e-6/ 250.0);
        double rad = alphaCW * nu1 * tEff * fcd->valueNormal() * s / (fSyd->valueNormal() * Asw) - 1.0;
        if ( rad > 0.0 ) {
            double cotTheta = sqrt(rad);
            if( cotTheta < 1.0 ){
                cotTheta = 1.0;
            } else if( cotTheta > 2.5 ){
                cotTheta = 2.5;
            }
            *theta = atan( 1.0 / cotTheta );
            *TRdc = 2.0 * Ak * tEff * alphaCW * nu1 * fcd->valueNormal() / (cotTheta + 1 / cotTheta );
            *TRds = 2.0 * Ak * Asw * cotTheta * fSyd->valueNormal() / s;
            return qMin( *TRdc, *TRds );
        }
    }
    return 0.0;
}

void SimpleSectionRCncrRect::ULST(DoublePlus *T, DoublePlus *TRdc, DoublePlus *TRds,
                                  DoublePlus *Asw, DoublePlus *s, DoublePlus *theta) {
    double Tc = 0.0, Ts = 0.0;
    T->setValueNormal( ULSTNormal( &Tc, &Ts, Asw->valueNormal(), s->valueNormal(), theta->valueNormal() ) );
    TRdc->setValueNormal( Tc );
    TRds->setValueNormal( Ts );
}

double SimpleSectionRCncrRect::ULSTNormal( double *TRdc, double *TRds,
                                           double Asw, double s, double theta ) {
    *TRdc = 0.0;
    *TRds = 0.0;
    if( s > 0.0 ){
        double u = 2.0 * (b->valueNormal() + h->valueNormal());
        if( u != 0.0 ) {
            double A = b->valueNormal() * h->valueNormal();
            double tEff = A / u;
            double Ak = (b->valueNormal() - tEff) * (h->valueNormal() - tEff);

            if( tan(theta) > 0 ){
                double cotTheta = 1 / tan(theta);
                if( cotTheta < 1.0 ){
                    cotTheta = 1.0;
                } else if( cotTheta > 2.5 ){
                    cotTheta = 2.5;
                }
                double tTheta = 1.0 / cotTheta;
                double alphaCW = 1.0;
                double nu1 = 0.60 * (1.0 - fck->valueNormal() * 1e-6/ 250.0);
                *TRdc = 2.0 * Ak * tEff * alphaCW * nu1 * fcd->valueNormal() / (cotTheta + tTheta );
                *TRds = 2.0 * Ak * Asw * cotTheta * fSyd->valueNormal() / s;
            }
        }
    }
    return qMin( *TRdc, *TRds );
}

double SimpleSectionRCncrRect::ULSVNoSteelNormal(double bw, double dw, double Asl, double scp ) {
    double CRd = 0.0;
    if( gammaC->valueNormal() > 0.0 ){
        CRd = 0.18 / gammaC->valueNormal();
    }

    double k = 0.0;
    if( dw > 0.0 ){
        k = 1.0 + sqrt( 200.0 / (dw*1.0e+3));
        if( k > 2.0 ){
            k = 2.0;
        }
    }

    double rhol = 0.0;
    if( bw > 0.0 && dw > 0.0 ){
        rhol = Asl / (bw * dw);
        if( rhol > 0.02 ){
            rhol = 0.02;
        }
    }

    double k1 = 0.15;

    if( scp > 0.2 * fcd->valueNormal() ){
        scp = 0.2 * fcd->valueNormal();
    }

    double ret = ( CRd * k * pow(100.0 * rhol * (fck->valueNormal()*1e-6), 1.0/3.0) + k1 * scp*1.0e-6 ) * bw * 1.0e+3 * dw * 1.0e+3;

    double nuMin = 0.035 * pow(k, 3.0/2.0) * pow(fck->valueNormal()*1e-6, 0.50) * 1e+6;
    double retMin = (nuMin + k1 * scp) * bw * dw;

    if( (ret > 0.0) && (ret < retMin) ){
        ret = retMin;
    }

    return ret;
}

void SimpleSectionRCncrRect::ULSVNoSteel( DoublePlus * V, DoublePlus * bw, DoublePlus * dw, DoublePlus * Asl, DoublePlus * scp ) {
    V->setValueNormal( ULSVNoSteelNormal(bw->valueNormal(), dw->valueNormal(), Asl->valueNormal(), scp->valueNormal() ));
}

double SimpleSectionRCncrRect::ULSVSteelNormal( double *VRdmax, double *VRds,
                                                double bw, double dw, double Asw, double s, double theta, double alpha) {
    *VRds = 0.0;
    *VRdmax = 0.0;
    if( tan(theta) > 0 && tan(alpha) > 0.0 ){
        double cotTheta = 1 / tan(theta);
        if( cotTheta < 1.0 ){
            cotTheta = 1.0;
        } else if( cotTheta > 2.5 ){
            cotTheta = 2.5;
        }
        double cotAlpha = 1 / tan(alpha);
        if( s > 0.0 ){
            *VRds = Asw * 0.90 * dw * fSyd->valueNormal() * ( cotTheta + 1 / tan(alpha) * sin(alpha)) / s;
        }
        double alphaCW = 1.0;
        double nu1 = 0.60 * (1.0 - fck->valueNormal() * 1e-6/ 250.0);

        *VRdmax = alphaCW * bw * (0.90 * dw) * nu1 * fcd->valueNormal() * (cotTheta + cotAlpha) / (1.0 + cotTheta * cotTheta);
    }

    return qMin( *VRdmax, *VRds);
}

void SimpleSectionRCncrRect::ULSVSteel( DoublePlus * V, DoublePlus * VMax, DoublePlus * Vs,
                                        DoublePlus * bw, DoublePlus * dw, DoublePlus * Asw, DoublePlus * s, DoublePlus * theta, DoublePlus * alpha) {
    double VRdmax, VRds;
    V->setValueNormal( ULSVSteelNormal( &VRdmax, &VRds, bw->valueNormal(), dw->valueNormal(), Asw->valueNormal(), s->valueNormal(), theta->valueNormal(), alpha->valueNormal() ));
    VMax->setValueNormal( VRdmax );
    Vs->setValueNormal( VRds );
}

double SimpleSectionRCncrRect::ULSVSteelMaxNormal( double *VRdmax, double *VRds,
                                                   double bw, double dw, double Asw, double s,
                                                   double * theta, double alpha) {
    *VRds = 0.0;
    *VRdmax = 0.0;
    *theta = M_PI/4.0;

    if( tan(alpha) > 0.0 ){
        double alphaCW = 1.0;
        double nu1 = 0.60 * (1.0 - fck->valueNormal() * 1e-6/ 250.0);

        // calcola theta
        double cotTheta = 1.0;
        double D = Asw * sin(alpha) * fSyd->valueNormal();
        if( D != 0.0 ){
            double rad = alphaCW * nu1 * s * bw * fcd->valueNormal() / D - 1.0;
            if( rad > 0.0 ){
                cotTheta = sqrt( rad );
            }
        }
        if( cotTheta < 1.0){
            cotTheta = 1.0;
        } else if( cotTheta > 2.5 ){
            cotTheta = 2.5;
        }
        *theta = atan( 1 / cotTheta );

        double cotAlpha = 1 / tan(alpha);
        if( s > 0.0 ){
            *VRds = Asw * 0.90 * dw * fSyd->valueNormal() * ( cotTheta + 1 / tan(alpha) * sin(alpha)) / s;
        }

        *VRdmax = alphaCW * bw * (0.90 * dw) * nu1 * fcd->valueNormal() * (cotTheta + cotAlpha) / (1.0 + cotTheta * cotTheta);
    }
    return qMin( *VRdmax, *VRds);
}

void SimpleSectionRCncrRect::ULSVSteelMax( DoublePlus * V, DoublePlus * VMax, DoublePlus * Vs,
                                           DoublePlus * bw, DoublePlus * dw, DoublePlus * Asw, DoublePlus * s, DoublePlus * theta, DoublePlus * alpha) {
    double t, VRdmax, VRds;
    V->setValueNormal( ULSVSteelMaxNormal( &VRdmax, &VRds,
                                           bw->valueNormal(), dw->valueNormal(), Asw->valueNormal(), s->valueNormal(), &t, alpha->valueNormal() ));
    theta->setValueNormal(t);
    VMax->setValueNormal( VRdmax );
    Vs->setValueNormal( VRds );
}

void SimpleSectionRCncrRect::SLSNM(DoublePlus * N, DoublePlus * M, DoublePlus * alphaE,
                                   SLSSteelSigmaModel * sigmaSModel, DoublePlus * sigmaCUp, DoublePlus * sigmaCDown,
                                   DoublePlus * x) {
    double sigmaSNormal[ m_d->steelAModel->count() ];
    double sigmaCUpNormal = 0.0, sigmaCDownNormal = 0.0, xNormal = 0.0;
    SLSNMNormal( N->valueNormal(), M->valueNormal(), alphaE->valueNormal(), sigmaSNormal, &sigmaCUpNormal, &sigmaCDownNormal, &xNormal );

    for( int i=0; i < m_d->steelAModel->count(); ++i){
        sigmaSModel->setValueNormal( sigmaSNormal[i], i );
    }
    sigmaCUp->setValueNormal( sigmaCUpNormal );
    sigmaCDown->setValueNormal( sigmaCDownNormal );
    x->setValueNormal( xNormal );
}



void SimpleSectionRCncrRect::SLSNMNormal(double N, double M, double alphaE, double sigmaS[], double *sigmaCUp, double *sigmaCDown, double *x) {
    if( N == 0.0 ){
        if( M > 0.0 ){
            // flessione semplice positiva
            double a1 = b->valueNormal() / 2.0;
            double b1 = alphaE * m_d->steelAModel->SANormal();
            double c1 = - alphaE * m_d->steelAModel->SAdNormal();
            double xReal[2];
            double xCmp[2];
            solvePoly2( xReal, xCmp, a1, b1, c1 );

            for( int i=0; i < 2; ++i){
                if( xCmp[i] == 0.0 && xReal[i] > 0.0 && xReal[i] < h->valueNormal() ){
                    double xTmp = xReal[i];
                    double I0 = b->valueNormal() * pow(xTmp, 3.0) / 3.0 + alphaE * ( m_d->steelAModel->SAd2Normal() - 2.0 * m_d->steelAModel->SAdNormal() * xTmp + m_d->steelAModel->SANormal() * pow(xTmp, 2.0) );
                    if( I0 != 0.0 ){
                        *x = xTmp;
                        *sigmaCUp = - M * xTmp / I0;
                        *sigmaCDown = 0.0;
                        for( int i=0; i<m_d->steelAModel->count(); ++i){
                            sigmaS[i] = alphaE * M * ( m_d->steelAModel->d(i)->valueNormal() - xTmp) / I0;
                        }
                        return;
                    }
                }
            }
        } else if( M < 0.0 ){
            // flessione semplice negativa
            double a1 = b->valueNormal() / 2.0;
            double b1 = alphaE * m_d->steelAModel->SANormal();
            double c1 = - alphaE * ( m_d->steelAModel->SANormal() * h->valueNormal() - m_d->steelAModel->SAdNormal() );
            double xReal[2];
            double xCmp[2];
            solvePoly2( xReal, xCmp, a1, b1, c1 );

            for( int i=0; i < 2; ++i){
                if( xCmp[i] == 0.0 && xReal[i] > 0.0 && xReal[i] < h->valueNormal() ){
                    double xTmp = xReal[i];
                    double I0 = b->valueNormal() * pow(xTmp, 3.0) / 3.0 +
                            alphaE *
                            ( m_d->steelAModel->SANormal() * pow(h->valueNormal(), 2.0) - 2.0 * h->valueNormal() * m_d->steelAModel->SAdNormal() + m_d->steelAModel->SAd2Normal()
                              - 2.0 * ( m_d->steelAModel->SANormal() * h->valueNormal() - m_d->steelAModel->SAdNormal() ) * xTmp +
                              m_d->steelAModel->SANormal() * pow(xTmp, 2.0) );
                    if( I0 != 0.0 ){
                        *x = h->valueNormal() - xTmp;
                        *sigmaCUp = 0.0;
                        *sigmaCDown = M * xTmp / I0;
                        for( int i=0; i<m_d->steelAModel->count(); ++i){
                            sigmaS[i] = - alphaE * M * ( (h->valueNormal() - m_d->steelAModel->d(i)->valueNormal() ) - xTmp) / I0;
                        }
                        return;
                    }
                    break;
                }
            }
        }
    } else {
        // pressoflessione/tensoflessione

        // verifichiamo se la sezione è interamente reagente
        double sCUp = 0.0, sCDown = 0.0;
        double sS[m_d->steelAModel->count()];
        double xTmp = calcSigmaNormal( &sCUp, &sCDown, sS, N, M, h->valueNormal(), alphaE, false );
        if( (sCUp <= 0.0 && sCDown <= 0.0) ){
            *x = xTmp;
            *sigmaCDown = sCDown;
            *sigmaCUp = sCUp;
            for( int i=0; i<m_d->steelAModel->count(); ++i){
                sigmaS[i] = sS[i];
            }
            return;
        }

        // verifichiamo se reagiscono solo le armature
        xTmp = calcSigmaNormal( &sCUp, &sCDown, sS, N, M, 0.0, alphaE, false );
        if( (sCUp >= 0.0 && sCDown >= 0.0) ){
            *x = xTmp;
            *sigmaCDown = 0.0;
            *sigmaCUp = 0.0;
            for( int i=0; i<m_d->steelAModel->count(); ++i){
                sigmaS[i] = sS[i];
            }
            return;
        }

        // asse neutro interno alla sezione: proseguiamo
        if( M > 0.0 ){
            // bordo superiore compresso
            double a1 = N * b->valueNormal();
            double b1 = -3.0 * (M + N * h->valueNormal() / 2.0) * b->valueNormal();
            double c1 = 6.0 * alphaE * ( N * m_d->steelAModel->SAdNormal() - (M + N * h->valueNormal() / 2.0) * m_d->steelAModel->SANormal() );
            double d1 = 6.0 * alphaE * ( - N * m_d->steelAModel->SAd2Normal() + (M + N * h->valueNormal() / 2.0) * m_d->steelAModel->SAdNormal() );
            double xReal[3], xIm[3];
            solvePoly3( xReal, xIm, a1, b1, c1, d1);

            for( int i=0; i < 3; ++i){
                if( xIm[i] == 0.0 && xReal[i] > 0.0 && xReal[i] < h->valueNormal() ){
                    *x = calcSigmaNormal( &sCUp, &sCDown, sS, N, M, xReal[i], alphaE, false );
                    if( sCDown < 0.0 ){
                        *sigmaCDown = sCDown;
                    } else {
                        *sigmaCDown = 0.0;
                    }
                    if( sCUp < 0.0 ){
                        *sigmaCUp = sCUp;
                    } else {
                        *sigmaCUp = 0.0;
                    }
                    for( int i=0; i<m_d->steelAModel->count(); ++i){
                        sigmaS[i] = sS[i];
                    }
                    return;
                }
            }

        } else { // M < 0.0
            // bordo inferiore compresso
            double a1 = N * b->valueNormal();
            double b1 = -3.0 * (-M + N * h->valueNormal() / 2.0) * b->valueNormal();
            double c1 = 6.0 * alphaE * ( N * ( h->valueNormal() * m_d->steelAModel->SANormal() / 2.0 - m_d->steelAModel->SAdNormal() ) + M * m_d->steelAModel->SANormal() );
            double d1 = 6.0 * alphaE * ( - M * (h->valueNormal() * m_d->steelAModel->SANormal() - m_d->steelAModel->SAdNormal() )
                                         - N * (pow(h->valueNormal(), 2.0) * m_d->steelAModel->SANormal() / 2.0 - 3.0 / 2.0 * h->valueNormal() * m_d->steelAModel->SAdNormal() + m_d->steelAModel->SAd2Normal() ) );
            double xReal[3], xIm[3];
            solvePoly3( xReal, xIm, a1, b1, c1, d1);

            for( int i=0; i < 3; ++i){
                if( xIm[i] == 0.0 && xReal[i] > 0.0 && xReal[i] < h->valueNormal() ){
                    xTmp = calcSigmaNormal( &sCUp, &sCDown, sS, N, M, xReal[i], alphaE, true );
                    if( M == 0.0 ){
                        *x = 0.0;
                    } else {
                        *x = xTmp;
                    }
                    if( sCDown < 0.0 ){
                        *sigmaCDown = sCDown;
                    } else {
                        *sigmaCDown = 0.0;
                    }
                    if( sCUp < 0.0 ){
                        *sigmaCUp = sCUp;
                    } else {
                        *sigmaCUp = 0.0;
                    }
                    for( int i=0; i<m_d->steelAModel->count(); ++i){
                        sigmaS[i] = sS[i];
                    }
                    return;
                }
            }
        }
    }

    // se siamo arrivati qui qualcosa è andata male
    // azzeriamo i valori di output
    *x = 0.0;
    *sigmaCUp = 0.0;
    *sigmaCDown = 0.0;
    for( int i=0; i < m_d->steelAModel->count(); ++i ){
        sigmaS[i] = 0.0;
    }
}

double SimpleSectionRCncrRect::calcSigmaNormal( double *sigmaCUp, double *sigmaCDown, double sigmaS[], double N, double M, double x, double alphaE, bool invert ){
    if( x < 0.0 ){
        x = 0.0;
    }
    if( x > h->valueNormal() ){
        x = h->valueNormal();
    }
    double A0 = b->valueNormal() * x + alphaE * m_d->steelAModel->SANormal();
    if( A0 != 0.0 ){
        double S0 = 0.0;
        if( invert ){
            S0 = b->valueNormal() * pow(x, 2.0) / 2.0 + alphaE * ( m_d->steelAModel->SANormal() * h->valueNormal() - m_d->steelAModel->SAdNormal() );
        } else {
            S0 = b->valueNormal() * pow(x, 2.0) / 2.0 + alphaE * m_d->steelAModel->SAdNormal();
        }
        double hG = S0 / A0;
        if( invert ){
            double I0 = b->valueNormal() * ( pow(x, 3.0) / 3.0 - hG * pow(x,2.0) + pow(hG,2.0) * x )
                    + alphaE * ( m_d->steelAModel->SAd2Normal()
                                 - 2.0 * ( (h->valueNormal() - hG) * m_d->steelAModel->SAdNormal() )
                                 + pow(h->valueNormal()-hG, 2.0) * m_d->steelAModel->SANormal() );
            double M0 = - M - N * ( hG - h->valueNormal() / 2.0 );
            if( I0!= 0.0 ){
                *sigmaCUp =  N / A0 + M0 / I0 * ( - hG );
                *sigmaCDown = N / A0 + M0 / I0 * (h->valueNormal() - hG);
                for( int i=0; i < m_d->steelAModel->count(); ++i ){
                    sigmaS[i] = alphaE * (N / A0 + M0 / I0 * ( (h->valueNormal() - m_d->steelAModel->d(i)->valueNormal()) -  hG ));
                }
                if( M0 != 0.0 ){
                    return h->valueNormal() - ( hG - I0 * N / (A0 * M0) );
                } else {
                    return 0.0;
                }
            }
        } else {
            double I0 = b->valueNormal() * ( pow(x, 3.0) / 3.0 - hG * pow(x,2.0) + pow(hG,2.0) * x )
                    + alphaE * ( m_d->steelAModel->SAd2Normal()
                                 - 2.0 * hG * m_d->steelAModel->SAdNormal()
                                 + pow(hG,2.0) * m_d->steelAModel->SANormal());
            if( I0!= 0.0 ){
                double M0 = M - N * ( hG - h->valueNormal() / 2.0 );
                *sigmaCUp =  N / A0 + M0 / I0 * ( - hG );
                *sigmaCDown = N / A0 + M0 / I0 * (h->valueNormal() - hG);
                for( int i=0; i < m_d->steelAModel->count(); ++i ){
                    sigmaS[i] = alphaE * (N / A0 + M0 / I0 * ( m_d->steelAModel->d(i)->valueNormal() -  hG ));
                }
                if( M0 != 0.0 ){
                    return hG - I0 * N / (A0 * M0);
                } else {
                    return 0.0;
                }
            }
        }
    }
    // Se siamo arrivati qui qualcosa è andata storta!
    *sigmaCUp = 0.0;
    *sigmaCDown = 0.0;
    for( int i=0; i < m_d->steelAModel->count(); ++i ){
        sigmaS[i] = 0.0;
    }
    return 0.0;
}


double SimpleSectionRCncrRect::MRdNormal(double NEd, double * x, bool inverted, double prec, int maxIter ){
    double NRd3 = NNormal( - epsC2->valueNormal() , - epsC2->valueNormal(), inverted );
    if( NEd < NRd3 ){
        *x = 0.0;
        return 0.0;
    }
    if( NEd == NRd3 )    {
        *x = 0.0;
        return MNormal( - epsC2->valueNormal() , - epsC2->valueNormal(), inverted );
    }

    if( NRd3 != 0.0 ){
        double NRd0 = NNormal( epsSUd->valueNormal(), epsSUd->valueNormal(), inverted );
        if( NEd > NRd0 ){
            *x = 0.0;
            return 0.0;
        }
        if( NEd == NRd0 )    {
            *x = 0.0;
            return MNormal( epsSUd->valueNormal(), epsSUd->valueNormal(), inverted );
        }

        double dVal = 0.0;
        if( inverted ){
            dVal = h->valueNormal() - dMin->valueNormal();
        } else {
            dVal = dMax->valueNormal();
        }

        double NRd1 = NNormal( epsSUd->valueNormal(), - epsCU2->valueNormal(), inverted );
        if( NEd == NRd1 ) {
            double xVal = 0.0;
            if( (epsSUd->valueNormal()+epsCU2->valueNormal()) != 0.0 ){
                xVal = dVal * epsCU2->valueNormal() / (epsSUd->valueNormal()+epsCU2->valueNormal());
            }
            *x = xVal;
            return MNormal( epsSUd->valueNormal(), - epsCU2->valueNormal(), inverted );
        }
        if( NEd > NRd1 ){
            if( steelFEpsModel->valueNormal() == SimpleSectionRCncrComp::inclinedTopBranch ){
                double eS = epsSUd->valueNormal();
                double eCA, eCB, eCC;

                // restringiamo il campo (vedi forma curva)
                double NRd01 = NNormal( eS, 0.0, inverted );
                if( NEd == NRd01 ){
                    *x = 0.0;
                    return MNormal( eS, 0.0, inverted );
                } else if( NEd > NRd01 ){
                    eCA = epsSUd->valueNormal();
                    eCB = 0.0;
                } else {
                    eCA = 0.0;
                    eCB = -epsCU2->valueNormal();
                }

                // metodo secante
                double NRdA = NNormal( eS, eCA, inverted );
                double NRdB = NNormal( eS, eCB, inverted );
                if( NRdA != NRdB ){
                    eCC = eCB - (NRdB-NEd) * (eCB - eCA) / (NRdB - NRdA);
                }
                double NRdC = NNormal( eS, eCC, inverted );
                double DN = fabs( (NRdC - NEd) / NRd3 );

                int i=0;
                while( (DN > prec) && (i < maxIter)){
                    if( NRdA != NRdB ){
                        eCC = eCB - (NRdB-NEd) * (eCB - eCA) / (NRdB - NRdA);
                    }
                    NRdC = NNormal( eS, eCC, inverted );
                    if( NEd == NRdC ){
                        double xVal = 0.0;
                        if( (eS-eCC) != 0.0 ){
                            xVal = dVal * (-eCC) / (eS-eCC);
                        }
                        *x = xVal;
                        return MNormal( eS, eCC, inverted );
                    } else if( NEd > NRdC ){
                        eCB = eCC;
                        NRdB = NRdC;
                    } else {
                        eCA = eCC;
                        NRdA = NRdC;
                    }
                    DN = fabs( (NRdC - NEd) / NRd3 );
                    ++i;
                }
                if( DN < prec ){
                    double xVal = 0.0;
                    if( (eS-eCC) != 0.0 ){
                        xVal = dVal * (-eCC) / (eS-eCC);
                    }
                    *x = xVal;
                    return MNormal( eS, eCC, inverted );
                } else {
                    *x = 0.0;
                    return 0.0;
                }
            } else { // if( steelFEpsModel->valueNormal() == horizontalTopBranch )
                double eSA = epsSUd->valueNormal();
                double de = epsSUd->valueNormal() * 0.10;
                double eSB = eSA + de;
                double eC = -epsCU2->valueNormal();

                double NRdA = NNormal( eSA, eC, inverted );
                double NRdB = NNormal( eSB, eC, inverted );

                if( NEd == NRdB ){
                    double xVal = 0.0;
                    if( (eSB-eC) != 0.0 ){
                        xVal = dVal * (-eC) / (eSB-eC);
                    }
                    *x = xVal;
                    return MNormal( eSB, eC, inverted );
                }

                while( NRdB < NEd ){
                    eSA += eSB;
                    eSB += de;
                    NRdA = NRdB;
                    NRdB = NNormal( eSB, eC, inverted );
                    if( NEd == NRdB ){
                        double xVal = 0.0;
                        if( (eSB-eC) != 0.0 ){
                            xVal = dVal * (-eC) / (eSB-eC);
                        }
                        *x = xVal;
                        return MNormal( eSB, eC, inverted );
                    }
                }

                // metodo secante
                double eSC = eSA;
                if( NRdA != NRdB ){
                    eSC = eSA + (NEd-NRdA) * (eSB - eSA) / (NRdB - NRdA);
                }
                double NRdC = NNormal( eSC, eC, inverted );
                double DN = fabs( (NRdC - NEd) / NRd3 );

                int i=0;
                while( (DN > prec) && (i < maxIter)){
                    if( NRdA != NRdB ){
                        eSC = eSA + (NEd-NRdA) * (eSB - eSA) / (NRdB - NRdA);
                    }
                    NRdC = NNormal( eSC, eC, inverted );
                    if( NEd == NRdC ){
                        double xVal = 0.0;
                        if( (eSC-eC) != 0.0 ){
                            xVal = dVal * (-eC) / (eSC-eC);
                        }
                        *x = xVal;
                        return MNormal( eSC, eC, inverted );
                    } else if( NEd > NRdC ){
                        eSA = eSC;
                        NRdA = NRdC;
                    } else {
                        eSB = eSC;
                        NRdB = NRdC;
                    }
                    DN = fabs( (NRdC - NEd) / NRd3 );
                    ++i;
                }
                if( DN < prec ){
                    double xVal = 0.0;
                    if( (eSC-eC) != 0.0 ){
                        xVal = dVal * (-eC) / (eSC-eC);
                    }
                    *x = xVal;
                    return MNormal( eSC, eC, inverted );
                } else {
                    *x = 0.0;
                    return 0.0;
                }
            }
        }

        double eS3 = 0.0;
        if( h->valueNormal() != 0.0 ){
            eS3 = - epsCU2->valueNormal() * (1.0 - dVal  / h->valueNormal() );
        }

        double eS = 0.0;
        if( h->valueNormal() != 0.0 ){
            eS = - epsCU2->valueNormal() * (1.0 - dVal  / h->valueNormal() );
        } else {
            *x = 0.0;
            return 0.0;
        }
        double NRd2 = NNormal( eS , - epsCU2->valueNormal(), inverted );
        if( NEd == NRd2 )    {
            double xVal = 0.0;
            if( (eS+epsCU2->valueNormal()) != 0.0 ){
                xVal = dVal * epsCU2->valueNormal() / (eS+epsCU2->valueNormal());
            }
            *x = xVal;
            return MNormal( eS, - epsCU2->valueNormal(), inverted );
        }
        if( NEd > NRd2 ){
            double eC = -epsCU2->valueNormal();
            double eSA, eSB, eSC;
            eSA = epsSUd->valueNormal();
            eSB = eS3;

            // metodo bisezione piu' peformante di metodo secante
            /* double NRdA = NNormal( eSA, eC, inverted );
            double NRdB = NNormal( eSB, eC, inverted );
            if( NRdA != NRdB ){
                eSC = eSB - (NRdB - NEd) * (eSB - eSA) / (NRdB - NRdA);
            } */
            eSC = (eSB + eSA) * 0.50;
            double NRdC = NNormal( eSC, eC );
            double DN = fabs( (NRdC - NEd) / NRd3 );

            int i=0;
            while( (DN > prec) && (i < maxIter)){
                /* if( NRdA != NRdB ){
                    eSC = eSB - (NRdB - NEd) * (eSB - eSA) / (NRdB - NRdA);
                }*/
                eSC = (eSB + eSA) * 0.50;
                NRdC = NNormal( eSC, eC, inverted );
                if( NEd == NRdC ){
                    double xVal = 0.0;
                    if( (eSC-eC) != 0.0 ){
                        xVal = dVal * (-eC) / (eSC-eC);
                    }
                    *x = xVal;
                    return MNormal( eSC, eC, inverted );
                } else if( NEd > NRdC ){
                    eSB = eSC;
                    // NRdB = NRdC;
                } else {
                    eSA = eSC;
                    // NRdA = NRdC;
                }
                DN = fabs( (NRdC - NEd) / NRd3 );
                ++i;
            }
            if( DN < prec ){
                double xVal = 0.0;
                if( (eSC-eC) != 0.0 ){
                    xVal = dVal * (-eC) / (eSC-eC);
                }
                *x = xVal;
                return MNormal( eSC, eC, inverted );
            } else {
                *x = 0.0;
                return 0.0;
            }
        }

        // if( NEd > NRd3 )
        // pivot attorno a cls - interno

        double xC = 0.0;
        if( epsCU2->valueNormal() != 0.0 ){
            xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * h->valueNormal();
        } else {
            *x = 0.0;
            return 0.0;
        }
        double xS = dVal - xC;
        if( xS == 0.0 ){
            *x = 0.0;
            return 0.0;
        }

        double eCA, eCB, eCC = 0.0;
        double eSA, eSB, eSC = 0.0;
        eSA = eS3;
        eCA = -epsCU2->valueNormal();
        eCB = -epsC2->valueNormal();
        eSB = -epsC2->valueNormal();

        // metodo secante
        double NRdA = NNormal( eSA, eCA, inverted );
        double NRdB = NNormal( eSB, eCB, inverted );
        if( NRdA != NRdB ){
            eSC = eSB - (NRdB - NEd) * (eSB - eSA) / (NRdB - NRdA);
            eCC = - ( epsC2->valueNormal() + xC / xS * ( epsC2->valueNormal() + eSC) );
        }
        double NRdC = NNormal( eSC, eCC, inverted );
        double DN = fabs( (NRdC - NEd) / NRd3 );

        int i=0;
        while( (DN > prec) && (i < maxIter)){
            if( NRdA != NRdB ){
                eSC = eSB - (NRdB - NEd) * (eSB - eSA) / (NRdB - NRdA);
                eCC = - ( epsC2->valueNormal() + xC / xS * ( epsC2->valueNormal() + eSC) );
            }
            NRdC = NNormal( eSC, eCC, inverted );
            if( NEd == NRdC ){
                double xVal = 0.0;
                if( (eSC-eCC) != 0.0 ){
                    xVal = dVal * (-eCC) / (eSC-eCC);
                }
                *x = xVal;
                return MNormal( eSC, eCC, inverted );
            } else if( NEd > NRdC ){
                eSB = eSC;
                NRdB = NRdC;
            } else {
                eSA = eSC;
                NRdA = NRdC;
            }
            DN = fabs( (NRdC - NEd) / NRd3 );
            ++i;
        }
        if( DN < prec ){
            double xVal = 0.0;
            if( (eSC-eCC) != 0.0 ){
                xVal = dVal * (-eCC) / (eSC-eCC);
            }
            *x = xVal;
            return MNormal( eSC, eCC, inverted );
        } else {
            *x = 0.0;
            return 0.0;
        }
    }
    *x = 0.0;
    return 0.0;
}

void SimpleSectionRCncrRect::MRd(DoublePlus * ret, DoublePlus * NEd, DoublePlus * x, bool inverted){
    double xVal = 0.0;
    double MRd = MRdNormal( NEd->valueNormal(), &xVal, inverted  );
    x->setValueNormal( xVal );
    ret->setValueNormal( MRd );
}
