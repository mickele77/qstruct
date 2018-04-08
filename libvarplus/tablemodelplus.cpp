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
#include "tablemodelplus.h"

#include "boolplus.h"
#include "doubleplus.h"
#include "unitmeasure.h"
#include "tablemodelplusprivate.h"

#include <QStringList>

TableModelPlus::TableModelPlus(const QString & modName, UnitMeasure * ump, QObject *parent) :
    QAbstractTableModel(parent),
    m_d( new TableModelPlusPrivate(modName, ump ) ) {
    connect( &(m_d->mapper), SIGNAL(mapped(QString)), this, SLOT(updateData(QString)) );
}

QString TableModelPlus::modelName() {
    return m_d->modelName;
}

int TableModelPlus::rowCount(const QModelIndex &) const {
    return m_d->rowCount();
}

int TableModelPlus::columnCount(const QModelIndex &) const {
    return m_d->colCount();
}

Qt::ItemFlags TableModelPlus::flags(const QModelIndex & index) const {
    if( index.isValid() ){
        Qt::ItemFlags ret = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        int dataType = m_d->displayedDataType( index.row(), index.column() );
        if( dataType == DisplayPointer ){
            ret = ret | Qt::ItemIsEditable;
        }
        if( (dataType == DisplayValue) ||
                (dataType == DisplayValueRO ) ||
                (dataType == DisplayValueROInv ) ) {
            if( m_d->var( index.row(), index.column() )){
                if( !m_d->var( index.row(), index.column() )->readOnly() ){
                    ret = ret | Qt::ItemIsEditable;
                }
            }
        }
        if( (dataType == DisplayValueRO ) ||
                (dataType == DisplayValueROInv )) {
            if( m_d->var( index.row(), index.column() )){
                ret = ret | Qt::ItemIsUserCheckable;
            }
        }
        return ret;
    } else {
        return QAbstractTableModel::flags( index );
    }
}

QVariant TableModelPlus::headerData(int section, Qt::Orientation orientation,
                                    int role) const {
    if ( role == Qt::DisplayRole){
        if( orientation == Qt::Horizontal){
            return QVariant( m_d->header( section ));
        } else if( orientation == Qt::Vertical){
            return QVariant(section+1);
        }
    }
    return QVariant();
}

QVariant TableModelPlus::data(const QModelIndex &index, int role) const {
    if( index.isValid() ){
        int dataType = m_d->displayedDataType( index.row(), index.column() );
        if( dataType == DisplayPointer ){
            if( (role == Qt::DisplayRole) || (role == Qt::EditRole)){
                return qVariantFromValue( m_d->pointer(index.row(), index.column()) );
            }
        }
        VarPlus * var = m_d->var( index.row(), index.column() );
        if( var ){
            if( var->enabled() ){
                if( role == Qt::CheckStateRole ){
                    BoolPlus * boolVar = dynamic_cast<BoolPlus *>(var);
                    if( boolVar ) {
                        if( boolVar->value() ){
                            return QVariant( Qt::Checked );
                        } else {
                            return QVariant( Qt::Unchecked );
                        }
                    } else {
                        if( dataType == DisplayValueRO ||
                                dataType == DisplayValueROInv ){
                            bool valRO = (dataType == DisplayValueRO)? var->readOnly(): !var->readOnly();
                            if( valRO ){
                                return QVariant( Qt::Checked );
                            } else {
                                return QVariant( Qt::Unchecked );
                            }
                        }
                    }
                }
                if( (role == Qt::DisplayRole) || (role == Qt::EditRole) ){
                    if( (dataType == DisplayValue) ||
                            (dataType == DisplayValueRO) ||
                            (dataType == DisplayValueROInv) ){
                        return QVariant( var->valueStr() );
                    }
                    if( dataType == DisplayName ){
                        return QVariant( var->name() );
                    }
                    if( dataType == DisplayRichName ){
                        return QVariant( var->richName() );
                    }
                    if( dataType == DisplayUnitMeasureString ){
                        if( var->unitMeasurePointer() ){
                            return var->unitMeasurePointer()->string( var->unitMeasureValue() );
                        }
                    }
                    if( dataType == DisplayNameUnitMeasure ){
                        QString ret = var->name();
                        if( var->unitMeasurePointer() ){
                            ret += QString(" [%1]").arg( var->unitMeasurePointer()->string( var->unitMeasureValue() ) );
                        }
                        return QVariant( ret );
                    }
                    if( dataType == DisplayRichNameUnitMeasure ){
                        QString ret = var->richName();
                        if( var->unitMeasurePointer() ){
                            ret += QString(" [%1]").arg( var->unitMeasurePointer()->richString( var->unitMeasureValue() ) );
                        }
                        return QVariant( ret );
                    }
                }
                if( role == Qt::TextAlignmentRole  ){
                    if( dynamic_cast<DoublePlus *> (var) != NULL ){
                        return QVariant( Qt::AlignRight | Qt::AlignVCenter );
                    } else {
                        return QVariant( Qt::AlignLeft | Qt::AlignVCenter );
                    }
                }
            }
        }
    }
    return QVariant();
}

