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
#include "elementvertexdelegate.h"

#include "vertex.h"
#include "vertexmodel.h"
#include "elementvertexmodel.h"
#include "uintplus.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QVariant>

ElementVertexDelegate::ElementVertexDelegate( VertexModel * model, ElementVertexModel * elementModel, QObject *parent ):
    QStyledItemDelegate(parent),
    m_vertexModel(model),
    m_elementVertexModel(elementModel){
}

void ElementVertexDelegate::paint(QPainter *painter,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const {
    if (index.row() == 0) {
        Q_ASSERT(index.isValid());
        painter->save();
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            QPen pen = painter->pen();
            pen.setBrush( option.palette.highlightedText());
            painter->setPen( pen );
        }
        painter->drawText(option.rect, Qt::AlignVCenter, QString("P%1").arg( index.data().toUInt() ) );
        painter->restore();
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QWidget * ElementVertexDelegate::createEditor(QWidget *parent,
                                              const QStyleOptionViewItem & viewItem,
                                              const QModelIndex & index ) const {
    if( index.row() == 0 ) {
        QComboBox * box = new QComboBox( parent );
        VertexModel::VertexIterator iter = m_vertexModel->begin();
        while( iter != m_vertexModel->end() ){
            box->addItem( QString("P%1").arg((*iter)->id->value() ) , QVariant( (*iter)->id->valueNormal() ) );
            iter++;
        }
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        box->setCurrentIndex( box->findData( value ) );
        box->installEventFilter(const_cast<ElementVertexDelegate *>(this));

        return box;
    } else {
        return QStyledItemDelegate::createEditor( parent, viewItem, index );
    }
}

void ElementVertexDelegate::setEditorData(QWidget *editor,
                                          const QModelIndex &index) const {
    if( index.row() == 0 ){
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        QComboBox * box = static_cast<QComboBox *>( editor );
        box->setCurrentIndex( box->findData( value ) );
    } else {
        QStyledItemDelegate::setEditorData( editor, index );
    }
}

void ElementVertexDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                         const QModelIndex &index) const {
    if( index.row() == 0 ){
        int currIndex = dynamic_cast<QComboBox*>( editor )->currentIndex();
        model->setData( index, (dynamic_cast<QComboBox*>( editor )->itemData( currIndex) ) );
    } else {
        QStyledItemDelegate::setModelData( editor, model, index );
    }
}
