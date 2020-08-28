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
#include "sectionrcncrgui.h"
#include "ui_sectionrcncrgui.h"

#include "sectiongeometricdatapanel.h"
#include "sectiontransformpanel.h"
#include "sectionmaterialdelegate.h"
#include "sectionsteelgui.h"

#include "materialmodel.h"
#include "sectionrcncr.h"
#include "multisectioncncrmodel.h"
#include "multisectionsteelcncrmodel.h"
#include "multisectionsteelmodel.h"
#include "multisectionfrpmodel.h"
#include "sectioncncr.h"
#include "sectionsteel.h"
#include "sectionfrp.h"
#include "sectionloadphasemodel.h"
#include "sectionloadphase.h"

#include "varplusguiutility.h"
#include "varpluscontainer.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "point2dplusmodel.h"
#include "point2dplus.h"
#include "point2dmodel.h"

#include "qcustomplotdialog.h"

#include <QTextStream>
#include <QFileDialog>
#include <QFile>
#include <QList>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QGraphicsPolygonItem>
#include <QClipboard>

class SectionRCncrGUIPrivate: public VarPlusContainer{
public:
    SectionRCncrGUIPrivate( UnitMeasure * ump, MaterialModel * matModel, const QString & sFile ):
        VarPlusContainer(ump, "", ""),
        settingsFile(sFile){
        ui = new Ui::SectionRCncrGUI;

        section = nullptr;
        materialModel = matModel;
        currentSectionCncrPointModel = nullptr;
        currentSectionFRP = nullptr;
        sectsToView = new QList<QGraphicsPolygonItem *>;

        geometricDataPanel = nullptr;
        transformPanel = nullptr;

        initVar();
    }
    void initVar(){
        bRect = new DoublePlus(0.0, "bRect", m_unitMeasure, UnitMeasure::sectL );
        bRect->setToolTip( QObject::tr("Base della sezione rettangolare"));
        bRect->setRichName( QObject::tr("b"));
        addVarToContainer( bRect );

        hRect = new DoublePlus(0.0, "hRect", m_unitMeasure, UnitMeasure::sectL );
        hRect->setToolTip( QObject::tr("Altezza della sezione rettangolare"));
        hRect->setRichName( QObject::tr("h"));
        addVarToContainer(hRect);

        NULS = new DoublePlus(0.0, "NULS", m_unitMeasure, UnitMeasure::loadF );
        NULS->setToolTip( QObject::tr("Sforzo normale agente") );
        NULS->setRichName( QObject::tr("N<span style=\"vertical-align:sub;\">Sd</span>"));
        addVarToContainer( NULS );

        alphaULS = new DoublePlus(0.0, "alphaULS", m_unitMeasure, UnitMeasure::angle );
        alphaULS->setToolTip( QObject::tr("Inclinazione piano di deformazione rispetto all'asse y") );
        alphaULS->setRichName( QObject::tr("α<span style=\" vertical-align:sub;\">Sd</span>"));
        addVarToContainer( alphaULS );

        NCenULS = new Point2DPlus( "NCenULS", "NCenULSY", 0.0, "NCenULSZ", 0.0, m_unitMeasure, UnitMeasure::sectL);
        addVarToContainer( NCenULS );
        NCenULS->y->setToolTip( QObject::tr("Ascissa del polo attorno a cui si calcola il momento"));
        NCenULS->y->setRichName( QObject::tr("y<span style=\"vertical-align:sub;\">N</span>"));
        NCenULS->y->setReadOnly( true );
        NCenULS->z->setToolTip( QObject::tr("Ordinata del polo attorno a cui si calcola il momento"));
        NCenULS->z->setRichName( QObject::tr("z<span style=\"vertical-align:sub;\">N</span>"));
        NCenULS->z->setReadOnly( true );

        MULSy = new DoublePlus(0.0, "MULSy", m_unitMeasure, UnitMeasure::loadM, true );
        MULSy->setToolTip( QObject::tr("Componente y del momento resistente"));
        MULSy->setRichName( QObject::tr("M<span style=\"vertical-align:sub;\">Rd,y</span>"));
        addVarToContainer( MULSy );

        MULSz = new DoublePlus(0.0, "MULSz", m_unitMeasure, UnitMeasure::loadM, true );
        MULSz->setToolTip( QObject::tr("Componente z del momento resistente"));
        MULSz->setRichName( QObject::tr("M<span style=\"vertical-align:sub;\">Rd,z</span>"));
        addVarToContainer( MULSz );

        MULSyMULSzKRed = new DoublePlus(1.0, "MULSyMULSzKRed", m_unitMeasure, UnitMeasure::noDimension );
        MULSyMULSzKRed->setToolTip( QObject::tr("Coefficiente di riduzione dei valori del momento"));
        MULSyMULSzKRed->setRichName( QObject::tr("k<span style=\"vertical-align:sub;\">red</span>"));
        addVarToContainer( MULSyMULSzKRed );

        LSPlusNULS = new DoublePlus(0.0, "LSPlusNULS", m_unitMeasure, UnitMeasure::loadF );
        LSPlusNULS->setToolTip( QObject::tr("Sforzo normale agente"));
        LSPlusNULS->setRichName( QObject::tr("N<span style=\"vertical-align:sub;\">Ed</span>"));
        addVarToContainer( LSPlusNULS );

        LSPlusMyULS = new DoublePlus(0.0, "LSPlusMyULS", m_unitMeasure, UnitMeasure::loadM );
        LSPlusMyULS->setToolTip( QObject::tr("Momento agente rispetto all'asse y"));
        LSPlusMyULS->setRichName( QObject::tr("M<span style=\"vertical-align:sub;\">Ed,y</span>"));
        addVarToContainer( LSPlusMyULS );

        LSPlusMzULS = new DoublePlus(0.0, "LSPlusMzULS", m_unitMeasure, UnitMeasure::loadM );
        LSPlusMzULS->setToolTip( QObject::tr("Momento agente rispetto all'asse z"));
        LSPlusMzULS->setRichName( QObject::tr("M<span style=\"vertical-align:sub;\">Ed,z</span>"));
        addVarToContainer( LSPlusMzULS );

        LSPlusNCenULS  = new Point2DPlus( "LSPlusNCenULS", "LSPlusNYCenULS", 0.0, "LSPlusNZCenULS", 0.0, m_unitMeasure, UnitMeasure::sectL);
        addVarToContainer( LSPlusNCenULS );
        LSPlusNCenULS->y->setToolTip( QObject::tr("Ascissa del polo attorno a cui si calcola il momento"));
        LSPlusNCenULS->y->setRichName( QObject::tr("y<span style=\"vertical-align:sub;\">M</span>"));
        LSPlusNCenULS->y->setReadOnly( true );
        LSPlusNCenULS->z->setToolTip( QObject::tr("Ordinata del polo attorno a cui si calcola il momento"));
        LSPlusNCenULS->z->setRichName( QObject::tr("z<span style=\"vertical-align:sub;\">M</span>"));
        LSPlusNCenULS->z->setReadOnly( true );

        LSPluslULS = new DoublePlus(0.0, "LSPluslULS", m_unitMeasure, UnitMeasure::deformation, true );
        LSPluslULS->setToolTip( QObject::tr("Deformazione assiale corrispondente"));
        LSPluslULS->setRichName( QObject::tr("λ"));
        addVarToContainer( LSPluslULS );

        LSPlusmyULS = new DoublePlus(0.0, "LSPlusmyULS", m_unitMeasure, UnitMeasure::curvature, true );
        LSPlusmyULS->setToolTip( QObject::tr("Curvatura rispetto ad asse y"));
        LSPlusmyULS->setRichName( QObject::tr("μ<span style=\"vertical-align:sub;\">y</span>"));
        addVarToContainer( LSPlusmyULS );

        LSPlusmzULS = new DoublePlus(0.0, "LSPlusmzULS", m_unitMeasure, UnitMeasure::curvature, true );
        LSPlusmzULS->setToolTip( QObject::tr("Curvatura rispetto ad asse z"));
        LSPlusmzULS->setRichName( QObject::tr("μ<span style=\"vertical-align:sub;\">z</span>"));
        addVarToContainer( LSPlusmzULS );

        NSLS = new DoublePlus(0.0, "NSLS", m_unitMeasure, UnitMeasure::loadF );
        NSLS->setToolTip( QObject::tr("Sforzo normale agente") );
        NSLS->setRichName( QObject::tr("N"));
        addVarToContainer( NSLS );

        NCenSLS = new Point2DPlus( "NCenSLS", "NCenYSLS", 0.0, "NCenZSLS", 0.0, m_unitMeasure, UnitMeasure::sectL);
        addVarToContainer( NCenSLS );
        NCenSLS->y->setToolTip( QObject::tr("Ascissa del polo attorno a cui si calcola il momento"));
        NCenSLS->y->setRichName( QObject::tr("y<span style=\"vertical-align:sub;\">N</span>"));
        NCenSLS->y->setReadOnly( true );
        NCenSLS->z->setToolTip( QObject::tr("Ordinata del polo attorno a cui si calcola il momento"));
        NCenSLS->z->setRichName( QObject::tr("z<span style=\"vertical-align:sub;\">N</span>"));
        NCenSLS->z->setReadOnly( true );

        MySLS = new DoublePlus(0.0, "MySLS", m_unitMeasure, UnitMeasure::loadM );
        MySLS->setToolTip( QObject::tr("Momento agente - componente y") );
        MySLS->setRichName( QObject::tr("M<span style=\"vertical-align:sub;\">y</span>"));
        addVarToContainer( MySLS );

        MzSLS = new DoublePlus(0.0, "MzSLS", m_unitMeasure, UnitMeasure::loadM );
        MzSLS->setToolTip( QObject::tr("Momento agente - componente z") );
        MzSLS->setRichName( QObject::tr("M<span style=\"vertical-align:sub;\">z</span>"));
        addVarToContainer( MzSLS );

        lSLS = new DoublePlus(0.0, "lSLS", m_unitMeasure, UnitMeasure::deformation );
        lSLS->setToolTip( QObject::tr("Deformazione nell'origine") );
        lSLS->setRichName( QObject::tr("λ"));
        addVarToContainer( lSLS );

        mySLS = new DoublePlus(0.0, "mySLS", m_unitMeasure, UnitMeasure::curvature );
        mySLS->setToolTip( QObject::tr("Curvatura - componente y") );
        mySLS->setRichName( QObject::tr("μ<span style=\"vertical-align:sub;\">y</span>"));
        addVarToContainer( mySLS );

        mzSLS = new DoublePlus(0.0, "mzSLS", m_unitMeasure, UnitMeasure::curvature );
        mzSLS->setToolTip( QObject::tr("Curvatura - componente z") );
        mzSLS->setRichName( QObject::tr("μ<span style=\"vertical-align:sub;\">z</span>"));
        addVarToContainer( mzSLS );

        fCncrMinSLS = new DoublePlus(0.0, "fCncrMinSLS", m_unitMeasure, UnitMeasure::tension );
        fCncrMinSLS->setToolTip( QObject::tr("Tensione minima nel calcestruzzo") );
        fCncrMinSLS->setRichName( QObject::tr("σ<span style=\"vertical-align:sub;\">c,min</span>"));
        addVarToContainer( fCncrMinSLS );

        fSteelCncrMinSLS = new DoublePlus(0.0, "fSteelCncrMinSLS", m_unitMeasure, UnitMeasure::tension );
        fSteelCncrMinSLS->setToolTip( QObject::tr("Tensione minima nell'armatura in acciaio") );
        fSteelCncrMinSLS->setRichName( QObject::tr("σ<span style=\"vertical-align:sub;\">s,min</span>"));
        addVarToContainer( fSteelCncrMinSLS );

        fSteelCncrMaxSLS = new DoublePlus(0.0, "fSteelCncrMaxSLS", m_unitMeasure, UnitMeasure::tension );
        fSteelCncrMaxSLS->setToolTip( QObject::tr("Tensione massima nell'armatura in acciaio") );
        fSteelCncrMaxSLS->setRichName( QObject::tr("σ<span style=\"vertical-align:sub;\">s,max</span>"));
        addVarToContainer( fSteelCncrMaxSLS );
    }
    ~SectionRCncrGUIPrivate(){
        delete ui;
    }
    void connectVar( QObject * parent  ){
        // ui->sectionsCncrTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        sectionsCncrMatDelegate = new SectionMaterialDelegate(materialModel, MaterialModel::ConcreteMaterial, 1, 2, parent );
        ui->sectionsCncrTableView->setItemDelegate( sectionsCncrMatDelegate );
        // ui->sectCncrPointsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        // ui->sectionsSteelCncrTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        sectionsSteelCncrMatDelegate = new SectionMaterialDelegate( materialModel, MaterialModel::SteelCncrMaterial,
                                                                    MultiSectionSteelCncrModel::materialColumn(), MultiSectionSteelCncrModel::loadPhaseColumn(), parent );
        ui->sectionsSteelCncrTableView->setItemDelegate( sectionsSteelCncrMatDelegate );
        // ui->sectionsSteelTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        sectionsSteelMatDelegate = new SectionMaterialDelegate( materialModel, MaterialModel::SteelMaterial,
                                                                MultiSectionSteelModel::materialColumn(), MultiSectionSteelModel::loadPhaseColumn(), parent );
        ui->sectionsSteelTableView->setItemDelegate( sectionsSteelMatDelegate );
        // ui->sectionsFRPTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        sectionsFRPMatDelegate = new SectionMaterialDelegate( materialModel, MaterialModel::FRPMaterial,
                                                              MultiSectionFRPModel::materialColumn(), MultiSectionFRPModel::loadPhaseColumn(), parent );
        ui->sectionsFRPTableView->setItemDelegate( sectionsFRPMatDelegate );

        ui->MULSyMULSzAccuracySBox->setMinimum( 4 );
        ui->MULSyMULSzAccuracySBox->setValue( 100 );
        ui->MULSNAccuracySBox->setMinimum( 1 );
        ui->MULSNAccuracySBox->setValue( 10 );

        sectionSteelGUI = new SectionSteelGUI( m_unitMeasure, materialModel, settingsFile );
        sectionSteelGUI->setEnabled( false );
        ui->steelSplitter->addWidget( sectionSteelGUI );

        geometricDataPanel = new SectionGeometricDataPanel( ui->geometryScrollContents );
        ui->geometryScrollContents->layout()->addWidget( geometricDataPanel );

        transformPanel = new SectionTransformPanel( m_unitMeasure, ui->transformationTab );
        ui->transformationScrollContents->layout()->addWidget( transformPanel );

        VarPlusGUI::connectVar( bRect, ui->bRectLabel, ui->bRectLEdit, ui->bRectUMLabel );
        VarPlusGUI::connectVar( hRect, ui->hRectLabel, ui->hRectLEdit, ui->hRectUMLabel );
        VarPlusGUI::connectVar( NULS, ui->NULSLabel, ui->NULSLEdit, ui->NULSUMLabel );
        VarPlusGUI::connectVar( alphaULS, ui->alphaULSLabel, ui->alphaULSLEdit, ui->alphaULSUMLabel );
        VarPlusGUI::connectVar( NCenULS->y, ui->yCenULSLabel, ui->yCenULSLEdit, ui->yCenULSUMLabel, ui->yCenULSCheckBox );
        VarPlusGUI::connectVar( NCenULS->z, ui->zCenULSLabel, ui->zCenULSLEdit, ui->zCenULSUMLabel, ui->zCenULSCheckBox );
        VarPlusGUI::connectVar( MULSy, ui->MULSyLabel, ui->MULSyLEdit, ui->MULSyUMLabel );
        VarPlusGUI::connectVar( MULSz, ui->MULSzLabel, ui->MULSzLEdit, ui->MULSzUMLabel  );
        VarPlusGUI::connectVar( MULSyMULSzKRed, ui->MULSyMULSzKRedLabel, ui->MULSyMULSzKRedLEdit, ui->MULSyMULSzKRedUMLabel  );

        VarPlusGUI::connectVar( MULSyMULSzKRed, ui->MULSyMULSzKRedLabel, ui->MULSyMULSzKRedLEdit, ui->MULSyMULSzKRedUMLabel  );

        VarPlusGUI::connectVar( LSPlusNULS, ui->LSPlusNULSLabel, ui->LSPlusNULSLineEdit, ui->LSPlusNULSUMLabel );
        VarPlusGUI::connectVar( LSPlusMyULS, ui->LSPlusMyULSLabel, ui->LSPlusMyULSLineEdit, ui->LSPlusMyULSUMLabel );
        VarPlusGUI::connectVar( LSPlusMzULS, ui->LSPlusMzULSLabel, ui->LSPlusMzULSLineEdit, ui->LSPlusMzULSUMLabel );
        VarPlusGUI::connectVar( LSPlusNCenULS->y, ui->LSPlusNyCenULSLabel, ui->LSPlusNyCenULSLEdit, ui->LSPlusNyCenULSUMLabel, ui->LSPlusNyCenULSCheckBox );
        VarPlusGUI::connectVar( LSPlusNCenULS->z, ui->LSPlusNzCenULSLabel, ui->LSPlusNzCenULSLEdit, ui->LSPlusNzCenULSUMLabel, ui->LSPlusNzCenULSCheckBox );
        VarPlusGUI::connectVar( LSPluslULS, ui->LSPluslULSLabel, ui->LSPluslULSLineEdit, ui->LSPluslULSUMLabel );
        VarPlusGUI::connectVar( LSPlusmyULS, ui->LSPlusmyULSLabel, ui->LSPlusmyULSLineEdit, ui->LSPlusmyULSUMLabel );
        VarPlusGUI::connectVar( LSPlusmzULS, ui->LSPlusmzULSLabel, ui->LSPlusmzULSLineEdit, ui->LSPlusmzULSUMLabel );

        VarPlusGUI::connectVar( NSLS, ui->NSLSLabel, ui->NSLSLEdit, ui->NSLSUMLabel );
        VarPlusGUI::connectVar( NCenSLS->y, ui->NCenYSLSLabel, ui->NCenYSLSLEdit, ui->NCenYSLSUMLabel, ui->NCenYSLSCheckBox );
        VarPlusGUI::connectVar( NCenSLS->z, ui->NCenZSLSLabel, ui->NCenZSLSLEdit, ui->NCenZSLSUMLabel, ui->NCenZSLSCheckBox );
        VarPlusGUI::connectVar( MySLS, ui->MySLSLabel, ui->MySLSLEdit, ui->MySLSUMLabel );
        VarPlusGUI::connectVar( MzSLS, ui->MzSLSLabel, ui->MzSLSLEdit, ui->MzSLSUMLabel );
        VarPlusGUI::connectVar( lSLS, ui->lSLSLabel, ui->lSLSLEdit, ui->lSLSUMLabel );
        VarPlusGUI::connectVar( mySLS, ui->mySLSLabel, ui->mySLSLEdit, ui->mySLSUMLabel );
        VarPlusGUI::connectVar( mzSLS, ui->mzSLSLabel, ui->mzSLSLEdit, ui->mzSLSUMLabel );
        VarPlusGUI::connectVar( fCncrMinSLS, ui->fSLSCncrMinLabel, ui->fSLSCncrMinLEdit, ui->fSLSCncrMinUMLabel );
        VarPlusGUI::connectVar( fSteelCncrMinSLS, ui->fSLSSteelCncrMinLabel, ui->fSLSSteelCncrMinLEdit, ui->fSLSSteelCncrMinUMLabel );
        VarPlusGUI::connectVar( fSteelCncrMaxSLS, ui->fSLSSteelCncrMaxLabel, ui->fSLSSteelCncrMaxLEdit, ui->fSLSSteelCncrMaxUMLabel );
    }

    Ui::SectionRCncrGUI * ui;
    SectionSteelGUI * sectionSteelGUI;
    SectionGeometricDataPanel * geometricDataPanel;
    SectionTransformPanel * transformPanel;

    SectionMaterialDelegate * sectionsCncrMatDelegate;
    SectionMaterialDelegate * sectionsSteelCncrMatDelegate;
    SectionMaterialDelegate * sectionsSteelMatDelegate;
    SectionMaterialDelegate * sectionsFRPMatDelegate;

    QString settingsFile;
    SectionRCncr * section;
    Point2DPlusModel * currentSectionCncrPointModel;
    MaterialModel * materialModel;
    SectionFRP * currentSectionFRP;

    DoublePlus * bRect;
    DoublePlus * hRect;
    DoublePlus * NULS;
    DoublePlus * alphaULS;
    Point2DPlus * NCenULS;
    DoublePlus * MULSy;
    DoublePlus * MULSz;
    DoublePlus * MULSyMULSzKRed;

    DoublePlus * LSPlusNULS;
    DoublePlus * LSPlusMyULS;
    DoublePlus * LSPlusMzULS;
    Point2DPlus * LSPlusNCenULS;
    DoublePlus * LSPluslULS;
    DoublePlus * LSPlusmyULS;
    DoublePlus * LSPlusmzULS;

    DoublePlus * NSLS;
    Point2DPlus * NCenSLS;
    DoublePlus * MySLS;
    DoublePlus * MzSLS;

    DoublePlus * lSLS;
    DoublePlus * mySLS;
    DoublePlus * mzSLS;
    DoublePlus * fCncrMinSLS;
    DoublePlus * fSteelCncrMinSLS;
    DoublePlus * fSteelCncrMaxSLS;

    QList<QGraphicsPolygonItem *> * sectsToView;
};

