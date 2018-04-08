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
#ifndef TIMBERCONNECTIONGUI_H
#define TIMBERCONNECTIONGUI_H

class VarPlus;
class UnitMeasure;
class MaterialModel;
class TimberConnection;
template <class Key, class T> class QMap;

#include "varpluscontainer.h"
#include <QWidget>

class TimberConnectionGUIPrivate;

class TimberConnectionGUI : public QWidget, public VarPlusContainer
{
    Q_OBJECT

public:
    explicit TimberConnectionGUI( UnitMeasure * ump, MaterialModel * matModel, QWidget *parent = 0);
    ~TimberConnectionGUI();

    void setConnection( TimberConnection * );

private:
    TimberConnectionGUIPrivate * m_d;

    void showEvent ( QShowEvent * event );
    void hideEvent(QHideEvent *);

    void populateTimberComboBoxes();

    void initVar();

private slots:
    void setTimber1( int );
    void setTimber2( int );
    void setSteel( int );

    void setVisibleGBox();

    void verify();
};

#endif // TIMBERCONNECTIONGUI_H
