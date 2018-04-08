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
#include "concretegui.h"
#include "ui_concretegui.h"

#include <QtSql>
#include <QMessageBox>

#include "concrete.h"
#include "varplusguiutility.h"
#include "qradiobuttonenum.h"
#include "doubleplus.h"
#include "boolplus.h"
#include "qstringplus.h"

#include <QSettings>

class ConcreteGUIPrivate{
public:
    ConcreteGUIPrivate(){
        dbCncrName = "cncrDB";
        cncr = NULL;
        graph = NULL;
    }
    QString settingsFile;
    QString dbCncrFileName;
    QString dbCncrName;

    Ui::ConcreteGUI * ui;

    Concrete * cncr;

    QCPGraph * graph;

    DoublePlus * phiSteelCncr;
    DoublePlus * fbk;
    DoublePlus * fbd;
};

ConcreteGUI::ConcreteGUI(UnitMeasure * um, const QString & settingsFile, QWidget *parent) :
    MaterialGUI(um, parent),
    m_d( new ConcreteGUIPrivate ){
    m_d->settingsFile = settingsFile;

    m_d->ui = new Ui::ConcreteGUI;
    m_d->ui->setupUi(this);

    // apre il database
    if( !QSqlDatabase::contains( m_d->dbCncrName )){
        QSqlDatabase dbCncr = QSqlDatabase::addDatabase("QSQLITE", m_d->dbCncrName );
        QSettings settings( m_d->settingsFile, QSettings::IniFormat );
        m_d->dbCncrFileName = settings.value("dbCncrFileName", QApplication::applicationDirPath() + "/../resources/cncr.db").toString();
        dbCncr.setDatabaseName(m_d->dbCncrFileName);
        if (!dbCncr.open() ) {
            QMessageBox::critical(0, tr("Non riesco ad aprire il database dei calcestruzzi"),
                                  tr("Non riesco a trovare il file cncr.db"), QMessageBox::Cancel);
        } else {
            loadStandardComboBox();
        }
    } else {
        loadStandardComboBox();
    }

    // set value of phi
    m_d->phiSteelCncr = new DoublePlus(32.0e-3, "phi", um, UnitMeasure::sectL );
    m_d->phiSteelCncr->setRichName( trUtf8("Ø"));
    addVarToContainer( m_d->phiSteelCncr );
    VarPlusGUI::connectVar( m_d->phiSteelCncr, m_d->ui->phiLabel, m_d->ui->phiLEdit, m_d->ui->phiUMLabel );

    m_d->fbk = new DoublePlus(0.0, "fbk", um, UnitMeasure::tension );
    m_d->fbk->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">bk</span>"));
    addVarToContainer( m_d->fbk );
    VarPlusGUI::connectVar( m_d->fbk, m_d->ui->fbkLabel, m_d->ui->fbkLEdit, m_d->ui->fbkUMLabel );

    m_d->fbd = new DoublePlus(0.0, "fbd", um, UnitMeasure::tension );
    m_d->fbd->setRichName( trUtf8("f<span style=\"vertical-align:sub;\">bd</span>"));
    addVarToContainer( m_d->fbd );
    VarPlusGUI::connectVar( m_d->fbd, m_d->ui->fbdLabel, m_d->ui->fbdLEdit, m_d->ui->fbdUMLabel );
}

void ConcreteGUI::loadStandardComboBox(){
    // populates nameCncrComboBox
    QString queryString = QString( "SELECT id, name from cncr" );
    QSqlQueryModel queryModel;
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbCncrName ) );
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant cncrData = queryModel.record(i).value("id");
        QString name = queryModel.record(i).value("name").toString();
        m_d->ui->standardCncrCBox->insertItem((i+1), name, cncrData );
    }
    connect( m_d->ui->loadStandardPButton, SIGNAL(clicked()), this, SLOT( loadStandardData()) );
}

ConcreteGUI::~ConcreteGUI(){
    QSqlDatabase dbCncr = QSqlDatabase::database( m_d->dbCncrName );
    dbCncr.close();

    QSettings settings(m_d->settingsFile,  QSettings::IniFormat);
    settings.setValue("dbCncrFileName", m_d->dbCncrFileName);

    delete m_d->ui;
}

