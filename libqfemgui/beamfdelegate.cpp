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

#include "beamfdelegate.h"

#include "beamf.h"
#include "hypothesis.h"

#include "qstringplus.h"
#include "uintplus.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QVariant>

class BeamFDelegatePrivate{
public:
    BeamFDelegatePrivate( Hypothesis * hyp ):
        hypothesis(hyp)
    {;}
    Hypothesis * hypothesis;
};

BeamFDelegate::BeamFDelegate(Hypothesis * hyp, QObject *parent ):
    QStyledItemDelegate(parent),
    m_d( new BeamFDelegatePrivate(hyp) ){
}

QString BeamFDelegate::sysRefGlobalCaption( int value ) const {
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

void BeamFDelegate::paint(QPainter *painter,
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
        QString ret = BeamFSysRef::enumStrStatic( value.toInt());
        painter->drawText(option.rect, Qt::AlignVCenter, ret );
        painter->restore();
        break;}
    default:{
        QStyledItemDelegate::paint(painter, option, index);
        break;}
    }
}

QWidget * BeamFDelegate::createEditor(QWidget *parent,
                                      const QStyleOptionViewItem & viewItem,
                                      const QModelIndex & index ) const {
    switch( index.column() ) {
    case 1: {
        QComboBox * box = new QComboBox( parent );
        int currIndex = -1;
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        for( int i = 0; i < BeamFSysRef::enumCountStatic(); i++ ){
            box->addItem( BeamFSysRef::enumStrStatic(i), QVariant( i ) );
            if( i == value.toInt() ){
                currIndex = i;
            }
        }
        box->setCurrentIndex( currIndex );
        box->installEventFilter(const_cast<BeamFDelegate *>(this));
        return box;
        break;
    }
    default: {
        break;
    }
    }
    return QStyledItemDelegate::createEditor( parent, viewItem, index );
}

void BeamFDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const {
    switch( index.column() ) {
    case 1:{
        int currIndex = -1;
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        for( int i = 0; i < BeamFSysRef::enumCountStatic(); i++ ){
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

void BeamFDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const {
    if( index.column() == 1 ){
        model->setData( index, static_cast<QComboBox*>( editor )->itemData( static_cast<QComboBox*>( editor )->currentIndex() ) );
    } else {
        QStyledItemDelegate::setModelData( editor, model, index );
    }
}
