#include "steelamodel.h"

#include "varpluscontainer.h"
#include "doubleplus.h"
#include "tablemodelplusprivate.h"

#include <cmath>

class SteelA: public VarPlusContainer {
public:
    SteelA( UnitMeasure * ump, double AVal=0.0, double dVal=0.0 ):
        VarPlusContainer( ump, "SteelA", ""){
        A = new DoublePlus( AVal, "A", m_unitMeasure, UnitMeasure::sectL2 );
        A->setRichName(QObject::trUtf8("A"));
        A->setToolTip( QObject::trUtf8("Area dell'armatura"));
        addVarToContainer(A);

        d = new DoublePlus( dVal, "d", m_unitMeasure, UnitMeasure::sectL );
        d->setRichName(QObject::trUtf8("d"));
        d->setToolTip( QObject::trUtf8("Altezza utile dell'armatura"));
        addVarToContainer(d);
    }
    DoublePlus * A;
    DoublePlus * d;
};

class SteelAModelPrivate{
public:
    SteelAModelPrivate(){
    }

    QList<SteelA *> AList;
};

void SteelAModel::updateHeaders(UnitMeasure::unitMeasure um ){
    if( um == UnitMeasure::sectL2 || um == UnitMeasure::sectL) {
        QList<QString> headers;
        headers << trUtf8("A [%1]").arg( m_d->unitMeasure->string( UnitMeasure::sectL2 ) );
        headers << trUtf8("d [%1]").arg( m_d->unitMeasure->string( UnitMeasure::sectL ) );
        setHeaders( headers );
    }
}

void SteelAModel::updateDMax(){
    double v = 0.0;
    QList<SteelA *>::iterator i = m_dd->AList.begin();
    if(i != m_dd->AList.end()){
        v = (*i)->d->valueNormal();
        ++i;
    }
    for( ; i != m_dd->AList.end(); ++i ){
        if( (*i)->d->valueNormal() > v ){
            v = (*i)->d->valueNormal();
        }
    }
    emit dMaxChanged(v);
}

void SteelAModel::updateDMin(){
    double v = 0.0;
    QList<SteelA *>::iterator i = m_dd->AList.begin();
    if(i != m_dd->AList.end()){
        v = (*i)->d->valueNormal();
        ++i;
    }
    for( ; i != m_dd->AList.end(); ++i ){
        if( (*i)->d->valueNormal() < v ){
            v = (*i)->d->valueNormal();
        }
    }
    emit dMinChanged(v);
}

void SteelAModel::insertSteelA( SteelA * addedA, int position ){
    if (position < 0 || position > m_dd->AList.size() )
        position = m_dd->AList.size();

    if( addedA ){
        connect( addedA->A, SIGNAL(valueChanged(QString)), this, SIGNAL(modelChanged()) );
        connect( addedA->d, SIGNAL(valueChanged(QString)), this, SIGNAL(modelChanged()) );
        m_dd->AList.insert( position, addedA );
        connect( addedA->d, SIGNAL(valueChanged(QString)), this, SLOT(updateDMax()) );
        connect( addedA->d, SIGNAL(valueChanged(QString)), this, SLOT(updateDMin()) );
        updateDMax();
        updateDMin();
        insertRowsPrivate( position );
        setVarValueRow( position, addedA->A, addedA->d );
    }
}

void SteelAModel::removeSteelA( int p ){
    delete m_dd->AList.takeAt( p );
    updateDMax();
    updateDMin();
}

void SteelAModel::writeXml(QXmlStreamWriter *writer) {
    Q_UNUSED( writer );
}

void SteelAModel::readXml(QXmlStreamReader *reader) {
    Q_UNUSED( reader );
}

SteelAModel::SteelAModel( UnitMeasure * ump, QObject *parent ):
    TableModelPlus( "SteelAModel", ump, parent ),
    m_dd( new SteelAModelPrivate() ){
    connect( m_d->unitMeasure, SIGNAL(stringsChanged(UnitMeasure::unitMeasure)), this, SLOT(updateHeaders(UnitMeasure::unitMeasure)) );
    QList<QString> headers;
    headers << trUtf8("A [%1]").arg( m_d->unitMeasure->string( UnitMeasure::sectL2 ) );
    headers << trUtf8("d [%1]").arg( m_d->unitMeasure->string( UnitMeasure::sectL ) );
    setHeaders( headers );
}

void SteelAModel::insertRows( int position, int count ){
    if (position < 0 || position > m_dd->AList.size() )
        return;

    for (int row = position; row < (position+count); ++row) {
        SteelA * addedA = new SteelA( m_d->unitMeasure );
        insertSteelA( addedA, position );
    }
    emit modelChanged();
}

void SteelAModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dd->AList.size())
        return;

    for (int row = position; row < (position+count); row++){
        // ok, procediamo a cancellare la riga
        removeRowsPrivate( position );
        // e la sezione
        removeSteelA( position );
    }
    emit modelChanged();
}

DoublePlus * SteelAModel::d( int i ){
    if( i >= 0 && i < m_dd->AList.size() )
        return m_dd->AList.at( i )->d;
    else
        return NULL;
}

DoublePlus * SteelAModel::A( int i ){
    if( i >= 0 && i < m_dd->AList.size() )
        return m_dd->AList.at( i )->A;
    else
        return NULL;
}

int SteelAModel::count(){
    return m_dd->AList.size();
}

double SteelAModel::SANormal(){
    double ret = 0.0;
    for( QList<SteelA *>::iterator i = m_dd->AList.begin(); i != m_dd->AList.end(); ++i ){
        ret += (*i)->A->valueNormal();
    }
    return ret;
}

double SteelAModel::SAdNormal(){
    double ret = 0.0;
    for( QList<SteelA *>::iterator i = m_dd->AList.begin(); i != m_dd->AList.end(); ++i ){
        ret += (*i)->A->valueNormal() * (*i)->d->valueNormal();
    }
    return ret;
}

double SteelAModel::SAd2Normal(){
    double ret = 0.0;
    for( QList<SteelA *>::iterator i = m_dd->AList.begin(); i != m_dd->AList.end(); ++i ){
        ret += (*i)->A->valueNormal() * pow((*i)->d->valueNormal(), 2.0);
    }
    return ret;
}

