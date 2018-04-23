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
#include "sectionsteelomega.h"

#include "sectionthin.h"
#include "multisectionlinemodel.h"
#include "sectionline.h"
#include "multisectionarcmodel.h"
#include "sectionarc.h"
#include "steel.h"

#include "point2dmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "structmath.h"

#include <QGraphicsPathItem>
#include <cmath>

#define ARC_PREC 8

class SectionSteelOmegaPrivate{
public:
    SectionSteelOmegaPrivate(UnitMeasure *  ump, DoublePlus ** p, Steel * mat){
        np = 5;

        h = p[0];
        h->setToolTip( QObject::trUtf8("Altezza"));
        h->setRichName( QObject::trUtf8("h"));
        h->setUnitMeasure( UnitMeasure::sectL );
        h->setEnabled( true );

        b = p[1];
        b->setToolTip( QObject::trUtf8("Base"));
        b->setRichName( QObject::trUtf8("b"));
        b->setUnitMeasure( UnitMeasure::sectL );
        b->setEnabled( true );

        c = p[2];
        c->setToolTip( QObject::trUtf8("Ala"));
        c->setRichName( QObject::trUtf8("c"));
        c->setUnitMeasure( UnitMeasure::sectL );
        c->setEnabled( true );

        t = p[3];
        t->setToolTip( QObject::trUtf8("Spessore"));
        t->setRichName( QObject::trUtf8("t"));
        t->setUnitMeasure( UnitMeasure::sectL );
        t->setEnabled( true );

        ri = p[4];
        ri->setToolTip( QObject::trUtf8("Raggio interno di piegatura"));
        ri->setRichName( QObject::trUtf8("r<span style=\" vertical-align:sub;\">i</span>"));
        ri->setUnitMeasure( UnitMeasure::sectL );
        ri->setEnabled( true );

        nLines = 5;

        sectionThin = new SectionThin( ump, mat, "sectionThin" );
        sectionThin->sectionArcModel()->insertRows(0, nLines - 1);
        sectionThin->sectionLineModel()->insertRows(0, nLines);

        sectionProj = new SectionThin( ump, mat, "sectionProj" );
        sectionProj->sectionLineModel()->insertRows(0, nLines);

        for( int i=0; i < nLines; i++){
            u.append( new Point2DPlus("u", "y", 0.0, "z", 0.0, ump, UnitMeasure::sectL ) );
        }

        sectionPoints = NULL;
    }
    ~SectionSteelOmegaPrivate(){
        delete sectionThin;
        delete sectionProj;
        for( int i=0; i < nLines; i++){
            VarPlus::deleteVarPlus( u[i] );
        }
        u.clear();
        if( sectionPoints ){
            delete sectionPoints;
        }
    }

    int np;
    DoublePlus * h;
    DoublePlus * b;
    DoublePlus * c;
    DoublePlus * t;
    DoublePlus * ri;

    int nLines;
    SectionThin * sectionThin;
    SectionThin * sectionProj;
    QList<Point2DPlus *> u;
    Point2DModel * sectionPoints;
};

SectionSteelOmega::SectionSteelOmega( DoublePlus ** p, Steel * st, UnitMeasure * ump ) :
    SectionSteelProfile(st, ump),
    m_d( new SectionSteelOmegaPrivate(ump, p, st)) {

    m_d->ri->setEnabled( true );
    m_d->ri->setReadOnly( true );
    connect( m_d->t, &DoublePlus::valueChanged, this, &SectionSteelOmega::setRi );
    connect( m_d->ri, &DoublePlus::readOnlyChanged, this, &SectionSteelOmega::setRi );

    connect( m_d->b, &DoublePlus::valueChanged, this, &SectionSteelOmega::updateSectionSteelProfile );
    connect( m_d->h, &DoublePlus::valueChanged, this, &SectionSteelOmega::updateSectionSteelProfile );
    connect( m_d->c, &DoublePlus::valueChanged, this, &SectionSteelOmega::updateSectionSteelProfile );
    connect( m_d->t, &DoublePlus::valueChanged, this, &SectionSteelOmega::updateSectionSteelProfile );
    connect( m_d->ri, &DoublePlus::valueChanged, this, &SectionSteelOmega::updateSectionSteelProfile );

    updateSectionThin();
}

SectionSteelOmega::~SectionSteelOmega(){
    disconnect( m_d->t, &DoublePlus::valueChanged, this, &SectionSteelOmega::setRi );
    disconnect( m_d->ri, &DoublePlus::readOnlyChanged, this, &SectionSteelOmega::setRi );
    delete m_d;
}

SectionSteel::SectionShape SectionSteelOmega::sectionShape() {
    return SectionSteel::sectionOmega;
}

Point2DModel *SectionSteelOmega::sectionPoints() {
    if( m_d->sectionPoints == NULL ){
        m_d->sectionPoints = new Point2DModel();
    }
    return m_d->sectionPoints;
}

void SectionSteelOmega::setRi(){
    if( m_d->ri->readOnly() ){
        m_d->ri->setValueNormal( m_d->t->valueNormal() );
    }
}

int SectionSteelOmega::np(){
    return m_d->np;
}

double SectionSteelOmega::ANormal(){
    return m_d->sectionThin->ANormal();
}

double SectionSteelOmega::SyNormal(){
    return m_d->sectionThin->SyNormal();
}

double SectionSteelOmega::SzNormal(){
    return m_d->sectionThin->SzNormal();
}

double SectionSteelOmega::IyyNormal(){
    return m_d->sectionThin->IyyNormal();
}

double SectionSteelOmega::IzzNormal(){
    return m_d->sectionThin->IzzNormal();
}

double SectionSteelOmega::IyzNormal(){
    return m_d->sectionThin->IyzNormal();
}

double SectionSteelOmega::Sz3DyDzNormal(){
    return m_d->sectionThin->Sz3DyDzNormal();
}

double SectionSteelOmega::Sy3DyDzNormal(){
    return m_d->sectionThin->Sy3DyDzNormal();
}

double SectionSteelOmega::Syz2DyDzNormal(){
    return m_d->sectionThin->Syz2DyDzNormal();
}

double SectionSteelOmega::Sy2zDyDzNormal(){
    return m_d->sectionThin->Sy2zDyDzNormal();
}

double SectionSteelOmega::tyNormal(){
    return 1.2;
}

double SectionSteelOmega::tzNormal(){
    return 1.2;
}

double SectionSteelOmega::tyzNormal(){
    return 0.0;
}

double SectionSteelOmega::maxTNormal(){
    return m_d->t->valueNormal();
}

double SectionSteelOmega::ITNormal(){
    return m_d->sectionThin->ITNormal();
}

double SectionSteelOmega::IwNormal(){
    // TODO
    return 0.0;
}

double SectionSteelOmega::WElMyNormal(){
    return m_d->sectionThin->WMyNormal();
}

double SectionSteelOmega::WElMzNormal(){
    return m_d->sectionThin->WMzNormal();
}

double SectionSteelOmega::WPlMyNormal(){
    return m_d->sectionThin->WPlMyNormal();
}

double SectionSteelOmega::WPlMzNormal(){
    return m_d->sectionThin->WPlMzNormal();
}

