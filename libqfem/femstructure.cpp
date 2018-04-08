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
#include "femstructure.h"

#include "beam.h"
#include "vertexmodel.h"
#include "vertex.h"
#include "elementmodel.h"
#include "element.h"
#include "sectionmodel.h"
#include "hypothesis.h"
#include "qgraphicspointitem.h"
#include "doubleplus.h"
#include "boolplus.h"
#include "glgroup.h"

#include <Eigen/Dense>

#include <QGraphicsWidget>

// DEBUG
// #include <iostream>

class FEMStructurePrivate{
public:
    FEMStructurePrivate( SectionModel * sm, Section * s, Hypothesis * hyp, UnitMeasure * ump, QObject * parent){
        hypothesis = hyp;
        vertexModel = new VertexModel( hyp, ump, parent );
        if( s == NULL ){
            elementModel = new ElementModel( vertexModel, sm, hyp, ump, parent );
        } else        {
            elementModel = new ElementModel( vertexModel, s, hyp, ump, parent );
        }
        graphicsItem = new QGraphicsWidget();
        vertexModel->graphicsItem()->setParentItem( graphicsItem );
        elementModel->graphicsItem()->setParentItem( graphicsItem );
    };

    Hypothesis * hypothesis;
    VertexModel * vertexModel;
    QList<Vertex *> usedVertexList;
    ElementModel * elementModel;

    /** Spostamenti nodali */
    Eigen::VectorXd eta;
    /** Carichi nodali */
    Eigen::VectorXd f;
    /** Carichi nodali equivalenti determinati da azioni infranodali */
    Eigen::VectorXd f0;

    /** Matrice di rigidezza della struttura */
    Eigen::MatrixXd K;

    // Grafica
    QGraphicsWidget * graphicsItem;
};

// graphics


FEMStructure::FEMStructure( SectionModel * sm, Hypothesis * hyp, UnitMeasure * ump, QObject * parent):
    QObject(parent),
    m_d( new FEMStructurePrivate( sm, NULL, hyp, ump, this ) ) {
    connect( m_d->vertexModel, SIGNAL(vertexChanged(Vertex*)), this, SLOT(updateQGraphics(Vertex*)) );
}

FEMStructure::FEMStructure( Section * s, Hypothesis * hyp, UnitMeasure * ump, QObject * parent):
    QObject(parent),
    m_d( new FEMStructurePrivate( NULL, s, hyp, ump, this ) ) {
    connect( m_d->vertexModel, SIGNAL(vertexChanged(Vertex*)), this, SLOT(updateQGraphics(Vertex*)) );
}

FEMStructure::~FEMStructure(){

}

VertexModel * FEMStructure::vertexModel(){
    return m_d->vertexModel;
}

ElementModel * FEMStructure::elementModel(){
    return m_d->elementModel;
}

int FEMStructure::nDOF(){
    return m_d->usedVertexList.size() * m_d->hypothesis->nDOFVert();
}

bool FEMStructure::isVertexUsed( Vertex * v ){
    int i = m_d->usedVertexList.indexOf( v );
    if( i == -1 ){
        return false;
    } else {
        return true;
    }
}

void FEMStructure::setUsedVertexList() {
    m_d->usedVertexList.clear();
    VertexModel::VertexIterator vertIterator = m_d->vertexModel->begin();
    while( vertIterator != m_d->vertexModel->end() ){
        ElementModel::ElementIterator elemIter = m_d->elementModel->begin();
        while( elemIter != m_d->elementModel->end() ){
            if( (*elemIter)->isVertexUsed(*vertIterator) ){
                if( !(m_d->usedVertexList.contains(*vertIterator)) ){
                    m_d->usedVertexList.append( *vertIterator );
                }
            }
            elemIter++;
        }
        vertIterator++;
    }
}

void FEMStructure::setProblem(){
    setUsedVertexList();

    int ndofv = m_d->hypothesis->nDOFVert();
    int ndof = m_d->usedVertexList.size() * ndofv;

    m_d->eta = Eigen::VectorXd::Zero( ndof );
    m_d->f = Eigen::VectorXd::Zero( ndof );
    m_d->f0 = Eigen::VectorXd::Zero( ndof );
    m_d->K = Eigen::MatrixXd::Zero( ndof, ndof );

    // Calcola matrice globale e vettore temini noti
    ElementModel::ElementIterator elemIter = m_d->elementModel->begin();
    while( elemIter != m_d->elementModel->end() ){
        (*elemIter)->setKF0Global();

        QList<Vertex*>::iterator vertIterator1 = m_d->usedVertexList.begin();
        int i1=0;
        QList<Vertex*>::iterator vertIterator2;
        int i2;
        while( vertIterator1 != m_d->usedVertexList.end() ){
            vertIterator2 = m_d->usedVertexList.begin();
            i2 = 0;
            while( vertIterator2 != m_d->usedVertexList.end() ){
                m_d->K.block( i1*ndofv, i2*ndofv, ndofv, ndofv ) += (*elemIter)->KGlobalVert( *vertIterator1, *vertIterator2 );
                vertIterator2++;
                i2++;
            }
            m_d->f0.block( i1*ndofv, 0, ndofv, 1 ) += (*elemIter)->f0GlobalVert( *vertIterator1 );
            i1++;
            vertIterator1++;
        }
        elemIter++;
    }
}

