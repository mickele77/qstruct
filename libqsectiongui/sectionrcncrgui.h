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
#ifndef SECTIONRCNCRGUI_H
#define SECTIONRCNCRGUI_H

#include "sectiongui.h"

namespace Ui {
class SectionRCncrGUI;
}
class SectionGeometricDataPanel;

class SectionRCncr;
class SectionFRP;
class MaterialModel;
class SectionRCncrGUIPrivate;

class QModelIndex;
class QGraphicsScene;
class QGraphicsPolygonItem;

class SectionRCncrGUI : public SectionGUI {
    Q_OBJECT

public:
    explicit SectionRCncrGUI(UnitMeasure * ump, MaterialModel * matModel, const QString & settingsFile, QWidget *parent = 0);
    ~SectionRCncrGUI();

public slots:
    void setSection( Section * sect );

private:
    SectionRCncrGUIPrivate * m_d;

    Material * material();

    void setCurrentSectionFRP(SectionFRP *sectFRP);

private slots:
    void addSectionCncr();
    void removeSectionCncr();
    void addSectCncrPoint();
    void removeSectCncrPoint();
    void addSectionSteelCncr();
    void removeSectionSteelCncr();
    void addSectionSteel();
    void removeSectionSteel();
    void addSectionFRP();
    void removeSectionFRP();
    void setSectionCncrRectangular();

    void FRPpopulateConcreteComboBox();
    void FRPsetConcreteFromCombobox();
    void FRPpopulateFRPComboBox();
    void FRPsetFRPFromCombobox();


    void setCurrentSectionCncr( const QModelIndex &, const QModelIndex & );
    void setCurrentSectionSteel( const QModelIndex &, const QModelIndex & );
    void setCurrentSectionFRP( const QModelIndex &, const QModelIndex & );

    void setNCenULSY();
    void setNCenULSZ();
    void calculateMULS();
    void copyToClipboardMULSyMULSz();
    void copyToFileMULSN();
    void plotMULSyMULSz();
    void copyToClipboardMULSN();
    void copyToFileMULSyMULSz();
    void plotMULSN();

    void setYLSPlusNCenULS();
    void setZLSPlusNCenULS();

    void calculateNMULS();

    void setMaterialFromComboBox();

    void LSPlusULSLoadStoryAddLoad();
    void LSPlusULSLoadStoryRemoveLoad();
    void LSPlusULSLoadStoryCalculate();

    void setCurrentSectionFRPNULL();

    void setNCenSLSY();
    void setNCenSLSZ();
    void SLSCalculate();
};

#endif // SECTIONRCNCRGUI_H
