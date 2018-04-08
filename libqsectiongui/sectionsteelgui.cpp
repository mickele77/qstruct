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
#include "sectionsteelgui.h"
#include "ui_sectionsteelgui.h"

#include "sectionclassdiagramgui.h"

#include "sectionsteel.h"
#include "sectionsteelvar.h"

#include "steel.h"
#include "materialmodel.h"
#include "material.h"
#include "sectionsteeldatasetmodel.h"

#include "qstringplus.h"
#include "doubleplus.h"
#include "boolplus.h"
#include "uintplus.h"
#include "point2dplus.h"
#include "point2dplusmodel.h"

#include "varplusguiutility.h"
#include "simpledataplotterdialog.h"

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

StandardSectionSteelModel::StandardSectionSteelModel( UnitMeasure * ump, QObject * parent ):
    QAbstractTableModel(parent ),
    m_dataContainer( new QList<SectionData> ),
    m_unitMeasure(ump){
}

int StandardSectionSteelModel::columnCount ( const QModelIndex & parent ) const{
    Q_UNUSED(parent);
    return 11;
}

int StandardSectionSteelModel::rowCount ( const QModelIndex & parent ) const{
    Q_UNUSED(parent);
    return m_dataContainer->size();
}

QVariant StandardSectionSteelModel::headerData ( int section, Qt::Orientation orientation, int role ) const{
    if ( role == Qt::DisplayRole){
        if( orientation == Qt::Horizontal){
            switch( section ){
            case(0) :
                return QVariant( trUtf8("id") );
            case(1) :
                return QVariant( trUtf8("nome") );
            case(2) :
                return QVariant( trUtf8("A") + " [" + m_unitMeasure->string(UnitMeasure::sectL2) + "]" );
            case(3) :
                return QVariant( trUtf8("Iy") + " [" + m_unitMeasure->string(UnitMeasure::sectL4) + "]" );
            case(4) :
                return QVariant( trUtf8("iy") + " [" + m_unitMeasure->string(UnitMeasure::sectL) + "]" );
            case(5) :
                return QVariant( trUtf8("WEly") + " [" + m_unitMeasure->string(UnitMeasure::sectL3) + "]" );
            case(6) :
                return QVariant( trUtf8("WPly") + " [" + m_unitMeasure->string(UnitMeasure::sectL3) + "]" );
            case(7) :
                return QVariant( trUtf8("Iz") + " [" + m_unitMeasure->string(UnitMeasure::sectL4) + "]" );
            case(8) :
                return QVariant( trUtf8("iz") + " [" + m_unitMeasure->string(UnitMeasure::sectL) + "]" );
            case(9) :
                return QVariant( trUtf8("WElz") + " [" + m_unitMeasure->string(UnitMeasure::sectL3) + "]" );
            case(10) :
                return QVariant( trUtf8("WPlz") + " [" + m_unitMeasure->string(UnitMeasure::sectL3) + "]" );
            }
        } else if( orientation == Qt::Vertical){
            return QVariant(section+1);
        }
    }
    return QVariant();
}

int StandardSectionSteelModel::id( int index ){
    return m_dataContainer->at(index).id;
}

QVariant StandardSectionSteelModel::data( const QModelIndex & index, int role ) const{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= m_dataContainer->size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch( index.column() ) {
        case(0) :
            return QVariant( m_dataContainer->at(index.row()).id );
        case(1) :
            return QVariant( m_dataContainer->at(index.row()).name );
        case(2) :
            return QVariant( m_dataContainer->at(index.row()).A );
        case(3) :
            return QVariant( m_dataContainer->at(index.row()).Iy );
        case(4) :
            return QVariant( m_dataContainer->at(index.row()).iy );
        case(5) :
            return QVariant( m_dataContainer->at(index.row()).Wy );
        case(6) :
            return QVariant( m_dataContainer->at(index.row()).WPly );
        case(7) :
            return QVariant( m_dataContainer->at(index.row()).Iz );
        case(8) :
            return QVariant( m_dataContainer->at(index.row()).iz );
        case(9) :
            return QVariant( m_dataContainer->at(index.row()).Wz );
        case(10) :
            return QVariant( m_dataContainer->at(index.row()).WPlz );
        }
    }
    return QVariant();
}

