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
#include "sectionmodule.h"

#include "sectionthincontsteelgui.h"
#include "sectionthincontgui.h"
#include "sectionthingui.h"
#include "sectionpointsgui.h"
#include "sectionrcncrgui.h"
#include "sectionsteelgui.h"
#include "sectiontimbergui.h"
#include "sectionprofilegui.h"
#include "sectiondefaultgui.h"

#include "section.h"
#include "sectionthincontsteel.h"
#include "sectionthincont.h"
#include "sectionthin.h"
#include "sectionpoints.h"
#include "sectioncncr.h"
#include "sectionrcncr.h"
#include "sectionsteel.h"
#include "sectiontimber.h"
#include "sectionprofile.h"

#include "sectionmodel.h"
#include "sectionmodelgui.h"

#include "qgraphicsitemviewer.h"

#include <QGraphicsItemGroup>
#include <QGridLayout>
#include <QIcon>
#include <QList>
#include <QDockWidget>
#include <QGraphicsScene>
#include <QSize>
#include <QWidget>

class SectionWidget : public QWidget {
private:
    QWidget emptyWidget;
    QList<QWidget *> widgetList;
public:
    SectionWidget( const QList<QWidget *> & guiList,
                   QWidget * parent = 0 ):
        QWidget(parent),
        emptyWidget(this),
        widgetList(guiList) {
        QGridLayout * layout = new QGridLayout( this );
        layout->addWidget(&emptyWidget);
        for( int i=0; i < widgetList.size(); ++i ){
            widgetList.at(i)->setParent(this);
            layout->addWidget( widgetList.at(i), 0,i );
        }
    }
    QSize sizeHint() const{
        QSize s = emptyWidget.sizeHint();
        for( int i=0; i < widgetList.size(); ++i ){
            s = s.expandedTo( widgetList.at(i)->size() );
        }
        return s;
    }
};

class SectionModulePrivate{
public:
    SectionModulePrivate(UnitMeasure * ump,
                         MaterialModel * materialModel,
                         SectionModel * sectionModel,
                         const QString & settingsFile,
                         QWidget *parent):
        model( sectionModel ){
        currentSection = NULL;

        currentSectionGUI = NULL;
        QList<QWidget *> widgetList;
        sectionDefaultGUI = new SectionDefaultGUI( ump, materialModel, parent );
        sectionDefaultGUI->hide();
        widgetList.append(sectionDefaultGUI);;
        sectionPointsGUI = new SectionPointsGUI( ump, materialModel, parent );
        sectionPointsGUI->hide();
        widgetList.append(sectionPointsGUI);
        sectionRCncrGUI = new SectionRCncrGUI( ump, materialModel, settingsFile, parent );
        sectionRCncrGUI->hide();
        widgetList.append(sectionRCncrGUI);
        sectionSteelGUI = new SectionSteelGUI( ump, materialModel, settingsFile, parent );
        sectionSteelGUI->hide();
        widgetList.append(sectionSteelGUI);
        sectionTimberGUI = new SectionTimberGUI( ump, materialModel, parent );
        sectionTimberGUI->hide();
        widgetList.append(sectionTimberGUI);
        sectionThinGUI = new SectionThinGUI( ump, materialModel, parent );
        sectionThinGUI->hide();
        widgetList.append(sectionThinGUI);
        sectionThinContGUI = new SectionThinContGUI( ump, materialModel, parent );
        sectionThinContGUI->hide();
        widgetList.append(sectionThinContGUI);
        sectionThinContSteelGUI = new SectionThinContSteelGUI( ump, materialModel, parent );
        sectionThinContSteelGUI->hide();
        widgetList.append(sectionThinContSteelGUI);
        sectionProfileGUI = new SectionProfileGUI( ump, materialModel, parent );
        sectionProfileGUI->hide();
        widgetList.append(sectionProfileGUI);
        sectionWidget = new SectionWidget( widgetList, parent );
        sectionModelGUI = new SectionModelGUI( sectionModel, materialModel, parent );

        sectionViewer = new QGraphicsItemViewer( parent );
    }

    SectionModel * model;
    Section * currentSection;

    SectionWidget * sectionWidget;
    SectionGUI * currentSectionGUI;
    SectionDefaultGUI * sectionDefaultGUI;
    SectionPointsGUI * sectionPointsGUI;
    SectionRCncrGUI * sectionRCncrGUI;
    SectionSteelGUI * sectionSteelGUI;
    SectionTimberGUI * sectionTimberGUI;
    SectionThinGUI * sectionThinGUI;
    SectionThinContGUI * sectionThinContGUI;
    SectionThinContSteelGUI * sectionThinContSteelGUI;
    SectionProfileGUI * sectionProfileGUI;

    SectionModelGUI * sectionModelGUI;

    QDockWidget * dockSectionViewerPanel;
    QGraphicsItemViewer * sectionViewer;
};

