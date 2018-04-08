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

#include "ssbeam.h"

#include "femstructure.h"
#include "vertexmodel.h"
#include "elementmodel.h"
#include "beam.h"
#include "hypothesis2d.h"
#include "loadcompltablemodel.h"
#include "loadcompl.h"
#include "loadtype.h"
#include "section.h"
#include "serviceclass.h"
#include "boolplus.h"
#include "doubleplus.h"

#include <cmath>

SSBeam::SSBeam( UnitMeasure * ump, Section * s):
    QObject(NULL),
    m_unitMeasure(ump),
    m_section(s){
    initVar();
}

void SSBeam::initVar(){
    alpha = new DoublePlus(0.0, "alpha", m_unitMeasure, UnitMeasure::angle );
    alpha->setRichName( trUtf8("α"));
    alpha->setToolTip( trUtf8("Inlinazione porzione trave tra appoggi"));
    connect( alpha, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL()));
    connect( alpha, SIGNAL(valueChanged(QString)), this, SLOT(setL()));

    alpha1 = new DoublePlus(0.0, "alpha1", m_unitMeasure, UnitMeasure::angle );
    alpha1->setRichName( trUtf8("α<span style=\" vertical-align:sub;\">1</span>"));
    alpha1->setToolTip( trUtf8("Inlinazione sbalzo sinistro trale"));
    connect( alpha1, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL1()));
    connect( alpha1, SIGNAL(valueChanged(QString)), this, SLOT(setL1()));

    alpha2 = new DoublePlus(0.0, "alpha2", m_unitMeasure, UnitMeasure::angle );
    alpha2->setRichName( trUtf8("α<span style=\" vertical-align:sub;\">2</span>"));
    alpha2->setToolTip( trUtf8("Inlinazione sbalzo destro trale"));
    connect( alpha2, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL2()));
    connect( alpha2, SIGNAL(valueChanged(QString)), this, SLOT(setL2()));

    L = new DoublePlus(5.0, "L", m_unitMeasure, UnitMeasure::length );
    L->setRichName( trUtf8("L"));
    L->setToolTip( trUtf8("Lunghezza porzione trave tra appoggi"));
    connect( L, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL()));
    connect( L, SIGNAL(valueChanged(QString)), this, SLOT(setL()));

    L1 = new DoublePlus(1.0, "L1", m_unitMeasure, UnitMeasure::length );
    L1->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">1</span>"));
    L1->setToolTip( trUtf8("Lunghezza sbalzo sinistro"));
    connect( L1, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL1()));
    connect( L1, SIGNAL(valueChanged(QString)), this, SLOT(setL1()));

    L2 = new DoublePlus(1.0, "L2", m_unitMeasure, UnitMeasure::length );
    L2->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">2</span>"));
    L2->setToolTip( trUtf8("Lunghezza sbalzo destro"));
    connect( L2, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL2()));
    connect( L2, SIGNAL(valueChanged(QString)), this, SLOT(setL2()));

    LHor = new DoublePlus(5.0, "LHor", m_unitMeasure, UnitMeasure::length, true  );
    LHor->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">h</span>"));
    LHor->setToolTip( trUtf8("Proiezione sul piano orizzontale della prozione di trave tra gli appoggi"));
    connect( LHor, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL()));
    connect( LHor, SIGNAL(valueChanged(QString)), this, SLOT(setL()));

    LHor1 = new DoublePlus(1.0, "LHor1", m_unitMeasure, UnitMeasure::length );
    LHor1->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">h,1</span>"));
    LHor1->setReadOnly( true );
    LHor1->setToolTip( trUtf8("Proiezione sul piano orizzontale dello sbalzo sinistro"));
    connect( LHor1, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL1()));
    connect( LHor1, SIGNAL(valueChanged(QString)), this, SLOT(setL1()));

    LHor2 = new DoublePlus(1.0, "LHor2", m_unitMeasure, UnitMeasure::length );
    LHor2->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">h,2</span>"));
    LHor2->setReadOnly( true );
    LHor2->setToolTip( trUtf8("Proiezione sul piano orizzontale dello sbalzo destro"));
    connect( LHor2, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL2()));
    connect( LHor2, SIGNAL(valueChanged(QString)), this, SLOT(setL2()));

    LVer = new DoublePlus(0.0, "LVer", m_unitMeasure, UnitMeasure::length );
    LVer->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">v</span>"));
    LVer->setReadOnly( true );
    LVer->setToolTip( trUtf8("Proiezione sul piano verticale della prozione di trave tra gli appoggi"));
    connect( LVer, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL()));
    connect( LVer, SIGNAL(valueChanged(QString)), this, SLOT(setL()));

    LVer1 = new DoublePlus(0.0, "LVer1", m_unitMeasure, UnitMeasure::length );
    LVer1->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">v,1</span>"));
    LVer1->setReadOnly( true );
    LVer1->setToolTip( trUtf8("Proiezione sul piano verticale dello sbalzo sinistro"));
    connect( LVer1, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL1()));
    connect( LVer1, SIGNAL(valueChanged(QString)), this, SLOT(setL1()));

    LVer2 = new DoublePlus(0.0, "LVer2", m_unitMeasure, UnitMeasure::length );
    LVer2->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">v,2</span>"));
    LVer2->setReadOnly( true );
    LVer2->setToolTip( trUtf8("Proiezione sul piano verticale dello sbalzo destro"));
    connect( LVer2, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL2()));
    connect( LVer2, SIGNAL(valueChanged(QString)), this, SLOT(setL2()));

    setL();
    setL1();
    setL2();

    L0LT = new DoublePlus(0.0, "L0LT", m_unitMeasure, UnitMeasure::length );
    L0LT->setRichName( trUtf8("L<span style=\" vertical-align:sub;\">0,LT</span>"));
    L0LT->setToolTip( trUtf8("Distanza massima tra ritegni torsionali"));
    L0LT->setReadOnly( true );
    connect( L0LT, SIGNAL(readOnlyChanged(bool)), this, SLOT(setL0LT()));
    connect( L, SIGNAL(valueChanged(QString)), this, SLOT(setL0LT()));
    setL0LT();
}

