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
#include "beam.h"

#include "beamfmodel.h"
#include "beamqmodel.h"
#include "section.h"
#include "material.h"
#include "vertex.h"
#include "hypothesis.h"
#include "doubleplus.h"
#include "glgroup.h"
#include "glline.h"

#include <Eigen/Geometry>

#include <QGraphicsLineItem>

#include <iostream>

class BeamPrivate{
public:
    BeamPrivate( int nDOF,  Section * s,  Hypothesis * hyp, UnitMeasure * ump ){
        section = s;

        fAppLocalArray = new double [ nDOF ];
        qAppLocalArray = new double [ nDOF ];
        for( int i=0; i < nDOF; i++ ){
            fAppLocalArray[i] = 0.0;
            qAppLocalArray[i] = 0.0;
        }
        for( int i=0; i < 2; i++ ){
            for( int j=0; j < hyp->nDOFVert(); j++){
                if( j < hyp->spaceDim()){
                    fAppLocalList.append( new DoublePlus( &(fAppLocalArray[i*hyp->nDOFVert()+j]), "fLocal" + QString::number(i+j), ump, UnitMeasure::loadF));
                    qAppLocalList.append( new DoublePlus( &(qAppLocalArray[i*hyp->nDOFVert()+j]), "qLocal" + QString::number(i+j), ump, UnitMeasure::loadFLine));
                } else {
                    fAppLocalList.append( new DoublePlus( &(fAppLocalArray[i*hyp->nDOFVert()+j]), "fLocal" + QString::number(i+j), ump, UnitMeasure::loadM));
                    qAppLocalList.append( new DoublePlus( &(qAppLocalArray[i*hyp->nDOFVert()+j]), "qLocal" + QString::number(i+j), ump, UnitMeasure::loadMLine));
                }
            }
        }
        fAppModel = new BeamFModel( hyp, ump );
        qAppModel = new BeamQModel( hyp, ump );

        qGraphicsItem = NULL;
        glItem = NULL;
    }
    ~BeamPrivate(){
        delete [] fAppLocalArray;
        delete fAppModel;
        delete [] qAppLocalArray;
        delete qAppModel;

        if( glItem ){
            delete glItem;
        }
    }

    // Sezione della trave
    Section * section;
    // Oggetto grafico di tipo QGraphicsItemItem associato all'elemento trave
    QGraphicsLineItem * qGraphicsItem;
    // Oggetto grafico di tipo GLItem associato all'elemento trave
    GLLine * glItem;
    // Array delle forze nodali esterne applicate
    double * fAppLocalArray;
    // Lista delle forze nodali esterne applicate, rappresentate tramite VarPlus
    QList<DoublePlus * > fAppLocalList;
    // Modello delle forze nodali esterne applicate
    BeamFModel * fAppModel;
    // Array delle forze nodali esterne distribuite
    double * qAppLocalArray;
    // Lista delle forze nodali esterne distribuite, rappresentate tramite VarPlus
    QList<DoublePlus * > qAppLocalList;
    // Modello delle forze nodali esterne distribuite
    BeamQModel * qAppModel;
};


Beam::Beam( unsigned int iidd, Section * sect, Vertex * vert1, Vertex * vert2, Hypothesis * hyp, UnitMeasure * ump, QObject * parent ):
    Element( iidd, hyp, ump, parent ) {

    m_vertexes->clear();
    if( vert1->hypothesis() == hyp ){
        m_vertexes->append( new VertexElement( vert1 ) );
    }
    if( vert2->hypothesis() == hyp ){
        m_vertexes->append( new VertexElement( vert2 ) );
    }

    commonInit();

    m_dd = new BeamPrivate( m_nDOF, sect, hyp, ump );

    init();
}

void Beam::init(){
    commonInit();

    L = new DoublePlus( 0.0, "L", m_unitMeasure, UnitMeasure::length );
    L->setReadOnly( true );
    L->setRichName( trUtf8("L"));
    L->setToolTip( trUtf8("Lunghezza dell'elemento trave"));
    for( int i=0; i < m_vertexes->size(); i++){
        connect( m_vertexes->at(i)->vertex, SIGNAL(vertexChanged()), this, SLOT(setL()));
        connect( m_vertexes->at(i)->vertex, SIGNAL(vertexChanged()), this, SLOT(setLCS()));
    }
    setL();

    bType = new BeamType( Beam::Timoshenko, "bType" );
    bType->setReadOnly( false );
    bType->setRichName( trUtf8("Tipo") );
    bType->setToolTip( trUtf8("Teoria applicata nel calcolo dell'elemento") );

    setKF0Local();

    updateQGraphicsItem();
}

Section * Beam::section(){
    return m_dd->section;
}

void Beam::setSection( Section * ns ){
    m_dd->section = ns;
}

void Beam::setL(){
    if( L->readOnly() ){
        if( m_vertexes->size() > 1 ){
            Eigen::VectorXd DL( m_hypothesis->spaceDim() );
            for( int i=0; i < m_hypothesis->spaceDim(); i++ ) {
                DL(i) = m_vertexes->at(1)->vertex->xGlobal(i)->valueNormal() - m_vertexes->at(0)->vertex->xGlobal(i)->valueNormal();
            }
            L->setValueNormal( DL.norm() );
        }
    }
}

void Beam::setVertex1(Vertex *vert) {
    setVertex( 0 , vert );
}

void Beam::setVertex2(Vertex *vert) {
    if( m_dd->glItem ){
        disconnect( m_vertexes->at(1)->vertex, SIGNAL(vertexChanged()), this, SLOT(updateGLItem()));
    }
    setVertex( 1 , vert );
    if( m_dd->glItem ){
        connect( m_vertexes->at(1)->vertex, SIGNAL(vertexChanged()), this, SLOT(updateGLItem()));
        updateGLItem();
    }
}

QString Beam::typeElement(){
    return QString("Beam");
}

void Beam::setLCS(){
    Eigen::Map<Eigen::MatrixXd> _LCS( m_LCS, m_LCSSize, m_LCSSize);
    _LCS.setIdentity();

    if( m_vertexes->size() >= 2 ){
        for( int i=0; i < m_LCSSize; i++ ){
            _LCS(0,i) = m_vertexes->at(1)->vertex->xGlobal(i)->valueNormal() - m_vertexes->at(0)->vertex->xGlobal(i)->valueNormal();
        }
        if( _LCS.row(0).norm() != 0.0 ){
            _LCS.row(0).normalize();

            if( m_LCSSize == 2 || m_LCSSize == 3 ){
                Eigen::Vector3d n1 = Eigen::Vector3d::Zero();
                n1.head( m_LCSSize ) = _LCS.block(0,0,1,m_LCSSize).transpose();
                Eigen::Vector3d n2 = n1.cross( Eigen::Vector3d::UnitZ() );
                if( n2.norm() == 0.0 ){
                    n2 = Eigen::Vector3d::UnitX();
                } else {
                    n2.normalize();
                }
                if( m_LCSSize == 2 ){
                    _LCS.block(1,0,1,m_LCSSize) = n2.transpose().head( m_LCSSize );
                } else if( m_LCSSize == 3) {
                    Eigen::Vector3d n3 = n1.cross(n2);
                    n3.normalize();
                    _LCS.block(1,0,1,m_LCSSize) = n2.transpose();
                    _LCS.block(2,0,1,m_LCSSize) = n3.transpose();
                }
            }
        }
    }

    // componente traslativa
    if( m_vertexes->size() > 0 ){
        for( int i=0; i < m_LCSSize; i++ ){
            m_LCSOrigin[i] = m_vertexes->at(0)->vertex->xGlobal(i)->valueNormal();
        }
    }
}