SectionModule::SectionModule(UnitMeasure * ump,
                             MaterialModel * materialModel,
                             SectionModel * sectionModel,
                             const QString & settingsFile,
                             QWidget *parent) :
    StructModule(ump, parent ),
    m_d( new SectionModulePrivate( ump, materialModel, sectionModel, settingsFile, this )){

    setLayout( new QGridLayout(this));
    layout()->addWidget( m_d->sectionWidget );

    QDockWidget * dock = new QDockWidget(richName() + " - " + trUtf8("Lista"));
    dock->setObjectName( "SectionList");
    dock->setWidget( m_d->sectionModelGUI );
    m_panels->append( dock );

    m_d->dockSectionViewerPanel = new QDockWidget( QObject::trUtf8("Sezioni") + " - " + QObject::trUtf8("Disegno"));
    m_d->dockSectionViewerPanel->setObjectName( "SectionViewer" );
    m_d->dockSectionViewerPanel->setWidget( m_d->sectionViewer );
    m_panels->append(  m_d->dockSectionViewerPanel );

    connect( m_d->sectionModelGUI, SIGNAL(currentChanged(Section *)), this, SLOT(setCurrentSection(Section *)));
}

QString SectionModule::name(){
    return QString("Section");
}

QString SectionModule::richName(){
    return trUtf8("Sezioni");
}

QIcon SectionModule::icon(){
    return QIcon(":/images/section.svg");
}

void SectionModule::setCurrentSection( Section * sect ){
    if( m_d->currentSection != sect ){
        if( m_d->currentSectionGUI != NULL ){
            m_d->sectionViewer->removeQGraphicsItem( m_d->currentSectionGUI->extraGraphicsItem() );
        }
        if( m_d->currentSection != NULL  ){
            if( m_d->currentSection->qGraphicsItem() != NULL ){
                m_d->sectionViewer->removeQGraphicsItem( m_d->currentSection->qGraphicsItem() );
            }
            disconnect( m_d->currentSection, &Section::aboutToBeDeleted, this, &SectionModule::setCurrentSectionNULL );
        }

        m_d->currentSection = sect;

        // imposta il sectionPanel della sezione corrente
        SectionGUI * newSectionPanel;
        if( dynamic_cast<SectionRCncr *> (m_d->currentSection) != NULL ){
            newSectionPanel = m_d->sectionRCncrGUI;
        } else if( dynamic_cast<SectionSteel *> (m_d->currentSection) != NULL ){
            newSectionPanel = m_d->sectionSteelGUI;
        } else if( dynamic_cast<SectionTimber *> (m_d->currentSection) != NULL ){
            newSectionPanel = m_d->sectionTimberGUI;
        } else if( dynamic_cast<SectionPoints *> (m_d->currentSection) != NULL ){
            newSectionPanel = m_d->sectionPointsGUI;
        } else if( dynamic_cast<SectionThinContSteel *> (m_d->currentSection) != NULL ){
            newSectionPanel = m_d->sectionThinContSteelGUI;
        } else if( dynamic_cast<SectionThinCont *> (m_d->currentSection) != NULL ){
            newSectionPanel = m_d->sectionThinContGUI;
        } else if( dynamic_cast<SectionThin *> (m_d->currentSection) != NULL ){
            newSectionPanel = m_d->sectionThinGUI;
        } else if( dynamic_cast<SectionProfile *> (m_d->currentSection) != NULL ){
            newSectionPanel = m_d->sectionProfileGUI;
        } else if( dynamic_cast<Section *> (m_d->currentSection) != NULL ){
            newSectionPanel = m_d->sectionDefaultGUI;
        } else {
            newSectionPanel = NULL;
        }
        if( newSectionPanel != m_d->currentSectionGUI ){
            if( m_d->currentSectionGUI != NULL ){
                m_d->currentSectionGUI->hide();
                m_d->currentSectionGUI->setSection(NULL);
            }
            m_d->currentSectionGUI = newSectionPanel;
            if( m_d->currentSectionGUI != NULL ){
                m_d->currentSectionGUI->show();
            }
        }

        // imposta la sezione del sectionPanel
        if( m_d->currentSectionGUI != NULL ) {
            m_d->currentSectionGUI->setSection(m_d->currentSection);
            m_d->sectionViewer->addQGraphicsItem( m_d->currentSectionGUI->extraGraphicsItem() );
        }
        // imposta il disegno della sezione
        if( m_d->currentSection != NULL ){
            if( m_d->currentSection->qGraphicsItem() != NULL ){
                m_d->sectionViewer->addQGraphicsItem( m_d->currentSection->qGraphicsItem() );
            }
            connect( m_d->currentSection, &Section::aboutToBeDeleted, this, &SectionModule::setCurrentSectionNULL );
        }
    }
}

void SectionModule::setCurrentSectionNULL(){
    setCurrentSection( NULL );
}
