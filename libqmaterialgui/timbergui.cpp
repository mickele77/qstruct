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
#include "timbergui.h"
#include "ui_timbergui.h"

#include <QtSql>
#include <QMessageBox>

#include "timber.h"
#include "serviceclass.h"
#include "loaddurationclass.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "boolplus.h"
#include "varplusguiutility.h"

class TimberGUIPrivate{
public:
    TimberGUIPrivate(){
        timber = NULL;
        dbTimberName = "timberDB";
    };
    ~TimberGUIPrivate(){
        delete ui;
    };

    QString settingsFile;
    QString dbTimberFileName;
    QString dbTimberName;

    Ui::TimberGUI * ui;

    Timber * timber;

    ServiceClass * sClass;
    LoadDurationClass * ldClass;
    LSType * lsType;

    SectionTimberShape * sectionShape;
    DoublePlus * b;
    DoublePlus * h;

    DoublePlus * fc0d;
    DoublePlus * fc90d;
    DoublePlus * ft0d;
    DoublePlus * ft90d;
    DoublePlus * fmdh;
    DoublePlus * fmdb;
    DoublePlus * fvd;
    DoublePlus * ftorsd;

    DoublePlus * kh;
    DoublePlus * V;
    DoublePlus * kV;

    DoublePlus * km;
    DoublePlus * khm;
    DoublePlus * kbm;

    DoublePlus * ksh;
};

TimberGUI::TimberGUI(UnitMeasure * um, const QString & settingsFile, QWidget *parent) :
    MaterialGUI(um, parent),
    m_d( new TimberGUIPrivate ){

    m_d->settingsFile = settingsFile;

    // Imposta la GUI
    m_d->ui = new Ui::TimberGUI;
    m_d->ui->setupUi(this);

    // Apre il database
    if( !QSqlDatabase::contains( m_d->dbTimberName )){
        QSqlDatabase dbTimber = QSqlDatabase::addDatabase("QSQLITE", m_d->dbTimberName );
        QSettings settings( m_d->settingsFile, QSettings::IniFormat );
        m_d->dbTimberFileName = settings.value("dbTimberFileName", QApplication::applicationDirPath() + "/../resources/timber.db").toString();
        dbTimber.setDatabaseName(m_d->dbTimberFileName);
        if (!dbTimber.open() ) {
            QMessageBox::critical(0, tr("Non riesco ad aprire il database dei legni"),
                                  tr("Non riesco a trovare il file timber.db"), QMessageBox::Cancel);
        } else {
            loadStandardComboBox();
        }
    } else {
        loadStandardComboBox();
    }

    initVar();
}

void TimberGUI::loadStandardComboBox(){
    QSqlQueryModel queryModel;
    QString queryString = QString( "SELECT idType1, nameType1 from type1" );
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbTimberName ) );
    m_d->ui->type1FilterStandardComboBox->insertItem(0, QString("------"), QVariant(-1) );
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant data = queryModel.record(i).value("idType1");
        QString name = queryModel.record(i).value("nameType1").toString();
        m_d->ui->type1FilterStandardComboBox->insertItem((i+1), name, data );
    }
    connect( m_d->ui->type1FilterStandardComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateType2FilterStandardComboBox()) );
    updateType2FilterStandardComboBox();
    connect( m_d->ui->type1FilterStandardComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWoodNameStandardComboBox()) );
    connect( m_d->ui->type2FilterStandardComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWoodNameStandardComboBox()) );
    updateWoodNameStandardComboBox();
    connect( m_d->ui->loadStandardPButton, SIGNAL(clicked()), this, SLOT(loadStandardData()));
}

TimberGUI::~TimberGUI()
{
    QSqlDatabase dbTimber = QSqlDatabase::database( m_d->dbTimberName );
    dbTimber.close();

    QSettings settings(m_d->settingsFile,  QSettings::IniFormat);
    settings.setValue("dbTimberFileName", m_d->dbTimberFileName);

    delete m_d;
}

