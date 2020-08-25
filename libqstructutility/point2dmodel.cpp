#include "point2dmodel.h"

#include "structmath.h"

#include <QList>
#include <QPolygonF>
#include <QPointF>
#include <cmath>

Point2DModel::Point2DModel() :
    QPolygonF() {
}

Point2DModel::Point2DModel( Point2DModel & val ):
    QPolygonF( val ) {
}

/*Point2DModel &Point2DModel::operator=(const Point2DModel &cp) {
    this->QPolygonF::operator =(cp);

    return *this;
}*/

Point2DModel::~Point2DModel() {
}

double Point2DModel::yNormal(int i) {
    return at(i).x();
}

double Point2DModel::zNormal(int i) {
    return at(i).y();
}

void Point2DModel::setYNormal(int i, double value) {
    if( i < size() ){
        this->operator [](i).setX( value );
    }
}

void Point2DModel::setZNormal(int i, double value) {
    if( i < size() ){
        this->operator [](i).setY( value );
    }
}

void Point2DModel::setYZNormal(int i, double yValue, double zValue) {
    if( i < size() ){
        this->operator [](i).setX( yValue );
        this->operator [](i).setY( zValue );
    }
}

int Point2DModel::nPoints() {
    return size();
}

void Point2DModel::resize(int newSize) {
    QPolygonF::resize( newSize );
}

void Point2DModel::translateNormal( double Dy, double Dz){
    translate( Dy, Dz);
}

void Point2DModel::rotateNormal( double rot){
    for( QPolygonF::iterator iter = begin(); iter != end(); ++iter ){
        double xTmp = iter->x();
        double yTmp = iter->y();
        rotate2D( &xTmp, &yTmp, rot );
        iter->setX( xTmp );
        iter->setY( yTmp );
    }
}

void Point2DModel::insertPoints(int position, int count) {
    insertPoints( position, count, 0.0, 0.0);
}

void Point2DModel::insertPoints( int position, int count, double yVal, double zVal ){
    int row = position;
    while( row < (position+count) ) {
        insert( row, QPointF( yVal, zVal ) );
        ++row;
    }
}

void Point2DModel::appendPoints( int count, double yVal, double zVal ){
    for( int i=0; i < count; ++i){
        append( QPointF(yVal, zVal));
    }
}

void Point2DModel::appendPoint( double yVal, double zVal ){
    append( QPointF(yVal, zVal) );
}

void Point2DModel::removePoints( int position, int count ){
    remove( position, count );
}

double Point2DModel::yMaxNormal(){
    double ret = 0.0;
    QPolygonF::iterator iter = begin();
    if( iter != end() ){
        ret = iter->x();
        iter++;
    }
    while( iter != end() ){
        if( iter->x() > ret ){
            ret = iter->x();
        }
        iter++;
    }
    return ret;
}

double Point2DModel::yMinNormal(){
    double ret = 0.0;
    QPolygonF::iterator iter = begin();
    if( iter != end() ){
        ret = iter->x();
        iter++;
    }
    while( iter != end() ){
        if( iter->x() < ret ){
            ret = iter->x();
        }
        iter++;
    }
    return ret;
}

double Point2DModel::zMaxNormal(){
    double ret = 0.0;
    QPolygonF::iterator iter = begin();
    if( iter != end() ){
        ret = iter->y();
        iter++;
    }
    while( iter != end() ){
        if( iter->y() > ret ){
            ret = iter->y();
        }
        iter++;
    }
    return ret;
}

double Point2DModel::zMinNormal(){
    double ret = 0.0;
    QPolygonF::iterator iter = begin();
    if( iter != end() ){
        ret = iter->y();
        iter++;
    }
    while( iter != end() ){
        if( iter->y() < ret ){
            ret = iter->y();
        }
        iter++;
    }
    return ret;
}

bool Point2DModel::sMinNormal( double * ret, double my, double mz ){
    bool wasRetSet = false;
    *ret = 0.0;
    QPolygonF::iterator iter = begin();
    if( iter != end() ){
        *ret = my * iter->x() + mz * iter->y();
        wasRetSet = true;
        iter++;
    }
    while( iter != end() ){
        double retCmp = my * iter->x() + mz * iter->y();
        if( retCmp < *ret ){
            *ret = retCmp;
        }
        iter++;
    }
    return wasRetSet;
}

