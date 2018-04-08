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
#ifndef SECTIONSTEELGUI_H
#define SECTIONSTEELGUI_H

#include <QString>

class SectionData {
public:
    int id;

    QString name;

    double A;

    double Iy;
    double iy;
    double Wy;
    double WPly;

    double Iz;
    double iz;
    double Wz;
    double WPlz;
};


#include <QAbstractTableModel>

class QSqlQueryModel;
class UnitMeasure;

class StandardSectionSteelModel: public QAbstractTableModel{
    Q_OBJECT
private:
    QList<SectionData> * m_dataContainer;
    UnitMeasure * m_unitMeasure;
public:
    StandardSectionSteelModel( UnitMeasure * ump, QObject * parent = 0 );
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;

    void setQueryModel( const QSqlQueryModel & );

    int id( int );
};

#include "sectiongui.h"

namespace Ui {
class SectionSteelGUI;
}
class SectionSteelGUIPrivate;

class QGraphicsPolygonItem;
class QLabel;
class QLineEdit;
class QGridLayout;

class MaterialModel;
class SectionSteelDataSetModel;
class SectionSteel;

class BoolPlus;
class DoublePlus;
class Point2DPlus;
class QStringPlus;
class SectionSteelMomentDistribution;
class SectionSteelTorsionalDeformability;

class SectionSteelGUI : public SectionGUI {
    Q_OBJECT

public:
    explicit SectionSteelGUI( UnitMeasure * um, MaterialModel * matModel, const QString & settingsFile, QWidget *parent = 0);
    ~SectionSteelGUI();

    SectionSteel *sectionSteel();

public slots:
    void setSection( Section * sect );

private:
    SectionSteelGUIPrivate * m_d;

    void initVar();
    void loadStandardComboBox();

    Material *material();
    QList<Material *> materialsList();

private slots:
    void updateType2StandardComboBox();
    void updateType3StandardComboBox();
    void updateStandardModel();
    void loadStandardData();

    void setBeamlY();
    void setBeamNCrY();
    void setBeamlRelY();
    void setBeamChiY();
    void setBeamNULSBucklY();
    void setBeamlZ();
    void setBeamNCrZ();
    void setBeamlRelZ();
    void setBeamChiZ();
    void setBeamNULSBucklZ();
    void setBeamMCr();
    void setBeamlRelLT();
    void setBeamChiLT();

    void verifyULS();
    void setYCenMSd();
    void setZCenMSd();

    void calculateMULS();
    void setYCenMULSN();
    void setZCenMULSN();
    void plotMULSyMULSz();
    void plotMULSN();
    void plotClassDiagram();

    void setMaterialFromComboBox();

    void clearSectsToView();
    void setNMyMzElastPlast();
    void setNMyMzPlast();

    void dataSetClear();
    void dataSetLoadFromFile();
    void dataSetVerifyULSAll();
    void dataSetFromTableToGUI();
    void dataSetFromGUIToTable();

    void setSectionNULL();

    void updateVisibleP();
};

#endif // SECTIONSTEELGUI_H
