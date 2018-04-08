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
#include "steelcncrgui.h"
#include "ui_steelcncrgui.h"

#include <QtSql>
#include <QMessageBox>

#include "steelcncr.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "changerichnamedialog.h"
#include "varplusguiutility.h"

class SteelCncrGUIPrivate{
public:
    SteelCncrGUIPrivate() {
        dbSteelCncrName = "steelCncrDB";
        steelCncr = NULL;
        graph = NULL;
    }
    QString settingsFile;
    QString dbSteelCncrName;
    QString dbSteelCncrFileName;

    Ui::SteelCncrGUI * ui;

    SteelCncr * steelCncr;

    QCPGraph * graph;
};

SteelCncrGUI::SteelCncrGUI(UnitMeasure * um, const QString & settingsFile, QWidget *parent) :
    MaterialGUI(um, parent),
    m_d( new SteelCncrGUIPrivate )
    {
    m_d->settingsFile = settingsFile;

    // Setup UI
    m_d->ui = new Ui::SteelCncrGUI;
    m_d->ui->setupUi(this);

    // Open database
    if( !QSqlDatabase::contains( m_d->dbSteelCncrName )){
        QSqlDatabase dbSteelCncr = QSqlDatabase::addDatabase("QSQLITE", m_d->dbSteelCncrName );
        QSettings settings( m_d->settingsFile, QSettings::IniFormat );
        m_d->dbSteelCncrFileName = settings.value("dbSteelCncrFileName", QApplication::applicationDirPath() + "/../resources/steelcncr.db").toString();
        dbSteelCncr.setDatabaseName(m_d->dbSteelCncrFileName);
        if (!dbSteelCncr.open() ) {
            QMessageBox::critical(0, trUtf8("Non riesco ad aprire il database dei degli acciai da c.a."),
                                  trUtf8("Non riesco a trovare il file steelcncr.db"), QMessageBox::Cancel);
        } else {
            loadStandardComboBox();
        }
    } else {
        loadStandardComboBox();
    }

    connect(m_d->ui->fykLabelPButton, SIGNAL(clicked()), this, SLOT(changeFykLabel()) );
    connect(m_d->ui->ftkLabelPButton, SIGNAL(clicked()), this, SLOT(changeFtkLabel()) );
}

void SteelCncrGUI::loadStandardComboBox(){
    // Aggiunge i dati in standardTypeComboBox
    QString queryString = QString( "SELECT idType, nameType from type" );
    QSqlQueryModel queryModel;
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSteelCncrName ) );
    m_d->ui->standardTypeCBox->insertItem(0, QString("------"), QVariant(-1));
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant typeData = queryModel.record(i).value("idType");
        QString name = queryModel.record(i).value("nameType").toString();
        m_d->ui->standardTypeCBox->insertItem((i+1), name, typeData );
    }
    m_d->ui->standardNameCBox->setCurrentIndex( 0 );
    connect( m_d->ui->standardTypeCBox, SIGNAL( currentIndexChanged(QString)), this, SLOT( updateStandardNameCBox()));

    // Aggiunge i dati in standardNameComboBox
    updateStandardNameCBox();

    connect( m_d->ui->loadStandardPButton, SIGNAL( clicked()) , this, SLOT(loadStandardData()));
}

SteelCncrGUI::~SteelCncrGUI(){
    // Closes database
    QSqlDatabase dbSteelCncr = QSqlDatabase::database( m_d->dbSteelCncrName );
    dbSteelCncr.close();

    QSettings settings(m_d->settingsFile,  QSettings::IniFormat);
    settings.setValue("dbSteelCncrFileName", m_d->dbSteelCncrFileName);

    delete m_d->ui;
}

