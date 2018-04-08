#include "sectionloadphasemodel.h"

#include "sectionrcncr.h"
#include "sectionloadphase.h"
#include "tablemodelplusprivate.h"

#include "point2dplus.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "uintplus.h"

class SectionLoadPhaseModelPrivate {
public:
    SectionLoadPhaseModelPrivate() {

    }

    QList<SectionLoadPhase *> phases;
};

SectionLoadPhaseModel::SectionLoadPhaseModel( UnitMeasure * ump, QObject * parent ):
    TableModelPlus("SectionLoadPhaseModel", ump, parent ),
    m_dd(new SectionLoadPhaseModelPrivate()){
    QList<QString> headerList;
    headerList << trUtf8("Denominazione");
    headerList << trUtf8("N [%1]").arg( m_d->unitMeasure->string(UnitMeasure::loadF) );
    headerList << trUtf8("yN [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL) );
    headerList << trUtf8("zN [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL) );
    headerList << trUtf8("My [%1]").arg( m_d->unitMeasure->string(UnitMeasure::loadM) );
    headerList << trUtf8("Mz [%1]").arg( m_d->unitMeasure->string(UnitMeasure::loadM) );
    headerList << trUtf8("λ [%1]").arg( m_d->unitMeasure->string(UnitMeasure::deformation) );
    headerList << trUtf8("μy [%1]").arg( m_d->unitMeasure->string(UnitMeasure::curvature) );
    headerList << trUtf8("μz [%1]").arg( m_d->unitMeasure->string(UnitMeasure::curvature) );
    setHeaders( headerList );
}

SectionLoadPhaseModel::~SectionLoadPhaseModel() {
    delete m_dd;
}

void SectionLoadPhaseModel::writeXml(QXmlStreamWriter *writer) {
    Q_UNUSED(writer);
}

void SectionLoadPhaseModel::readXml(QXmlStreamReader *reader)
{
    Q_UNUSED(reader);
}

void SectionLoadPhaseModel::insertRows(int position, int count) {
    if (position < 0 || position > m_dd->phases.size() )
        position = m_dd->phases.size();

    for( int row =position; row < (position+count); ++row ){

        SectionLoadPhase * addedPhase = new SectionLoadPhase( m_d->unitMeasure, this, "SectionLoadPhase" );
        addedPhase->name->setValue( trUtf8("Fase %1").arg(row+1) );

        m_dd->phases.insert( row, addedPhase );
        insertRowsPrivate( position );

        setVar( DisplayValue, position, 0, addedPhase->name );
        setVar( DisplayValue, position, 1, addedPhase->NEd );
        setVar( DisplayValue, position, 2, addedPhase->NCen->y );
        setVar( DisplayValue, position, 3, addedPhase->NCen->z );
        setVar( DisplayValue, position, 4, addedPhase->MEdY );
        setVar( DisplayValue, position, 5, addedPhase->MEdZ );
        setVar( DisplayValue, position, 6, addedPhase->l );
        setVar( DisplayValue, position, 7, addedPhase->my );
        setVar( DisplayValue, position, 8, addedPhase->mz );

        connect( addedPhase, &SectionLoadPhase::containerChanged, this, &SectionLoadPhaseModel::modelChanged );

        emit modelChanged();
    }
}

void SectionLoadPhaseModel::removeRows(int position, int count) {
    if(position < 0 || (position + count) > m_dd->phases.size() || count == 0 )
        return;

    for (int row = position; row < (position+count); row++){
        SectionLoadPhase * phase = m_dd->phases.at(position);
        disconnect( phase, &SectionLoadPhase::containerChanged, this, &SectionLoadPhaseModel::modelChanged );
        removeRowsPrivate( position );

        delete m_dd->phases.takeAt( position );
    }
    emit modelChanged();
}

int SectionLoadPhaseModel::size() {
    return m_dd->phases.size();
}

void SectionLoadPhaseModel::calculate(SectionRCncr *sectRCncr) {
    if( sectRCncr != NULL ){
        QList<DoublePlus *> l, my, mz;
        for( int phase=0; phase < m_dd->phases.size(); ++phase ){
            l.append( m_dd->phases.at(phase)->l );
            my.append( m_dd->phases.at(phase)->my );
            mz.append( m_dd->phases.at(phase)->mz );
            sectRCncr->NMULS( &l, &my, &mz,
                              m_dd->phases.at(phase),
                              m_dd->phases.at(phase)->NEd, m_dd->phases.at(phase)->MEdY, m_dd->phases.at(phase)->MEdZ,
                              m_dd->phases.at(phase)->NCen );
        }
    }
}

QList<SectionLoadPhase *> SectionLoadPhaseModel::loadPhasesList() {
    return m_dd->phases;
}

int SectionLoadPhaseModel::loadPhaseNumber(SectionLoadPhase *lp) {
    return m_dd->phases.indexOf(lp);
}

SectionLoadPhase * SectionLoadPhaseModel::loadPhaseId(unsigned int idInp ) {
    for( QList<SectionLoadPhase *>::iterator iter = m_dd->phases.begin(); iter != m_dd->phases.end(); iter++ ) {
        if( (*iter)->id->valueNormal() == idInp ) {
            return (*iter);
        }
    }
    return NULL;
}
