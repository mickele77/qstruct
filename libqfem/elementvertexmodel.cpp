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
#include "elementvertexmodel.h"

#include "element.h"
#include "vertex.h"
#include "vertexmodel.h"
#include "hypothesis.h"
#include "uintplus.h"
#include "doubleplus.h"
#include "boolplus.h"

#include <QList>
#include <QVariant>

ElementVertexModel::ElementVertexModel(VertexModel * gvm,  Element * el):
    QAbstractTableModel( el ),
    m_element( el ),
    m_vertexModel( gvm ){
}

int ElementVertexModel::rowCount( const QModelIndex & ) const {
    return (1 + m_element->hypothesis()->spaceDim() + m_element->hypothesis()->nDOFVert());
}

int ElementVertexModel::columnCount( const QModelIndex & ) const {
    return m_element->nVertexes();
}

QVariant ElementVertexModel::data( const QModelIndex & index, int role ) const {
    if (!index.isValid())
        return QVariant();

    if (index.column() >= m_element->nVertexes() || index.column() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        Vertex * vert = m_element->vertex( index.column() );
        if( index.row() == 0 ){
            return QVariant( vert->id->value() );
        } else if( (index.row() > 0) && (index.row() < (m_element->hypothesis()->spaceDim()+1))) {
            return QVariant( vert->xGlobal( index.row()-1 )->value() );
        }
    }
    if( role == Qt::CheckStateRole ){
        if( (index.row() > m_element->hypothesis()->spaceDim()) && (index.row() < (m_element->hypothesis()->spaceDim() + m_element->hypothesis()->nDOFVert()+1)) ){
            BoolPlus * conn = m_element->vertexConnection(index.column(), index.row() - m_element->hypothesis()->spaceDim() - 1 );
            if(  conn ){
                if( conn->value() )
                    return Qt::Checked;
            }
            return Qt::Unchecked;
        }
    }

    return QVariant();
}

QVariant ElementVertexModel::headerData(int section, Qt::Orientation orientation,
                                        int role) const{
    if ( role == Qt::DisplayRole){
        if( orientation == Qt::Vertical ){
            if( section == 0 ){
                return QVariant( trUtf8("Id") );
            } else if( section < (m_element->hypothesis()->spaceDim()+1) ){
                return QVariant( m_element->hypothesis()->dimCaption(section-1) );
            } else if( section < (m_element->hypothesis()->spaceDim() + m_element->hypothesis()->nDOFVert() + 1) ){
                return QVariant( trUtf8("c") + QString::number( section - m_element->hypothesis()->spaceDim() ) );
            }
        } else if( orientation == Qt::Horizontal){
            return QVariant( trUtf8("P") + QString::number(section+1) );
        }
    }
    return QVariant();
}

Qt::ItemFlags ElementVertexModel::flags(const QModelIndex &index) const{
    if (!index.isValid())
        return 0;

    if( index.row() == 0)
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else if( index.row() > m_element->hypothesis()->spaceDim() && index.row() < (m_element->hypothesis()->spaceDim() + m_element->hypothesis()->nDOFVert()+1) )
        return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ElementVertexModel::setData(const QModelIndex &index, const QVariant &value,
                                 int role ){
    if ( role == Qt::EditRole ){
        if( index.row() == 0 ){
            m_element->setVertex( index.column(), m_vertexModel->vertexId( value.toUInt()) );
            QModelIndex indexLast = createIndex( rowCount() -1, index.column());
            emit dataChanged(index, indexLast);
            return true;
        }
    }
    if( role == Qt::CheckStateRole &&
            (index.row() > m_element->hypothesis()->spaceDim()) && (index.row() < (m_element->hypothesis()->spaceDim() + m_element->hypothesis()->nDOFVert()+1)) ){
        BoolPlus * conn = m_element->vertexConnection(index.column(), index.row() - m_element->hypothesis()->spaceDim() - 1 );
        conn->setValue( value.toInt() == Qt::Checked );
        emit dataChanged( index,index);
        return true;
    }
    return false;
}