void TimberGUI::initVar(){
    m_d->sClass = new ServiceClass(ServiceClass::SC1, "ServiceClass" );
    m_d->sClass->setRichName(trUtf8("Classe di servizio"));
    addVarToContainer( m_d->sClass );
    VarPlusGUI::connectEnumVar(  m_d->sClass, m_d->ui->sClassLabel, m_d->ui->sClassComboBox );
    connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(setCurrentKmod()));

    m_d->ldClass = new LoadDurationClass(LoadDurationClass::permanent, "LoadDurationClass" );
    m_d->ldClass->setRichName(trUtf8("Durata del carico"));
    addVarToContainer( m_d->ldClass );
    VarPlusGUI::connectEnumVar( m_d->ldClass, m_d->ui->ldClassLabel, m_d->ui->ldClassComboBox );
    connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(setCurrentKmod()));

    m_d->lsType = new LSType(LSType::ULSFundamental, "LSType");
    m_d->lsType->setRichName(trUtf8("Combinazione"));
    addVarToContainer( m_d->lsType );
    VarPlusGUI::connectEnumVar( m_d->lsType, m_d->ui->lsTypeLabel, m_d->ui->lsTypeComboBox );
    connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(setCurrentGammaM()));

    m_d->sectionShape = new SectionTimberShape(SectionTimber::rectangular, "SectionShape" );
    m_d->sectionShape->setRichName(trUtf8("Tipologia di sezione"));
    addVarToContainer( m_d->sectionShape );
    VarPlusGUI::connectEnumVar( m_d->sectionShape, m_d->ui->sectionTypeLabel, m_d->ui->sectionTypeComboBox );
    connect( m_d->sectionShape, SIGNAL(valueChanged(QString)), this, SLOT(updateSectionData()));

    m_d->b = new DoublePlus(0.0, "b", m_unitMeasure, UnitMeasure::sectL );
    m_d->b->setRichName(trUtf8("b"));
    addVarToContainer( m_d->b );
    VarPlusGUI::connectVar( m_d->b, m_d->ui->bLabel, m_d->ui->bLEdit, m_d->ui->bUMLabel );

    m_d->h = new DoublePlus(0.0, "b", m_unitMeasure, UnitMeasure::sectL );
    m_d->h->setRichName(trUtf8("h"));
    addVarToContainer( m_d->h );
    VarPlusGUI::connectVar( m_d->h, m_d->ui->hLabel, m_d->ui->hLEdit, m_d->ui->hUMLabel );

    m_d->V = new DoublePlus(0.0, "V", m_unitMeasure, UnitMeasure::sectL3 );
    m_d->V->setRichName(trUtf8("V"));
    addVarToContainer( m_d->V );
    VarPlusGUI::connectVar( m_d->V, m_d->ui->VLabel, m_d->ui->VLEdit, m_d->ui->VUMLabel );

    m_d->khm = new DoublePlus(0.0, "khm", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->khm->setRichName(trUtf8("k<span style=\" vertical-align:sub;\">h</span>"));
    m_d->khm->setToolTip("Coefficiente amplificativo per sforzi normali di trazione paralleli alle fibre - flessione nel piano parallelo ad h");
    addVarToContainer( m_d->khm );
    VarPlusGUI::connectVar( m_d->khm, m_d->ui->khmLabel, m_d->ui->khmLEdit, m_d->ui->khmUMLabel, m_d->ui->khmCheckBox );
    connect( m_d->h, SIGNAL(valueChanged(QString)), this, SLOT(updatekhm()) );
    connect( m_d->khm, SIGNAL(readOnlyChanged(bool)), this, SLOT(updatekhm()) );

    m_d->kbm = new DoublePlus(0.0, "kbm", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->kbm->setRichName(trUtf8("k<span style=\" vertical-align:sub;\">b</span>"));
    m_d->kbm->setToolTip("Coefficiente amplificativo per sforzi normali di trazione paralleli alle fibre - flessione nel piano parallelo a b");
    addVarToContainer( m_d->kbm );
    VarPlusGUI::connectVar( m_d->kbm, m_d->ui->kbmLabel, m_d->ui->kbmLEdit, m_d->ui->kbmUMLabel, m_d->ui->kbmCheckBox );
    connect( m_d->b, SIGNAL(valueChanged(QString)), this, SLOT(updatekbm()) );
    connect( m_d->kbm, SIGNAL(readOnlyChanged(bool)), this, SLOT(updatekbm()) );

    m_d->km = new DoublePlus(0.0, "km", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->km->setRichName(trUtf8("k<span style=\" vertical-align:sub;\">m</span>"));
    m_d->km->setToolTip(trUtf8("Coefficiente di ridistribuzione delle tensioni"));
    addVarToContainer( m_d->km );
    VarPlusGUI::connectVar( m_d->km, m_d->ui->kmLabel, m_d->ui->kmLEdit, m_d->ui->kmUMLabel, m_d->ui->kmCheckBox );
    connect( m_d->sectionShape, SIGNAL(valueChanged(QString)), this, SLOT(updatekm()) );
    connect( m_d->km, SIGNAL(readOnlyChanged(bool)), this, SLOT(updatekm()) );

    m_d->kh = new DoublePlus(0.0, "kh", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->kh->setRichName(trUtf8("k<span style=\" vertical-align:sub;\">h</span>"));
    m_d->kh->setToolTip("Coefficiente amplificativo per sforzi normali di trazione paralleli alle fibre");
    addVarToContainer( m_d->kh );
    VarPlusGUI::connectVar( m_d->kh, m_d->ui->khLabel, m_d->ui->khLEdit, m_d->ui->khUMLabel, m_d->ui->khCheckBox );
    connect( m_d->h, SIGNAL(valueChanged(QString)), this, SLOT(updatekh()) );
    connect( m_d->b, SIGNAL(valueChanged(QString)), this, SLOT(updatekh()) );

    m_d->kV = new DoublePlus(1.0, "kV", m_unitMeasure, UnitMeasure::noDimension );
    m_d->kV->setRichName(trUtf8("k<span style=\" vertical-align:sub;\">V</span>"));
    m_d->kV->setToolTip("Coefficiente amplificativo per sforzi normali di trazione perpendicolari alle fibre");
    addVarToContainer( m_d->kV );
    VarPlusGUI::connectVar( m_d->kV, m_d->ui->kVLabel, m_d->ui->kVLEdit, m_d->ui->kVUMLabel, m_d->ui->kVCheckBox );
    connect( m_d->V, SIGNAL(valueChanged(QString)), this, SLOT(updatekV()) );
    connect( m_d->kV, SIGNAL(readOnlyChanged(bool)), this, SLOT(updatekV()) );

    m_d->ksh = new DoublePlus(0.0, "ksh", m_unitMeasure, UnitMeasure::noDimension, true );
    m_d->ksh->setRichName(trUtf8("k<span style=\" vertical-align:sub;\">sh</span>"));
    m_d->ksh->setToolTip("Coefficiente di correlazione tra resistenza a taglio e resistenza a torsione");
    addVarToContainer( m_d->ksh );
    VarPlusGUI::connectVar(m_d->ksh, m_d->ui->kshLabel, m_d->ui->kshLEdit, m_d->ui->kshUMLabel, m_d->ui->kshCheckBox );
    connect( m_d->h, SIGNAL(valueChanged(QString)), this, SLOT(updateksh()) );
    connect( m_d->b, SIGNAL(valueChanged(QString)), this, SLOT(updateksh()) );
    connect( m_d->sectionShape, SIGNAL(readOnlyChanged(bool)), this, SLOT(updateksh()) );

    m_d->fc0d = new DoublePlus(0.0, "fc0d", m_unitMeasure, UnitMeasure::tension, true );
    m_d->fc0d->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">c0d</span>"));
    addVarToContainer( m_d->fc0d );
    VarPlusGUI::connectVar( m_d->fc0d, m_d->ui->fc0dLabel, m_d->ui->fc0dLEdit, m_d->ui->fc0dUMLabel, m_d->ui->fc0dCheckBox );
    connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(updatefc0d()) );
    connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(updatefc0d()) );
    connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(updatefc0d()) );

    m_d->fc90d = new DoublePlus(0.0, "fc90d", m_unitMeasure, UnitMeasure::tension, true );
    m_d->fc90d->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">c90d</span>"));
    addVarToContainer( m_d->fc90d );
    VarPlusGUI::connectVar( m_d->fc90d, m_d->ui->fc90dLabel, m_d->ui->fc90dLEdit, m_d->ui->fc90dUMLabel, m_d->ui->fc90dCheckBox );
    connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(updatefc90d()) );
    connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(updatefc90d()) );
    connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(updatefc90d()) );

    m_d->ft0d = new DoublePlus(0.0, "ft0d", m_unitMeasure, UnitMeasure::tension, true );
    m_d->ft0d->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">t0d</span>"));
    addVarToContainer( m_d->ft0d );
    VarPlusGUI::connectVar( m_d->ft0d, m_d->ui->ft0dLabel, m_d->ui->ft0dLEdit, m_d->ui->ft0dUMLabel, m_d->ui->ft0dCheckBox );
    connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(updateft0d()) );
    connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(updateft0d()) );
    connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(updateft0d()) );
    connect( m_d->kh, SIGNAL(valueChanged(QString)), this, SLOT(updateft0d()) );


    m_d->ft90d = new DoublePlus(0.0, "ft90d", m_unitMeasure, UnitMeasure::tension, true );
    m_d->ft90d->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">t90d</span>"));
    addVarToContainer( m_d->ft90d );
    VarPlusGUI::connectVar( m_d->ft90d, m_d->ui->ft90dLabel, m_d->ui->ft90dLEdit, m_d->ui->ft90dUMLabel, m_d->ui->ft90dCheckBox );
    connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(updateft90d()) );
    connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(updateft90d()) );
    connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(updateft90d()) );
    connect( m_d->kV, SIGNAL(valueChanged(QString)), this, SLOT(updateft90d()) );

    m_d->fmdh = new DoublePlus(0.0, "fmdh", m_unitMeasure, UnitMeasure::tension, true );
    m_d->fmdh->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">md,h</span>"));
    addVarToContainer( m_d->fmdh );
    VarPlusGUI::connectVar( m_d->fmdh, m_d->ui->fmdhLabel, m_d->ui->fmdhLEdit, m_d->ui->fmdhUMLabel, m_d->ui->fmdhCheckBox );
    connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdh()) );
    connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdh()) );
    connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdh()) );
    connect( m_d->khm, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdh()) );

    m_d->fmdb = new DoublePlus(0.0, "fmdb", m_unitMeasure, UnitMeasure::tension, true );
    m_d->fmdb->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">md,b</span>"));
    addVarToContainer( m_d->fmdb );
    VarPlusGUI::connectVar( m_d->fmdb, m_d->ui->fmdbLabel, m_d->ui->fmdbLEdit, m_d->ui->fmdbUMLabel, m_d->ui->fmdbCheckBox );
    connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdb()) );
    connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdb()) );
    connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdb()) );
    connect( m_d->kbm, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdb()) );

    m_d->fvd = new DoublePlus(0.0, "fvd", m_unitMeasure, UnitMeasure::tension, true );
    m_d->fvd->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">vd</span>"));
    addVarToContainer( m_d->fvd );
    VarPlusGUI::connectVar( m_d->fvd, m_d->ui->fvdLabel, m_d->ui->fvdLEdit, m_d->ui->fvdUMLabel, m_d->ui->fvdCheckBox );
    connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(updatefvd()) );
    connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(updatefvd()) );
    connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(updatefvd()) );

    m_d->ftorsd = new DoublePlus(0.0, "ftorsd", m_unitMeasure, UnitMeasure::tension, true );
    m_d->ftorsd->setRichName(trUtf8("f<span style=\" vertical-align:sub;\">tors,d</span>"));
    addVarToContainer( m_d->ftorsd);
    VarPlusGUI::connectVar( m_d->ftorsd, m_d->ui->ftorsdLabel, m_d->ui->ftorsdLEdit, m_d->ui->ftorsdUMLabel, m_d->ui->ftorsdCheckBox );
    connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(updateftorsd()) );
    connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(updateftorsd()) );
    connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(updateftorsd()) );
    connect( m_d->ksh, SIGNAL(valueChanged(QString)), this, SLOT(updateftorsd()) );
}

