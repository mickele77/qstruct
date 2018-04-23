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

#include "soil.h"

#include "doubleplus.h"
#include "uintplus.h"
#include "boolplus.h"
#include <cmath>

#include <QList>

Soil::Soil( UnitMeasure * ump, const QString &n, Soil::SoilType st, QObject * parent ):
    QObject( parent ),
    VarPlusContainer( ump, "Soil", n ){
    nextId();
    initVar(st);
}

Soil::Soil(UnitMeasure *ump, const QXmlStreamAttributes &attrs, QObject *parent):
    QObject( parent ),
    VarPlusContainer( ump, "Soil", "" ){
    initVar( Soil::sand );
    loadFromXML( attrs );
}

void Soil::initVar( Soil::SoilType st) {
    soilType = new SoilTypePlus( st, "soilType"  );
    soilType->setRichName( trUtf8("Classificazione") );
    soilType->setToolTip( trUtf8("Classificazione del terreno") );
    addVarToContainer( soilType );

    phiPrimeK = new DoublePlus( 34.0*M_PI/180.0, "phiPrimeK", m_unitMeasure, UnitMeasure::angle );;
    phiPrimeK->setRichName( trUtf8("φ'<span style=\" vertical-align:sub;\">k</span>"));
    phiPrimeK->setToolTip( trUtf8("Angolo di attrito interno del terreno in condizioni drenate"));
    addVarToContainer( phiPrimeK );

    cPrimeK = new DoublePlus( 0.0, "cPrimeK", m_unitMeasure, UnitMeasure::tension );;
    cPrimeK->setRichName( trUtf8("c'<span style=\" vertical-align:sub;\">k</span>"));
    cPrimeK->setToolTip( trUtf8("Coesione del terreno in condizioni drenate"));
    addVarToContainer( cPrimeK );

    cUK = new DoublePlus( 0.0, "cUK", m_unitMeasure, UnitMeasure::tension );;
    cUK->setRichName( trUtf8("c<span style=\" vertical-align:sub;\">u,k</span>"));
    cUK->setToolTip( trUtf8("Coesione del terreno in condizioni non drenate"));
    addVarToContainer( cUK );

    gammaW = new DoublePlus( 1.9e+4, "gammaW", m_unitMeasure, UnitMeasure::loadFVolume );
    gammaW->setRichName( trUtf8("γ<span style=\" vertical-align:sub;\">w</span>"));
    gammaW->setToolTip( trUtf8("Peso specifico del terreno"));
    addVarToContainer( gammaW );

    KaTheory = new SoilKTheoryPlus( Soil::Coulomb, "soilKaTheory" );
    KaTheory->setRichName( trUtf8("Ipotesi") );
    KaTheory->setToolTip( trUtf8("Teoria impiegata per il calcolo della spinta attiva") );
    addVarToContainer( KaTheory );

    KpTheory = new SoilKTheoryPlus( Soil::Lancellotta, "soilKpTheory" );
    KpTheory->setRichName( trUtf8("Ipotesi") );
    KpTheory->setToolTip( trUtf8("Teoria impiegata per il calcolo della spinta passiva") );
    addVarToContainer( KpTheory );

    soilEQCategory = new SoilEQCategory( Soil::B, "soilEQCategory");
    soilEQCategory->setRichName( trUtf8("Categoria sismica"));
    soilEQCategory->setToolTip( trUtf8("Categoria sismica del sottosuolo (NTC08 3.2.2)"));
    addVarToContainer( soilEQCategory );
}

double Soil::LPrimeNormal( double L, double M, double V ){
    if( V != 0.0 ){
        return L - 2.0 * fabs(M/V);
    } else if( M == 0.0 ){
        return L;
    } else {
        return 0.0;
    }
}

void Soil::LPrime( DoublePlus * ret, DoublePlus * L, DoublePlus * M, DoublePlus * V ){
    ret->setValueNormal( LPrimeNormal(L->valueNormal(), M->valueNormal(), V->valueNormal()));
}

double Soil::phiPrimeDNormal(double gammaTanPhiPrime) {
    if( gammaTanPhiPrime != 0.0 ){
        return atan( tan(phiPrimeK->valueNormal()) / gammaTanPhiPrime );
    } else {
        return 0.0;
    }
}