bool TableModelPlus::setData(const QModelIndex &index, const QVariant &value, int role) {
    VarPlus * var = m_d->var( index.row(), index.column() );
    if( var ){
        if( var->enabled() ){
            int dataType = m_d->displayedDataType( index.row(), index.column() );
            if (role == Qt::CheckStateRole ) {
                BoolPlus * boolVar = dynamic_cast<BoolPlus *>(var);
                if( boolVar ) {
                    if( (dataType == DisplayValue) && !(var->readOnly()) ){
                        boolVar->setValue( value.toInt() == Qt::Checked );
                        return true;
                    }
                }
                if( (dataType == DisplayValueRO) ||
                        (dataType == DisplayValueROInv) ){
                    bool newValRO = (value.toInt() == Qt::Checked);
                    if( dataType == DisplayValueRO ){
                        var->setReadOnly( newValRO );
                    } else if( dataType == DisplayValueROInv ){
                        var->setReadOnly( !newValRO );
                    }
                }
            }
            if (role == Qt::EditRole) {
                if( !(var->readOnly()) ){
                    if( (dataType == DisplayValue) ||
                            (dataType == DisplayValueRO) ||
                            (dataType == DisplayValueROInv) ) {
                        var->setValue( value.toString() );
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void TableModelPlus::clear() {
    removeRows( 0, rowCount() );
}

void TableModelPlus::insertRowsPrivate( int position, int count ) {
    if( position >= 0 && position <= m_d->rowCount() && count > 0 ){
        beginInsertRows( QModelIndex(), position, position+count-1 );
        m_d->insertRows( position, count );
        endInsertRows();
        emit modelChanged();
    }
}

void TableModelPlus::removeRowsPrivate( int position, int count ) {
    if( position >= 0 && position < m_d->rowCount() && count > 0 ){
        beginRemoveRows( QModelIndex(), position, position+count-1 );
        m_d->removeRows( position, count );
        endRemoveRows();
        emit modelChanged();
    }
}

bool TableModelPlus::setVar(TableModelPlus::DisplayedData dataType, int r, int c, VarPlus *var) {
    if( (r >= 0 && r < m_d->rowCount()) && (c >=0 && c < m_d->colCount() ) ){
        int oldDataType = m_d->displayedDataType(r,c);
        VarPlus * oldVar = m_d->var(r,c);
        if( oldVar ){
            m_d->mapper.removeMappings( oldVar );
            if( oldDataType == DisplayValueRO || oldDataType == DisplayValueROInv ){
                disconnect( oldVar, SIGNAL(readOnlyChanged(bool)), &(m_d->mapper), SLOT(map()) );
            }
            if( oldDataType == DisplayValue || oldDataType == DisplayValueRO || oldDataType == DisplayValueROInv ){
                disconnect( oldVar, SIGNAL(valueChanged(QString)), &(m_d->mapper), SLOT(map()) );
            }
            if( (oldDataType == DisplayName) || (oldDataType == DisplayNameUnitMeasure) ){
                disconnect( oldVar, SIGNAL(nameChanged(QString)), &(m_d->mapper), SLOT(map()) );
            }
            if( (oldDataType == DisplayRichName) || (oldDataType == DisplayRichNameUnitMeasure) ){
                disconnect( oldVar, SIGNAL(richNameChanged(QString)), &(m_d->mapper), SLOT(map()) );
            }
            if( (oldDataType == DisplayUnitMeasureRichString) || (oldDataType == DisplayRichNameUnitMeasure) ){
                disconnect( oldVar, SIGNAL(unitMeasureRichStringChanged(QString)), &(m_d->mapper), SLOT(map()) );
            }
            if( (oldDataType == DisplayUnitMeasureString) || (oldDataType == DisplayNameUnitMeasure) ){
                disconnect( oldVar, SIGNAL(unitMeasureStringChanged(QString)), &(m_d->mapper), SLOT(map()) );
            }
        }

        m_d->setVar( dataType, r, c, var );

        if( var ){
            m_d->mapper.setMapping( var, QString("%1,%2").arg( QString::number(r),QString::number(c)) );
            if( dataType == DisplayValueRO || dataType == DisplayValueROInv ){
                connect( var, SIGNAL(readOnlyChanged(bool)), &(m_d->mapper), SLOT(map()) );
            }
            if( dataType == DisplayValue || dataType == DisplayValueRO || dataType == DisplayValueROInv ){
                connect( var, SIGNAL(valueChanged(QString)), &(m_d->mapper), SLOT(map()) );
            }
            if( (dataType == DisplayName) || (dataType == DisplayNameUnitMeasure) ){
                connect( var, SIGNAL(nameChanged(QString)), &(m_d->mapper), SLOT(map()) );
            }
            if( (dataType == DisplayRichName) || (dataType == DisplayRichNameUnitMeasure) ){
                connect( var, SIGNAL(richNameChanged(QString)), &(m_d->mapper), SLOT(map()) );
            }
            if( (dataType == DisplayUnitMeasureRichString) || (dataType == DisplayRichNameUnitMeasure) ){
                connect( var, SIGNAL(unitMeasureRichStringChanged(QString)), &(m_d->mapper), SLOT(map()) );
            }
            if( (dataType == DisplayUnitMeasureString) || (dataType == DisplayNameUnitMeasure) ){
                connect( var, SIGNAL(unitMeasureStringChanged(QString)), &(m_d->mapper), SLOT(map()) );
            }
        }

        QModelIndex index = createIndex( r, c );
        emit dataChanged( index,index);

        return true;
    }
    return false;
}

bool TableModelPlus::setVarValue(int r, int c , VarPlus * var ){
    return setVar( DisplayValue, r, c, var );
}

void TableModelPlus::setVarReadOnly(int r, int c, bool ro) {
    if( (r >= 0 && r < m_d->rowCount()) && (c >=0 && c < m_d->colCount() ) ){
        m_d->var(r, c)->setReadOnly( ro );
    }
}

void TableModelPlus::setVarValueRow(int r, VarPlus *firstVar, ...){
    va_list vars;
    va_start (vars, firstVar);

    int c=0;
    VarPlus * var = firstVar;
    while( var != NULL && c < m_d->colCount() ) {
        setVarValue( r, c, var  );
        ++c;
        var = va_arg(vars, VarPlus *);
    }
    va_end(vars);
}

bool TableModelPlus::setVarNameUnitMeasure(int r, int c, VarPlus *var) {
    return setVar( DisplayNameUnitMeasure, r, c, var );
}

void TableModelPlus::updateData(const QString &rowColString) {
    QStringList rowCol = rowColString.split( ",");
    if( rowCol.size() > 1 ){
        QModelIndex index = createIndex( rowCol.at(0).toInt(), rowCol.at(1).toInt());
        emit dataChanged( index, index);
        emit modelChanged();
    }
}

void TableModelPlus::setHeaders(const QList<QString> &hC) {
    m_d->setHeaders( hC );
    emit headerDataChanged( Qt::Horizontal, 0, columnCount() );
}
