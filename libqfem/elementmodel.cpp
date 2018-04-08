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
#include "elementmodel.h"

#include "beam.h"
#include "vertexmodel.h"
#include "sectionmodel.h"
#include "hypothesis.h"
#include "unitmeasure.h"
#include "glgroup.h"

#include <QGraphicsWidget>
#include <QGraphicsLineItem>
#include <QMap>
#include <QVariant>
#include <QString>
#include <QtGlobal>

class ElementModelPrivate {
public:
    ElementModelPrivate( VertexModel * vm, SectionModel * sm, Section * s, Hypothesis * hyp, UnitMeasure * ump ){
        hypothesis = hyp;
        unitMeasure = ump;
        vertexModel = vm;
        sectionModel = sm;
        section = s;
        graphicsItem = new QGraphicsWidget();
        glItem = NULL;
    };
    ~ElementModelPrivate(){
        if( glItem ){
            delete glItem;
        }
    }

    static unsigned int nextId;

    Hypothesis * hypothesis;
    UnitMeasure * unitMeasure;

    QList<Element *> elementContainer;
    QMap<unsigned int, int> elementMap;
    VertexModel * vertexModel;
    SectionModel * sectionModel;
    Section * section;
    QGraphicsWidget * graphicsItem;
    GLGroup * glItem;
};

unsigned int ElementModelPrivate::nextId = 1;

ElementModel::ElementModel( VertexModel * vm, SectionModel * sm, Hypothesis * hyp, UnitMeasure * ump, QObject *parent ):
    QAbstractTableModel( parent ),
    m_d( new ElementModelPrivate( vm, sm, NULL, hyp, ump) ){
}

ElementModel::ElementModel( VertexModel *vm, Section *s, Hypothesis *hyp, UnitMeasure * ump, QObject *parent) :
    QAbstractTableModel( parent ),
    m_d( new ElementModelPrivate( vm, NULL, s, hyp, ump ) ){
}

int ElementModel::rowCount( const QModelIndex & ) const {
    return m_d->elementContainer.size();
}

int ElementModel::columnCount( const QModelIndex & ) const {
    return 2;
}