void Soil::phiPrimeD(DoublePlus * ret, DoublePlus *gammaTanPhiPrime) {
    ret->setValueNormal( phiPrimeDNormal( gammaTanPhiPrime->valueNormal() ));
}

double Soil::cPrimeDNormal(double gammaCPrime) {
    if( gammaCPrime != 0.0 ){
        return cPrimeK->valueNormal() / gammaCPrime;
    } else {
        return 0.0;
    }
}

void Soil::cPrimeD(DoublePlus * ret, DoublePlus *gammaCPrime) {
    ret->setValueNormal( cPrimeDNormal( gammaCPrime->valueNormal() ) );
}

double Soil::cUD(double gammaCU) {
    if( gammaCU != 0.0 ){
        return cUK->valueNormal() / gammaCU;
    } else {
        return 0.0;
    }
}

void Soil::cUD( DoublePlus * ret, DoublePlus *gammaCU) {
    ret->setValueNormal( gammaCU->valueNormal() );
}

double Soil::gammaWDNormal(double gammaGammaW) {
    if( gammaGammaW != 0.0 ){
        return gammaW->valueNormal() / gammaGammaW;
    } else {
        return 0.0;
    }
}

void Soil::gammaWD(DoublePlus * ret, DoublePlus *gammaGammaW) {
    ret->setValueNormal( gammaWDNormal( gammaGammaW->valueNormal() ) );
}

double Soil::KaCoulombNormal(double phi, double i, double beta, double delta) {
    return pow(cos(phi - beta), 2.0) / ( pow(cos( beta ),2.0) * cos(delta + beta) * \
                                         pow( 1.0 + sqrt( (sin(delta + phi ) * sin(phi - i) ) / (cos (delta + beta) * cos(i - beta) ) ), 2.0 ) );
}

double Soil::KaLancellottaNormal(double phi, double delta) {
    double theta = asin( sin(delta) / sin(phi) ) - delta;
    return (cos(delta) / (1.0 + sin(phi))) * ( cos(delta) - sqrt( pow(sin(phi),2.0) - pow(sin(delta),2.0) ) ) * exp( - 2.0 * theta * tan(phi) );
}

double Soil::KaNormal(double phiD, double i, double beta, double delta){
    if( KaTheory->valueNormal() == Soil::Coulomb ){
        return KaCoulombNormal( phiD, i, beta, delta );
    } else if( KaTheory->valueNormal() == Soil::Lancellotta ){
        return KaLancellottaNormal( phiD, delta );
    }
    return 0.0;
}

void Soil::Ka(DoublePlus *ret, DoublePlus *phiD, DoublePlus *i, DoublePlus *beta, DoublePlus *delta) {
    double ii = 0;
    if( i ){
        ii = i->valueNormal();
    }
    double bb = 0.0;
    if( beta ){
        bb = beta->valueNormal();
    }
    double dd = 0.0;
    if( delta ){
        dd = delta->valueNormal();
    }
    ret->setValueNormal( KaNormal(phiD->valueNormal(), ii, bb, dd) );
}

double Soil::KaeNormal( double theta,
                        double phiD,
                        double i,
                        double beta,
                        double delta ){
    double N1 = sin( phiD + delta) * sin( phiD-theta-i);
    double D1 = cos( delta+beta+theta) * cos(i-beta);
    if( (D1 != 0.0) && (D1 * N1 > 0.0) ){
        double D2 = cos(theta) * pow(cos(beta),2.0) * cos(delta+ beta+theta) * pow((1.0 + sqrt( N1/D1 )),2.0);
        if( D2 != 0.0 ){
            return pow( cos(phiD-theta-beta), 2.0) / D2;
        }
    }
    return 0.0;
}

void Soil::Kae( DoublePlus * ret,
                DoublePlus * theta,
                DoublePlus * phiD,
                DoublePlus * i,
                DoublePlus * beta,
                DoublePlus * delta ){
    double ii = 0;
    if( i ){
        ii = i->valueNormal();
    }
    double bb = 0.0;
    if( beta ){
        bb = beta->valueNormal();
    }
    double dd = 0.0;
    if( delta ){
        dd = delta->valueNormal();
    }
    ret->setValueNormal( KaeNormal(theta->valueNormal(), phiD->valueNormal(), ii, bb, dd) );
}