void TimberGUI::setMaterial( Material * timber ){
    if( m_d->timber != 0 ){
        VarPlusGUI::disconnectVar(m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar(m_d->ui->gammaWmLabel, m_d->ui->gammaWmLEdit, m_d->ui->gammaWmUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->gammaWkLabel, m_d->ui->gammaWkLEdit, m_d->ui->gammaWkUMLabel );
        VarPlusGUI::disconnectEnumVar(m_d->ui->timberTypeLabel, m_d->ui->timberTypeCBox);
        VarPlusGUI::disconnectEnumVar(m_d->ui->woodEssenceLabel, m_d->ui->woodEssenceCBox);
        VarPlusGUI::disconnectVar(m_d->ui->fmkLabel, m_d->ui->fmkLEdit, m_d->ui->fmkUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->fc0kLabel, m_d->ui->fc0kLEdit, m_d->ui->fc0kUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->fc90kLabel, m_d->ui->fc90kLEdit, m_d->ui->fc90kUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->ft0kLabel, m_d->ui->ft0kLEdit, m_d->ui->ft0kUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->ft90kLabel, m_d->ui->ft90kLEdit, m_d->ui->ft90kUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->fvkLabel, m_d->ui->fvkLEdit, m_d->ui->fvkUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->E0kLabel, m_d->ui->E0kLEdit, m_d->ui->E0kUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->E0mLabel, m_d->ui->E0mLEdit, m_d->ui->E0mUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->E90mLabel, m_d->ui->E90mLEdit, m_d->ui->E90mUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->GmLabel, m_d->ui->GmLEdit, m_d->ui->GmUMLabel );

        disconnect( m_d->timber->fc0k, SIGNAL(valueChanged(QString)), this, SLOT(updatefc0d()));
        disconnect( m_d->timber->fc90k, SIGNAL(valueChanged(QString)), this, SLOT(updatefc90d()));
        disconnect( m_d->timber->fmk, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdh()));
        disconnect( m_d->timber->fmk, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdb()));
        disconnect( m_d->timber->ft0k, SIGNAL(valueChanged(QString)), this, SLOT(updateft0d()));
        disconnect( m_d->timber->ft90k, SIGNAL(valueChanged(QString)), this, SLOT(updateft90d()));
        disconnect( m_d->timber->fvk, SIGNAL(valueChanged(QString)), this, SLOT(updatefvd()));
        disconnect( m_d->timber->fvk, SIGNAL(valueChanged(QString)), this, SLOT(updateftorsd()));
        disconnect( m_d->timber->timberType, SIGNAL(valueChanged(QString)), this, SLOT(updatekhm()));
        disconnect( m_d->timber->timberType, SIGNAL(valueChanged(QString)), this, SLOT(updatekbm()));
        disconnect( m_d->timber->timberType, SIGNAL(valueChanged(QString)), this, SLOT(updatekh()));
        disconnect( m_d->ui->copyfkToClipBoardPButton, SIGNAL(clicked()), m_d->timber, SLOT(copyfkToClipBoard()));
        disconnect( m_d->ui->copyEToClipBoardPButton, SIGNAL(clicked()), m_d->timber, SLOT(copyEToClipBoard()));
    }

    m_d->timber = dynamic_cast<Timber *>(timber);

    if( m_d->timber != 0 ){
        VarPlusGUI::connectVar(m_d->timber->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar(m_d->timber->gammaW, m_d->ui->gammaWmLabel, m_d->ui->gammaWmLEdit, m_d->ui->gammaWmUMLabel );
        VarPlusGUI::connectVar(m_d->timber->gammaWk, m_d->ui->gammaWkLabel, m_d->ui->gammaWkLEdit, m_d->ui->gammaWkUMLabel );
        VarPlusGUI::connectEnumVar(m_d->timber->timberType, m_d->ui->timberTypeLabel, m_d->ui->timberTypeCBox);
        VarPlusGUI::connectEnumVar(m_d->timber->woodEssence, m_d->ui->woodEssenceLabel, m_d->ui->woodEssenceCBox);
        VarPlusGUI::connectVar(m_d->timber->fmk, m_d->ui->fmkLabel, m_d->ui->fmkLEdit, m_d->ui->fmkUMLabel );
        VarPlusGUI::connectVar(m_d->timber->fc0k, m_d->ui->fc0kLabel, m_d->ui->fc0kLEdit, m_d->ui->fc0kUMLabel );
        VarPlusGUI::connectVar(m_d->timber->fc90k, m_d->ui->fc90kLabel, m_d->ui->fc90kLEdit, m_d->ui->fc90kUMLabel );
        VarPlusGUI::connectVar(m_d->timber->ft0k, m_d->ui->ft0kLabel, m_d->ui->ft0kLEdit, m_d->ui->ft0kUMLabel );
        VarPlusGUI::connectVar(m_d->timber->ft90k, m_d->ui->ft90kLabel, m_d->ui->ft90kLEdit, m_d->ui->ft90kUMLabel );
        VarPlusGUI::connectVar(m_d->timber->fvk, m_d->ui->fvkLabel, m_d->ui->fvkLEdit, m_d->ui->fvkUMLabel );
        VarPlusGUI::connectVar(m_d->timber->E0k, m_d->ui->E0kLabel, m_d->ui->E0kLEdit, m_d->ui->E0kUMLabel );
        VarPlusGUI::connectVar(m_d->timber->E0m, m_d->ui->E0mLabel, m_d->ui->E0mLEdit, m_d->ui->E0mUMLabel );
        VarPlusGUI::connectVar(m_d->timber->E90m, m_d->ui->E90mLabel, m_d->ui->E90mLEdit, m_d->ui->E90mUMLabel );
        VarPlusGUI::connectVar(m_d->timber->Gm, m_d->ui->GmLabel, m_d->ui->GmLEdit, m_d->ui->GmUMLabel );

        connect( m_d->timber->fc0k, SIGNAL(valueChanged(QString)), this, SLOT(updatefc0d()));
        connect( m_d->timber->fc90k, SIGNAL(valueChanged(QString)), this, SLOT(updatefc90d()));
        connect( m_d->timber->fmk, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdh()));
        connect( m_d->timber->fmk, SIGNAL(valueChanged(QString)), this, SLOT(updatefmdb()));
        connect( m_d->timber->ft0k, SIGNAL(valueChanged(QString)), this, SLOT(updateft0d()));
        connect( m_d->timber->ft90k, SIGNAL(valueChanged(QString)), this, SLOT(updateft90d()));
        connect( m_d->timber->fvk, SIGNAL(valueChanged(QString)), this, SLOT(updatefvd()));
        connect( m_d->timber->fvk, SIGNAL(valueChanged(QString)), this, SLOT(updateftorsd()));
        connect( m_d->timber->timberType, SIGNAL(valueChanged(QString)), this, SLOT(updatekhm()));
        connect( m_d->timber->timberType, SIGNAL(valueChanged(QString)), this, SLOT(updatekbm()));
        connect( m_d->timber->timberType, SIGNAL(valueChanged(QString)), this, SLOT(updatekh()));
        connect( m_d->ui->copyfkToClipBoardPButton, SIGNAL(clicked()), m_d->timber, SLOT(copyfkToClipBoard()));
        connect( m_d->ui->copyEToClipBoardPButton, SIGNAL(clicked()), m_d->timber, SLOT(copyEToClipBoard()));

        connect( m_d->lsType, SIGNAL(valueChanged(QString)), this, SLOT(setCurrentGammaM()) );
        setCurrentGammaM();
        connect( m_d->ldClass, SIGNAL(valueChanged(QString)), this, SLOT(setCurrentKmod()) );
        connect( m_d->sClass, SIGNAL(valueChanged(QString)), this, SLOT(setCurrentKmod()) );
        setCurrentKmod();

        updatekhm();
        updatekbm();
        updatekm();
        updatekh();
        updatekV();
        updateksh();

        updatefc0d();
        updatefc90d();
        updateft0d();
        updateft90d();
        updatefmdh();
        updatefmdb();
        updatefvd();
        updateftorsd();
    }
}