void SectionSteelOmega::EffNMinusNormal( double * AEff, double * yGEff, double * zGEff, double prec){
    // sezione su cui effettuiamo il calcolo delle caratteristiche della sezione
    SectionThin sectEff( m_unitMeasure, steel(), "");
    *sectEff.sectionArcModel() = *m_d->sectionThin->sectionArcModel();
    sectEff.sectionLineModel()->insertRows( 0, 10 );
    sectEff.t->setValueNormal( m_d->t->valueNormal());
    sectEff.t->setReadOnly( false );

    for( int i=0; i < m_d->nLines; i++){
        // larghezze collaboranti
        double bEff[2];
        // Assumiamo di default tutta la sezione
        bEff[0] = m_d->sectionProj->sectionLine(i)->L->valueNormal() * 0.50;
        bEff[1] = m_d->sectionProj->sectionLine(i)->L->valueNormal() * 0.50;
        calcSectEff( sectEff, i, bEff );
    }

    double AOld = sectEff.A->valueNormal();
    calcSectEffLocalBuckling( sectEff, -1.0, 0.0, 0.0);
    calcSectEffDistorsionBuckling( sectEff, -1.0, 0.0, 0.0);
    double ANew = sectEff.A->valueNormal();
    double DA = 0.0;
    if( ANew != 0.0 ){
        DA = fabs((ANew - AOld) / ANew);
    } else {
        DA = 0.0;
    }
    while( DA > prec ){
        calcSectEffLocalBuckling( sectEff, -1.0, 0.0, 0.0);
        calcSectEffDistorsionBuckling( sectEff, -1.0, 0.0, 0.0);

        AOld = ANew;
        ANew = sectEff.A->valueNormal();
        if( ANew != 0.0 ){
            DA = fabs((ANew - AOld) / ANew);
        } else {
            DA = 0.0;
        }
    }

    *AEff = sectEff.A->valueNormal();
    *yGEff = sectEff.G->y->valueNormal();
    *zGEff = sectEff.G->z->valueNormal();
}

double SectionSteelOmega::WEffMyMaxNormal( double prec ){
    return WEffMyMzNormal( 1.0, 0.0, prec );
}

double SectionSteelOmega::WEffMyMinNormal( double prec ){
    return WEffMyMzNormal( -1.0, 0.0, prec );
}

double SectionSteelOmega::WEffMzMaxNormal( double prec ){
    // return 0.0;
    return WEffMyMzNormal( 0.0, 1.0, prec );
}

double SectionSteelOmega::WEffMzMinNormal( double prec ){
    // return 0.0;
    return -WEffMyMzNormal( 0.0, -1.0, prec );
}

double SectionSteelOmega::WEffMyMzNormal( double My, double Mz, double prec ) {
    // sezione su cui effettuiamo il calcolo delle caratteristiche della sezione
    SectionThin sectEff( m_unitMeasure, steel(), "");
    *sectEff.sectionArcModel() = *m_d->sectionThin->sectionArcModel();
    sectEff.sectionLineModel()->insertRows( 0, 10 );
    sectEff.t->setValueNormal( m_d->t->valueNormal());
    sectEff.t->setReadOnly( false );

    for( int i=0; i < 5; i++){
        // larghezze collaboranti
        double bEff[2];
        // Assumiamo di default tutta la sezione
        bEff[0] = m_d->sectionProj->sectionLine(i)->L->valueNormal() * 0.50;
        bEff[1] = m_d->sectionProj->sectionLine(i)->L->valueNormal() * 0.50;
        calcSectEff( sectEff, i, bEff );
    }

    double WOld = WEff(sectEff, 0.0, My, Mz);
    calcSectEffLocalBuckling( sectEff, 0.0, My, Mz);
    calcSectEffDistorsionBuckling( sectEff, 0.0, My, Mz);
    double WNew = WEff(sectEff, 0.0, My, Mz);
    double DW = 0.0;
    if( WNew != 0.0 ){
        DW = fabs((WNew - WOld) / WNew);
    } else {
        DW = 0.0;
    }
    int iter1 = 0;
    while( DW > prec && iter1 < MAXITER ){
        // Affrontiamo separatamente l'instabilitÃ   distorsionale e quella locale
        double WOldL, WNewL = WNew, DWL = DW;
        int iter2 = 0;
        while( DWL > prec && iter2 < MAXITER ){
            calcSectEffLocalBuckling( sectEff, 0.0, My, Mz);
            WOldL = WNewL;
            WNewL = WEff(sectEff, 0.0, My, Mz);
            if( WNewL != 0.0 ){
                DWL = fabs((WNewL - WOldL) / WNewL);
            } else {
                DWL = 0.0;
            }
            iter2++;
        }
        calcSectEffDistorsionBuckling( sectEff, 0.0, My, Mz);
        WOld = WNew;
        WNew = WEff(sectEff, 0.0, My, Mz);
        if( WNew != 0.0 ){
            DW = fabs((WNew - WOld) / WNew);
        } else {
            DW = 0.0;
        }
        iter1++;
    }

    return WNew;
}

