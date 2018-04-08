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
#include "qfem2dgui.h"

#include "femstructure.h"
#include "femstructureviewer.h"
#include "glgroup.h"
#include "glwindow.h"
#include "vertexmodel.h"
#include "elementmodel.h"

#include "glline.h"

#include <QGridLayout>
#include <QGraphicsScene>

class QFem2DGUIPrivate {
public:
    QFem2DGUIPrivate() {
        scene = new GLGroup();
        extraGLItem = NULL;
    }
    ~QFem2DGUIPrivate(){
    }

    GLGroup * scene;
    GLItem * extraGLItem;
};

QFem2DGUI::QFem2DGUI( FEMStructure * str, QWidget *parent ) :
    QWidget(parent),
    m_structure( str ),
    m_d( new QFem2DGUIPrivate()) {

    setLayout( new QGridLayout() );

    m_structure->elementModel()->glItem()->setGroup( m_d->scene );
    m_structure->vertexModel()->glItem()->setGroup( m_d->scene );

    GLWindow * viewer = new GLWindow( m_d->scene );
    QWidget * viewerWidget = QWidget::createWindowContainer( viewer );
    ((QGridLayout *) layout() )->addWidget( viewerWidget , 0, 0 );

    // ((QGridLayout *) layout() )->addWidget( new FEMStructureViewer(m_structure, this ), 1, 0 );
}

QFem2DGUI::~QFem2DGUI(){
    delete m_d;
}

void QFem2DGUI::setExtraGLItem( GLItem * item ){
    if( m_d->extraGLItem != NULL ){
        delete m_d->extraGLItem;
    }
    m_d->extraGLItem = item;
    if( item ){
        item->setGroup( m_d->scene );
    }
}