void TimberGUI::setCurrentKmod(){
    if( m_d->timber != 0 ){
        VarPlusGUI::disconnectVar( m_d->ui->kmodLabel, m_d->ui->kmodLEdit, m_d->ui->kmodUMLabel, m_d->ui->kmodROCBox );
        VarPlusGUI::connectVar( m_d->timber->kmod( m_d->sClass, m_d->ldClass ), m_d->ui->kmodLabel, m_d->ui->kmodLEdit, m_d->ui->kmodUMLabel, m_d->ui->kmodROCBox );
    }
}

void TimberGUI::setCurrentGammaM(){
    if( m_d->timber != 0 ){
        VarPlusGUI::disconnectVar( m_d->ui->gammaMLabel, m_d->ui->gammaMLEdit, m_d->ui->gammaMUMLabel, m_d->ui->gammaMROCBox );
        VarPlusGUI::connectVar( m_d->timber->gammaM( m_d->lsType ), m_d->ui->gammaMLabel, m_d->ui->gammaMLEdit, m_d->ui->gammaMUMLabel, m_d->ui->gammaMROCBox );
    }
}

void TimberGUI::updateSectionData(){
    if( m_d->sectionShape->valueNormal() == SectionTimber::circular ){
        m_d->ui->bLEdit->setDisabled( true );
        m_d->ui->hLEdit->setDisabled( true );
    } else {
        m_d->ui->bLEdit->setEnabled( true );
        m_d->ui->hLEdit->setEnabled( true );
    }
}