Qt::ItemFlags StandardSectionSteelModel::flags ( const QModelIndex & index ) const{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void StandardSectionSteelModel::setQueryModel( const QSqlQueryModel & queryModel ){
    beginResetModel();

    m_dataContainer->clear();
    for( int i=0; i < queryModel.rowCount(); i++ ){
        SectionData data;

        data.id = queryModel.record(i).value("id").toInt();
        data.name = queryModel.record(i).value("name").toString();

        int type1 = queryModel.record(i).value("type1").toInt();
        switch( type1 ){
        case 1:{
            // H
            SectionSteel sect( m_unitMeasure, NULL, SectionSteel::sectionRolled, SectionSteel::sectionI, "" );
            for( int j=0; j < sect.np(); j++ ){
                if( sect.p(j) ){
                    double val = queryModel.record(i).value("p"+QString::number(j)).toDouble() * 1.0e-3;
                    sect.p(j)->setValueNormal( val );
                }
            }

            data.A = sect.A->value();
            data.Iy = sect.Iyy->value();
            data.iy = sect.iy->value();
            data.Wy = sect.WElMy->value();
            data.WPly = sect.WPlMy->value();

            data.Iz = sect.Izz->value();
            data.iz = sect.iz->value();
            data.Wz = sect.WElMz->value();
            data.WPlz = sect.WPlMz->value();

            break;}
        default:
            break;
        }

        m_dataContainer->append( data );
    }

    endResetModel();
}

class SectionSteelGUIPrivate{
public:
    SectionSteelGUIPrivate(UnitMeasure * ump, SectionSteelGUI * parent ){
        section = NULL;

        standardSectionModel = new StandardSectionSteelModel(ump, parent );
        ui = new Ui::SectionSteelGUI;
        pLabel = new QList<QLabelRichName *>;
        pUMLabel = new QList<QLabelUnitMeasure *>;
        pLEdit = new QList<QLineEditValue *>;
        pCheckBox = new QList<QCheckBoxReadOnly *>;

        dbSectionSteelName = "sectionSteelDB";
    }
    QString settingsFile;
    QString dbSectionSteelFileName;
    QString dbSectionSteelName;

    SectionSteel * section;
    StandardSectionSteelModel * standardSectionModel;
    SectionSteelDataSetModel * dataSetModel;

    Ui::SectionSteelGUI *ui;

    QGridLayout * pLayout;
    QList<QLabelRichName *> * pLabel;
    QList<QLabelUnitMeasure *> * pUMLabel;
    QList<QLineEditValue *> * pLEdit;
    QList<QCheckBoxReadOnly *> * pCheckBox;

    /* verifica */

    DoublePlus * beamL0y;
    DoublePlus * beamL0z;
    DoublePlus * beamL0LT;
    SectionSteelTorsionalDeformability * torsDeformability;

    DoublePlus * beamlY;
    DoublePlus * beamNCrY;
    DoublePlus * beamlRelY;
    DoublePlus * beamChiY;
    DoublePlus * beamNULSBucklY;

    DoublePlus * beamlZ;
    DoublePlus * beamNCrZ;
    DoublePlus * beamlRelZ;
    DoublePlus * beamChiZ;
    DoublePlus * beamNULSBucklZ;

    DoublePlus * NEd;

    DoublePlus * MEdY;
    DoublePlus * MEdZ;
    Point2DPlus * cenMEd;

    SectionSteelMomentDistribution * beamMSdYDistribution;
    DoublePlus * beamMEdYH1;
    DoublePlus * beamMEdYS;
    DoublePlus * beamMEdYH2;
    SectionSteelMomentDistribution * beamMEdZDistribution;
    DoublePlus * beamMEdZH1;
    DoublePlus * beamMEdZS;
    DoublePlus * beamMEdZH2;
    SectionSteelMomentDistribution * beamMEdLTDistribution;
    DoublePlus * beamMEdLTH1;
    DoublePlus * beamMEdLTS;
    DoublePlus * beamMEdLTH2;

    DoublePlus * beamMCr;
    DoublePlus * beamlRelLT;
    DoublePlus * beamChiLT;

    DoublePlus * VSdZ;
    DoublePlus * VSdY;

    DoublePlus * NMULSN;
    DoublePlus * alphaMULSN;
    Point2DPlus * cenMULSN;
    DoublePlus * MyULSN;
    DoublePlus * MzULSN;

    DoublePlus * resultSd;

    /* debug - begin */
    DoublePlus * lPlast;
    DoublePlus * myPlast;
    DoublePlus * mzPlast;
    DoublePlus * NPlast;
    DoublePlus * MyPlast;
    DoublePlus * MzPlast;
    DoublePlus * lElastPlast;
    DoublePlus * myElastPlast;
    DoublePlus * mzElastPlast;
    DoublePlus * NElastPlast;
    DoublePlus * MyElastPlast;
    DoublePlus * MzElastPlast;
    /* debug - end  */

    QList<QGraphicsPolygonItem *> * sectsToView;
};

SectionSteelGUI::SectionSteelGUI(UnitMeasure * um, MaterialModel * matModel, const QString & settingsFile, QWidget *parent) :
    SectionGUI(um, matModel, parent),
    m_d( new SectionSteelGUIPrivate(um, this) ) {

    m_d->ui->setupUi(this);
    m_d->ui->filterStandardSectionGBox->setHidden( true );
    m_d->pLayout = new QGridLayout();
    m_d->ui->standardSectionTableView->setModel( m_d->standardSectionModel );
    m_d->ui->standardSectionTableView->hideColumn(0);

    m_d->ui->parametersGroupBox->setLayout( m_d->pLayout );
    // aggiunge label e LineEdit dei parametri
    for( int i=0; i < SectionSteel::npMax(); i++ ){
        m_d->pLabel->append( new QLabelRichName(m_d->ui->parametersGroupBox) );
        m_d->pLayout->addWidget( m_d->pLabel->at(i), i, 0 );
        m_d->pUMLabel->append( new QLabelUnitMeasure(m_d->ui->parametersGroupBox) );
        m_d->pLayout->addWidget( m_d->pUMLabel->at(i), i, 1 );
        m_d->pLEdit->append( new QLineEditValue(m_d->ui->parametersGroupBox) );
        m_d->pLayout->addWidget( m_d->pLEdit->at(i), i, 2 );
        m_d->pCheckBox->append( new QCheckBoxReadOnly(m_d->ui->parametersGroupBox) );
        m_d->pLayout->addWidget( m_d->pCheckBox->at(i), i, 3 );
    }


    initVar();
    m_d->dataSetModel = new SectionSteelDataSetModel( um, m_d->beamlRelY, m_d->beamChiY, m_d->beamlRelZ, m_d->beamChiZ, m_d->torsDeformability, m_d->beamL0LT );
    m_d->ui->dataSetTableView->setModel( m_d->dataSetModel );
    connect( m_d->ui->dataSetClearPButton, SIGNAL(clicked()), this, SLOT(dataSetClear()));
    connect( m_d->ui->dataSetLoadFromFilePButton, SIGNAL(clicked()), this, SLOT(dataSetLoadFromFile()));
    connect( m_d->ui->dataSetVerifyULSAllPButton, SIGNAL(clicked()), this, SLOT(dataSetVerifyULSAll()));
    connect( m_d->ui->dataSetFromFormToTablePButton, SIGNAL(clicked()), this, SLOT(dataSetFromGUIToTable()));
    connect( m_d->ui->dataSetFromTableToFormPButton, SIGNAL(clicked()), this, SLOT(dataSetFromTableToGUI()));

    m_d->sectsToView = new QList<QGraphicsPolygonItem *>;

    // Apre il database
    m_d->settingsFile = settingsFile;
    if( !QSqlDatabase::contains( m_d->dbSectionSteelName )){
        QSqlDatabase dbSectionSteel = QSqlDatabase::addDatabase("QSQLITE", m_d->dbSectionSteelName );
        QSettings settings( m_d->settingsFile, QSettings::IniFormat );
        m_d->dbSectionSteelFileName = settings.value("dbSectionSteelFileName", QApplication::applicationDirPath() + "/../resources/sectionsteel.db").toString();
        dbSectionSteel.setDatabaseName( m_d->dbSectionSteelFileName );
        if (!dbSectionSteel.open() ) {
            QMessageBox::critical(0, trUtf8("Non riesco ad aprire il database delle sezioni in acciaio"),
                                  trUtf8("Non riesco a trovare il file sectionsteel.db"), QMessageBox::Cancel);
        } else {
            loadStandardComboBox();
        }
    } else {
        loadStandardComboBox();
    }

    m_d->ui->MULSyMULSzAccuracySBox->setMinimum( 4 );
    m_d->ui->MULSyMULSzAccuracySBox->setValue( 8 );
    connect( m_d->ui->MULSyMULSzPButton, SIGNAL(clicked()), this, SLOT(plotMULSyMULSz()));
    m_d->ui->MULSNAccuracySBox->setMinimum( 2 );
    m_d->ui->MULSNAccuracySBox->setValue( 8 );
    connect( m_d->ui->MULSNPButton, SIGNAL(clicked()), this, SLOT(plotMULSN()));

    connect( m_d->ui->classDIagramPButton, SIGNAL(clicked()), this, SLOT(plotClassDiagram()));

    m_materialComboBox = m_d->ui->steelComboBox;
    connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    refreshMaterialComboBox();

    setEnabled( false );
}

SectionSteelGUI::~SectionSteelGUI() {
    delete m_d->ui;

    QSettings settings(m_d->settingsFile,  QSettings::IniFormat);
    settings.setValue(m_d->dbSectionSteelName, m_d->dbSectionSteelFileName );
}

void SectionSteelGUI::initVar(){
    m_d->beamL0y = new DoublePlus(0.0, "beamL0y", m_unitMeasure, UnitMeasure::length, false );
    m_d->beamL0y->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">0,y</span>"));
    m_d->beamL0y->setToolTip( trUtf8("Distanza tra ritegni flessionali; inflessione nel piano y-y"));
    addVarToContainer( m_d->beamL0y );
    VarPlusGUI::connectVar( m_d->beamL0y, m_d->ui->beamL0yLabel, m_d->ui->beamL0yLEdit, m_d->ui->beamL0yUMLabel);

    m_d->beamL0z = new DoublePlus(0.0, "beamL0z", m_unitMeasure, UnitMeasure::length, false );
    m_d->beamL0z->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">0,z</span>"));
    m_d->beamL0z->setToolTip( trUtf8("Distanza tra ritegni flessionali; inflessione nel piano z-z"));
    addVarToContainer( m_d->beamL0z );
    VarPlusGUI::connectVar( m_d->beamL0z, m_d->ui->beamL0zLabel, m_d->ui->beamL0zLEdit, m_d->ui->beamL0zUMLabel);

    m_d->torsDeformability = new SectionSteelTorsionalDeformability( SectionSteel::torsDeformable, "torsionalDeformability" );
    m_d->torsDeformability->setRichName( trUtf8("Deformabilità   torsionale"));
    addVarToContainer(m_d->torsDeformability);
    VarPlusGUI::connectEnumVar( m_d->torsDeformability, m_d->ui->beamMxDeformabilityLabel, m_d->ui->beamMxDeformabilityComboBox );

    m_d->beamL0LT = new DoublePlus(0.0, "beamL0lt", m_unitMeasure, UnitMeasure::length, false );
    m_d->beamL0LT->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">0,LT</span>"));
    m_d->beamL0LT->setToolTip( trUtf8("Distanza tra ritegni torsionali"));
    addVarToContainer( m_d->beamL0LT );
    VarPlusGUI::connectVar( m_d->beamL0LT, m_d->ui->beamL0LTLabel, m_d->ui->beamL0LTLEdit, m_d->ui->beamL0LTUMLabel );

    m_d->beamlY = new DoublePlus( 0.0, "beamlY", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->beamlY->setRichName( trUtf8("λ<span style=\" vertical-align:sub;\">y</span>"));
    m_d->beamlY->setToolTip( trUtf8("Snellezza della trave per instabilità  nel piano y-y"));
    addVarToContainer( m_d->beamlY );
    VarPlusGUI::connectVar( m_d->beamlY, m_d->ui->beamlYLabel, m_d->ui->beamlYLEdit, m_d->ui->beamlYUMLabel, m_d->ui->beamlYCheckBox );
    connect( m_d->beamlY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamlY()));
    connect( m_d->beamL0y, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlY()));

    m_d->beamNCrY = new DoublePlus( 0.0, "beamNCrY", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->beamNCrY->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">cr,y</span>"));
    m_d->beamNCrY->setToolTip( trUtf8("Carico critico euleriano; inflessione in y-y"));
    addVarToContainer( m_d->beamNCrY );
    VarPlusGUI::connectVar( m_d->beamNCrY, m_d->ui->beamNCrYLabel, m_d->ui->beamNCrYLEdit, m_d->ui->beamNCrYUMLabel, m_d->ui->beamNCrYCheckBox );
    connect( m_d->beamNCrY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamNCrY()));
    connect( m_d->beamL0y, SIGNAL(valueChanged(QString)), this, SLOT(setBeamNCrY()));

    m_d->beamlRelY = new DoublePlus( 0.0, "beamlRelY", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->beamlRelY->setRichName( trUtf8("<span style=\" text-decoration: overline;\">λ</span><span style=\" vertical-align:sub;\">y</span>"));
    m_d->beamlRelY->setToolTip( trUtf8("Snellezza relativa della trave per instabilità  nel piano y-y"));
    addVarToContainer( m_d->beamlRelY );
    VarPlusGUI::connectVar( m_d->beamlRelY, m_d->ui->beamlRelYLabel, m_d->ui->beamlRelYLEdit, m_d->ui->beamlRelYUMLabel, m_d->ui->beamlRelYCheckBox );
    connect( m_d->beamlRelY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamlRelY()));
    connect( m_d->beamNCrY, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlRelY()));

    m_d->beamChiY = new DoublePlus( 0.0, "beamChiY", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->beamChiY->setRichName( trUtf8("χ<span style=\" vertical-align:sub;\">y</span>"));
    m_d->beamChiY->setToolTip( trUtf8("Fattore di riduzione per instabilità  compressione, asse y-y"));
    addVarToContainer( m_d->beamChiY );
    VarPlusGUI::connectVar( m_d->beamChiY, m_d->ui->beamChiYLabel, m_d->ui->beamChiYLEdit, m_d->ui->beamChiYUMLabel, m_d->ui->beamChiYCheckBox );
    connect( m_d->beamChiY, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamChiY()));
    connect( m_d->beamlRelY, SIGNAL(valueChanged(QString)), this, SLOT(setBeamChiY()));

    m_d->beamNULSBucklY = new DoublePlus( 0.0, "beamNULSBucklY", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->beamNULSBucklY->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">b,Rd,y</span>"));
    m_d->beamNULSBucklY->setToolTip( trUtf8("Resistenza di progetto della trave per instabilità  a compressione; inflessione attorno ad asse z-z"));
    addVarToContainer( m_d->beamNULSBucklY );
    VarPlusGUI::connectVar( m_d->beamNULSBucklY, m_d->ui->beamNULSBucklYLabel, m_d->ui->beamNULSBucklYLEdit, m_d->ui->beamNULSBucklYUMLabel, m_d->ui->beamNULSBucklYCheckBox );
    connect( m_d->beamChiY, SIGNAL(valueChanged(QString)), this, SLOT(setBeamNULSBucklY()));

    m_d->beamlZ = new DoublePlus( 0.0, "beamlZ", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->beamlZ->setRichName( trUtf8("λ<span style=\" vertical-align:sub;\">z</span>"));
    m_d->beamlZ->setToolTip( trUtf8("Snellezza della trave per instabilità  nel piano z-z"));
    addVarToContainer( m_d->beamlZ );
    VarPlusGUI::connectVar( m_d->beamlZ, m_d->ui->beamlZLabel, m_d->ui->beamlZLEdit, m_d->ui->beamlZUMLabel,  m_d->ui->beamlZCheckBox );
    connect( m_d->beamlZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamlZ()));
    connect( m_d->beamL0z, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlZ()));

    m_d->beamNCrZ = new DoublePlus( 0.0, "beamNCrZ", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->beamNCrZ->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">cr,z</span>"));
    m_d->beamNCrZ->setToolTip( trUtf8("Carico critico euleriano della trave; inflessione in z-z"));
    addVarToContainer( m_d->beamNCrZ );
    VarPlusGUI::connectVar( m_d->beamNCrZ, m_d->ui->beamNCrZLabel, m_d->ui->beamNCrZLEdit, m_d->ui->beamNCrZUMLabel, m_d->ui->beamNCrZCheckBox );
    connect( m_d->beamNCrZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamNCrZ()));
    connect( m_d->beamL0z, SIGNAL(valueChanged(QString)), this, SLOT(setBeamNCrZ()));

    m_d->beamlRelZ = new DoublePlus( 0.0, "beamlRelZ", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->beamlRelZ->setRichName( trUtf8("<span style=\" text-decoration: overline;\">λ</span><span style=\" vertical-align:sub;\">y</span>"));
    m_d->beamlRelZ->setToolTip( trUtf8("Snellezza relativa della trave per instabilità  nel piano y-y"));
    addVarToContainer( m_d->beamlRelZ );
    VarPlusGUI::connectVar( m_d->beamlRelZ, m_d->ui->beamlRelZLabel, m_d->ui->beamlRelZLEdit, m_d->ui->beamlRelZUMLabel, m_d->ui->beamlRelZCheckBox );
    connect( m_d->beamlRelZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamlRelZ()));
    connect( m_d->beamNCrZ, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlRelZ()));

    m_d->beamChiZ = new DoublePlus( 0.0, "beamChiZ", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->beamChiZ->setRichName( trUtf8("χ<span style=\" vertical-align:sub;\">z</span>"));
    m_d->beamChiZ->setToolTip( trUtf8("Fattore di riduzione per instabilità a compressione, asse z-z"));
    addVarToContainer( m_d->beamChiZ );
    VarPlusGUI::connectVar( m_d->beamChiZ, m_d->ui->beamChiZLabel, m_d->ui->beamChiZLEdit, m_d->ui->beamChiZUMLabel, m_d->ui->beamChiZCheckBox );
    connect( m_d->beamChiZ, SIGNAL(readOnlyChanged(bool)), this, SLOT(setBeamChiZ()));
    connect( m_d->beamlRelZ, SIGNAL(valueChanged(QString)), this, SLOT(setBeamChiZ()));

    m_d->beamNULSBucklZ = new DoublePlus( 0.0, "beamNULSBucklZ", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->beamNULSBucklZ->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">b,Rd,z</span>"));
    m_d->beamNULSBucklZ->setToolTip( trUtf8("Resistenza di progetto della trave per instabilità a compressione; inflessione attorno ad asse z-z"));
    addVarToContainer( m_d->beamNULSBucklZ );
    VarPlusGUI::connectVar( m_d->beamNULSBucklZ, m_d->ui->beamNULSBucklZLabel, m_d->ui->beamNULSBucklZLEdit, m_d->ui->beamNULSBucklZUMLabel, m_d->ui->beamNULSBucklZCheckBox );
    connect( m_d->beamChiZ, SIGNAL(valueChanged(QString)), this, SLOT(setBeamNULSBucklZ()));

    m_d->NEd = new DoublePlus(0.0, "NSd", m_unitMeasure, UnitMeasure::loadF, false );
    m_d->NEd->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">Sd</span>"));
    addVarToContainer( m_d->NEd );
    VarPlusGUI::connectVar( m_d->NEd, m_d->ui->NSdLabel, m_d->ui->NSdLEdit, m_d->ui->NSdUMLabel );

    m_d->MEdY = new DoublePlus(0.0, "MSdY", m_unitMeasure, UnitMeasure::loadM  );
    m_d->MEdY->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">Sd,y</span>"));
    addVarToContainer( m_d->MEdY );
    VarPlusGUI::connectVar(m_d->MEdY, m_d->ui->MSdYLabel, m_d->ui->MSdYLEdit, m_d->ui->MSdYUMLabel );

    m_d->MEdZ = new DoublePlus(0.0, "MSdZ", m_unitMeasure, UnitMeasure::loadM  );
    m_d->MEdZ->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">Sd,z</span>"));
    addVarToContainer( m_d->MEdZ );
    VarPlusGUI::connectVar(m_d->MEdZ, m_d->ui->MSdZLabel, m_d->ui->MSdZLEdit, m_d->ui->MSdZUMLabel );

    m_d->cenMEd = new Point2DPlus( "cenMSd", "yMSd", 0.0, "zMSd", 0.0, m_unitMeasure, UnitMeasure::sectL );
    m_d->cenMEd->y->setRichName( trUtf8("y<span style=\" vertical-align:sub;\">M</span>"));
    m_d->cenMEd->z->setRichName( trUtf8("z<span style=\" vertical-align:sub;\">M</span>"));
    addVarToContainer( m_d->cenMEd );
    m_d->cenMEd->y->setReadOnly( true );
    VarPlusGUI::connectVar(m_d->cenMEd->y, m_d->ui->yMSdLabel, m_d->ui->yMSdLEdit, m_d->ui->yMSdUMLabel, m_d->ui->yMSdCheckBox );
    m_d->cenMEd->z->setReadOnly( true );
    VarPlusGUI::connectVar(m_d->cenMEd->z, m_d->ui->zMSdLabel, m_d->ui->zMSdLEdit, m_d->ui->zMSdUMLabel, m_d->ui->zMSdCheckBox );

    m_d->beamMSdYDistribution = new SectionSteelMomentDistribution( SectionSteel::momentLinear, "beamMSdYDistribution"  );
    m_d->beamMSdYDistribution->setRichName( trUtf8("Distribuzione momento y-y"));
    m_d->beamMSdYDistribution->setToolTip( trUtf8("Distribuzione del momento nella trave, asse y-y"));
    addVarToContainer( m_d->beamMSdYDistribution );
    VarPlusGUI::connectEnumVar( m_d->beamMSdYDistribution, m_d->ui->beamMSdYDistributionLabel, m_d->ui->beamMSdYDistributionComboBox );

    m_d->beamMEdYH1 = new DoublePlus( 0.0, "beamMSdYH1", m_unitMeasure, UnitMeasure::loadM );
    m_d->beamMEdYH1->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">H,1,y</span>"));
    m_d->beamMEdYH1->setToolTip( trUtf8("Momento nel primo estremo della trave, y-y"));
    addVarToContainer( m_d->beamMEdYH1 );
    VarPlusGUI::connectVar( m_d->beamMEdYH1, m_d->ui->beamMSdYH1Label, m_d->ui->beamMSdYH1LEdit, m_d->ui->beamMSdYH1UMLabel );

    m_d->beamMEdYS = new DoublePlus( 0.0, "beamMSdYS", m_unitMeasure, UnitMeasure::loadM );
    m_d->beamMEdYS->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">S,y</span>"));
    m_d->beamMEdYS->setToolTip( trUtf8("Momento massimo interno della trave, y-y"));
    addVarToContainer( m_d->beamMEdYS );
    VarPlusGUI::connectVar( m_d->beamMEdYS, m_d->ui->beamMSdYSLabel, m_d->ui->beamMSdYSLEdit, m_d->ui->beamMSdYSUMLabel );

    m_d->beamMEdYH2 = new DoublePlus( 0.0, "beamMSdYH2", m_unitMeasure, UnitMeasure::loadM );
    m_d->beamMEdYH2->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">H,2,y</span>"));
    m_d->beamMEdYH2->setToolTip( trUtf8("Momento nel secondo estremo della trave, y-y"));
    addVarToContainer( m_d->beamMEdYH2 );
    VarPlusGUI::connectVar( m_d->beamMEdYH2, m_d->ui->beamMSdYH2Label, m_d->ui->beamMSdYH2LEdit, m_d->ui->beamMSdYH2UMLabel );

    m_d->beamMEdZDistribution = new SectionSteelMomentDistribution( SectionSteel::momentLinear, "beamMSdZDistribution"  );
    m_d->beamMEdZDistribution->setRichName( trUtf8("Distribuzione momento z-z"));
    m_d->beamMEdZDistribution->setToolTip( trUtf8("Distribuzione del momento nella trave, asse z-z"));
    addVarToContainer( m_d->beamMEdZDistribution );
    VarPlusGUI::connectEnumVar( m_d->beamMEdZDistribution, m_d->ui->beamMSdZDistributionLabel, m_d->ui->beamMSdZDistributionComboBox );

    m_d->beamMEdZH1 = new DoublePlus( 0.0, "beamMSdZH1", m_unitMeasure, UnitMeasure::loadM );
    m_d->beamMEdZH1->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">H,1,z</span>"));
    m_d->beamMEdZH1->setToolTip( trUtf8("Momento nel primo estremo della trave, z-z"));
    addVarToContainer( m_d->beamMEdZH1 );
    VarPlusGUI::connectVar( m_d->beamMEdZH1, m_d->ui->beamMSdZH1Label, m_d->ui->beamMSdZH1LEdit, m_d->ui->beamMSdZH1UMLabel );

    m_d->beamMEdZS = new DoublePlus( 0.0, "beamMSdZS", m_unitMeasure, UnitMeasure::loadM );
    m_d->beamMEdZS->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">S,z</span>"));
    m_d->beamMEdZS->setToolTip( trUtf8("Momento massimo interno della trave, z-z"));
    addVarToContainer( m_d->beamMEdZS );
    VarPlusGUI::connectVar( m_d->beamMEdZS, m_d->ui->beamMSdZSLabel, m_d->ui->beamMSdZSLEdit, m_d->ui->beamMSdZSUMLabel );

    m_d->beamMEdZH2 = new DoublePlus( 0.0, "beamMSdYH2", m_unitMeasure, UnitMeasure::loadM );
    m_d->beamMEdZH2->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">H,2,y</span>"));
    m_d->beamMEdZH2->setToolTip( trUtf8("Momento nel secondo estremo della trave, y-y"));
    addVarToContainer( m_d->beamMEdZH2 );
    VarPlusGUI::connectVar( m_d->beamMEdZH2, m_d->ui->beamMSdZH2Label, m_d->ui->beamMSdZH2LEdit, m_d->ui->beamMSdZH2UMLabel );

    m_d->beamMEdLTDistribution = new SectionSteelMomentDistribution( SectionSteel::momentLinear, "beamMSdLTDistribution"  );
    m_d->beamMEdLTDistribution->setRichName( trUtf8("Distribuzione momento y-y"));
    m_d->beamMEdLTDistribution->setToolTip( trUtf8("Distribuzione del momento nella trave, asse y-y"));
    addVarToContainer( m_d->beamMEdLTDistribution );
    VarPlusGUI::connectEnumVar( m_d->beamMEdLTDistribution, m_d->ui->beamMSdLTDistributionLabel, m_d->ui->beamMSdLTDistributionComboBox );

    m_d->beamMEdLTH1 = new DoublePlus( 0.0, "beamMSdLTH1", m_unitMeasure, UnitMeasure::loadM );
    m_d->beamMEdLTH1->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">H,1,y</span>"));
    m_d->beamMEdLTH1->setToolTip( trUtf8("Momento nel primo estremo della trave, y-y"));
    addVarToContainer( m_d->beamMEdLTH1 );
    VarPlusGUI::connectVar( m_d->beamMEdLTH1, m_d->ui->beamMSdLTH1Label, m_d->ui->beamMSdLTH1LEdit, m_d->ui->beamMSdLTH1UMLabel );

    m_d->beamMEdLTS = new DoublePlus( 0.0, "beamMSdLTS", m_unitMeasure, UnitMeasure::loadM );
    m_d->beamMEdLTS->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">S,y</span>"));
    m_d->beamMEdLTS->setToolTip( trUtf8("Momento massimo interno della trave, y-y"));
    addVarToContainer( m_d->beamMEdLTS );
    VarPlusGUI::connectVar( m_d->beamMEdLTS, m_d->ui->beamMSdLTSLabel, m_d->ui->beamMSdLTSLEdit, m_d->ui->beamMSdLTSUMLabel );

    m_d->beamMEdLTH2 = new DoublePlus( 0.0, "beamMSdLTH2", m_unitMeasure, UnitMeasure::loadM );
    m_d->beamMEdLTH2->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">H,2,y</span>"));
    m_d->beamMEdLTH2->setToolTip( trUtf8("Momento nel secondo estremo della trave, y-y"));
    addVarToContainer( m_d->beamMEdLTH2 );
    VarPlusGUI::connectVar( m_d->beamMEdLTH2, m_d->ui->beamMSdLTH2Label, m_d->ui->beamMSdLTH2LEdit, m_d->ui->beamMSdLTH2UMLabel );

    m_d->beamMCr = new DoublePlus(0.0, "beamMCr", m_unitMeasure, UnitMeasure::loadM, true );
    m_d->beamMCr->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">cr</span>"));
    m_d->beamMCr->setToolTip( trUtf8("Momento critico per instabilità flessotorsionale"));
    addVarToContainer( m_d->beamMCr );
    VarPlusGUI::connectVar( m_d->beamMCr, m_d->ui->beamMCrLTLabel, m_d->ui->beamMCrLTLEdit, m_d->ui->beamMCrLTUMLabel, m_d->ui->beamMCrLTCheckBox );
    connect( m_d->beamL0LT, SIGNAL(valueChanged(QString)), this, SLOT(setBeamMCr()));
    connect( m_d->beamMEdLTH1, SIGNAL(valueChanged(QString)), this, SLOT(setBeamMCr()));
    connect( m_d->beamMEdLTH2, SIGNAL(valueChanged(QString)), this, SLOT(setBeamMCr()));

    m_d->beamlRelLT = new DoublePlus( 0.0, "beamlRelLT", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->beamlRelLT->setRichName( trUtf8("<span style=\" text-decoration: overline;\">λ</span><span style=\" vertical-align:sub;\">LT</span>"));
    m_d->beamlRelLT->setToolTip( trUtf8("Snellezza relativa per instabilità flessotorsionale"));
    addVarToContainer( m_d->beamlRelLT );
    VarPlusGUI::connectVar( m_d->beamlRelLT, m_d->ui->beamlRelLTLabel, m_d->ui->beamlRelLTLEdit, m_d->ui->beamlRelLTUMLabel, m_d->ui->beamlRelLTCheckBox );
    connect( m_d->beamMCr, SIGNAL(valueChanged(QString)), this, SLOT(setBeamlRelLT()));

    m_d->beamChiLT = new DoublePlus( 1.0, "beamChiLT", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->beamChiLT->setRichName( trUtf8("χ<span style=\" vertical-align:sub;\">LT</span>"));
    m_d->beamChiLT->setToolTip( trUtf8("Fattore di riduzione per instabilità flessotorsionale"));
    addVarToContainer( m_d->beamChiLT );
    VarPlusGUI::connectVar( m_d->beamChiLT, m_d->ui->beamChiLTLabel, m_d->ui->beamChiLTLEdit, m_d->ui->beamChiLTUMLabel, m_d->ui->beamChiLTCheckBox );
    connect( m_d->beamlRelLT, SIGNAL(valueChanged(QString)), this, SLOT(setBeamChiLT()));
    connect( m_d->beamMEdLTDistribution, SIGNAL(valueChanged(QString)), this, SLOT(setBeamChiLT()));
    connect( m_d->beamMEdLTH1, SIGNAL(valueChanged(QString)), this, SLOT(setBeamChiLT()));
    connect( m_d->beamMEdLTH2, SIGNAL(valueChanged(QString)), this, SLOT(setBeamChiLT()));

    m_d->VSdZ = new DoublePlus(0.0, "VSdZ", m_unitMeasure, UnitMeasure::loadF  );
    m_d->VSdZ->setRichName( trUtf8("V<span style=\" vertical-align:sub;\">Sd,z</span>"));
    addVarToContainer( m_d->VSdZ );
    VarPlusGUI::connectVar( m_d->VSdZ, m_d->ui->VSdZLabel, m_d->ui->VSdZLEdit, m_d->ui->VSdZUMLabel );

    m_d->VSdY = new DoublePlus(0.0, "VSdY", m_unitMeasure, UnitMeasure::loadF  );
    m_d->VSdY->setRichName( trUtf8("V<span style=\" vertical-align:sub;\">Sd,y</span>"));
    addVarToContainer( m_d->VSdY );
    VarPlusGUI::connectVar( m_d->VSdY, m_d->ui->VSdYLabel, m_d->ui->VSdYLEdit, m_d->ui->VSdYUMLabel );

    m_d->resultSd = new DoublePlus(0.0, "result", m_unitMeasure, UnitMeasure::noDimension  );
    m_d->resultSd->setRichName( trUtf8("Risultato"));
    addVarToContainer( m_d->resultSd );
    VarPlusGUI::connectVar( m_d->resultSd, m_d->ui->resultSdULSLabel, m_d->ui->resultSdULSLEdit );

    connect(m_d->ui->verifyPushButton, SIGNAL(clicked()), this, SLOT(verifyULS()));

    m_d->NMULSN = new DoublePlus(0.0, "NMULSN", m_unitMeasure, UnitMeasure::loadF  );
    m_d->NMULSN->setToolTip( trUtf8("Sforzo normale agente") );
    m_d->NMULSN->setRichName( trUtf8("N<span style=\"vertical-align:sub;\">Sd</span>"));
    addVarToContainer( m_d->NMULSN );
    VarPlusGUI::connectVar( m_d->NMULSN, m_d->ui->NMULSNLabel, m_d->ui->NMULSNLEdit, m_d->ui->NMULSNUMLabel );

    m_d->alphaMULSN = new DoublePlus(0.0, "alphaMULSN", m_unitMeasure, UnitMeasure::angle  );
    m_d->alphaMULSN->setToolTip( trUtf8("Inclinazione piano di deformazione rispetto al piano yz") );
    m_d->alphaMULSN->setRichName( trUtf8("àŽÂ±<span style=\" vertical-align:sub;\">Sd</span>"));
    addVarToContainer( m_d->alphaMULSN );
    VarPlusGUI::connectVar( m_d->alphaMULSN, m_d->ui->alphaMULSNLabel, m_d->ui->alphaMULSNLEdit, m_d->ui->alphaMULSNUMLabel );

    m_d->cenMULSN = new Point2DPlus( "cenMULSN", "xCenMULSN", 0.0, "yCenMULSN", 0.0, m_unitMeasure, UnitMeasure::sectL, true );
    addVarToContainer( m_d->cenMULSN );
    m_d->cenMULSN->y->setToolTip( trUtf8("Ascissa del polo attorno a cui si calcola il momento"));
    m_d->cenMULSN->y->setRichName( trUtf8("y<span style=\"vertical-align:sub;\">M</span>"));
    VarPlusGUI::connectVar( m_d->cenMULSN->y, m_d->ui->yCenMULSNLabel, m_d->ui->yCenMULSNLEdit, m_d->ui->yCenULSNUMLabel, m_d->ui->yCenMULSNCheckBox );
    m_d->cenMULSN->z->setToolTip( trUtf8("Ordinata del polo attorno a cui si calcola il momento"));
    m_d->cenMULSN->z->setRichName( trUtf8("z<span style=\"vertical-align:sub;\">M</span>"));
    VarPlusGUI::connectVar( m_d->cenMULSN->z, m_d->ui->zCenMULSNLabel, m_d->ui->zCenMULSNLEdit, m_d->ui->zCenMULSNUMLabel, m_d->ui->zCenMULSNCheckBox );
    connect( m_d->cenMULSN->y, SIGNAL(readOnlyChanged(bool)), this, SLOT(setYCenMULSN()));
    connect( m_d->cenMULSN->z, SIGNAL(readOnlyChanged(bool)), this, SLOT(setZCenMULSN()));

    m_d->MyULSN = new DoublePlus(0.0, "MyULSN", m_unitMeasure, UnitMeasure::loadM, true  );
    m_d->MyULSN->setToolTip( trUtf8("Componente y del momento resistente"));
    m_d->MyULSN->setRichName( trUtf8("M<span style=\"vertical-align:sub;\">Rd,y</span>"));
    addVarToContainer( m_d->MyULSN );
    VarPlusGUI::connectVar( m_d->MyULSN, m_d->ui->MyULSNLabel, m_d->ui->MyULSNLEdit, m_d->ui->MyULSNUMLabel );

    m_d->MzULSN = new DoublePlus(0.0, "MULSz", m_unitMeasure, UnitMeasure::loadM, true  );
    m_d->MzULSN->setToolTip( trUtf8("Componente z del momento resistente"));
    m_d->MzULSN->setRichName( trUtf8("M<span style=\"vertical-align:sub;\">Rd,z</span>"));
    addVarToContainer( m_d->MzULSN );
    VarPlusGUI::connectVar( m_d->MzULSN, m_d->ui->MzULSNLabel, m_d->ui->MzULSNLEdit, m_d->ui->MzULSNUMLabel );

    m_d->lPlast = new DoublePlus( 0.0, "lPlast", m_unitMeasure, UnitMeasure::noDimension );
    m_d->lPlast->setRichName( trUtf8("λ<span style=\" vertical-align:sub;\">p</span>") );
    addVarToContainer( m_d->lPlast );
    VarPlusGUI::connectVar( m_d->lPlast, m_d->ui->lPlastLabel, m_d->ui->lPlastLEdit, m_d->ui->lPlastUMLabel );

    m_d->myPlast = new DoublePlus( 0.0, "myPlast", m_unitMeasure, UnitMeasure::noDimension );
    m_d->myPlast->setRichName( trUtf8("μ<span style=\" vertical-align:sub;\">p,y</span>") );
    addVarToContainer( m_d->myPlast );
    VarPlusGUI::connectVar( m_d->myPlast, m_d->ui->myPlastLabel, m_d->ui->myPlastLEdit, m_d->ui->myPlastUMLabel );

    m_d->mzPlast = new DoublePlus( 0.0, "mzPlast", m_unitMeasure, UnitMeasure::noDimension );
    m_d->mzPlast->setRichName( trUtf8("μ<span style=\" vertical-align:sub;\">p,z</span>") );
    addVarToContainer( m_d->mzPlast );
    VarPlusGUI::connectVar( m_d->mzPlast, m_d->ui->mzPlastLabel, m_d->ui->mzPlastLEdit, m_d->ui->mzPlastUMLabel );

    m_d->NPlast = new DoublePlus( 0.0, "NPlast", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->NPlast->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">p</span>") );
    addVarToContainer( m_d->NPlast );
    VarPlusGUI::connectVar( m_d->NPlast, m_d->ui->NPlastLabel, m_d->ui->NPlastLEdit, m_d->ui->NPlastUMLabel );

    m_d->MyPlast = new DoublePlus( 0.0, "MyPlast", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->MyPlast->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">p,y</span>") );
    addVarToContainer( m_d->MyPlast );
    VarPlusGUI::connectVar( m_d->MyPlast, m_d->ui->MyPlastLabel, m_d->ui->MyPlastLEdit, m_d->ui->MyPlastUMLabel );

    m_d->MzPlast = new DoublePlus( 0.0, "MzPlast", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->MzPlast->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">p,z</span>") );
    addVarToContainer( m_d->MzPlast );
    VarPlusGUI::connectVar( m_d->MzPlast, m_d->ui->MzPlastLabel, m_d->ui->MzPlastLEdit, m_d->ui->MzPlastUMLabel );

    connect( m_d->ui->calculateNMyMzPlastPButton, SIGNAL(clicked()), this, SLOT(setNMyMzPlast()));

    m_d->lElastPlast = new DoublePlus( 0.0, "lElastPlast", m_unitMeasure, UnitMeasure::noDimension );
    m_d->lElastPlast->setRichName( trUtf8("λ<span style=\" vertical-align:sub;\">ep</span>") );
    addVarToContainer( m_d->lElastPlast );
    VarPlusGUI::connectVar( m_d->lElastPlast, m_d->ui->lElastPlastLabel, m_d->ui->lElastPlastLEdit, m_d->ui->lElastPlastUMLabel );

    m_d->myElastPlast = new DoublePlus( 0.0, "myElastPlast", m_unitMeasure, UnitMeasure::noDimension );
    m_d->myElastPlast->setRichName( trUtf8("μ<span style=\" vertical-align:sub;\">ep,y</span>") );
    addVarToContainer( m_d->myElastPlast );
    VarPlusGUI::connectVar( m_d->myElastPlast, m_d->ui->myElastPlastLabel, m_d->ui->myElastPlastLEdit, m_d->ui->myElastPlastUMLabel );

    m_d->mzElastPlast = new DoublePlus( 0.0, "mzElastPlast", m_unitMeasure, UnitMeasure::noDimension );
    m_d->mzElastPlast->setRichName( trUtf8("μ<span style=\" vertical-align:sub;\">ep,z</span>") );
    addVarToContainer( m_d->mzElastPlast );
    VarPlusGUI::connectVar( m_d->mzElastPlast, m_d->ui->mzElastPlastLabel, m_d->ui->mzElastPlastLEdit, m_d->ui->mzElastPlastUMLabel );

    m_d->NElastPlast = new DoublePlus( 0.0, "NElastPlast", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->NElastPlast->setRichName( trUtf8("N<span style=\" vertical-align:sub;\">ep</span>") );
    addVarToContainer( m_d->NElastPlast );
    VarPlusGUI::connectVar( m_d->NElastPlast, m_d->ui->NElastPlastLabel, m_d->ui->NElastPlastLEdit, m_d->ui->NElastPlastUMLabel );

    m_d->MyElastPlast = new DoublePlus( 0.0, "MyElastPlast", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->MyElastPlast->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">ep,y</span>") );
    addVarToContainer( m_d->MyElastPlast );
    VarPlusGUI::connectVar( m_d->MyElastPlast, m_d->ui->MyElastPlastLabel, m_d->ui->MyElastPlastLEdit, m_d->ui->MyElastPlastUMLabel );

    m_d->MzElastPlast = new DoublePlus( 0.0, "MzElastPlast", m_unitMeasure, UnitMeasure::loadF, true );
    m_d->MzElastPlast->setRichName( trUtf8("M<span style=\" vertical-align:sub;\">ep,z</span>") );
    addVarToContainer( m_d->MzElastPlast );
    VarPlusGUI::connectVar( m_d->MzElastPlast, m_d->ui->MzElastPlastLabel, m_d->ui->MzElastPlastLEdit, m_d->ui->MzElastPlastUMLabel );

    connect( m_d->ui->calculateNMyMzElastPlastPButton, SIGNAL(clicked()), this, SLOT(setNMyMzElastPlast()));
}