double Soil::KpCoulombNormal(double phi, double i, double beta, double delta) {
    return pow(cos(phi + beta), 2.0) / ( pow(cos( beta ),2.0) * cos(delta - beta) * \
                                         pow( 1.0 - sqrt( (sin(delta + phi ) * sin(phi + i) ) / (cos (delta - beta) * cos(i - beta) ) ), 2.0 ) );
}

double Soil::KpLancellottaNormal(double phi, double delta) {
    double v = 0.0;
    if( sin(phi) != 0.0 ){
        double theta = asin( sin(delta) / sin(phi) ) + delta;
        double D1 = 1.0 - sin(phi);
        double D2 = 0.0;
        // (con phi == delta, in alcuni casi ho ottenuto D2 negativo)
        if( phi != delta ){
            D2 = pow(sin(phi),2.0) - pow(sin(delta),2.0);
        }
        if( D1 != 0.0 && D2 >= 0.0 ){
            v = (cos(delta) / D1) * ( cos(delta) + sqrt( D2 ) ) * exp( 2.0 * theta * tan(phi) );
        }
    }
    return v;
}

double Soil::KpNormal(double phiD, double i, double beta, double delta) {
    if( KpTheory->valueNormal() == Soil::Coulomb ){
        return KpCoulombNormal( phiD, i, beta, delta );
    } else if( KpTheory->valueNormal() == Soil::Lancellotta ){
        return KpLancellottaNormal( phiD, delta );
    }
    return 0.0;
}

void Soil::Kp(DoublePlus *ret, DoublePlus *phiD, DoublePlus *i, DoublePlus *beta, DoublePlus *delta) {
    double ii= 0.0;
    if( i ){
        ii = i->valueNormal();
    }
    double bb = 0.0;
    if( beta ){
        bb = beta->valueNormal();
    }
    double dd = 0.0;
    if( delta ){
        dd = delta->valueNormal();
    }
    double v = KpNormal( phiD->valueNormal(), ii, bb, dd);
    ret->setValueNormal( v );
}

double Soil::KpeNormal( double theta,
                        double phiD,
                        double i,
                        double beta,
                        double delta ){
    double D1 = cos( delta-beta+theta) * cos(i-beta);
    double N1 = sin( phiD + delta) * sin( phiD-theta+i);
    if( (D1!=0.0) && (D1 * N1 > 0.0)){
        double D2 = cos(theta) * pow(cos(beta),2.0) * cos(delta-beta+theta) * pow((1.0 - sqrt( N1/D1 )),2.0);
        if( D2 != 0.0 ){
            return pow( cos(phiD-theta+beta),2.0) / D2;
        }
    }
    return 0.0;
}

void Soil::Kpe( DoublePlus * ret,
                DoublePlus * theta,
                DoublePlus * phiD,
                DoublePlus * i,
                DoublePlus * beta,
                DoublePlus * delta ){
    double ii = 0;
    if( i ){
        ii = i->valueNormal();
    }
    double bb = 0.0;
    if( beta ){
        bb = beta->valueNormal();
    }
    double dd = 0.0;
    if( delta ){
        dd = delta->valueNormal();
    }
    ret->setValueNormal( KpeNormal(theta->valueNormal(), phiD->valueNormal(), ii, bb, dd) );
}

double Soil::NGammaWPrimeNormal( double phiD ){
    return 2.0 * (NqPrimeNormal(phiD) + 2.0) * tan(phiD);
}

void Soil::NGammaWPrime( DoublePlus * ret, DoublePlus * phiD ){
    ret->setValueNormal( NGammaWPrimeNormal( phiD->valueNormal() ));
}

double Soil::sGammaWPrimeNormal( double phiD, double B, double L, bool infinite ){
    double b = B;
    double l = L;
    if( b > l ){
        b = L;
        l = B;
    }
    if( ( (1-sin( phiD ))* l ) != 0.0 && !infinite){
        return (1.0 + 0.1 * b * (1.0+sin( phiD)) / ( (1-sin(phiD))* l ));
    }
    return 1.0;
}

