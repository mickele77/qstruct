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
#include "vertexmodel.h"

#include "tablemodelplusprivate.h"

#include "hypothesis.h"

#include "qgraphicspointitem.h"
#include "glgroup.h"

#include "doubleplus.h"
#include "uintplus.h"
#include "boolplus.h"

#include <QXmlStreamWriter>
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QMap>
#include <QVariant>

#include <QDebug>

class VertexModelPrivate{
public:
    VertexModelPrivate( Hypothesis * hyp ){
        hypothesis = hyp;
        qGraphicsItem = new QGraphicsWidget();
        qGraphicsContainer = new QList<QGraphicsPointItem *>;
        vertexGraphicsRadius = 1.0;
        glGroup = NULL;
    };
    ~VertexModelPrivate(){
        if( glGroup ){
            delete glGroup;
        }
    }
    QPointF vertexToQPointF( Vertex * v ){
        if( hypothesis->spaceDim() == 1 ){
            return QPointF( v->xGlobal(0)->value(), 0.0 );
        }
        if( hypothesis->spaceDim() >= 2 ){
            return QPointF( v->xGlobal(0)->value(), v->xGlobal(1)->value() );
        }
        return QPointF();
    }
    Vertex * vertex( int p ){
        if( (p >= 0) && (p < vertexContainer.size()) ){
            return vertexContainer.at(p);
        } else {
            return NULL;
        }
    };
    Vertex * vertexId( unsigned int id ){
        if( vertexMap.contains( id ) ){
            return vertexMap.value( id, NULL );
        } else {
            return NULL;
        }
    };
    void insertVertex( Vertex * vert, int pos ){
        // l'id deve essere unico
        while( vertexMap.contains(vert->id->valueNormal()) ){
            vert->nextId();
        }
        // inserisce il vertice
        vertexContainer.insert( pos, vert );
        vertexMap.insert( vert->id->valueNormal(), vert );
        // Aggiorna grafica
        qGraphicsContainer->insert( pos, new QGraphicsPointItem( vertexToQPointF( vert ), vertexGraphicsRadius, qGraphicsItem ) );
        if( glGroup ){
            vert->glItem()->setGroup( glGroup );
        }
    };
    void removeVertex( int pos ){
        // Aggiorna grafica
        delete qGraphicsContainer->at(pos);
        qGraphicsContainer->removeAt(pos);
        // rimuove il vertice
        vertexMap.remove( vertexContainer.at(pos)->id->valueNormal() );
        delete vertexContainer.at(pos);
        vertexContainer.removeAt(pos);
    };
    void updateQGraphics( int pos ){
        qGraphicsContainer->at(pos)->setPoint( vertexToQPointF( vertexContainer.at( pos ) )  );
    };

    static unsigned int s_nextId;
    Hypothesis * hypothesis;
    QList<Vertex *> vertexContainer;
    QMap<unsigned int, Vertex *> vertexMap;

    QGraphicsItem * qGraphicsItem;
    QList<QGraphicsPointItem *> * qGraphicsContainer;
    double vertexGraphicsRadius;
    GLGroup * glGroup;
};

VertexModel::VertexModel( Hypothesis * hyp, UnitMeasure * ump, QObject *parent ):
    TableModelPlus( "VertexModel", ump, parent ),
    m_dd( new VertexModelPrivate( hyp ) ){
    setHeaders( headersList() );
}

QList<QString> VertexModel::headersList( ) {
    QList<QString> ret;

    ret << trUtf8("Id");
    for( int i=0;i < m_dd->hypothesis->spaceDim(); ++i ){
        ret << m_dd->hypothesis->dimCaption(i);
    }

    if( (m_dd->hypothesis->nDOFVert() == 3) && (m_dd->hypothesis->spaceDim() == 2) ){
        ret << trUtf8("ηx") << trUtf8("ηy") << trUtf8("θ");
        ret << trUtf8("Fx") << trUtf8("Fy") << trUtf8("M");
    } else {
        for( int i=0; i < m_dd->hypothesis->nDOFVert(); ++i ){
            ret << trUtf8("η%1").arg( i+1 );
        }
        for( int i=0; i < m_dd->hypothesis->nDOFVert(); ++i ){
            ret << trUtf8("F%1").arg( i+1 );
        }
    }
    return ret;
}

void VertexModel::insertRows( int position, int count ){
    for (int row = position; row < (position+count); ++row) {
        // Crea il vertice
        Vertex * addedVert = new Vertex( m_dd->hypothesis, m_d->unitMeasure );
        insertVertex( addedVert, position );
    }
}

void VertexModel::insertVertex( Vertex * addedVert, int position ){
    if (position < 0 || position > m_dd->vertexContainer.size() )
        position = m_dd->vertexContainer.size();
    if( addedVert != NULL ){
        m_dd->insertVertex( addedVert, position );
        connect( addedVert, SIGNAL(vertexChanged()), this, SIGNAL(modelChanged()) );
        insertRowsPrivate( position );
        setVar( TableModelPlus::DisplayValue, position, 0, addedVert->id );
        for( int i=0; i < m_dd->hypothesis->spaceDim(); ++i){
            setVar( TableModelPlus::DisplayValue, position, 1+i, addedVert->xGlobal(i) );
        }
        for( int i=0; i < m_dd->hypothesis->nDOFVert(); ++i){
            setVar( TableModelPlus::DisplayValueROInv, position, 1+i+m_dd->hypothesis->spaceDim(), addedVert->etaGlobal(i) );
        }
        for( int i=0; i < m_dd->hypothesis->nDOFVert(); ++i){
            setVar( TableModelPlus::DisplayValueROInv, position, 1+i+m_dd->hypothesis->spaceDim()+m_dd->hypothesis->nDOFVert(), addedVert->fGlobal(i) );
        }
    }
}