void SectionSteelOmega::calcSectEffLocalBuckling( SectionThin & sectEff,
                                                  double N,
                                                  double My,
                                                  double Mz ){

    // momenti proiettati nel sistema centrale di inerzia della sezione efficace di partenza
    double aC = sectEff.thetaC->valueNormal();
    double MCy = My;
    double MCz = Mz;
    rotate2D( &MCy, &MCz, aC );

    // calcolo sezione efficace

    // double t = m_d->t->valueNormal();
    // ciclo sui vari tratti
    for( int i=0; i < 5; i++){
        // larghezze collaboranti
        // leggera difformità‚  rispetto alla normativa che invece prevede che le be
        // siano le porzioni collaboranti delle sole parti compresse
        double bEff[2];
        // Assumiamo di default tutta la sezione
        bEff[0] = m_d->sectionProj->sectionLine(i)->L->valueNormal() * 0.50;
        bEff[1] = m_d->sectionProj->sectionLine(i)->L->valueNormal() * 0.50;
        if( m_d->sectionThin->sectionLine(i)->t->valueNormal() != 0.0 ){
            // calcola s1 ed s2
            // Parziale differenza rispetto alla normativa: s1 tensione in P1 - s2 tensione in P2
            // invece nella normativa s1 e la maggiore delle due, s2 la minore
            // tensioni calcolate nel sistema centrale di inerzia della sezione efficace
            double s1 = 0.0;
            double s2 = 0.0;
            if( N != 0.0 ){
                double A = sectEff.A->valueNormal();
                if( A != 0.0 ){
                    s1 += N / A;
                    s2 += N / A;
                }
            }
            if( MCy != 0.0 || MCz != 0.0 ){
                double ICy = sectEff.ICyy->valueNormal();
                double ICz = sectEff.ICzz->valueNormal();
                if( !(ICy == 0.0 && MCy != 0.0) && !(ICz == 0.0 && MCz != 0.0)){
                    double y1 = m_d->sectionProj->sectionLine(i)->P1->y->valueNormal() - sectEff.G->y->valueNormal();
                    double z1 = m_d->sectionProj->sectionLine(i)->P1->z->valueNormal() - sectEff.G->z->valueNormal();
                    rotate2D( &y1, &z1, aC );
                    s1 = 0.0;
                    if( ICy != 0.0 ){
                        s1 += (MCy / ICy) * z1;
                    }
                    if( ICz != 0.0 ){
                        s1 +=  - (MCz / ICz) * y1;
                    }
                    double y2 = m_d->sectionProj->sectionLine(i)->P2->y->valueNormal() - sectEff.G->y->valueNormal();
                    double z2 = m_d->sectionProj->sectionLine(i)->P2->z->valueNormal() - sectEff.G->z->valueNormal();
                    rotate2D( &y2, &z2, aC );
                    s2 = 0.0;
                    if( ICy != 0.0 ){
                        s2 += (MCy / ICy) * z2;
                    }
                    if( ICz != 0.0 ){
                        s2 += - (MCz / ICz) * y2;
                    }
                }
            }

            if( s1 < 0.0 || s2 < 0.0 ){ // quindi una delle due Ä‚Â¨ != 0.0
                // psi : rapporto tra tensione massima e tensione minima (con segno)
                double psi = 1.0;
                if( s2 > s1 ){
                    psi = s2 / s1;
                } else {
                    psi = s1 / s2;
                }

                double ks = 0.43; // caso peggiore
                if( i == 0 ){
                    if( s1 > s2 ){  // tensione maggiore su lembo libero
                        if( psi >= 0.0 ){
                            ks = 0.578 / (psi + 0.34);
                        } else if( psi > -1){
                            ks = 1.70 - 5.0 * psi + 17.1 * psi * psi;
                        } else {
                            ks = 23.8; // = 1.70 + 5.0 + 17.1
                        }
                    } else {  // tensione maggiore su lembo vincolato
                        if( psi < 1.0 ){
                            if( psi > 0.0){
                                ks = 0.57 - 0.14 * psi;
                            } else if( psi > -1.0 ){
                                ks = 0.57 - 0.28 * psi;
                            } else if( psi > -3.0 ){
                                ks = 0.57 - 0.21 * psi + 0.07 * psi * psi;
                            } else {
                                ks = 1.83; // 0.57 + 0.21 * 3.0 + 0.07 * 9.0;
                            }
                        }
                    }
                    // rapporto tra lunghezza totale tratto in compressione e tratto efficace
                    double rho = 1.0;
                    double eps = 1.0;
                    if( steel() ){
                        eps = steel()->epsilon( m_d->sectionThin->sectionLine(i)->t->valueNormal() );
                    }
                    double l = m_d->sectionProj->sectionLine(i)->L->valueNormal() /
                            ( m_d->sectionThin->sectionLine(i)->t->valueNormal() * 28.4 * eps * sqrt(ks));
                    if( l > 0.748 ){
                        rho = (l - 0.188) / (l*l);
                    }
                    // inizializzo larghezze efficaci
                    bEff[0] =  0.0;
                    bEff[1] =  0.0;
                    if( psi >= 0.0 ){
                        // tensioni concordi negative
                        bEff[1] += rho * m_d->sectionProj->sectionLine(i)->L->valueNormal();
                    } else {
                        // tensioni discordi
                        if( s1 > 0.0 ){
                            bEff[0] += - psi * m_d->sectionProj->sectionLine(i)->L->valueNormal() / (1.0 - psi);
                        }
                        if( s2 > 0.0 ){
                            bEff[1] += - psi * m_d->sectionProj->sectionLine(i)->L->valueNormal() / (1.0 - psi);
                        }
                        bEff[1] += rho * m_d->sectionProj->sectionLine(i)->L->valueNormal() / (1.0 - psi);
                    }
                } else if( i == 4 ){
                    if( s2 > s1 ){ // tensione maggiore su lembo libero
                        if( psi >= 0.0 ){
                            ks = 0.578 / (psi + 0.34);
                        } else if( psi > -1){
                            ks = 1.70 - 5.0 * psi + 17.1 * psi * psi;
                        } else {
                            ks = 23.8; // = 1.70 + 5.0 + 17.1
                        }
                    } else { // tensione maggiore su lembo vincolato
                        if( psi > 0.0){
                            ks = 0.57 - 0.14 * psi;
                        } else if( psi > -1.0 ){
                            ks = 0.57 - 0.28 * psi;
                        } else if( psi > -3.0 ){
                            ks = 0.57 - 0.21 * psi + 0.07 * psi * psi;
                        } else {
                            ks = 1.83; // 0.57 + 0.21 * 3.0 + 0.07 * 9.0;
                        }
                    }
                    double rho = 1.0;
                    double eps = 1.0;
                    if( steel() ){
                        eps = steel()->epsilon( m_d->sectionThin->sectionLine(i)->t->valueNormal() );
                    }
                    double l = m_d->sectionProj->sectionLine(i)->L->valueNormal() /
                            ( m_d->sectionThin->sectionLine(i)->t->valueNormal() * 28.4 * eps * sqrt(ks));
                    if( l > 0.748 ){
                        rho = (l - 0.188) / (l*l);
                    }
                    bEff[0] =  0.0;
                    bEff[1] =  0.0;
                    if( psi >= 0.0 ){
                        // tensioni concordi negative
                        bEff[0] += rho * m_d->sectionProj->sectionLine(i)->L->valueNormal();
                    } else {
                        // tensioni discordi
                        if( s1 > 0.0 ){
                            bEff[0] += - psi * m_d->sectionProj->sectionLine(i)->L->valueNormal() / (1.0 - psi);
                        }
                        if( s2 > 0.0 ){
                            bEff[1] += - psi * m_d->sectionProj->sectionLine(i)->L->valueNormal() / (1.0 - psi);
                        }
                        bEff[0] += rho * m_d->sectionProj->sectionLine(i)->L->valueNormal() / (1.0 - psi);
                    }
                } else { // i != 0 && i != 4
                    if( psi >= 0.0 ){
                        ks = 8.2 / (1.05 + psi);
                    } else if( psi >= -1 ){
                        ks = 7.81 - 6.29 * psi + 9.78 * psi * psi;
                    } else if( psi > -3.0 ){
                        ks = 5.98 * pow((1.0 - psi),2.0);
                    } else {
                        ks = 95.68; // = 5.98 * 16.0
                    }
                    double rho = 1.0;
                    double eps = 1.0;
                    if( steel() ){
                        eps = steel()->epsilon( m_d->sectionThin->sectionLine(i)->t->valueNormal() );
                    }
                    double l = m_d->sectionProj->sectionLine(i)->L->valueNormal() /
                            ( m_d->sectionThin->sectionLine(i)->t->valueNormal() * 28.4 * eps * sqrt(ks));
                    if( l > 0.673 ){
                        rho = (l - 0.055 * (3.0 + psi)) / (l * l);
                    }
                    bEff[0] = 0.0;
                    bEff[1] = 0.0;
                    if( s1 > 0.0 ){
                        bEff[0] += - psi * m_d->sectionProj->sectionLine(i)->L->valueNormal() / (1.0 - psi);
                    }
                    if( s2 > 0.0 ){
                        bEff[1] += - psi * m_d->sectionProj->sectionLine(i)->L->valueNormal() / (1.0 - psi);
                    }
                    if( psi == 1.0 ){
                        bEff[0] += 0.50 * m_d->sectionProj->sectionLine(i)->L->valueNormal() * rho;
                        bEff[1] += 0.50 * m_d->sectionProj->sectionLine(i)->L->valueNormal() * rho;
                    } else if( psi >= 0.0 ){
                        double bEffTot = m_d->sectionProj->sectionLine(i)->L->valueNormal() * rho;
                        if(s1 < s2){
                            bEff[0] += (2.0 / (5.0 - psi)) * bEffTot;
                            bEff[1] += ((3.0 - psi) / (5.0 - psi)) * bEffTot;
                        } else { // s2 < s1
                            bEff[0] += ((3.0 - psi) / (5.0 - psi)) * bEffTot;
                            bEff[1] += (2.0 / (5.0 - psi)) * bEffTot;
                        }
                    } else { // psi < 0.0
                        double bEffTot = m_d->sectionProj->sectionLine(i)->L->valueNormal() * rho / (1.0 - psi);
                        if(s1 < s2){
                            bEff[0] += 0.40 * bEffTot;
                            bEff[1] += 0.60 * bEffTot;
                        } else {
                            bEff[0] += 0.60 * bEffTot;
                            bEff[1] += 0.40 * bEffTot;
                        }
                    }
                }
            }
        }

        // Aggiorna sectEff sulla base dei valori di bEff calcolati (lnghezze efficaci dei singoli tratti)
        // Parziale differenza rispetto alla normativa: in bEff vengono invlusi anche i tratti in trazione (bt)
        calcSectEff( sectEff, i, bEff );

    }
}