void SectionSteelGUI::loadStandardComboBox(){
    QSqlQueryModel queryModel;
    QString queryString = QString( "SELECT idType1, nameType1 from type1" );
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSectionSteelName ) );
    m_d->ui->type1FilterStandardComboBox->insertItem(0, QString("------"), QVariant(-1) );
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant data = queryModel.record(i).value("idType1");
        QString name = queryModel.record(i).value("nameType1").toString();
        m_d->ui->type1FilterStandardComboBox->insertItem((i+1), name, data );
    }

    connect( m_d->ui->type1FilterStandardComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateType2StandardComboBox()) );
    updateType2StandardComboBox();

    connect( m_d->ui->type1FilterStandardComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateType3StandardComboBox()) );
    connect( m_d->ui->type2FilterStandardComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateType3StandardComboBox()) );
    updateType3StandardComboBox();

    connect( m_d->ui->type1FilterStandardComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateStandardModel()) );
    connect( m_d->ui->type2FilterStandardComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateStandardModel()) );
    connect( m_d->ui->type3FilterStandardComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateStandardModel()) );
    updateStandardModel();

    connect( m_d->ui->loadStandardPButton, SIGNAL(clicked()), this, SLOT(loadStandardData()));
}

void SectionSteelGUI::updateType2StandardComboBox(){
    QSqlQueryModel queryModel;
    QString queryString = QString( "SELECT idType2, nameType2, type1 from type2" );

    m_d->ui->type2FilterStandardComboBox->clear();
    m_d->ui->type2FilterStandardComboBox->insertItem(0, QString("------"), QVariant(-1) );
    QVariant type1Data = m_d->ui->type1FilterStandardComboBox->itemData( m_d->ui->type1FilterStandardComboBox->currentIndex() );
    if(  type1Data != QVariant::Invalid ) {
        if( type1Data.toInt() > 0 ){
            queryString.append( QString(" WHERE type1=") + QString::number(type1Data.toInt()) );
        }
    }
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSectionSteelName ) );
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant data = queryModel.record(i).value("idType2");
        QString name = queryModel.record(i).value("nameType2").toString();
        m_d->ui->type2FilterStandardComboBox->insertItem((i+1), name, data );
    }
}

