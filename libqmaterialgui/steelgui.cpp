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
#include "steelgui.h"
#include "ui_steelgui.h"

#include <QtSql>
#include <QMessageBox>

#include <QSqlQueryModel>
#include <QSettings>

#include "steel.h"

#include "qstringplus.h"
#include "doubleplus.h"

#include "varplusguiutility.h"

class SteelGUIPrivate{
public:
    SteelGUIPrivate(const QString &sFile ):
        settingsFile(sFile),
        dbSteelName("steelDB"),
        steel(NULL ) {
    }
    QString settingsFile;
    QString dbSteelFileName;
    QString dbSteelName;

    Ui::SteelGUI * ui;

    Steel * steel;
};

SteelGUI::SteelGUI(UnitMeasure * um, const QString & settingsFile, QWidget *parent) :
    MaterialGUI(um, parent),
    m_d( new SteelGUIPrivate(settingsFile) ){

    // setup UI
    m_d->ui = new Ui::SteelGUI;
    m_d->ui->setupUi(this);

    // apre il database
    if( !QSqlDatabase::contains( m_d->dbSteelName )){
        QSqlDatabase dbSteel = QSqlDatabase::addDatabase("QSQLITE", m_d->dbSteelName );
        QSettings settings( m_d->settingsFile, QSettings::IniFormat );
        m_d->dbSteelFileName = settings.value("dbSteelFileName", QApplication::applicationDirPath() + "/../resources/steel.db").toString();
        dbSteel.setDatabaseName(m_d->dbSteelFileName);
        if (!dbSteel.open() ) {
            QMessageBox::critical(0, trUtf8("Non riesco ad aprire il database degli acciai da carpenteria"),
                                  trUtf8("Non riesco a trovare il file") + " " + "steel.db", QMessageBox::Cancel);
        } else {
            loadStandardComboBox();
        }
    } else {
        loadStandardComboBox();
    }
}

void SteelGUI::loadStandardComboBox(){
    // Aggiunge i dati in standardTypeComboBox
    QString queryString = QString( "SELECT idType, nameType from type" );
    QSqlQueryModel queryModel;
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSteelName ) );
    m_d->ui->standardTypeCBox->insertItem(0, QString("------"), QVariant(-1));
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant data = queryModel.record(i).value("idType");
        QString name = queryModel.record(i).value("nameType").toString();
        m_d->ui->standardTypeCBox->insertItem((i+1), name, data );
    }
    m_d->ui->standardNameCBox->setCurrentIndex( 0 );
    connect( m_d->ui->standardTypeCBox, SIGNAL( currentIndexChanged(QString)), this, SLOT( updateStandardUNICBox()));
    connect( m_d->ui->standardTypeCBox, SIGNAL( currentIndexChanged(QString)), this, SLOT( updateStandardNameCBox()));

    // Aggiunge i dati in standardUNICBox
    updateStandardUNICBox();
    connect( m_d->ui->standardUNICBox, SIGNAL( currentIndexChanged(QString)), this, SLOT( updateStandardNameCBox()));
    // Aggiunge i dati in standardNameComboBox
    updateStandardNameCBox();

    connect( m_d->ui->loadStandardPButton, SIGNAL(clicked()), this, SLOT(loadStandardData()));
}

SteelGUI::~SteelGUI() {
    QSqlDatabase dbSteel = QSqlDatabase::database( m_d->dbSteelName );
    dbSteel.close();

    QSettings settings(m_d->settingsFile,  QSettings::IniFormat);
    settings.setValue("dbSteelFileName", m_d->dbSteelFileName);

    delete m_d->ui;
}