SectionRCncrGUI::SectionRCncrGUI(UnitMeasure* ump, MaterialModel * matModel, const QString & settingsFile, QWidget *parent) :
    SectionGUI(ump, matModel, parent),
    m_d( new SectionRCncrGUIPrivate(ump, matModel, settingsFile )){

    m_d->ui->setupUi(this);

    m_d->connectVar( this );

    connect( m_d->ui->addSectionCncrPButton, &QPushButton::clicked, this, &SectionRCncrGUI::addSectionCncr );
    connect( m_d->ui->removeSectionCncrPButton, &QPushButton::clicked, this, &SectionRCncrGUI::removeSectionCncr );

    connect( m_d->ui->addSectCncrPointPButton, &QPushButton::clicked, this, &SectionRCncrGUI::addSectCncrPoint );
    connect( m_d->ui->removeSectCncrPointPButton, &QPushButton::clicked, this, &SectionRCncrGUI::removeSectCncrPoint );

    connect( m_d->ui->addSectionSteelCncrPButton, &QPushButton::clicked, this, &SectionRCncrGUI::addSectionSteelCncr );
    connect( m_d->ui->removeSectionSteelCncrPButton, &QPushButton::clicked, this, &SectionRCncrGUI::removeSectionSteelCncr );

    connect( m_d->ui->addSectionSteelPButton, &QPushButton::clicked, this, &SectionRCncrGUI::addSectionSteel );
    connect( m_d->ui->removeSectionSteelPButton, &QPushButton::clicked, this, &SectionRCncrGUI::removeSectionSteel );

    connect( m_d->ui->addSectionFRPPushButton, &QPushButton::clicked, this, &SectionRCncrGUI::addSectionFRP );
    connect( m_d->ui->removeSectionFRPPushButton, &QPushButton::clicked, this, &SectionRCncrGUI::removeSectionFRP );

    connect( m_d->ui->copyToClipBoardMULSyMULSzPButton, &QPushButton::clicked, this, &SectionRCncrGUI::copyToClipboardMULSyMULSz );
    connect( m_d->ui->copyToFileMULSyMULSzPButton, &QPushButton::clicked, this, &SectionRCncrGUI::copyToFileMULSyMULSz );
    connect( m_d->ui->plotMULSyMULSzPButton, &QPushButton::clicked, this, &SectionRCncrGUI::plotMULSyMULSz );
    connect( m_d->ui->copyToClipboardMULSNPButton, &QPushButton::clicked, this, &SectionRCncrGUI::copyToClipboardMULSN );
    connect( m_d->ui->copyToFileMULSNPButton, &QPushButton::clicked, this, &SectionRCncrGUI::copyToFileMULSN );
    connect( m_d->ui->plotMULSNPButton, &QPushButton::clicked, this, &SectionRCncrGUI::plotMULSN );
    connect( m_d->ui->SLSCalculatePButton, &QPushButton::clicked, this, &SectionRCncrGUI::SLSCalculate );

    connect( m_d->ui->SectionRectPushButton, &QPushButton::clicked, this, &SectionRCncrGUI::setSectionCncrRectangular );

    connect( m_d->NCenULS->y, &VarPlus::readOnlyChanged, this, &SectionRCncrGUI::setNCenULSY );
    connect( m_d->NCenULS->z, &VarPlus::readOnlyChanged, this, &SectionRCncrGUI::setNCenULSZ );

    connect( m_d->NULS, &VarPlus::valueChanged, this, &SectionRCncrGUI::calculateMULS );
    connect( m_d->alphaULS, &VarPlus::valueChanged, this, &SectionRCncrGUI::calculateMULS );
    connect( m_d->NCenULS, &VarPlus::valueChanged, this, &SectionRCncrGUI::calculateMULS );

    connect( m_d->NCenSLS->y, &VarPlus::readOnlyChanged, this, &SectionRCncrGUI::setNCenSLSY );
    connect( m_d->NCenSLS->z, &VarPlus::readOnlyChanged, this, &SectionRCncrGUI::setNCenSLSZ );

    connect( m_d->LSPlusNULS, &VarPlus::valueChanged, this, &SectionRCncrGUI::calculateNMULS );
    connect( m_d->LSPlusMyULS, &VarPlus::valueChanged, this, &SectionRCncrGUI::calculateNMULS );
    connect( m_d->LSPlusMzULS, &VarPlus::valueChanged, this, &SectionRCncrGUI::calculateNMULS );

    connect( m_d->LSPlusNCenULS->y, &VarPlus::readOnlyChanged, this, &SectionRCncrGUI::setYLSPlusNCenULS );
    connect( m_d->LSPlusNCenULS->z, &VarPlus::readOnlyChanged, this, &SectionRCncrGUI::setZLSPlusNCenULS );

    connect( matModel, &MaterialModel::modelChanged, this, &SectionRCncrGUI::FRPpopulateConcreteComboBox );
    connect( matModel, &MaterialModel::modelChanged, this, &SectionRCncrGUI::FRPpopulateFRPComboBox );

    connect( m_d->ui->LSPlusULSLoadStoryAddLoadPushButton, &QPushButton::clicked,
             this, &SectionRCncrGUI::LSPlusULSLoadStoryAddLoad );
    connect( m_d->ui->LSPlusULSLoadStoryRemoveLoadPushButton, &QPushButton::clicked,
             this, &SectionRCncrGUI::LSPlusULSLoadStoryRemoveLoad );
    connect( m_d->ui->LSPlusULSLoadStoryCalculatePushButton, &QPushButton::clicked,
             this, &SectionRCncrGUI::LSPlusULSLoadStoryCalculate );
}

