#include "loadresultant.h"
#include "tablemodelplusprivate.h"

#include "doubleplus.h"
#include "qstringplus.h"

#include <QFile>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QStringList>

LoadSimple::LoadSimple( UnitMeasure * ump,
                        double PxInp, double PyInp, double PzInp,
                        double FxInp, double FyInp, double FzInp,
                        double MxInp, double MyInp, double MzInp ):
    VarPlusContainer(ump, "LoadSimple", ""){

    xP = new DoublePlus( PxInp, "xP", ump, UnitMeasure::length, false );
    xP->setRichName( trUtf8("x<span style=\" vertical-align:sub;\">C</span>") );
    connect(xP, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()) );
    addVarToContainer( xP );

    yP = new DoublePlus( PyInp, "yP", ump, UnitMeasure::length, false );
    yP->setRichName( trUtf8("y<span style=\" vertical-align:sub;\">C</span>") );
    connect(yP, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()) );
    addVarToContainer( yP );

    zP = new DoublePlus( PzInp, "zP", ump, UnitMeasure::length, false );
    zP->setRichName( trUtf8("z<span style=\" vertical-align:sub;\">C</span>") );
    connect(zP, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()) );
    addVarToContainer( zP );

    Fx = new DoublePlus( FxInp, "Fx", ump, UnitMeasure::loadF, false );
    Fx->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">x</span>") );
    connect(Fx, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()) );
    addVarToContainer( Fx );

    Fy = new DoublePlus( FyInp, "Fy", ump, UnitMeasure::loadF, false );
    Fy->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">y</span>") );
    connect(Fy, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()) );
    addVarToContainer( Fy );

    Fz = new DoublePlus( FzInp, "Fz", ump, UnitMeasure::loadF, false );
    Fz->setRichName( trUtf8("F<span style=\" vertical-align:sub;\">z</span>") );
    connect(Fz, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()) );
    addVarToContainer( Fz );

    Mx = new DoublePlus( MxInp, "Mx", ump, UnitMeasure::loadM, false );
    Mx->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">x</span>") );
    connect(Mx, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()) );
    addVarToContainer( Mx );

    My = new DoublePlus( MyInp, "My", ump, UnitMeasure::loadM, false );
    My->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">y</span>") );
    connect(My, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()) );
    addVarToContainer( My );

    Mz = new DoublePlus( MzInp, "Mz", ump, UnitMeasure::loadM, false );
    Mz->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">z</span>") );
    connect(Mz, SIGNAL(valueChanged(QString)), this, SIGNAL(loadChanged()) );
    addVarToContainer( Mz );
}

#include <QList>

typedef QList<LoadSimple *>::iterator LoadIterator;

class LoadResultantPrivate{
public:
    LoadResultantPrivate(){
    };
    QList<LoadSimple *> loadContainer;
};

LoadResultant::LoadResultant( UnitMeasure * ump, QObject * parent ):
    TableModelPlus( "LoadResultant", ump, parent ),
    m_dd( new LoadResultantPrivate() ){

    connect( ump, SIGNAL(stringsChanged(UnitMeasure::unitMeasure)), this, SLOT(updateHeaders()) );
    updateHeaders();

    R = new LoadSimple( ump );
    connect( R->xP, SIGNAL(valueChanged(QString)), this, SIGNAL(modelChanged()) );
    connect( R->yP, SIGNAL(valueChanged(QString)), this, SIGNAL(modelChanged()) );
    connect( R->zP, SIGNAL(valueChanged(QString)), this, SIGNAL(modelChanged()) );
    R->Fx->setReadOnly( true );
    R->Fy->setReadOnly( true );
    R->Fz->setReadOnly( true );
    R->Mx->setReadOnly( true );
    R->My->setReadOnly( true );
    R->Mz->setReadOnly( true );

    connect( this, SIGNAL(modelChanged()), this, SLOT(updateR()));
}

void LoadResultant::updateHeaders(){
    QList<QString> headerList;
    headerList << trUtf8("Tag")
               << trUtf8("xC [%1]").arg( m_d->unitMeasure->string(UnitMeasure::length ) )
               << trUtf8("yC [%1]").arg( m_d->unitMeasure->string(UnitMeasure::length ) )
               << trUtf8("zC [%1]").arg( m_d->unitMeasure->string(UnitMeasure::length ) )
               << trUtf8("Fx [%1]").arg( m_d->unitMeasure->string(UnitMeasure::loadF ) )
               << trUtf8("Fy [%1]").arg( m_d->unitMeasure->string(UnitMeasure::loadF ) )
               << trUtf8("Fz [%1]").arg( m_d->unitMeasure->string(UnitMeasure::loadF ) )
               << trUtf8("Mx [%1]").arg( m_d->unitMeasure->string(UnitMeasure::loadM ) )
               << trUtf8("My [%1]").arg( m_d->unitMeasure->string(UnitMeasure::loadM ) )
               << trUtf8("Mz [%1]").arg( m_d->unitMeasure->string(UnitMeasure::loadM ) );
    setHeaders( headerList );
}

void LoadResultant::insertRows( int position, int count ){
    if (position < 0 || position > m_dd->loadContainer.size() || count <= 0 )
        return;
    for( int i=0; i < count; ++i){
        LoadSimple * addedLoad = new LoadSimple( m_d->unitMeasure );
        insertRowsPrivate( position );
        m_dd->loadContainer.insert( position, addedLoad );
        setVarValueRow( position, addedLoad->name,
                        addedLoad->xP, addedLoad->yP, addedLoad->zP,
                        addedLoad->Fx, addedLoad->Fy, addedLoad->Fz,
                        addedLoad->Mx, addedLoad->My, addedLoad->Mz);
        connect(addedLoad, SIGNAL(loadChanged()), this, SIGNAL(modelChanged()) );
    }
    emit modelChanged();
}

