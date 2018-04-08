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
#include "element.h"

#include "elementvertexmodel.h"
#include "vertex.h"
#include "hypothesis.h"
#include "boolplus.h"
#include "doubleplus.h"
#include <QList>
#include <Eigen/Core>

VertexElement::VertexElement( Vertex * vert ):
    vertex(vert),
    vertexConnection( new QList<BoolPlus *> ) {
    for( int i=0; i < vertex->hypothesis()->nDOFVert(); i++ ){
        vertexConnection->append( new BoolPlus(true, "vertexConnection" + QString::number(i)));
    }
}

unsigned int Element::s_nextId = 1;

Element::Element( unsigned int iidd, Hypothesis * hyp, UnitMeasure * ump, QObject * parent ):
    QObject(parent),
    m_id( iidd ),
    m_hypothesis( hyp ),
    m_unitMeasure( ump ),
    m_vertexes( new QList<VertexElement *> ),
    m_vertexModel( NULL ),
    m_LCS(NULL),
    m_LCSOrigin(NULL),
    m_N(NULL),
    m_f0Local(NULL),
    m_f0Global(NULL),
    m_KLocal(NULL),
    m_KGlobal(NULL),
    m_etaLocal(NULL),
    m_fLocal(NULL){
}

Element::~Element(){
    delete m_vertexes;
    if( m_vertexModel )
        delete m_vertexModel;
    if( m_LCS )
        delete [] m_LCS;
    if( m_LCSOrigin )
        delete [] m_LCSOrigin;
    if( m_N )
        delete [] m_N;
    if( m_f0Local )
        delete [] m_f0Local;
    if( m_f0Global )
        delete [] m_f0Global;
    if( m_KLocal )
        delete []m_KLocal;
    if( m_KGlobal )
        delete [] m_KGlobal;
    if(m_etaLocal )
        delete [] m_etaLocal;
    if( m_fLocal )
        delete [] m_fLocal;
}

void Element::commonInit() {

    // crea le matrici KLocal, KLocalGlobal, KGlobal ed N
    m_nDOF = m_hypothesis->nDOFVert() * m_vertexes->size();
    m_N = new double[ m_nDOF * m_nDOF ];
    m_KLocal = new double[ m_nDOF * m_nDOF ];
    m_KGlobal = new double[ m_nDOF * m_nDOF ];

    // crea i vettori etaLocal, etaGlobal, fLocal, f0Local, f0LocalGlobal, f0Global
    m_f0Local = new double[ m_nDOF ];
    m_f0Global = new double[ m_nDOF ];
    m_etaLocal = new double[ m_nDOF ];
    m_fLocal = new double[ m_nDOF ];

    int k=0;
    double val = 0.0;
    for( int i=0; i < m_nDOF; i++ ){
        // inizializza le matrici KLocal, KGlobal ed N alla matrice identitÃ
        for( int j=0; j < m_nDOF; j++ ){
            if( i == j ){
                val = 1.0;
            } else {
                val = 0.0;
            }
            m_N[ k ] = val;
            m_KLocal[ k ] = val;
            m_KGlobal[ k ] = val;
        }
        k++;
        // inizializza i vettori etaLocal, etaGlobal, fLocal, f0Local, f0Global
        // al vettore nullo
        m_f0Local[ i ] = 0.0;
        m_f0Global[ i ] = 0.0;
        m_etaLocal[ i ] = 0.0;
        m_fLocal[ i ] = 0.0;
    }

    // crea la matrice LCS
    m_LCSSize = m_hypothesis->spaceDim();
    m_LCS = new double[ m_LCSSize * m_LCSSize ];
    m_LCSOrigin = new double[ m_LCSSize ];

    // inizializza la matrice LCS alla matrice identitÃ
    k = 0;
    for( int i=0; i < m_LCSSize; i++ ){
        for( int j=0; j < m_LCSSize; j++ ){
            if( i == j ){
                m_LCS[k] = 1.0;
            } else {
                m_LCS[k] = 0.0;
            }
        }
        k++;
        m_LCSOrigin[i] = 0.0;
    }
}

Hypothesis * Element::hypothesis(){
    return m_hypothesis;
}

int Element::nDOF(){
    return m_nDOF;
}

int Element::nVertexes(){
    return m_vertexes->size();
}

Vertex * Element::vertex(int index){
    if( index < m_vertexes->size() ){
        return m_vertexes->at(index)->vertex;
    } else {
        return NULL;
    }
}

void Element::setVertexes(const QList<Vertex *> & vertL) {
    int i=0;
    while( i < m_vertexes->size() && i< vertL.size() ){
        Vertex * oldVert = (*m_vertexes)[i]->vertex;
        (*m_vertexes)[i]->vertex = vertL.at(i);
        Vertex * newVert = (*m_vertexes)[i]->vertex;
        emit vertexChanged( i, oldVert, newVert );
        i++;
    }
}

