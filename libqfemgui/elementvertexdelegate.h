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
#ifndef SECTIONSCNCRDELEGATE_H
#define SECTIONSCNCRDELEGATE_H

#include <QStyledItemDelegate>

class VertexModel;
class ElementVertexModel;

class ElementVertexDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    ElementVertexDelegate( VertexModel *, ElementVertexModel *, QObject * = 0 );

    QWidget *createEditor( QWidget *, const QStyleOptionViewItem &,
                          const QModelIndex &) const;

    void setEditorData( QWidget *, const QModelIndex & ) const;
    void setModelData( QWidget *, QAbstractItemModel *,
                      const QModelIndex &) const;

    void paint(QPainter *, const QStyleOptionViewItem &,
               const QModelIndex & ) const;

private:
    VertexModel * m_vertexModel;
    ElementVertexModel * m_elementVertexModel;
};

#endif // SECTIONSCNCRDELEGATE_H

