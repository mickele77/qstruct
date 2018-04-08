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
#ifndef WOODGUI_H
#define WOODGUI_H

#include "materialgui.h"

namespace Ui {
    class TimberGUI;
}
class TimberGUIPrivate;
class Timber;

class LSType;
class ServiceClass;
class LoadDurationClass;
class SectionTimberShape;
class DoublePlus;

class TimberGUI : public MaterialGUI {
    Q_OBJECT

public:
    TimberGUI(UnitMeasure * um, const QString & settingsFile, QWidget *parent = 0);
    ~TimberGUI();

public slots:
    void setMaterial( Material * );

private:
    TimberGUIPrivate * m_d;

    void loadStandardComboBox();
    void initVar();

private slots:
    void updateType2FilterStandardComboBox();
    void updateWoodNameStandardComboBox();
    void loadStandardData();

    void setCurrentKmod();
    void setCurrentGammaM();

    void updateSectionData();

    void updatefc0d();
    void updatefc90d();
    void updateft0d();
    void updateft90d();
    void updatefmdh();
    void updatefmdb();
    void updatefvd();
    void updateftorsd();

    void updatekm();
    void updatekhm();
    void updatekbm();
    void updatekh();
    void updatekV();
    void updateksh();
};


#endif // WOODGUI_H
