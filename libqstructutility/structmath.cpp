#include "structmath.h"

#include <cmath>

int solveTrigonometric( double ret[], double a, double b, double c ){
    if ( a == 0.0 && b == 0.0 && c == 0.0 ){
        return 0;
    }

    int n=0;

    if( c == 0.0) { // a != 0.0 || b != 0.0
        ret[n] = atan2( -b, a );
        n++;
    } else if( a == 0.0){ // c != 0.0
        if( b != 0.0 ){
            if( fabs(c/b) <= 1.0){
                ret[n] = acos( -c / b );
                n++;
            }
        }
    } else if( b == 0.0){ // a != 0.0 && c != 0.0
        if( fabs( c/a ) <= 1.0 ){
            ret[n] = asin( -c / a );
            n++;
        }
    } else if( b == c ){ // c != 0.0 && b!= 0.0 c != 0.0 && b == c
        ret[n] = M_PI;
        n++;
        ret[n] = 2.0 * atan2( -(b+c), 2.0 * a);
        n++;
    } else { // c != 0.0 && b!= 0.0 c != 0.0 && b != c
        double sp[2];
        int nsp = solvePoly2( sp, (c-b), 2.0 * a, b+c );
        for( int i=0; i < nsp; i++){
            ret[i] = 2.0 * atan( sp[i]);
            n++;
        }
    }

    return n;
}

int solvePoly2( double ret[], double a, double b, double c ){
    if ( a == 0.0 && b == 0.0 && c == 0.0 ){
        return 0;
    }

    int n = 0;

    if( a == 0 ){
        if( b != 0){
            ret[n] = -c / b;
            n++;
        }
    } else { // a != 0.0
        double D = b * b - 4.0 * a * c;
        if( D == 0.0 ){
            ret[n] = - b / (2.0 * a);
            n++;
        } else if( D > 0.0 ){
            D = sqrt(D);
            ret[n] = (-b + D) / (2.0 * a);
            n++;
            ret[n] = (-b - D) / (2.0 * a);
            n++;
        }
    }

    return n;
}

int solvePoly2( double realRet[], double imRet[], double a, double b, double c ){
    if( a == 0.0 ){
        if( b == 0.0 ){
            return 0;
        }
        realRet[0] = -c / b;
        imRet[0] = 0.0;
        return 1;
    }
    double D = pow(b, 2.0 ) - 4.0 * a * c;
    if( D < 0.0 ){
        D = sqrt( -D );
        realRet[0] = - b / (2.0 * a);
        imRet[0] = D / (2.0 * a);
        realRet[1] = realRet[0];
        imRet[1] = - imRet[0];
    } else if( D == 0.0 ){
        realRet[0] = - b / (2.0 * a);
        imRet[0] = 0.0;
        realRet[1] = realRet[0];
        imRet[1] = 0.0;
    } else { // D > 0.0
        D = sqrt( D );
        realRet[0] = (-b + D) / (2.0 * a);
        imRet[0] = 0.0;
        realRet[1] = (-b - D) / (2.0 * a);
        imRet[1] = 0.0;
    }
    return 2;
}

bool solveCardano( double * ret, double p, double q){
    double D = pow(q, 2.0) / 4.0 + pow(p, 3.0) / 27.0;
    if( D >= 0.0 ){
        D = pow( D, 0.50 );
        double u = -q/2.0 + D;
        double v = -q/2.0 - D;
        *ret = sgn(u) * pow( sgn(u) * u  ,1.0/3.0) + sgn(v) * pow( sgn(v) * v ,1.0/3.0);
        return true;
    } else {
        return false;
    }
}