void SectionSteelGUI::updateType3StandardComboBox(){
    QSqlQueryModel queryModel;
    QString queryString = QString( "SELECT idType3, nameType3, type2 from type3" );

    QVariant type1Data = m_d->ui->type1FilterStandardComboBox->itemData( m_d->ui->type1FilterStandardComboBox->currentIndex() );
    QVariant type2Data = m_d->ui->type2FilterStandardComboBox->itemData( m_d->ui->type2FilterStandardComboBox->currentIndex() );

    if(  type2Data != QVariant::Invalid && type2Data.toInt() > 0 ){
        queryString.append( QString(" WHERE type2=") + QString::number(type2Data.toInt()) );
    } else if(  type1Data != QVariant::Invalid && type1Data.toInt() > -1 ){
        queryString.append( QString(" INNER JOIN type2 ON type2=idType2 WHERE type1=") + QString::number(type1Data.toInt()) );
    }

    m_d->ui->type3FilterStandardComboBox->clear();
    m_d->ui->type3FilterStandardComboBox->insertItem(0, QString("------"), QVariant(-1) );
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSectionSteelName ) );
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant data = queryModel.record(i).value("idType3");
        QString name = queryModel.record(i).value("nameType3").toString();
        m_d->ui->type3FilterStandardComboBox->insertItem((i+1), name, data );
    }
}