void Soil::sGammaWPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite ){
    if( LInfinite ){
        ret->setValueNormal( sGammaWPrimeNormal( phiD->valueNormal(), B->valueNormal(), L->valueNormal(), LInfinite->valueNormal() ));
    } else {
        ret->setValueNormal( sGammaWPrimeNormal( phiD->valueNormal(), B->valueNormal(), L->valueNormal(), false ));
    }
}

double Soil::iGammaWPrimeNormal( double phiD, double cd, double V, double H, double B, double L, bool infinite ){
    if( !infinite ){
        double b = B;
        double l = L;
        if( b > l ){
            b = L;
            l = B;
        }
        if( tan(phiD) != 0.0 &&  l != 0.0 ){
            if( (fabs(V)+b*l*cd/tan(phiD) != 0.0) && (1.0+b/l != 0.0 )){
                double m = (2.0 + b/l) / (1.0 + b/l);
                return pow( (1.0 - fabs(H) / (fabs(V)+b*l*cd/tan(phiD)) ),m+1.0);
            }
        }
    } else if( phiD != 0.0 && V != 0.0 ){
        if( fabs(H) < fabs(V) ){
            return pow(1.0 - fabs(H) / fabs(V), 3.0);
        } else {
            return 0.0;
        }
        /* double alpha = atan2( fabs(H), fabs(V) );
        if( alpha < phiD ){
            return pow( 1.0 - alpha / phiD, 2.0 );
        } else {
            return 0.0;
        }*/
    }
    return 1.0;
}

void Soil::iGammaWPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * cd, DoublePlus * V, DoublePlus * H, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite){
    if( LInfinite ){
        ret->setValueNormal( iGammaWPrimeNormal( phiD->valueNormal(), cd->valueNormal(), V->valueNormal(), H->valueNormal(), B->valueNormal(), L->valueNormal(), LInfinite->valueNormal() ));
    } else {
        ret->setValueNormal( iGammaWPrimeNormal( phiD->valueNormal(), cd->valueNormal(), V->valueNormal(), H->valueNormal(), B->valueNormal(), L->valueNormal(), false ));
    }
}

double Soil::bGammaWPrimeNormal( double phiD, double alpha ){
    return pow(1.0 - alpha * tan(phiD), 2.0);
}

void Soil::bGammaWPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * alpha ){
    ret->setValueNormal( bGammaWPrimeNormal( phiD->valueNormal(), alpha->valueNormal()) );
}

double Soil::gGammaWPrimeNormal( double omega ){
    return pow(1.0 - tan(omega), 2.0);
}

void Soil::gGammaWPrime( DoublePlus * ret, DoublePlus * omega ){
    ret->setValueNormal( gGammaWPrimeNormal( omega->valueNormal() ) );
}

double Soil::NcPrimeNormal( double phiD ){
    if( tan(phiD) != 0.0 ){
        return ( NqPrimeNormal( phiD ) - 1.0 ) / tan(phiD);
    }
    return 0.0;
}

void Soil::NcPrime( DoublePlus * ret, DoublePlus * phiD ){
    ret->setValueNormal( NcPrimeNormal( phiD->valueNormal() ));
}

double Soil::scPrimeNormal( double phiD, double B, double L, bool infinite ){
    double b = B;
    double l = L;
    if( b > l ){
        b = L;
        l = B;
    }
    if( ( (1-sin( phiD ))* l ) != 0.0 && !infinite ){
        return 1.0 + 0.2 * b * (1.0+sin(phiD)) / ( (1-sin(phiD))* l);
    }
    return 1.0;
}

void Soil::scPrime( DoublePlus * ret, DoublePlus * phiD , DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite  ){
    if( LInfinite ){
        ret->setValueNormal( scPrimeNormal( B->valueNormal(), L->valueNormal(), phiD->valueNormal(), LInfinite->valueNormal() ) );
    } else {
        ret->setValueNormal( scPrimeNormal( B->valueNormal(), L->valueNormal(), phiD->valueNormal(), false ) );
    }
}