SectionRCncrGUI::~SectionRCncrGUI() {
    delete m_d;
}

void SectionRCncrGUI::setSection(Section * s){
    m_d->geometricDataPanel->setSection( s );
    m_d->transformPanel->setSection( s );

    if( m_d->section ){
        {
            QList<QGraphicsPolygonItem *>::iterator iter = m_d->sectsToView->begin();
            while( iter != m_d->sectsToView->end() ){
                delete *iter;
                iter++;
            }
        }
        m_d->sectsToView->clear();

        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );

        m_d->ui->sectionsCncrTableView->setModel( nullptr );
        m_d->sectionsCncrMatDelegate->setLoadPhaseModel( nullptr );
        m_d->ui->sectionsSteelCncrTableView->setModel( nullptr );
        m_d->sectionsSteelCncrMatDelegate->setLoadPhaseModel( nullptr );
        m_d->ui->sectionsSteelTableView->setModel( nullptr );
        m_d->sectionsSteelMatDelegate->setLoadPhaseModel( nullptr );
        m_d->ui->sectionsFRPTableView->setModel( nullptr );
        m_d->sectionsFRPMatDelegate->setLoadPhaseModel( nullptr );
        m_d->ui->LSPlusULSLoadStoryTableView->setModel( nullptr );

        disconnect( m_d->ui->sectionsCncrTableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &SectionRCncrGUI::setCurrentSectionCncr );
        disconnect( m_d->ui->sectionsSteelTableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &SectionRCncrGUI::setCurrentSectionSteel );
        disconnect( m_d->ui->sectionsFRPTableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
                    this, static_cast<void (SectionRCncrGUI::*)(const QModelIndex &, const QModelIndex &)>(&SectionRCncrGUI::setCurrentSectionFRP) );
        disconnect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::calculateMULS );
        disconnect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setNCenULSY );
        disconnect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setNCenULSZ );
        disconnect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setYLSPlusNCenULS );
        disconnect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setZLSPlusNCenULS );
        disconnect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setNCenSLSY );
        disconnect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setNCenSLSZ );
    }

    m_d->section = dynamic_cast<SectionRCncr *>(s);
    setCurrentSectionFRP( nullptr );

    if( m_d->section ){
        VarPlusGUI::connectVar( m_d->section->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar( m_d->section->pW, m_d->ui->pWLabel, m_d->ui->pWLEdit, m_d->ui->pWUMLabel, m_d->ui->pWCheckBox );

        m_d->ui->sectionsCncrTableView->setModel( m_d->section->sectionsCncr() );
        m_d->sectionsCncrMatDelegate->setLoadPhaseModel( m_d->section->loadPhaseModel() );
        m_d->ui->sectionsSteelCncrTableView->setModel( m_d->section->sectionsSteelCncr() );
        m_d->sectionsSteelCncrMatDelegate->setLoadPhaseModel( m_d->section->loadPhaseModel() );
        m_d->ui->sectionsSteelTableView->setModel( m_d->section->sectionsSteel() );
        m_d->sectionsSteelMatDelegate->setLoadPhaseModel( m_d->section->loadPhaseModel() );
        m_d->ui->sectionsFRPTableView->setModel( m_d->section->sectionsFRP() );
        m_d->sectionsFRPMatDelegate->setLoadPhaseModel( m_d->section->loadPhaseModel() );

        m_d->ui->LSPlusULSLoadStoryTableView->setModel( m_d->section->loadPhaseModel() );

        connect( m_d->ui->sectionsCncrTableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &SectionRCncrGUI::setCurrentSectionCncr );
        connect( m_d->ui->sectionsSteelTableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &SectionRCncrGUI::setCurrentSectionSteel );
        connect( m_d->ui->sectionsFRPTableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
                 this, static_cast<void (SectionRCncrGUI::*)(const QModelIndex &, const QModelIndex &)>(&SectionRCncrGUI::setCurrentSectionFRP) );
        connect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::calculateMULS );
        connect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setNCenULSY );
        connect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setNCenULSZ );
        connect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setNCenSLSY );
        connect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setNCenSLSZ );
        connect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setYLSPlusNCenULS );
        connect( m_d->section, &Section::sectionChanged, this, &SectionRCncrGUI::setZLSPlusNCenULS );

        calculateMULS();
        calculateNMULS();
    }

    setNCenULSY();
    setNCenULSZ();
    setYLSPlusNCenULS();
    setZLSPlusNCenULS();
    setNCenSLSY();
    setNCenSLSZ();
}