void SSBeam::setSection( Section * ss ){
    if( ss != m_section  ){
        m_section = ss;
        emit sectionChanged();
    }
}

Section * SSBeam::section(){
    return m_section;
}

bool SSBeam::verify( QString *, QString *, LoadComplTableModel *){
    return true;
}

void SSBeam::calcS( LoadComplTableModel * lModel ){
    if( L->valueNormal() > 0.0 ){
        QList<double> nodes;
        if( L1->valueNormal() > 0.0 ){
            nodes.append( -L1->valueNormal() );
        }
        nodes.append(0.0);
        nodes.append(L->valueNormal());
        if( L2->valueNormal() > 0.0 ){
            nodes.append( -L2->valueNormal() );
        }
        for( int k = 0; k < lModel->count(); k++){
            LoadCompl * load = lModel->load(k);
            switch( load->loadType->valueNormal() ){
            case LoadType::loadPoint :{
                nodes.append( load->P(2)->valueNormal() );
                break;
            }
            case LoadType::loadLine :{
                nodes.append( load->P(2)->valueNormal() );
                nodes.append( load->P(5)->valueNormal() );
                break;
            }
            case LoadType::loadArea :{
                nodes.append( load->P(3)->valueNormal() );
                nodes.append( load->P(7)->valueNormal() );
                break;
            }
            case LoadType::loadVolume :{
                break;
            }
            }
        }
        // Riordina la lista
        qSort( nodes );
        // Elimina i duplicati
        for( int i=0; i < nodes.size(); i++ ){
            if( (i+1) < nodes.size() ){
                while( nodes.at(i) == nodes.at( i+1 ) ){
                    nodes.removeAt( i+1 );
                    if( (i+1) < nodes.size() )
                        break;
                }
            }
        }

        // Elimina i valori fuori dal range
        for( int i=0; i < nodes.size(); i++ ){
            if( nodes.at(i) < -L1->valueNormal() ){
                nodes.removeAt(i);
            }
            if( nodes.at(i) > (L->valueNormal() + L2->valueNormal()) ){
                nodes.removeAt(i);
            }
        }

        FEMStructure femStr(  m_section, new Hypothesis2D(), m_unitMeasure, this );
        for( int i=0; i < nodes.size(); i++ ){
            if( nodes.at(i) < 0.0 ){
                femStr.vertexModel()->appendRow();
                femStr.vertexModel()->lastVertex()->xGlobal(0)->setValueNormal( nodes.at(i) * cos(alpha1->valueNormal()) );
                femStr.vertexModel()->lastVertex()->xGlobal(1)->setValueNormal( nodes.at(i) * sin(alpha1->valueNormal()) );
            } else if( nodes.at(i) == 0.0 ){
                femStr.vertexModel()->appendRow();
                femStr.vertexModel()->lastVertex()->xGlobal(0)->setValueNormal( 0.0 );
                femStr.vertexModel()->lastVertex()->xGlobal(1)->setValueNormal( 0.0 );
                femStr.vertexModel()->lastVertex()->isEtaSet(0)->setValueNormal( true );
                femStr.vertexModel()->lastVertex()->etaGlobal(0)->setValueNormal( 0.0 );
                femStr.vertexModel()->lastVertex()->isEtaSet(1)->setValueNormal( true );
                femStr.vertexModel()->lastVertex()->etaGlobal(1)->setValueNormal( 0.0 );
            } else if( nodes.at(i) < L->valueNormal() ){
                femStr.vertexModel()->appendRow();
                femStr.vertexModel()->lastVertex()->xGlobal(0)->setValueNormal( nodes.at(i) * cos(alpha->valueNormal()) );
                femStr.vertexModel()->lastVertex()->xGlobal(1)->setValueNormal( nodes.at(i) * sin(alpha->valueNormal()) );
            } else if( nodes.at(i) == L->valueNormal() ){
                femStr.vertexModel()->appendRow();
                femStr.vertexModel()->lastVertex()->xGlobal(0)->setValueNormal( LHor->valueNormal() );
                femStr.vertexModel()->lastVertex()->xGlobal(1)->setValueNormal( LVer->valueNormal() );
                femStr.vertexModel()->lastVertex()->isEtaSet(0)->setValueNormal( true );
                femStr.vertexModel()->lastVertex()->etaGlobal(0)->setValueNormal( 0.0 );
                femStr.vertexModel()->lastVertex()->isEtaSet(1)->setValueNormal( true );
                femStr.vertexModel()->lastVertex()->etaGlobal(1)->setValueNormal( 0.0 );
            } else if( nodes.at(i) <= (L->valueNormal() + L2->valueNormal()) ){
                femStr.vertexModel()->appendRow();
                femStr.vertexModel()->lastVertex()->xGlobal(0)->setValueNormal( LHor->valueNormal() + (nodes.at(i) - L->valueNormal()) * cos(alpha2->valueNormal()) );
                femStr.vertexModel()->lastVertex()->xGlobal(1)->setValueNormal( LVer->valueNormal() + (nodes.at(i) - L->valueNormal()) * sin(alpha2->valueNormal()) );
            }
        }
        for( int i=1; i < femStr.vertexModel()->vertexCount(); i++ ){
            femStr.elementModel()->append( ElementModel::elementBeam );
            QList<Vertex *> vertL;
            vertL.append( femStr.vertexModel()->vertex(i-1));
            vertL.append( femStr.vertexModel()->vertex(i));
            femStr.elementModel()->lastElement()->setVertexes( vertL );
        }

        for( int i=0; i < nodes.size(); i++ ){
            for( int k = 0; k < lModel->count(); k++){
                LoadCompl * load = lModel->load(k);
                switch( load->loadType->valueNormal() ){
                case LoadType::loadPoint :{
                    // Verifica se il vertice è caricato
                    if( load->P(3)->valueNormal() == nodes.at(i) ){
                        femStr.vertexModel()->vertex(i)->fGlobal( 0 )->setValueNormal( load->P(0)->valueNormal() );
                        femStr.vertexModel()->vertex(i)->fGlobal( 1 )->setValueNormal( load->P(1)->valueNormal() );
                        femStr.vertexModel()->vertex(i)->fGlobal( 2 )->setValueNormal( load->P(2)->valueNormal() );
                    }
                    break;
                }
                case LoadType::loadLine :{
                    //Verifica se la trave è caricata
                    if( (i+1) < nodes.size()  ){
                        if( nodes.at(i) >= load->P(2)->valueNormal() && nodes.at(i+1) <= load->P(5)->valueNormal() ){
                            Beam * b = dynamic_cast<Beam *> (femStr.elementModel()->element(i));
                            if( b ){
                                /* b->qGlobal(1,0)->setValueNormal( load->gamma->valueNormal() * load->P(0)->valueNormal() );
                                b->qGlobal(1,1)->setValueNormal( load->gamma->valueNormal() * load->P(1)->valueNormal() );
                                b->qGlobal(1,3)->setValueNormal( load->gamma->valueNormal() * load->P(3)->valueNormal() );
                                b->qGlobal(1,4)->setValueNormal( load->gamma->valueNormal() * load->P(4)->valueNormal() );*/
                            }
                        }
                    }
                    break;
                }
                case LoadType::loadArea :{
                    if( (i+1) < nodes.size()  ){
                        if( nodes.at(i) >= load->P(3)->valueNormal() && nodes.at(i+1) <= load->P(7)->valueNormal() ){
                            Beam * b = dynamic_cast<Beam *> (femStr.elementModel()->element(i));
                            if( b ){
                                /*b->qGlobal(1,0)->setValueNormal( load->gamma->valueNormal() * load->P(0)->valueNormal() * load->P(2)->valueNormal() );
                                b->qGlobal(1,1)->setValueNormal( load->gamma->valueNormal() * load->P(1)->valueNormal() * load->P(2)->valueNormal() );
                                b->qGlobal(1,3)->setValueNormal( load->gamma->valueNormal() * load->P(4)->valueNormal() * load->P(6)->valueNormal() );
                                b->qGlobal(1,4)->setValueNormal( load->gamma->valueNormal() * load->P(5)->valueNormal() * load->P(6)->valueNormal() );*/
                            }
                        }
                    }
                    break;
                }
                case LoadType::loadVolume :{
                    if( (i+1) < nodes.size()  ){
                        Beam * b = dynamic_cast<Beam *> (femStr.elementModel()->element(i));
                        if( b ){
                            /*b->qGlobalLocal(1)->setValueNormal( load->gamma->valueNormal() * load->P(0)->valueNormal() * b->section()->pW->valueNormal() );
                            b->qGlobalLocal(4)->setValueNormal( load->gamma->valueNormal() * load->P(0)->valueNormal() * b->section()->pW->valueNormal() );*/
                        }
                    }
                    break;
                }
                }
            }
        }

        // Imposta i parametri e le equazioni alla base del problema
        femStr.setProblem();

        // Risolve il problema
        femStr.solveProblem();
    }
}