void TimberGUI::updatefc0d(){
    if( m_d->fc0d->readOnly() ){
        m_d->timber->fc0d( m_d->fc0d, m_d->lsType, m_d->sClass, m_d->ldClass );
    }
}

void TimberGUI::updatefc90d(){
    if( m_d->fc90d->readOnly() ){
        m_d->timber->fc90d( m_d->fc90d, m_d->lsType, m_d->sClass, m_d->ldClass );
    }
}

void TimberGUI::updateft0d(){
    if( m_d->ft0d->readOnly() ){
        m_d->timber->ft0d(m_d->ft0d, m_d->lsType, m_d->sClass, m_d->ldClass, m_d->kh );
    }
}

void TimberGUI::updateft90d(){
    if( m_d->ft90d->readOnly() ){
        m_d->timber->ft90d( m_d->ft90d, m_d->lsType, m_d->sClass, m_d->ldClass, m_d->kV );
    }
}

void TimberGUI::updatefmdh(){
    if( m_d->fmdh->readOnly() ){
        m_d->timber->fmd( m_d->fmdh, m_d->lsType, m_d->sClass, m_d->ldClass, m_d->khm );
    }
}

void TimberGUI::updatefmdb(){
    if( m_d->fmdb->readOnly() ){
        m_d->timber->fmd( m_d->fmdb, m_d->lsType, m_d->sClass, m_d->ldClass, m_d->kbm );
    }
}

