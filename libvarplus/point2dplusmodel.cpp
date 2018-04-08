#include "point2dplusmodel.h"

#include "tablemodelplusprivate.h"

#include "varplusmath.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "point2dmodel.h"
#include "structmath.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QList>
#include <QVector>
#include <QPolygonF>
#include <QPointF>
#include <cmath>

typedef QList<Point2DPlus *>::iterator PointIterator;

class Point2DPlusModelPrivate{
public:
    Point2DPlusModelPrivate(UnitMeasure::unitMeasure um, const QString & yH, const QString &zH):
        unitMeasureValue(um),
        yHeader( yH),
        zHeader( zH ){
    }
    ~Point2DPlusModelPrivate(){
        for( PointIterator iter = P.begin(); iter != P.end(); ++iter ){
            VarPlus::deleteVarPlus( *iter );
        }
        P.clear();
    }
    void insertP( int pos, Point2DPlus * p ){
        P.insert( pos, p );
    }
    void removeP( int pos ){
        P.removeAt(pos);
    }
    UnitMeasure::unitMeasure unitMeasureValue;
    QString yHeader;
    QString zHeader;
    QList<Point2DPlus *> P;
};

Point2DPlusModel::Point2DPlusModel(UnitMeasure *ump, UnitMeasure::unitMeasure um, const QString & yH, const QString &zH, QObject *parent):
    TableModelPlus( "Point2DPlusModel", ump, parent),
    m_dd( new Point2DPlusModelPrivate( um, yH, zH) ){
    connect( m_d->unitMeasure, SIGNAL(stringsChanged(UnitMeasure::unitMeasure)), this, SLOT(updateHeadersUM( UnitMeasure::unitMeasure )) );
    updateHeaders();
}

Point2DPlusModel::Point2DPlusModel( Point2DPlusModel & val ):
    TableModelPlus( val.m_d->modelName, val.m_d->unitMeasure, ((QObject &)val).parent() ),
    m_dd( new Point2DPlusModelPrivate( val.m_dd->unitMeasureValue, val.m_dd->yHeader, val.m_dd->zHeader) ){
    for( PointIterator i = val.m_dd->P.begin(); i != val.m_dd->P.end(); ++i ){
        m_dd->P.append( new Point2DPlus( **i ));
    }
}

void Point2DPlusModel::updateHeaders(){
    QList<QString> headerList;
    headerList << m_dd->yHeader + " [" + m_d->unitMeasure->string( m_dd->unitMeasureValue ) + "]"
               << m_dd->zHeader + " [" + m_d->unitMeasure->string( m_dd->unitMeasureValue ) + "]";
    setHeaders( headerList );
}

void Point2DPlusModel::updateHeadersUM( UnitMeasure::unitMeasure u ){
    if( u == m_dd->unitMeasureValue ){
        QList<QString> headerList;
        headerList << m_dd->yHeader + " [" + m_d->unitMeasure->string( m_dd->unitMeasureValue ) + "]"
                   << m_dd->zHeader + " [" + m_d->unitMeasure->string( m_dd->unitMeasureValue ) + "]";
        setHeaders( headerList );
    }
}


Point2DPlusModel::~Point2DPlusModel() {
    delete m_dd;
}

Point2DPlus *Point2DPlusModel::createPoint( double yVal, double zVal ) {
    return new Point2DPlus( "P", "y", yVal, "z", zVal, m_d->unitMeasure, m_dd->unitMeasureValue, false );
}