int solvePoly3( double ret[], double a, double b, double c, double d ){
    if( a == 0.0 ){
        return solvePoly2( ret, b, c, d );
    }
    int n = 0;
    double p = c / a - pow(b, 2.0) / (3.0 * pow(a, 2.0));
    double q = d / a - b * c / (3.0 * pow(a, 2.0)) + 2.0 * pow(b,3.0) / (27.0 * pow(a,3.0));
    double D = pow(q, 2.0) / 4.0 + pow(p, 3.0) / 27.0;
    if( D > 0.0 ) {
        double val = - q * 0.50 + sqrt(D);
        double u = sgn(val) * pow( val * sgn(val), 1.0 / 3.0 );
        val = - q * 0.50 - sqrt(D);
        double v = sgn(val) * pow( val * sgn(val), 1.0 / 3.0 );
        ret[0] = u + v - b / (3.0 * a);
        n = 1;
    } else if( D < 0.0 ){
        double realP = - q * 0.50;
        double imP = sqrt( -D );
        double theta = atan2( imP, realP );
        if( p < 0.0 ){
            ret[0] = 2.0 * sqrt( -p / 3.0 ) * cos( theta / 3.0 );
            ret[1] = 2.0 * sqrt( -p / 3.0 ) * cos( (theta + 2.0 * M_PI) / 3.0 );
            ret[2] = 2.0 * sqrt( -p / 3.0 ) * cos( (theta + 4.0 * M_PI) / 3.0 );
            n = 3;
        } else if( p == 0 ){
            ret[0] = 0.0;
            ret[1] = 0.0;
            ret[2] = 0.0;
            n = 1;
        }
    } else { // D == 0.0
        ret[0] = - 2.0 * sgn(q) * pow(q*sgn(q)*0.50, 1.0/3.0);
        ret[1] = sgn(q) * pow(q*sgn(q)*0.50, 1.0/3.0);
        ret[2] = ret[1];
        n = 2;
    }
    return n;
}

int solvePoly3( double realRet[], double imRet[], double a, double b, double c, double d ){
    if( a == 0.0 ){
        return solvePoly2( realRet, imRet, b, c, d );
    }
    double p = c / a - pow(b, 2.0) / (3.0 * pow(a, 2.0));
    double q = d / a - b * c / (3.0 * pow(a, 2.0)) + 2.0 * pow(b,3.0) / (27.0 * pow(a,3.0));
    double D = pow(q, 2.0) / 4.0 + pow(p, 3.0) / 27.0;
    if( D > 0.0 ) {
        double val = - q * 0.50 + sqrt(D);
        double u = sgn(val) * pow( val * sgn(val), 1.0 / 3.0 );
        val = - q * 0.50 - sqrt(D);
        double v = sgn(val) * pow( val * sgn(val), 1.0 / 3.0 );
        realRet[0] = u + v - b / (3.0 * a);
        imRet[0] = 0.0;
        realRet[1] = - u * 0.50 - v * 0.50 - b / (3.0 * a);;
        imRet[1] = u * 0.50 * sqrt(3.0) - v * 0.50 * sqrt(3.0);
        realRet[2] = - u * 0.50 - v * 0.50 - b / (3.0 * a);;
        imRet[2] = - u * 0.50 * sqrt(3.0) + v * 0.50 * sqrt(3.0);
    } else if( D < 0.0 ){
        double realP = - q * 0.50;
        double imP = sqrt( -D );
        double theta = atan2( imP, realP );
        if( p > 0.0 ){
            realRet[0] = - b / (3.0 * a);
            imRet[0] = 2.0 * sqrt( p / 3.0 ) * cos( theta / 3.0 );
            realRet[1] = - b / (3.0 * a);
            imRet[1] = 2.0 * sqrt( p / 3.0 ) * cos( (theta + 2.0 * M_PI) / 3.0 );
            realRet[2] = - b / (3.0 * a);
            imRet[2] = 2.0 * sqrt( p / 3.0 ) * cos( (theta + 4.0 * M_PI) / 3.0 );
        } else if( p < 0.0 ){
            realRet[0] = 2.0 * sqrt( -p / 3.0 ) * cos( theta / 3.0 ) - b / (3.0 * a);
            imRet[0] = 0.0;
            realRet[1] = 2.0 * sqrt( -p / 3.0 ) * cos( (theta + 2.0 * M_PI) / 3.0 ) - b / (3.0 * a);
            imRet[1] = 0.0;
            realRet[2] = 2.0 * sqrt( -p / 3.0 ) * cos( (theta + 4.0 * M_PI) / 3.0 ) - b / (3.0 * a);
            imRet[2] = 0.0;
        } else { // p == 0.0
            realRet[0] = - b / (3.0 * a);
            imRet[0] = 0.0;
            realRet[1] = - b / (3.0 * a);
            imRet[1] = 0.0;
            realRet[2] = - b / (3.0 * a);
            imRet[2] = 0.0;
        }
    } else { // D == 0.0
        realRet[0] = - 2.0 * sgn(q) * pow(q*sgn(q)*0.50, 1.0/3.0)  - b / (3.0 * a);;
        imRet[0] = 0.0;
        realRet[1] = sgn(q) * pow(q*sgn(q)*0.50, 1.0/3.0) - b / (3.0 * a);;
        imRet[1] = 0.0;
        realRet[2] = realRet[1]  - b / (3.0 * a);;
        imRet[2] = imRet[1];
    }
    return 3;
}

