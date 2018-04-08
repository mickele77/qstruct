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

#include "beamqdelegate.h"

#include "beamq.h"
#include "hypothesis.h"

#include "qstringplus.h"
#include "uintplus.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QVariant>

class BeamQDelegatePrivate{
public:
    BeamQDelegatePrivate( Hypothesis * hyp ):
        hypothesis(hyp)
    {;}
    Hypothesis * hypothesis;
};

BeamQDelegate::BeamQDelegate(Hypothesis * hyp, QObject *parent ):
    QStyledItemDelegate(parent),
    m_d( new BeamQDelegatePrivate(hyp) ){
}

QString BeamQDelegate::sysRefGlobalCaption( int value ) const {
    QString ret;
    if( value >= 0 && value < m_d->hypothesis->spaceDim()){
        for( int j = 0; j < m_d->hypothesis->spaceDim(); j++){
            if( j != value ){
                ret += m_d->hypothesis->dimCaption( j );
            }
        }
    }
    return ret;
}

void BeamQDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const {
    switch( index.column() ){
    case 1:{
        Q_ASSERT(index.isValid());
        painter->save();
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            QPen pen = painter->pen();
            pen.setBrush( option.palette.highlightedText());
            painter->setPen( pen );
        }
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        QString ret = BeamQSysRef::enumStrStatic( value.toInt());
        painter->drawText(option.rect, Qt::AlignVCenter, ret );
        painter->restore();
        break;}
    case 2:{
        Q_ASSERT(index.isValid());
        painter->save();
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            QPen pen = painter->pen();
            pen.setBrush( option.palette.highlightedText());
            painter->setPen( pen );
        }
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        QString caption = sysRefGlobalCaption(value.toInt());
        painter->drawText(option.rect, Qt::AlignVCenter, caption );
        painter->restore();
        break;}
    default:{
        QStyledItemDelegate::paint(painter, option, index);
        break;}
    }
}

QWidget * BeamQDelegate::createEditor(QWidget *parent,
                                      const QStyleOptionViewItem & viewItem,
                                      const QModelIndex & index ) const {
    switch( index.column() ) {
    case 1: {
        QComboBox * box = new QComboBox( parent );
        int currIndex = -1;
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        for( int i = 0; i < BeamQSysRef::enumCountStatic(); i++ ){
            box->addItem( BeamQSysRef::enumStrStatic(i), QVariant( i ) );
            if( i == value.toInt() ){
                currIndex = i;
            }
        }
        box->setCurrentIndex( currIndex );
        box->installEventFilter(const_cast<BeamQDelegate *>(this));
        return box;
        break;
    }
    case 2: {
        QComboBox * box = new QComboBox( parent );
        int currIndex = -1;
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        for( int i = 0; i < m_d->hypothesis->spaceDim(); i++ ){
            box->addItem( sysRefGlobalCaption(i), QVariant( i ) );
            if( i == value.toInt() ){
                currIndex = i;
            }
        }
        box->setCurrentIndex( currIndex );
        box->installEventFilter(const_cast<BeamQDelegate *>(this));
        return box;
        break;
    }
    default: {
        break;
    }
    }
    return QStyledItemDelegate::createEditor( parent, viewItem, index );
}

void BeamQDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const {
    switch( index.column() ) {
    case 1:{
        int currIndex = -1;
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        for( int i = 0; i < BeamQSysRef::enumCountStatic(); i++ ){
            if( i == value.toInt() ){
                currIndex = i;
            }
        }
        static_cast<QComboBox *>( editor )->setCurrentIndex( currIndex );
        break;
    }
    case 2:{
        int currIndex = -1;
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        for( int i = 0; i < m_d->hypothesis->spaceDim(); i++ ){
            if( i == value.toInt() ){
                currIndex = i;
            }
        }
        static_cast<QComboBox *>( editor )->setCurrentIndex( currIndex );
        break;
    }
    default: {
        QStyledItemDelegate::setEditorData( editor, index );
        break;
    }
    }
}

void BeamQDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const {
    if( index.column() == 1 || index.column() == 2 ){
        // int currId = (static_cast<QComboBox*>( editor )->itemData( static_cast<QComboBox*>( editor )->currentIndex() )).toInt();
        model->setData( index, static_cast<QComboBox*>( editor )->itemData( static_cast<QComboBox*>( editor )->currentIndex() ) );
    } else {
        QStyledItemDelegate::setModelData( editor, model, index );
    }
}