Material *SectionRCncrGUI::material() {
    return nullptr;
}

void SectionRCncrGUI::addSectionCncr(){
    if( m_d->section ){
        QModelIndexList listRows = m_d->ui->sectionsCncrTableView->selectionModel()->selectedRows();
        if( listRows.size() > 0 ){
            m_d->section->sectionsCncr()->insertRows( listRows.last().row()+1, listRows.size() );
        } else {
            int row = m_d->ui->sectionsCncrTableView->currentIndex().row();
            if( row < 0 || row > m_d->section->sectionsCncr()->count() )
                row = m_d->section->sectionsCncr()->count() - 1 ;
            m_d->section->sectionsCncr()->insertRows( row + 1 );
        }
    }
}

void SectionRCncrGUI::removeSectionCncr(){
    if( m_d->section ){
        QModelIndexList listRows = m_d->ui->sectionsCncrTableView->selectionModel()->selectedRows();
        if( listRows.size() > 0 ){
            m_d->section->sectionsCncr()->removeRows( listRows.first().row(), listRows.size());
        }
    }
}

void SectionRCncrGUI::addSectCncrPoint(){
    if( m_d->currentSectionCncrPointModel ){
        QModelIndexList listRows = m_d->ui->sectCncrPointsTableView->selectionModel()->selectedRows();
        if( listRows.size() > 0 ){
            m_d->currentSectionCncrPointModel->insertRows( listRows.last().row()+1, listRows.size() );
        } else {
            int row = m_d->ui->sectCncrPointsTableView->currentIndex().row();
            if( row < 0 || row > m_d->currentSectionCncrPointModel->pCount()-1 )
                row = m_d->currentSectionCncrPointModel->pCount() - 1 ;
            m_d->currentSectionCncrPointModel->insertRows(row + 1, 1 );
        }
    }
}