double normalAngle(double val , double limit){
    double ret = val;
    while( (ret >= (limit + 2.0 * M_PI)) || (ret < limit ) ){
        if( ret >= (limit + 2.0 * M_PI) ){
            ret -= 2.0 * M_PI;
        }
        if( ret < limit ){
            ret += 2.0 * M_PI;
        }
    }
    return ret;
}

void rotate2D( double *y, double *z, double a ){
    double cosa = cos(a);
    double sina = sin(a);
    double yTmp = *y;
    double zTmp = *z;
    *y = yTmp * cosa - zTmp * sina;
    *z = yTmp * sina + zTmp * cosa;
}

bool system2D( double *x, double *y, double a11, double a12, double c1, double a21, double a22, double c2 ){
    double D = a11 * a22 - a21 * a12;
    if( D == 0.0 ){
        return false;
    } else {
        *x = (c1 * a22 - c2 * a12) / D;
        *y = (a11 * c2 - a21 * c1) / D;
        return true;
    }
}

double length2D( double x1, double y1, double x2, double y2){
    return pow( pow(y2-y1, 2.0) + pow(x2-x1, 2.0), 0.50);
}

void middle2D( double *x, double *y, double x1, double y1, double x2, double y2){
    *x = (x1 + x2) / 2.0;
    *y = (y1 + y2) / 2.0;
}

double length3D( double x1, double y1,double z1, double x2, double y2, double z2){
    return pow( pow(z2-z1, 2.0) + pow(y2-y1, 2.0) + pow(x2-x1, 2.0), 0.50);
}

void middle3D( double *x, double *y, double *z, double x1, double y1, double z1, double x2, double y2, double z2){
    *x = (x1 + x2) / 2.0;
    *y = (y1 + y2) / 2.0;
    *z = (z1 + z2) / 2.0;
}

#include <QPointF>

double DyDz(const QPointF& p1, const QPointF& p2) {
    double ret = ( p2.x() - p1.x() ) *
            ( p2.y() + p1.y() ) / 2.0 ;
    return ret;
}

double yDyDz(const QPointF& p1, const QPointF& p2) {
    return ( ( p2.x() - p1.x() ) * \
             ( p2.x() * ( 2.0 * p2.y() + p1.y() ) + \
               p1.x() * ( p2.y() + 2.0 * p1.y() ) ) / 6.0 );
}

double y2DyDz(const QPointF& p1, const QPointF& p2) {
    return ( ( p2.x() - p1.x() ) * \
             (pow(p2.x(), 2.0) * (3.0 * p2.y() + p1.y()) + \
              2.0 * p2.x() * p1.x() * (p2.y()+p1.y()) + \
              pow(p1.x(), 2.0) * (3.0 * p1.y() + p2.y()))/ 12.0 );
}