void SSBeam::setL0LT(){
    if( L0LT->readOnly() ){
        double LL = L->valueNormal();
        if( L1->valueNormal() * 2.0 > LL )
            LL = L1->valueNormal() * 2.0;
        if( L2->valueNormal() * 2.0 > LL )
            LL = L2->valueNormal() * 2.0;
        L0LT->setValueNormal( LL );
    }
}

void SSBeam::setL(DoublePlus *l, DoublePlus *lHor, DoublePlus *lVer, DoublePlus *a) {
    if( !l->readOnly() && lHor->readOnly() && lVer->readOnly() && !a->readOnly() ){
        lHor->setValueNormal( l->valueNormal() * cos(a->valueNormal() ));
        lVer->setValueNormal( l->valueNormal() * sin(a->valueNormal() ));
    }
    if( l->readOnly() && !lHor->readOnly() && lVer->readOnly() && !a->readOnly() ){
        if( cos( a->valueNormal() ) == 0.0 ){
            lVer->setValueNormal( 0.0 );
            l->setValueNormal( lHor->valueNormal() );
        } else {
            lVer->setValueNormal( lHor->valueNormal() * tan( a->valueNormal() ) );
            l->setValueNormal( pow( pow(lHor->valueNormal(),2.0) + pow(lVer->valueNormal(),2.0),0.50));
        }
    }
    if( l->readOnly() && lHor->readOnly() && !lVer->readOnly() && !a->readOnly() ){
        if( sin( a->valueNormal() ) == 0.0 ){
            lHor->setValueNormal( 0.0 );
            l->setValueNormal( lVer->valueNormal() );
        } else {
            lHor->setValueNormal( lHor->valueNormal() / tan( a->valueNormal() ) );
            l->setValueNormal( pow( pow(lHor->valueNormal(),2.0) + pow(lVer->valueNormal(),2.0),0.50));
        }
    }
    if( l->readOnly() && !lHor->readOnly() && !lVer->readOnly() && a->readOnly() ){
        l->setValueNormal( pow( pow(lHor->valueNormal(),2.0) + pow(lVer->valueNormal(),2.0),0.50));
        a->setValueNormal( atan2( lVer->valueNormal(), lHor->valueNormal() ));
    }
    if( !l->readOnly() && !lHor->readOnly() && lVer->readOnly() && a->readOnly() ){
        lVer->setValueNormal( pow( pow(l->valueNormal(),2.0) - pow(lHor->valueNormal(),2.0),0.50));
        a->setValueNormal( atan2( lVer->valueNormal(), lHor->valueNormal() ));
    }
    if( !l->readOnly() && lHor->readOnly() && !lVer->readOnly() && a->readOnly() ){
        lHor->setValueNormal( pow( pow(l->valueNormal(),2.0) - pow(lVer->valueNormal(),2.0),0.50));
        a->setValueNormal( atan2( lVer->valueNormal(), lHor->valueNormal() ));
    }
}