void VertexModel::appendRow(){
    insertRows( m_dd->vertexContainer.size(), 1 );
}

void VertexModel::removeRows(int position, int count ){
    if (position < 0 || (position + count) > m_dd->vertexContainer.size())
        return;

    for (int row = position; row < (position+count); row++){
        if( m_dd->vertex(position)->isUsed()){
            QString title = trUtf8("Materiale già in uso");
            QString message = trUtf8("Vertice usato da almeno unelemento.");
            qWarning() << title;
            qWarning() << message;
        } else {
            // scolleghiamo il materiale
            disconnect( m_dd->vertex(position), SIGNAL(vertexChanged()), this, SIGNAL(modelChanged()) );
            // rimuoviamo la riga
            removeRowsPrivate( position );
            // ... e il materiale
            m_dd->removeVertex( position );
        }
    }
}

QPointF VertexModel::vertexToQPointF( Vertex * v ){
    if( m_dd->hypothesis->spaceDim() == 1 ){
        return QPointF( v->xGlobal(0)->value(), 0.0 );
    }
    if( m_dd->hypothesis->spaceDim() >= 2 ){
        return QPointF( v->xGlobal(0)->value(), v->xGlobal(1)->value() );
    }
    return QPointF();
}

Hypothesis * VertexModel::hypothesis(){
    return m_dd->hypothesis;
}


Vertex * VertexModel::vertex( int i ){
    return m_dd->vertex( i );
}

Vertex * VertexModel::vertex(const QModelIndex & index ){
    return m_dd->vertex( index.row() );
}

Vertex *VertexModel::lastVertex() {
    if( m_dd->vertexContainer.isEmpty() ){
        return NULL;
    } else {
        return m_dd->vertexContainer.last();
    }
}

Vertex * VertexModel::vertexId( unsigned int i){
    return m_dd->vertexId(i);
}

VertexModel::VertexIterator VertexModel::begin(){
    return m_dd->vertexContainer.begin();
}

VertexModel::VertexIterator VertexModel::end(){
    return m_dd->vertexContainer.end();
}

int VertexModel::vertexCount(){
    return m_dd->vertexContainer.size();
}

QGraphicsItem * VertexModel::graphicsItem(){
    return m_dd->qGraphicsItem;
}

GLGroup *VertexModel::glItem(){
    if( m_dd->glGroup == NULL ){
        m_dd->glGroup = new GLGroup();
        for( VertexIterator iter = m_dd->vertexContainer.begin();  iter != m_dd->vertexContainer.end(); ++iter){
            (*iter)->glItem()->setGroup( m_dd->glGroup );
        }
    }
    return m_dd->glGroup;
}

void VertexModel::setGraphicsRadius( double val ){
    m_dd->vertexGraphicsRadius = val;

    QList<QGraphicsPointItem *>::iterator iter = m_dd->qGraphicsContainer->begin();
    while( iter != m_dd->qGraphicsContainer->end()){
        (*iter)->setRadius( m_dd->vertexGraphicsRadius );
        iter++;
    }
}

QRectF VertexModel::graphicsItemBoudingRect(){
    VertexIterator iterBegin = m_dd->vertexContainer.begin();
    if( iterBegin != m_dd->vertexContainer.end() ){
        VertexIterator iter = iterBegin;
        iter++;
        if( iter != m_dd->vertexContainer.end() ){
            QRectF ret = qRectFromVertexes( *iterBegin, *iter );
            iter++;
            while( iter != m_dd->vertexContainer.end() ){
                ret = ret.united( qRectFromVertexes( *iterBegin, *iter ) );
                iter++;
            }
            return ret;
        }
    }
    return m_dd->qGraphicsItem->boundingRect() ;
}

void VertexModel::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement(m_d->modelName);
    for( VertexIterator i = m_dd->vertexContainer.begin(); i != m_dd->vertexContainer.end(); ++i ){
        (*i)->writeXml( writer );
    }
    writer->writeEndElement();
}

void VertexModel::readXml(QXmlStreamReader *reader) {
    QXmlStreamReader::TokenType t = reader->tokenType();
    QString n = reader->name().toString().toUpper();
    QString myName = m_d->modelName.toUpper();

    // Ci mettiamo sul tag iniziale
    while( !(n == myName && t == QXmlStreamReader::StartElement) &&
           !(reader->hasError()) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }

    // Ci mettiamo sul tag finale. Nel frattempo leggiamo i vertici
    while( !(t == QXmlStreamReader::EndElement && n == myName) &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        if( t == QXmlStreamReader::StartElement && (n == "VERTEX") ){
            Vertex * addedVertex = new Vertex( m_dd->hypothesis, m_d->unitMeasure, reader->attributes() );
            insertVertex( addedVertex, m_dd->vertexContainer.size() );
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

QRectF VertexModel::qRectFromVertexes( Vertex * p1, Vertex * p2){
    if( p1->hypothesis()->spaceDim() > 1 && p2->hypothesis()->spaceDim() > 1 ){
        double left = p1->xGlobal(0)->value();
        double right = p2->xGlobal(0)->value();
        if( left > right ){
            left = p2->xGlobal(0)->value();
            right = p1->xGlobal(0)->value();
        }
        double bottom = p1->xGlobal(1)->value();
        double top =  p2->xGlobal(1)->value();
        if( bottom > top ){
            bottom = p2->xGlobal(1)->value();
            top = p1->xGlobal(1)->value();
        }
        return QRect( left, top, right-left, top - bottom);
    } else {
        return QRect();
    }
}
