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
#include "sectionmaterialdelegate.h"

#include "qstringplus.h"
#include "uintplus.h"

#include "concrete.h"
#include "materialmodel.h"
#include "sectionloadphasemodel.h"
#include "sectionloadphase.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QVariant>

SectionMaterialDelegate::SectionMaterialDelegate(MaterialModel * matModel,
                                                 MaterialModel::MaterialType mt,
                                                 int matCol,
                                                 int lPhaseCol,
                                                 QObject * parent ):
    QStyledItemDelegate(parent),
    m_materialModel(matModel),
    m_materialType(mt),
    m_materialColumn(matCol),
    m_loadPhaseModel(NULL),
    m_loadPhaseColumn(lPhaseCol){
}

void SectionMaterialDelegate::paint(QPainter *painter,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const {
    if (index.column() == m_materialColumn) {
        Q_ASSERT(index.isValid());
        painter->save();
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            QPen pen = painter->pen();
            pen.setBrush( option.palette.highlightedText());
            painter->setPen( pen );
        }
        Material * currentMaterial = (Material *) index.data().value<void *>();
        if( currentMaterial ) {
            painter->drawText(option.rect, Qt::AlignVCenter, currentMaterial->name->valueStr() );
        }
        painter->restore();
    } else if (index.column() == m_loadPhaseColumn) {
        Q_ASSERT(index.isValid());
        painter->save();
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            QPen pen = painter->pen();
            pen.setBrush( option.palette.highlightedText());
            painter->setPen( pen );
        }
        SectionLoadPhase * currLoadPhase = (SectionLoadPhase *) index.data().value<void *>();
        if( currLoadPhase ) {
            painter->drawText(option.rect, Qt::AlignVCenter, currLoadPhase->name->valueStr() );
        }
        painter->restore();
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QWidget * SectionMaterialDelegate::createEditor(QWidget *parent,
                                                const QStyleOptionViewItem & viewItem,
                                                const QModelIndex & index ) const {
    if( index.column() == m_materialColumn ) {
        QComboBox * cBox = new QComboBox( parent );
        QList< Material * > matList = m_materialModel->materialsList( m_materialType );
        for( QList< Material * >::iterator i=matList.begin(); i != matList.end(); ++i ){
            cBox->addItem( (*i)->name->valueStr(), qVariantFromValue( (void *)(*i) ) );
        }
        return cBox;
    } else if( index.column() == m_loadPhaseColumn && m_loadPhaseModel != NULL ) {
        QComboBox * cBox = new QComboBox( parent );
        QList< SectionLoadPhase * > lpList = m_loadPhaseModel->loadPhasesList();
        for( QList< SectionLoadPhase * >::iterator i=lpList.begin(); i != lpList.end(); ++i ){
            cBox->addItem( (*i)->name->valueStr(), qVariantFromValue( (void *)(*i) ) );
        }
        return cBox;
    } else {
        return QStyledItemDelegate::createEditor( parent, viewItem, index );
    }
}

void SectionMaterialDelegate::setEditorData(QWidget *editor,
                                            const QModelIndex &index) const {
    if( (index.column() == m_materialColumn) || (index.column() == m_loadPhaseColumn) ){
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        QComboBox * cBox = static_cast<QComboBox *>( editor );
        cBox->setCurrentIndex( cBox->findData( value ) );
    } else {
        QStyledItemDelegate::setEditorData( editor, index );
    }
}

void SectionMaterialDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                           const QModelIndex &index) const {
    if( (index.column() == m_materialColumn) || (index.column() == m_loadPhaseColumn) ){
        model->setData( index, static_cast<QComboBox*>( editor )->itemData( static_cast<QComboBox*>( editor )->currentIndex()) );
    } else {
        QStyledItemDelegate::setModelData( editor, model, index );
    }
}

void SectionMaterialDelegate::setLoadPhaseModel( SectionLoadPhaseModel * loadModel) {
    m_loadPhaseModel = loadModel;
}
