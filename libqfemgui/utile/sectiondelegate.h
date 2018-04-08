#ifndef SECTIONSSTEELCNCRDELEGATE_H
#define SECTIONSSTEELCNCRDELEGATE_H

#include <QStyledItemDelegate>

class SectionModel;

class SectionDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    SectionDelegate( SectionModel *, QObject * = 0 );

    QWidget *createEditor( QWidget *,
                           const QStyleOptionViewItem &,
                           const QModelIndex &) const;

    void setEditorData( QWidget *,
                        const QModelIndex & ) const;
    void setModelData( QWidget *,
                       QAbstractItemModel *,
                       const QModelIndex &) const;

    void paint(QPainter *, const QStyleOptionViewItem &,
               const QModelIndex & ) const;

private:
    SectionModel * m_sectionModel;
};

#endif // SECTIONSSTEELCNCRDELEGATE_H