void SteelGUI::setMaterial( Material * steel ){
    if( m_d->steel != 0 ){
        VarPlusGUI::disconnectVar(m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::disconnectVar(m_d->ui->gammaWLabel, m_d->ui->gammaWLEdit, m_d->ui->gammaWUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->EsLabel,  m_d->ui->EsLEdit, m_d->ui->EsUMLabel );
        VarPlusGUI::disconnectVar(m_d->ui->GsLabel, m_d->ui->GsLEdit,  m_d->ui->GsUMLabel, m_d->ui->GsCheckBox);
        VarPlusGUI::disconnectVar(m_d->ui->nusLabel, m_d->ui->nusLEdit, m_d->ui->nusUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->alphaLabel, m_d->ui->alphaLEdit, m_d->ui->alphaUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->fykLabel, m_d->ui->fykLEdit, m_d->ui->fykUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->fyk40Label, m_d->ui->fyk40LEdit, m_d->ui->fyk40UMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->ftkLabel, m_d->ui->ftkLEdit, m_d->ui->ftkUMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->ftk40Label, m_d->ui->ftk40LEdit, m_d->ui->ftk40UMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->gammaS0Label, m_d->ui->gammaS0LEdit, m_d->ui->gammaS0UMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->gammaS1Label, m_d->ui->gammaS1LEdit, m_d->ui->gammaS1UMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->gammaS2Label, m_d->ui->gammaS2LEdit, m_d->ui->gammaS2UMLabel);
        VarPlusGUI::disconnectVar(m_d->ui->fyd0Label, m_d->ui->fyd0LEdit, m_d->ui->fyd0UMLabel, m_d->ui->fyd0CheckBox);
        VarPlusGUI::disconnectVar(m_d->ui->fyd1Label, m_d->ui->fyd1LEdit, m_d->ui->fyd1UMLabel, m_d->ui->fyd1CheckBox);
        VarPlusGUI::disconnectVar(m_d->ui->ftd2Label, m_d->ui->ftd2LEdit, m_d->ui->ftd2UMLabel, m_d->ui->ftd2CheckBox);
        VarPlusGUI::disconnectVar(m_d->ui->fyd0_40Label, m_d->ui->fyd0_40LEdit, m_d->ui->fyd0_40UMLabel, m_d->ui->fyd0_40CheckBox);
        VarPlusGUI::disconnectVar(m_d->ui->fyd1_40Label, m_d->ui->fyd1_40LEdit, m_d->ui->fyd1_40UMLabel, m_d->ui->fyd1_40CheckBox);
        VarPlusGUI::disconnectVar(m_d->ui->ftd2_40Label, m_d->ui->ftd2_40LEdit, m_d->ui->ftd2_40UMLabel, m_d->ui->ftd2_40CheckBox);
    }

    m_d->steel = dynamic_cast<Steel*>(steel);

    if( m_d->steel != 0 ){
        VarPlusGUI::connectVar(m_d->steel->name, m_d->ui->nameLabel, m_d->ui->nameLEdit );
        VarPlusGUI::connectVar(m_d->steel->gammaW, m_d->ui->gammaWLabel, m_d->ui->gammaWLEdit, m_d->ui->gammaWUMLabel);
        VarPlusGUI::connectVar( m_d->steel->E, m_d->ui->EsLabel, m_d->ui->EsLEdit, m_d->ui->EsUMLabel);
        VarPlusGUI::connectVar( m_d->steel->G, m_d->ui->GsLabel, m_d->ui->GsLEdit, m_d->ui->GsUMLabel, m_d->ui->GsCheckBox);
        VarPlusGUI::connectVar( m_d->steel->nu, m_d->ui->nusLabel, m_d->ui->nusLEdit, m_d->ui->nusUMLabel);
        VarPlusGUI::connectVar( m_d->steel->alpha, m_d->ui->alphaLabel, m_d->ui->alphaLEdit, m_d->ui->alphaUMLabel);
        VarPlusGUI::connectVar( m_d->steel->fyk, m_d->ui->fykLabel, m_d->ui->fykLEdit, m_d->ui->fykUMLabel);
        VarPlusGUI::connectVar( m_d->steel->fyk40, m_d->ui->fyk40Label, m_d->ui->fyk40LEdit, m_d->ui->fyk40UMLabel);
        VarPlusGUI::connectVar( m_d->steel->ftk, m_d->ui->ftkLabel, m_d->ui->ftkLEdit, m_d->ui->ftkUMLabel);
        VarPlusGUI::connectVar( m_d->steel->ftk40, m_d->ui->ftk40Label, m_d->ui->ftk40LEdit, m_d->ui->ftk40UMLabel);
        VarPlusGUI::connectVar( m_d->steel->gammaS(0), m_d->ui->gammaS0Label, m_d->ui->gammaS0LEdit, m_d->ui->gammaS0UMLabel);
        VarPlusGUI::connectVar( m_d->steel->gammaS(1), m_d->ui->gammaS1Label, m_d->ui->gammaS1LEdit, m_d->ui->gammaS1UMLabel);
        VarPlusGUI::connectVar( m_d->steel->gammaS(2), m_d->ui->gammaS2Label, m_d->ui->gammaS2LEdit, m_d->ui->gammaS2UMLabel);
        VarPlusGUI::connectVar( m_d->steel->fyd(0), m_d->ui->fyd0Label, m_d->ui->fyd0LEdit, m_d->ui->fyd0UMLabel, m_d->ui->fyd0CheckBox);
        VarPlusGUI::connectVar( m_d->steel->fyd(1), m_d->ui->fyd1Label, m_d->ui->fyd1LEdit, m_d->ui->fyd1UMLabel, m_d->ui->fyd1CheckBox);
        VarPlusGUI::connectVar( m_d->steel->fyd(2), m_d->ui->ftd2Label, m_d->ui->ftd2LEdit, m_d->ui->ftd2UMLabel, m_d->ui->ftd2CheckBox);
        VarPlusGUI::connectVar( m_d->steel->fyd40(0), m_d->ui->fyd0_40Label, m_d->ui->fyd0_40LEdit, m_d->ui->fyd0_40UMLabel, m_d->ui->fyd0_40CheckBox);
        VarPlusGUI::connectVar( m_d->steel->fyd40(1), m_d->ui->fyd1_40Label, m_d->ui->fyd1_40LEdit, m_d->ui->fyd1_40UMLabel, m_d->ui->fyd1_40CheckBox);
        VarPlusGUI::connectVar( m_d->steel->fyd40(2), m_d->ui->ftd2_40Label, m_d->ui->ftd2_40LEdit, m_d->ui->ftd2_40UMLabel, m_d->ui->ftd2_40CheckBox);
    }
}

void SteelGUI::updateStandardUNICBox( void ){
    QSqlQueryModel queryModel;
    QString queryString = QString("SELECT idUNI, nameUNI, type FROM uni" );

    m_d->ui->standardUNICBox->clear();
    m_d->ui->standardUNICBox->insertItem(0, QString("------"), QVariant(-1) );
    QVariant typeData = m_d->ui->standardTypeCBox->itemData( m_d->ui->standardTypeCBox->currentIndex() );
    if(  typeData != QVariant::Invalid ) {
        if( typeData.toInt() > 0 ){
            queryString.append( QString(" WHERE type=") + QString::number(typeData.toInt()) );
        }
    }
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSteelName ) );
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant data = queryModel.record(i).value("idUNI");
        QString name = queryModel.record(i).value("nameUNI").toString();
        m_d->ui->standardUNICBox->insertItem((i+1), name, data );
    }
}