void SSBeam::setL() {
    setL( L, LHor, LVer, alpha );
}

void SSBeam::setL1() {
    setL( L1, LHor1, LVer1, alpha1 );
}

void SSBeam::setL2() {
    setL( L2, LHor2, LVer2, alpha2 );
}

BeamS::BeamS( UnitMeasure * ump, double ss):
    s(new DoublePlus(ss, "s", ump, UnitMeasure::length)),
    N(new DoublePlus(0.0, "N", ump, UnitMeasure::loadF)),
    Vy(new DoublePlus(0.0, "Vy", ump, UnitMeasure::loadF)),
    Vz(new DoublePlus(0.0, "Vz", ump, UnitMeasure::loadF)),
    Mx(new DoublePlus(0.0, "Mx", ump, UnitMeasure::loadM)),
    My(new DoublePlus(0.0, "My", ump, UnitMeasure::loadM)),
    Mz(new DoublePlus(0.0, "Mz", ump, UnitMeasure::loadM)){
}

BeamS::~BeamS(){
    VarPlus::deleteVarPlus( s );
    VarPlus::deleteVarPlus( N );
    VarPlus::deleteVarPlus( Vy );
    VarPlus::deleteVarPlus( Vz );
    VarPlus::deleteVarPlus( Mx );
    VarPlus::deleteVarPlus( My );
    VarPlus::deleteVarPlus( Mz );
}