void TimberGUI::updatefvd(){
    if( m_d->fvd->readOnly() ){
        m_d->timber->fvd( m_d->fvd, m_d->lsType, m_d->sClass, m_d->ldClass );
    }
}

void TimberGUI::updateftorsd(){
    if( m_d->ftorsd->readOnly() ){
        m_d->timber->ftorsd( m_d->ftorsd, m_d->lsType, m_d->sClass, m_d->ldClass, m_d->ksh );
    }
}


void TimberGUI::updatekhm(){
    if( m_d->khm->readOnly() ){
        m_d->timber->kh( m_d->khm, m_d->h );
    }
}

void TimberGUI::updatekbm(){
    if( m_d->kbm->readOnly() ){
        m_d->timber->kh( m_d->kbm, m_d->b );
    }
}

void TimberGUI::updatekm(){
    if( m_d->km->readOnly() ){
        m_d->timber->km( m_d->km, m_d->sectionShape->valueNormal() );
    }
}

void TimberGUI::updatekh(){
    if( m_d->kh->readOnly() ){
        if( m_d->b->valueNormal() > m_d->h->valueNormal() ){
            m_d->timber->kh( m_d->kh, m_d->b );
        } else {
            m_d->timber->kh( m_d->kh, m_d->h );
        }
    }
}