bool Point2DModel::sMaxNormal( double *ret, double my, double mz ){
    bool wasRetSet = false;
    *ret = 0.0;
    QPolygonF::iterator iter = begin();
    if( iter != end() ){
        *ret = my * iter->x() + mz * iter->y();
        wasRetSet = true;
        iter++;
    }
    while( iter != end() ){
        double retCmp = my * iter->x() + mz * iter->y();
        if( retCmp > *ret ){
            *ret = retCmp;
        }
        iter++;
    }
    return wasRetSet;
}

double Point2DModel::ANormal(){
    double ret = 0.0;
    if( size() > 2 ){
        QPolygonF::iterator p1 = begin();
        QPolygonF::iterator p2 = p1;
        p2++;
        while( p2 != end() ){
            ret -= DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= DyDz( *p1, *(begin()) );
    }
    return ret;
}

double Point2DModel::SyNormal(){
    double ret = 0.0;
    if( size() > 2 ){
        QPolygonF::iterator p1 = begin();
        QPolygonF::iterator p2 = p1;
        p2++;
        while( p2 != end() ){
            ret -= zDyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= zDyDz( *p1, *(begin()) );
    }
    return ret;
}

double Point2DModel::SzNormal(){
    double ret = 0.0;
    if( size() > 2 ){
        QPolygonF::iterator p1 = begin();
        QPolygonF::iterator p2 = p1;
        p2++;
        while( p2 != end() ){
            ret -= yDyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= yDyDz( *p1, *(begin()) );
    }
    return ret;
}

double Point2DModel::IyyNormal(){
    double ret = 0.0;
    if( size() > 2 ){
        QPolygonF::iterator p1 = begin();
        QPolygonF::iterator p2 = p1;
        p2++;
        while( p2 != end() ){
            ret -= z2DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= z2DyDz( *p1, *(begin()) );
    }
    return ret;
}

double Point2DModel::IzzNormal(){
    double ret = 0.0;
    if( size() > 2 ){
        QPolygonF::iterator p1 = begin();
        QPolygonF::iterator p2 = p1;
        p2++;
        while( p2 != end() ){
            ret -= y2DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= y2DyDz( *p1, *(begin()) );
    }
    return ret;
}

double Point2DModel::IyzNormal(){
    double ret = 0.0;
    if( size() > 2 ){
        QPolygonF::iterator p1 = begin();
        QPolygonF::iterator p2 = p1;
        p2++;
        while( p2 != end() ){
            ret -= yzDyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= yzDyDz( *p1, *(begin()) );
    }
    return ret;
}

double Point2DModel::Sz3DyDzNormal(){
    double ret = 0.0;
    if( size() > 2 ){
        QPolygonF::iterator p1 = begin();
        QPolygonF::iterator p2 = p1;
        p2++;
        while( p2 != end() ){
            ret -= z3DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= z3DyDz( *p1, *(begin()) );
    }
    return ret;
}

double Point2DModel::Sy3DyDzNormal(){
    double ret = 0.0;
    if( size() > 2 ){
        QPolygonF::iterator p1 = begin();
        QPolygonF::iterator p2 = p1;
        p2++;
        while( p2 != end() ){
            ret -= y3DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= y3DyDz( *p1, *(begin()) );
    }
    return ret;
}

double Point2DModel::Syz2DyDzNormal(){
    double ret = 0.0;
    if( size() > 2 ){
        QPolygonF::iterator p1 = begin();
        QPolygonF::iterator p2 = p1;
        p2++;
        while( p2 != end() ){
            ret -= yz2DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= yz2DyDz( *p1, *(begin()) );
    }
    return ret;
}

double Point2DModel::Sy2zDyDzNormal(){
    double ret = 0.0;
    if( size() > 2 ){
        QPolygonF::iterator p1 = begin();
        QPolygonF::iterator p2 = p1;
        p2++;
        while( p2 != end() ){
            ret -= y2zDyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= y2zDyDz( *p1, *(begin()) );
    }
    return ret;
}

QPolygonF Point2DModel::qPolygonF( double scale, bool closed ){
    QPolygonF ret;
    for( QPolygonF::iterator iter = begin(); iter != end(); ++iter ){
        ret.append( QPointF( iter->x() * scale, iter->y() * scale ) );
    }
    if( ret.size() > 0 ){
        if( closed ){
            ret.append( ret.first() );
        }
    }
    return ret;
}

void Point2DModel::cut( Point2DModel * sectPlus, Point2DModel * sectMinus, double ll, double my, double mz, double t){
    if( size() > 2 ) {
        QPolygonF::iterator iter = begin();
        double l = ll - t;
        double y = iter->x();
        double z = iter->y();
        double res = l + mz * y + my * z;
        if((res >= 0.0) && (sectPlus != NULL)){
            sectPlus->appendPoint( y, z);
        }
        if((res <= 0.0) && (sectMinus != NULL)){
            sectMinus->appendPoint( y, z);
        }
        double yPrev = y; double zPrev = z; double resPrev = res;
        while( iter != end() ){
            iter++;
            if( iter == end() ){
                y = first().x();
                z = first().y();
            } else {
                y = iter->x();
                z = iter->y();
            }
            res = l + mz * y + my * z;
            if( res * resPrev < 0.0 ){
                double yR  = 0.0, zR = 0.0;
                if( system2D( &yR, &zR, mz, my, -l, z-zPrev, -(y-yPrev), yPrev * (z-zPrev) - zPrev * (y-yPrev)  ) ){
                    if( sectPlus != NULL ){
                        sectPlus->appendPoint( yR, zR );
                    }
                    if( sectMinus != NULL ){
                        sectMinus->appendPoint( yR, zR );
                    }
                }
            }
            if( iter != end() ) {
                if((res >= 0.0) && (sectPlus != NULL)){
                    sectPlus->appendPoint( y, z);
                }
                if((res <= 0.0) && (sectMinus != NULL)){
                    sectMinus->appendPoint( y, z);
                }
            }
            yPrev = y; zPrev = z; resPrev = res;
        }
    }
}

void Point2DModel::calculateSects( const double &l, const double &my, const double &mz,
                                   const double &eMinus, const double &ePlus,
                                   Point2DModel *sectMinus, Point2DModel *sectEl, Point2DModel *sectPlus) {
    if( size() > 0 ){
        int d1 = where( yNormal(0), zNormal(0),
                        l, my, mz, eMinus, ePlus );
        int d2;
        if( d1 == 0 ){
            if( sectMinus ){
                sectMinus->appendPoint( yNormal(0), zNormal(0) );
            }
        } else if( d1 == 1 ){
            if( sectEl ) {
                sectEl->appendPoint( yNormal(0), zNormal(0) );
            }
        } else if( d1 == 2 ){
            if( sectPlus ) {
                sectPlus->appendPoint( yNormal(0), zNormal(0) );
            }
        }
        for( int j = 0; j < size(); j++){
            int i = j+1;
            if( i == size() ){
                i = 0;
            }
            double yi = yNormal(i);
            double zi = zNormal(i);
            double yj = yNormal(j);
            double zj = zNormal(j);
            d2 = where( yi, zi, l, my, mz, eMinus, ePlus );

            // Calcola intersezioni tra retta di passaggio tra deformazioni limite
            // mz * y + my *z + l = e[0,1,2]
            // e retta congiungente Pi e Pj
            // - (zj - zi) * y + (yj - yi) * z + (zj - zi) * yi - (yj - yi) * zi = 0
            double Dz = zj - zi;
            double Dy = yj - yi;
            double yMinus = 0.0, zMinus = 0.0;
            if( !system2D( &yMinus, &zMinus,
                           mz, my, -l+eMinus,
                           Dz, - Dy, Dz*yi - Dy*zi ) ) {
                return;
            }
            double yPlus = 0.0, zPlus = 0.0;
            if( !system2D( &yPlus, &zPlus,
                           mz, my, -l+ePlus,
                           Dz, - Dy, Dz*yi - Dy*zi ) ) {
                return;
            }

            if( d1 == 0 ){
                if( d2 == 0 ){
                    if( sectMinus ){
                        sectMinus->appendPoint( yi, zi );
                    }
                } else if( d2 == 1 ){
                    if( sectMinus ){
                        sectMinus->appendPoint( yMinus, zMinus );
                    }
                    if( sectEl ){
                        sectEl->appendPoint( yMinus, zMinus );
                        sectEl->appendPoint( yi, zi );
                    }
                } else if( d2 == 2 ){
                    if( sectMinus ){
                        sectMinus->appendPoint( yMinus, zMinus );
                    }
                    if( sectEl ){
                        sectEl->appendPoint( yMinus, zMinus );
                        sectEl->appendPoint( yPlus, zPlus );
                    }
                    if( sectPlus ){
                        sectPlus->appendPoint( yPlus, zPlus );
                        sectPlus->appendPoint( yi, zi );
                    }
                }
            } else if( d1 == 1 ){
                if( d2 == 0 ){
                    if( sectEl ){
                        sectEl->appendPoint( yMinus, zMinus );
                    }
                    if( sectMinus ){
                        sectMinus->appendPoint( yMinus, zMinus );
                        sectMinus->appendPoint( yi, zi );
                    }
                } else if( d2 == 1 ){
                    if( sectEl ){
                        sectEl->appendPoint( yi, zi );
                    }
                } else if( d2 == 2 ){
                    if( sectEl ){
                        sectEl->appendPoint( yPlus, zPlus );
                    }
                    if( sectPlus ){
                        sectPlus->appendPoint( yPlus, zPlus );
                        sectPlus->appendPoint( yi, zi );
                    }
                }
            } else if( d1 == 2 ){
                if( d2 == 0 ){
                    if( sectPlus ){
                        sectPlus->appendPoint( yPlus, zPlus );
                    }
                    if( sectEl ){
                        sectEl->appendPoint( yPlus, zPlus );
                        sectEl->appendPoint( yMinus, zMinus );
                    }
                    if( sectMinus ){
                        sectMinus->appendPoint( yMinus, zMinus );
                        sectMinus->appendPoint( yi, zi );
                    }
                } else if( d2 == 1 ){
                    if( sectPlus ){
                        sectPlus->appendPoint( yPlus, zPlus );
                    }
                    if( sectEl ){
                        sectEl->appendPoint( yPlus, zPlus );
                        sectEl->appendPoint( yMinus, zMinus );
                    }
                } else if( d2 == 2 ){
                    if( sectPlus ){
                        sectPlus->appendPoint( yi, zi );
                    }
                }
            }
            d1 = d2;
        }
    }
}

int Point2DModel::where( double y, double z, double l, double my, double mz, double e1, double e2){
    double e = l + my * z + mz * y;
    if( e1 < e2 ){
        if( e < e1 )
            return 0;
        else if( e <= e2 )
            return 1;
        else
            return 2;
    } else if( e1 > e2 ){
        if( e < e2 )
            return 2;
        else if( e <= e1 )
            return 1;
        else
            return 0;
    } else {
        return 0;
    }
}

void Point2DModel::calculateSects(const double &l, const double &my, const double &mz, QList<double> &eVal, QList<Point2DModel *> &sects) {
    if( (eVal.size()+1) == sects.size() ){
        if( size() > 0 ){
            int d1 = where( yNormal(0), zNormal(0),
                            l, my, mz, eVal );
            int d2;
            if( sects.at( d1 ) != NULL ){
                sects.at( d1 )->appendPoint( yNormal(0), zNormal(0) );
            }

            for( int i = 0; i < size(); i++){
                int j = i+1;
                if( j == size() ){
                    j = 0;
                }
                double yi = yNormal(i);
                double zi = zNormal(i);
                double yj = yNormal(j);
                double zj = zNormal(j);
                double Dy = yj - yi;
                double Dz = zj - zi;
                d2 = where( yj, zj, l, my, mz, eVal );

                int k = d1;
                while( k != d2 ){
                    double y0=0.0, z0=0.0, ei = 0.0;
                    if( k < d2 ){
                        ei = eVal.at(k);
                    } else {
                        ei = eVal.at(k-1);
                    }
                    if( system2D( &y0, &z0, mz, my, ei-l,
                                  Dz, - Dy, Dz*yi - Dy*zi ) ) {
                        if( sects.at(k) != NULL ){
                            sects.at(k)->appendPoint(y0, z0 );
                        }
                        if( k < d2 ){
                            if( sects.at(k+1) != NULL ){
                                sects.at(k+1)->appendPoint(y0, z0 );
                            }
                        } else {
                            if( sects.at(k-1) != NULL ){
                                sects.at(k-1)->appendPoint(y0, z0 );
                            }
                        }
                    }
                    if( k < d2 ){
                        k++;
                    } else {
                        k--;
                    }
                }
                if( sects.at(k) != NULL ){
                    sects.at(k)->appendPoint( yj, zj );
                }

                d1 = d2;
            }
        }
    }
}

int Point2DModel::where(double y, double z, double l, double my, double mz, QList<double> &eVal) {
    double e = l + my * z + mz * y;
    qSort( eVal );
    for( int i=0; i < eVal.size(); ++i ){
        if( e < eVal.at(i) ){
            return i;
        }
    }
    return eVal.size();
}