double y3DyDz(const QPointF& p1, const QPointF& p2) {
    return ( ( p2.x() - p1.x() ) * \
             ( pow(p2.x(), 3.0 ) * (4.0 * p2.y()+p1.y()) +
               pow(p2.x(), 2.0 ) * p1.x() * (3.0 * p2.y()+ 2.0 * p1.y()) + \
               p2.x() * pow(p1.x(), 2.0 ) * (2.0 * p2.y()+ 3.0 * p1.y()) + \
               pow(p1.x(), 3.0 ) * (p2.y()+4.0*p1.y()) ) / 20.0 );
}

double zDyDz(const QPointF& p1, const QPointF& p2) {
    return ( ( p2.x() - p1.x() ) *	\
             ( p2.y() * p2.y() + \
               p2.y() * p1.y() + \
               p1.y() * p1.y()) / 6.0 );
}

double z2DyDz(const QPointF& p1, const QPointF& p2) {
    return ( ( p2.x() - p1.x() ) *	\
             ( p2.y() * p2.y() * p2.y() + \
               p2.y() * p2.y() * p1.y() + \
               p2.y() * p1.y() * p1.y() + \
               p1.y() * p1.y() * p1.y() ) / 12.0 );
}

double z3DyDz(const QPointF& p1, const QPointF& p2) {
    return ( ( p2.x() - p1.x() ) *	\
             ( p2.y() * p2.y() * p2.y() * p2.y() + \
               p2.y() * p2.y() * p2.y() * p1.y() + \
               p2.y() * p2.y() * p1.y() * p1.y() + \
               p2.y() * p1.y() * p1.y() * p1.y() + \
               p1.y() * p1.y() * p1.y() * p1.y() ) / 20.0 ) ;
}

double yzDyDz(const QPointF& p1, const QPointF& p2) {
    return ( ( p2.x() - p1.x() ) * \
             ( pow( p1.y(), 2.0) * ( 3.0*p1.x() + p2.x()) + \
               2.0 * p1.y() * p2.y() * (p1.x() + p2.x()) + \
               pow( p2.y(), 2.0) * ( p1.x() + 3.0*p2.x())) / 24.0 );
}

double yz2DyDz(const QPointF& p1, const QPointF& p2) {
    return ( ( p2.x() - p1.x() ) * \
             ( p1.y() * p1.y() * p1.y() * ( 4.0*p1.x() + p2.x()) + \
               p1.y() * p1.y() * p2.y() * ( 3.0*p1.x() + 2.0*p2.x()) + \
               p1.y() * p2.y() * p2.y() * ( 2.0*p1.x() + 3.0*p2.x()) + \
               p2.y() * p2.y() * p2.y() * ( p1.x() + 4.0*p2.x())) / 60.0 );
}

double y2zDyDz(const QPointF& p1, const QPointF& p2) {
    return ( ( p2.x() - p1.x() ) *	    \
             ( pow(p2.y(), 2.0) * (6.0 * pow(p2.x(), 2.0) + 3.0 * p2.x() * p1.x() + pow(p1.x(), 2.0)) + \
               p2.y() * p1.y() * (3.0 * pow(p2.x(), 2.0) + 4.0 * p2.x() * p1.x() + 3.0 * pow(p1.x(), 2.0)) + \
               pow(p1.y(), 2.0) * (pow(p2.x(), 2.0) + 3.0 * p2.x() * p1.x() + 6.0 * pow(p1.x(), 2.0)) ) / 60.0 );
}

double roundOff( double p_number, double p_precision, double p_anchor){
    if( p_number < p_anchor) {
        return ceil((p_number - p_anchor) / p_precision) * p_precision + p_anchor;
    } else {
        return floor((p_number - p_anchor) / p_precision) * p_precision + p_anchor;
    }
}
