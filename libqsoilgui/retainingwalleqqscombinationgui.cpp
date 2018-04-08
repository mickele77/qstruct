#include "retainingwalleqqscombinationgui.h"

#include "retainingwallqscombinationguiprivate.h"
#include "ui_retainingwallqscombinationgui.h"

#include "retainingwall.h"
#include "varpluscontainer.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "boolplus.h"

#include <QMimeData>
#include <QClipboard>

#include <cmath>

class RetainingWallEQQSCombinationGUIPrivate: public VarPlusContainer {
public:
    RetainingWallEQQSCombinationGUIPrivate( UnitMeasure * ump,
                                            RWEarthQuakeDirection * EQDir,
                                            DoublePlus * kkv,
                                            DoublePlus * kkh,
                                            DoublePlus * t,
                                            DoublePlus * psi ):
        VarPlusContainer( ump, "", "" ),
        EQDirection( EQDir ),
        kv( kkv ),
        kh( kkh ),
        EQtheta(t),
        psi2Q(psi){
    }
    RWEarthQuakeDirection * EQDirection;
    DoublePlus * kv;
    DoublePlus * kh;
    DoublePlus * EQtheta;
    DoublePlus * psi2Q;
};

RetainingWallEQQSCombinationGUI::RetainingWallEQQSCombinationGUI(UnitMeasure * ump,
                                                                 const QString & cn,
                                                                 RWEarthQuakeDirection * EQDirection,
                                                                 DoublePlus * kv,
                                                                 DoublePlus * kh,
                                                                 DoublePlus * theta,
                                                                 DoublePlus * Gk2, DoublePlus * Qk, DoublePlus * psi2Q,
                                                                 DoublePlus * gammaTanPhiPrime, DoublePlus * gammaCPrime, DoublePlus *gammaSliding, DoublePlus *gammaQLim,
                                                                 RetainingWallGUI *parent) :
    RetainingWallQSCombinationGUI(ump,
                                  cn,
                                  Gk2, Qk,
                                  gammaTanPhiPrime, gammaCPrime,
                                  NULL, NULL,
                                  NULL, NULL,
                                  NULL, NULL,
                                  gammaSliding, gammaQLim,
                                  parent),
    m_dd( new RetainingWallEQQSCombinationGUIPrivate( ump, EQDirection, kv, kh, theta, psi2Q ) ){
    connect( m_ui->copyClipPButton, SIGNAL(clicked()), this, SLOT(copyClip()));
}