void SteelCncrGUI::setMaterial( Material * steelCncr ){
    if( m_d->steelCncr ){
        VarPlusGUI::disconnectVar(m_d->ui->nameLabel, m_d->ui->nameLEdit);
        VarPlusGUI::disconnectVar(m_d->ui->EsLabel, m_d->ui->EsLEdit, m_d->ui->EsUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->epsYdLabel, m_d->ui->epsYdLEdit, m_d->ui->epsYdUMLabel, m_d->ui->epsYdCheckBox);
        VarPlusGUI::disconnectVar(m_d->ui->epsUkLabel, m_d->ui->epsUkLEdit, m_d->ui->epsUkUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->epsUdLabel, m_d->ui->epsUdLEdit, m_d->ui->epsUdUMLabel, m_d->ui->epsUdCheckBox);
        VarPlusGUI::disconnectVar(m_d->ui->ratioEpsUdEpsUkLabel, m_d->ui->ratioEpsUdEpsUkLEdit, m_d->ui->ratioEpsUdEpsUkUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->kLabel, m_d->ui->kLEdit, m_d->ui->kUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->fykLabel, m_d->ui->fykLEdit, m_d->ui->fykUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->ftkLabel, m_d->ui->ftkLEdit, m_d->ui->ftkUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->gammaSLabel, m_d->ui->gammaSLEdit, m_d->ui->gammaSUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->fydLabel, m_d->ui->fydLEdit, m_d->ui->fydUMLabel, m_d->ui->fydCheckBox);
        VarPlusGUI::disconnectVar(m_d->ui->fudLabel, m_d->ui->fudLEdit, m_d->ui->fudUMLabel, m_d->ui->fudCheckBox);
        VarPlusGUI::disconnectVar(m_d->ui->fRareLabel, m_d->ui->fRareLEdit, m_d->ui->fRareUMLabel, m_d->ui->fRareCheckBox);
        m_d->ui->modelULSRB1->setVar( 0 );
        m_d->ui->modelULSRB2->setVar( 0 );

        // disconnette i valori
        disconnect( m_d->steelCncr->modelfEps, SIGNAL(valueChanged(QString)), this, SLOT(plot()));
        disconnect( m_d->steelCncr->epsUd, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        disconnect( m_d->steelCncr->epsYd, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
    }

    m_d->steelCncr = dynamic_cast<SteelCncr*>(steelCncr);

    if( m_d->steelCncr != 0 ){
        // sincronizza i valori
        VarPlusGUI::connectVar(m_d->steelCncr->name, m_d->ui->nameLabel, m_d->ui->nameLEdit);
        VarPlusGUI::connectVar(m_d->steelCncr->E, m_d->ui->EsLabel, m_d->ui->EsLEdit, m_d->ui->EsUMLabel);
        VarPlusGUI::connectVar(m_d->steelCncr->epsYd, m_d->ui->epsYdLabel, m_d->ui->epsYdLEdit, m_d->ui->epsYdUMLabel, m_d->ui->epsYdCheckBox);
        VarPlusGUI::connectVar(m_d->steelCncr->epsUk, m_d->ui->epsUkLabel, m_d->ui->epsUkLEdit, m_d->ui->epsUkUMLabel);
        VarPlusGUI::connectVar(m_d->steelCncr->epsUd, m_d->ui->epsUdLabel, m_d->ui->epsUdLEdit, m_d->ui->epsUdUMLabel, m_d->ui->epsUdCheckBox);
        VarPlusGUI::connectVar(m_d->steelCncr->ratioEpsUdEpsUk,  m_d->ui->ratioEpsUdEpsUkLabel, m_d->ui->ratioEpsUdEpsUkLEdit, m_d->ui->ratioEpsUdEpsUkUMLabel);
        VarPlusGUI::connectVar(m_d->steelCncr->k, m_d->ui->kLabel, m_d->ui->kLEdit, m_d->ui->kUMLabel);
        VarPlusGUI::connectVar(m_d->steelCncr->fyk, m_d->ui->fykLabel, m_d->ui->fykLEdit, m_d->ui->fykUMLabel);
        VarPlusGUI::connectVar(m_d->steelCncr->ftk, m_d->ui->ftkLabel, m_d->ui->ftkLEdit, m_d->ui->ftkUMLabel);
        VarPlusGUI::connectVar(m_d->steelCncr->gammaS, m_d->ui->gammaSLabel, m_d->ui->gammaSLEdit, m_d->ui->gammaSUMLabel);
        VarPlusGUI::connectVar(m_d->steelCncr->fyd, m_d->ui->fydLabel, m_d->ui->fydLEdit, m_d->ui->fydUMLabel, m_d->ui->fydCheckBox);
        VarPlusGUI::connectVar(m_d->steelCncr->fud,  m_d->ui->fudLabel, m_d->ui->fudLEdit, m_d->ui->fudUMLabel, m_d->ui->fudCheckBox);
        VarPlusGUI::connectVar(m_d->steelCncr->fRare, m_d->ui->fRareLabel, m_d->ui->fRareLEdit, m_d->ui->fRareUMLabel, m_d->ui->fRareCheckBox);
        m_d->ui->modelULSRB1->setVar( m_d->steelCncr->modelfEps, 0 );
        m_d->ui->modelULSRB2->setVar( m_d->steelCncr->modelfEps, 1 );

        // sets slots to redraw model diagram
        connect( m_d->steelCncr->modelfEps, SIGNAL(valueChanged(QString)), this, SLOT(plot()));
        connect( m_d->steelCncr->epsUd, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        connect( m_d->steelCncr->epsYd, SIGNAL(valueChanged(QString)), this, SLOT(replot()) );
        plot();
    }
}

void SteelCncrGUI::updateStandardNameCBox( ){
    QSqlQueryModel queryModel;
    QString queryString = QString( "SELECT id, name, type from steelcncr" );

    m_d->ui->standardNameCBox->clear();
    QVariant typeData = m_d->ui->standardTypeCBox->itemData( m_d->ui->standardTypeCBox->currentIndex() );
    if(  typeData != QVariant::Invalid ) {
        if( typeData.toInt() > 0 ){
            queryString.append( QString(" WHERE type=") + QString::number(typeData.toInt()) );
        }
    }
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSteelCncrName ) );
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant data = queryModel.record(i).value("id");
        QString name = queryModel.record(i).value("name").toString();
        m_d->ui->standardNameCBox->insertItem(i, name, data );
    }
}