double Soil::icPrimeNormal( double phiD, double cd, double V, double H, double B, double L, bool infinite ){
    if( !infinite ){
        double b = B;
        double l = L;
        if( b > l ){
            b = L;
            l = B;
        }
        double iq = iqPrimeNormal( phiD, cd, V, H, b, l, infinite );
        double Nc = NcPrimeNormal( phiD );
        if( Nc * tan(phiD) != 0.0 ){
            return iq - (1.0-iq) / (Nc*tan(phiD));
        }
    } else if( V != 0.0 ){
        double iq = pow( 1.0 - fabs(H)/fabs(V), 2.0 );
        double Nc = NcPrimeNormal( phiD );
        if( Nc * tan(phiD) != 0.0 ){
            return iq - (1.0-iq) / (Nc*tan(phiD));
        }
        /* double alpha = atan2( H, V);
        return pow(1.0 - 2.0 * alpha / M_PI, 2.0); */
    }
    return 1.0;
}

void Soil::icPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * cd, DoublePlus * V, DoublePlus * H, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite){
    if( LInfinite ){
        ret->setValueNormal( iqPrimeNormal( phiD->valueNormal(), cd->valueNormal(), V->valueNormal(), H->valueNormal(), B->valueNormal(), L->valueNormal(), LInfinite->valueNormal()) );
    } else {
        ret->setValueNormal( iqPrimeNormal( phiD->valueNormal(), cd->valueNormal(), V->valueNormal(), H->valueNormal(), B->valueNormal(), L->valueNormal(), false ) );
    }
}

double Soil::bcPrimeNormal( double phiD, double alpha ){
    double bq = bqPrimeNormal( phiD, alpha );
    double Nc = NcPrimeNormal( phiD );
    if( Nc * tan(phiD) != 0.0 ){
        return bq - (1.0 - bq) / (Nc *tan(phiD));
    }
    return 1.0;
}

void Soil::bcPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * alpha ){
    ret->setValueNormal( bcPrimeNormal( phiD->valueNormal(), alpha->valueNormal()) );
}


double Soil::gcPrimeNormal( double phiD, double omega ){
    double Nc = NcPrimeNormal( phiD );
    if( Nc * tan(phiD) != 0.0 ){
        double gq = gqPrimeNormal( omega );
        return gq - (1.0 - gq) / (Nc * tan(phiD));
    }
    return 1.0;
}

void Soil::gcPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * omega ){
    ret->setValueNormal( gcPrimeNormal(phiD->valueNormal(), omega->valueNormal()) );
}

double Soil::dcPrimeNormal( double phiD, double D, double Bp, double Lp, bool LInfinite ){
    double den = NcPrimeNormal( phiD ) * tan(phiD);
    if( den != 0.0 ){
        double dq = dqPrimeNormal( phiD, D, Bp, Lp, LInfinite );
        return dq - (1.0 - dq) / den;
    }
    return 1.0;
}

void Soil::dcPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * D, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite ){
    ret->setValueNormal( dcPrimeNormal(phiD->valueNormal(), D->valueNormal(), B->valueNormal(), L->valueNormal(), LInfinite->valueNormal()));
}

double Soil::NqPrimeNormal( double phiD ){
    if( (1-sin(phiD)) != 0.0 ){
        return (1.0 + sin(phiD)) / (1.0-sin(phiD))  * exp(M_PI * tan(phiD));
    }
    return 0.0;
}

void Soil::NqPrime( DoublePlus * ret, DoublePlus * phiD ){
    ret->setValueNormal( NqPrimeNormal(phiD->valueNormal()) );
}

double Soil::sqPrimeNormal( double phiD, double B, double L, bool infinite ){
    double b = B;
    double l = L;
    if( b > l ){
        b = L;
        l = B;
    }
    if( ( (1-sin( phiD ))* l ) != 0.0 && !infinite ){
        return 1.0 + 0.2 * b * (1.0+sin(phiD)) / ( (1-sin(phiD))* l);
    }
    return 1.0;
}

void Soil::sqPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite  ){
    if( LInfinite ){
        ret->setValueNormal( sqPrimeNormal( B->valueNormal(), L->valueNormal(), phiD->valueNormal(), LInfinite->valueNormal() ) );
    } else {
        ret->setValueNormal( sqPrimeNormal( B->valueNormal(), L->valueNormal(), phiD->valueNormal(), false ) );
    }
}