void LoadResultant::appendRows(int count) {
    insertRows( m_dd->loadContainer.size(), count );
}

void LoadResultant::removeRows(int position, int count) {
    if (position < 0 || (position + count) > m_dd->loadContainer.size())
        return;

    for (int row = position; row < (position+count); row++){
        // scolleghiamo la sezione
        disconnect(m_dd->loadContainer.at(position), SIGNAL(loadChanged()), this, SIGNAL(modelChanged()) );

        // procediamo a cancellare la riga
        removeRowsPrivate( position );
        delete m_dd->loadContainer.at(position);
        m_dd->loadContainer.removeAt(position);
    }
    emit modelChanged();
}

void LoadResultant::writeXml(QXmlStreamWriter *writer) {
    writer->setAutoFormatting(true);
    writer->setCodec("UTF-8");

    writer->writeStartDocument();
    writer->writeStartElement( m_d->modelName );
    for( LoadIterator i = m_dd->loadContainer.begin(); i!= m_dd->loadContainer.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
    writer->writeEndDocument();
}

void LoadResultant::readXml(QXmlStreamReader *reader) {
    QXmlStreamReader::TokenType t = reader->tokenType();
    QString n = reader->name().toString().toUpper();
    QString myName = m_d->modelName.toUpper();

    while( !((t == QXmlStreamReader::StartElement) && (n == myName))  &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }

    while( !((t == QXmlStreamReader::EndElement) && (n == myName)) &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        if( (t == QXmlStreamReader::StartElement) && (n == "LOADSIMPLE")){
            appendRows();
            m_dd->loadContainer.last()->loadFromXML( reader->attributes() );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void LoadResultant::updateR() {
    double Fx = 0.0, Fy = 0.0, Fz = 0.0;
    double Mx = 0.0, My = 0.0, Mz = 0.0;
    for( LoadIterator i = m_dd->loadContainer.begin(); i != m_dd->loadContainer.end(); ++i){
        Fx += (*i)->Fx->valueNormal();
        Fy += (*i)->Fy->valueNormal();
        Fz += (*i)->Fz->valueNormal();
        Mx += (*i)->Mx->valueNormal()
                - (*i)->Fy->valueNormal() * ((*i)->zP->valueNormal() - R->zP->valueNormal())
                + (*i)->Fz->valueNormal() * ((*i)->yP->valueNormal() - R->yP->valueNormal());
        My += (*i)->My->valueNormal()
                + (*i)->Fx->valueNormal() * ((*i)->zP->valueNormal() - R->zP->valueNormal())
                - (*i)->Fz->valueNormal() * ((*i)->xP->valueNormal() - R->xP->valueNormal());
        Mz += (*i)->Mz->valueNormal()
                - (*i)->Fx->valueNormal() * ((*i)->yP->valueNormal() - R->yP->valueNormal())
                + (*i)->Fy->valueNormal() * ((*i)->xP->valueNormal() - R->xP->valueNormal());
    }
    R->Fx->setValueNormal( Fx );
    R->Fy->setValueNormal( Fy );
    R->Fz->setValueNormal( Fz );
    R->Mx->setValueNormal( Mx );
    R->My->setValueNormal( My );
    R->Mz->setValueNormal( Mz );
}

void LoadResultant::loadFromFileTXT( QFile & file ){
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in( &file );

    // legge prima riga con intestazioni
    QStringList headersList;
    if( !in.atEnd()){
        QString line = in.readLine();
        QStringList list = line.split( '\t', QString::SkipEmptyParts );
        for( int i=0; i < list.size(); i++ ){
            headersList.append( list.at(i).toUpper() );
        }
    }

    // legge seconda riga con conversioni unità  di misura
    QList<double> UMList;
    if( !in.atEnd()){
        QString line = in.readLine();
        QStringList list = line.split( '\t', QString::SkipEmptyParts );
        for( int i=0; i < list.size(); i++ ){
            UMList.append( list.at(i).toDouble() );
        }
    }

    int nColHUM = qMin( UMList.size(), headersList.size() );
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList dataList = line.split( '\t', QString::SkipEmptyParts );
        if( dataList.size() >= 0 ){
            int nCol = qMin( dataList.size(), nColHUM );
            appendRows();
            LoadSimple * load = m_dd->loadContainer.last();
            for( int i=0; i < nCol; i++ ){
                if( headersList.at(i).toUpper() == "TAG" ){
                    load->tag->setValueNormal( dataList.at(i) );
                } else if( headersList.at(i).toUpper() == "XP" ){
                    load->xP->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "YP" ){
                    load->yP->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "ZP" ){
                    load->zP->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "FX" ){
                    load->Fx->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "FY" ){
                    load->Fy->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "FZ" ){
                    load->Fz->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MX" ){
                    load->Mx->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MY" ){
                    load->My->setValueNormal( dataList.at(i).toDouble() * UMList.at(i));
                } else if( headersList.at(i).toUpper() == "MZ" ){
                    load->Mz->setValueNormal(  dataList.at(i).toDouble() * UMList.at(i));
                }
            }
        }
    }
}