void SectionSteelOmega::calcSectEffDistorsionBuckling( SectionThin & sectEff, double N, double My, double Mz ){
    if( steel() == NULL ){
        return;
    }
    // instabilità   distorsionale
    SectionThin sect1( m_unitMeasure, steel(), "");
    sect1.suspendSignals( true );
    sect1.sectionArcModel()->insertRows(0,1);
    *(sect1.sectionArc(0)) = *(sectEff.sectionArc(0));
    sect1.sectionLineModel()->insertRows(0,2);
    *(sect1.sectionLine(0)) = *(sectEff.sectionLine(1));
    *(sect1.sectionLine(1)) = *(sectEff.sectionLine(2));
    sect1.updateReadOnlyVars();

    SectionThin sect2( m_unitMeasure, steel(), "");
    sect2.suspendSignals( true );
    sect2.sectionArcModel()->insertRows(0,1);
    *(sect2.sectionArc(0)) = *(sectEff.sectionArc(3));
    sect2.sectionLineModel()->insertRows(0,2);
    *(sect2.sectionLine(0)) = *(sectEff.sectionLine(9-2));
    *(sect2.sectionLine(1)) = *(sectEff.sectionLine(9-1));
    sect2.updateReadOnlyVars();

    double s1 = 0.0;
    double s2 = 0.0;

    double ICy = sectEff.ICyy->valueNormal();
    double ICz = sectEff.ICzz->valueNormal();
    if( ICy != 0.0 && ICz != 0.0 ){
        double aC = sectEff.thetaC->valueNormal();
        double MCy = My;
        double MCz = Mz;
        rotate2D( &MCy, &MCz, aC );
        double y1 = sect1.G->y->valueNormal() - sectEff.G->y->valueNormal();
        double z1 = sect1.G->y->valueNormal() - sectEff.G->y->valueNormal();
        rotate2D( &y1, &z1, aC );
        s1 += (MCy / ICy) * z1 - (MCz / ICz) * y1;
        double y2 = sect2.G->y->valueNormal() - sectEff.G->y->valueNormal();
        double z2 = sect2.G->z->valueNormal() - sectEff.G->z->valueNormal();
        rotate2D( &y2, &z2, aC );
        s2 += (MCy / ICy) * z2 - (MCz / ICz) * y2;
    }

    if( sectEff.A->valueNormal() != 0.0 ){
        s1 += N / sectEff.A->valueNormal();
        s2 += N / sectEff.A->valueNormal();
    }

    if( s1 < 0.0 && sect1.A->valueNormal() != 0.0 ){
        double b1 = fabs( m_d->sectionThin->sectionLine(2)->P1->z->valueNormal()
                          - sect1.G->z->valueNormal());
        double hw = fabs( m_d->sectionThin->sectionLine(3)->P1->y->valueNormal()
                          - m_d->sectionThin->sectionLine(1)->P2->y->valueNormal() );
        double D1 = b1 * b1 * hw + pow(b1,3.0);
        if( s2 < 0.0 ){
            double b2 = fabs( sect2.G->z->valueNormal()
                              - m_d->sectionThin->sectionLine(2)->P2->z->valueNormal() );
            double kf = sect2.A->valueNormal() / sect1.A->valueNormal();
            D1 += 0.50 * b1 * b2 * hw * kf;
        }
        double D2 = 1.0 - pow( steel()->nu->valueNormal(), 2.0);
        double K1 = steel()->E->valueNormal() * pow( m_d->t->valueNormal(),3.0 ) / ( 4.0 * D1 * D2);
        double sCr1 = 2.0 * sqrt( K1 * steel()->E->valueNormal() * sect1.IGzz->valueNormal() ) / sect1.A->valueNormal();
        double l = 0.0;
        if( sCr1 != 0.0 ){
            l = sqrt( fykNormal() / sCr1 );
        }
        double chiD1 = 1.0;
        if( l > 1.38 ){
            chiD1 = 0.66 / l;
        } else if( l > 0.65 ) {
            chiD1 = 1.47 - 0.723 * l;
        }
        sectEff.sectionArc(0)->t->setValueNormal( m_d->t->valueNormal() * chiD1 );
        sectEff.sectionLine(1)->t->setValueNormal( m_d->t->valueNormal() * chiD1 );
        sectEff.sectionLine(2)->t->setValueNormal( m_d->t->valueNormal() * chiD1 );
    }
    if( s2 < 0.0 && sect2.A->valueNormal() != 0.0 ){
        double b2 = fabs( sect2.G->z->valueNormal()
                          - m_d->sectionThin->sectionLine(2)->P2->z->valueNormal() );
        double hw = fabs( m_d->sectionThin->sectionLine(3)->P1->y->valueNormal()
                          - m_d->sectionThin->sectionLine(1)->P2->y->valueNormal() );
        double D1 = b2 * b2 * hw + pow(b2,3.0);
        if( s1 < 0.0 ){
            double b1 = fabs( m_d->sectionThin->sectionLine(2)->P1->z->valueNormal()
                              - sect1.G->z->valueNormal());
            double kf = sect1.A->valueNormal() / sect2.A->valueNormal();
            D1 += 0.50 * b1 * b2 * hw * kf;
        }
        double D2 = 1.0 - pow( steel()->nu->valueNormal(), 2.0);
        double K2 = steel()->E->valueNormal() * pow( m_d->t->valueNormal(),3.0 ) / ( 4.0 * D1 * D2);
        double sCr2 = 2.0 * sqrt( K2 * steel()->E->valueNormal() * sect2.IGzz->valueNormal() ) / sect2.A->valueNormal();
        double l = 0.0;
        if( sCr2 != 0.0 ){
            l = sqrt( fykNormal() / sCr2 );
        }
        double chiD2 = 1.0;
        if( l > 1.38 ){
            chiD2 = 0.66 / l;
        } else if( l > 0.65 ) {
            chiD2 = 1.47 - 0.723 * l;
        }
        sectEff.sectionArc(3)->t->setValueNormal( m_d->t->valueNormal() * chiD2 );
        sectEff.sectionLine(9-1)->t->setValueNormal( m_d->t->valueNormal() * chiD2 );
        sectEff.sectionLine(9-2)->t->setValueNormal( m_d->t->valueNormal() * chiD2 );
    }
}