double Soil::iqPrimeNormal( double phiD, double cd, double V, double H, double B, double L, bool infinite ){
    if( !infinite ){
        double b = B;
        double l = L;
        if( b > l ){
            b = L;
            l = B;
        }
        if( tan(phiD != 0.0) && l != 0.0){
            if( (fabs(V)+b*l*cd/tan(phiD) != 0.0) && (1.0+B/L != 0.0 )){
                double m = (2.0 + B/L) / (1.0 + b/l);
                return pow( (1.0 - fabs(H) / (fabs(V)+b*l*cd/tan(phiD)) ),m);
            }
        }
    } else if( V != 0.0 ){
        return pow( 1.0 - fabs(H)/fabs(V), 2.0 );
        /* double alpha = atan2( H, V);
        return pow(1.0-2.0*alpha / M_PI, 2.0); */
    }
    return 1.0;
}

void Soil::iqPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * cd, DoublePlus * V, DoublePlus * H, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite){
    if( LInfinite ){
        ret->setValueNormal( iqPrimeNormal( phiD->valueNormal(), cd->valueNormal(), V->valueNormal(), H->valueNormal(), B->valueNormal(), L->valueNormal(), LInfinite->valueNormal() ));
    } else {
        ret->setValueNormal( iqPrimeNormal( phiD->valueNormal(), cd->valueNormal(), V->valueNormal(), H->valueNormal(), B->valueNormal(), L->valueNormal(), false ));
    }
}

double Soil::bqPrimeNormal( double phiD, double alpha ){
    return pow(1.0 - alpha * tan(phiD), 2.0);
}

void Soil::bqPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * alpha ){
    ret->setValueNormal( bqPrimeNormal( phiD->valueNormal(), alpha->valueNormal()) );
}

double Soil::gqPrimeNormal( double omega ){
    return pow(1.0 - tan(omega), 2.0);
}

void Soil::gqPrime( DoublePlus * ret, DoublePlus * omega ){
    ret->setValueNormal( gqPrimeNormal(omega->valueNormal()) );
}

double Soil::dqPrimeNormal( double phiD, double D, double B, double L, bool infinite ){
    double b = B;
    if( !infinite ){
        if( b > L )
            b = L;
    }
    if( b != 0.0 ){
        if( D <= B ){
            return 1.0 + 2.0 * tan( phiD ) * pow((1.0 - sin(phiD)),2.0) * D / b;
        }  else {
            return 1.0 + 2.0 * tan(phiD) * pow((1.0 - sin(phiD)),2.0) * atan2(D, b);
        }
    }
    return 1.0;
}

void Soil::dqPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus *D, DoublePlus *B, DoublePlus *L, BoolPlus * LInfinite ){
    ret->setValueNormal( dqPrimeNormal(phiD->valueNormal(), D->valueNormal(), B->valueNormal(), L->valueNormal(), LInfinite->valueNormal() ));
}