void SectionRCncrGUI::removeSectCncrPoint(){
    if( m_d->currentSectionCncrPointModel ){
        QModelIndexList listRows = m_d->ui->sectCncrPointsTableView->selectionModel()->selectedRows();
        if( listRows.size() > 0 ){
            m_d->currentSectionCncrPointModel->removeRows( listRows.first().row(), listRows.size());
        }
    }
}

void SectionRCncrGUI::addSectionSteelCncr(){
    QModelIndexList listRows = m_d->ui->sectionsSteelCncrTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->sectionsSteelCncr()->insertRows( listRows.last().row()+1, listRows.size() );
    } else {
        int row = m_d->ui->sectionsSteelCncrTableView->currentIndex().row();
        if( row < 0 || row > m_d->section->sectionsSteelCncr()->count() )
            row = m_d->section->sectionsSteelCncr()->count() - 1 ;
        m_d->section->sectionsSteelCncr()->insertRows( row + 1 );
    }
}

void SectionRCncrGUI::removeSectionSteelCncr(){
    QModelIndexList listRows = m_d->ui->sectionsSteelCncrTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->sectionsSteelCncr()->removeRows( listRows.first().row(), listRows.size());
    }
}

void SectionRCncrGUI::addSectionSteel(){
    QModelIndexList listRows = m_d->ui->sectionsSteelTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->sectionsSteel()->insertRows( listRows.last().row()+1, listRows.size() );
    } else {
        int row = m_d->ui->sectionsSteelTableView->currentIndex().row();
        if( row < 0 || row > m_d->section->sectionsSteel()->count() )
            row = m_d->section->sectionsSteel()->count() - 1 ;
        m_d->section->sectionsSteel()->insertRows( row + 1 );
    }
}

void SectionRCncrGUI::removeSectionSteel(){
    QModelIndexList listRows = m_d->ui->sectionsSteelTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->sectionsSteel()->removeRows( listRows.first().row(), listRows.size());
    }
}

void SectionRCncrGUI::addSectionFRP(){
    QModelIndexList listRows = m_d->ui->sectionsFRPTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->sectionsFRP()->insertRows( listRows.last().row()+1, listRows.size() );
    } else {
        int row = m_d->ui->sectionsFRPTableView->currentIndex().row();
        if( row < 0 || row > m_d->section->sectionsFRP()->count() )
            row = m_d->section->sectionsFRP()->count() - 1 ;
        m_d->section->sectionsFRP()->insertRows( row + 1 );
    }
}

void SectionRCncrGUI::removeSectionFRP(){
    QModelIndexList listRows = m_d->ui->sectionsFRPTableView->selectionModel()->selectedRows();
    if( listRows.size() > 0 ){
        m_d->section->sectionsFRP()->removeRows( listRows.first().row(), listRows.size());
    }
}

void SectionRCncrGUI::setCurrentSectionCncr( const QModelIndex & current, const QModelIndex & previous){
    Q_UNUSED(previous);
    if( current.row()>= 0 && current.row() < m_d->section->sectionsCncr()->count() ){
        if( m_d->currentSectionCncrPointModel != m_d->section->sectionsCncr()->sectionCncr(current.row())->pointsModel() ){
            m_d->currentSectionCncrPointModel = m_d->section->sectionsCncr()->sectionCncr(current.row())->pointsModel();
        }
    } else {
        m_d->currentSectionCncrPointModel = nullptr;
    }
    m_d->ui->sectCncrPointsTableView->setModel( m_d->currentSectionCncrPointModel );
    m_d->ui->sectCncrPointsTabWidget->setEnabled( m_d->currentSectionCncrPointModel != nullptr );
}