double SectionSteelOmega::WEff( SectionThin & sectEff,
                                double N,
                                double My,
                                double Mz ){
    // tensiona massima in modulo agente sulla sezione
    double sMax = 0.0;

    // momenti proiettati nel sistema centrale di inerzia della sezione efficace aggiornata
    double MCy = My;
    double MCz = Mz;
    rotate2D( &MCy, &MCz, sectEff.thetaC->valueNormal() );

    // tensione dovuta a sforzo normale
    double sN = 0.0;
    if( sectEff.A->valueNormal() != 0.0 ){
        sN = N / sectEff.A->valueNormal();
    }

    // calcolo tensioni in punti sezione
    for( int i=0; i < sectEff.nSectionLine(); i++){
        if( sectEff.sectionLine(i)->L->valueNormal() != 0.0 ){
            // calcola s1 ed s2
            // s1 tensione in P1 - s2 tensione in P2
            // tensioni calcolate nel sistema centrale di inerzia della sezione efficace
            double s1 = sN;
            double s2 = sN;
            double ICy = sectEff.ICyy->valueNormal();
            double ICz = sectEff.ICzz->valueNormal();
            if( ICy != 0.0 && ICz != 0.0 ){
                double aC = sectEff.thetaC->valueNormal();
                double y1 = sectEff.sectionLine(i)->P1->y->valueNormal() - sectEff.G->y->valueNormal();
                double z1 = sectEff.sectionLine(i)->P1->z->valueNormal() - sectEff.G->z->valueNormal();
                rotate2D( &y1, &z1, aC );
                s1 = (MCy / ICy) * z1 - (MCz / ICz) * y1;
                double y2 = sectEff.sectionLine(i)->P2->y->valueNormal() - sectEff.G->y->valueNormal();
                double z2 = sectEff.sectionLine(i)->P2->z->valueNormal() - sectEff.G->z->valueNormal();
                rotate2D( &y2, &z2, aC );
                s2 = (MCy / ICy) * z2 - (MCz / ICz) * y2;
            }
            if( fabs(s1) > sMax )
                sMax = fabs(s1);
            if( fabs(s2) > sMax ){
                sMax = fabs(s2);
            }
        }
    }
    if( sMax > 0.0 ){
        return length2D(0.0,0.0,My,Mz) / sMax;
    }
    return 0.0;
}

void SectionSteelOmega::calcSectEff( SectionThin & sectEff, int i, double bEff[] ){
    if( i == 0 ){
        sectEff.sectionLine(2*i)->P1->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->y->valueNormal() );
        sectEff.sectionLine(2*i)->P1->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->z->valueNormal() );
        sectEff.sectionLine(2*i)->P2->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->y->valueNormal() + m_d->u.at(i)->y->valueNormal() * bEff[0] );
        sectEff.sectionLine(2*i)->P2->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->z->valueNormal() + m_d->u.at(i)->z->valueNormal() * bEff[0] );

        sectEff.sectionLine(2*i+1)->P1->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->y->valueNormal() );
        sectEff.sectionLine(2*i+1)->P1->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->z->valueNormal() );
        sectEff.sectionLine(2*i+1)->P2->y->setValueNormal( m_d->sectionProj->sectionLine(i)->P2->y->valueNormal() - m_d->u.at(i)->y->valueNormal() * bEff[1] );
        sectEff.sectionLine(2*i+1)->P2->z->setValueNormal( m_d->sectionProj->sectionLine(i)->P2->z->valueNormal() - m_d->u.at(i)->z->valueNormal() * bEff[1] );
    } else if( i == (m_d->nLines - 1) ){
        sectEff.sectionLine(2*i)->P1->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->y->valueNormal() );
        sectEff.sectionLine(2*i)->P1->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->z->valueNormal() );
        sectEff.sectionLine(2*i)->P2->y->setValueNormal( m_d->sectionProj->sectionLine(i)->P1->y->valueNormal() + m_d->u.at(i)->y->valueNormal() * bEff[0] );
        sectEff.sectionLine(2*i)->P2->z->setValueNormal( m_d->sectionProj->sectionLine(i)->P1->z->valueNormal() + m_d->u.at(i)->z->valueNormal() * bEff[0] );

        sectEff.sectionLine(2*i+1)->P1->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->y->valueNormal() );
        sectEff.sectionLine(2*i+1)->P1->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->z->valueNormal() );
        sectEff.sectionLine(2*i+1)->P2->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->y->valueNormal() - m_d->u.at(i)->y->valueNormal() * bEff[1] );
        sectEff.sectionLine(2*i+1)->P2->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->z->valueNormal() - m_d->u.at(i)->z->valueNormal() * bEff[1] );
    } else { // i != 0 && i != 4
        sectEff.sectionLine(2*i)->P1->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->y->valueNormal() );
        sectEff.sectionLine(2*i)->P1->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->z->valueNormal() );
        sectEff.sectionLine(2*i)->P2->y->setValueNormal( m_d->sectionProj->sectionLine(i)->P1->y->valueNormal() + m_d->u.at(i)->y->valueNormal() * bEff[0] );
        sectEff.sectionLine(2*i)->P2->z->setValueNormal( m_d->sectionProj->sectionLine(i)->P1->z->valueNormal() + m_d->u.at(i)->z->valueNormal() * bEff[0] );

        sectEff.sectionLine(2*i+1)->P1->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->y->valueNormal() );
        sectEff.sectionLine(2*i+1)->P1->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->z->valueNormal() );
        sectEff.sectionLine(2*i+1)->P2->y->setValueNormal( m_d->sectionProj->sectionLine(i)->P2->y->valueNormal() - m_d->u.at(i)->y->valueNormal() * bEff[1] );
        sectEff.sectionLine(2*i+1)->P2->z->setValueNormal( m_d->sectionProj->sectionLine(i)->P2->z->valueNormal() - m_d->u.at(i)->z->valueNormal() * bEff[1] );
    }
}

double SectionSteelOmega::AVyNormal(){
    // TODO
    return 0.0;
}

double SectionSteelOmega::AVzNormal(){
    // TODO
    return 0.0;
}

QString SectionSteelOmega::NClass(){
    int fClass = 0;
    int wClass = 0;
    for( int i=0; i < m_d->sectionProj->nSectionLine(); i++ ){
        if( i == 0 || i == (m_d->sectionProj->nSectionLine() -1)){
            int fClassOther = outstandFlangeClass( m_d->sectionProj->sectionLine( m_d->sectionProj->nSectionLine() - 1 )->L->valueNormal(),
                                                   m_d->t->valueNormal(), 1.0, 1.0);
            if( fClassOther > fClass ){
                fClass = fClassOther;
            }
        } else {
            int wClassOther = internalPartClass( m_d->sectionProj->sectionLine(i)->L->valueNormal(),
                                                 m_d->t->valueNormal(), 1.0, 1.0);
            if( wClassOther > wClass ){
                wClass = wClassOther;
            }
        }
    }

    if( fClass == wClass ){
        return QString::number(fClass);
    } else {
        if( fClass > wClass ){
            return ( QString::number(fClass) + QString("f") );
        } else {
            return ( QString::number(wClass) + QString("w") );
        }
    }
}