void TimberGUI::updatekV(){
    if( m_d->kV->readOnly() ){
        m_d->timber->kV( m_d->kV, m_d->V );
    }
}

void TimberGUI::updateksh(){
    if( m_d->ksh->readOnly() ){
        m_d->timber->ksh( m_d->ksh, m_d->sectionShape->valueNormal(), m_d->b, m_d->h);
    }
}

void TimberGUI::updateType2FilterStandardComboBox(){
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
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbTimberName ) );
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant data = queryModel.record(i).value("idType2");
        QString name = queryModel.record(i).value("nameType2").toString();
        m_d->ui->type2FilterStandardComboBox->insertItem((i+1), name, data );
    }
}

void TimberGUI::updateWoodNameStandardComboBox(){
    QSqlQueryModel queryModel;
    QString queryString = QString( "SELECT id, name, type2 from timber" );

    QVariant type1Data = m_d->ui->type1FilterStandardComboBox->itemData( m_d->ui->type1FilterStandardComboBox->currentIndex() );
    QVariant type2Data = m_d->ui->type2FilterStandardComboBox->itemData( m_d->ui->type2FilterStandardComboBox->currentIndex() );

    if(  type2Data != QVariant::Invalid && type2Data.toInt() > 0 ){
        queryString.append( QString(" WHERE type2=") + QString::number(type2Data.toInt()) );
    } else if(  type1Data != QVariant::Invalid && type1Data.toInt() > -1 ){
        queryString.append( QString(" INNER JOIN type2 ON type2=idType2 WHERE type1=") + QString::number(type1Data.toInt()) );
    }

    m_d->ui->woodNameStandardComboBox->clear();
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbTimberName ) );
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant data = queryModel.record(i).value("id");
        QString name = queryModel.record(i).value("name").toString();
        m_d->ui->woodNameStandardComboBox->insertItem(i, name, data );
    }
}

