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
#include "beamqmodel.h"

#include "hypothesis.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "uintplus.h"
#include "unitmeasure.h"

class BeamQModelPrivate{
public:
    BeamQModelPrivate( Hypothesis * hyp, UnitMeasure * ump ){
        unitMeasure = ump;
        hypothesis = hyp;
    };

    QList<BeamQ *> container;
    UnitMeasure * unitMeasure;
    Hypothesis * hypothesis;
};

BeamQModel::BeamQModel(Hypothesis *hyp, UnitMeasure *ump, QObject *parent) :
    QAbstractTableModel(parent),
    m_d( new BeamQModelPrivate( hyp, ump ) ) {
}

UnitMeasure * BeamQModel::unitMeasure(){
    return m_d->unitMeasure;
}

Hypothesis * BeamQModel::hypothesis(){
    return m_d->hypothesis;
}

int BeamQModel::rowCount( const QModelIndex & ) const {
    return m_d->container.size();
}

int BeamQModel::columnCount( const QModelIndex & ) const {
    return (3 + BeamQ::nVert() * m_d->hypothesis->nDOFVert());
}

QVariant BeamQModel::data( const QModelIndex & index, int role ) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_d->container.size() || index.row() < 0)
        return QVariant();

    if( index.row() < m_d->container.size() && role == Qt::DisplayRole ){
        BeamQ * q = m_d->container.at( index.row() );
        switch( index.column() ){
        case 0:{
            return QVariant( q->name->valueStr() );
            break; }
        case 1:{
            return QVariant( q->sysRef()->valueNormalStr() );
            break;}
        case 2: {
            if( q->sysRef()->valueNormal() == BeamQ::qGlobalSR ){
                return QVariant( q->sysRefGlobal()->valueNormalStr() );
            } else {
                return QVariant(-1);
            }
            break;}
        default:{
            if( (index.column()-3) < (BeamQ::nVert() * m_d->hypothesis->nDOFVert()) ){
                return QVariant( q->q(index.column() - 3 )->valueStr() );
            }
            break; }
        }
    }

    return QVariant();
}

QVariant BeamQModel::headerData(int section, Qt::Orientation orientation,
                                int role) const{
    if ( role == Qt::DisplayRole){
        if( orientation == Qt::Horizontal){
            switch( section ){
            case 0:{
                return trUtf8("Nome"); }
            case 1:{
                return QVariant( trUtf8("Rif-1") );}
            case 2:{
                return QVariant( trUtf8("Rif-2") );}
            default:{
                if( (section - 3) < (BeamQ::nVert() * m_d->hypothesis->nDOFVert()) ){
                    QString tit = "q" + QString::number( section - 2 );
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

Qt::ItemFlags BeamQModel::flags(const QModelIndex &index) const{
    if (!index.isValid())
        return 0;

    if( (index.column() >= 0) && ((index.column()-3) < (BeamQ::nVert() * m_d->hypothesis->nDOFVert())) ){
        if( index.column() == 2 ){
            BeamQ * q = m_d->container.at(index.row());
            if( q->sysRef()->valueNormal() != BeamQ::qGlobalSR){
                return 0;
            }
        }
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool BeamQModel::setData(const QModelIndex &index, const QVariant &value, int role ){
    if ( role == Qt::EditRole ){
        if( index.row() < m_d->container.size() ){
            BeamQ * q = m_d->container.at(index.row());
            if( index.column() == 0 ){
                q->name->setValue( value.toString() );
                emit dataChanged(index, index);
                return true;
            } else if( index.column() == 1 ){
                q->sysRef()->setValueInt( value.toInt() );
                return true;
            } else if( index.column() == 2 ){
                q->sysRefGlobal()->setValueNormal( value.toInt() );
                return true;
            } else if( (index.column() - 3) <  (BeamQ::nVert() * m_d->hypothesis->nDOFVert()) ){
                q->q(index.column() - 3)->setValue( value.toDouble() );
                return true;
            }
        }
    }
    return false;
}

BeamQ * BeamQModel::q(int i){
    if( i < m_d->container.size() ){
        return m_d->container.at(i);
    }
    return NULL;
}

int BeamQModel::nQ(){
    return m_d->container.size();
}

bool BeamQModel::insert( int position, int count ){
    if (position < 0 || position > m_d->container.size() )
        return false;
    for (int row = position; row < (position+count); ++row) {
        beginInsertRows( QModelIndex(), position, position );

        BeamQ * addedQ = new BeamQ( m_d->hypothesis, m_d->unitMeasure, "q" );
        // Inserisce l'elemento inserito nel contenitore
        m_d->container.insert( position, addedQ );

        endInsertRows();
    }
    return true;
}

bool BeamQModel::append(){
    return insert( m_d->container.size() , 1 );
}

bool BeamQModel::remove(int position, int count ){
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

bool BeamQModel::clear(){
    return remove( 0, m_d->container.size() );
}