QString SectionSteelOmega::MClass( double My, double Mz){
    int fClass = 0;
    int wClass = 0;
    if( m_d->sectionThin->Iyy->valueNormal() != 0.0 && m_d->sectionThin->Izz->valueNormal() != 0.0){
        for( int i=0; i < m_d->sectionProj->nSectionLine(); i++ ){
            double s1 = My / m_d->sectionThin->Iyy->valueNormal() * m_d->sectionProj->sectionLine(i)->P1->z->valueNormal() -
                    Mz / m_d->sectionThin->Izz->valueNormal() * m_d->sectionProj->sectionLine(i)->P1->y->valueNormal();
            double s2 = My / m_d->sectionThin->Iyy->valueNormal() * m_d->sectionProj->sectionLine(i)->P2->z->valueNormal() -
                    Mz / m_d->sectionThin->Izz->valueNormal() * m_d->sectionProj->sectionLine(i)->P2->y->valueNormal();
            if( i == 0 || i == (m_d->sectionProj->nSectionLine() -1)){
                int fClassOther = 0;
                if( i == 0 ){
                    fClassOther = outstandFlangeClass( m_d->sectionProj->sectionLine( m_d->sectionProj->nSectionLine() - 1 )->L->valueNormal(),
                                                       m_d->t->valueNormal(), -s2, -s1);
                } else {
                    fClassOther = outstandFlangeClass( m_d->sectionProj->sectionLine( m_d->sectionProj->nSectionLine() - 1 )->L->valueNormal(),
                                                       m_d->t->valueNormal(), -s1, -s2);
                }
                if( fClassOther > fClass ){
                    fClass = fClassOther;
                }
            } else {
                int wClassOther = internalPartClass( m_d->sectionProj->sectionLine(i)->L->valueNormal(),
                                                     m_d->t->valueNormal(), -s1, -s2);
                if( wClassOther > wClass ){
                    wClass = wClassOther;
                }
            }
        }
    }

    if( fClass == wClass ){
        return QString::number(fClass);
    } else {
        if( fClass > wClass ){
            return ( QString::number(fClass) + QString("f") );
        } else {
            return ( QString::number(wClass) + QString("w") );
        }
    }
}

QString SectionSteelOmega::MyMaxClass(){
    return MClass(1.0, 0.0);
}

QString SectionSteelOmega::MyMinClass(){
    return MClass(-1.0, 0.0);
}

QString SectionSteelOmega::MzMaxClass(){
    return MClass(0.0, 1.0);
}

QString SectionSteelOmega::MzMinClass(){
    return MClass(0.0, -1.0);
}

QGraphicsItem * SectionSteelOmega::qGraphicsItem(){
    return m_d->sectionThin->qGraphicsItem();
}

void SectionSteelOmega::updateSectionSteelProfile() {
    updateSectionPoints();
    updateSectionThin();
    updateQGraphics();

    if( !m_signalsSuspended ){
        emit sectionSteelProfileChanged();
    }
}

