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
#include "beamfmodel.h"

#include "hypothesis.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "uintplus.h"
#include "unitmeasure.h"

class BeamFModelPrivate{
public:
    BeamFModelPrivate( Hypothesis * hyp, UnitMeasure * ump ){
        unitMeasure = ump;
        hypothesis = hyp;
    };

    QList<BeamF *> container;
    UnitMeasure * unitMeasure;
    Hypothesis * hypothesis;
};

BeamFModel::BeamFModel(Hypothesis *hyp, UnitMeasure *ump, QObject *parent) :
    QAbstractTableModel(parent),
    m_d( new BeamFModelPrivate( hyp, ump ) ) {
}

UnitMeasure * BeamFModel::unitMeasure(){
    return m_d->unitMeasure;
}

Hypothesis * BeamFModel::hypothesis(){
    return m_d->hypothesis;
}

int BeamFModel::rowCount( const QModelIndex & ) const {
    return m_d->container.size();
}

int BeamFModel::columnCount( const QModelIndex & ) const {
    return (2 + BeamF::nVert() * m_d->hypothesis->nDOFVert());
}

QVariant BeamFModel::data( const QModelIndex & index, int role ) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_d->container.size() || index.row() < 0)
        return QVariant();

    if( index.row() < m_d->container.size() && role == Qt::DisplayRole ){
        BeamF * q = m_d->container.at( index.row() );
        switch( index.column() ){
        case 0:{
            return QVariant( q->name->valueStr() );
            break; }
        case 1:{
            return QVariant( q->sysRef()->valueNormalStr() );
            break;}
        default:{
            if( (index.column()-2) < (BeamF::nVert() * m_d->hypothesis->nDOFVert()) ){
                return QVariant( q->f(index.column() - 2 )->valueStr() );
            }
            break; }
        }
    }

    return QVariant();
}

QVariant BeamFModel::headerData(int section, Qt::Orientation orientation,
                                int role) const{
    if ( role == Qt::DisplayRole){
        if( orientation == Qt::Horizontal){
            switch( section ){
            case 0:{
                return trUtf8("Nome"); }
            case 1:{
                return QVariant( trUtf8("Rif-1") );}
            default:{
                if( (section - 2) < (BeamF::nVert() * m_d->hypothesis->nDOFVert()) ){
                    QString tit = "f" + QString::number( section - 1 );
                    return QVariant( tit );
                } else {
                    return QVariant();
                }
            }
            }
        } else if( orientation == Qt::Vertical){
            return QVariant(section+1);
        }
    }
    return QVariant();
}

Qt::ItemFlags BeamFModel::flags(const QModelIndex &index) const{
    if (!index.isValid())
        return 0;

    if( (index.column() >= 0) && ((index.column()-2) < (BeamF::nVert() * m_d->hypothesis->nDOFVert())) ){
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool BeamFModel::setData(const QModelIndex &index, const QVariant &value, int role ){
    if ( role == Qt::EditRole ){
        if( index.row() < m_d->container.size() ){
            BeamF * f = m_d->container.at(index.row());
            if( index.column() == 0 ){
                f->name->setValue( value.toString() );
                emit dataChanged(index, index);
                return true;
            } else if( index.column() == 1 ){
                f->sysRef()->setValueInt( value.toInt() );
                return true;
            } else if( (index.column() - 2) <  (BeamF::nVert() * m_d->hypothesis->nDOFVert()) ){
                f->f(index.column() - 2)->setValue( value.toDouble() );
                return true;
            }
        }
    }
    return false;
}

BeamF * BeamFModel::f(int i){
    if( i < m_d->container.size() ){
        return m_d->container.at(i);
    }
    return NULL;
}

int BeamFModel::nF(){
    return m_d->container.size();
}

bool BeamFModel::insert( int position, int count ){
    if (position < 0 || position > m_d->container.size() )
        return false;
    for (int row = position; row < (position+count); ++row) {
        beginInsertRows( QModelIndex(), position, position );

        BeamF * addedF = new BeamF( m_d->hypothesis, m_d->unitMeasure, "F" );
        // Inserisce l'elemento inserito nel contenitore
        m_d->container.insert( position, addedF );

        endInsertRows();
    }
    return true;
}

bool BeamFModel::append(){
    return insert( m_d->container.size() , 1 );
}

bool BeamFModel::remove(int position, int count ){
    if (position < 0 || (position + count) > m_d->container.size())
        return false;

    for (int row = position; row < (position+count); row++){
        // ok, procediamo a cancellare
        beginRemoveRows( QModelIndex(), position, position);

        delete m_d->container.at(position);
        m_d->container.removeAt( position );

        endRemoveRows();
    }
    return true;
}

bool BeamFModel::clear(){
    return remove( 0, m_d->container.size() );
}
