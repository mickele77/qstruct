#include "sectiondelegate.h"

#include "sectionmodel.h"
#include "section.h"

#include "qstringplus.h"
#include "uintplus.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QVariant>

SectionDelegate::SectionDelegate( SectionModel * model, QObject *parent ):
    QStyledItemDelegate(parent),
    m_sectionModel(model) {
}

void SectionDelegate::paint(QPainter *painter,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const {
    if (index.column() == 1) {
        Q_ASSERT(index.isValid());
        painter->save();
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            QPen pen = painter->pen();
            pen.setBrush( option.palette.highlightedText());
            painter->setPen( pen );
        }
        Section * currentSection = m_sectionModel->sectionId( index.data().toUInt() );
        if( currentSection ) {
            painter->drawText(option.rect, Qt::AlignVCenter, currentSection->name->valueStr() );
        }
        painter->restore();
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QWidget * SectionDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem & viewItem,
                                        const QModelIndex & index ) const {
    if( index.column() == 1 ) {
        QComboBox * box = new QComboBox( parent );
        int currIndex = -1;
        for( int i = 0; i < m_sectionModel->nSection(); i++ ){
            Section * section = m_sectionModel->section(i);
            if( section ){
                box->addItem( section->name->valueStr(), QVariant(section->id->value() ) );
                QVariant value = index.model()->data( index, Qt::DisplayRole );
                if( section->id->value() == value.toUInt() ){
                    currIndex = i;
                }
            }
        }
        if( currIndex > -1 ){
            box->setCurrentIndex( currIndex );
        }
        box->installEventFilter(const_cast<SectionDelegate *>(this));

        return box;
    } else {
        return QStyledItemDelegate::createEditor( parent, viewItem, index );
    }
}

void SectionDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const {
    if( index.column() == 1 ){
        QVariant value = index.model()->data( index, Qt::DisplayRole );
        int currIndex = -1;
        for( int i = 0; i < m_sectionModel->nSection(); i++ ){
            Section * section = m_sectionModel->section(i);
            if( section ){
                if( section->id->value() == value.toUInt() ){
                    currIndex = i;
                }
            }
        }
        if( currIndex > -1 ){
            static_cast<QComboBox *>( editor )->setCurrentIndex( currIndex );
        }
    } else {
        QStyledItemDelegate::setEditorData( editor, index );
    }
}

void SectionDelegate::setModelData(QWidget *editor,
                                   QAbstractItemModel *model,
                                   const QModelIndex &index) const {
    if( index.column() == 1 ){
        unsigned int currId = (static_cast<QComboBox*>( editor )->itemData( static_cast<QComboBox*>( editor )->currentIndex() )).toUInt();
        model->setData( index, QVariant(currId) );
    } else {
        QStyledItemDelegate::setModelData( editor, model, index );
    }
}
