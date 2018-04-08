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
#include "loadcompltabledelegate.h"

#include "loaddurationclass.h"
#include "loadtype.h"

#include <QComboBox>

LoadComplTableDelegate::LoadComplTableDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
    m_indexLType = 1;
    m_indexDuration = 4;
}

QWidget * LoadComplTableDelegate::createEditor(QWidget *parent,
                                             const QStyleOptionViewItem & viewItem,
                                             const QModelIndex & index ) const {
    if( index.column() == m_indexLType ) {
        QComboBox * box = new QComboBox( parent );
        for( int i = 0; i < LoadType::enumCountStatic(); i++ ){
             box->addItem( LoadType::enumStrStatic(i) );
        }
        box->setCurrentIndex( index.model()->data( index, Qt::DisplayRole ).toInt() );
        box->installEventFilter(const_cast<LoadComplTableDelegate *>(this));
        return box;
    } else if( index.column() == m_indexDuration ) {
        QComboBox * box = new QComboBox( parent );
        for( int i = 0; i < LoadDurationClass::nVal(); i++ ){
             box->addItem( LoadDurationClass::valStr(i), QVariant( i ) );
        }
        box->setCurrentIndex( index.model()->data( index, Qt::DisplayRole ).toInt() );
        box->installEventFilter(const_cast<LoadComplTableDelegate *>(this));
        return box;
    } else {
        return QStyledItemDelegate::createEditor( parent, viewItem, index );
    }
}

void LoadComplTableDelegate::setEditorData(QWidget *editor,
                                         const QModelIndex &index) const {
    if( index.column() == m_indexLType || index.column() == m_indexDuration ){
        static_cast<QComboBox *>( editor )->setCurrentIndex( index.model()->data( index, Qt::DisplayRole ).toInt() );
    } else {
        QStyledItemDelegate::setEditorData( editor, index );
    }
}

void LoadComplTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                        const QModelIndex &index) const {
    if( index.column() == m_indexLType || index.column() == m_indexDuration ){
        int id = static_cast<QComboBox*>( editor )->currentIndex() ;
        model->setData( index, QVariant( id ) );
    } else {
        QStyledItemDelegate::setModelData( editor, model, index );
    }
}