void RetainingWallEQQSCombinationGUI::calculateQSCombination() {
    if( m_d->retainingWall ){
        m_d->retainingWall->calculatePhiPrimeD( m_d->SoilDownPhiPrimeD, m_d->SoilUpPhiPrimeD, m_d->gammaTanPhiPrime );
        m_d->retainingWall->calculateGammaWD( m_d->QLimSoilDownGammaWPrimeD, NULL );
        m_d->retainingWall->calculateCPrimeD( m_d->SoilDownCPrimeD, m_d->SoilUpCPrimeD, m_d->gammaCPrime );
        m_d->retainingWall->calculateDeltaD( m_d->SoilDownDeltaD, m_d->SoilUpDeltaD, m_d->gammaTanPhiPrime );
        m_d->retainingWall->calculateEQK( m_dd->EQtheta, m_d->SoilUpPhiPrimeD, m_d->SoilUpDeltaD, m_d->Ka, m_d->SoilDownPhiPrimeD, m_d->SoilDownDeltaD, m_d->Kp, m_d->ChiKp, m_d->KpRed );

        m_d->retainingWall->calculateEQSlidingVH( m_d->SlidingVEd, m_d->SlidingHEd, m_d->SlidingHRdP, m_d->SoilDownCPrimeD, m_d->SoilUpCPrimeD, m_d->Ka, m_d->KpRed, m_dd->EQDirection, m_dd->kv, m_dd->kh, m_d->Gk2, m_d->Qk, m_dd->psi2Q );
        m_d->retainingWall->calculateSliding( m_d->SlidingHRd, m_d->SlidingApplyKp, m_d->SlidingHRdP, m_d->SlidingHEd, m_d->SlidingVEd, m_d->SoilDownDeltaD, m_d->gammaSliding );

        if( m_d->QLimMEdC->y->readOnly() ){
            m_d->QLimMEdC->y->setValueNormal( 0.50 * (-(m_d->retainingWall->stemTBottom->valueNormal() + m_d->retainingWall->toeB->valueNormal()) + m_d->retainingWall->heelB->valueNormal() ));
        }
        if( m_d->QLimMEdC->z->readOnly() ){
            m_d->QLimMEdC->z->setValueNormal( 0.50 * (-(m_d->retainingWall->stemTBottom->valueNormal() + m_d->retainingWall->toeB->valueNormal()) + m_d->retainingWall->heelB->valueNormal() ) * tan(m_d->retainingWall->baseAlpha->valueNormal()));
        }
        if( m_d->QLimQEd->readOnly() ){
            m_d->QLimQEd->setValueNormal( m_d->QLimSoilDownGammaWPrimeD->valueNormal() * m_d->retainingWall->soilDownZ->valueNormal() );
        }
        m_d->retainingWall->calculateEQQLimVHM( m_d->QLimVEd, m_d->QLimHEd, m_d->QLimMEdC, m_d->QLimMEd, m_d->SoilDownCPrimeD, m_d->SoilUpCPrimeD, m_d->Ka, m_d->QLimApplyKp, m_d->KpRed, m_d->QLimHRdPMax, m_d->QLimHRdP, m_dd->EQDirection, m_dd->kv, m_dd->kh, m_d->Gk2, m_d->Qk, m_dd->psi2Q );
        m_d->retainingWall->calculateQLim( m_d->qLimD, m_d->QLimVRd,
                                           m_d->SoilDownPhiPrimeD, m_d->SoilDownCPrimeD, m_d->QLimSoilDownGammaWPrimeD, m_d->QLimBPrime, m_d->QLimLPrime, m_d->QLimLInfinite, m_d->QLimD, m_d->QLimHEd, m_d->QLimVEd, m_d->QLimMEd, m_d->QLimQEd,
                                           m_d->QLimNGammaW, m_d->QLimSGammaW, m_d->QLimIGammaW, m_d->QLimBGammaW, m_d->QLimGGammaW,
                                           m_d->QLimNC, m_d->QLimSC, m_d->QLimIC, m_d->QLimBC, m_d->QLimGC, m_d->QLimDC,
                                           m_d->QLimNQ, m_d->QLimSQ, m_d->QLimIQ, m_d->QLimBQ, m_d->QLimGQ, m_d->QLimDQ, m_d->gammaQLim );
    }
}

void RetainingWallEQQSCombinationGUI::copyClip(){
    QList<VarPlus *> v;

    // v << trUtf8("Verifica scorrimento e capacità  portante in condizioni sismiche");

    // v << trUtf8("Parametri azione sismica");
    v << m_dd->EQDirection << m_dd->kv << m_dd->kh << m_dd->EQtheta;

    // v << trUtf8("Parametri geotecnici");
    v << m_d->gammaTanPhiPrime << m_d->SoilDownPhiPrimeD << m_d->SoilUpPhiPrimeD;
    v << m_d->gammaCPrime << m_d->SoilDownCPrimeD << m_d->SoilUpCPrimeD;
    v << m_d->gammaTanPhiPrime << m_d->SoilDownDeltaD << m_d->SoilUpDeltaD;
    v << m_d->Ka << m_d->Kp << m_d->ChiKp << m_d->KpRed;

    // v << trUtf8("Verifica scorrimento");
    v << m_d->SlidingVEd << m_d->SlidingHEd << m_d->SlidingHRdP <<  m_d->SlidingHRd;

    // v << trUtf8("Verifica capacità  portante");
    v << m_d->QLimSoilDownGammaWPrimeD << m_d->QLimQEd;
    v << m_d->QLimHEd << m_d->QLimVEd << m_d->QLimMEdC->y << m_d->QLimMEdC->z << m_d->QLimMEd;
    v << m_d->QLimBPrime << m_d->QLimLPrime << m_d->QLimD;
    v << m_d->QLimNGammaW << m_d->QLimSGammaW << m_d->QLimIGammaW << m_d->QLimBGammaW << m_d->QLimGGammaW;
    v << m_d->QLimNC << m_d->QLimSC << m_d->QLimIC << m_d->QLimBC << m_d->QLimGC << m_d->QLimDC;
    v << m_d->QLimNQ << m_d->QLimSQ << m_d->QLimIQ << m_d->QLimBQ << m_d->QLimGQ << m_d->QLimDQ;
    v << m_d->qLimD;
    v << m_d->QLimVRd;

    QClipboard * clip = QApplication::clipboard();
    QMimeData * data = new QMimeData;
    data->setHtml( printData(v) );
    clip->setMimeData( data  );
}
