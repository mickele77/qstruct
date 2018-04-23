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
#include "sectionsteelprofile.h"

#include "steel.h"
#include "doubleplus.h"
#include "point2dmodel.h"

#include <cmath>

SectionSteelProfile::SectionSteelProfile(Steel * st, UnitMeasure * ump):
    QObject(),
    m_unitMeasure( ump ),
    m_steel( st ),
    m_sectionPoints(NULL){
}

SectionSteelProfile::~SectionSteelProfile() {
    if( m_sectionPoints != NULL ){
        delete m_sectionPoints;
    }
}

Point2DModel *SectionSteelProfile::sectionPoints() {
    if( m_sectionPoints == NULL ){
        m_sectionPoints = new Point2DModel();
        updateSectionPoints();
    }
    return m_sectionPoints;
}

int SectionSteelProfile::internalPartClass( double c, double t, double e1, double e2){
    if( e1 <= 0.0 && e2 <= 0.0){
        return 0;
    }
    if( e1 < e2 ){
        double c = e1;
        e1 = e2;
        e2 = c;
    }
    if( (t * fykNormal()) != 0.0 ){
        double eps = pow( 2.35e+8 /  fykNormal(), 0.50 );
        double alpha = 1.0 ;
        if( e2 < 0.0 ){
            alpha = e1 / (e1 - e2);
        }
        double ratio = (c / t) / eps;
        if( alpha > 0.50 ){
            if( ratio <= (396.0 / (13.0 * alpha - 1.0 )) )
                return 1;
        } else {
            if( ratio <= (36.0 / alpha) )
                return 1;
        }
        if( alpha > 0.50 ){
            if( ratio <= (456.0 / (13.0 * alpha - 1.0 )) )
                return 2;
        } else {
            if( ratio <= (41.5 / alpha) )
                return 2;
        }

        double eMax = e1;
        double eMin = e2;
        if( eMin > eMax ){
            eMax = e2;
            eMin = e1;
        }
        double phi = 0.0;
        if( eMax != 0.0){
            phi = eMin / eMax;
        }
        if( phi > -1.0 ){
            if( ratio <= (42.0 / (0.67+0.33*phi))){
                return 3;
            }
        } else {
            if( ratio <= (62.0 * (1.0 - phi) * pow(-phi, 0.50)) ){
                return 3;
            }
        }
        return 4;
    }
    return 0;
}

int SectionSteelProfile::outstandFlangeClass( double c, double t, double e1, double e2){
    if( e1 <= 0.0 && e2 <= 0.0){
        return 0;
    }
    if( (t * fykNormal()) != 0.0 ){
        double eps = pow( 2.35e+8 /  fykNormal(), 0.50 );
        double alpha;
        if( e1 * e2 < 0.0  ){
            alpha = -e1 / e2;
            if( alpha > 1.0 ){
                alpha = 1.0 / alpha;
            }
        } else {
            alpha = 1.0;
        }
        double ratio = (c / t) / eps;
        if( e2 > 0.0 ){
            if( ratio <= (9.0 / alpha) )
                return 1;
        } else {
            if( ratio <= (9.0 / (alpha * pow(alpha, 0.50))) )
                return 1;
        }
        if( e2 > 0.0 ){
            if( ratio <= (10.0 / alpha) )
                return 2;
        } else {
            if( ratio <= (10.0 / (alpha * pow(alpha, 0.50))) )
                return 2;
        }

        double eMax = e1;
        double eMin = e2;
        if( eMin > eMax ){
            eMax = e2;
            eMin = e1;
        }
        double phi = 0.0;
        if( eMax !=  0.0 ){
            phi = eMin / eMax;
        }

        double ks = 0.43;

        if( e1 > e2 ){
            if( phi >= 1.0 ) {
                ks =0.43;
            } else if( phi < 1.0 && phi >= 0.0) {
                ks = 0.578 / (phi + 0.34);
            } else if( phi < 0.0 && phi > -1 ){
                ks = 1.70 - 5.0 * phi + 17.1 * pow(phi, 2.0);
            } else if( phi <= -1 ){
                ks = 23.8;
            }
        } else if( e2 > e1 ){
            if( phi >= 1.0 ){
                ks =0.43;
            } else if( phi < 1.0 && phi >  -3.0) {
                ks = 0.57 - 0.21 * phi + 0.07 * pow(phi, 2.0);
            } else if( phi <= -3 ){
                ks =1.83;
            }
        }

        if( ratio <= (21.0 * pow(ks,0.50))){
            return 3;
        }
        return 4;
    }
    return 0;
}

double SectionSteelProfile::fykNormal(){
    if( m_steel != NULL ){
        if( maxTNormal() > 0.04 ){
            return m_steel->fyk40->valueNormal();
        } else {
            return m_steel->fyk->valueNormal();
        }
    }
    return 0.0;
}

double SectionSteelProfile::fydNormal( int i ){
    if( i >= 0 && i < 3 && m_steel != NULL ){
        if( maxTNormal() > 0.04 ){
            return m_steel->fyd40(i)->valueNormal();
        } else {
            return m_steel->fyd(i)->valueNormal();
        }
    }
    return 0.0;
}



double SectionSteelProfile::MyClassLimitNormal(){
    return 0.0;
}

double SectionSteelProfile::MzClassLimitNormal(){
    return 0.0;
}