void Point2DPlusModel::writeXml( QXmlStreamWriter * writer ){
    writer->writeStartElement( m_d->modelName );
    for( PointIterator i = m_dd->P.begin(); i != m_dd->P.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void Point2DPlusModel::readXml(QXmlStreamReader *reader) {
    QXmlStreamReader::TokenType t = reader->tokenType();
    QString n = reader->name().toString().toUpper();
    QString myName = m_d->modelName.toUpper();

    while( !(n == myName && t == QXmlStreamReader::StartElement) &&
           !(reader->hasError()) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }

    while( !(n == myName && t == QXmlStreamReader::EndElement)  &&
           !(reader->hasError()) &&
           (t != QXmlStreamReader::EndDocument) ){
        if( n == "POINT2D" && t == QXmlStreamReader::StartElement ) {
            insertRows( reader->attributes() );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void Point2DPlusModel::deleteModel( Point2DPlusModel* var ){
    delete var;
}

Point2DPlusModel & Point2DPlusModel::operator=( Point2DPlusModel & val){
    if( this != &val ){
        removeRows( 0, m_dd->P.size() );
        insertRows( 0, val.m_dd->P.size() );

        PointIterator iter = m_dd->P.begin() ;
        PointIterator iterVal = val.m_dd->P.begin() ;
        while( (iter != m_dd->P.end()) && (iterVal != val.m_dd->P.end())){
            **iter = **iterVal;
            ++iter;
            ++iterVal;
        }
    }
    return *this;
}


int Point2DPlusModel::pCount(){
    return m_dd->P.size();
}

void Point2DPlusModel::translate( Point2DPlus * trans){
    PointIterator iter;
    iter = m_dd->P.begin();
    while( iter != m_dd->P.end() ){
        (*iter)->translate( trans );
        iter ++;
    }
}

void Point2DPlusModel::translateNormal( double yTrans, double zTrans ){
    PointIterator iter;
    iter = m_dd->P.begin();
    while( iter != m_dd->P.end() ){
        (*iter)->translateNormal( yTrans, zTrans );
        iter ++;
    }
}

void Point2DPlusModel::rotate( DoublePlus * rot){
    rotateNormal( rot->valueNormal());
}

void Point2DPlusModel::rotateNormal( double rot){
    PointIterator iter = m_dd->P.begin();
    while( iter != m_dd->P.end() ){
        (*iter)->rotateNormal( rot );
        iter++;
    }
}

Point2DPlus * Point2DPlusModel::P( const int & i){
    if( i < m_dd->P.count() )
        return m_dd->P.at(i);
    else
        return 0;
}


void Point2DPlusModel::insertRows(int position, int count) {
    insertRows( position, count, 0.0, 0.0);
}

void Point2DPlusModel::insertRows( int position, int count, double yVal, double zVal ){
    int row = position;
    while( row < (position+count) ) {
        insertRows( createPoint( yVal, zVal ), row );
        ++row;
    }
}

void Point2DPlusModel::insertRows( const QXmlStreamAttributes &attrs, int position , int count){
    if( position < 0 || position > m_dd->P.size() ){
        position = m_dd->P.size();
    }
    for( int i=0; i < count; ++i){
        Point2DPlus * addedP = createPoint();
        addedP->loadFromXML( attrs );
        insertRows( addedP, position );
    }
}

void Point2DPlusModel::appendRows( int count, double xVal, double yVal ){
    insertRows( m_dd->P.count(), count, xVal, yVal );
}

void Point2DPlusModel::appendPoint( double xVal, double yVal ){
    insertRows( m_dd->P.count(), 1, xVal, yVal );
}

void Point2DPlusModel::insertRows( Point2DPlus * addedP, int position ){
    if (position < 0 || position > m_dd->P.size() )
        position = m_dd->P.size();
    if( addedP ){
        m_dd->insertP( position, addedP );
        insertRowsPrivate( position );
        setVarValueRow( position, addedP->y, addedP->z );
        connect( addedP, &Point2DPlus::valueChanged, this, &Point2DPlusModel::modelChanged );
        emit modelChanged();
    }
}

void Point2DPlusModel::removeRows( int position, int count ){
    if (position < 0 || (position + count) > m_dd->P.size())
        return;

    for (int row = position; row < (position+count); row++){
        // scolleghiamo il punto
        disconnect( m_dd->P.at(position), &Point2DPlus::valueChanged, this, &Point2DPlusModel::modelChanged );
        // rimuoviamo la riga
        removeRowsPrivate( position );
        // ... e il punto
        m_dd->removeP( position );
    }
    emit modelChanged();
}

double Point2DPlusModel::yMaxNormal(){
    double ret = 0.0;
    PointIterator iter = m_dd->P.begin();
    if( iter != m_dd->P.end() ){
        ret = (*iter)->y->valueNormal();
        iter++;
    }
    while( iter != m_dd->P.end() ){
        if( (*iter)->y->valueNormal() > ret ){
            ret = (*iter)->y->valueNormal();;
        }
        iter++;
    }
    return ret;
}

double Point2DPlusModel::yMinNormal(){
    double ret = 0.0;
    PointIterator iter = m_dd->P.begin();
    if( iter != m_dd->P.end() ){
        ret = (*iter)->y->valueNormal();
        iter++;
    }
    while( iter != m_dd->P.end() ){
        if( (*iter)->y->valueNormal() < ret ){
            ret = (*iter)->y->valueNormal();;
        }
        iter++;
    }
    return ret;
}

double Point2DPlusModel::zMaxNormal(){
    double ret = 0.0;
    PointIterator iter = m_dd->P.begin();
    if( iter != m_dd->P.end() ){
        ret = (*iter)->z->valueNormal();
        iter++;
    }
    while( iter != m_dd->P.end() ){
        if( (*iter)->z->valueNormal() > ret ){
            ret = (*iter)->z->valueNormal();;
        }
        iter++;
    }
    return ret;
}

double Point2DPlusModel::zMinNormal(){
    double ret = 0.0;
    PointIterator iter = m_dd->P.begin();
    if( iter != m_dd->P.end() ){
        ret = (*iter)->z->valueNormal();
        iter++;
    }
    while( iter != m_dd->P.end() ){
        if( (*iter)->z->valueNormal() < ret ){
            ret = (*iter)->z->valueNormal();
        }
        iter++;
    }
    return ret;
}

bool Point2DPlusModel::sMinNormal( double * ret, double my, double mz ){
    bool wasRetSet = false;
    *ret = 0.0;
    PointIterator iter = m_dd->P.begin();
    if( iter != m_dd->P.end() ){
        *ret = my * (*iter)->y->valueNormal() + mz * (*iter)->z->valueNormal();
        wasRetSet = true;
        iter++;
    }
    while( iter != m_dd->P.end() ){
        double retCmp = my * (*iter)->y->valueNormal() + mz * (*iter)->z->valueNormal();
        if( retCmp < *ret ){
            *ret = retCmp;
        }
        iter++;
    }
    return wasRetSet;
}

bool Point2DPlusModel::sMaxNormal( double *ret, double my, double mz ){
    bool wasRetSet = false;
    *ret = 0.0;
    PointIterator iter = m_dd->P.begin();
    if( iter != m_dd->P.end() ){
        *ret = my * (*iter)->y->valueNormal() + mz * (*iter)->z->valueNormal();
        wasRetSet = true;
        iter++;
    }
    while( iter != m_dd->P.end() ){
        double retCmp = my * (*iter)->y->valueNormal() + mz * (*iter)->z->valueNormal();
        if( retCmp > *ret ){
            *ret = retCmp;
        }
        iter++;
    }
    return wasRetSet;
}

double Point2DPlusModel::ANormal(){
    double ret = 0.0;
    if( m_dd->P.count() > 2 ){
        PointIterator p1 = m_dd->P.begin();
        PointIterator p2 = p1;
        p2++;
        while( p2 != m_dd->P.end() ){
            ret -= DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= DyDz( *p1, *(m_dd->P.begin()) );
    }
    return ret;
}

double Point2DPlusModel::SyNormal(){
    double ret = 0.0;
    if( m_dd->P.count() > 2 ){
        PointIterator p1 = m_dd->P.begin();
        PointIterator p2 = p1;
        p2++;
        while( p2 != m_dd->P.end() ){
            ret -= zDyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= zDyDz( *p1, *(m_dd->P.begin()) );
    }
    return ret;
}

double Point2DPlusModel::SzNormal(){
    double ret = 0.0;
    if( m_dd->P.count() > 2 ){
        PointIterator p1 = m_dd->P.begin();
        PointIterator p2 = p1;
        p2++;
        while( p2 != m_dd->P.end() ){
            ret -= yDyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= yDyDz( *p1, *(m_dd->P.begin()) );
    }
    return ret;
}

double Point2DPlusModel::IyyNormal(){
    double ret = 0.0;
    if( m_dd->P.count() > 2 ){
        PointIterator p1 = m_dd->P.begin();
        PointIterator p2 = p1;
        p2++;
        while( p2 != m_dd->P.end() ){
            ret -= z2DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= z2DyDz( *p1, *(m_dd->P.begin()) );
    }
    return ret;
}

double Point2DPlusModel::IzzNormal(){
    double ret = 0.0;
    if( m_dd->P.count() > 2 ){
        PointIterator p1 = m_dd->P.begin();
        PointIterator p2 = p1;
        p2++;
        while( p2 != m_dd->P.end() ){
            ret -= y2DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= y2DyDz( *p1, *(m_dd->P.begin()) );
    }
    return ret;
}

double Point2DPlusModel::IyzNormal(){
    double ret = 0.0;
    if( m_dd->P.count() > 2 ){
        PointIterator p1 = m_dd->P.begin();
        PointIterator p2 = p1;
        p2++;
        while( p2 != m_dd->P.end() ){
            ret -= yzDyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= yzDyDz( *p1, *(m_dd->P.begin()) );
    }
    return ret;
}

double Point2DPlusModel::Sz3DyDzNormal(){
    double ret = 0.0;
    if( m_dd->P.count() > 2 ){
        PointIterator p1 = m_dd->P.begin();
        PointIterator p2 = p1;
        p2++;
        while( p2 != m_dd->P.end() ){
            ret -= z3DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= z3DyDz( *p1, *(m_dd->P.begin()) );
    }
    return ret;
}

double Point2DPlusModel::Sy3DyDzNormal(){
    double ret = 0.0;
    if( m_dd->P.count() > 2 ){
        PointIterator p1 = m_dd->P.begin();
        PointIterator p2 = p1;
        p2++;
        while( p2 != m_dd->P.end() ){
            ret -= y3DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= y3DyDz( *p1, *(m_dd->P.begin()) );
    }
    return ret;
}

double Point2DPlusModel::Syz2DyDzNormal(){
    double ret = 0.0;
    if( m_dd->P.count() > 2 ){
        PointIterator p1 = m_dd->P.begin();
        PointIterator p2 = p1;
        p2++;
        while( p2 != m_dd->P.end() ){
            ret -= yz2DyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= yz2DyDz( *p1, *(m_dd->P.begin()) );
    }
    return ret;
}

double Point2DPlusModel::Sy2zDyDzNormal(){
    double ret = 0.0;
    if( m_dd->P.count() > 2 ){
        PointIterator p1 = m_dd->P.begin();
        PointIterator p2 = p1;
        p2++;
        while( p2 != m_dd->P.end() ){
            ret -= y2zDyDz( *p1, *p2 );
            p1 = p2;
            p2++;
        }
        ret -= y2zDyDz( *p1, *(m_dd->P.begin()) );
    }
    return ret;
}

QPolygonF Point2DPlusModel::qPolygonF( bool closed ){
    QPolygonF ret;
    PointIterator iter;
    for( iter = m_dd->P.begin(); iter != m_dd->P.end(); ++iter ){
        ret << QPointF((*iter)->y->value(), (*iter)->z->value());
    }
    iter = m_dd->P.begin();
    if( closed && iter != m_dd->P.end() ){
        ret << QPointF((*iter)->y->value(), (*iter)->z->value());
    }
    return ret;
}

void Point2DPlusModel::cut( Point2DPlusModel * sectPlus, Point2DPlusModel * sectMinus, double ll, double my, double mz, double t){
    if( m_dd->P.size() > 2 ) {
        double l = ll - t;
        double y = P(0)->y->valueNormal();
        double z = P(0)->z->valueNormal();
        double res = l + mz * y + my * z;
        if((res >= 0.0) && (sectPlus != NULL)){
            sectPlus->appendPoint( y, z);
        }
        if((res <= 0.0) && (sectMinus != NULL)){
            sectMinus->appendPoint( y, z);
        }
        double yPrev = y; double zPrev = z; double resPrev = res;
        for( int i=1; i <= m_dd->P.size(); i++){
            if( i == m_dd->P.size() ){
                y = P(0)->y->valueNormal();
                z = P(0)->z->valueNormal();
            } else {
                y = P(i)->y->valueNormal();
                z = P(i)->z->valueNormal();
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
            if( i < m_dd->P.size()) {
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

void Point2DPlusModel::suspendSignals(bool susp) {
    for( QList<Point2DPlus *>::iterator point = m_dd->P.begin(); point != m_dd->P.end(); ++point ){
        (*point)->suspendSignals( susp );
    }
}

void Point2DPlusModel::calculateSects(const double &l, const double &my, const double &mz,
                                      QList<double> &eVal, QList<Point2DModel *> sects) {
    if( (eVal.size()+1) == sects.size() ){
        if( m_dd->P.size() > 0 ){
            double y0 = m_dd->P.at(0)->y->valueNormal();
            double z0 = m_dd->P.at(0)->z->valueNormal();
            int d1 = where( y0, z0, l, my, mz, eVal );
            int d2;
            if( sects.at( d1 ) != NULL ){
                sects.at( d1 )->appendPoint( y0, z0 );
            }

            for( int i = 0; i < m_dd->P.size(); i++){
                int j = i+1;
                if( j == m_dd->P.size() ){
                    j = 0;
                }
                double yi = m_dd->P.at(i)->y->valueNormal();
                double zi = m_dd->P.at(i)->z->valueNormal();
                double yj = m_dd->P.at(j)->y->valueNormal();
                double zj = m_dd->P.at(j)->z->valueNormal();
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

int Point2DPlusModel::where(double y, double z, double l, double my, double mz, QList<double> &eVal) {
    double e = l + my * z + mz * y;
    qSort( eVal );
    for( int i=0; i < eVal.size(); ++i ){
        if( e < eVal.at(i) ){
            return i;
        }
    }
    return eVal.size();
}
