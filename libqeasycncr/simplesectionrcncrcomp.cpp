#include "simplesectionrcncrcomp.h"

#include "tablemodelplusprivate.h"
#include "cncrrectmodel.h"
#include "steelamodel.h"
#include "steelfepsmodel.h"
#include "slssteelsigmamodel.h"
#include "doubleplus.h"
#include "uintplus.h"
#include "unitmeasure.h"
#include "structmath.h"

#include <QVector>
#include <QPointF>
#include <cmath>

class SimpleSectionRCncrCompPrivate{
public:
    SimpleSectionRCncrCompPrivate( UnitMeasure * ump ):
        steelAModel( new SteelAModel( ump )),
        cncrRectModel( new CncrRectModel( ump )){
    }
    SteelAModel * steelAModel;
    CncrRectModel * cncrRectModel;
};

SimpleSectionRCncrComp::SimpleSectionRCncrComp(UnitMeasure *ump, const QString &n, QObject *parent) :
    VarPlusContainerPlus(ump, "SimpleSectionRCncrComp", n, parent ),
    m_d( new SimpleSectionRCncrCompPrivate(ump)){
    connect( m_d->steelAModel, SIGNAL(modelChanged()), this, SIGNAL(sectionChanged()) );
    initVar();
}

CncrRectModel *SimpleSectionRCncrComp::cncrRectModel() {
    return m_d->cncrRectModel;
}

SteelAModel *SimpleSectionRCncrComp::steelAModel() {
    return m_d->steelAModel;
}