QVariant ElementModel::data( const QModelIndex & index, int role ) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_d->elementContainer.size() || index.row() < 0)
        return QVariant();

    Element * elem = m_d->elementContainer.at( index.row() );
    if (role == Qt::DisplayRole) {
        switch( index.column() ){
        case 0:
            return QVariant( elem->id() );
        case 1:
            return QVariant( elem->typeElement() );
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QVariant ElementModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const{
    if ( role == Qt::DisplayRole){
        if( orientation == Qt::Horizontal){
            switch( section ){
            case 0:
                return QVariant( trUtf8("Id") );
            case 1:
                return QVariant( trUtf8("Tipo") );
            default:
                return QVariant();
            }
        } else if( orientation == Qt::Vertical){
            return QVariant(section+1);
        }
    }
    return QVariant();
}

Qt::ItemFlags ElementModel::flags(const QModelIndex &index) const{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ElementModel::insert( ElementModel::ElementType t, int position, int count ){
    if (position < 0 || position > m_d->elementContainer.size() )
        return false;
    for (int row = position; row < (position+count); ++row) {
        Element * addedElement = createElement( t );
        if( addedElement != NULL ){
            beginInsertRows( QModelIndex(), position, position );

            // Inserisce l'elemento inserito nel contenitore
            m_d->elementContainer.insert( position, addedElement );

            // Aggiorna mappa id <--> numero di ordine
            QMap<unsigned int, int>::iterator iter = m_d->elementMap.begin();
            while( iter != m_d->elementMap.end()){
                if( iter.value() >= position ){
                    iter.value()++;
                }
                iter++;
            }
            m_d->elementMap.insert( addedElement->id(), position );

            // Aggiorna grafica
            addedElement->qGraphicsItem()->setParentItem( m_d->graphicsItem );

            if( m_d->glItem ){
                addedElement->glItem()->setGroup( m_d->glItem );
            }

            endInsertRows();
        }
    }
    return true;
}

bool ElementModel::append(ElementModel::ElementType t ) {
    return insert( t, m_d->elementContainer.size() );
}

bool ElementModel::remove(int position, int count ){
    if (position < 0 || (position + count) > m_d->elementContainer.size())
        return false;

    for (int row = position; row < (position+count); row++){

        // ok, procediamo a cancellare il materiale
        beginRemoveRows( QModelIndex(), position, position);

        // Aggiorna mappa id <--> numero d'ordine
        // scala di 1 le corrispondenze tra gli id e gli indici
        QMap<unsigned int, int>::iterator iter = m_d->elementMap.begin();
        while( iter != m_d->elementMap.end()){
            if( iter.value() > position ){
                iter.value()--;
            }
            iter++;
        }
        m_d->elementMap.remove( m_d->elementContainer.at(position)->id());

        // Aggiorna il contenitore
        delete m_d->elementContainer.at(position);
        m_d->elementContainer.removeAt( position );

        endRemoveRows();
    }
    return true;
}

bool ElementModel::clear(){
    return remove(0, m_d->elementContainer.size() );
}

Hypothesis * ElementModel::hypothesis(){
    return m_d->hypothesis;
}

Element * ElementModel::element( int i ){
    if( i >= 0 && i < m_d->elementContainer.size() )
        return m_d->elementContainer.at( i );
    else
        return 0;
}

Element * ElementModel::element(const QModelIndex & index ){
    return element( index.row() );
}

Element *ElementModel::lastElement() {
    if( m_d->elementContainer.isEmpty() ){
        return NULL;
    } else {
        return m_d->elementContainer.last();
    }
}

Element * ElementModel::elementId( unsigned int i){
    if( m_d->elementMap.find(i) == m_d->elementMap.end() )
        return 0;
    else
        return element( m_d->elementMap.value( i ) );
}

int ElementModel::mapId( unsigned int i ){
    if( m_d->elementMap.find(i) == m_d->elementMap.end() )
        return -1;
    else
        return m_d->elementMap.value( i );
}

QModelIndex ElementModel::elementIndex( int row ){
    return createIndex(row, 2);
}

QModelIndex ElementModel::elementIndexId( unsigned int id ){
    return elementIndex( mapId(id) );
}

ElementModel::ElementIterator ElementModel::begin(){
    return m_d->elementContainer.begin();
}

ElementModel::ElementIterator ElementModel::end(){
    return m_d->elementContainer.end();
}

int ElementModel::nElements(){
    return m_d->elementContainer.size();
}

Element * ElementModel::createElement( ElementModel::ElementType tt ){
    while( mapId( m_d->nextId ) >= 0 ){
        m_d->nextId++;
    }
    switch( tt ){
    case ElementModel::elementBeam :{
        if( m_d->vertexModel->vertexCount() > 1 ){
            Section * sect = m_d->section;
            if( sect == NULL ){
                if( m_d->sectionModel != NULL ){
                    sect = m_d->sectionModel->firstSection();
                    if( sect != NULL ){
                        sect = m_d->sectionModel->firstSection();
                    }
                }
            }
            if( sect ){
                Element * elem = new Beam( m_d->nextId, sect, m_d->vertexModel->vertex(0), m_d->vertexModel->vertex(1), m_d->hypothesis, m_d->unitMeasure, this );
                m_d->nextId++;
                return elem;
            } else {
                qWarning(trUtf8("Sezione Corrente non definita").toStdString().c_str() );
            }
        } else {
            qWarning(trUtf8("Per creare un elemento BEAM devono essere definiti almeno due vertici").toStdString().c_str() );
        }
    }
    }
    return NULL;
}

QGraphicsItem * ElementModel::graphicsItem(){
    return m_d->graphicsItem;
}

GLGroup *ElementModel::glItem(){
    if( m_d->glItem == NULL ){
        m_d->glItem = new GLGroup();
        for( ElementIterator iter = m_d->elementContainer.begin(); iter != m_d->elementContainer.end(); ++iter ){
            (*iter)->glItem()->setGroup( m_d->glItem );
        }
    }
    return m_d->glItem;
}

SectionModel * ElementModel::sectionModel(){
    return m_d->sectionModel;
}