void SectionRCncrGUI::setCurrentSectionSteel( const QModelIndex & current, const QModelIndex & previous){
    Q_UNUSED(previous);
    if( current.row()>= 0 && current.row() < m_d->section->sectionsSteel()->count() ){
        if( m_d->sectionSteelGUI->sectionSteel() != m_d->section->sectionsSteel()->sectionSteel(current.row()) ){
            m_d->sectionSteelGUI->setSection( m_d->section->sectionsSteel()->sectionSteel(current.row()) );
        }
    }
}

void SectionRCncrGUI::setCurrentSectionFRPNULL(){
    setCurrentSectionFRP( nullptr);
}

void SectionRCncrGUI::setCurrentSectionFRP( SectionFRP * sectFRP ){
    if( m_d->currentSectionFRP != nullptr ){
        VarPlusGUI::disconnectVar( m_d->ui->FRPNameLabel, m_d->ui->FRPNameLineEdit );
        VarPlusGUI::disconnectVar( m_d->ui->FRPtfLabel, m_d->ui->FRPtfLineEdit, m_d->ui->FRPtfUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->FRPy1Label, m_d->ui->FRPy1LineEdit, m_d->ui->FRPy1UMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->FRPz1Label, m_d->ui->FRPz1LineEdit, m_d->ui->FRPz1UMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->FRPy2Label, m_d->ui->FRPy2LineEdit, m_d->ui->FRPy2UMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->FRPz2Label, m_d->ui->FRPz2LineEdit, m_d->ui->FRPz2UMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->FRPledLabel, m_d->ui->FRPledLineEdit, m_d->ui->FRPledUMLabel, m_d->ui->FRPledCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FRPbfLabel, m_d->ui->FRPbfLineEdit, m_d->ui->FRPbfUMLabel, m_d->ui->FRPbfCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FRPbLabel, m_d->ui->FRPbLineEdit, m_d->ui->FRPbUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->FRPkbLabel, m_d->ui->FRPkbLineEdit, m_d->ui->FRPkbUMLabel, m_d->ui->FRPkbCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FRPkqLabel, m_d->ui->FRPkqLineEdit, m_d->ui->FRPkqUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->FRPkGLabel, m_d->ui->FRPkGLineEdit, m_d->ui->FRPkGUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->FRPkG2Label, m_d->ui->FRPkG2LineEdit, m_d->ui->FRPkG2UMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->FRPfbdLabel, m_d->ui->FRPfbdLineEdit, m_d->ui->FRPfbdUMLabel, m_d->ui->FRPfbdCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FRPLFdLabel, m_d->ui->FRPLFdLineEdit, m_d->ui->FRPLFdUMLabel, m_d->ui->FRPLFdCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FRPffddLabel, m_d->ui->FRPffddLineEdit, m_d->ui->FRPffddUMLabel, m_d->ui->FRPffddCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FRPffdd2Label, m_d->ui->FRPffdd2LineEdit, m_d->ui->FRPffdd2UMLabel, m_d->ui->FRPffdd2CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FRPepsfddLabel, m_d->ui->FRPepsfddLineEdit, m_d->ui->FRPepsfddUMLabel, m_d->ui->FRPepsfddLabelCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FRPepsfdLabel, m_d->ui->FRPepsfdLineEdit, m_d->ui->FRPepsfdUMLabel, m_d->ui->FRPepsfdLabelCheckBox );
        disconnect( m_d->currentSectionFRP, &SectionFRP::materialChanged, this, &SectionRCncrGUI::FRPpopulateFRPComboBox );
        disconnect( m_d->currentSectionFRP, &SectionFRP::concreteChanged, this, &SectionRCncrGUI::FRPpopulateConcreteComboBox);
        disconnect( m_d->currentSectionFRP, &SectionFRP::aboutToBeDeleted, this, &SectionRCncrGUI::setCurrentSectionFRPNULL );
    }

    m_d->currentSectionFRP = sectFRP;

    if( m_d->currentSectionFRP != nullptr ){
        VarPlusGUI::connectVar( m_d->currentSectionFRP->name, m_d->ui->FRPNameLabel, m_d->ui->FRPNameLineEdit );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->t, m_d->ui->FRPtfLabel, m_d->ui->FRPtfLineEdit, m_d->ui->FRPtfUMLabel );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->P1->y, m_d->ui->FRPy1Label, m_d->ui->FRPy1LineEdit, m_d->ui->FRPy1UMLabel );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->P1->z, m_d->ui->FRPz1Label, m_d->ui->FRPz1LineEdit, m_d->ui->FRPz1UMLabel );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->P2->y, m_d->ui->FRPy2Label, m_d->ui->FRPy2LineEdit, m_d->ui->FRPy2UMLabel );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->P2->z, m_d->ui->FRPz2Label, m_d->ui->FRPz2LineEdit, m_d->ui->FRPz2UMLabel );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->led, m_d->ui->FRPledLabel, m_d->ui->FRPledLineEdit, m_d->ui->FRPledUMLabel, m_d->ui->FRPledCheckBox );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->L, m_d->ui->FRPbfLabel, m_d->ui->FRPbfLineEdit, m_d->ui->FRPbfUMLabel, m_d->ui->FRPbfCheckBox );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->b, m_d->ui->FRPbLabel, m_d->ui->FRPbLineEdit, m_d->ui->FRPbUMLabel );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->kb, m_d->ui->FRPkbLabel, m_d->ui->FRPkbLineEdit, m_d->ui->FRPkbUMLabel, m_d->ui->FRPkbCheckBox );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->kq, m_d->ui->FRPkqLabel, m_d->ui->FRPkqLineEdit, m_d->ui->FRPkqUMLabel );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->kG, m_d->ui->FRPkGLabel, m_d->ui->FRPkGLineEdit, m_d->ui->FRPkGUMLabel );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->kG2, m_d->ui->FRPkG2Label, m_d->ui->FRPkG2LineEdit, m_d->ui->FRPkG2UMLabel );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->fbd, m_d->ui->FRPfbdLabel, m_d->ui->FRPfbdLineEdit, m_d->ui->FRPfbdUMLabel, m_d->ui->FRPfbdCheckBox );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->LFd, m_d->ui->FRPLFdLabel, m_d->ui->FRPLFdLineEdit, m_d->ui->FRPLFdUMLabel, m_d->ui->FRPLFdCheckBox );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->ffdd, m_d->ui->FRPffddLabel, m_d->ui->FRPffddLineEdit, m_d->ui->FRPffddUMLabel, m_d->ui->FRPffddCheckBox );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->ffdd2, m_d->ui->FRPffdd2Label, m_d->ui->FRPffdd2LineEdit, m_d->ui->FRPffdd2UMLabel, m_d->ui->FRPffdd2CheckBox );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->epsfdd, m_d->ui->FRPepsfddLabel, m_d->ui->FRPepsfddLineEdit, m_d->ui->FRPepsfddUMLabel, m_d->ui->FRPepsfddLabelCheckBox );
        VarPlusGUI::connectVar( m_d->currentSectionFRP->epsfd, m_d->ui->FRPepsfdLabel, m_d->ui->FRPepsfdLineEdit, m_d->ui->FRPepsfdUMLabel, m_d->ui->FRPepsfdLabelCheckBox );
        connect( m_d->currentSectionFRP, &SectionFRP::materialChanged, this, &SectionRCncrGUI::FRPpopulateFRPComboBox );
        connect( m_d->currentSectionFRP, &SectionFRP::concreteChanged, this, &SectionRCncrGUI::FRPpopulateConcreteComboBox );
        connect( m_d->currentSectionFRP, &SectionFRP::aboutToBeDeleted, this, &SectionRCncrGUI::setCurrentSectionFRPNULL );
    }
    FRPpopulateFRPComboBox();
    FRPpopulateConcreteComboBox();
}