void SectionSteelGUI::updateStandardModel(){
    QSqlQueryModel queryModel;
    QString queryString = QString( "SELECT id, name, type3, type2, type1" );
    for( int i=0; i < SectionSteel::npMax(); i++ ){
        queryString.append( QString(" ,p") + QString::number(i) );
    }
    queryString.append(" FROM (section INNER JOIN type3 ON type3=idType3) INNER JOIN type2 ON type2=idType2");

    QVariant type1Data = m_d->ui->type1FilterStandardComboBox->itemData( m_d->ui->type1FilterStandardComboBox->currentIndex() );
    QVariant type2Data = m_d->ui->type2FilterStandardComboBox->itemData( m_d->ui->type2FilterStandardComboBox->currentIndex() );
    QVariant type3Data = m_d->ui->type3FilterStandardComboBox->itemData( m_d->ui->type3FilterStandardComboBox->currentIndex() );

    if( type3Data != QVariant::Invalid && type3Data.toInt() > 0 ){
        queryString.append( QString(" WHERE type3=") + QString::number(type3Data.toInt()) );
    } else if( type2Data != QVariant::Invalid && type2Data.toInt() > 0 ){
        queryString.append( QString(" WHERE type2=") + QString::number(type2Data.toInt()) );
    } else if(  type1Data != QVariant::Invalid && type1Data.toInt() > 0 ){
        queryString.append( QString(" WHERE type1=") + QString::number(type1Data.toInt()) );
    }

    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSectionSteelName ) );

    m_d->standardSectionModel->setQueryModel( queryModel );
}

void SectionSteelGUI::loadStandardData(){
    if( m_d->section != NULL ){
        int currRow = m_d->ui->standardSectionTableView->selectionModel()->currentIndex().row();
        if( currRow >= 0 && currRow < m_d->standardSectionModel->rowCount() ){
            QSqlQueryModel queryModel;
            QString queryString = QString( "SELECT id, name, type1, manufactType" );
            for( int i=0; i < SectionSteel::npMax(); i++ ){
                queryString.append( QString(" ,p") + QString::number(i) );
            }
            queryString.append(" FROM (section INNER JOIN type3 ON type3=idType3) INNER JOIN type2 ON type2=idType2 WHERE id=" + QString::number(m_d->standardSectionModel->id(currRow)));
            queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSectionSteelName ) );

            if( queryModel.rowCount() > 0 ){
                m_d->section->suspendSignals( true );
                m_d->section->name->setValue( queryModel.record(0).value("name").toString() );
                int type1 = queryModel.record(0).value("type1").toInt();
                switch(type1){
                case 1:
                    m_d->section->sectionShape->setValueNormal( SectionSteel::sectionI, true );
                    break;
                case 2:
                    m_d->section->sectionShape->setValueNormal( SectionSteel::sectionCirc, true );
                    break;
                case 3:
                    m_d->section->sectionShape->setValueNormal( SectionSteel::sectionRect, true );
                    break;
                default:
                    m_d->section->sectionShape->setValueNormal( SectionSteel::sectionI, true );
                    break;
                }
                int manufactType = queryModel.record(0).value("manufactType").toInt();
                switch(manufactType){
                case 1:
                    m_d->section->manufactType->setValueNormal( SectionSteel::sectionRolled, false );
                    break;
                case 2:
                    m_d->section->manufactType->setValueNormal( SectionSteel::sectionWelded, false );
                    break;
                default:
                    m_d->section->manufactType->setValueNormal( SectionSteel::sectionManufactTypeOther, false );
                    break;
                }
                for( int j=0; j < m_d->section->np(); j++ ){
                    if( m_d->section->p(j) ){
                        double val = queryModel.record(0).value("p"+QString::number(j)).toDouble() * 1.0e-3;
                        m_d->section->p(j)->setValueNormal( val, false );
                    }
                }
                m_d->section->suspendSignals(false);
            }
        }
    }
}

SectionSteel * SectionSteelGUI::sectionSteel(){
    return m_d->section;
}