void SteelGUI::updateStandardNameCBox( void ){
    QSqlQueryModel queryModel;
    QString queryString = QString( "SELECT id, name, uni from steel" );

    QVariant typeData = m_d->ui->standardTypeCBox->itemData( m_d->ui->standardTypeCBox->currentIndex() );
    QVariant uniData = m_d->ui->standardUNICBox->itemData( m_d->ui->standardUNICBox->currentIndex() );

    if(  uniData != QVariant::Invalid && uniData.toInt() > 0 ){
        queryString.append( QString(" WHERE uni=") + QString::number(uniData.toInt()) );
    } else if(  typeData != QVariant::Invalid && typeData.toInt() > -1 ){
        queryString.append( QString(" INNER JOIN uni ON uni=idUNI WHERE type=") + QString::number(typeData.toInt()) );
    }

    m_d->ui->standardNameCBox->clear();
    queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSteelName ) );
    for (int i = 0; i < queryModel.rowCount(); i++) {
        QVariant data = queryModel.record(i).value("id");
        QString name = queryModel.record(i).value("name").toString();
        m_d->ui->standardNameCBox->insertItem(i, name, data );
    }
}

void SteelGUI::loadStandardData(){
    QVariant steelData = m_d->ui->standardNameCBox->itemData( m_d->ui->standardNameCBox->currentIndex() );
    if( steelData != QVariant::Invalid ){
        QSqlQueryModel queryModel;
        QString queryString = QString( "SELECT id, name, uni, description, fyk, ftk, fyk40, ftk40 from steel WHERE id=") + QString::number( steelData.toInt());
        queryModel.setQuery( queryString, QSqlDatabase::database( m_d->dbSteelName ) );
        if( queryModel.rowCount() > 0 ){
            m_d->steel->fyk->setValueNormal( queryModel.record(0).value("fyk").toDouble() );
            m_d->steel->ftk->setValueNormal( queryModel.record(0).value("ftk").toDouble() );
            m_d->steel->fyk40->setValueNormal( queryModel.record(0).value("fyk40").toDouble() );
            m_d->steel->ftk40->setValueNormal( queryModel.record(0).value("ftk40").toDouble() );
        }
    }
}