void SectionSteelOmega::updateSectionThin(){
    double r = 0.0;
    if( m_d->ri->valueNormal() > 0.0 ){
        r = m_d->ri->valueNormal() + 0.5 * m_d->t->valueNormal();
    }

    m_d->sectionThin->suspendSignals( true );
    m_d->sectionThin->sectionLine(0)->t->setValueNormal( m_d->t->valueNormal() );
    m_d->sectionThin->sectionLine(0)->P1->y->setValueNormal( -0.5 * m_d->b->valueNormal() - m_d->c->valueNormal() );
    m_d->sectionThin->sectionLine(0)->P1->z->setValueNormal( 0.0 );
    m_d->sectionThin->sectionLine(0)->P2->y->setValueNormal( -0.5 * m_d->b->valueNormal() - r );
    m_d->sectionThin->sectionLine(0)->P2->z->setValueNormal( 0.0 );

    m_d->sectionThin->sectionArc(0)->t->setValueNormal( m_d->t->valueNormal() );
    m_d->sectionThin->sectionArc(0)->R->setValueNormal( r );
    m_d->sectionThin->sectionArc(0)->C->y->setValueNormal( -0.5 * m_d->b->valueNormal() - r );
    m_d->sectionThin->sectionArc(0)->C->z->setValueNormal( r );
    m_d->sectionThin->sectionArc(0)->startAngle->setValueNormal( 1.5 * M_PI );
    m_d->sectionThin->sectionArc(0)->sweepAngle->setValueNormal( 0.5 * M_PI );

    m_d->sectionThin->sectionLine(1)->t->setValueNormal( m_d->t->valueNormal() );
    m_d->sectionThin->sectionLine(1)->P1->y->setValueNormal( -0.5 * m_d->b->valueNormal() );
    m_d->sectionThin->sectionLine(1)->P1->z->setValueNormal( r );
    m_d->sectionThin->sectionLine(1)->P2->y->setValueNormal( -0.5 * m_d->b->valueNormal() );
    m_d->sectionThin->sectionLine(1)->P2->z->setValueNormal( m_d->h->valueNormal() - r );

    m_d->sectionThin->sectionArc(1)->t->setValueNormal( m_d->t->valueNormal() );
    m_d->sectionThin->sectionArc(1)->R->setValueNormal( r );
    m_d->sectionThin->sectionArc(1)->C->y->setValueNormal( -0.5 * m_d->b->valueNormal() + r );
    m_d->sectionThin->sectionArc(1)->C->z->setValueNormal( m_d->h->valueNormal() - r );
    m_d->sectionThin->sectionArc(1)->startAngle->setValueNormal( 0.5 * M_PI );
    m_d->sectionThin->sectionArc(1)->sweepAngle->setValueNormal( 0.5 * M_PI );

    m_d->sectionThin->sectionLine(2)->t->setValueNormal( m_d->t->valueNormal() );
    m_d->sectionThin->sectionLine(2)->P1->y->setValueNormal( -0.5 * m_d->b->valueNormal() + r);
    m_d->sectionThin->sectionLine(2)->P1->z->setValueNormal( m_d->h->valueNormal() );
    m_d->sectionThin->sectionLine(2)->P2->y->setValueNormal( 0.5 * m_d->b->valueNormal() - r );
    m_d->sectionThin->sectionLine(2)->P2->z->setValueNormal( m_d->h->valueNormal() );

    m_d->sectionThin->sectionArc(2)->t->setValueNormal( m_d->t->valueNormal() );
    m_d->sectionThin->sectionArc(2)->R->setValueNormal( r );
    m_d->sectionThin->sectionArc(2)->C->y->setValueNormal( 0.5 * m_d->b->valueNormal() - r );
    m_d->sectionThin->sectionArc(2)->C->z->setValueNormal( m_d->h->valueNormal() - r );
    m_d->sectionThin->sectionArc(2)->startAngle->setValueNormal( 0.0 );
    m_d->sectionThin->sectionArc(2)->sweepAngle->setValueNormal( 0.5 * M_PI );

    m_d->sectionThin->sectionLine(3)->t->setValueNormal( m_d->t->valueNormal() );
    m_d->sectionThin->sectionLine(3)->P1->y->setValueNormal( 0.5 * m_d->b->valueNormal() );
    m_d->sectionThin->sectionLine(3)->P1->z->setValueNormal( m_d->h->valueNormal() - r );
    m_d->sectionThin->sectionLine(3)->P2->y->setValueNormal( 0.5 * m_d->b->valueNormal() );
    m_d->sectionThin->sectionLine(3)->P2->z->setValueNormal( r );

    m_d->sectionThin->sectionArc(3)->t->setValueNormal( m_d->t->valueNormal() );
    m_d->sectionThin->sectionArc(3)->R->setValueNormal( r );
    m_d->sectionThin->sectionArc(3)->C->y->setValueNormal( 0.5 * m_d->b->valueNormal() + r );
    m_d->sectionThin->sectionArc(3)->C->z->setValueNormal( r );
    m_d->sectionThin->sectionArc(3)->startAngle->setValueNormal( M_PI );
    m_d->sectionThin->sectionArc(3)->sweepAngle->setValueNormal( 0.5 * M_PI );

    m_d->sectionThin->sectionLine(4)->t->setValueNormal( m_d->t->valueNormal() );
    m_d->sectionThin->sectionLine(4)->P1->y->setValueNormal( 0.5 * m_d->b->valueNormal() + r );
    m_d->sectionThin->sectionLine(4)->P1->z->setValueNormal( 0.0 );
    m_d->sectionThin->sectionLine(4)->P2->y->setValueNormal( 0.5 * m_d->b->valueNormal() + m_d->c->valueNormal() );
    m_d->sectionThin->sectionLine(4)->P2->z->setValueNormal( 0.0 );

    m_d->sectionThin->suspendSignals( false );

    m_d->sectionThin->GTranslate();

    m_d->sectionProj->t->setValueNormal( m_d->t->valueNormal() );
    for( int i=0; i < m_d->nLines; i++){
        // u = P2 - P1
        m_d->u.at(i)->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->y->valueNormal()
                                         - m_d->sectionThin->sectionLine(i)->P1->y->valueNormal()  );
        m_d->u.at(i)->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->z->valueNormal()
                                         - m_d->sectionThin->sectionLine(i)->P1->z->valueNormal()  );
        // u = u / |u|
        m_d->u.at(i)->normalize();

        if( i == 0 ){
            // m_d->sectionProj->P1 = sectionThin.P1
            m_d->sectionProj->sectionLine(i)->P1->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->y->valueNormal() );
            m_d->sectionProj->sectionLine(i)->P1->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->z->valueNormal() );
            // sectE.P2 = sectionThin.P2 + (r*u) u
            double a = m_d->sectionThin->sectionArc(i)->startAngle->valueNormal() + m_d->sectionThin->sectionArc(i)->sweepAngle->valueNormal() * 0.50;
            double R = m_d->sectionThin->sectionArc(i)->R->valueNormal();
            double yR = R * cos(a);
            double zR = R * sin(a);
            double cdot = (m_d->u.at(i)->y->valueNormal() * yR) + (m_d->u.at(i)->z->valueNormal() * zR);
            m_d->sectionProj->sectionLine(i)->P2->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->y->valueNormal()
                                                                     + cdot * m_d->u.at(i)->y->valueNormal() );
            m_d->sectionProj->sectionLine(i)->P2->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->z->valueNormal()
                                                                     + cdot * m_d->u.at(i)->z->valueNormal() );
        } else if( i == (m_d->nLines - 1) ){
            // sectE.P1 = sectionThin.P1 + (r*u) u
            double a = m_d->sectionThin->sectionArc(i-1)->startAngle->valueNormal() + m_d->sectionThin->sectionArc(i-1)->sweepAngle->valueNormal() * 0.50;
            double R = m_d->sectionThin->sectionArc(i-1)->R->valueNormal();
            double yR = R * cos(a);
            double zR = R * sin(a);
            // prodotto scalare
            double cdot = (m_d->u.at(i)->y->valueNormal() * yR) + (m_d->u.at(i)->z->valueNormal() * zR);
            m_d->sectionProj->sectionLine(i)->P1->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->y->valueNormal()
                                                                     + cdot * m_d->u.at(i)->y->valueNormal() );
            m_d->sectionProj->sectionLine(i)->P1->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->z->valueNormal()
                                                                     + cdot * m_d->u.at(i)->z->valueNormal() );
            // sectE.P2 = sectionThin.P2
            m_d->sectionProj->sectionLine(i)->P2->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->y->valueNormal() );
            m_d->sectionProj->sectionLine(i)->P2->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->z->valueNormal() );
        } else {
            // sectE.P1 = sectionThin.P1 + (r*u) u
            double a = m_d->sectionThin->sectionArc(i-1)->startAngle->valueNormal() + m_d->sectionThin->sectionArc(i-1)->sweepAngle->valueNormal() * 0.50;
            double R = m_d->sectionThin->sectionArc(i-1)->R->valueNormal();
            double yR = R * cos(a);
            double zR = R * sin(a);
            double cdot = (m_d->u.at(i)->y->valueNormal() * yR) + (m_d->u.at(i)->z->valueNormal() * zR);
            m_d->sectionProj->sectionLine(i)->P1->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->y->valueNormal()
                                                                     + cdot * m_d->u.at(i)->y->valueNormal() );
            m_d->sectionProj->sectionLine(i)->P1->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P1->z->valueNormal()
                                                                     + cdot * m_d->u.at(i)->z->valueNormal() );
            // sectE.P2 = sectionThin.P2 + (r*u) u
            a = m_d->sectionThin->sectionArc(i)->startAngle->valueNormal() + m_d->sectionThin->sectionArc(i)->sweepAngle->valueNormal() * 0.50;
            R = m_d->sectionThin->sectionArc(i)->R->valueNormal();
            yR = R * cos(a);
            zR = R * sin(a);
            cdot = (m_d->u.at(i)->y->valueNormal() * yR) + (m_d->u.at(i)->z->valueNormal() * zR);
            m_d->sectionProj->sectionLine(i)->P2->y->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->y->valueNormal()
                                                                     + cdot * m_d->u.at(i)->y->valueNormal() );
            m_d->sectionProj->sectionLine(i)->P2->z->setValueNormal( m_d->sectionThin->sectionLine(i)->P2->z->valueNormal()
                                                                     + cdot * m_d->u.at(i)->z->valueNormal() );
        }
    }
}

void SectionSteelOmega::updateQGraphics(){
    m_d->sectionThin->updateQGraphics();
}