void FEMStructure::solveProblem(){
    int ndofv = m_d->hypothesis->nDOFVert();

    Eigen::MatrixXd KCalc = m_d->K;
    Eigen::VectorXd fCalc = - m_d->f0;
    Eigen::VectorXd etaCalc = Eigen::VectorXd::Zero( fCalc.rows() );

    // Impone condizioni al contorno prendendole dai vertici
    QList<Vertex*>::iterator usedVertIter = m_d->usedVertexList.begin();
    int i=0;
    while( usedVertIter != m_d->usedVertexList.end() ){
        for( int j=0; j < ndofv; j++ ){
            if( ( (*usedVertIter)->isEtaSet(j)->valueNormal()) ) {
                fCalc -= KCalc.col( i ) * (*usedVertIter)->etaGlobal(j)->valueNormal();
            } else {
                fCalc(i) += (*usedVertIter)->fGlobal(j)->valueNormal();
            }
            i++;
        }
        usedVertIter++;
    }
    // Per preservare la simmetria ugugliamo gli spostamenti nodali incogniti ai valori imposti
    usedVertIter = m_d->usedVertexList.begin();
    i = 0;
    while( usedVertIter != m_d->usedVertexList.end() ){
        for( int j=0; j < ndofv; j++ ){
            if( (*usedVertIter)->isEtaSet(j)->valueNormal() ){
                KCalc.col( i ).setZero();
                KCalc.row( i ).setZero();
                KCalc(i,i) = 1;
                fCalc(i) = (*usedVertIter)->etaGlobal(j)->valueNormal();
            }
            i++;
        }
        usedVertIter++;
    }

    // Risolve il sistema
    Eigen::FullPivLU<Eigen::MatrixXd> solver( KCalc );
    etaCalc = solver.solve( fCalc );

    fCalc = m_d->K * etaCalc + m_d->f0;

    // Passa i valori delle forze esterne e degli spostamenti ai vertici
    usedVertIter = m_d->usedVertexList.begin();
    i = 0;
    while( usedVertIter != m_d->usedVertexList.end() ){
        for( int j=0; j < ndofv; j++ ){
            if( (*usedVertIter)->isEtaSet(j)->valueNormal() ){
                (*usedVertIter)->fGlobal(j)->setValueNormal( fCalc(i) );
            } else {
                (*usedVertIter)->etaGlobal(j)->setValueNormal( etaCalc(i) );
            }
            i++;
        }
        usedVertIter++;
    }

    // Chiede agli elementi di calcolare i valori delle forze e degli spostamenti locali
    ElementModel::ElementIterator elemIter = m_d->elementModel->begin();
    while( elemIter != m_d->elementModel->end() ){
        (*elemIter)->setFEtaLocal();
        elemIter++;
    }
}

QGraphicsItem * FEMStructure::graphicsItem(){
    return m_d->graphicsItem;
}

QRectF FEMStructure::graphicsItemBoudingRect(){
    return m_d->vertexModel->graphicsItemBoudingRect();
}

GLItem * FEMStructure::glItemDeformed( double scale ){
    GLGroup * ret = new GLGroup;

    ElementModel::ElementIterator elemIter = m_d->elementModel->begin();
    while( elemIter != m_d->elementModel->end() ){
        (*elemIter)->glItemDeformed(20, scale)->setGroup( ret );
        elemIter++;
    }

    return ret;
}

GLItem * FEMStructure::glItemBeamFPLocal( int nFP, double scale ){
    GLGroup * ret = new GLGroup;

    ElementModel::ElementIterator elemIter = m_d->elementModel->begin();
    while( elemIter != m_d->elementModel->end() ){
        Beam * b = dynamic_cast<Beam *> (*elemIter);
        if( b ){
            b->glItemFPLocal(nFP, 20, scale)->setGroup( ret );
        }
        elemIter++;
    }

    return ret;
}

void FEMStructure::setVertexRadius( double r ){
    m_d->vertexModel->setGraphicsRadius( r );
}

void FEMStructure::updateQGraphics( Vertex * vert ){
    ElementModel::ElementIterator iter = m_d->elementModel->begin();
    while( iter != m_d->elementModel->end() ){
        if( (*iter)->isVertexUsed(vert) ){
            (*iter)->updateQGraphicsItem();
        }
        iter++;
    }
}

Hypothesis * FEMStructure::hypothesis(){
    return m_d->hypothesis;
}