void ConcreteGUI::setMaterial( Material * cncr ){
    if( m_d->cncr != NULL ){
        VarPlusGUI::disconnectVar( m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar( m_d->ui->isExistingLabel, m_d->ui->isExistingCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->FCLabel, m_d->ui->FCLEdit, m_d->ui->FCUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->gammaWLabel, m_d->ui->gammaWLEdit, m_d->ui->gammaWUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->RckLabel, m_d->ui->RckLEdit, m_d->ui->RckUMLabel);
        VarPlusGUI::disconnectVar( m_d->ui->fckLabel, m_d->ui->fckLEdit, m_d->ui->fckUMLabel, m_d->ui->fckCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fcdLabel, m_d->ui->fcdLEdit, m_d->ui->fcdUMLabel, m_d->ui->fcdCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fcdRedLabel, m_d->ui->fcdRedLEdit, m_d->ui->fcdRedUMLabel, m_d->ui->fcdRedCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fcmLabel, m_d->ui->fcmLEdit, m_d->ui->fcmUMLabel, m_d->ui->fcmCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fcfmLabel, m_d->ui->fcfmLEdit, m_d->ui->fcfmUMLabel, m_d->ui->fcfmCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fctdLabel, m_d->ui->fctdLEdit, m_d->ui->fctdUMLabel, m_d->ui->fctdCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fctdRedLabel, m_d->ui->fctdRedLEdit, m_d->ui->fctdRedUMLabel, m_d->ui->fctdRedCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fctkLabel, m_d->ui->fctkLEdit, m_d->ui->fctkUMLabel, m_d->ui->fctkCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fctk95Label, m_d->ui->fctk95LEdit, m_d->ui->fctk95UMLabel, m_d->ui->fctk95CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fctmLabel, m_d->ui->fctmLEdit, m_d->ui->fctmUMLabel, m_d->ui->fctmCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fcRareLabel, m_d->ui->fcRareLEdit, m_d->ui->fcRareUMLabel, m_d->ui->fcRareCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fcRareRedLabel, m_d->ui->fcRareRedLEdit, m_d->ui->fcRareRedUMLabel, m_d->ui->fcRareRedCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fcPermanentLabel, m_d->ui->fcPermanentLEdit, m_d->ui->fcPermanentUMLabel, m_d->ui->fcPermanentCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->fcPermanentRedLabel, m_d->ui->fcPermanentRedLEdit, m_d->ui->fcPermanentRedUMLabel, m_d->ui->fcPermanentRedCheckBox );

        VarPlusGUI::disconnectVar( m_d->ui->EcmLabel, m_d->ui->EcmLEdit, m_d->ui->EcmUMLabel, m_d->ui->EcmCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->ratioEEcmLabel, m_d->ui->ratioEEcmLEdit, m_d->ui->ratioEEcmUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->ELabel, m_d->ui->ELEdit, m_d->ui->EUMLabel, m_d->ui->ECheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->GLabel, m_d->ui->GLEdit, m_d->ui->GUMLabel, m_d->ui->GCheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->nucLabel, m_d->ui->nucLEdit, m_d->ui->nucUMLabel );

        VarPlusGUI::disconnectVar( m_d->ui->epsC1Label, m_d->ui->epsC1LEdit, m_d->ui->epsC1UMLabel, m_d->ui->epsC1CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->epsC2Label, m_d->ui->epsC2LEdit, m_d->ui->epsC2UMLabel, m_d->ui->epsC2CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->epsC3Label, m_d->ui->epsC3LEdit, m_d->ui->epsC3UMLabel, m_d->ui->epsC3CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->epsC4Label, m_d->ui->epsC4LEdit, m_d->ui->epsC4UMLabel, m_d->ui->epsC4CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->epsCU1Label, m_d->ui->epsCU1LEdit, m_d->ui->epsCU1UMLabel, m_d->ui->epsCU1CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->epsCU2Label, m_d->ui->epsCU2LEdit, m_d->ui->epsCU2UMLabel, m_d->ui->epsCU2CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->epsCU3Label, m_d->ui->epsCU3LEdit, m_d->ui->epsCU3UMLabel, m_d->ui->epsCU3CheckBox );
        VarPlusGUI::disconnectVar( m_d->ui->epsCU4Label, m_d->ui->epsCU4LEdit, m_d->ui->epsCU4UMLabel, m_d->ui->epsCU4CheckBox );

        VarPlusGUI::disconnectVar( m_d->ui->alphaLabel, m_d->ui->alphaLEdit, m_d->ui->alphaUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->gammaCLabel, m_d->ui->gammaCLEdit, m_d->ui->gammaCUMLabel );
        VarPlusGUI::disconnectVar( m_d->ui->alphaCCLabel, m_d->ui->alphaCCLEdit, m_d->ui->alphaCCUMLabel );

        QList<QRadioButtonEnum *> listQRB;
        listQRB << m_d->ui->modelSLURB1 << m_d->ui->modelSLURB2 << m_d->ui->modelSLURB3;
        VarPlusGUI::disconnectEnumVar( listQRB );

        disconnect( m_d->cncr->fctk, SIGNAL(valueChanged(QString)), this, SLOT( setFb()) );
        disconnect( m_d->cncr->modelfEpsSection, SIGNAL(valueChanged(QString)), this, SLOT(plot()));

        disconnect( m_d->cncr->epsC2, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        disconnect( m_d->cncr->epsC3, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        disconnect( m_d->cncr->epsC4, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );

        disconnect( m_d->cncr->epsCU2, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        disconnect( m_d->cncr->epsCU3, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        disconnect( m_d->cncr->epsCU4, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        disconnect( m_d->cncr->fcd, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );

        disconnect( m_d->cncr, &Material::aboutToBeDeleted, this, &MaterialGUI::setMaterialNULL );
    }

    m_d->cncr = dynamic_cast<Concrete*>(cncr);

    if( m_d->cncr != NULL ){
        // sync value of name
        VarPlusGUI::connectVar( m_d->cncr->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar( m_d->cncr->isExisting, m_d->ui->isExistingLabel, m_d->ui->isExistingCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->FC, m_d->ui->FCLabel, m_d->ui->FCLEdit, m_d->ui->FCUMLabel );
        VarPlusGUI::connectVar( m_d->cncr->gammaW, m_d->ui->gammaWLabel, m_d->ui->gammaWLEdit, m_d->ui->gammaWUMLabel );
        VarPlusGUI::connectVar( m_d->cncr->Rck, m_d->ui->RckLabel, m_d->ui->RckLEdit, m_d->ui->RckUMLabel);
        VarPlusGUI::connectVar( m_d->cncr->fck, m_d->ui->fckLabel, m_d->ui->fckLEdit, m_d->ui->fckUMLabel, m_d->ui->fckCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fcd, m_d->ui->fcdLabel, m_d->ui->fcdLEdit, m_d->ui->fcdUMLabel, m_d->ui->fcdCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fcdRed, m_d->ui->fcdRedLabel, m_d->ui->fcdRedLEdit, m_d->ui->fcdRedUMLabel, m_d->ui->fcdRedCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fcm, m_d->ui->fcmLabel, m_d->ui->fcmLEdit, m_d->ui->fcmUMLabel, m_d->ui->fcmCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fcfm, m_d->ui->fcfmLabel, m_d->ui->fcfmLEdit, m_d->ui->fcfmUMLabel, m_d->ui->fcfmCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fctd, m_d->ui->fctdLabel, m_d->ui->fctdLEdit, m_d->ui->fctdUMLabel, m_d->ui->fctdCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fctdRed, m_d->ui->fctdRedLabel, m_d->ui->fctdRedLEdit, m_d->ui->fctdRedUMLabel, m_d->ui->fctdRedCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fctk, m_d->ui->fctkLabel, m_d->ui->fctkLEdit, m_d->ui->fctkUMLabel, m_d->ui->fctkCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fctk95, m_d->ui->fctk95Label, m_d->ui->fctk95LEdit, m_d->ui->fctk95UMLabel, m_d->ui->fctk95CheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fctm, m_d->ui->fctmLabel, m_d->ui->fctmLEdit, m_d->ui->fctmUMLabel, m_d->ui->fctmCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fcRare, m_d->ui->fcRareLabel, m_d->ui->fcRareLEdit, m_d->ui->fcRareUMLabel, m_d->ui->fcRareCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fcRareRed, m_d->ui->fcRareRedLabel, m_d->ui->fcRareRedLEdit, m_d->ui->fcRareRedUMLabel, m_d->ui->fcRareRedCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fcPermanent, m_d->ui->fcPermanentLabel, m_d->ui->fcPermanentLEdit, m_d->ui->fcPermanentUMLabel, m_d->ui->fcPermanentCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->fcPermanentRed, m_d->ui->fcPermanentRedLabel, m_d->ui->fcPermanentRedLEdit, m_d->ui->fcPermanentRedUMLabel, m_d->ui->fcPermanentRedCheckBox );

        VarPlusGUI::connectVar( m_d->cncr->Ecm, m_d->ui->EcmLabel, m_d->ui->EcmLEdit, m_d->ui->EcmUMLabel, m_d->ui->EcmCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->ratioEEcm, m_d->ui->ratioEEcmLabel, m_d->ui->ratioEEcmLEdit, m_d->ui->ratioEEcmUMLabel );
        VarPlusGUI::connectVar( m_d->cncr->E, m_d->ui->ELabel, m_d->ui->ELEdit, m_d->ui->EUMLabel, m_d->ui->ECheckBox );
        VarPlusGUI::connectVar( m_d->cncr->G, m_d->ui->GLabel, m_d->ui->GLEdit, m_d->ui->GUMLabel, m_d->ui->GCheckBox );
        VarPlusGUI::connectVar( m_d->cncr->nu, m_d->ui->nucLabel, m_d->ui->nucLEdit, m_d->ui->nucUMLabel );

        VarPlusGUI::connectVar( m_d->cncr->epsC1, m_d->ui->epsC1Label, m_d->ui->epsC1LEdit, m_d->ui->epsC1UMLabel, m_d->ui->epsC1CheckBox );
        VarPlusGUI::connectVar( m_d->cncr->epsC2, m_d->ui->epsC2Label, m_d->ui->epsC2LEdit, m_d->ui->epsC2UMLabel, m_d->ui->epsC2CheckBox );
        VarPlusGUI::connectVar( m_d->cncr->epsC3, m_d->ui->epsC3Label, m_d->ui->epsC3LEdit, m_d->ui->epsC3UMLabel, m_d->ui->epsC3CheckBox );
        VarPlusGUI::connectVar( m_d->cncr->epsC4, m_d->ui->epsC4Label, m_d->ui->epsC4LEdit, m_d->ui->epsC4UMLabel, m_d->ui->epsC4CheckBox );
        VarPlusGUI::connectVar( m_d->cncr->epsCU1, m_d->ui->epsCU1Label, m_d->ui->epsCU1LEdit, m_d->ui->epsCU1UMLabel, m_d->ui->epsCU1CheckBox );
        VarPlusGUI::connectVar( m_d->cncr->epsCU2, m_d->ui->epsCU2Label, m_d->ui->epsCU2LEdit, m_d->ui->epsCU2UMLabel, m_d->ui->epsCU2CheckBox );
        VarPlusGUI::connectVar( m_d->cncr->epsCU3, m_d->ui->epsCU3Label, m_d->ui->epsCU3LEdit, m_d->ui->epsCU3UMLabel, m_d->ui->epsCU3CheckBox );
        VarPlusGUI::connectVar( m_d->cncr->epsCU4, m_d->ui->epsCU4Label, m_d->ui->epsCU4LEdit, m_d->ui->epsCU4UMLabel, m_d->ui->epsCU4CheckBox );

        VarPlusGUI::connectVar( m_d->cncr->alpha, m_d->ui->alphaLabel, m_d->ui->alphaLEdit, m_d->ui->alphaUMLabel );
        VarPlusGUI::connectVar( m_d->cncr->gammaC, m_d->ui->gammaCLabel, m_d->ui->gammaCLEdit, m_d->ui->gammaCUMLabel );
        VarPlusGUI::connectVar( m_d->cncr->alphaCC, m_d->ui->alphaCCLabel, m_d->ui->alphaCCLEdit, m_d->ui->alphaCCUMLabel );

        QList<QRadioButtonEnum *> listQRB;
        listQRB << m_d->ui->modelSLURB1 << m_d->ui->modelSLURB2 << m_d->ui->modelSLURB3;
        VarPlusGUI::connectEnumVar( m_d->cncr->modelfEpsSection, listQRB);

        // set value of phi
        connect( m_d->cncr->fctk, SIGNAL(valueChanged(QString)), this, SLOT( setFb()) );
        setFb();

        // sets slots to redraw model diagram
        connect( m_d->cncr->modelfEpsSection, SIGNAL(valueChanged(QString)), this, SLOT(plot()));

        connect( m_d->cncr->epsC2, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        connect( m_d->cncr->epsC3, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        connect( m_d->cncr->epsC4, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );

        connect( m_d->cncr->epsCU2, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        connect( m_d->cncr->epsCU3, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        connect( m_d->cncr->epsCU4, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        connect( m_d->cncr->fcd, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        plot();

        connect( m_d->cncr, &Material::aboutToBeDeleted, this, &MaterialGUI::setMaterialNULL );
    }
}

void ConcreteGUI::setFb( void ) {
    m_d->cncr->fbk( m_d->fbk, m_d->phiSteelCncr );
    m_d->cncr->fbd( m_d->fbd, m_d->phiSteelCncr );
}

void ConcreteGUI::plot() {
    if( m_d->graph == NULL){
        m_d->graph = m_d->ui->plotCULS->addGraph();
    }

    updatePlotData();

    // finally, refresh the plot
    m_d->ui->plotCULS->replot();
}

void ConcreteGUI::replot( void ) {
    updatePlotData();

    // finally, refresh the plot
    m_d->ui->plotCULS->replot();
}

void ConcreteGUI::updatePlotData() {
    QVector<double> xData;
    QVector<double> yData;
    if( m_d->cncr->modelfEpsSection->value() == Concrete::SectionParabolaRectangle ){
        unsigned int i;
        double De = m_d->cncr->epsC2->value() / ( CONCRETE_PLOT_ACCURACY - 2);
        for( i = 0; i < (CONCRETE_PLOT_ACCURACY - 1) ; i++ ){
            xData.append( i * De );
            yData.append( (m_d->cncr->fcd->value() / m_d->cncr->epsC2->value() ) * (-xData[i] / m_d->cncr->epsC2->value() + 2.0 ) * xData[i] );
        }
        xData.append( m_d->cncr->epsCU2->value() );
        yData.append( m_d->cncr->fcd->value() );
    } else if( m_d->cncr->modelfEpsSection->value() == Concrete::SectionBiLinear ){
        xData.append( 0.0 );
        yData.append( 0.0 );
        xData.append( m_d->cncr->epsC3->value() );
        yData.append( m_d->cncr->fcd->value() );
        xData.append( m_d->cncr->epsCU3->value() );
        yData.append( m_d->cncr->fcd->value() );
    } else if(m_d->cncr->modelfEpsSection->value() == Concrete::SectionStressBlock ){
        xData.append( m_d->cncr->epsC4->value() );
        yData.append( m_d->cncr->fcd->value() );
        xData.append( m_d->cncr->epsCU4->value() );
        yData.append( m_d->cncr->fcd->value() );
    }

    m_d->ui->plotCULS->xAxis->setRange( 0.0, xData.last() );
    m_d->ui->plotCULS->yAxis->setRange( 0.0, yData.last() );
    m_d->graph->setData( xData, yData );
}

void ConcreteGUI::loadStandardData( void ) {
    QVariant cncrData = m_d->ui->standardCncrCBox->itemData( m_d->ui->standardCncrCBox->currentIndex() );
    if( cncrData != QVariant::Invalid ){
        QSqlQueryModel queryModel;
        QString queryString = QString( "SELECT id, name, Rck, fck from cncr WHERE id=" ) + QString::number( cncrData.toInt());
        queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbCncrName ) );
        if( queryModel.rowCount() > 0 ){
            m_d->cncr->Rck->setValueNormal( queryModel.record(0).value("Rck").toString() );
            m_d->cncr->fck->setReadOnly( false );
            m_d->cncr->fck->setValueNormal( queryModel.record(0).value("fck").toString() );
        }
    }
}