void SectionRCncrGUI::setCurrentSectionFRP( const QModelIndex & current, const QModelIndex & previous){
    Q_UNUSED(previous);
    setCurrentSectionFRP( m_d->section->sectionsFRP()->sectionFRP( current.row() ) );
}

void SectionRCncrGUI::FRPpopulateConcreteComboBox() {
    if( m_d->currentSectionFRP ){
        disconnect( m_d->ui->FRPconcreteComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionRCncrGUI::FRPsetConcreteFromCombobox );

        QList< Material * > matList = m_materialModel->materialsList( MaterialModel::ConcreteMaterial );
        m_d->ui->FRPconcreteComboBox->clear();
        for( QList< Material * >::iterator i=matList.begin(); i!=matList.end(); ++i ){
            m_d->ui->FRPconcreteComboBox->addItem( (*i)->name->valueStr(), qVariantFromValue((void *) (*i) ) );
        }
        if( m_d->currentSectionFRP->concrete() != nullptr ){
            m_d->ui->FRPconcreteComboBox->setCurrentIndex( m_d->ui->FRPconcreteComboBox->findData( qVariantFromValue((void *) m_d->currentSectionFRP->concrete() )));
        } else {
            m_d->ui->FRPconcreteComboBox->setCurrentIndex( -1 );
        }
        connect( m_d->ui->FRPconcreteComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionRCncrGUI::FRPsetConcreteFromCombobox );
    }
}

void SectionRCncrGUI::FRPsetConcreteFromCombobox(){
    if( m_d->currentSectionFRP != nullptr ){
        m_d->currentSectionFRP->setConcrete( (Concrete *) (m_d->ui->FRPconcreteComboBox->currentData().value<void *>()) );
    }
}

void SectionRCncrGUI::FRPpopulateFRPComboBox() {
    if( m_d->currentSectionFRP ){
        disconnect( m_d->ui->FRPFRPComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionRCncrGUI::FRPsetFRPFromCombobox );

        QList< Material * > matList = m_materialModel->materialsList( MaterialModel::FRPMaterial );
        m_d->ui->FRPFRPComboBox->clear();
        for( QList< Material * >::iterator i=matList.begin(); i!=matList.end(); ++i ){
            m_d->ui->FRPFRPComboBox->addItem( (*i)->name->valueStr(), qVariantFromValue((void *) (*i) ) );
        }
        if( m_d->currentSectionFRP->material() != nullptr ){
            m_d->ui->FRPFRPComboBox->setCurrentIndex( m_d->ui->FRPFRPComboBox->findData( qVariantFromValue((void *) m_d->currentSectionFRP->material() )));
        } else {
            m_d->ui->FRPFRPComboBox->setCurrentIndex( -1 );
        }
        connect( m_d->ui->FRPFRPComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionRCncrGUI::FRPsetConcreteFromCombobox );
    }
}

void SectionRCncrGUI::FRPsetFRPFromCombobox(){
    if( m_d->currentSectionFRP != nullptr ){
        m_d->currentSectionFRP->setFRP( (FRP *) (m_d->ui->FRPFRPComboBox->currentData().value<void *>()) );
    }
}

void SectionRCncrGUI::setSectionCncrRectangular(){
    if( m_d->section ){
        int currRow = m_d->ui->sectionsCncrTableView->selectionModel()->currentIndex().row();
        if( currRow > -1 && currRow < m_d->section->sectionsCncr()->count() ){
            SectionCncr * currSection = m_d->section->sectionsCncr()->sectionCncr( currRow );
            currSection->setRectangular( m_d->bRect, m_d->hRect );
        }
    }
}

void SectionRCncrGUI::setNCenSLSY(){
    if( m_d->section ){
        if( m_d->NCenSLS->y->readOnly() ){
            m_d->NCenSLS->y->setValueNormal( m_d->section->GCncr->y->valueNormal() );
        }
    } else {
        if( m_d->NCenSLS->y->readOnly() ){
            m_d->NCenSLS->y->setValueNormal( 0.0 );
        }
    }
}

void SectionRCncrGUI::setNCenSLSZ(){
    if( m_d->section ){
        if( m_d->NCenSLS->z->readOnly() ){
            m_d->NCenSLS->z->setValueNormal( m_d->section->GCncr->z->valueNormal() );
        }
    } else {
        if( m_d->NCenSLS->z->readOnly() ){
            m_d->NCenSLS->z->setValueNormal( 0.0 );
        }
    }
}

void SectionRCncrGUI::setNCenULSY(){
    if( m_d->section ){
        if( m_d->NCenULS->y->readOnly() ){
            m_d->NCenULS->y->setValueNormal( m_d->section->GCncr->y->valueNormal() );
        }
    } else {
        if( m_d->NCenULS->y->readOnly() ){
            m_d->NCenULS->y->setValueNormal( 0.0 );
        }
    }
}

void SectionRCncrGUI::setNCenULSZ(){
    if( m_d->section ){
        if( m_d->NCenULS->z->readOnly() ){
            m_d->NCenULS->z->setValueNormal( m_d->section->GCncr->z->valueNormal() );
        }
    } else {
        if( m_d->NCenULS->z->readOnly() ){
            m_d->NCenULS->z->setValueNormal( 0.0 );
        }
    }
}

void SectionRCncrGUI::setYLSPlusNCenULS(){
    if( m_d->section ){
        if( m_d->LSPlusNCenULS->y->readOnly() ){
            m_d->NCenULS->y->setValueNormal( m_d->section->GCncr->y->valueNormal() );
        }
    } else {
        if( m_d->LSPlusNCenULS->y->readOnly() ){
            m_d->NCenULS->y->setValueNormal( 0.0 );
        }
    }
}

void SectionRCncrGUI::setZLSPlusNCenULS(){
    if( m_d->section ){
        if( m_d->LSPlusNCenULS->z->readOnly() ){
            m_d->LSPlusNCenULS->z->setValueNormal( m_d->section->GCncr->z->valueNormal() );
        }
    } else {
        if( m_d->LSPlusNCenULS->z->readOnly() ){
            m_d->LSPlusNCenULS->z->setValueNormal( 0.0 );
        }
    }
}

void SectionRCncrGUI::calculateMULS(){
    if( m_d->section ){
        {
            QList<QGraphicsPolygonItem *>::iterator iter = m_d->sectsToView->begin();
            while( iter != m_d->sectsToView->end() ){
                delete *iter;
                iter++;
            }
        }
        m_d->sectsToView->clear();

        QList<Point2DModel *> * pointsModel = new QList<Point2DModel *>;

        m_d->section->MULS( m_d->MULSy, m_d->MULSz, m_d->NULS, m_d->NCenULS, m_d->alphaULS, pointsModel );

        QList<Point2DModel *>::iterator iter = pointsModel->begin();
        while( iter != pointsModel->end() ){
            QGraphicsPolygonItem * line = new QGraphicsPolygonItem( (*iter)->qPolygonF( m_d->unitMeasure()->scale(UnitMeasure::sectL)), m_graphicsItem  );
            QPen p = line->pen();
            p.setColor( Qt::red);
            line->setPen(p);
            m_d->sectsToView->append( line );
            delete *iter;
            iter++;
        }
        delete pointsModel;
    }
}

