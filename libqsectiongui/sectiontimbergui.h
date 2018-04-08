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
#ifndef SECTIONTIMBERGUI_H
#define SECTIONTIMBERGUI_H

class SectionTimberGUIPrivate;

#include "sectiongui.h"

namespace Ui {
class SectionTimberGUI;
}
class SectionTimber;
class SectionTimberDataSetModel;
class LSType;
class ServiceClass;
class LoadDurationClass;
class LoadComplTableModel;
class SSBeamTimber;

class DoublePlus;
class Point2DPlus;

class QGraphicsScene;
class QGraphicsItemGroup;

class SectionTimberGUI : public SectionGUI
{
    Q_OBJECT

public:
    explicit SectionTimberGUI(UnitMeasure * um, MaterialModel * matModel, QWidget *parent = 0);
    ~SectionTimberGUI();

public slots:
    void setSection( Section * sect );

private:
    SectionTimberGUIPrivate * m_d;

    void initVar();

    Material *material();
    QList<Material *> materialsList();

private slots:
    void setBeamL0z();
    void setBeamL0LT();
    void setNULSMinus();
    void setNULSPlus();
    void setVULS();
    void setMULSX();
    void setMULSY();
    void setMULSZ();

    void setBeamlY();
    void setBeamlRelY();
    void setKBucklY();
    void setKCrBucklY();
    void setNULSBucklY();

    void setBeamlZ();
    void setBeamlRelZ();
    void setKBucklZ();
    void setKCrBucklZ();
    void setNULSBucklZ();

    void setYCenMSd();
    void setZCenMSd();

    void setBeamlRelLTY();
    void setKCrBucklLTY();
    void setMULSBucklLTY();
    void setBeamlRelLTZ();
    void setKCrBucklLTZ();
    void setMULSBucklLTZ();

    void setRd();

    void verifyULS();

    void dataSetClear();
    void dataSetLoadFromFile();
    void dataSetVerifyULSAll();
    void dataSetFromTableToGUI();
    void dataSetFromGUIToTable();

    void SSBeamAddLoad();
    void SSBeamRemoveLoad();

    void setMaterialFromComboBox();
};

#endif // SECTIONTIMBERGUI_H