void Element::setVertex(int i, Vertex *vert){
    if( i < m_vertexes->size() ){
        Vertex * oldVert = (*m_vertexes)[i]->vertex;
        (*m_vertexes) [i]->vertex = vert;
        Vertex * newVert = (*m_vertexes)[i]->vertex;
        emit vertexChanged( i, oldVert, newVert );
    }
}

bool Element::isVertexUsed( Vertex * v ){
    QList<VertexElement *>::iterator iter = m_vertexes->begin();
    while( iter != m_vertexes->end() ){
        if( (*iter)->vertex == v ){
            return true;
        }
        iter++;
    }
    return false;
}

QList<BoolPlus *> * Element::vertexConnection(int vert ) {
    if( vert >= 0 && vert < m_vertexes->size() ){
        return m_vertexes->at(vert)->vertexConnection;
    }
    return NULL;
}

BoolPlus * Element::vertexConnection( int vert, int ind ){
    if( vert < m_vertexes->size() ){
        if( ind < m_vertexes->at( vert )->vertexConnection->size() ){
            return m_vertexes->at(vert)->vertexConnection->at(ind);
        }
    }
    return NULL;
}


void Element::createVertexModel( VertexModel * vm ){
    if( m_vertexModel == NULL ){
        m_vertexModel = new ElementVertexModel( vm, this);
        connect( m_vertexModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateQGraphicsItem()));
    }
}

ElementVertexModel * Element::vertexModel(){
    return m_vertexModel;
}

void  Element::nextId( void ){
    m_id = s_nextId++;
}

unsigned int Element::id(){
    return m_id;
}

void Element::setN(){
    setLCS();
    Eigen::Map<Eigen::MatrixXd> _N( m_N, m_nDOF, m_nDOF);
    _N.setIdentity();

    Eigen::Map<Eigen::MatrixXd> _LCS( m_LCS, m_LCSSize, m_LCSSize );
    Eigen::MatrixXd NVert = m_hypothesis->N( _LCS );
    for(int i=0; i < m_vertexes->size(); i++ ){
        _N.block( m_hypothesis->nDOFVert()*i,
                  m_hypothesis->nDOFVert()*i,
                  m_hypothesis->nDOFVert(),
                  m_hypothesis->nDOFVert()) = NVert;
    }
}

void Element::setKF0Global(){
    setKF0Local();

    // Applica le connessioni tra l'elemento e i vertici analizzati
    Eigen::MatrixXd _KCon = Eigen::Map<Eigen::MatrixXd>( m_KLocal, m_nDOF, m_nDOF);
    Eigen::VectorXd _f0Con = Eigen::Map<Eigen::VectorXd>( m_f0Local, m_nDOF);

    int m=0;
    for( int i=0; i < m_vertexes->size(); i++){
        for( int j=0; j < m_hypothesis->nDOFVert(); j++ ){
            if( ! (m_vertexes->at(i)->vertexConnection->at(j)->valueNormal() ) ){
                // colonna di indice m
                Eigen::VectorXd k_cm = _KCon.col(m);
                // Dividiamo ciascuna riga per il termine della riga corrispondente alla connessione
                for( int k = 0; k < _KCon.rows(); k++ ){
                    if( k_cm(k) != 0.0 ){
                        _KCon.row(k) /= k_cm(k);
                        _f0Con(k) /= k_cm(k);
                    }
                }
                // riga di indice m
                Eigen::MatrixXd k_rm = _KCon.row(m);
                // termine noto di indice m
                double  f0_m = _f0Con(m);
                // sottraiamo a ciascuna riga la riga di indice m
                for( int k = 0; k < _KCon.rows(); k++ ){
                    _KCon.row(k) -= k_rm;
                    _f0Con(k) -= f0_m;
                }
                // moltiplichiamo nuovamente le righe per il termine della riga corrispondente alla connessione
                for( int k = 0; k < _KCon.rows(); k++ ){
                    _KCon.row(k) *= k_cm(k);
                    _f0Con(k) *= k_cm(k);
                }
            }
            m++;
        }
    }

    setN();

    Eigen::Map<Eigen::MatrixXd> _N( m_N, m_nDOF, m_nDOF);

    Eigen::Map<Eigen::VectorXd> _f0Global( m_f0Global, m_nDOF);
    _f0Global = _N.transpose() * _f0Con;

    Eigen::Map<Eigen::MatrixXd> _KGlobal( m_KGlobal, m_nDOF, m_nDOF);
    _KGlobal = _N.transpose() * _KCon * _N;

}