void SectionSteelGUI::setSection(Section * s){
    if( m_d->section ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );
        VarPlusGUI::disconnectEnumVar( m_d->ui->sectionShapeLabel, m_d->ui->sectionShapeComboBox );
        VarPlusGUI::disconnectEnumVar( m_d->ui->manufactTypeLabel, m_d->ui->manufactTypeComboBox );
        // aggiunge label e LineEdit dei parametri
        for( int i=0; i < SectionSteel::npMax(); i++ ){
            VarPlusGUI::disconnectVar( m_d->pLabel->at(i), m_d->pLEdit->at(i), m_d->pUMLabel->at(i), m_d->pCheckBox->at(i) );
        }
        disconnect( m_d->section->sectionShape, &SectionSteelShape::valueChanged, this, &SectionSteelGUI::updateVisibleP );

        VarPlusGUI::disconnectVar( m_d->ui->ALabel, m_d->ui->ALEdit, m_d->ui->AUMLabel, m_d->ui->ACheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->AEffNLabel, m_d->ui->AEffNLEdit, m_d->ui->AEffNUMLabel, m_d->ui->AEffNCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->yGEffLabel, m_d->ui->yGEffLEdit, m_d->ui->yGEffUMLabel, m_d->ui->yGEffCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->zGEffLabel, m_d->ui->zGEffLEdit, m_d->ui->zGEffUMLabel, m_d->ui->zGEffCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->IyLabel, m_d->ui->IyLEdit, m_d->ui->IyUMLabel, m_d->ui->IyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->IzLabel, m_d->ui->IzLEdit, m_d->ui->IzUMLabel, m_d->ui->IzCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->WMyLabel, m_d->ui->WMyLEdit, m_d->ui->WMyUMLabel, m_d->ui->WMyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->WMzLabel, m_d->ui->WMzLEdit, m_d->ui->WMzUMLabel, m_d->ui->WMzCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->WPlMyLabel, m_d->ui->WPlMyLEdit, m_d->ui->WPlMyUMLabel, m_d->ui->WPlMyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->WPlMzLabel, m_d->ui->WPlMzLEdit, m_d->ui->WPlMzUMLabel, m_d->ui->WPlMzCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->WEffMyMaxLabel, m_d->ui->WEffMyMaxLEdit, m_d->ui->WEffMyMaxUMLabel, m_d->ui->WEffMyMaxCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->WEffMyMinLabel, m_d->ui->WEffMyMinLEdit, m_d->ui->WEffMyMinUMLabel, m_d->ui->WEffMyMinCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->WEffMzMaxLabel, m_d->ui->WEffMzMaxLEdit, m_d->ui->WEffMzMaxUMLabel, m_d->ui->WEffMzMaxCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->WEffMzMinLabel, m_d->ui->WEffMzMinLEdit, m_d->ui->WEffMzMinUMLabel, m_d->ui->WEffMzMinCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->iyLabel, m_d->ui->iyLEdit, m_d->ui->iyUMLabel, m_d->ui->iyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->izLabel, m_d->ui->izLEdit, m_d->ui->izUMLabel, m_d->ui->izCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->ITLabel, m_d->ui->ITLEdit, m_d->ui->ITUMLabel, m_d->ui->ITCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->IwLabel, m_d->ui->IwLEdit, m_d->ui->IwUMLabel, m_d->ui->IwCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->tyLabel, m_d->ui->tyLEdit, m_d->ui->tyUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->tzLabel, m_d->ui->tzLEdit, m_d->ui->tzUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->tyzLabel, m_d->ui->tyzLEdit, m_d->ui->tyzUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->NClassLabel, m_d->ui->NClassLEdit, NULL, m_d->ui->NClassCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->MyMaxClassLabel, m_d->ui->MyMaxClassLEdit, NULL, m_d->ui->MyMaxClassCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->MyMinClassLabel, m_d->ui->MyMinClassLEdit, NULL, m_d->ui->MyMinClassCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->MyClassLimitLabel, m_d->ui->MyClassLimitLEdit, m_d->ui->MyClassLimitUMLabel, m_d->ui->MyClassLimitCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->MzMaxClassLabel, m_d->ui->MzMaxClassLEdit, NULL, m_d->ui->MzMaxClassCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->MzMinClassLabel, m_d->ui->MzMinClassLEdit, NULL, m_d->ui->MzMinClassCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->MzClassLimitLabel, m_d->ui->MzClassLimitLEdit, m_d->ui->MzClassLimitUMLabel, m_d->ui->MzClassLimitCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->NULSPlusLabel, m_d->ui->NULSPlusLEdit, m_d->ui->NULSPlusUMLabel, m_d->ui->NULSPlusCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->NULSMinusLabel, m_d->ui->NULSMinusLEdit, m_d->ui->NULSMinusUMLabel, m_d->ui->NULSMinusCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->MULSyLabel, m_d->ui->MULSyLEdit, m_d->ui->MULSyUMLabel, m_d->ui->MULSyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->MULSzLabel, m_d->ui->MULSzLEdit, m_d->ui->MULSzUMLabel, m_d->ui->MULSzCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->AVyLabel, m_d->ui->AVyLEdit, m_d->ui->AVyUMLabel, m_d->ui->AVyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->VULSyLabel, m_d->ui->VULSyLEdit, m_d->ui->VULSyUMLabel, m_d->ui->VULSyCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->AVzLabel, m_d->ui->AVzLEdit, m_d->ui->AVzUMLabel, m_d->ui->AVzCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->VULSzLabel, m_d->ui->VULSzLEdit, m_d->ui->VULSzUMLabel, m_d->ui->VULSzCheckBox );

        clearSectsToView();

        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamlY()));
        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamNCrY()));
        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamlRelY()));
        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamChiY()));
        disconnect( m_d->section->NULSMin, SIGNAL(valueChanged(QString)), this, SLOT(setBeamNULSBucklY()));
        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamlZ()));
        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamNCrZ()));
        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamlRelZ()));
        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamNCrZ()));
        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamChiZ()));
        disconnect( m_d->section->NULSMin, SIGNAL(valueChanged(QString)), this, SLOT(setBeamNULSBucklZ()));
        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamChiLT()));
        disconnect( m_d->section->G->y, SIGNAL(valueChanged(QString)), this, SLOT(setYCenMSd()));
        disconnect( m_d->section->G->z, SIGNAL(valueChanged(QString)), this, SLOT(setZCenMSd()));
        disconnect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(clearSectsToView()));

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        m_materialComboBox->setCurrentIndex( -1 );
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );

        disconnect( m_d->section, &Section::aboutToBeDeleted, this, &SectionSteelGUI::setSectionNULL );
    }

    m_d->section = dynamic_cast<SectionSteel *>(s);
    m_d->dataSetModel->setSection( m_d->section );

    if( m_d->section ){
        VarPlusGUI::connectVar( m_d->section->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar( m_d->section->pW, m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );
        VarPlusGUI::connectEnumVar( m_d->section->sectionShape, m_d->ui->sectionShapeLabel, m_d->ui->sectionShapeComboBox );
        VarPlusGUI::connectEnumVar( m_d->section->manufactType, m_d->ui->manufactTypeLabel, m_d->ui->manufactTypeComboBox );

        // aggiunge label e LineEdit dei parametri
        for( int i=0; i < m_d->section->np(); ++i ){
            VarPlusGUI::connectVar( m_d->section->p(i), m_d->pLabel->at(i), m_d->pLEdit->at(i), m_d->pUMLabel->at(i), m_d->pCheckBox->at(i) );
        }
        updateVisibleP();
        connect( m_d->section->sectionShape, &SectionSteelShape::valueChanged, this, &SectionSteelGUI::updateVisibleP );

        VarPlusGUI::connectVar( m_d->section->A, m_d->ui->ALabel, m_d->ui->ALEdit, m_d->ui->AUMLabel, m_d->ui->ACheckBox );
        VarPlusGUI::connectVar( m_d->section->AEffN, m_d->ui->AEffNLabel, m_d->ui->AEffNLEdit, m_d->ui->AEffNUMLabel, m_d->ui->AEffNCheckBox );
        VarPlusGUI::connectVar( m_d->section->GEffN->y, m_d->ui->yGEffLabel, m_d->ui->yGEffLEdit, m_d->ui->yGEffUMLabel, m_d->ui->yGEffCheckBox );
        VarPlusGUI::connectVar( m_d->section->GEffN->z, m_d->ui->zGEffLabel, m_d->ui->zGEffLEdit, m_d->ui->zGEffUMLabel, m_d->ui->zGEffCheckBox );
        VarPlusGUI::connectVar( m_d->section->Iyy, m_d->ui->IyLabel, m_d->ui->IyLEdit, m_d->ui->IyUMLabel, m_d->ui->IyCheckBox );
        VarPlusGUI::connectVar( m_d->section->Izz, m_d->ui->IzLabel, m_d->ui->IzLEdit, m_d->ui->IzUMLabel, m_d->ui->IzCheckBox );
        VarPlusGUI::connectVar( m_d->section->WElMy, m_d->ui->WMyLabel, m_d->ui->WMyLEdit, m_d->ui->WMyUMLabel, m_d->ui->WMyCheckBox );
        VarPlusGUI::connectVar( m_d->section->WElMz, m_d->ui->WMzLabel, m_d->ui->WMzLEdit, m_d->ui->WMzUMLabel, m_d->ui->WMzCheckBox );
        VarPlusGUI::connectVar( m_d->section->WPlMy, m_d->ui->WPlMyLabel, m_d->ui->WPlMyLEdit, m_d->ui->WPlMyUMLabel, m_d->ui->WPlMyCheckBox );
        VarPlusGUI::connectVar( m_d->section->WPlMz, m_d->ui->WPlMzLabel, m_d->ui->WPlMzLEdit, m_d->ui->WPlMzUMLabel, m_d->ui->WPlMzCheckBox );
        VarPlusGUI::connectVar( m_d->section->WEffMyMax, m_d->ui->WEffMyMaxLabel, m_d->ui->WEffMyMaxLEdit, m_d->ui->WEffMyMaxUMLabel, m_d->ui->WEffMyMaxCheckBox );
        VarPlusGUI::connectVar( m_d->section->WEffMyMin, m_d->ui->WEffMyMinLabel, m_d->ui->WEffMyMinLEdit, m_d->ui->WEffMyMinUMLabel, m_d->ui->WEffMyMinCheckBox );
        VarPlusGUI::connectVar( m_d->section->WEffMzMax, m_d->ui->WEffMzMaxLabel, m_d->ui->WEffMzMaxLEdit, m_d->ui->WEffMzMaxUMLabel, m_d->ui->WEffMzMaxCheckBox );
        VarPlusGUI::connectVar( m_d->section->WEffMzMin, m_d->ui->WEffMzMinLabel, m_d->ui->WEffMzMinLEdit, m_d->ui->WEffMzMinUMLabel, m_d->ui->WEffMzMinCheckBox );
        VarPlusGUI::connectVar( m_d->section->iy, m_d->ui->iyLabel, m_d->ui->iyLEdit, m_d->ui->iyUMLabel, m_d->ui->iyCheckBox );
        VarPlusGUI::connectVar( m_d->section->iz, m_d->ui->izLabel, m_d->ui->izLEdit, m_d->ui->izUMLabel, m_d->ui->izCheckBox );
        VarPlusGUI::connectVar( m_d->section->IT, m_d->ui->ITLabel, m_d->ui->ITLEdit, m_d->ui->ITUMLabel, m_d->ui->ITCheckBox );
        VarPlusGUI::connectVar( m_d->section->Iw, m_d->ui->IwLabel, m_d->ui->IwLEdit, m_d->ui->IwUMLabel, m_d->ui->IwCheckBox );
        VarPlusGUI::connectVar( m_d->section->ty, m_d->ui->tyLabel, m_d->ui->tyLEdit, m_d->ui->tyUMLabel );
        VarPlusGUI::connectVar( m_d->section->tz, m_d->ui->tzLabel, m_d->ui->tzLEdit, m_d->ui->tzUMLabel );
        VarPlusGUI::connectVar( m_d->section->tyz, m_d->ui->tyzLabel, m_d->ui->tyzLEdit, m_d->ui->tyzUMLabel );
        VarPlusGUI::connectVar( m_d->section->NClass, m_d->ui->NClassLabel, m_d->ui->NClassLEdit, NULL, m_d->ui->NClassCheckBox );
        VarPlusGUI::connectVar( m_d->section->MyMaxClass, m_d->ui->MyMaxClassLabel, m_d->ui->MyMaxClassLEdit, NULL, m_d->ui->MyMaxClassCheckBox );
        VarPlusGUI::connectVar( m_d->section->MyMinClass, m_d->ui->MyMinClassLabel, m_d->ui->MyMinClassLEdit, NULL, m_d->ui->MyMinClassCheckBox );
        VarPlusGUI::connectVar( m_d->section->MyClassLimit, m_d->ui->MyClassLimitLabel, m_d->ui->MyClassLimitLEdit, m_d->ui->MyClassLimitUMLabel, m_d->ui->MyClassLimitCheckBox );
        VarPlusGUI::connectVar( m_d->section->MzMaxClass, m_d->ui->MzMaxClassLabel, m_d->ui->MzMaxClassLEdit, NULL, m_d->ui->MzMaxClassCheckBox );
        VarPlusGUI::connectVar( m_d->section->MzMinClass, m_d->ui->MzMinClassLabel, m_d->ui->MzMinClassLEdit, NULL, m_d->ui->MzMinClassCheckBox );
        VarPlusGUI::connectVar( m_d->section->MzClassLimit, m_d->ui->MzClassLimitLabel, m_d->ui->MzClassLimitLEdit, m_d->ui->MzClassLimitUMLabel, m_d->ui->MzClassLimitCheckBox );
        VarPlusGUI::connectVar( m_d->section->NULSMax, m_d->ui->NULSPlusLabel, m_d->ui->NULSPlusLEdit, m_d->ui->NULSPlusUMLabel, m_d->ui->NULSPlusCheckBox );
        VarPlusGUI::connectVar( m_d->section->NULSMin, m_d->ui->NULSMinusLabel, m_d->ui->NULSMinusLEdit, m_d->ui->NULSMinusUMLabel, m_d->ui->NULSMinusCheckBox );
        VarPlusGUI::connectVar( m_d->section->MULSyMax, m_d->ui->MULSyLabel, m_d->ui->MULSyLEdit, m_d->ui->MULSyUMLabel, m_d->ui->MULSyCheckBox );
        VarPlusGUI::connectVar( m_d->section->MULSzMax, m_d->ui->MULSzLabel, m_d->ui->MULSzLEdit, m_d->ui->MULSzUMLabel, m_d->ui->MULSzCheckBox );
        VarPlusGUI::connectVar( m_d->section->AVy, m_d->ui->AVyLabel, m_d->ui->AVyLEdit, m_d->ui->AVyUMLabel, m_d->ui->AVyCheckBox );
        VarPlusGUI::connectVar( m_d->section->VULSy, m_d->ui->VULSyLabel, m_d->ui->VULSyLEdit, m_d->ui->VULSyUMLabel, m_d->ui->VULSyCheckBox );
        VarPlusGUI::connectVar( m_d->section->AVz, m_d->ui->AVzLabel, m_d->ui->AVzLEdit, m_d->ui->AVzUMLabel, m_d->ui->AVzCheckBox );
        VarPlusGUI::connectVar( m_d->section->VULSz, m_d->ui->VULSzLabel, m_d->ui->VULSzLEdit, m_d->ui->VULSzUMLabel, m_d->ui->VULSzCheckBox );

        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
        if( m_d->section->steel() ){
            m_d->ui->steelComboBox->setCurrentIndex( m_d->ui->steelComboBox->findData( QVariant(m_d->section->steel()->id->valueNormal()) ) );
        } else {
            m_d->ui->steelComboBox->setCurrentIndex(-1);
        }
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );

        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamlY()));
        setBeamlY();
        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamNCrY()));
        setBeamNCrY();
        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamlRelY()));
        setBeamlRelY();
        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamChiY()));
        setBeamChiY();
        connect( m_d->section->NULSMin, SIGNAL(valueChanged(QString)), this, SLOT(setBeamNULSBucklY()));
        setBeamNULSBucklY();
        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamlZ()));
        setBeamlZ();
        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamNCrZ()));
        setBeamNCrZ();
        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamlRelZ()));
        setBeamlRelZ();
        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamNCrZ()));
        setBeamNCrZ();
        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamChiZ()));
        setBeamChiZ();
        connect( m_d->section->NULSMin, SIGNAL(valueChanged(QString)), this, SLOT(setBeamNULSBucklZ()));
        setBeamNULSBucklZ();
        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(setBeamChiLT()));
        setBeamChiLT();

        connect( m_d->section->G->y, SIGNAL(valueChanged(QString)), this, SLOT(setYCenMSd()));
        setYCenMSd();
        connect( m_d->section->G->z, SIGNAL(valueChanged(QString)), this, SLOT(setZCenMSd()));
        setZCenMSd();

        connect( m_d->section, SIGNAL(sectionChanged()), this, SLOT(clearSectsToView()));

        connect( m_d->section, &Section::aboutToBeDeleted, this, &SectionSteelGUI::setSectionNULL );
    } else {
        updateVisibleP();
    }
    setEnabled( m_d->section != NULL );
}