void Beam::setFAppLocal(){
    setN();
    Eigen::Map<Eigen::MatrixXd> _N( m_N, m_nDOF, m_nDOF);
    Eigen::Map<Eigen::VectorXd> _fLocal( m_dd->fAppLocalArray, m_nDOF );
    _fLocal.setZero();

    for(int i=0; i < fAppModel()->nF(); i++ ){
        Eigen::Map<Eigen::VectorXd> _f( fAppModel()->f(i)->fVal(), m_nDOF );
        if( fAppModel()->f(i)->sysRef()->valueNormal() == BeamF::fLocal ){
            _fLocal += _f;
        } else if( fAppModel()->f(i)->sysRef()->valueNormal() == BeamF::fGlobal ){
            _fLocal += _N * _f;
        }
    }
}

void Beam::setQAppLocal(){
    setN();
    // setN chiama anche setLCS()
    Eigen::Map<Eigen::MatrixXd> _LCS( m_LCS, m_LCSSize, m_LCSSize);
    Eigen::Map<Eigen::MatrixXd> _N( m_N, m_nDOF, m_nDOF);

    // mappiamo i valori qAppLocalArray in _q (vettore Eigen)
    Eigen::Map<Eigen::VectorXd> _qLocalTot( m_dd->qAppLocalArray, m_nDOF );

    // inizializziamo _q
    _qLocalTot.setZero();

    for(int i=0; i < qAppModel()->nQ(); i++ ){
        Eigen::Map<Eigen::VectorXd> _q( qAppModel()->q(i)->qVal(), m_nDOF );
        if( qAppModel()->q(i)->sysRef()->valueNormal() == BeamQ::qGlobalSR ){
            double b = 0.0;
            for( int j=0; j < m_LCSSize; j++ ){
                if( qAppModel()->q(i)->sysRefGlobal()->valueNormal() != j ){
                    b += pow( _LCS(0,j), 2.0);
                }
            }
            if( b > 0.0 ){
                _qLocalTot += _N * _q / sqrt(b);
            }
        } else if( qAppModel()->q(i)->sysRef()->valueNormal() == BeamQ::qGlobalLocalSR ){
            _qLocalTot += _N * _q;
        } else { // qModel()->q(i)->sysRef()->valueNormal() == BeamQ::qLocalSR
            _qLocalTot += _q;
        }
    }
}

