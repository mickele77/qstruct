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
#include "materialmodule.h"

#include "materialmodel.h"
#include "materialmodelgui.h"
#include "materialdefaultgui.h"
#include "material.h"
#include "concretegui.h"
#include "concrete.h"
#include "steelcncrgui.h"
#include "steelcncr.h"
#include "frpgui.h"
#include "frp.h"
#include "steelgui.h"
#include "steel.h"
#include "timbergui.h"
#include "timber.h"

#include <QGridLayout>
#include <QIcon>
#include <QList>
#include <QDockWidget>
#include <QXmlStreamReader>

class MaterialModulePrivate{
public:
    MaterialModulePrivate( UnitMeasure * ump, MaterialModel * mModel, const QString & settingsFile, QWidget * parent ):
        model( mModel ),
        modelGUI( new MaterialModelGUI( mModel, parent ) ),
        currentMaterial( NULL ),
        currentMaterialGUI(NULL) {

        // Aggiungere qui le nuove GUI
        materialDefaultGUI = new MaterialDefaultGUI( ump, parent );
        materialDefaultGUI->hide();
        cncrGUI = new ConcreteGUI( ump, settingsFile, parent );
        cncrGUI->hide();
        steelCncrGUI = new SteelCncrGUI( ump, settingsFile, parent );
        steelCncrGUI->hide();
        frpGUI = new FRPGUI( ump, settingsFile, parent );
        frpGUI->hide();
        steelGUI = new SteelGUI( ump, settingsFile, parent );
        steelGUI->hide();
        timberGUI = new TimberGUI( ump, settingsFile, parent );
        timberGUI->hide();
    }

    MaterialModel * model;
    MaterialModelGUI * modelGUI;

    Material * currentMaterial;
    MaterialGUI * currentMaterialGUI;

    MaterialDefaultGUI * materialDefaultGUI;
    ConcreteGUI * cncrGUI;
    SteelCncrGUI * steelCncrGUI;
    FRPGUI * frpGUI;
    SteelGUI * steelGUI;
    TimberGUI * timberGUI;
};

MaterialModule::MaterialModule(UnitMeasure * ump,
                               MaterialModel * materialModel,
                               const QString & settingsFile,
                               QWidget *parent) :
    StructModule(ump, parent ),
    m_d( new MaterialModulePrivate( ump, materialModel, settingsFile, this ) ) {
    setLayout( new QGridLayout );

    QDockWidget * dock = new QDockWidget(richName() + " - " + trUtf8("Lista"));
    dock->setObjectName( "MaterialDock");
    dock->setWidget( m_d->modelGUI );
    m_panels->append( dock );

    connect( m_d->modelGUI, SIGNAL(currentChanged(Material*)), this, SLOT(setCurrentMaterial(Material*)));
}

QString MaterialModule::name(){
    return QString("Material");
}

QString MaterialModule::richName(){
    return trUtf8("Materiali");
}

QIcon MaterialModule::icon(){
    return QIcon(":/images/material.svg");
}

void MaterialModule::setCurrentMaterial( Material * mat ){
    if( m_d->currentMaterial != NULL ){
        if( m_d->currentMaterialGUI != NULL ){
            layout()->removeWidget( m_d->currentMaterialGUI );
            m_d->currentMaterialGUI->hide();
            m_d->currentMaterialGUI->setMaterial( NULL );
        }
        disconnect( m_d->currentMaterial, &Material::aboutToBeDeleted, this, &MaterialModule::setCurrentMaterialNULL );
    }

    m_d->currentMaterial = mat;
    if( dynamic_cast<Concrete *> (mat) != NULL ){
        m_d->currentMaterialGUI = m_d->cncrGUI;
    } else if( dynamic_cast<SteelCncr *> (mat) != NULL ){
        m_d->currentMaterialGUI = m_d->steelCncrGUI;
    } else if( dynamic_cast<FRP *> (mat) != NULL ){
        m_d->currentMaterialGUI = m_d->frpGUI;
    } else if( dynamic_cast<Steel *> (mat) != NULL ){
        m_d->currentMaterialGUI = m_d->steelGUI;
    } else if( dynamic_cast<Timber *> (mat) != NULL ){
        m_d->currentMaterialGUI = m_d->timberGUI;
    }  else if( dynamic_cast<Material *> (mat) != NULL ){
        m_d->currentMaterialGUI = m_d->materialDefaultGUI;
    } else {
        m_d->currentMaterialGUI = NULL;
    }

    if( m_d->currentMaterial != NULL ){
        if( m_d->currentMaterialGUI != NULL ) {
            m_d->currentMaterialGUI->setMaterial( mat );
            m_d->currentMaterialGUI->show();
            layout()->addWidget( m_d->currentMaterialGUI );
        }
        connect( m_d->currentMaterial, &Material::aboutToBeDeleted, this, &MaterialModule::setCurrentMaterialNULL );
    }
}

void MaterialModule::setCurrentMaterialNULL() {
    setCurrentMaterial( NULL );
}