void SteelCncrGUI::loadStandardData(){
    QVariant steelData = m_d->ui->standardNameCBox->itemData( m_d->ui->standardNameCBox->currentIndex() );
    if( steelData != QVariant::Invalid ){
        QSqlQueryModel queryModel;
        QString queryString = QString( "SELECT id, name, fykName, fyk, ftkName, ftk, k, E, epsUk, ratioEpsUdEpsUk from steelcncr WHERE id=" ) + QString::number( steelData.toInt());
        queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSteelCncrName ) );
        if( queryModel.rowCount() > 0 ){
            m_d->steelCncr->fyk->setRichName( queryModel.record(0).value("fykName").toString() );
            m_d->steelCncr->fyk->setValueNormal( queryModel.record(0).value("fyk").toDouble() );
            m_d->steelCncr->ftk->setRichName( queryModel.record(0).value("ftkName").toString() );
            m_d->steelCncr->ftk->setValueNormal( queryModel.record(0).value("ftk").toDouble() );
            m_d->steelCncr->k->setValueNormal( queryModel.record(0).value("k").toDouble() );
            m_d->steelCncr->E->setValueNormal( queryModel.record(0).value("E").toDouble() );
            m_d->steelCncr->epsUk->setValueNormal( queryModel.record(0).value("epsUk").toDouble() );
            m_d->steelCncr->ratioEpsUdEpsUk->setValueNormal( queryModel.record(0).value("ratioEpsUdEpsUk").toDouble() );
        }
    }
}

void SteelCncrGUI::plot(){
    if( m_d->graph == NULL){
        m_d->graph = m_d->ui->plotSULS->addGraph();
    }

    // *** plots model curve ***
    updatePlotData();

    // finally, refresh the plot
    m_d->ui->plotSULS->replot();
}

void SteelCncrGUI::replot(){
    // *** update model plot if values change ***
    updatePlotData();

    // finally, refresh the plot
    m_d->ui->plotSULS->replot();
}

void SteelCncrGUI::updatePlotData(){
    QVector<double> xData;
    QVector<double> yData;
    // *** update values of plot's array ***
    if( m_d->steelCncr->modelfEps->valueNormal() == SteelCncr::horizontalTopBranch ){
        xData.append( 0.0 );
        yData.append( 0.0 );
        xData.append( m_d->steelCncr->epsYd->value() );
        yData.append( m_d->steelCncr->fyd->value() );
        xData.append( 1.1 * m_d->steelCncr->epsUd->value() );
        yData.append( m_d->steelCncr->fyd->value() );
    } else if( m_d->steelCncr->modelfEps->valueNormal() == SteelCncr::inclinedTopBranch ) {
        xData.append( 0.0 );
        yData.append( 0.0 );
        xData.append( m_d->steelCncr->epsYd->value() );
        yData.append( m_d->steelCncr->fyd->value() );
        xData.append( m_d->steelCncr->epsUd->value() );
        yData.append( m_d->steelCncr->fud->value() );
    }
    m_d->ui->plotSULS->xAxis->setRange( xData.first(), xData.last() );
    m_d->ui->plotSULS->yAxis->setRange( yData.first(), yData.last() );
    m_d->graph->setData( xData, yData );

}

void SteelCncrGUI::changeFykLabel(){
    QString label = m_d->steelCncr->fyk->richName();
    ChangeRichNameDialog dialog( &label, this );
    if( dialog.exec() == QDialog::Accepted ){
        m_d->steelCncr->fyk->setRichName( label );
    }
}

void SteelCncrGUI::changeFtkLabel(){
    QString label = m_d->steelCncr->ftk->richName();
    ChangeRichNameDialog dialog( &label, this );
    if( dialog.exec() == QDialog::Accepted ){
        m_d->steelCncr->ftk->setRichName( label );
    }
}
