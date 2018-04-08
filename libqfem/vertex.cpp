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
#include "vertex.h"

#include "hypothesis.h"

#include "doubleplus.h"
#include "boolplus.h"
#include "uintplus.h"
#include "unitmeasure.h"
#include "glpoint.h"

#include <QList>

class VertexPrivate{
public:
    VertexPrivate( Hypothesis * hyp ):
        hypothesis(hyp) {
        glItem = NULL;
    };
    ~VertexPrivate(){
        if( glItem ){
            delete glItem;
        }
    };
    Hypothesis * hypothesis;
    QList<DoublePlus *> xPlusContainer;
    QList<BoolPlus *> isEtaSetPlusContainer;
    QList<DoublePlus *> etaPlusContainer;
    QList<DoublePlus *> fPlusContainer;
    GLPoint * glItem;
};

Vertex::Vertex( Hypothesis * hyp, UnitMeasure * ump, QObject *parent ):
    QObject( parent ),
    VarPlusContainer( ump, "Vertex", "P"),
    m_d( new VertexPrivate( hyp )){

    initVar();

    for( int i=0; i < m_d->xPlusContainer.size(); i++){
        connect( m_d->xPlusContainer.at(i), SIGNAL(valueChanged(QString)), this, SIGNAL(vertexChanged()));
    }
}

Vertex::Vertex(Hypothesis *hyp, UnitMeasure *ump, const QXmlStreamAttributes &attrs, QObject *parent):
    QObject(parent),
    VarPlusContainer( ump, "Vertex", "P"),
    m_d( new VertexPrivate( hyp )){
    initVar();
    loadFromXML( attrs );
}

Vertex::~Vertex() {
    delete m_d;
}

void Vertex::initVar(){
    for( int i=0; i < m_d->hypothesis->spaceDim(); i++ ){
        m_d->xPlusContainer.append( new DoublePlus( 0.0, "xPlusContainer" + QString::number(i), m_unitMeasure, UnitMeasure::length) );
        addVarToContainer( m_d->xPlusContainer.last() );
    }
    for( int i=0; i < m_d->hypothesis->nDOFVert(); i++ ){
        m_d->isEtaSetPlusContainer.append( new BoolPlus(false, "isEtaSetPlusContainer" + QString::number(i)) );
        addVarToContainer( m_d->isEtaSetPlusContainer.last() );
        m_d->etaPlusContainer.append( new DoublePlus( 0.0, "etaPlusContainer" + QString::number(i), m_unitMeasure, m_d->hypothesis->unitMeasureEta(i), false ));
        addVarToContainer( m_d->etaPlusContainer.last() );
        m_d->fPlusContainer.append( new DoublePlus( 0.0, "fPlusContainer" + QString::number(i), m_unitMeasure, m_d->hypothesis->unitMeasureF(i), true ) );
        addVarToContainer( m_d->fPlusContainer.last() );
        connect(m_d->etaPlusContainer.last(), &VarPlus::readOnlyChanged,
                m_d->fPlusContainer.last(), &VarPlus::setReadOnlyInv );
        connect(m_d->fPlusContainer.last(), &VarPlus::readOnlyChanged,
                m_d->etaPlusContainer.last(), &VarPlus::setReadOnlyInv );
    }
}

Hypothesis * Vertex::hypothesis(){
    return m_d->hypothesis;
}

GLItem * Vertex::glItem(){
    if( m_d->glItem == NULL ){
        m_d->glItem = new GLPoint();
        m_d->glItem->setColor( Qt::red );
        for( int i=0; i < m_d->xPlusContainer.size(); i++){
            connect( m_d->xPlusContainer.at(i), SIGNAL(valueChanged(QString)), this, SLOT(updateGLItem()));
        }
        updateGLItem();
    }
    return m_d->glItem;
}

void Vertex::updateGLItem(){
    if( m_d->glItem ){
        if(m_d->xPlusContainer.size() == 1 ){
            m_d->glItem->setP( m_d->xPlusContainer.at(0)->value(), 0.0, 0.0);
        } else if(m_d->xPlusContainer.size() == 2 ){
            m_d->glItem->setP( m_d->xPlusContainer.at(0)->value(), m_d->xPlusContainer.at(1)->value(), 0.0);
        } else if(m_d->xPlusContainer.size() >= 3 ){
            m_d->glItem->setP( m_d->xPlusContainer.at(0)->value(), m_d->xPlusContainer.at(1)->value(), m_d->xPlusContainer.at(2)->value());
        }
    }
}

DoublePlus * Vertex::xGlobal(int i){
    if( i < 0 || i >= m_d->xPlusContainer.size() )
        return NULL;
    else
        return m_d->xPlusContainer.at(i);
}

int Vertex::xGlobalSize() {
    return m_d->xPlusContainer.size();
}

BoolPlus * Vertex::isEtaSet(int i){
    if( i < 0 || i >= m_d->isEtaSetPlusContainer.size() )
        return NULL;
    else
        return m_d->isEtaSetPlusContainer.at(i);
}

DoublePlus * Vertex::etaGlobal(int i){
    if( i < 0 || i >= m_d->etaPlusContainer.size() )
        return NULL;
    else
        return m_d->etaPlusContainer.at(i);
}

DoublePlus * Vertex::fGlobal(int i){
    if( i < 0 || i >= m_d->fPlusContainer.size() )
        return NULL;
    else
        return m_d->fPlusContainer.at(i);
}