void Element::setFEtaLocal(){
    // vettore degli spostamenti nodali globali
    Eigen::VectorXd etaGlobal = Eigen::VectorXd::Zero( m_nDOF );

    // copia gli spostamenti nodali dei vertici nel vettore etaGlobal
    int m=0;
    for( int i=0; i < m_vertexes->size(); i++){
        for( int j=0; j < m_hypothesis->nDOFVert(); j++){
            etaGlobal(m) = m_vertexes->at(i)->vertex->etaGlobal(j)->valueNormal();
            m++;
        }
    }

    // Calcola gli spostamenti nodali locali
    Eigen::Map<Eigen::MatrixXd> _N(m_N, m_nDOF, m_nDOF);
    Eigen::Map<Eigen::VectorXd> _etaLocal( m_etaLocal, m_nDOF);
    _etaLocal = _N * etaGlobal;

    /* Annulliamo le colonne della matrice locale in corrispondenza degli spostamenti nodali
       non connessi */
    Eigen::MatrixXd KCon = Eigen::Map<Eigen::MatrixXd>( m_KLocal, m_nDOF, m_nDOF);
    Eigen::VectorXd f0Con = Eigen::Map<Eigen::VectorXd>( m_f0Local, m_nDOF);
    m=0;
    for( int i=0; i < m_vertexes->size(); i++){
        for( int j=0; j < m_hypothesis->nDOFVert(); j++ ){
            if( ! (m_vertexes->at(i)->vertexConnection->at(j)->valueNormal() ) ){
                // colonna di indice m
                Eigen::VectorXd k_cm = KCon.col(m);
                // Dividiamo ciascuna riga per il termine della riga corrispondente alla connessione
                for( int k = 0; k < KCon.rows(); k++ ){
                    if( k_cm(k) != 0.0 ){
                        KCon.row(k) /= k_cm(k);
                        f0Con(k) /= k_cm(k);
                    }
                }
                // riga di indice m
                Eigen::MatrixXd k_rm = KCon.row(m);
                // termine noto di indice m
                double  f0_m = f0Con(m);
                // sottraiamo a ciascuna riga la riga di indice m
                for( int k = 0; k < KCon.rows(); k++ ){
                    if( k != m ){
                        KCon.row(k) -= k_rm;
                        f0Con(k) -= f0_m;
                    }
                }
                // moltiplichiamo nuovamente le righe per il termine della riga corrispondente alla connessione
                for( int k = 0; k < KCon.rows(); k++ ){
                    KCon.row(k) *= k_cm(k);
                    f0Con(k) *= k_cm(k);
                }
            }
            m++;
        }
    }

    /* Per gli spostamenti nodali non connessi di indice m, si invertono gli spostamenti
       con le forze nodali
         etaLocal(m) <-> fLocal(m)
       Si modifica quindi la matrice locale ed il vettore dei carichi nodali equivalenti
       per fare in modo che, a seguito della sostituzione,
         {fLocal} = [KCon] * {etaLocal} + {f0Con}
       a seguire sarà quindi necessario effettuare nuovamente la sostituzione
          fLocal(m) <-> etaLocal(m) */
    m=0;
    for( int i=0; i < m_vertexes->size(); i++){
        for( int j=0; j < m_hypothesis->nDOFVert(); j++ ){
            if( ! m_vertexes->at(i)->vertexConnection->at(j)->valueNormal() ){
                double k_mm = KCon(m,m);
                if( k_mm != 0.0 ){
                    KCon.row(m) /= -k_mm;
                    KCon(m, m) = 1.0 / k_mm;
                    f0Con(m) /= -k_mm;
                    _etaLocal(m) = 0.0;
                }
            }
            m++;
        }
    }

    // Calcola le forze nodali
    Eigen::Map<Eigen::VectorXd> _fLocal( m_fLocal, m_nDOF);
    _fLocal = KCon * _etaLocal + f0Con;

    // Per i vertici non connessi, scambia i valori di spostamenti e forze nodali
    m = 0;
    for( int i=0; i < m_vertexes->size(); i++){
        for( int j=0; j < m_hypothesis->nDOFVert(); j++ ){
            if( !m_vertexes->at(i)->vertexConnection->at(j)->valueNormal() ){
                double tmp = _fLocal(m);
                _fLocal(m) = _etaLocal(m);
                _etaLocal(m) = tmp;
            }
            m++;
        }
    }
}

Eigen::MatrixXd Element::KGlobalVert( Vertex * vert1, Vertex * vert2 ){
    int ndofv = m_hypothesis->nDOFVert();
    Eigen::MatrixXd ret( ndofv, ndofv );
    ret.setZero();
    Eigen::Map<Eigen::MatrixXd> _KGlobal( m_KGlobal, m_nDOF, m_nDOF);
    for( int i=0; i < m_vertexes->size(); i++ ){
        if( m_vertexes->at(i)->vertex == vert1 ){
            for( int j=0; j < m_vertexes->size(); j++ ){
                if( m_vertexes->at(j)->vertex == vert2 ){
                    ret += _KGlobal.block(ndofv*i,ndofv*j,ndofv,ndofv);
                }
            }
        }
    }
    return ret;
}

Eigen::VectorXd Element::f0GlobalVert( Vertex * vert ){
    int ndofv = m_hypothesis->nDOFVert();
    Eigen::VectorXd ret = Eigen::VectorXd::Zero( ndofv );
    Eigen::Map<Eigen::VectorXd> _f0Global( m_f0Global, m_nDOF );
    for( int i=0; i < m_vertexes->size(); i++ ){
        if( m_vertexes->at(i)->vertex == vert ){
            ret += _f0Global.block(ndofv*i,0,ndofv,1);
        }
    }
    return ret;
}
