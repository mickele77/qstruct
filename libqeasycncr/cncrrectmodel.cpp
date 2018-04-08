#include "cncrrectmodel.h"

#include "varpluscontainer.h"
#include "doubleplus.h"
#include "tablemodelplusprivate.h"

#include <cmath>

class CncrRect: public VarPlusContainer {
public:
    CncrRect( UnitMeasure * ump, double bVal=0.0, double tVal=0.0 ):
        VarPlusContainer( ump, "CncrRect", ""){
        b = new DoublePlus( bVal, "b", m_unitMeasure, UnitMeasure::sectL );
        b->setRichName(QObject::trUtf8("b"));
        b->setToolTip( QObject::trUtf8("Larghezza della sezione rettangolare in cls"));
        addVarToContainer(b);

        t = new DoublePlus( tVal, "t", m_unitMeasure, UnitMeasure::sectL );
        t->setRichName(QObject::trUtf8("d"));
        t->setToolTip( QObject::trUtf8("Altezza sezione rettangolare in cls"));
        addVarToContainer(t);
    }
    DoublePlus * b;
    DoublePlus * t;
};

class CncrRectModelPrivate{
public:
    CncrRectModelPrivate():
    hG(0.0){
    }
    QList<CncrRect *> sectList;
    double hG;
};

void CncrRectModel::updateHeaders(UnitMeasure::unitMeasure um ){
    if( um == UnitMeasure::sectL) {
        QList<QString> headers;
        headers << trUtf8("b [%1]").arg( m_d->unitMeasure->string( UnitMeasure::sectL ) );
        headers << trUtf8("t [%1]").arg( m_d->unitMeasure->string( UnitMeasure::sectL ) );
        setHeaders( headers );
    }
}

void CncrRectModel::updateHTot(){
    double v = 0.0;
    for( QList<CncrRect *>::iterator i = m_dd->sectList.begin(); i != m_dd->sectList.end(); ++i ){
        v += (*i)->t->valueNormal();
    }
    emit hTotChanged(v);
}

void CncrRectModel::insertCncrRect( CncrRect * addedSect, int position ){
    if (position < 0 || position > m_dd->sectList.size() )
        position = m_dd->sectList.size();

    if( addedSect ){
        connect( addedSect->b, SIGNAL(valueChanged(QString)), this, SIGNAL(modelChanged()) );
        connect( addedSect->t, SIGNAL(valueChanged(QString)), this, SIGNAL(modelChanged()) );
        m_dd->sectList.insert( position, addedSect );
        updateHTot();
        insertRowsPrivate( position );
        setVarValueRow( position, addedSect->b, addedSect->t );
    }
}

void CncrRectModel::removeCncrRect( int p ){
    delete m_dd->sectList.takeAt( p );
    updateHTot();
}

void CncrRectModel::writeXml(QXmlStreamWriter *writer) {
    Q_UNUSED( writer );
}

void CncrRectModel::readXml(QXmlStreamReader *reader) {
    Q_UNUSED( reader );
}

CncrRectModel::CncrRectModel( UnitMeasure * ump, QObject *parent ):
    TableModelPlus( "CncrRectModel", ump, parent ),
    m_dd( new CncrRectModelPrivate() ){
    connect( m_d->unitMeasure, SIGNAL(stringsChanged(UnitMeasure::unitMeasure)), this, SLOT(updateHeaders(UnitMeasure::unitMeasure)) );
    QList<QString> headers;
    headers << trUtf8("b [%1]").arg( m_d->unitMeasure->string( UnitMeasure::sectL ) );
    headers << trUtf8("t [%1]").arg( m_d->unitMeasure->string( UnitMeasure::sectL ) );
    setHeaders( headers );

    connect( this, SIGNAL(modelChanged()), this, SLOT(updateHG()) );
    connect( this, SIGNAL(modelChanged()), this, SLOT(updateHTot()) );
}

void CncrRectModel::insertRows( int position, int count ){
    if (position < 0 || position > m_dd->sectList.size() )
        return;

    for (int row = position; row < (position+count); ++row) {
        CncrRect * addedA = new CncrRect( m_d->unitMeasure );
        insertCncrRect( addedA, position );
    }
    emit modelChanged();
}

void CncrRectModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dd->sectList.size())
        return;

    for (int row = position; row < (position+count); row++){
        // ok, procediamo a cancellare la riga
        removeRowsPrivate( position );
        // e la sezione
        removeCncrRect( position );
    }
    emit modelChanged();
}

DoublePlus * CncrRectModel::b( int i ){
    if( i >= 0 && i < m_dd->sectList.size() )
        return m_dd->sectList.at( i )->b;
    else
        return NULL;
}

DoublePlus * CncrRectModel::t( int i ){
    if( i >= 0 && i < m_dd->sectList.size() )
        return m_dd->sectList.at( i )->t;
    else
        return NULL;
}

int CncrRectModel::count(){
    return m_dd->sectList.size();
}

double CncrRectModel::ANormal() {
    double ret = 0.0;
    for( QList<CncrRect *>::iterator i=m_dd->sectList.begin(); i != m_dd->sectList.end(); ++i ){
        ret += (*i)->b->valueNormal() * (*i)->t->valueNormal();
    }
    return ret;
}

double CncrRectModel::SxNormal() {
    double ret = 0.0;
    double h = 0.0;
    for( QList<CncrRect *>::iterator i=m_dd->sectList.begin(); i != m_dd->sectList.end(); ++i ){
        double t = (*i)->t->valueNormal();
        ret += (*i)->b->valueNormal() * t * (h + t / 2.0);
        h += t;
    }
    return ret;
}

double CncrRectModel::hGNormal() {
    return m_dd->hG;
}

void CncrRectModel::updateHG() {
    double v = 0.0;
    double a =  ANormal();
    if( a != 0.0 ){
        v = SxNormal() / a;
    }
    if( v != m_dd->hG ){
        m_dd->hG = v;
        emit hGChanged( m_dd->hG );
    }
}