void SectionSteelOmega::updateSectionPoints() {
    if( m_sectionPoints == NULL ){
        m_sectionPoints = new Point2DModel();
    }

    int np = 2 + ARC_PREC + 1 + ARC_PREC + 1 + ARC_PREC + 1 + ARC_PREC + 3 + ARC_PREC + 1 + ARC_PREC + 1 + ARC_PREC + 1 +ARC_PREC + 1;
    m_d->sectionPoints->resize(np);

    double re = m_d->ri->valueNormal() + m_d->t->valueNormal();
    double ri = m_d->ri->valueNormal();

    m_d->sectionPoints->setYZNormal( 0,
                                     -0.5 * m_d->b->valueNormal() - m_d->c->valueNormal(),
                                     0.0 );
    m_d->sectionPoints->setYZNormal( 1,
                                     - 0.5 * m_d->b->valueNormal() + 0.50 * m_d->t->valueNormal() - re,
                                     0.0 );

    double r = re;
    double yC = - 0.5 * m_d->b->valueNormal() + m_d->t->valueNormal() / 2.0 - re;
    double zC = re;
    double dt = (M_PI/2.0) / ARC_PREC;
    double t = - M_PI/2.0 + dt;
    for( int i=2; i < (ARC_PREC+2); ++i ){
        m_d->sectionPoints->setYZNormal( i, yC + r * cos(t), zC + r * sin(t) );
        t += dt;
    }

    m_d->sectionPoints->setYZNormal( ARC_PREC+2,
                                     - (0.5 * m_d->b->valueNormal() - m_d->t->valueNormal() / 2.0),
                                     m_d->h->valueNormal() - m_d->t->valueNormal() / 2.0 - ri );

    r = ri;
    yC = - (0.5 * m_d->b->valueNormal() - m_d->t->valueNormal() / 2.0 - ri);
    zC = m_d->h->valueNormal() - m_d->t->valueNormal() / 2.0 - ri;
    t = M_PI - dt;
    for( int i=ARC_PREC+3; i < (2*ARC_PREC+3); ++i ){
        m_d->sectionPoints->setYZNormal( i, yC + r * cos(t), zC + r * sin(t) );
        t -= dt;
    }

    m_d->sectionPoints->setYZNormal( 2*ARC_PREC+3,
                                     0.5 * m_d->b->valueNormal() - m_d->t->valueNormal() / 2.0 - ri,
                                     m_d->h->valueNormal() - m_d->t->valueNormal() / 2.0 );

    r = ri;
    yC = 0.5 * m_d->b->valueNormal() - m_d->t->valueNormal() / 2.0 - ri;
    zC = m_d->h->valueNormal() - m_d->t->valueNormal() / 2.0 - ri;
    t = M_PI/2.0 - dt;
    for( int i=2*ARC_PREC+4; i < (3*ARC_PREC+4); ++i ){
        m_d->sectionPoints->setYZNormal( i, yC + r * cos(t), zC + r * sin(t) );
        t -= dt;
    }

    m_d->sectionPoints->setYZNormal( 3*ARC_PREC+4,
                                     0.5 * m_d->b->valueNormal() - m_d->t->valueNormal() / 2.0,
                                     re );

    r = re;
    yC = 0.5 * m_d->b->valueNormal() - m_d->t->valueNormal() / 2.0 + re;
    zC = re;
    t = M_PI + dt;
    for( int i=3*ARC_PREC+5; i < (4*ARC_PREC+5); ++i ){
        m_d->sectionPoints->setYZNormal( i, yC + r * cos(t), zC + r * sin(t) );
        t += dt;
    }

    m_d->sectionPoints->setYZNormal( 4*ARC_PREC+5,
                                     0.5 * m_d->b->valueNormal() + m_d->c->valueNormal(),
                                     0.0 );

    m_d->sectionPoints->setYZNormal( 4*ARC_PREC+6,
                                     0.5 * m_d->b->valueNormal() + m_d->c->valueNormal(),
                                     m_d->t->valueNormal() );

    m_d->sectionPoints->setYZNormal( 4*ARC_PREC+7,
                                     0.5 * m_d->b->valueNormal() + m_d->t->valueNormal() / 2.0 + ri,
                                     m_d->t->valueNormal() );

    r = ri;
    yC = 0.5 * m_d->b->valueNormal() + m_d->t->valueNormal() / 2 + ri;
    zC = re;
    t = 3.0*M_PI/2.0 - dt;
    for( int i=4*ARC_PREC+8; i < (5*ARC_PREC+8); ++i ){
        m_d->sectionPoints->setYZNormal( i, yC + r * cos(t), zC + r * sin(t) );
        t -= dt;
    }

    m_d->sectionPoints->setYZNormal( 5*ARC_PREC+8,
                                     0.5 * m_d->b->valueNormal() + m_d->t->valueNormal() / 2.0,
                                     m_d->h->valueNormal() + m_d->t->valueNormal() / 2.0  - re);

    r = re;
    yC = 0.5 * m_d->b->valueNormal() + m_d->t->valueNormal() / 2.0 - re;
    zC = m_d->h->valueNormal() + m_d->t->valueNormal() / 2.0 - re;
    t = dt;
    for( int i=5*ARC_PREC+9; i < (6*ARC_PREC+9); ++i ){
        m_d->sectionPoints->setYZNormal( i, yC + r * cos(t), zC + r * sin(t) );
        t += dt;
    }

    m_d->sectionPoints->setYZNormal( 6*ARC_PREC+9,
                                     - (0.5 * m_d->b->valueNormal() + m_d->t->valueNormal() / 2.0 -re),
                                     m_d->h->valueNormal() + m_d->t->valueNormal() / 2.0 );

    r = re;
    yC = - (0.5 * m_d->b->valueNormal() + m_d->t->valueNormal() / 2.0 - re);
    zC = m_d->h->valueNormal() + m_d->t->valueNormal() / 2.0 - re;
    t = M_PI/ 2.0 + dt;
    for( int i=6*ARC_PREC+10; i < (7*ARC_PREC+10); ++i ){
        m_d->sectionPoints->setYZNormal( i, yC + r * cos(t), zC + r * sin(t) );
        t += dt;
    }

    m_d->sectionPoints->setYZNormal( 7*ARC_PREC+10,
                                     - (0.5 * m_d->b->valueNormal() + m_d->t->valueNormal() / 2.0 ),
                                     re );

    r = ri;
    yC = - (0.5 * m_d->b->valueNormal() + m_d->t->valueNormal() / 2.0 + ri);
    zC = re;
    t = - dt;
    for( int i=7*ARC_PREC+11; i < (8*ARC_PREC+11); ++i ){
        m_d->sectionPoints->setYZNormal( i, yC + r * cos(t), zC + r * sin(t) );
        t -= dt;
    }

    m_d->sectionPoints->setYZNormal( 8*ARC_PREC+11,
                                     - (0.5 * m_d->b->valueNormal() + m_d->c->valueNormal() ) ,
                                     m_d->t->valueNormal() );

    double Ap = m_d->sectionPoints->ANormal();
    if( Ap != 0.0 ){
        double zGp = - m_d->sectionPoints->SyNormal() / Ap;
        double yGp = - m_d->sectionPoints->SzNormal() / Ap;
        m_d->sectionPoints->translateNormal( yGp, zGp );
    }
}

void SectionSteelOmega::classDiagram(QList<QPolygonF *> *, double NULSMax, double MyULSMax, double MzULSMax){
    // TODO
}

void SectionSteelOmega::point( int i, double *y, double *z ){
    if( i >= 0 ){
        if( i < m_d->sectionThin->sectionArcModel()->count() * 3 ){
            int j = i / 3;
            int k = i % 3;
            double a = m_d->sectionThin->sectionArc(j)->startAngle->valueNormal() + 0.50 * k * m_d->sectionThin->sectionArc(j)->sweepAngle->valueNormal();
            *y = m_d->sectionThin->sectionArc(j)->C->y->valueNormal() + m_d->sectionThin->sectionArc(j)->R->valueNormal() * cos( a ) ;
            *z = m_d->sectionThin->sectionArc(j)->C->z->valueNormal() + m_d->sectionThin->sectionArc(j)->R->valueNormal() * sin( a ) ;
            return;
        } else if( (i - m_d->sectionThin->sectionArcModel()->count() * 3) < m_d->sectionThin->sectionLineModel()->count() * 3 ){
            int j = i / 3;
            int k = i % 3;
            int Dy = m_d->sectionThin->sectionLine(j)->P2->y->valueNormal() - m_d->sectionThin->sectionLine(j)->P1->y->valueNormal();
            int Dz = m_d->sectionThin->sectionLine(j)->P2->z->valueNormal() - m_d->sectionThin->sectionLine(j)->P1->z->valueNormal();
            *y = m_d->sectionThin->sectionLine(j)->P1->y->valueNormal() + 0.50 * k * Dy;
            *z = m_d->sectionThin->sectionLine(j)->P1->z->valueNormal() + 0.50 * k * Dz;
            return;
        }
    }
    *y = 0.0;
    *z = 0.0;
}

int SectionSteelOmega::nPoints(){
    return (m_d->sectionThin->sectionArcModel()->count() + m_d->sectionThin->sectionLineModel()->count() ) * 3;
}