void SectionSteelGUI::setSectionNULL(){
    setSection( NULL );
}

void SectionSteelGUI::updateVisibleP() {
    for( int i=0; i < m_d->pLabel->size(); ++i ){
        m_d->pLabel->at(i)->setVisible( false );
    }
    for( int i=0; i < m_d->pLEdit->size(); ++i ){
        m_d->pLEdit->at(i)->setVisible( false );
    }
    for( int i=0; i < m_d->pUMLabel->size(); ++i ){
        m_d->pUMLabel->at(i)->setVisible( false );
    }
    for( int i=0; i < m_d->pCheckBox->size(); ++i ){
        m_d->pCheckBox->at(i)->setVisible( false );
    }

    if( m_d->section != NULL ){
        for( int i=0; i < m_d->section->np(); ++i ){
            m_d->pLabel->at(i)->setVisible( true );
            m_d->pLEdit->at(i)->setVisible( true );
            m_d->pUMLabel->at(i)->setVisible( true );
            m_d->pCheckBox->at(i)->setVisible( true );
        }
    }
}

void SectionSteelGUI::setBeamlY(){
    if( m_d->section != NULL && m_d->beamlY->readOnly()){
        m_d->section->beamlBuckl( m_d->beamlY, m_d->beamL0y, NULL );
    }
}

void SectionSteelGUI::setBeamNCrY(){
    if( m_d->section != NULL && m_d->beamNCrY->readOnly() ){
        m_d->section->beamNCr( m_d->beamNCrY, m_d->beamL0y, NULL );
    }
}

void SectionSteelGUI::setBeamlRelY(){
    if( m_d->section != NULL && m_d->beamlRelY->readOnly() ){
        m_d->section->beamlRelBuckl( m_d->beamlRelY, m_d->beamNCrY );
    }
}

void SectionSteelGUI::setBeamChiY(){
    if( m_d->section != NULL && m_d->beamChiY->readOnly() ){
        m_d->section->beamChiBucklY( m_d->beamChiY, m_d->beamlRelY );
    }
}

void SectionSteelGUI::setBeamNULSBucklY(){
    if( m_d->section != NULL && m_d->beamNULSBucklY->readOnly() ){
        m_d->section->beamNULSBuckl( m_d->beamNULSBucklY, m_d->beamChiY );
    }
}

void SectionSteelGUI::setBeamlZ(){
    if( m_d->section != NULL && m_d->beamlZ->readOnly() ){
        m_d->section->beamlBuckl( m_d->beamlZ, NULL, m_d->beamL0z );
    }
}

void SectionSteelGUI::setBeamNCrZ(){
    if( m_d->section != NULL && m_d->beamNCrZ->readOnly() ){
        m_d->section->beamNCr( m_d->beamNCrZ, NULL, m_d->beamL0z );
    }
}

void SectionSteelGUI::setBeamlRelZ(){
    if( m_d->section != NULL && m_d->beamlRelZ->readOnly() ){
        m_d->section->beamlRelBuckl( m_d->beamlRelZ, m_d->beamNCrZ );
    }
}

void SectionSteelGUI::setBeamChiZ(){
    if( m_d->section != NULL && m_d->beamChiZ->readOnly() ){
        m_d->section->beamChiBucklZ( m_d->beamChiZ, m_d->beamlRelZ );
    }
}

void SectionSteelGUI::setBeamNULSBucklZ(){
    if( m_d->section != NULL && m_d->beamNULSBucklZ->readOnly() ){
        m_d->section->beamNULSBuckl( m_d->beamNULSBucklZ, m_d->beamChiZ );
    }
}

void SectionSteelGUI::setBeamMCr(){
    if( m_d->section != NULL && m_d->beamMCr->readOnly() ){
        m_d->section->beamMCrLT( m_d->beamMCr, m_d->beamL0LT, m_d->beamMEdLTH1, m_d->beamMEdLTH2 );
    }
}

void SectionSteelGUI::setBeamlRelLT(){
    if( m_d->section != NULL && m_d->beamlRelLT->readOnly() ){
        m_d->section->beamlRelBucklLT( m_d->beamlRelLT, m_d->beamMCr );
    }
}

void SectionSteelGUI::setBeamChiLT(){
    if( m_d->section != NULL && m_d->beamChiLT->readOnly() ){
        m_d->section->beamChiBucklLT( m_d->beamChiLT,  m_d->beamMEdLTH1, m_d->beamMEdLTH2, m_d->beamMEdLTDistribution, m_d->beamlRelLT );
    }
}

void SectionSteelGUI::setYCenMSd(){
    if( m_d->section != NULL && m_d->cenMEd->y->readOnly() ){
        *(m_d->cenMEd->y) = *(m_d->section->G->y);
    }
}

void SectionSteelGUI::setZCenMSd(){
    if( m_d->section != NULL && m_d->cenMEd->z->readOnly() ){
        *(m_d->cenMEd->z) = *(m_d->section->G->z);
    }
}

void SectionSteelGUI::calculateMULS(){
    if( m_d->section != NULL ){
        clearSectsToView();

        m_d->section->MULS( m_d->MyULSN, m_d->MzULSN, m_d->NMULSN, m_d->cenMULSN, m_d->alphaMULSN );
    }
}