void SectionRCncrGUI::calculateNMULS(){
    if( m_d->section ){
        m_d->section->NMULS( m_d->LSPluslULS, m_d->LSPlusmyULS, m_d->LSPlusmzULS,
                             m_d->LSPlusNULS, m_d->LSPlusMyULS, m_d->LSPlusMzULS, m_d->LSPlusNCenULS );
    }
}

void SectionRCncrGUI::copyToClipboardMULSyMULSz(){
    if( m_d->section ){
        QPolygonF data = m_d->section->MULSyMULSz(m_d->NULS, m_d->NCenULS, m_d->ui->MULSyMULSzAccuracySBox->value());
        QString text;
        for( QPolygonF::iterator i = data.begin(); i != data.end(); ++i ){
            text += QString::number( i->x()*m_d->MULSyMULSzKRed->valueNormal() ) + QString("\t") + QString::number( i->y()*m_d->MULSyMULSzKRed->valueNormal() ) + QString("\n");
        }
        QApplication::clipboard()->setText( text);
    }
}

void SectionRCncrGUI::copyToFileMULSyMULSz(){
    if( m_d->section ){
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        trUtf8("Salva su file TXT"), ".",
                                                        trUtf8("File TXT(*.txt)"));
        if (fileName.isEmpty()){
            return;
        }
        QFile file( fileName );
        file.open( QIODevice::WriteOnly );
        if( !file.isOpen() ){
            return;
        }
        QTextStream stream( &file );

        QPolygonF data = m_d->section->MULSyMULSz(m_d->NULS, m_d->NCenULS, m_d->ui->MULSyMULSzAccuracySBox->value());
        for( QPolygonF::iterator i = data.begin(); i != data.end(); ++i ){
            stream << QString::number( i->x()*m_d->MULSyMULSzKRed->valueNormal() ) + QString("\t") + QString::number( i->y()*m_d->MULSyMULSzKRed->valueNormal() ) + QString("\n");
        }

        file.close();
    }
}

void SectionRCncrGUI::plotMULSyMULSz(){
    if( m_d->section ){
        QCustomPlotDialog d( trUtf8("Interazione My-Mz"), this );
        QPolygonF plotData = m_d->section->MULSyMULSz(m_d->NULS, m_d->NCenULS, m_d->ui->MULSyMULSzAccuracySBox->value());
        for( QPolygonF::iterator i = plotData.begin(); i != plotData.end(); ++i ){
            (*i) *= m_d->MULSyMULSzKRed->valueNormal();
        }
        d.setCurveData( 0, plotData );
        d.exec();
    }
}

void SectionRCncrGUI::copyToClipboardMULSN(){
    if( m_d->section ){
        QPolygonF data = m_d->section->MULSN( m_d->NCenULS, m_d->alphaULS, m_d->ui->MULSNAccuracySBox->value());
        QString text;
        for( QPolygonF::iterator i = data.begin(); i != data.end(); ++i ){
            text += QString::number( i->x() ) + QString("\t") + QString::number( i->y() ) + QString("\n");
        }
        QApplication::clipboard()->setText( text);
    }
}

void SectionRCncrGUI::copyToFileMULSN(){
    if( m_d->section ){
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        trUtf8("Salva su file TXT"), ".",
                                                        trUtf8("File TXT(*.txt)"));
        if (fileName.isEmpty()){
            return;
        }
        QFile file( fileName );
        file.open( QIODevice::WriteOnly );
        if( !file.isOpen() ){
            return;
        }
        QTextStream stream( &file );

        QPolygonF data = m_d->section->MULSN( m_d->NCenULS, m_d->alphaULS, m_d->ui->MULSNAccuracySBox->value());
        for( QPolygonF::iterator i = data.begin(); i != data.end(); ++i ){
            stream << QString::number( i->x() ) + QString("\t") + QString::number( i->y() ) + QString("\n");
        }

        file.close();
    }
}

void SectionRCncrGUI::plotMULSN(){
    if( m_d->section ){
        QCustomPlotDialog d( trUtf8("Interazione N-M"), this );
        d.setCurveData( 0, m_d->section->MULSN( m_d->NCenULS, m_d->alphaULS, m_d->ui->MULSNAccuracySBox->value()) );
        d.setXAxisInverted( true );
        d.exec();
    }
}

void SectionRCncrGUI::SLSCalculate(){
    if( m_d->section ){
        m_d->section->NMSLS( m_d->lSLS, m_d->mySLS, m_d->mzSLS, m_d->NSLS, m_d->MySLS, m_d->MzSLS, m_d->NCenSLS );
        QList<double> fMinCncr;
        QList<double> fSteelCncr;
        QList<double> fMinSteel;
        QList<double> fMaxSteel;
        QList<double> fMinFRP;
        QList<double> fMaxFRP;
        m_d->section->fSLSNormal( &fMinCncr, &fSteelCncr, &fMinSteel, &fMaxSteel, &fMinFRP, &fMaxFRP,
                                  m_d->lSLS->valueNormal(), m_d->mySLS->valueNormal(), m_d->mzSLS->valueNormal() );
        m_d->fCncrMinSLS->setValueNormal( *std::min_element(fMinCncr.begin(), fMinCncr.end()) );
        m_d->fSteelCncrMaxSLS->setValueNormal( *std::max_element(fSteelCncr.begin(), fSteelCncr.end()) );
        m_d->fSteelCncrMinSLS->setValueNormal( *std::min_element(fSteelCncr.begin(), fSteelCncr.end()) );
    }
}

void SectionRCncrGUI::setMaterialFromComboBox() {
    ;
}

void SectionRCncrGUI::LSPlusULSLoadStoryAddLoad () {
    if( m_d->ui->LSPlusULSLoadStoryTableView->selectionModel() ) {
        QModelIndexList listRows = m_d->ui->LSPlusULSLoadStoryTableView->selectionModel()->selectedRows();
        if( listRows.size() > 0 ){
            m_d->section->loadPhaseModel()->insertRows( listRows.last().row()+1, listRows.size() );
        } else {
            int row = m_d->ui->LSPlusULSLoadStoryTableView->currentIndex().row();
            if( row < 0 || row > m_d->section->loadPhaseModel()->size() ) {
                row = m_d->section->loadPhaseModel()->size();
            }
            m_d->section->loadPhaseModel()->insertRows( row );
        }
    }
}

void SectionRCncrGUI::LSPlusULSLoadStoryRemoveLoad () {
    if( m_d->ui->LSPlusULSLoadStoryTableView->selectionModel() ){
        QModelIndexList listRows = m_d->ui->LSPlusULSLoadStoryTableView->selectionModel()->selectedRows();
        if( listRows.size() > 0 ){
            m_d->section->loadPhaseModel()->removeRows( listRows.first().row(), listRows.size());
        }
    }
}

void SectionRCncrGUI::LSPlusULSLoadStoryCalculate () {
    if( m_d->section ){
        m_d->section->loadPhaseModel()->calculate( m_d->section );
    }
}