void SimpleSectionRCncrComp::initVar() {
    connect( this, &SimpleSectionRCncrComp::containerChanged, this, &SimpleSectionRCncrComp::sectionChanged );

    hTot = new DoublePlus( 0.0, "hTot", m_unitMeasure, UnitMeasure::sectL );
    hTot->setRichName(trUtf8("h<span style=\" vertical-align:sub;\">tot</span>"));
    hTot->setToolTip( trUtf8("Altezza complessiva della sezione"));
    connect( m_d->cncrRectModel, SIGNAL(hTotChanged(double)), hTot, SLOT(setValueNormal(double)) );
    addVarToContainer(hTot);

    hG = new DoublePlus( 0.0, "hG", m_unitMeasure, UnitMeasure::sectL );
    hG->setRichName(trUtf8("h<span style=\" vertical-align:sub;\">G</span>"));
    hG->setToolTip( trUtf8("Distanza del baricentro della sezione in cls dal bordo superiore"));
    connect( m_d->cncrRectModel, SIGNAL(hGChanged(double)), hG, SLOT(setValueNormal(double)) );
    addVarToContainer(hG);

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
    epsCU2->setRichName( tr("ε<span style=\" vertical-align:sub;\">cu2</span>") );
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

    steelFEpsModel = new SteelFEpsModel( inclinedTopBranch, "steelFEpsModel", false );
    steelFEpsModel->setRichName( trUtf8("Legge σ-ε"));
    steelFEpsModel->setToolTip( trUtf8("Legge costitutiva dell'acciaio a SLU"));
    addVarToContainer( steelFEpsModel );

    NRdMin = new DoublePlus( 0.0, "NRdMin", m_unitMeasure, UnitMeasure::loadF );
    NRdMin->setRichName(trUtf8("N<span style=\" vertical-align:sub;\">Rd,min</span>"));
    NRdMin->setToolTip(trUtf8("Sforzo normale resistente minimo"));
    NRdMin->setReadOnly( true );
    addVarToContainer( NRdMin );
    connect( m_d->cncrRectModel, SIGNAL(modelChanged()), this, SLOT(setNRdMin()) );
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

void SimpleSectionRCncrComp::setFcd() {
    if( fcd->readOnly() ){
        double ret = 0.0;
        if( gammaC->valueNormal() != 0.0 ){
            ret = fck->valueNormal() * alphaCC->valueNormal() / gammaC->valueNormal();
        }
        fcd->setValueNormal( ret );
    }
}

void SimpleSectionRCncrComp::setFSyd() {
    if( fSyd->readOnly() ){
        double ret = 0.0;
        if( gammaS->valueNormal() != 0.0 ){
            ret = fSyk->valueNormal() / gammaS->valueNormal();
        }
        fSyd->setValueNormal( ret );
    }
}

void SimpleSectionRCncrComp::setFSud() {
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

void SimpleSectionRCncrComp::setNRdMin() {
    double N = - m_d->cncrRectModel->ANormal() * fcd->valueNormal();
    for( int i=0; i < m_d->steelAModel->count(); ++i ) {
        N += m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( -epsCU2->valueNormal());
    }
    NRdMin->setValueNormal( N );
}

void SimpleSectionRCncrComp::setNRdMax() {
    double N = 0.0;
    for( int i=0; i < m_d->steelAModel->count(); ++i ) {
        if( steelFEpsModel->valueNormal() == inclinedTopBranch ){
            N += m_d->steelAModel->A(i)->valueNormal() * fSud->valueNormal();
        } else {
            N += m_d->steelAModel->A(i)->valueNormal() * fSyd->valueNormal();
        }
    }
    NRdMax->setValueNormal( N );
}

void SimpleSectionRCncrComp::setEpsSUd() {
    if( epsSUd->readOnly() ){
        double ret = 0.9 * epsSUk->valueNormal();
        epsSUd->setValueNormal( ret );
    }
}

double SimpleSectionRCncrComp::beta1Normal( double eC ){
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

double SimpleSectionRCncrComp::beta2Normal( double eC ){
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

void SimpleSectionRCncrComp::setEpsC2( void ){
    if( epsC2->readOnly() ){
        if( (fck->valueNormal() * 1.0e-6) > 50.0 ){
            epsC2->setValueNormal( 0.002 + 0.000085 * pow( (fck->valueNormal() * 1.0e-6 - 50.0), 0.53) );
        } else {
            epsC2->setValueNormal( 0.002 );
        }
    }
}

void SimpleSectionRCncrComp::setEpsCU2( void ){
    if( epsCU2->readOnly() ){
        if( (fck->valueNormal() * 1.0e-6) >= 50.0 ){
            epsCU2->setValueNormal( 0.0026 + 0.035 * pow( ((90.0 - fck->valueNormal() * 1.0e-6 ) /100.0), 4.0 ) );
        } else {
            epsCU2->setValueNormal( 0.0035 );
        }
    }
}

void SimpleSectionRCncrComp::setNC( void ){
    if( nC->readOnly() ){
        if( (fck->valueNormal() * 1.0e-6) >= 50.0 ){
            nC->setValueNormal( 1.4 + 23.4 * pow( (90-fck->valueNormal()*1.0e-6)/100.0, 4.0) );
        } else {
            nC->setValueNormal(2.0);
        }
    }
}

double SimpleSectionRCncrComp::fCncrNormal( double eC ){
    if( eC < 0.0 && eC >= -epsCU2->valueNormal() ){
        if( eC <= -epsC2 ->valueNormal() ){
            return - fcd->valueNormal();
        } else if( epsC2->valueNormal() != 0.0){
            return - fcd->valueNormal() * ( 1.0 - pow( (1.0 + eC / epsC2->valueNormal()), nC->valueNormal()) );
        }
    }
    return 0.0;
}

double SimpleSectionRCncrComp::fSteelNormal( double eS ){
    if( Es->valueNormal() != 0.0 ){
        double eSYd = fSyd->valueNormal() / Es->valueNormal();
        if( fabs(eS) <= eSYd ){
            return Es->valueNormal() * eS;
        }
        double eSSgn = 1.0;
        if( eS < 0.0 ){
            eSSgn = -1.0;
            eSYd *= -1.0;
        }
        if( steelFEpsModel->value() == inclinedTopBranch ){
            if( fabs(eS) <= epsSUd->valueNormal() ){
                double D = epsSUd->valueNormal() - eSYd;
                if( D != 0.0 ){
                    return (eSSgn * fSyd->valueNormal()) + (eS - eSYd ) * (fSud->valueNormal() - fSyd->valueNormal() ) / D;
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

double SimpleSectionRCncrComp::NNormal( double eS, double eC, bool inverted ){
    double ret = 0.0;

    if( eS == eC ){
        // risultante cls
        ret += m_d->cncrRectModel->ANormal() * fCncrNormal( eC );
        // risultante armature acciaio
        for( int i=0; i < m_d->steelAModel->count(); ++i ) {
            ret += m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eS );
        }
    } else {
        // altezza utile sezione
        double dVal = 0.0;
        if( inverted ){
            dVal = hTot->valueNormal() - dMin->valueNormal();
        } else {
            dVal = dMax->valueNormal();
        }

        if( dVal != 0.0 ){
            if( eC < 0.0 ){
                double eCiSup = eC;
                double eCiInf = eC;
                if( inverted ){
                    for( int i=m_d->cncrRectModel->count()-1; i >-1; --i ){
                        if( dVal != 0.0 ){
                            eCiInf = eCiSup + m_d->cncrRectModel->t(i)->valueNormal() * (eS - eCiSup) / dVal;
                            double x = m_d->cncrRectModel->t(i)->valueNormal() * (-eCiSup) / (eCiInf - eCiSup);
                            if( x > 0.0 ){
                                if( x > m_d->cncrRectModel->t(i)->valueNormal() ){
                                    // deformazione bordo inferiore compresso
                                    double eCP = (1 - m_d->cncrRectModel->t(i)->valueNormal() / x ) * eCiSup;
                                    ret -= fcd->valueNormal() * m_d->cncrRectModel->b(i)->valueNormal() * (x * beta1Normal(eCiSup) - (x - m_d->cncrRectModel->t(i)->valueNormal() ) * beta1Normal(eCP));
                                } else {
                                    ret -= fcd->valueNormal() * m_d->cncrRectModel->b(i)->valueNormal() * x * beta1Normal(eCiSup);
                                }
                            }
                            if( eCiInf >= 0.0 ){
                                break;
                            }
                        }
                        eCiSup = eCiInf;
                        dVal -= m_d->cncrRectModel->t(i)->valueNormal();
                    }
                } else {
                    for( int i=0; i < m_d->cncrRectModel->count(); ++i ){
                        if( dVal != 0.0 ){
                            eCiInf = eCiSup + m_d->cncrRectModel->t(i)->valueNormal() * (eS - eCiSup) / dVal;
                            double x = m_d->cncrRectModel->t(i)->valueNormal() * (-eCiSup) / (eCiInf - eCiSup);

                            if( x > 0.0 ){
                                if( x > m_d->cncrRectModel->t(i)->valueNormal() ){
                                    // deformazione bordo inferiore compresso
                                    double eCP = (1 - m_d->cncrRectModel->t(i)->valueNormal() / x ) * eCiSup;
                                    ret -= fcd->valueNormal() * m_d->cncrRectModel->b(i)->valueNormal() * (x * beta1Normal(eCiSup) - (x - m_d->cncrRectModel->t(i)->valueNormal() ) * beta1Normal(eCP));
                                } else {
                                    ret -= fcd->valueNormal() * m_d->cncrRectModel->b(i)->valueNormal() * x * beta1Normal(eCiSup);
                                }
                            }
                            if( eCiInf >= 0.0 ){
                                break;
                            }
                        }
                        eCiSup = eCiInf;
                        dVal -= m_d->cncrRectModel->t(i)->valueNormal();
                    }
                }
            }

            if( inverted ){
                dVal = hTot->valueNormal() - dMin->valueNormal();
            } else {
                dVal = dMax->valueNormal();
            }
            for( int i=0; i < m_d->steelAModel->count(); ++i ) {
                // Altezza utile armatura
                double di = 0.0;
                if( inverted ){
                    di = hTot->valueNormal() - m_d->steelAModel->d(i)->valueNormal();
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
                ret += m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eSi );
            }
        }
    }
    return ret;
}

double SimpleSectionRCncrComp::MNormal( double eS, double eC, bool inverted ){
    double ret = 0.0;

    if( eS == eC ){
        // momento nullo della sezione in cls rispetto al suo baricentro

        // momento delle armature
        for( int i=0; i < m_d->steelAModel->count(); ++i ) {
            if( inverted ){
                double hGVal = hTot->valueNormal() - hG->valueNormal();
                // altezza utile armatura
                double dVal = hTot->valueNormal() - m_d->steelAModel->d(i)->valueNormal();
                ret -= m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eS ) * (dVal - hGVal ) ;
            } else {
                double hGVal = hG->valueNormal();
                // altezza utile armatura
                double dVal = m_d->steelAModel->d(i)->valueNormal();
                ret += m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eS ) * (dVal - hGVal ) ;
            }
        }
    } else {
        // altezza utile sezione
        double dVal = 0.0;
        if( inverted ){
            dVal = hTot->valueNormal() - dMin->valueNormal();
        } else {
            dVal = dMax->valueNormal();
        }

        if( dVal != 0.0 ){
            if( eC < 0.0 ){
                double eCiSup = eC;
                double eCiInf = eC;
                double hGVal = hG->valueNormal();
                if( inverted ){
                    hGVal = hTot->valueNormal() - hG->valueNormal();
                    double hSup = 0.0;
                    for( int i=m_d->cncrRectModel->count()-1; i >-1; --i ){
                        eCiInf = eCiSup + m_d->cncrRectModel->t(i)->valueNormal() * (eS - eCiSup) / (dVal-hSup);
                        double x = m_d->cncrRectModel->t(i)->valueNormal() * (-eCiSup) / (eCiInf - eCiSup);

                        if( x > 0.0 ){
                            if( x > m_d->cncrRectModel->t(i)->valueNormal() ){
                                // deformazione bordo inferiore compresso
                                double eCP = (1 - m_d->cncrRectModel->t(i)->valueNormal() / x ) * eCiSup;
                                double M1 = fcd->valueNormal() * m_d->cncrRectModel->b(i)->valueNormal() * (x * beta1Normal(eCiSup) );
                                M1 *= (hGVal - hSup - x * beta2Normal(eCiSup) );
                                double M2 = fcd->valueNormal() * m_d->cncrRectModel->b(i)->valueNormal() * (x - m_d->cncrRectModel->t(i)->valueNormal() ) * beta1Normal(eCP);
                                M2 *= (hGVal - (hSup+m_d->cncrRectModel->t(i)->valueNormal()) - (x - m_d->cncrRectModel->t(i)->valueNormal() ) * beta2Normal(eCP) );
                                ret -= M1 - M2;
                            } else {
                                double M1 = fcd->valueNormal() * m_d->cncrRectModel->b(i)->valueNormal() * (x * beta1Normal(eCiSup) );
                                M1 *= (hGVal - hSup - x * beta2Normal(eCiSup) );
                                ret -= M1;
                            }
                        }
                        if( eCiInf >= 0.0 ){
                            break;
                        }
                        eCiSup = eCiInf;
                        hSup += m_d->cncrRectModel->t(i)->valueNormal();
                    }
                } else {
                    double hSup = 0.0;
                    for( int i=0; i < m_d->cncrRectModel->count(); ++i ){
                        eCiInf = eCiSup + m_d->cncrRectModel->t(i)->valueNormal() * (eS - eCiSup) / (dVal-hSup);
                        double x = m_d->cncrRectModel->t(i)->valueNormal() * (-eCiSup) / (eCiInf - eCiSup);

                        if( x > 0.0 ){
                            if( x > m_d->cncrRectModel->t(i)->valueNormal() ){
                                // deformazione bordo inferiore compresso
                                double eCP = (1 - m_d->cncrRectModel->t(i)->valueNormal() / x ) * eCiSup;
                                double M1 = fcd->valueNormal() * m_d->cncrRectModel->b(i)->valueNormal() * (x * beta1Normal(eCiSup) );
                                M1 *= (hGVal - hSup - x * beta2Normal(eCiSup) );
                                double M2 = fcd->valueNormal() * m_d->cncrRectModel->b(i)->valueNormal() * (x - m_d->cncrRectModel->t(i)->valueNormal() ) * beta1Normal(eCP);
                                M2 *= (hGVal - (hSup+m_d->cncrRectModel->t(i)->valueNormal()) - (x - m_d->cncrRectModel->t(i)->valueNormal() ) * beta2Normal(eCP) );
                                ret += M1 - M2;
                            } else {
                                double M1 = fcd->valueNormal() * m_d->cncrRectModel->b(i)->valueNormal() * (x * beta1Normal(eCiSup) );
                                M1 *= (hGVal - hSup - x * beta2Normal(eCiSup) );
                                ret += M1;
                            }
                        }
                        if( eCiInf >= 0.0 ){
                            break;
                        }
                        eCiSup = eCiInf;
                        hSup += m_d->cncrRectModel->t(i)->valueNormal();
                    }
                }
            }

            // momento armature
            for( int i=0; i < m_d->steelAModel->count(); ++i ) {
                if( inverted ){
                    double hGVal = hTot->valueNormal() - hG->valueNormal();
                    // altezza utile armatura
                    double di = hTot->valueNormal() - m_d->steelAModel->d(i)->valueNormal();
                    // deformazione armatura
                    /* trick necessario per evitare che eSi diventi maggiore di eSud per
                           problemi di approssimazione */
                    double eSi = eS;
                    if( di < dVal ) {
                        eSi = eC + (eS - eC ) * di / dVal;
                    } else if( di > dVal ){
                        eSi = 0.0;
                    }
                    ret -= m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eSi ) * (di - hGVal );
                } else {
                    double hGVal = hG->valueNormal();
                    // altezza utile armatura
                    double di = m_d->steelAModel->d(i)->valueNormal();
                    // deformazione armatura
                    /* trick necessario per evitare che eSi diventi maggiore di eSud per
                           problemi di approssimazione */
                    double eSi = eS;
                    if( di < dVal ) {
                        eSi = eC + (eS - eC ) * di / dVal;
                    } else if( di > dVal ){
                        eSi = 0.0;
                    }
                    ret += m_d->steelAModel->A(i)->valueNormal() * fSteelNormal( eSi ) * (di - hGVal );
                }
            }
        }
    }

    return ret;
}

QVector<QPointF> SimpleSectionRCncrComp::NEpsConfigurations1(int nP ) {
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

QVector<QPointF> SimpleSectionRCncrComp::NEpsConfigurations23( int nP) {
    QVector<QPointF> conf;
    if(nP > 0 ){
        double eS = epsSUd->valueNormal();
        double eC = -epsCU2->valueNormal();

        double eS3 = 0.0;
        if( hTot->valueNormal() != 0.0 ){
            eS3 = - epsCU2->valueNormal() * (1.0 - dMax->valueNormal()  / hTot->valueNormal() );
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

QVector<QPointF> SimpleSectionRCncrComp::NEpsConfigurations4( int nP) {
    QVector<QPointF> conf;

    if( epsCU2->valueNormal() > epsC2->valueNormal() ){
        double eC = -epsCU2->valueNormal();
        double eS3 = 0.0;
        if( hTot->valueNormal() != 0.0 ){
            eS3 = - epsCU2->valueNormal() * (1.0 - dMax->valueNormal()  / hTot->valueNormal() );
        }
        double eS = eS3;
        double de = (eS3 + epsC2->valueNormal()) / double(nP);

        if( epsCU2->valueNormal() != 0.0 ){
            double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * hTot->valueNormal();
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

QVector<QPointF> SimpleSectionRCncrComp::MChiDiagram( DoublePlus * NEdPlus, int nP, double prec, int maxIter ) {
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
            double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * hTot->valueNormal();
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

QVector<QPointF> SimpleSectionRCncrComp::NRdMRd(int nP) {
    QVector<QPointF> conf;

    if(nP > 0 ){
        if( steelFEpsModel->valueNormal() == inclinedTopBranch ){
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
            if( hTot->valueNormal() != 0.0 ){
                eS3 = - epsCU2->valueNormal() * (1.0 - dMax->valueNormal()  / hTot->valueNormal() );
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
                double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * hTot->valueNormal();
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
            if( hTot->valueNormal() != 0.0 ){
                eS3 = - epsCU2->valueNormal() * (1.0 - (hTot->valueNormal()-dMin->valueNormal()) / hTot->valueNormal() );
            }

            // campo 4 al contrario
            if( epsCU2->valueNormal() != 0.0 ){
                eS = - epsC2->valueNormal();
                eC = - epsC2->valueNormal();
                de = (eS3 + epsC2->valueNormal()) / double(nP);
                double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * hTot->valueNormal();
                double xS = (hTot->valueNormal() - dMin->valueNormal()) - xC;
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
            if( hTot->valueNormal() != 0.0 ){
                eS3 = - epsCU2->valueNormal() * (1.0 - dMax->valueNormal()  / hTot->valueNormal() );
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
                double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * hTot->valueNormal();
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
            if( hTot->valueNormal() != 0.0 ){
                eS3 = - epsCU2->valueNormal() * (1.0 - (hTot->valueNormal() - dMin->valueNormal())  / hTot->valueNormal() );
            }

            // campo 4 al contrario
            if( epsCU2->valueNormal() != 0.0 ){
                eS = - epsC2->valueNormal();
                eC = - epsC2->valueNormal();
                de = (eS3 + epsC2->valueNormal()) / double(nP);
                double xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * hTot->valueNormal();
                double xS = (hTot->valueNormal() - dMin->valueNormal()) - xC;
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

void SimpleSectionRCncrComp::SLSNM(DoublePlus * N, DoublePlus * M, DoublePlus * alphaE,
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

double SimpleSectionRCncrComp::ULSVNoSteelNormal(double bw, double dw, double Asl, double scp ) {
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

void SimpleSectionRCncrComp::ULSVNoSteel( DoublePlus * V, DoublePlus * bw, DoublePlus * dw, DoublePlus * Asl, DoublePlus * scp ) {
    V->setValueNormal( ULSVNoSteelNormal(bw->valueNormal(), dw->valueNormal(), Asl->valueNormal(), scp->valueNormal() ));
}

void SimpleSectionRCncrComp::SLSNMNormal(double N, double M, double alphaE, double sigmaS[], double *sigmaCUp, double *sigmaCDown, double *x) {
    if( N == 0.0 ){
        if( M > 0.0 ){
            // flessione semplice positiva
            // WORKING
            double Sti = 0.0;
            double Sbiti = 0.0;
            double Sbiti2 = 0.0;
            // m: indice della sezione analizzata
            for( int m = 0; m < m_d->cncrRectModel->count(); ++m ){
                double SbitiStj = 0.0;
                for( int i=0; i < m; ++i){  // i < m <=> i <= m-1
                    double Stj = 0.0;
                    for( int j=i+1; j < m; ++j){ // j < m <=> j <= m-1
                        Stj += m_d->cncrRectModel->t(j)->valueNormal();
                    }
                    SbitiStj += m_d->cncrRectModel->b(i)->valueNormal() * m_d->cncrRectModel->t(i)->valueNormal() * Stj;
                }
                double a1 = m_d->cncrRectModel->b(m)->valueNormal() / 2.0;
                double b1 = Sbiti + alphaE * m_d->steelAModel->SANormal();
                double c1 = SbitiStj + Sbiti2 / 2.0 - alphaE * (m_d->steelAModel->SAdNormal() - m_d->steelAModel->SANormal() * Sti );
                double xReal[2];
                double xCmp[2];
                solvePoly2( xReal, xCmp, a1, b1, c1 );

                for( int i=0; i < 2; ++i){
                    if( xCmp[i] == 0.0 && xReal[i] > 0.0 && xReal[i] <= m_d->cncrRectModel->t(m)->valueNormal() ){
                        double xTmp = xReal[i];
                        double I0=m_d->cncrRectModel->b(m)->valueNormal() * pow(xTmp, 3.0) / 3.0;
                        double hInf = xTmp;
                        for( int j=m-1; j >-1; --j){
                            double tj = m_d->cncrRectModel->t(j)->valueNormal();
                            I0 += m_d->cncrRectModel->b(j)->valueNormal() * ( pow(tj, 3.0) / 12.0 + tj * pow((hInf + tj/2.0), 2.0) );
                            hInf += tj;
                        }
                        xTmp = Sti + xTmp;
                        I0 += alphaE * ( m_d->steelAModel->SAd2Normal() - 2.0 * m_d->steelAModel->SAdNormal() * xTmp + m_d->steelAModel->SANormal() * pow(xTmp, 2.0) );
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
                Sti += m_d->cncrRectModel->t(m)->valueNormal();
                Sbiti += m_d->cncrRectModel->b(m)->valueNormal() * m_d->cncrRectModel->t(m)->valueNormal();
                Sbiti2 += m_d->cncrRectModel->b(m)->valueNormal() * pow(m_d->cncrRectModel->t(m)->valueNormal(), 2.0);
            }
        } else if( M < 0.0 ){
            // flessione semplice negativa
            // WORKING
            double Sti = 0.0;
            double Sbiti = 0.0;
            double Sbiti2 = 0.0;
            // m: indice della sezione analizzata
            for( int m = (m_d->cncrRectModel->count()-1); m > -1 ; --m ){
                double SbitiStj = 0.0;
                for( int i = (m_d->cncrRectModel->count()-1); i > m; --i){  // i < m <=> i <= m-1
                    double Stj = 0.0;
                    for( int j=i-1; j > m; --j){ // j < m <=> j <= m-1
                        Stj += m_d->cncrRectModel->t(j)->valueNormal();
                    }
                    SbitiStj += m_d->cncrRectModel->b(i)->valueNormal() * m_d->cncrRectModel->t(i)->valueNormal() * Stj;
                }
                double a1 = m_d->cncrRectModel->b(m)->valueNormal() / 2.0;
                double b1 = Sbiti + alphaE * m_d->steelAModel->SANormal();
                double c1 = SbitiStj + Sbiti2 / 2.0 - alphaE * (m_d->steelAModel->SAdNormal() - m_d->steelAModel->SANormal() * Sti );
                double xReal[2];
                double xCmp[2];
                solvePoly2( xReal, xCmp, a1, b1, c1 );

                for( int i=0; i < 2; ++i){
                    if( xCmp[i] == 0.0 && xReal[i] > 0.0 && xReal[i] <= m_d->cncrRectModel->t(m)->valueNormal() ){
                        double xTmp = xReal[i];
                        double I0=m_d->cncrRectModel->b(m)->valueNormal() * pow(xTmp, 3.0) / 3.0;
                        double hInf = xTmp;
                        for( int j=m+1; j <m_d->cncrRectModel->count(); ++j){
                            double tj = m_d->cncrRectModel->t(j)->valueNormal();
                            I0 += m_d->cncrRectModel->b(j)->valueNormal() * ( pow(tj, 3.0) / 12.0 + tj * pow((hInf + tj/2.0), 2.0) );
                            hInf += tj;
                        }
                        xTmp = Sti + xTmp;
                        double xTmpRevert = hTot->valueNormal() - xTmp;
                        I0 += alphaE * ( m_d->steelAModel->SAd2Normal() - 2.0 * m_d->steelAModel->SAdNormal() * xTmpRevert + m_d->steelAModel->SANormal() * pow(xTmpRevert, 2.0) );
                        if( I0 != 0.0 ){
                            *x = xTmpRevert;
                            *sigmaCUp = 0.0;
                            *sigmaCDown = M * xTmp / I0;
                            for( int i=0; i<m_d->steelAModel->count(); ++i){
                                sigmaS[i] = - alphaE * M * ( m_d->steelAModel->d(i)->valueNormal() - xTmpRevert) / I0;
                            }
                            return;
                        }
                    }
                }
                Sti += m_d->cncrRectModel->t(m)->valueNormal();
                Sbiti += m_d->cncrRectModel->b(m)->valueNormal() * m_d->cncrRectModel->t(m)->valueNormal();
                Sbiti2 += m_d->cncrRectModel->b(m)->valueNormal() * pow(m_d->cncrRectModel->t(m)->valueNormal(), 2.0);
            }
        }
    } else {
        // pressoflessione/tensoflessione

        // verifichiamo se la sezione è interamente reagente
        double sCUp = 0.0, sCDown = 0.0;
        double sS[m_d->steelAModel->count()];
        double xTmp = calcSigmaNormal( &sCUp, &sCDown, sS, N, M, m_d->cncrRectModel->count(), 0.0, alphaE, false );
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
        xTmp = calcSigmaNormal( &sCUp, &sCDown, sS, N, M, 0, 0.0, alphaE, false );
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
            double Sti = 0.0;
            double Sbiti = 0.0;
            double Sbiti2 = 0.0;
            double Sbiti3 = 0.0;
            for( int m = 0; m < m_d->cncrRectModel->count(); ++m ){
                double SbitiStj = 0.0;
                double Sbiti2Stj = 0.0;
                double SbitiStj2 = 0.0;
                for( int i=0; i < m; ++i){  // i < m <=> i <= m-1
                    double Stj = 0.0;
                    for( int j=i+1; j < m; ++j){ // j < m <=> j <= m-1
                        Stj += m_d->cncrRectModel->t(j)->valueNormal();
                    }
                    SbitiStj += m_d->cncrRectModel->b(i)->valueNormal() * m_d->cncrRectModel->t(i)->valueNormal() * Stj;
                    SbitiStj2 += m_d->cncrRectModel->b(i)->valueNormal() * m_d->cncrRectModel->t(i)->valueNormal() * pow(Stj, 2.0);
                    Sbiti2Stj += m_d->cncrRectModel->b(i)->valueNormal() * pow(m_d->cncrRectModel->t(i)->valueNormal(), 2.0) * Stj;
                }
                double a1 = N * m_d->cncrRectModel->b(m)->valueNormal() / 6.0;
                double b1 = - (m_d->cncrRectModel->b(m)->valueNormal() / 2.0) * (M + N * hG->valueNormal() - N * Sti);
                double c1 = - ( (M + N * hG->valueNormal()) * (Sbiti+alphaE*m_d->steelAModel->SANormal())
                                + N * (Sbiti2 / 2.0 + SbitiStj - Sti * Sbiti - alphaE * m_d->steelAModel->SAdNormal() ) );
                double d1 = - (M + N * hG->valueNormal()) * (SbitiStj + Sbiti2 / 2.0 - alphaE * m_d->steelAModel->SAdNormal() + alphaE * m_d->steelAModel->SANormal() * Sti )
                        + N * ( Sti * (SbitiStj + Sbiti2 / 2.0) - SbitiStj2 - Sbiti2Stj - Sbiti3 / 3.0
                                - alphaE * m_d->steelAModel->SAd2Normal() + alphaE * m_d->steelAModel->SAdNormal() * Sti);
                double xReal[3], xIm[3];
                solvePoly3( xReal, xIm, a1, b1, c1, d1);

                for( int i=0; i < 3; ++i){
                    if( xIm[i] == 0.0 && xReal[i] > 0.0 && xReal[i] < m_d->cncrRectModel->t(m)->valueNormal() ){
                        *x = calcSigmaNormal( &sCUp, &sCDown, sS, N, M, m, xReal[i], alphaE, false );
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
                Sti += m_d->cncrRectModel->t(m)->valueNormal();
                Sbiti += m_d->cncrRectModel->b(m)->valueNormal() * m_d->cncrRectModel->t(m)->valueNormal();
                Sbiti2 += m_d->cncrRectModel->b(m)->valueNormal() * pow(m_d->cncrRectModel->t(m)->valueNormal(), 2.0);
                Sbiti3 += m_d->cncrRectModel->b(m)->valueNormal() * pow(m_d->cncrRectModel->t(m)->valueNormal(), 3.0);
            }
        } else {
            // bordo inferiore compresso
            double Sti = 0.0;
            double Sbiti = 0.0;
            double Sbiti2 = 0.0;
            double Sbiti3 = 0.0;

            for( int m = m_d->cncrRectModel->count()-1; m > -1 ; --m ){
                double SbitiStj = 0.0;
                double Sbiti2Stj = 0.0;
                double SbitiStj2 = 0.0;
                for( int i = (m_d->cncrRectModel->count()-1); i > m; --i){  // i < m <=> i <= m-1
                    double Stj = 0.0;
                    for( int j=i-1; j > m; --j){ // j < m <=> j <= m-1
                        Stj += m_d->cncrRectModel->t(j)->valueNormal();
                    }
                    SbitiStj += m_d->cncrRectModel->b(i)->valueNormal() * m_d->cncrRectModel->t(i)->valueNormal() * Stj;
                    SbitiStj2 += m_d->cncrRectModel->b(i)->valueNormal() * m_d->cncrRectModel->t(i)->valueNormal() * pow(Stj, 2.0);
                    Sbiti2Stj += m_d->cncrRectModel->b(i)->valueNormal() * pow(m_d->cncrRectModel->t(i)->valueNormal(), 2.0) * Stj;
                }

                double a1 = N * m_d->cncrRectModel->b(m)->valueNormal() / 6.0;
                double b1 = - (m_d->cncrRectModel->b(m)->valueNormal() / 2.0) * (M + N * hG->valueNormal() - N * Sti);
                double c1 = - ( (M + N * hG->valueNormal()) * (Sbiti+alphaE*m_d->steelAModel->SANormal())
                                + N * (Sbiti2 / 2.0 + SbitiStj - Sti * Sbiti - alphaE * m_d->steelAModel->SAdNormal() ) );
                double d1 = - (M + N * hG->valueNormal()) * (SbitiStj + Sbiti2 / 2.0 - alphaE * m_d->steelAModel->SAdNormal() + alphaE * m_d->steelAModel->SANormal() * Sti )
                        + N * ( Sti * (SbitiStj + Sbiti2 / 2.0) - SbitiStj2 - Sbiti2Stj - Sbiti3 / 3.0
                                - alphaE * m_d->steelAModel->SAd2Normal() + alphaE * m_d->steelAModel->SAdNormal() * Sti);
                double xReal[3], xIm[3];
                solvePoly3( xReal, xIm, a1, b1, c1, d1);

                for( int i=0; i < 3; ++i){
                    if( xIm[i] == 0.0 && xReal[i] > 0.0 && xReal[i] < m_d->cncrRectModel->t(m)->valueNormal() ){
                        xTmp = calcSigmaNormal( &sCUp, &sCDown, sS, N, M, m, xReal[i], alphaE, true );
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
                Sti += m_d->cncrRectModel->t(m)->valueNormal();
                Sbiti += m_d->cncrRectModel->b(m)->valueNormal() * m_d->cncrRectModel->t(m)->valueNormal();
                Sbiti2 += m_d->cncrRectModel->b(m)->valueNormal() * pow(m_d->cncrRectModel->t(m)->valueNormal(), 2.0);
                Sbiti3 += m_d->cncrRectModel->b(m)->valueNormal() * pow(m_d->cncrRectModel->t(m)->valueNormal(), 3.0);
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

double SimpleSectionRCncrComp::calcSigmaNormal( double *sigmaCUp, double *sigmaCDown, double sigmaS[], double N, double M, int sectI, double xi, double alphaE, bool invert ){
    if( invert ){
        if( sectI < 0 ){
            sectI = 0;
            xi = m_d->cncrRectModel->t(0)->valueNormal();
        }
        if( xi < 0.0 ){
            xi = 0.0;
        }
        if( xi > m_d->cncrRectModel->t(sectI)->valueNormal() ){
            xi = m_d->cncrRectModel->t(sectI)->valueNormal();
        }

        double A0 = 0.0;
        for( int i=(m_d->cncrRectModel->count()-1); i > sectI ; --i ){
            A0 += m_d->cncrRectModel->b(i)->valueNormal() * m_d->cncrRectModel->t(i)->valueNormal();
        }
        if( sectI < m_d->cncrRectModel->count() ){
            A0 += m_d->cncrRectModel->b(sectI)->valueNormal() * xi + alphaE * m_d->steelAModel->SANormal();
        }
        A0 += alphaE * m_d->steelAModel->SANormal();

        if( A0 != 0.0 ){
            double hInf = 0.0;
            double S0 = 0.0;
            for( int i=(m_d->cncrRectModel->count()-1); i > sectI ; --i ){
                double ti = m_d->cncrRectModel->t(i)->valueNormal();
                S0 += m_d->cncrRectModel->b(i)->valueNormal() * ti * (hInf + ti / 2.0);
                hInf += ti;
            }
            if( sectI < m_d->cncrRectModel->count() ){
                S0 += m_d->cncrRectModel->b(sectI)->valueNormal() * xi * (hInf + xi / 2.0);
            }
            S0 += alphaE * ( m_d->steelAModel->SANormal() * hTot->valueNormal() - m_d->steelAModel->SAdNormal() );

            // distanza del baricentro dal bordo inferiore compresso
            double hG = S0 / A0;

            hInf = 0.0;
            double I0 = 0.0;
            for( int i=m_d->cncrRectModel->count()-1; i > sectI ; --i ){
                double ti = m_d->cncrRectModel->t(i)->valueNormal();
                I0 += m_d->cncrRectModel->b(i)->valueNormal() * ( pow(ti,3.0) / 12.0 + ti * pow( hG - (hInf + ti / 2.0), 2.0) );
                hInf += m_d->cncrRectModel->t(i)->valueNormal();
            }
            if( sectI < m_d->cncrRectModel->count() ){
                I0 += m_d->cncrRectModel->b(sectI)->valueNormal() * ( pow(xi,3.0) / 12.0 + xi * pow( hG - (hInf + xi / 2.0), 2.0) );
            }
            I0 += alphaE * ( m_d->steelAModel->SAd2Normal()
                             - 2.0 * ( (hTot->valueNormal() - hG) * m_d->steelAModel->SAdNormal() )
                             + pow(hTot->valueNormal()-hG, 2.0) * m_d->steelAModel->SANormal() );

            if( I0!= 0.0 ){
                double M0 = M - N * ( hTot->valueNormal() - (hG + m_d->cncrRectModel->hGNormal()) );
                *sigmaCUp =  N / A0 - M0 / I0 * (hTot->valueNormal() - hG);
                *sigmaCDown = N / A0 + M0 / I0 * hG;
                for( int i=0; i < m_d->steelAModel->count(); ++i ){
                    sigmaS[i] = alphaE * (N / A0 + M0 / I0 *
                                          ( m_d->steelAModel->d(i)->valueNormal() - (hTot->valueNormal() - hG)));
                }
                if( M0 != 0.0 ){
                    return hG - I0 * N / (A0 * M0);
                } else {
                    return 0.0;
                }
            }
        }
    } else {
        if( sectI > m_d->cncrRectModel->count()-1 ){
            sectI = m_d->cncrRectModel->count()-1;
            xi = m_d->cncrRectModel->t(sectI)->valueNormal();
        }
        if( xi < 0.0 ){
            xi = 0.0;
        }
        if( sectI > -1 ){
            if( xi > m_d->cncrRectModel->t(sectI)->valueNormal() ){
                xi = m_d->cncrRectModel->t(sectI)->valueNormal();
            }
        }

        double A0 = 0.0;
        for( int i=0; i < sectI; ++i ){
            A0 += m_d->cncrRectModel->b(i)->valueNormal() * m_d->cncrRectModel->t(i)->valueNormal();
        }
        if( sectI > -1 ){
            A0 += m_d->cncrRectModel->b(sectI)->valueNormal() * xi;
        }
        A0 += alphaE * m_d->steelAModel->SANormal();

        if( A0 != 0.0 ){
            double S0 = 0.0;
            double hSup = 0.0;
            for( int i=0; i < sectI; ++ i ){
                double ti = m_d->cncrRectModel->t(i)->valueNormal();
                S0 += m_d->cncrRectModel->b(i)->valueNormal() * ti * (hSup + ti / 2.0);
                hSup += ti;
            }
            if( sectI > -1 ){
                S0 += m_d->cncrRectModel->b(sectI)->valueNormal() * xi * (hSup + xi / 2.0);
            }
            S0 += alphaE * m_d->steelAModel->SAdNormal();

            double hG = S0 / A0;
            double I0 = 0.0;
            hSup = 0.0;
            for( int i=0; i < sectI; ++ i ){
                double ti = m_d->cncrRectModel->t(i)->valueNormal();
                I0 += m_d->cncrRectModel->b(i)->valueNormal() * (pow(ti,3.0) / 12.0 + ti*pow((hG-hSup-ti/2.0),2.0) );
                hSup += ti;
            }
            if( sectI > -1 ){
                I0 += m_d->cncrRectModel->b(sectI)->valueNormal() * (pow(xi,3.0) / 12.0 + xi*pow((hG-hSup-xi/2.0),2.0) );
            }
            I0 += alphaE * ( m_d->steelAModel->SAd2Normal()
                             - 2.0 * hG * m_d->steelAModel->SAdNormal()
                             + pow(hG,2.0) * m_d->steelAModel->SANormal());
            if( I0!= 0.0 ){
                double M0 = M + N * ( m_d->cncrRectModel->hGNormal() - hG );
                *sigmaCUp = N / A0 - M0 / I0 * hG;
                *sigmaCDown = N / A0 + M0 / I0 * (hTot->valueNormal() - hG);
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
        *sigmaCUp = 0.0;
        *sigmaCDown = 0.0;
        for( int i=0; i < m_d->steelAModel->count(); ++i ){
            sigmaS[i] = 0.0;
        }
    }
    return 0.0;
}

double SimpleSectionRCncrComp::MRdNormal(double NEd, double * x, bool inverted, double prec, int maxIter ){
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
            dVal = hTot->valueNormal() - dMin->valueNormal();
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
            if( steelFEpsModel->valueNormal() == inclinedTopBranch ){
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
        if( hTot->valueNormal() != 0.0 ){
            eS3 = - epsCU2->valueNormal() * (1.0 - dVal  / hTot->valueNormal() );
        }

        double eS = 0.0;
        if( hTot->valueNormal() != 0.0 ){
            eS = - epsCU2->valueNormal() * (1.0 - dVal  / hTot->valueNormal() );
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
            xC = ( 1.0 - epsC2->valueNormal() / epsCU2->valueNormal()) * hTot->valueNormal();
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

void SimpleSectionRCncrComp::MRd(DoublePlus * ret, DoublePlus * NEd, DoublePlus * x, bool inverted){
    double xVal = 0.0;
    double MRd = MRdNormal( NEd->valueNormal(), &xVal, inverted  );
    x->setValueNormal( xVal );
    ret->setValueNormal( MRd );
}
