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
#include "sectiongui.h"

#include "materialmodel.h"
#include "material.h"

#include <qstringplus.h>
#include "uintplus.h"

#include <QGraphicsItemGroup>
#include <QComboBox>
#include <QString>

SectionGUI::SectionGUI( UnitMeasure * ump, MaterialModel * matModel, QWidget *parent ):
    QWidget( parent ),
    VarPlusContainer( ump, "", "" ),
    m_materialModel(matModel),
    m_graphicsItem( new QGraphicsItemGroup() ),
    m_materialComboBox(NULL){
#ifdef WIN32
    setStyleSheet("QWidget {font-size: 14px}");
#endif
}

void SectionGUI::refreshMaterialComboBox (){
    if( m_materialComboBox  != NULL ){
        disconnect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );

        QList< Material * > matList = materialsList();
        m_materialComboBox->clear();
        for( QList< Material * >::iterator i=matList.begin(); i!=matList.end(); ++i ){
            m_materialComboBox->addItem( (*i)->name->valueStr(), QVariant((*i)->id->valueNormal() ) );
        }
        if( material() != NULL ){
            m_materialComboBox->setCurrentIndex( m_materialComboBox->findData( QVariant(material()->id->valueNormal())));
        } else {
             m_materialComboBox->setCurrentIndex( -1 );
        }
        connect( m_materialComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SectionGUI::setMaterialFromComboBox );
    }
}

QList< Material * > SectionGUI::materialsList(){
    return m_materialModel->materialsList( MaterialModel::GenericMaterial );
}

void SectionGUI::showEvent ( QShowEvent *){
    refreshMaterialComboBox();
}

QGraphicsItemGroup *SectionGUI::extraGraphicsItem(){
    return m_graphicsItem;
}