double Soil::qLimPrimeNormal( double gammaWDes, double cDes, double qDes, double BDes, double LDes, bool LDesInfinite,
                              double NGammaWDes, double iGammaWDes, double sGammaWDes, double bGammaWDes, double gGammaWDes,
                              double NcDes, double icDes, double scDes, double bcDes, double gcDes, double dcDes,
                              double NqDes, double iqDes, double sqDes, double bqDes, double gqDes, double dqDes ){
    double B = BDes;
    if( !LDesInfinite && B > LDes ){
        B = LDes;
    }
    return ( 0.50 * gammaWDes * BDes * NGammaWDes * iGammaWDes * sGammaWDes * bGammaWDes * gGammaWDes +
             + cDes * NcDes * icDes * scDes * bcDes * gcDes * dcDes +
             + qDes * NqDes * iqDes * sqDes * bqDes * gqDes * dqDes );
}
void Soil::qLimPrime( DoublePlus * ret,
                      DoublePlus * gammaWDes, DoublePlus * cDes, DoublePlus * qDes, DoublePlus * BDes, DoublePlus * LDes, BoolPlus * LDesInfinite,
                      DoublePlus * NGammaWDes, DoublePlus * iGammaWDes, DoublePlus * sGammaWDes, DoublePlus * bGammaWDes, DoublePlus * gGammaWDes,
                      DoublePlus * NcDes, DoublePlus * icDes, DoublePlus * scDes, DoublePlus * bcDes, DoublePlus * gcDes, DoublePlus * dcDes,
                      DoublePlus * NqDes, DoublePlus * iqDes, DoublePlus * sqDes, DoublePlus * bqDes, DoublePlus * gqDes, DoublePlus * dqDes ){
    ret->setValueNormal( qLimPrimeNormal( gammaWDes->valueNormal(), cDes->valueNormal(), qDes->valueNormal(), BDes->valueNormal(), LDes->valueNormal(), LDesInfinite->valueNormal(),
                                          NGammaWDes->valueNormal(), iGammaWDes->valueNormal(), sGammaWDes->valueNormal(), bGammaWDes->valueNormal(), gGammaWDes->valueNormal(),
                                          NcDes->valueNormal(), icDes->valueNormal(), scDes->valueNormal(), bcDes->valueNormal(), gcDes->valueNormal(), dcDes->valueNormal(),
                                          NqDes->valueNormal(), iqDes->valueNormal(), sqDes->valueNormal(), bqDes->valueNormal(), gqDes->valueNormal(), dqDes->valueNormal() ));
}

double Soil::QLimVRdPrimeNormal( double qLimD, double B, double L ){
    return B * L * qLimD;
}

void Soil::QLimVRdPrime( DoublePlus * ret, DoublePlus * qLimD, DoublePlus * B, DoublePlus * L ){
    ret->setValueNormal( QLimVRdPrimeNormal( qLimD->valueNormal(), B->valueNormal(), L->valueNormal() ) );
}

#include <QCoreApplication>

class SoilTypePlusPrivate{
    Q_DECLARE_TR_FUNCTIONS(SoilTypePlusPrivate)

