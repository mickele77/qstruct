#include "slssteelsigmamodel.h"

#include "steelamodel.h"
#include "doubleplus.h"

#include "tablemodelplusprivate.h"

class SLSSteelSigmaModelPrivate{
public:
    SLSSteelSigmaModelPrivate(SteelAModel *model):
        steelModel(model){
    };

    SteelAModel * steelModel;
    QList<DoublePlus *> sigmaList;
};

SLSSteelSigmaModel::SLSSteelSigmaModel(SteelAModel *steelModel, UnitMeasure *ump, QObject *parent):
    TableModelPlus("SLSSteelSigmaModel", ump, parent),
    m_dd( new SLSSteelSigmaModelPrivate(steelModel)){
    connect( m_dd->steelModel, &SteelAModel::rowsInserted, this, &SLSSteelSigmaModel::insertRows );
    connect( m_dd->steelModel, &SteelAModel::rowsRemoved, this, static_cast<void (SLSSteelSigmaModel::*) (const QModelIndex &, int, int)> (&SLSSteelSigmaModel::removeRows) );

    connect( m_d->unitMeasure, &UnitMeasure::stringsChanged, this, &SLSSteelSigmaModel::updateHeaders );
    QList<QString> headers;
    headers << tr("d [%1]").arg( m_d->unitMeasure->string( UnitMeasure::sectL ) );
    headers << tr("σs [%1]").arg( m_d->unitMeasure->string( UnitMeasure::tension ) );
    setHeaders( headers );

    insertRows( QModelIndex(), 0, m_dd->steelModel->count()-1 );
}

void SLSSteelSigmaModel::insertRows( const QModelIndex & parent, int start, int end){
    Q_UNUSED(parent);
    insertRowsPrivate( start, end-start+1);
    for( int i=start; i <=end; ++i ){
        m_dd->sigmaList.insert( i, new DoublePlus( 0.0, "sigmaSteelSLS", m_d->unitMeasure, UnitMeasure::tension, true));
        setVarValue(i, 0, m_dd->steelModel->d(i) );
        setVarValue( i, 1, m_dd->sigmaList.at(i));
        setVarReadOnly( i, 1, true );
    }
}

void SLSSteelSigmaModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dd->sigmaList.size())
        return;

    for (int row = position; row < (position+count); row++){
        // ok, procediamo a cancellare la riga
        removeRowsPrivate( position );
        // e la sezione
        m_dd->sigmaList.removeAt( position );
    }
    emit modelChanged();
}

void SLSSteelSigmaModel::removeRows( const QModelIndex & parent, int start, int end){
    Q_UNUSED(parent);
    removeRows( start, end-start+1);
}

void SLSSteelSigmaModel::updateHeaders(UnitMeasure::unitMeasure um) {
    if( um == UnitMeasure::sectL || um == UnitMeasure::tension) {
        QList<QString> headers;
        headers << tr("d [%1]").arg( m_d->unitMeasure->string( UnitMeasure::sectL ) );
        headers << tr("σₛ [%1]").arg( m_d->unitMeasure->string( UnitMeasure::tension ) );
        setHeaders( headers );
    }
}

void SLSSteelSigmaModel::writeXml(QXmlStreamWriter *writer) {
    //TODO
}

void SLSSteelSigmaModel::readXml(QXmlStreamReader *reader) {
    //TODO
}

void SLSSteelSigmaModel::resetValues(){
    for( QList<DoublePlus *>::iterator i = m_dd->sigmaList.begin(); i != m_dd->sigmaList.end(); ++i ){
        (*i)->setValueNormal( 0.0 );
    }
}

void SLSSteelSigmaModel::setValueNormal( double v, int i ){
    if( i >= 0 && i < m_dd->sigmaList.size() ){
        m_dd->sigmaList.at(i)->setValueNormal( v );
    }
}