void SectionSteelGUI::plotMULSyMULSz(){
    if( m_d->section != NULL ){
        SimpleDataPlotterDialog d( trUtf8("Interazione My-Mz"), this );
        d.setCurveData( 0, m_d->section->MULSyMULSz(m_d->NMULSN, m_d->cenMULSN, m_d->ui->MULSyMULSzAccuracySBox->value()) );
        d.exec();
    }
}

void SectionSteelGUI::setYCenMULSN(){
    if( m_d->section ){
        if( m_d->cenMULSN->y->readOnly() ){
            *(m_d->cenMULSN->y) = *(m_d->section->G->y);
        }
    }
}

void SectionSteelGUI::setZCenMULSN(){
    if( m_d->section ){
        if( m_d->cenMULSN->z->readOnly() ){
            *(m_d->cenMULSN->z) = *(m_d->section->G->z);
        }
    }
}

void SectionSteelGUI::plotMULSN(){
    if( m_d->section ){
        SimpleDataPlotterDialog d( trUtf8("Interazione N-M"), this );
        d.setCurveData( 0, m_d->section->MULSN( m_d->cenMULSN, m_d->alphaMULSN, m_d->ui->MULSNAccuracySBox->value()) );
        d.exec();
    }
}

void SectionSteelGUI::plotClassDiagram(){
    if( m_d->section ){
        SectionClassDiagramGUI gui( m_d->section, this );
        gui.exec();
    }
}

void SectionSteelGUI::verifyULS(){
    if( m_d->section ){
        QString cl;
        QString mes;

        if( m_d->section->verifyULS( m_d->beamlRelY,
                                     m_d->beamChiY,
                                     m_d->beamlRelZ,
                                     m_d->beamChiZ,
                                     m_d->NEd,
                                     m_d->torsDeformability,
                                     m_d->beamChiLT,
                                     m_d->beamMSdYDistribution,
                                     m_d->beamMEdYH1,
                                     m_d->beamMEdYS,
                                     m_d->beamMEdYH2,
                                     m_d->beamMEdZDistribution,
                                     m_d->beamMEdZH1,
                                     m_d->beamMEdZS,
                                     m_d->beamMEdZH2,
                                     m_d->beamMEdLTDistribution,
                                     m_d->beamMEdLTH1,
                                     m_d->beamMEdLTS,
                                     m_d->beamMEdLTH2,
                                     m_d->cenMEd,
                                     m_d->MEdY,
                                     m_d->MEdZ,
                                     m_d->VSdY,
                                     m_d->VSdZ,
                                     m_d->resultSd,
                                     &cl,
                                     &mes ) ) {
            m_d->ui->resultSdULSLEdit->setStyleSheet( "* {background-color: rgb(0, 255, 0)}" );
        } else {
            m_d->ui->resultSdULSLEdit->setStyleSheet( "* {background-color: rgb(255, 0, 0)}" );
        }
        m_d->ui->ClassULSLEdit->setText( cl );
        m_d->ui->messagesSdULSTBrowser->clear();
        m_d->ui->messagesSdULSTBrowser->append( mes );

    }
}

void SectionSteelGUI::setMaterialFromComboBox(){
    if( m_d->section != NULL && m_materialModel != NULL && m_materialComboBox != NULL ){
        if( m_materialComboBox->currentIndex() == -1 ){
            m_d->section->setSteel( NULL );
        } else {
            Steel * steel = dynamic_cast<Steel *>(m_materialModel->materialId( m_materialComboBox->currentData().toUInt()));
            if( steel != NULL ){
                m_d->section->setSteel( steel );
            }
        }
    }
}

Material * SectionSteelGUI::material(){
    if( m_d->section != NULL ){
        return m_d->section->steel();
    }
    return NULL;
}

void SectionSteelGUI::setNMyMzElastPlast(){
    if( m_d->section ){
        clearSectsToView();
        // QList<Point2DPlusModel *> * pointsModel = new QList<Point2DPlusModel *>;
        double N, My, Mz;
        m_d->section->NMULSElastPlastNormal( &N, &My, &Mz, m_d->lElastPlast->valueNormal(), m_d->myElastPlast->valueNormal(), m_d->mzElastPlast->valueNormal(), 0.0,0.0 );
        m_d->NElastPlast->setValueNormal(N);
        m_d->MyElastPlast->setValueNormal(My);
        m_d->MzElastPlast->setValueNormal(Mz);
        /* QList<Point2DPlusModel *>::iterator iter = pointsModel->begin();
        int i=0;
        while( iter != pointsModel->end() ){
            QGraphicsPolygonItem * line = new QGraphicsPolygonItem( (*iter)->qPolygonF(), m_graphicsItem );
            QPen p = line->pen();
            p.setColor( Qt::transparent );
            if( i == 0 ){
                line->setBrush( QBrush(Qt::green));
            } else if(  i== 1){
                line->setBrush( QBrush(Qt::red));
            } else if(  i== 2){
                line->setBrush( QBrush(Qt::magenta));
            } else {
                line->setBrush( QBrush(Qt::blue));
            }
            m_d->sectsToView->append( line );
            Point2DPlusModel::deleteModel( *iter );
            i++;
            iter++;
        }
        delete pointsModel;*/
    }
}

void SectionSteelGUI::setNMyMzPlast(){
    if( m_d->section ){
        clearSectsToView();
        // QList<Point2DPlusModel *> * pointsModel = new QList<Point2DPlusModel *>;
        double N, My, Mz;
        m_d->section->NMULSPlastNormal( &N, &My, &Mz, m_d->lPlast->valueNormal(), m_d->myPlast->valueNormal(), m_d->mzPlast->valueNormal(), 0.0,0.0 );
        m_d->NPlast->setValueNormal(N);
        m_d->MyPlast->setValueNormal(My);
        m_d->MzPlast->setValueNormal(Mz);
        /* QList<Point2DPlusModel *>::iterator iter = pointsModel->begin();
        int i=0;
        while( iter != pointsModel->end() ){
            QGraphicsPolygonItem * line = new QGraphicsPolygonItem( (*iter)->qPolygonF(), m_graphicsItem );
            QPen p = line->pen();
            p.setColor( Qt::transparent );
            if( i == 0 ){
                line->setBrush( QBrush(Qt::green));
            } else {
                line->setBrush( QBrush(Qt::red));
            }
            m_d->sectsToView->append( line );
            Point2DPlusModel::deleteModel( *iter );
            i++;
            iter++;
        }
        delete pointsModel;*/
    }
}

void SectionSteelGUI::clearSectsToView(){
    for( int i=0; i < m_d->sectsToView->size(); i++ ){
        m_graphicsItem->removeFromGroup( m_d->sectsToView->at(i));
    }
    qDeleteAll( m_d->sectsToView->begin(), m_d->sectsToView->end() );
    m_d->sectsToView->clear();
}

void SectionSteelGUI::dataSetClear(){
    m_d->dataSetModel->clear();
}

void SectionSteelGUI::dataSetLoadFromFile(){
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning( this, trUtf8("QStruct"), trUtf8("Non riesco ad aprire il file file %1:\n%2.").arg(fileName).arg(file.errorString()));
            return;
        }
        m_d->dataSetModel->clear();
        m_d->dataSetModel->loadFromFile( file );
        file.close();
    }
}

void SectionSteelGUI::dataSetVerifyULSAll(){
    m_d->dataSetModel->verifyULSAll();
}

void SectionSteelGUI::dataSetFromTableToGUI(){
    int currRow = m_d->ui->dataSetTableView->selectionModel()->currentIndex().row();
    if( currRow >= 0 && currRow < m_d->dataSetModel->rowCount() ){
        SectionSteelData * currData = m_d->dataSetModel->data( currRow );

        *m_d->NEd = *(currData->N);
        *m_d->MEdY = *(currData->My);
        *m_d->MEdZ = *(currData->Mz);
        *m_d->cenMEd = *(currData->cenM);
        *m_d->beamMSdYDistribution = *(currData->beamMyDistribution);
        *m_d->beamMEdYH1 = *(currData->beamMyH1);
        *m_d->beamMEdYS = *(currData->beamMyS);
        *m_d->beamMEdYH2 = *(currData->beamMyH2);
        *m_d->beamMEdZDistribution = *(currData->beamMzDistribution);
        *m_d->beamMEdZH1 = *(currData->beamMzH1);
        *m_d->beamMEdZS = *(currData->beamMzS);
        *m_d->beamMEdZH2 = *(currData->beamMzH2);
        *m_d->beamMEdLTDistribution = *(currData->beamMLTDistribution);
        *m_d->beamMEdLTH1 = *(currData->beamMLTH1);
        *m_d->beamMEdLTS = *(currData->beamMLTS);
        *m_d->beamMEdLTH2 = *(currData->beamMLTH2);
        *m_d->beamMCr = *(currData->beamMCr);
        *m_d->beamlRelLT = *(currData->beamlRelLT);
        *m_d->beamChiLT = *(currData->beamChiLT);
        *m_d->VSdZ = *(currData->Vz);
        *m_d->VSdY = *(currData->Vy);
    }
}

void SectionSteelGUI::dataSetFromGUIToTable(){
    int currRow = m_d->ui->dataSetTableView->selectionModel()->currentIndex().row();
    if( currRow >= 0 && currRow < m_d->dataSetModel->rowCount() ){
        SectionSteelData * currData = m_d->dataSetModel->data( currRow );

        *(currData->N) = *m_d->NEd;
        *(currData->My) = *m_d->MEdY;
        *(currData->Mz) = *m_d->MEdZ;
        *(currData->cenM) = *m_d->cenMEd;
        *(currData->beamMyDistribution) = *m_d->beamMSdYDistribution;
        *(currData->beamMyH1) = *m_d->beamMEdYH1;
        *(currData->beamMyS) = *m_d->beamMEdYS;
        *(currData->beamMyH2) = *m_d->beamMEdYH2;
        *(currData->beamMzDistribution) = *m_d->beamMEdZDistribution;
        *(currData->beamMzH1) = *m_d->beamMEdZH1;
        *(currData->beamMzS) = *m_d->beamMEdZS;
        *(currData->beamMzH2) = *m_d->beamMEdZH2;
        *(currData->beamMLTDistribution) = *m_d->beamMEdLTDistribution;
        *(currData->beamMLTH1) = *m_d->beamMEdLTH1;
        *(currData->beamMLTS) = *m_d->beamMEdLTS;
        *(currData->beamMLTH2) = *m_d->beamMEdLTH2;
        *(currData->beamMCr) = *m_d->beamMCr;
        *(currData->beamlRelLT) = *m_d->beamlRelLT;
        *(currData->beamChiLT) = *m_d->beamChiLT;
        *(currData->Vz) = *m_d->VSdZ;
        *(currData->Vy) = *m_d->VSdY;
    }
}

QList< Material * > SectionSteelGUI::materialsList(){
    return m_materialModel->materialsList( MaterialModel::SteelMaterial );
}