void SectionSteelProfile::setSteel( Steel * st ){
    m_steel = st;
}

Steel * SectionSteelProfile::steel(){
    return m_steel;
}

double SectionSteelProfile::Theta( double N, double My, double Mz, double NULSMax, double MyULSMax, double MzULSMax){
    double n = 0.0;
    if( NULSMax != 0.0 ){
        n = N * NULSMax;
    }
    double my = 0.0;
    if( MyULSMax != 0.0 ){
        my = My / MyULSMax;
    }
    double mz = 0.0;
    if( MzULSMax != 0.0 ){
        mz = Mz / MzULSMax;
    }
    double d = sqrt( pow(my,2.0) + pow(mz,2.0) );
    double ret = atan2( n, d );
    if( std::isnan( ret ) ){
        return 0.0;
    } else {
        return ret;
    }
}

double SectionSteelProfile::xi( double My, double Mz, double MyULSMax, double MzULSMax){
    double my = 0.0;
    if( MyULSMax != 0.0 ){
        my = My / MyULSMax;
    }
    double mz = 0.0;
    if( MzULSMax != 0.0 ){
        mz = Mz / MzULSMax;
    }
    double ret = fabs( atan2(mz, my ) );
    if( std::isnan(ret) ){
        return 0.0;
    } else {
        return ret;
    }
}

void SectionSteelProfile::NMULSElastPlastNormal( double * NRet, double * MyRet, double * MzRet,
                                                 double l, double my, double mz,
                                                 double yCen, double zCen ){
    if( NRet ){
        *NRet = 0.0;
    }
    if( MyRet ){
        *MyRet = 0.0;
    }
    if( MzRet ){
        *MzRet = 0.0;
    }
    if( m_steel ){
        if( m_steel->E->valueNormal() ){
            updateSectionPoints();

            Point2DModel sectPlus;
            Point2DModel sectMinus;

            m_sectionPoints->cut( &sectPlus, &sectMinus, l, my, mz, 0.0 );

            Point2DModel sectPlusElast;
            Point2DModel sectPlusPlast;
            Point2DModel sectMinusElast;
            Point2DModel sectMinusPlast;

            sectPlus.cut( &sectPlusPlast, &sectPlusElast, l, my, mz, fydNormal(0)/steel()->E->valueNormal() );
            sectMinus.cut( &sectMinusElast, &sectMinusPlast, l, my, mz, -fydNormal(0)/steel()->E->valueNormal() );

            double AElast = sectPlusElast.ANormal() + sectMinusElast.ANormal();
            double SyElast = sectPlusElast.SyNormal() + sectMinusElast.SyNormal();
            double SzElast = sectPlusElast.SzNormal() + sectMinusElast.SzNormal();
            double IyElast = sectPlusElast.IyyNormal() + sectMinusElast.IyyNormal();
            double IzElast = sectPlusElast.IzzNormal() + sectMinusElast.IzzNormal();
            double IyzElast = sectPlusElast.IyzNormal() + sectMinusElast.IyzNormal();
            double APlast = sectPlusPlast.ANormal() - sectMinusPlast.ANormal();
            double SyPlast = sectPlusPlast.SyNormal() - sectMinusPlast.SyNormal();
            double SzPlast = sectPlusPlast.SzNormal() - sectMinusPlast.SzNormal();

            if( NRet ){
                *NRet = steel()->E->valueNormal() * ( l * AElast + my * SyElast + mz * SzElast ) + fydNormal(0) * APlast;
            }
            if( MyRet ){
                *MyRet = steel()->E->valueNormal() * ( l * ( SyElast - zCen * AElast) + my * (IyElast - zCen * SyElast) + mz * (IyzElast - zCen * SzElast ) ) \
                        + fydNormal(0) * ( SyPlast - zCen * APlast );
            }
            if( MzRet ){
                *MzRet = - (steel()->E->valueNormal() * ( l * ( SzElast - yCen * AElast) + my * (IyzElast - yCen * SyElast) + mz * (IzElast - yCen * SzElast ) ) \
                            + fydNormal(0) * ( SzPlast - yCen * APlast ) );
            }
        }
    }
}

void SectionSteelProfile::NMULSPlastNormal( double *NRet, double *MyRet, double * MzRet,
                                            double l, double my, double mz,
                                            double yCen, double zCen){
    if( m_sectionPoints == NULL ){
        m_sectionPoints = new Point2DModel();
    }

    // updateSectionSteelProfile();

    Point2DModel sectPlus;
    Point2DModel sectMinus;

    m_sectionPoints->cut( &sectPlus, &sectMinus, l, my, mz, 0.0 );

    if( NRet ){
        *NRet = sectPlus.ANormal() - sectMinus.ANormal();
        *NRet *= fydNormal(0);
    }
    if( MyRet ){
        *MyRet = ((sectPlus.SyNormal() - zCen * sectPlus.ANormal()) - (sectMinus.SyNormal() - zCen * sectMinus.ANormal()));
        *MyRet *= fydNormal(0);
    }
    if( MzRet ){
        *MzRet = -((sectPlus.SzNormal() - yCen * sectPlus.ANormal()) - (sectMinus.SzNormal() - yCen * sectMinus.ANormal()));
        *MzRet *= fydNormal(0);
    }
}

void SectionSteelProfile::suspendSignals(bool susp) {
    m_signalsSuspended = susp;
}