    class enumVal{
    public:
        enumVal(Soil::SoilType v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        Soil::SoilType val;
        QString normalStr;
        QString str;
    };

public:
    SoilTypePlusPrivate(Soil::SoilType v):
        value(v){
        enumList.append( enumVal( Soil::clay, "clay", trUtf8("Terreno coesivo")) );
        enumList.append( enumVal( Soil::sand, "sand", trUtf8("Terreno granulare")) );
    };
    ~SoilTypePlusPrivate(){
    };

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    Soil::SoilType value;
    QList<enumVal> enumList;
};

SoilTypePlus::SoilTypePlus( Soil::SoilType tt,
                            const QString & nn,
                            bool ro):
    EnumPlus( nn, ro),
    m_d( new SoilTypePlusPrivate(tt) ){
}

SoilTypePlus::~SoilTypePlus(){
    delete m_d;
}

SoilTypePlus & SoilTypePlus::operator=( SoilTypePlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & SoilTypePlus::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        SoilTypePlus * valCncrModelPlus = dynamic_cast<SoilTypePlus *>(&val);
        if(valCncrModelPlus)
        {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString SoilTypePlus::typeVar(){
    return QString("SoilTypePlus");
}

Soil::SoilType SoilTypePlus::value(){
    return m_d->value;
}

Soil::SoilType SoilTypePlus::valueNormal(){
    return m_d->value;
}

QString SoilTypePlus::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString SoilTypePlus::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void SoilTypePlus::setValueNormal( Soil::SoilType vv, bool emitAuto ){
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

void SoilTypePlus::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SoilTypePlus::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void SoilTypePlus::setValue( Soil::SoilType vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void SoilTypePlus::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int SoilTypePlus::enumCount() {
    return m_d->enumList.size();
}

QString SoilTypePlus::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int SoilTypePlus::valueNormalInt() {
    return m_d->valueIndex();
}

class SoilKTheoryPlusPrivate{
    Q_DECLARE_TR_FUNCTIONS(SoilKTheoryPlusPrivate)

    class enumVal{
    public:
        enumVal(Soil::SoilKTheory v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        Soil::SoilKTheory val;
        QString normalStr;
        QString str;
    };

public:
    SoilKTheoryPlusPrivate(Soil::SoilKTheory v):
        value(v){
        // Superfici di rottura piane
        enumList.append( enumVal( Soil::Coulomb, "Coulomb", trUtf8("Teoria di Coulomb")) );
        // Metodo statico, presenza di attrito tra muro e terreno
        enumList.append( enumVal( Soil::Lancellotta, "Lancellotta", trUtf8("Lancellotta")) );
    };
    ~SoilKTheoryPlusPrivate(){
    };

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    Soil::SoilKTheory value;
    QList<enumVal> enumList;
};

SoilKTheoryPlus::SoilKTheoryPlus( Soil::SoilKTheory tt,
                                  const QString & nn,
                                  bool ro):
    EnumPlus( nn, ro),
    m_d( new SoilKTheoryPlusPrivate(tt) ){
}

SoilKTheoryPlus::~SoilKTheoryPlus(){
    delete m_d;
}

SoilKTheoryPlus & SoilKTheoryPlus::operator=( SoilKTheoryPlus & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & SoilKTheoryPlus::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        SoilKTheoryPlus * valCncrModelPlus = dynamic_cast<SoilKTheoryPlus *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString SoilKTheoryPlus::typeVar(){
    return QString("SoilKTheoryPlus");
}

Soil::SoilKTheory SoilKTheoryPlus::value(){
    return m_d->value;
}

Soil::SoilKTheory SoilKTheoryPlus::valueNormal(){
    return m_d->value;
}

QString SoilKTheoryPlus::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString SoilKTheoryPlus::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void SoilKTheoryPlus::setValueNormal( Soil::SoilKTheory vv, bool emitAuto ){
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

void SoilKTheoryPlus::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SoilKTheoryPlus::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void SoilKTheoryPlus::setValue( Soil::SoilKTheory vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void SoilKTheoryPlus::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int SoilKTheoryPlus::enumCount() {
    return m_d->enumList.size();
}

QString SoilKTheoryPlus::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int SoilKTheoryPlus::valueNormalInt() {
    return m_d->valueIndex();
}

class SoilEQCategoryPrivate{
    Q_DECLARE_TR_FUNCTIONS(SoilEQCategoryPrivate)

    class enumVal{
    public:
        enumVal(Soil::EQCategory v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        Soil::EQCategory val;
        QString normalStr;
        QString str;
    };

public:
    SoilEQCategoryPrivate(Soil::EQCategory v):
        value(v){
        enumList.append( enumVal( Soil::A, "A", trUtf8("A")) );
        enumList.append( enumVal( Soil::B, "B", trUtf8("B")) );
        enumList.append( enumVal( Soil::C, "C", trUtf8("C")) );
        enumList.append( enumVal( Soil::D, "D", trUtf8("D")) );
        enumList.append( enumVal( Soil::E, "E", trUtf8("E")) );
    };
    ~SoilEQCategoryPrivate(){
    };

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    Soil::EQCategory value;
    QList<enumVal> enumList;
};

SoilEQCategory::SoilEQCategory( Soil::EQCategory tt,
                                const QString & nn,
                                bool ro):
    EnumPlus( nn, ro),
    m_d( new SoilEQCategoryPrivate(tt) ){
}

SoilEQCategory::~SoilEQCategory(){
    delete m_d;
}

SoilEQCategory & SoilEQCategory::operator=( SoilEQCategory & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & SoilEQCategory::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        SoilEQCategory * valCncrModelPlus = dynamic_cast<SoilEQCategory *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString SoilEQCategory::typeVar(){
    return QString("SoilEQCategory");
}

Soil::EQCategory SoilEQCategory::value(){
    return m_d->value;
}

Soil::EQCategory SoilEQCategory::valueNormal(){
    return m_d->value;
}

QString SoilEQCategory::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString SoilEQCategory::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void SoilEQCategory::setValueNormal( Soil::EQCategory vv, bool emitAuto ){
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

void SoilEQCategory::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SoilEQCategory::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void SoilEQCategory::setValue( Soil::EQCategory vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void SoilEQCategory::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int SoilEQCategory::enumCount() {
    return m_d->enumList.size();
}

QString SoilEQCategory::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int SoilEQCategory::valueNormalInt() {
    return m_d->valueIndex();
}