void TimberGUI::loadStandardData(){
    QVariant woodData = m_d->ui->woodNameStandardComboBox->itemData( m_d->ui->woodNameStandardComboBox->currentIndex() );
    if( woodData != QVariant::Invalid ){
        QSqlQueryModel queryModel;
        QString queryString = QString( "SELECT id, name, description, type2, woodEssence, fmk, ft0k, ft90k, fc0k, fc90k, fvk, E0m, E0k, E90m, Gm, gammaWk, gammaWm, type1 from timber" );
        queryString.append( QString(" INNER JOIN type2 ON type2=idType2 WHERE id=") + QString::number( woodData.toInt()));
        queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbTimberName ) );
        if( queryModel.rowCount() > 0 ){
            m_d->timber->name->setValueNormal( queryModel.record(0).value("name").toString() );
            m_d->timber->fmk->setValueNormal( queryModel.record(0).value("fmk").toDouble() );
            m_d->timber->ft0k->setValueNormal( queryModel.record(0).value("ft0k").toDouble() );
            m_d->timber->ft90k->setValueNormal( queryModel.record(0).value("ft90k").toDouble() );
            m_d->timber->fc0k->setValueNormal( queryModel.record(0).value("fc0k").toDouble() );
            m_d->timber->fc90k->setValueNormal( queryModel.record(0).value("fc90k").toDouble() );
            m_d->timber->fvk->setValueNormal( queryModel.record(0).value("fvk").toDouble() );
            m_d->timber->E0m->setValueNormal( queryModel.record(0).value("E0m").toDouble() );
            m_d->timber->E0k->setValueNormal( queryModel.record(0).value("E0k").toDouble() );
            m_d->timber->E90m->setValueNormal( queryModel.record(0).value("E90m").toDouble() );
            m_d->timber->Gm->setValueNormal( queryModel.record(0).value("Gm").toDouble() );
            m_d->timber->gammaWk->setValueNormal( queryModel.record(0).value("gammaWk").toDouble() );
            m_d->timber->gammaW->setValueNormal( queryModel.record(0).value("gammaWm").toDouble() );
            if( m_d->timber->gammaW->valueNormal() == 0.0 ){
                m_d->timber->gammaW->setValueNormal( m_d->timber->gammaWk->valueNormal() );
            }
            if( queryModel.record(0).value("type1").toInt() > 1 )
                m_d->timber->timberType->setValueInt( (queryModel.record(0).value("type1").toInt()-1) );
            else
                m_d->timber->timberType->setValueNormal(Timber::solid );
            if( queryModel.record(0).value("woodEssence").toInt() > 1 )
                m_d->timber->woodEssence->setValueInt( (queryModel.record(0).value("woodEssence").toInt()-1) );
            else
                m_d->timber->woodEssence->setValueNormal(Timber::softwoods );
        }
    }
}
