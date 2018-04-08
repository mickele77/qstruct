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
#ifndef UNITMEASUREGUI_H
#define UNITMEASUREGUI_H

#include "varplus_export.h"
#include <QDialog>

class UnitMeasure;
class QString;
class QLineEdit;
template <typename T> class QVector;

class EXPORT_VARPLUS_LIB_OPT UnitMeasureGUI : public QDialog
{
    Q_OBJECT
public:
    explicit UnitMeasureGUI(UnitMeasure *, QWidget *parent = 0);

signals:

public slots:
    void accept();

private:
    UnitMeasure * m_model;
    QVector<QLineEdit *> * m_scaleLEVector;
    QVector<QLineEdit *> * m_strLEVector;
    QVector<QLineEdit *> * m_richStrLEVector;

    void buildGUI();
    QString toHTML( const QString & );
};

#endif // UNITMEASUREGUI_H