void Beam::setKF0Local(){
    setL();

    if( L->valueNormal() != 0.0 ){
        setFAppLocal();
        setQAppLocal();

        // inizializza f0Local
        for( int i=0; i < m_nDOF; i++ ){
            m_f0Local[i] = 0.0;
        }

        // inizializza KLocal
        // per semplicita' di notazione adottiamo Eigen
        Eigen::Map<Eigen::MatrixXd> _KLocal( m_KLocal, m_nDOF, m_nDOF);
        _KLocal.setZero();
        // inizializza f0Local
        Eigen::Map<Eigen::VectorXd> _f0Local( m_f0Local, m_nDOF);
        _f0Local.setZero();
        // forze nodali trave semplicemente appoggiata con carichi infranodali elemento
        Eigen::VectorXd fA(m_nDOF);
        fA.setZero();
        // spostamenti nodali trave semplicemente appoggiata con carichi infranodali elemento
        Eigen::VectorXd etaA(m_nDOF);
        etaA.setZero();

        if( m_hypothesis->spaceDim() == 2 ){
            for( int i=0; i < m_nDOF; i++){
                // calcolo matrice rigidezza
                if( bType->valueNormal() == Beam::Bernoulli ){
                    for( int j=0; j < m_nDOF; j++){
                        _KLocal(i,j) = KBernoulli( i, j, m_dd->section->material()->E->valueNormal(),
                                                   m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                    }
                } else if( bType->valueNormal() == Beam::Timoshenko ){
                    for( int j=0; j < m_nDOF; j++){
                        _KLocal(i,j) = KTimoshenko( i, j, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                                    m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                    }
                } else if( bType->valueNormal() == Beam::Winkler ){
                    // TODO
                }
                // calcolo forze nodali
                fA(i) = fSS(i,m_dd->qAppLocalArray[0], m_dd->qAppLocalArray[1], m_dd->qAppLocalArray[2],
                        m_dd->qAppLocalArray[3], m_dd->qAppLocalArray[4], m_dd->qAppLocalArray[5], L->valueNormal() );

                // calcolo spostamenti trave semplicemente appoggiata
                if( bType->valueNormal() == Beam::Bernoulli ){
                    etaA(i) = etaSSBernoulli(i,m_dd->qAppLocalArray[0], m_dd->qAppLocalArray[1], m_dd->qAppLocalArray[2],
                            m_dd->qAppLocalArray[3], m_dd->qAppLocalArray[4], m_dd->qAppLocalArray[5],
                            m_dd->section->material()->E->valueNormal(),
                            m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(),
                            L->valueNormal() );
                } else if( bType->valueNormal() == Beam::Timoshenko ){
                    etaA(i) = etaSSTimoshenko(i,m_dd->qAppLocalArray[0], m_dd->qAppLocalArray[1], m_dd->qAppLocalArray[2],
                            m_dd->qAppLocalArray[3], m_dd->qAppLocalArray[4], m_dd->qAppLocalArray[5],
                            m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                            m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(),
                            L->valueNormal() );
                } else if( bType->valueNormal() == Beam::Winkler ){
                    // TODO
                }
            }
        } else if( m_hypothesis->spaceDim() == 3 ){
            // Azioni assiali
            _KLocal(0,0) = KTimoshenko( 0, 0, m_dd->section->material()->E->valueNormal(), 0.0,
                                        0.0, m_dd->section->A->valueNormal(), 0.0, L->valueNormal());
            _KLocal(6,6) = KTimoshenko( 3, 3, m_dd->section->material()->E->valueNormal(), 0.0,
                                        0.0, m_dd->section->A->valueNormal(), 0.0, L->valueNormal());
            _KLocal(0,6) = KTimoshenko( 0, 3, m_dd->section->material()->E->valueNormal(), 0.0,
                                        0.0, m_dd->section->A->valueNormal(), 0.0, L->valueNormal());
            _KLocal(6,0) = _KLocal(0,6);

            // Forze lungo asse y, momenti lungo asse z
            if( bType->valueNormal() == Beam::Bernoulli ){
                _KLocal(1,1) = KBernoulli( 1, 1, m_dd->section->material()->E->valueNormal(),
                                           m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(1,5) = - KBernoulli( 1, 2, m_dd->section->material()->E->valueNormal(),
                                             m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(5,1) = _KLocal(1,5);
                _KLocal(1,7) = KBernoulli( 1, 3, m_dd->section->material()->E->valueNormal(),
                                           m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(7,1) = _KLocal(1,7);
                _KLocal(1,11) = - KBernoulli( 1, 5, m_dd->section->material()->E->valueNormal(),
                                              m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(1,11) = _KLocal(11,1);
                _KLocal(5,5) = KBernoulli( 2, 2, m_dd->section->material()->E->valueNormal(),
                                           m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(5,7) = - KBernoulli( 2, 3, m_dd->section->material()->E->valueNormal(),
                                             m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(7,5) = _KLocal(5,7);
                _KLocal(5,11) = KBernoulli( 2, 5, m_dd->section->material()->E->valueNormal(),
                                            m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(11,5) = _KLocal(5,11);
                _KLocal(7,7) = KBernoulli( 3, 3, m_dd->section->material()->E->valueNormal(),
                                           m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(7,11) = - KBernoulli( 3, 5, m_dd->section->material()->E->valueNormal(),
                                              m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(11,7) = _KLocal(7,11);
                _KLocal(11,11) = KBernoulli( 5, 5, m_dd->section->material()->E->valueNormal(),
                                             m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());

                // Forze lungo asse z, momenti lungo asse y
                _KLocal(2,2) = KBernoulli( 1, 1, m_dd->section->material()->E->valueNormal(),
                                           m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(2,4) = KBernoulli( 1, 2, m_dd->section->material()->E->valueNormal(),
                                           m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(2,4) = _KLocal(4,2);
                _KLocal(2,8) = KBernoulli( 1, 4, m_dd->section->material()->E->valueNormal(),
                                           m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(2,8) = _KLocal(8,2);
                _KLocal(2,10) = KBernoulli( 1, 5, m_dd->section->material()->E->valueNormal(),
                                            m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(2,10) = _KLocal(10,2);
                _KLocal(4,4) = KBernoulli( 2, 2, m_dd->section->material()->E->valueNormal(),
                                           m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(4,8) = KBernoulli( 2, 4, m_dd->section->material()->E->valueNormal(),
                                           m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(8,4) = _KLocal(4,8);
                _KLocal(4,10) = KBernoulli( 2, 5, m_dd->section->material()->E->valueNormal(),
                                            m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(10,4) = _KLocal(4,10);
                _KLocal(8,8) = KBernoulli( 4, 4, m_dd->section->material()->E->valueNormal(),
                                           m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(8,10) = KBernoulli( 4, 5, m_dd->section->material()->E->valueNormal(),
                                            m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(8,10) = _KLocal(10,8);
                _KLocal(10,10) = KBernoulli( 5, 5, m_dd->section->material()->E->valueNormal(),
                                             m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
            } else if( bType->valueNormal() == Beam::Timoshenko ){
                _KLocal(1,1) = KTimoshenko( 1, 1, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                            m_dd->section->ty->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(1,5) = - KTimoshenko( 1, 2, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                              m_dd->section->ty->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(5,1) = _KLocal(1,5);
                _KLocal(1,7) = KTimoshenko( 1, 3, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                            m_dd->section->ty->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(7,1) = _KLocal(1,7);
                _KLocal(1,11) = - KTimoshenko( 1, 5, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                               m_dd->section->ty->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(1,11) = _KLocal(11,1);
                _KLocal(5,5) = KTimoshenko( 2, 2, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                            m_dd->section->ty->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(5,7) = - KTimoshenko( 2, 3, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                              m_dd->section->ty->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(7,5) = _KLocal(5,7);
                _KLocal(5,11) = KTimoshenko( 2, 5, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                             m_dd->section->ty->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(11,5) = _KLocal(5,11);
                _KLocal(7,7) = KTimoshenko( 3, 3, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                            m_dd->section->ty->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(7,11) = - KTimoshenko( 3, 5, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                               m_dd->section->ty->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());
                _KLocal(11,7) = _KLocal(7,11);
                _KLocal(11,11) = KTimoshenko( 5, 5, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                              m_dd->section->ty->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICzz->valueNormal(), L->valueNormal());

                // Forze lungo asse z, momenti lungo asse y
                _KLocal(2,2) = KTimoshenko( 1, 1, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                            m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(2,4) = KTimoshenko( 1, 2, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                            m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(2,4) = _KLocal(4,2);
                _KLocal(2,8) = KTimoshenko( 1, 4, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                            m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(2,8) = _KLocal(8,2);
                _KLocal(2,10) = KTimoshenko( 1, 5, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                             m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(2,10) = _KLocal(10,2);
                _KLocal(4,4) = KTimoshenko( 2, 2, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                            m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(4,8) = KTimoshenko( 2, 4, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                            m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(8,4) = _KLocal(4,8);
                _KLocal(4,10) = KTimoshenko( 2, 5, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                             m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(10,4) = _KLocal(4,10);
                _KLocal(8,8) = KTimoshenko( 4, 4, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                            m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(8,10) = KTimoshenko( 4, 5, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                             m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
                _KLocal(8,10) = _KLocal(10,8);
                _KLocal(10,10) = KTimoshenko( 5, 5, m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                                              m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(), L->valueNormal());
            }
            // Azioni torsionali
            _KLocal(3,3) = m_dd->section->material()->G->valueNormal() * m_dd->section->IT->valueNormal() / L->valueNormal();
            _KLocal(3,9) = - m_dd->section->material()->G->valueNormal() * m_dd->section->IT->valueNormal() / L->valueNormal();
            _KLocal(3,9) = _KLocal(9,3);
            _KLocal(9,9) = m_dd->section->material()->G->valueNormal() * m_dd->section->IT->valueNormal() / L->valueNormal();

            // TODO
            // Manca calcolo spostamenti trave semplicemente appoggiata
        }
        Eigen::Map<Eigen::VectorXd> fConc( m_dd->fAppLocalArray, m_nDOF );

        _f0Local = -fConc + (fA - _KLocal * etaA);
    }
}

double Beam::KLocal(int i, int j, double E, double G, double chi, double A, double J, double l){
    if( bType->valueNormal() == Timoshenko ){
        return KTimoshenko( i, j, E, G, chi, A, J, l);
    } else if( bType->valueNormal() == Winkler ){
        // TODO
        return 0.0;
    }

    return KBernoulli( i, j, E, A, J, l);
}


double Beam::KTimoshenko(int i, int j, double E, double G, double chi, double A, double J, double l){
    if( l != 0.0 ){
        double Phi = 0.0;
        if( G != 0.0 && A != 0.0 ){
            Phi = 12.0 * chi * E * J / (G * A * pow(l, 2.0));
        }
        if( (1.0 + Phi)!= 0.0 ){
            if( (i == 0 && j == 0) || (i == 3 && j == 3) ){
                return E * A / l;
            }
            if( (i == 0 && j == 3) || (i == 3 && j == 0) ){
                return - E * A / l;
            }
            if( (i == 1 && j == 1) || (i == 4 && j == 4)  ){
                return 12.0 * E * J / (pow(l,3.0) * (1.0 + Phi) );
            }
            if( (i == 1 && j == 2) || (i == 2 && j == 1)  ){
                return - 6.0 * E * J / (pow(l,2.0) * (1.0 + Phi) );
            }
            if( (i == 1 && j == 4) || (i == 4 && j == 1)  ){
                return - 12.0 * E * J / (pow(l,3.0) * (1.0 + Phi) );
            }
            if( (i == 1 && j == 5) || (i == 5 && j == 1)  ){
                return - 6.0 * E * J / (pow(l,2.0) * (1.0 + Phi) );
            }
            if( (i == 2 && j == 2) || (i == 5 && j == 5)  ){
                return (4.0 + Phi) * E * J / (l * (1.0 + Phi) );
            }
            if( (i == 2 && j == 4) || (i == 4 && j == 2)  ){
                return 6.0 * E * J / (pow(l, 2.0) * (1.0 + Phi) );
            }
            if( (i == 2 && j == 5) || (i == 5 && j == 2)  ){
                return (2.0 - Phi) * E * J / (l * (1.0 + Phi) );
            }
            if( (i == 4 && j == 5) || (i == 5 && j == 4)  ){
                return 6.0 * E * J / (pow(l,2.0) * (1.0 + Phi) );
            }
        }
    }
    return 0.0;
}

double Beam::KBernoulli(int i, int j, double E, double A, double J, double l){
    if( l != 0.0 ){
        if( (i == 0 && j == 0) || (i == 3 && j == 3) ){
            return E * A / l;
        }
        if( (i == 0 && j == 3) || (i == 3 && j == 0) ){
            return - E * A / l;
        }
        if( (i == 1 && j == 1) || (i == 4 && j == 4)  ){
            return 12.0 * E * J / pow(l,3.0);
        }
        if( (i == 1 && j == 2) || (i == 2 && j == 1)  ){
            return - 6.0 * E * J / pow(l,2.0);
        }
        if( (i == 1 && j == 4) || (i == 4 && j == 1)  ){
            return - 12.0 * E * J / pow(l,3.0);
        }
        if( (i == 1 && j == 5) || (i == 5 && j == 1)  ){
            return - 6.0 * E * J / pow(l,2.0);
        }
        if( (i == 2 && j == 2) || (i == 5 && j == 5)  ){
            return 4.0 * E * J / l;
        }
        if( (i == 2 && j == 4) || (i == 4 && j == 2)  ){
            return 6.0 * E * J / pow(l, 2.0);
        }
        if( (i == 2 && j == 5) || (i == 5 && j == 2)  ){
            return 2.0 * E * J / l;
        }
        if( (i == 4 && j == 5) || (i == 5 && j == 4)  ){
            return 6.0 * E * J / pow(l,2.0);
        }
    }
    return 0.0;
}

double Beam::etaSSTimoshenko(int i, double n1, double v1, double m1, double n2, double v2, double m2, double E, double G, double chi, double A, double J, double l){
    double ret = 0.0;
    if( i== 2 ){
        double DM = E * J;
        if( DM != 0.0 ){
            ret += - (v1 / 24.0 + 7.0 * (v2-v1) / 360.0) * pow(l,3.0)/ DM
                    - (m2 - m1) * pow(l,2.0) / (24.0 * DM);
        }
        double DT = G * A;
        if( DT != 0.0 ){
            ret += chi * ( m1 + (m2 - m1) / 2.0) / DT;
        }
    } else if( i == 3 ){
        double DN = E * A;
        if( DN != 0.0 ){
            ret += pow(l,2.0) * (  n1 / 2.0 + (n2 - n1) / 3.0) / DN;
        }
    }else if( i== 5 ){
        double DM = E * J;
        if( DM != 0.0 ){
            ret += (v1 / 24.0 + (v2 - v1) / 35.0) * pow(l,3.0)/ DM
                    + (m2 - m1) * pow(l,2.0) / (24.0 * DM);
        }
        double DT = G * A;
        if( DT != 0.0 ){
            ret += chi * ( m1 + (m2 - m1) / 2.0) / DT;
        }
    }
    return ret;
}

double Beam::etaSSBernoulli(int i, double n1, double v1, double m1, double n2, double v2, double m2, double E, double A, double J, double l){
    return etaSSTimoshenko(i, n1, v1, m1, n2, v2, m2, E, 0.0, 0.0, A, J, l);
}

double Beam::fSS(int i, double n1, double v1, double m1, double n2, double v2, double m2, double l){
    if( i == 0){
        return (l * (n1 + (n2-n1) / 2.0));
    } else if( i == 1 ){
        return (- l * ( v1 / 2.0 + (v2-v1) / 6.0)
                - (m1 + (m2-m1) / 2.0));
    } else if ( i == 4 ){
        return (- l * ( v1 / 2.0 + (v2-v1) / 3.0)
                + (m1 + (m2-m1) / 2.0));
    } else {
        return 0.0;
    }
}

void Beam::etaPAx(QVector< double> * xVect, QVector< double> * valVect,
                  double eta1, double eta4,
                  double p1, double p2,
                  double E, double A, double l){
    if( (E*A*l) != 0.0 ){
        double Dp = p2 - p1;

        double C5 = ((l*((p1/2.0) + (Dp/6.0))) + (E*A*(eta4 - eta1)/l));
        double C6 = eta1;

        QVector<double>::iterator xP = xVect->begin();
        valVect->clear();
        while( xP != xVect->end() ){
            double x = *xP;
            double u = ((((x*C5) - ((x*x)*p1/2.0) - (pow(x, 3.0)*Dp/(6.0*l)))/(A*E)) + C6);
            valVect->append( u );
            xP++;
        }
    }
}

void Beam::etaPBernoulli(QVector< double> * xVect, QVector< double> * wVect, QVector< double> * thetaVect,
                         double eta2, double eta3, double eta5, double eta6,
                         double q1, double m1, double q2, double m2,
                         double E, double J, double l){
    wVect->clear();
    thetaVect->clear();
    if( (E*J*l) != 0.0 ){
        double Dq = q2 - q1;
        double Dm = m2 - m1;

        double C1 = ((6.0*((2.0*(eta2 - eta5)/l) - eta3 - eta6)/(l*l)) - (((l*((3.0*Dq/20.0) + (q1/2.0))) + (Dm/2.0))/(E*J)));
        double C2 = ((2.0*((2.0*eta3) + eta6 + (3.0*(eta5 - eta2)/l))/l) + ((((l*l)*((Dq/30.0) + (q1/12.0))) + (l*Dm/12.0))/(E*J)));
        double C3 = -eta3;
        double C4 = eta2;

        QVector<double>::iterator xP = xVect->begin();
        while( xP != xVect->end() ){
            double x = *xP;
            double w = ((((pow(x, 4.0)*q1/24.0) + (((pow(x, 4.0)*Dm/24.0) + (pow(x, 5.0)*Dq/120.0))/l))/(J*E)) + (pow(x, 3.0)*C1/6.0) + ((x*x)*C2/2.0) + (x*C3) + C4);
            wVect->append( w );
            double theta = ((-1.0)*((((((pow(x, 4.0)*Dq) + (4.0*pow(x, 3.0)*Dm))/l) + (4.0*pow(x, 3.0)*q1))/(24.0*J*E)) + C3 + ((x*x)*C1/2.0) + (x*C2)));
            thetaVect->append( theta );
            xP++;
        }
    }
}

void Beam::etaPTimoshenko(QVector< double> * xVect, QVector< double> * wVect, QVector< double> * thetaVect,
                          double eta2, double eta3, double eta5, double eta6,
                          double q1, double m1, double q2, double m2,
                          double E, double G, double chi, double A, double J, double l){
    wVect->clear();
    thetaVect->clear();
    if( (E*J*G*A*l) != 0.0 ){
        double Dq = q2 - q1;
        double Dm = m2 - m1;

        double C1 = ((((l*((3.0*Dq/20.0) + (q1/2.0))) + (Dm/2.0))/(J*E)) + (E*J*(((l*l)*chi*Dq) + (30.0*l*((G*A*(eta3 + eta6)) - (chi*((2.0*m1) + Dm)))) + (60.0*G*A*(eta5 - eta2)))/(5.0*((pow(l, 3.0)*J*E*A*G) + (12.0*l*pow((J*E), 2.0)*chi)))));
        double C2 = ((((4.0*l*Dm) - ((l*l)*(Dq + (2.0*q1))))/(24.0*J*E)) + ((eta6 - eta3 + (((3.0*E*J*((2.0*l*G*A*(eta2 - eta5)) + ((l*l)*((2.0*chi*m1) - (G*A*(eta6 + eta3)))))) + (G*A*((pow(l, 5.0)*Dq/120.0) - (pow(l, 4.0)*Dm/4.0))))/(((l*l)*E*J*A*G) + (12.0*pow((E*J), 2.0)*chi))))/l));
        double C3 = ((chi*l*((3.0*Dq/20.0) + (q1/2.0))/(A*G)) - eta3 + (((5.0*chi*G*A*((pow(l, 3.0)*(Dm + (2.0*m1))) + (12.0*E*J*((l*(eta6 + eta3)) + (2.0*(eta5 - eta2)))))/2.0) + (pow((l*chi), 2.0)*E*J*Dq))/(5.0*l*(pow((l*A*G), 2.0) + (12.0*A*G*J*E*chi)))));
        double C4 = eta2;

        QVector<double>::iterator xP = xVect->begin();
        while( xP != xVect->end() ){
            double x = *xP;
            double w = ((((((pow(x, 5.0)*Dq/120.0) + (pow(x, 4.0)*Dm/24.0))/(J*E)) - (pow(x, 3.0)*chi*Dq/(6.0*G*A)))/l) + (pow(x, 4.0)*q1/(24.0*E*J)) - (pow(x, 3.0)*C1/6.0) - ((x*x)*(C2 + (chi*q1/(G*A)))/2.0) + (x*C3) + C4);
            wVect->append( w );
            double theta = ((chi*((E*J*C1) + m1)/(A*G)) + (x*C2) + ((x*x)*C1/2.0) - (((4.0*pow(x, 3.0)*q1) + (((pow(x, 4.0)*Dq) + (4.0*pow(x, 3.0)*Dm))/l))/(24.0*J*E)) - C3);
            thetaVect->append( theta );
            xP++;
        }
    }
}

QGraphicsItem * Beam::qGraphicsItem(){
    if( m_dd->qGraphicsItem == NULL ){
        m_dd->qGraphicsItem = new QGraphicsLineItem();
        updateQGraphicsItem();
    }
    return m_dd->qGraphicsItem;
}

void Beam::updateQGraphicsItem(){
    if( m_dd->qGraphicsItem ){
        if( m_hypothesis->spaceDim() == 2 ){
            m_dd->qGraphicsItem->setLine( m_vertexes->at(0)->vertex->xGlobal(0)->value(), m_vertexes->at(0)->vertex->xGlobal(1)->value(), m_vertexes->at(1)->vertex->xGlobal(0)->value(), m_vertexes->at(1)->vertex->xGlobal(1)->value() );
        } else if( m_hypothesis->spaceDim() == 3 ){
            // TODO
        }
    }
}

GLItem * Beam::glItem(){
    if( m_dd->glItem == NULL ){
        m_dd->glItem = new GLLine();
        m_dd->glItem->setColor(Qt::green);
        connect( this, SIGNAL(vertexChanged(int,Vertex*,Vertex*)), this, SLOT(reconnectVertexToGLItem(int,Vertex*,Vertex*)));
        for( int i=0; i < 2; i++ ){
            connect( m_vertexes->at(i)->vertex, SIGNAL(vertexChanged()), this, SLOT(updateGLItem()));
        }
        updateGLItem();
    }
    return m_dd->glItem;
}

void Beam::updateGLItem(){
    if( m_dd->glItem && m_vertexes->size() >= 2 ){
        for( int i=0; i < 2; i++ ){
            if( m_hypothesis->spaceDim() == 1 ){
                m_dd->glItem->setP( i, m_vertexes->at(i)->vertex->xGlobal(0)->value(), 0.0, 0.0 );
            } else if( m_hypothesis->spaceDim() == 2 ){
                m_dd->glItem->setP( i, m_vertexes->at(i)->vertex->xGlobal(0)->value(), m_vertexes->at(i)->vertex->xGlobal(1)->value(), 0.0 );
            } else if( m_hypothesis->spaceDim() >= 3 ){
                m_dd->glItem->setP( i, m_vertexes->at(i)->vertex->xGlobal(0)->value(), m_vertexes->at(i)->vertex->xGlobal(1)->value(), m_vertexes->at(i)->vertex->xGlobal(2)->value() );
            }
        }
    }
}

void Beam::reconnectVertexToGLItem( int, Vertex * oldVert, Vertex *newVert){
    if( m_dd->glItem ){
        disconnect( oldVert, SIGNAL(vertexChanged()), this, SLOT(updateGLItem()));
        connect( newVert, SIGNAL(vertexChanged()), this, SLOT(updateGLItem()));
        updateGLItem();
    }
}

GLItem * Beam::glItemDeformed( int nP, double scale ){
    GLGroup * ret = new GLGroup();

    setL();

    if( nP > 1 && m_vertexes->size() > 1 && L->valueNormal() > 0.0 ){
        setLCS();
        Eigen::Map<Eigen::MatrixXd> _LCS( m_LCS, m_LCSSize, m_LCSSize);
        Eigen::Map<Eigen::VectorXd> _LCSOrigin( m_LCSOrigin, m_LCSSize);

        QVector<double> xVect;
        QVector<QVector <double> > etaPLVal;
        etaPLocal( NULL, &xVect, NULL, &etaPLVal, nP, false);

        Eigen::VectorXd P1( m_hypothesis->spaceDim() );
        Eigen::VectorXd P2( m_hypothesis->spaceDim() );
        Eigen::VectorXd etaPL1( m_hypothesis->spaceDim() );
        Eigen::VectorXd etaPL2( m_hypothesis->spaceDim() );

        bool etaPLValSizeOk = true;
        for( int i = 1; (i < xVect.size()); ++i ){
            for( int k=0; k < m_hypothesis->spaceDim(); k++){
                if( i < etaPLVal.at(k).size()){
                    etaPL1(k) = etaPLVal.at(k).at(i-1);
                    etaPL2(k) = etaPLVal.at(k).at(i);
                } else {
                    etaPLValSizeOk = false;
                }
            }
            if( ! etaPLValSizeOk ){
                break;
            }
            P1 = xVect.at(i-1) * _LCS.row(0).transpose() + scale * _LCS.transpose() * etaPL1 + _LCSOrigin;
            P1 *= m_unitMeasure->scale( UnitMeasure::length );
            P2 = xVect.at(i) * _LCS.row(0).transpose() + scale * _LCS.transpose() * etaPL2 + _LCSOrigin;
            P2 *= m_unitMeasure->scale( UnitMeasure::length );
            GLLine * l = new GLLine();
            l->setGroup( ret );
            if( m_hypothesis->spaceDim() == 1 ){
                l->setP0( P1(0), 0.0, 0.0 );
                l->setP1( P2(0), 0.0, 0.0 );
            } else if( m_hypothesis->spaceDim() == 2 ){
                l->setP0( P1(0), P1(1), 0.0 );
                l->setP1( P2(0), P2(1), 0.0 );
            } else if( m_hypothesis->spaceDim() > 2 ){
                l->setP0( P1(0), P1(1), P1(2) );
                l->setP1( P2(0), P2(1), P2(2) );
            }
        }
    }
    return ret;
}

void Beam::etaPLocal(QString * xName, QVector<double> * xVect, QVector<QString> * valNames, QVector<QVector< double> > * valVect, int nP , bool scale){
    // resetta i vettori di output
    if( xName ){
        xName->clear();
    }
    xVect->clear();
    if( valNames ){
        valNames->clear();
    }
    for( int i=0; i <valVect->size(); i++ ){
        (*valVect)[i].clear();
    }
    valVect->clear();

    if( nP > 1 ){
        if( m_hypothesis->spaceDim() == 2 && m_hypothesis->nDOFVert() == 3 ){
            QVector<double> uVect, wVect, thetaVect;
            if( L->valueNormal() != 0.0 && nP > 1 ){
                double x = 0.0;
                double DL = L->valueNormal() / (nP - 1);
                for( int i=0; i < nP ; i++) {
                    xVect->append( x );
                    x += DL;
                }
                if( xName ){
                    *xName = trUtf8("x");
                    if( scale ) {
                        *xName +=  QString(" [") + m_unitMeasure->string( UnitMeasure::length ) + "]";
                    }
                }
                etaPAx( xVect, &uVect, m_etaLocal[0], m_etaLocal[3], m_dd->qAppLocalArray[0], m_dd->qAppLocalArray[3],
                        m_dd->section->material()->E->valueNormal(), m_dd->section->A->valueNormal(), L->valueNormal() );
                if( bType->valueNormal() == Beam::Bernoulli ){
                    etaPBernoulli( xVect, &wVect, &thetaVect,
                                   m_etaLocal[1], m_etaLocal[2], m_etaLocal[4], m_etaLocal[5],
                            m_dd->qAppLocalArray[1], m_dd->qAppLocalArray[2],
                            m_dd->qAppLocalArray[4], m_dd->qAppLocalArray[5],
                            m_dd->section->material()->E->valueNormal(), m_dd->section->ICyy->valueNormal(),
                            L->valueNormal() );
                } else if( bType->valueNormal() == Beam::Timoshenko ){
                    etaPTimoshenko( xVect, &wVect, &thetaVect,
                                    m_etaLocal[1], m_etaLocal[2], m_etaLocal[4], m_etaLocal[5],
                            m_dd->qAppLocalArray[1], m_dd->qAppLocalArray[2],
                            m_dd->qAppLocalArray[4], m_dd->qAppLocalArray[5],
                            m_dd->section->material()->E->valueNormal(), m_dd->section->material()->G->valueNormal(),
                            m_dd->section->tz->valueNormal(), m_dd->section->A->valueNormal(), m_dd->section->ICyy->valueNormal(),
                            L->valueNormal() );
                } else if( bType->valueNormal() == Beam::Winkler ){
                    // TODO
                }
            }
            if( valNames ){
                valNames->append( trUtf8("u") );
                valNames->append( trUtf8("w") );
                valNames->append( trUtf8("Î¸") );
            }

            // assegna l'unitÃ  di misura utente alle ascisse
            if( scale ){
                QVector<double>::iterator iter = xVect->begin();
                while( iter != xVect->end() ){
                    *iter = *iter * m_unitMeasure->scale( UnitMeasure::length );
                    iter++;
                }

                // assegna l'unita  misura utente agli spostamenti assiali e li inserisce nel vettore di output
                iter = uVect.begin();
                while( iter != uVect.end() ){
                    *iter = *iter * m_unitMeasure->scale( m_hypothesis->unitMeasureEta(0) );
                    iter++;
                }
                (*valNames)[0] += QString( " [") + m_unitMeasure->string( m_hypothesis->unitMeasureEta(0) ) + QString( "]" );

                // assegna l'unita  misura utente agli spostamenti trasversali e li inserisce nel vettore di output
                iter = wVect.begin();
                while( iter != wVect.end() ){
                    *iter = *iter * m_unitMeasure->scale( m_hypothesis->unitMeasureEta(1) );
                    iter++;
                }
                (*valNames)[1] +=  QString( " [") + m_unitMeasure->string( m_hypothesis->unitMeasureEta(1) ) + QString( "]" );
                // assegna l'unita  misura utente alle rotazioni e le inserisce nel vettore di output
                iter = thetaVect.begin();
                while( iter != thetaVect.end() ){
                    *iter = *iter * m_unitMeasure->scale( m_hypothesis->unitMeasureEta(2) );
                    iter++;
                }
                (*valNames)[2] +=  QString( " [") + m_unitMeasure->string( m_hypothesis->unitMeasureEta(2) ) + QString( "]" );
            }
            valVect->append(uVect);
            valVect->append( wVect );
            valVect->append( thetaVect );
        }
    }
}

QVector<QString> Beam::FPLocalNames( Hypothesis * hyp ){
    QVector<QString> ret;
    if(  hyp->spaceDim() == 2 && hyp->nDOFVert() == 3 ){
        ret.append( trUtf8("N") );
        ret.append( trUtf8("T") );
        ret.append( trUtf8("M") );
    } else if(  hyp->spaceDim() == 3 && hyp->nDOFVert() == 6 ){
        ret.append( trUtf8("N") );
        ret.append( trUtf8("Ty") );
        ret.append( trUtf8("Tz") );
        ret.append( trUtf8("Mx") );
        ret.append( trUtf8("My") );
        ret.append( trUtf8("Mz") );
    }
    return ret;
}

void Beam::FPLocal( QString * xName, QVector< double> * xVect, QVector<QString> * valNames, QVector<QVector< double> > * valVect, int nP ){
    // resetta i vettori di output
    if( xName ){
        xName->clear();
        *xName = QString("x [") + m_unitMeasure->string( UnitMeasure::length ) + "]";
    }
    xVect->clear();

    if( valNames ){
        valNames->clear();
    }
    for( int i=0; i <valVect->size(); i++ ){
        valVect[i].clear();
    }
    valVect->clear();

    if( nP > 1 ){
        // Ascissa curvilinea del punto in cui si calcolano le caratteristiche di sollecitazione
        double x = 0.0;
        // Incremento dell'ascissa curvilinea
        double Dx = L->valueNormal() / (nP - 1);
        // imposta i valori in xVect
        for( int i=0; i < nP ; i++) {
            xVect->append( x );
            x += Dx;
        }

        if( m_hypothesis->spaceDim() == 2 && m_hypothesis->nDOFVert() == 3 ){
            if( m_dd->section->A->valueNormal() != 0.0 && m_dd->section->ICyy->valueNormal() != 0.0 && m_dd->section->material()->E->valueNormal() != 0.0  && m_dd->section->material()->G->valueNormal() != 0.0 && L->valueNormal() != 0.0 ){
                // Vettori su cui vengono scritti i risultati
                QVector<double> N, T, M;

                // carichi nodali (stessa notazione presente nella documentazione teorica)
                double H1 = m_fLocal[0] + m_dd->fAppLocalArray[0];
                double V1 = m_fLocal[1] + m_dd->fAppLocalArray[1];
                double M1 = m_fLocal[2] + m_dd->fAppLocalArray[2];
                // carichi distribuiti (stessa notazione presente nella documentazione teorica)
                double h1 = m_dd->qAppLocalArray[0];
                double Dh = m_dd->qAppLocalArray[3] - m_dd->qAppLocalArray[0];
                double v1 = m_dd->qAppLocalArray[1];
                double Dv = m_dd->qAppLocalArray[4] - m_dd->qAppLocalArray[1];
                double m1 = m_dd->qAppLocalArray[2];
                double Dm = m_dd->qAppLocalArray[5] - m_dd->qAppLocalArray[2];

                // Calcolo delle caratteristiche di sollecitazione
                for( QVector<double>::iterator i = xVect->begin(); i != xVect->end(); ++i){
                    double n = (- H1  - (h1 * *i + Dh * pow(*i,2.0) / (2.0 * L->valueNormal())));
                    N.append( n * m_unitMeasure->scale( m_hypothesis->unitMeasureF(0)) );
                    double t = (- V1  - (v1 * *i + Dv * pow(*i,2.0) / (2.0 * L->valueNormal())));
                    T.append( t * m_unitMeasure->scale( m_hypothesis->unitMeasureF(1) ) );
                    double m = (- V1 * *i - (v1 * pow(*i,2.0) / 2.0 + Dv * pow(*i,3.0) / (6.0 * L->valueNormal())) - M1  - (m1 * *i + Dm * pow(*i,2.0) / (2.0 * L->valueNormal())));
                    M.append( m * m_unitMeasure->scale( m_hypothesis->unitMeasureF(2) ) );
                    *i *=  m_unitMeasure->scale( UnitMeasure::length );
                }
                valVect->append(N);
                valVect->append(T);
                valVect->append(M);
                if( valNames ){
                    valNames->append( trUtf8("N") + QString(" [") + m_unitMeasure->string( m_hypothesis->unitMeasureF(0) ) + "]");
                    valNames->append( trUtf8("T") + QString(" [") + m_unitMeasure->string( m_hypothesis->unitMeasureF(1) ) + "]");
                    valNames->append( trUtf8("M") + QString(" [") + m_unitMeasure->string( m_hypothesis->unitMeasureF(2) ) + "]");
                }
            }
        } else if( m_hypothesis->spaceDim() == 3 && m_hypothesis->nDOFVert() == 6 ){
            // TODO
            // caratteristiche di sollecitazione nel caso 3D
            QVector<double> N, Ty, Tz, Mx, My, Mz;

            // carichi nodali (stessa notazione presente nella documentazione teorica)
            double H1 = m_fLocal[0] + m_dd->fAppLocalArray[0];
            double Vy1 = m_fLocal[1] + m_dd->fAppLocalArray[1];
            double Vz1 = m_fLocal[2] + m_dd->fAppLocalArray[2];
            double Mx1 = m_fLocal[3] + m_dd->fAppLocalArray[3];
            double My1 = m_fLocal[4] + m_dd->fAppLocalArray[4];
            double Mz1 = m_fLocal[5] + m_dd->fAppLocalArray[5];
            // carichi distribuiti (stessa notazione presente nella documentazione teorica)
            double h1 = m_dd->qAppLocalArray[0];
            double Dh = m_dd->qAppLocalArray[6] - m_dd->qAppLocalArray[0];
            double vy1 = m_dd->qAppLocalArray[1];
            double Dvy = m_dd->qAppLocalArray[7] - m_dd->qAppLocalArray[1];
            double vz1 = m_dd->qAppLocalArray[2];
            double Dvz = m_dd->qAppLocalArray[8] - m_dd->qAppLocalArray[2];
            double mx1 = m_dd->qAppLocalArray[3];
            double Dmx = m_dd->qAppLocalArray[9] - m_dd->qAppLocalArray[3];
            double my1 = m_dd->qAppLocalArray[4];
            double Dmy = m_dd->qAppLocalArray[10] - m_dd->qAppLocalArray[4];
            double mz1 = m_dd->qAppLocalArray[5];
            double Dmz = m_dd->qAppLocalArray[11] - m_dd->qAppLocalArray[5];

            // Calcolo delle caratteristiche di sollecitazione
            for( QVector<double>::iterator i = xVect->begin(); i != xVect->end(); ++i){
                // TODO verificare

                *i *=  m_unitMeasure->scale( UnitMeasure::length );
            }

            valVect->append(N);
            valVect->append(Ty);
            valVect->append(Tz);
            valVect->append(Mx);
            valVect->append(My);
            valVect->append(Mz);
            if( valNames ){
                valNames->append( trUtf8("N") + QString(" [") + m_unitMeasure->string( m_hypothesis->unitMeasureF(0) ) + "]");
                valNames->append( trUtf8("Ty") + QString(" [") + m_unitMeasure->string( m_hypothesis->unitMeasureF(1) ) + "]");
                valNames->append( trUtf8("Tz") + QString(" [") + m_unitMeasure->string( m_hypothesis->unitMeasureF(2) ) + "]");
                valNames->append( trUtf8("Mx") + QString(" [") + m_unitMeasure->string( m_hypothesis->unitMeasureF(3) ) + "]");
                valNames->append( trUtf8("My") + QString(" [") + m_unitMeasure->string( m_hypothesis->unitMeasureF(4) ) + "]");
                valNames->append( trUtf8("Mz") + QString(" [") + m_unitMeasure->string( m_hypothesis->unitMeasureF(5) ) + "]");
            }
        }
    }
}

GLItem * Beam::glItemFPLocal( int nFP, int nP, double scale ){
    GLGroup * ret = new GLGroup();

    setL();

    if( nP > 1 && m_vertexes->size() > 1 && L->valueNormal() > 0.0 && nFP < m_hypothesis->nDOFVert() ){
        setLCS();
        Eigen::Map<Eigen::MatrixXd> _LCS( m_LCS, m_LCSSize, m_LCSSize);
        Eigen::Map<Eigen::VectorXd> _LCSOrigin( m_LCSOrigin, m_LCSSize);

        QVector<double> xVect;
        QVector<QVector <double> > valFPL;
        FPLocal( NULL, &xVect, NULL, &valFPL, nP );

        if( nFP < valFPL.size() ){
            Eigen::VectorXd P1 = Eigen::VectorXd::Zero( m_hypothesis->spaceDim() );
            Eigen::VectorXd P2 = Eigen::VectorXd::Zero( m_hypothesis->spaceDim() );
            Eigen::VectorXd P10 = Eigen::VectorXd::Zero( m_hypothesis->spaceDim() );
            Eigen::VectorXd P20 = Eigen::VectorXd::Zero( m_hypothesis->spaceDim() );
            Eigen::VectorXd valFPL1 = Eigen::VectorXd::Zero( m_hypothesis->spaceDim() );
            Eigen::VectorXd valFPL2 = Eigen::VectorXd::Zero( m_hypothesis->spaceDim() );
            for( int i = 1; (i < xVect.size()); ++i ){
                valFPL1( m_hypothesis->spaceDim() - 1 ) = valFPL.at(nFP).at(i-1);
                valFPL2( m_hypothesis->spaceDim() - 1 ) = valFPL.at(nFP).at(i);

                P10 = xVect.at(i-1) * _LCS.row(0).transpose() + _LCSOrigin;
                P1 = P10 + scale * _LCS.transpose() * valFPL1;
                P10 *= m_unitMeasure->scale( UnitMeasure::length );
                P1 *= m_unitMeasure->scale( UnitMeasure::length );
                P20 = xVect.at(i) * _LCS.row(0).transpose() + _LCSOrigin;
                P2 = P20 + scale * _LCS.transpose() * valFPL2;
                P2 *= m_unitMeasure->scale( UnitMeasure::length );
                P20 *= m_unitMeasure->scale( UnitMeasure::length );
                if( (valFPL.at(nFP).at(i) * valFPL.at(nFP).at(i-1)) >= 0.0 ){
                    GLLine * valL = new GLLine();
                    valL->setWidth(2);
                    if( (valFPL.at(nFP).at(i) < 0.0) || (valFPL.at(nFP).at(i-1) < 0.0) ){
                        valL->setColor( Qt::blue );
                    } else {
                        valL->setColor( Qt::red );
                    }
                    valL->setGroup( ret );
                    if( m_hypothesis->spaceDim() == 1 ){
                        valL->setP0( P1(0), 0.0, 0.0 );
                        valL->setP1( P2(0), 0.0, 0.0 );
                    } else if( m_hypothesis->spaceDim() == 2 ){
                        valL->setP0( P1(0), P1(1), 0.0 );
                        valL->setP1( P2(0), P2(1), 0.0 );
                    } else if( m_hypothesis->spaceDim() > 2 ){
                        valL->setP0( P1(0), P1(1), P1(2) );
                        valL->setP1( P2(0), P2(1), P2(2) );
                    }
                } else {
                    Eigen::VectorXd P0 = Eigen::VectorXd::Zero( m_hypothesis->spaceDim() );
                    double x0 = xVect.at(i-1) + (xVect.at(i) - xVect.at(i-1)) / (fabs(valFPL.at(nFP).at(i)) + fabs(valFPL.at(nFP).at(i-1))) * fabs(valFPL.at(nFP).at(i-1));
                    P0 = x0 * _LCS.row(0).transpose() + _LCSOrigin;
                    GLLine * valL1 = new GLLine();
                    valL1->setWidth(2);
                    if( valFPL.at(nFP).at(i-1) < 0.0 ){
                        valL1->setColor( Qt::blue );
                    } else {
                        valL1->setColor( Qt::red );
                    }
                    valL1->setGroup( ret );
                    if( m_hypothesis->spaceDim() == 1 ){
                        valL1->setP0( P1(0), 0.0, 0.0 );
                        valL1->setP1( P0(0), 0.0, 0.0 );
                    } else if( m_hypothesis->spaceDim() == 2 ){
                        valL1->setP0( P1(0), P1(1), 0.0 );
                        valL1->setP1( P0(0), P0(1), 0.0 );
                    } else if( m_hypothesis->spaceDim() > 2 ){
                        valL1->setP0( P1(0), P1(1), P1(2) );
                        valL1->setP1( P0(0), P0(1), P0(2) );
                    }
                    GLLine * valL2 = new GLLine();
                    valL2->setWidth(2);
                    if( valFPL.at(nFP).at(i) < 0.0 ){
                        valL2->setColor( Qt::blue );
                    } else {
                        valL2->setColor( Qt::red );
                    }
                    valL2->setGroup( ret );
                    if( m_hypothesis->spaceDim() == 1 ){
                        valL2->setP0( P0(0), 0.0, 0.0 );
                        valL2->setP1( P2(0), 0.0, 0.0 );
                    } else if( m_hypothesis->spaceDim() == 2 ){
                        valL2->setP0( P0(0), P0(1), 0.0 );
                        valL2->setP1( P2(0), P2(1), 0.0 );
                    } else if( m_hypothesis->spaceDim() > 2 ){
                        valL2->setP0( P0(0), P0(1), P0(2) );
                        valL2->setP1( P2(0), P2(1), P2(2) );
                    }
                }
                if( valFPL.at(nFP).at(i) != 0.0 ){
                    GLLine * postL = new GLLine();
                    postL->setWidth(2);
                    if( valFPL.at(nFP).at(i) > 0.0 ){
                        postL->setColor( Qt::red );
                    } else {
                        postL->setColor( Qt::blue );
                    }
                    postL->setGroup( ret );
                    if( m_hypothesis->spaceDim() == 1 ){
                        postL->setP0( P20(0), 0.0, 0.0 );
                        postL->setP1( P2(0), 0.0, 0.0 );
                    } else if( m_hypothesis->spaceDim() == 2 ){
                        postL->setP0( P20(0), P20(1), 0.0 );
                        postL->setP1( P2(0), P2(1), 0.0 );
                    } else if( m_hypothesis->spaceDim() > 2 ){
                        postL->setP0( P20(0), P20(1), P20(2) );
                        postL->setP1( P2(0), P2(1), P2(2) );
                    }
                }
                if( i==1 ){
                    if( valFPL.at(nFP).at(i) != 0.0 ) {
                        GLLine * preL = new GLLine();
                        preL->setWidth(2);
                        if( valFPL.at(nFP).at(i) > 0.0 ) {
                            preL->setColor( Qt::red );
                        } else {
                            preL->setColor( Qt::blue );
                        }
                        preL->setGroup( ret );
                        if( m_hypothesis->spaceDim() == 1 ){
                            preL->setP0( P10(0), 0.0, 0.0 );
                            preL->setP1( P1(0), 0.0, 0.0 );
                        } else if( m_hypothesis->spaceDim() == 2 ){
                            preL->setP0( P10(0), P10(1), 0.0 );
                            preL->setP1( P1(0), P1(1), 0.0 );
                        } else if( m_hypothesis->spaceDim() > 2 ){
                            preL->setP0( P10(0), P10(1), P20(2) );
                            preL->setP1( P1(0), P1(1), P2(2) );
                        }
                    }
                }
            }
        }
    }
    return ret;
}

DoublePlus * Beam::fAppLocal(int i ) {
    if( i < m_dd->fAppLocalList.size() ){
        return m_dd->fAppLocalList.at(i);
    } else {
        return NULL;
    }
}

BeamFModel * Beam::fAppModel(){
    return m_dd->fAppModel;
}

DoublePlus * Beam::qAppLocal(int i ) {
    if( i < m_dd->qAppLocalList.size() ){
        return m_dd->qAppLocalList.at(i);
    } else {
        return NULL;
    }
}

BeamQModel * Beam::qAppModel(){
    return m_dd->qAppModel;
}

#include <QApplication>

class BeamTypePrivate{
    Q_DECLARE_TR_FUNCTIONS(BeamTypePrivate)

    class enumVal{
    public:
        enumVal(Beam::Type v, const QString & ns, const QString & s):
            val(v),
            normalStr(ns),
            str(s){
        };
        Beam::Type val;
        QString normalStr;
        QString str;
    };

public:
    BeamTypePrivate(Beam::Type v):
        value(v){
        enumList.append( enumVal( Beam::Bernoulli, "Bernoulli", trUtf8("Teoria di Bernoulli")) );
        enumList.append( enumVal( Beam::Timoshenko, "Timoshenko", trUtf8("Teoria di Timoshenko")) );
        enumList.append( enumVal( Beam::Winkler, "Winkler", trUtf8("Trave elastica su suolo alla Winkler")) );
    };
    ~BeamTypePrivate(){
    };

    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    Beam::Type value;
    QList<enumVal> enumList;
};

BeamType::BeamType( Beam::Type bt,
                    const QString & nn,
                    bool ro):
    EnumPlus( nn, ro),
    m_d( new BeamTypePrivate(bt )){
}

BeamType::~BeamType(){
    delete m_d;
}

BeamType & BeamType::operator=( BeamType & val){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & BeamType::operator=( VarPlus & val){
    this->VarPlus::operator=( val );

    if( this != &val ){
        BeamType * valCncrModelPlus = dynamic_cast<BeamType *>(&val);
        if(valCncrModelPlus) {
            setValueNormal( valCncrModelPlus->m_d->value );
        }
    }
    return *this;
}

QString BeamType::typeVar(){
    return QString("BeamType");
}

Beam::Type BeamType::value(){
    return m_d->value;
}

Beam::Type BeamType::valueNormal(){
    return m_d->value;
}

QString BeamType::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString BeamType::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

void BeamType::setValueNormal( Beam::Type vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void BeamType::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void BeamType::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

void BeamType::setValue( Beam::Type vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void BeamType::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

int BeamType::enumCount() {
    return m_d->enumList.size();
}

QString BeamType::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int BeamType::valueNormalInt() {
    return m_d->valueIndex();
}
