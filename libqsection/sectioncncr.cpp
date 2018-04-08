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
#include "sectioncncr.h"

#include "sectionloadphasemodel.h"
#include "concrete.h"
#include "doubleplus.h"
#include "point2dplusmodel.h"
#include "point2dplus.h"
#include "point2dmodel.h"
#include "qstringplus.h"
#include "unitmeasure.h"
#include "structmath.h"

#include <cmath>

#include <QXmlStreamAttributes>
#include <QXmlStreamAttribute>

SectionCncr::SectionCncr( UnitMeasure * um, Concrete * c, const QString & n, QObject * parent ):
    SectionPoints( um, c, n, parent ){
    initVar();
}

SectionCncr::SectionCncr(UnitMeasure * ump, const QXmlStreamAttributes & attrs, MaterialModel * mModel, SectionLoadPhaseModel *lModel, QObject * parent ):
    SectionPoints( ump, NULL, "", parent ){
    initVar();
    loadFromXML( attrs, mModel, lModel );
}

SectionCncr::~SectionCncr(){
}

void SectionCncr::initVar(){
    *m_typeNameInternal = "SectionCncr";
    typeName->setValue( trUtf8("Sezione Cls"));
}

void SectionCncr::loadFromXML( const QXmlStreamAttributes &attrs, MaterialModel * mModel, SectionLoadPhaseModel * loadModel ){
    for ( int i=0; i<attrs.count(); ++i ) {
        for( QMap<QString, VarPlus *>::iterator iter = m_varContainer->begin(); iter != m_varContainer->end(); ++iter ){
            if ( attrs.at(i).name().toString().toUpper() == (*iter)->name().toUpper() ) {
                (*iter)->setValueNormal( attrs.at(i).value().toString() );
            } else if ( attrs.at(i).name().toString().toUpper() == "MATERIAL" ) {
                Concrete * cncr = dynamic_cast<Concrete *>(mModel->materialId( attrs.at(i).value().toString().toUInt() ));
                if( cncr == NULL ){
                    cncr = dynamic_cast<Concrete *>(mModel->firstMaterial( MaterialModel::ConcreteMaterial ) );
                }
                if( cncr != NULL ){
                    setMaterial( cncr );
                }
            } else if ( attrs.at(i).name().toString().toUpper() == "LOADPHASE" ) {
                if( loadModel != NULL ){
                    setLoadPhase( loadModel->loadPhaseId( attrs.at(i).value().toString().toUInt() ), false );
                }
            }
        }
    }
    updateReadOnlyVars();
}

Concrete * SectionCncr::concrete(){
    return dynamic_cast<Concrete *> (m_material);
}

void SectionCncr::setMaterial( Material * mat, bool emitAuto ){
    if( dynamic_cast<Concrete *>(mat) != NULL ){
        Section::setMaterial( mat, emitAuto );
    }
}

void SectionCncr::setConcrete( Concrete * c, bool emitAuto ){
    Section::setMaterial( c, emitAuto );
}

SectionSpace::SectionType SectionCncr::sectionType() {
    return SectionSpace::CncrSection;
}

QString SectionCncr::typeSection(){
    return trUtf8("Sezione in Cls");
}

double SectionCncr::NSLSNormal(double l, double my, double mz, QList<Point2DModel *> *sectsOut) {
    double NRet = 0.0;
    if( concrete() ){
        if( m_pointsModel->pCount() > 2 ){
            if( my == 0.0 && mz == 0.0 ){
                NRet = concrete()->fSLSNormal(l) * A->valueNormal();
            } else {
                Point2DModel * sectMinus = new Point2DModel();

                QList<double> eVal; eVal << 0.0;
                QList<Point2DModel *> sects; sects << sectMinus << NULL;
                m_pointsModel->calculateSects( l, my, mz, eVal, sects );

                NRet += concrete()->E->valueNormal() * ( l * sectMinus->ANormal() + my * sectMinus->SyNormal() + mz * sectMinus->SzNormal() );
                if( sectsOut != NULL ){
                    sectsOut->append( sectMinus );
                } else {
                    delete sectMinus;
                }
            }
        }
    }
    return NRet;
}

void SectionCncr::NMSLSNormal( double *yRet, double * zRet,
                              double l, double my, double mz,
                              QList<Point2DModel *> *sectsOut){
    *yRet = 0.0;
    *zRet = 0.0;
    if( concrete() ){
        if( m_pointsModel->pCount() > 2 ){
            if( my == 0.0 && mz == 0.0 ){
                *yRet = concrete()->fULSNormal(l) * Sy->valueNormal();
                *zRet = - concrete()->fULSNormal(l) * Sz->valueNormal();
            } else {
                Point2DModel * sectMinus = new Point2DModel();

                QList<double> eVal; eVal << 0.0;
                QList<Point2DModel *> sects; sects << sectMinus << NULL;
                m_pointsModel->calculateSects( l, my, mz, eVal, sects );

                *yRet = concrete()->E->valueNormal() * ( l * sectMinus->SyNormal() + my * sectMinus->IyyNormal() + mz * sectMinus->IyzNormal() );
                *zRet -= concrete()->E->valueNormal()* ( l * sectMinus->SzNormal() + my * sectMinus->IyzNormal() + mz * sectMinus->IzzNormal() );

                if( sectsOut ){
                    sectsOut->append( sectMinus );
                } else {
                    delete sectMinus;
                }
            }
        }
    }
}

double SectionCncr::NULSNormal(double l, double my, QList<Point2DModel *> *sects){
    double NRet = 0.0;
    if( concrete() ){
        if( m_pointsModel->pCount() > 2 ){
            if( my == 0.0 ){
                NRet = concrete()->fULSNormal(l) * A->valueNormal();
            } else {
                Point2DModel * sect1 = new Point2DModel();
                Point2DModel * sect2 = new Point2DModel();

                if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionParabolaRectangle ){ // m != 0.0
                    calculateSects( l, my, -concrete()->epsC2->valueNormal(), -concrete()->epsCU2->valueNormal(), sect1, sect2 );
                    double ec2 = concrete()->epsC2->valueNormal();
                    NRet = (concrete()->fcd->valueNormal()/ec2) * ( pow(my,2.0) * sect1->IyyNormal() / ec2 + 2.0 * my * (1.0 + l/ec2) * sect1->SyNormal() + l * (2.0+l/ec2) * sect1->ANormal());
                    NRet -= concrete()->fcd->valueNormal() * sect2->ANormal();
                } else if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionBiLinear ){
                    calculateSects( l, my, -concrete()->epsC3->valueNormal(), -concrete()->epsCU3->valueNormal(), sect1, sect2 );
                    NRet = ( concrete()->fcd->valueNormal()/ concrete()->epsC3->valueNormal()) * (l * sect1->ANormal() + my  * sect1->SyNormal());
                    NRet -= concrete()->fcd->valueNormal() * sect2->ANormal();
                } else if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionStressBlock ){
                    calculateSects( l, my, -concrete()->epsC4->valueNormal(), -concrete()->epsCU3->valueNormal(), sect1, sect2 );
                    NRet -= concrete()->fcd->valueNormal() * sect2->ANormal();
                }
                if( sects ){
                    sects->append( sect1 );
                    sects->append( sect2 );
                } else {
                    delete sect1;
                    delete sect2;
                }
            }
        }
    }
    return NRet;
}

void SectionCncr::MULSNormal( double *yRet,
                              double *zRet,
                              double l,
                              double m,
                              QList<Point2DModel * > * sects){
    *yRet = 0.0;
    *zRet = 0.0;
    if( concrete() ){
        if( m_pointsModel->pCount() > 2 ) {
            if( m == 0.0 ){
                *yRet = concrete()->fULSNormal( l ) * Sz->valueNormal();
                *zRet = - (concrete()->fULSNormal(l) * Sy->valueNormal());
            } else {
                Point2DModel * sect1 = new Point2DModel();
                Point2DModel * sect2 = new Point2DModel();

                if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionParabolaRectangle ){ // m != 0.0
                    calculateSects( l, m, -concrete()->epsC2->valueNormal(), -concrete()->epsCU2->valueNormal(), sect1, sect2 );
                    double ec = concrete()->epsC2->valueNormal();
                    double yParab = (concrete()->fcd->valueNormal()/ec) * ( pow(m,2.0) * sect1->Sz3DyDzNormal() / ec + 2.0 * m * (1.0 + l/ec) * sect1->IyyNormal() + l * (2.0+l/ec) * sect1->SyNormal() );
                    double zParab = (concrete()->fcd->valueNormal()/ec) * ( pow(m,2.0) * sect1->Syz2DyDzNormal() / ec + 2.0 * m * (1.0 + l/ec) * sect1->IyzNormal() + l * (2.0+l/ec) * sect1->SzNormal() );
                    double yConst = - concrete()->fcd->valueNormal() * sect2->SyNormal();
                    double zConst = - concrete()->fcd->valueNormal() * sect2->SzNormal();
                    *yRet = yParab + yConst;
                    *zRet = - (zParab + zConst);
                } else if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionBiLinear ){
                    calculateSects( l, m, -concrete()->epsC3->valueNormal(), -concrete()->epsCU3->valueNormal(), sect1, sect2 );
                    double ec = concrete()->epsC3->valueNormal();
                    double yLin = ( concrete()->fcd->valueNormal() / ec ) * (l * sect1->SyNormal() + m  * sect1->IyyNormal());
                    double zLin = ( concrete()->fcd->valueNormal() / ec ) * (l * sect1->SzNormal() + m  * sect1->IyzNormal());
                    double yConst = - concrete()->fcd->valueNormal() * sect2->SyNormal();
                    double zConst = - concrete()->fcd->valueNormal() * sect2->SzNormal();
                    *yRet = yLin + yConst;
                    *zRet = - (zLin + zConst);
                } else if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionStressBlock ){
                    calculateSects( l, m, -concrete()->epsC4->valueNormal(), -concrete()->epsCU2->valueNormal(), sect1, sect2 );
                    *yRet = - concrete()->fcd->valueNormal() * sect2->SyNormal();
                    *zRet = -( - concrete()->fcd->valueNormal() * sect2->SzNormal());
                }
                if( sects ){
                    sects->append( sect1 );
                    sects->append( sect2 );
                } else {
                    delete sect1;
                    delete sect2;
                }
            }
        }
    }
}

double SectionCncr::NULSNormal(double l, double my, double mz, QList<Point2DModel *> *sects){
    double NRet = 0.0;
    if( concrete() ){
        if( m_pointsModel->pCount() > 2 ){
            if( my == 0.0 && mz == 0.0 ){
                NRet = concrete()->fULSNormal(l) * A->valueNormal();
            } else {
                Point2DModel * sect1 = new Point2DModel();
                Point2DModel * sect2 = new Point2DModel();

                if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionParabolaRectangle ){ // m != 0.0
                    double fcd = concrete()->fcd->valueNormal();
                    double ec2 = concrete()->epsC2->valueNormal();

                    QList<double> eVal; eVal << -concrete()->epsCU2->valueNormal() << -ec2 << 0.0;
                    QList<Point2DModel *> sects; sects << NULL << sect2 << sect1 << NULL;
                    m_pointsModel->calculateSects( l, my, mz, eVal, sects );

                    if( ec2 != 0.0 ){
                        NRet += fcd / (ec2 * ec2) * \
                                (l*l*sect1->ANormal() + my * my * sect1->IyyNormal() + mz * mz * sect1->IzzNormal() +\
                                 2.0 * l * my * sect1->SyNormal() + 2.0 * l * mz * sect1->SzNormal() + 2.0 * my * mz * sect1->IyzNormal() ) + \
                                2.0 * fcd / ec2 * (l * sect1->ANormal() + my * sect1->SyNormal() + mz * sect1->SzNormal() ) ;
                    }
                    NRet += - fcd * sect2->ANormal();
                } else if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionBiLinear ){
                    double fcd = concrete()->fcd->valueNormal();
                    double ec3 = concrete()->epsC2->valueNormal();

                    QList<double> eVal; eVal << -concrete()->epsCU3->valueNormal() << -ec3 << 0.0;
                    QList<Point2DModel *> sects; sects << NULL << sect2 << sect1 << NULL;
                    m_pointsModel->calculateSects( l, my, mz, eVal, sects );

                    if( ec3 != 0.0 ){
                        NRet += fcd / ec3 * ( l * sect1->ANormal() + my * sect1->SyNormal() + mz * sect1->SzNormal() );
                    }
                    NRet += - fcd * sect2->ANormal();
                } else if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionStressBlock ){
                    QList<double> eVal; eVal << -concrete()->epsCU3->valueNormal() << -concrete()->epsC4->valueNormal();
                    QList<Point2DModel *> sects; sects << NULL << sect2 << NULL;
                    m_pointsModel->calculateSects( l, my, mz, eVal, sects );

                    NRet = - concrete()->fcd->valueNormal() * sect2->ANormal();
                }
                if( sects ){
                    sects->append( sect1 );
                    sects->append( sect2 );
                } else {
                    delete sect1;
                    delete sect2;
                }
            }
        }
    }
    return NRet;
}

void SectionCncr::MULSNormal( double *yRet, double * zRet,
                              double l, double my, double mz,
                              QList<Point2DModel *> *sects){
    *yRet = 0.0;
    *zRet = 0.0;
    if( concrete() ){
        if( m_pointsModel->pCount() > 2 ){
            if( my == 0.0 && mz == 0.0 ){
                *yRet = concrete()->fULSNormal(l) * Sy->valueNormal();
                *zRet = - concrete()->fULSNormal(l) * Sz->valueNormal();
            } else {
                Point2DModel * sect1 = new Point2DModel();
                Point2DModel * sect2 = new Point2DModel();

                if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionParabolaRectangle ){ // m != 0.0
                    double ec2 = concrete()->epsC2->valueNormal();
                    double fcd = concrete()->fcd->valueNormal();

                    QList<double> eVal; eVal << -concrete()->epsCU2->valueNormal() << -ec2 << 0.0;
                    QList<Point2DModel *> sects; sects << NULL << sect2 << sect1 << NULL;
                    m_pointsModel->calculateSects( l, my, mz, eVal, sects );

                    if( ec2 != 0.0 ){
                        *yRet = fcd / (ec2 * ec2) * \
                                (l*l*sect1->SyNormal() + my * my * sect1->Sz3DyDzNormal() + mz * mz * sect1->Sy2zDyDzNormal() +\
                                 2.0 * l * my * sect1->IyyNormal() + 2.0 * l * mz * sect1->IyzNormal() + 2.0 * my * mz * sect1->Syz2DyDzNormal() ) + \
                                2.0 * fcd / ec2 * (l * sect1->SyNormal() + my * sect1->IyyNormal() + mz * sect1->IyzNormal() ) ;
                        *zRet -= fcd / (ec2 * ec2) * \
                                (l*l*sect1->SzNormal() + my * my * sect1->Syz2DyDzNormal() + mz * mz * sect1->Sy3DyDzNormal() +\
                                 2.0 * l * my * sect1->IyzNormal() + 2.0 * l * mz * sect1->IzzNormal() + 2.0 * my * mz * sect1->Sy2zDyDzNormal() ) + \
                                2.0 * fcd / ec2 * (l * sect1->SzNormal() + my * sect1->IyzNormal() + mz * sect1->IzzNormal() ) ;
                    }
                    *yRet += - fcd * sect2->SyNormal();
                    *zRet -= - fcd * sect2->SzNormal();
                } else if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionBiLinear ){
                    double ec3 = concrete()->epsC2->valueNormal();
                    double fcd = concrete()->fcd->valueNormal();

                    QList<double> eVal; eVal << -concrete()->epsCU3->valueNormal() << -ec3 << 0.0;
                    QList<Point2DModel *> sects; sects << NULL << sect2 << sect1 << NULL;
                    m_pointsModel->calculateSects( l, my, mz, eVal, sects );

                    if( ec3 != 0.0 ){
                        *yRet = fcd / ec3 * ( l * sect1->SyNormal() + my * sect1->IyyNormal() + mz * sect1->IyzNormal() );
                        *zRet -= fcd / ec3 * ( l * sect1->SzNormal() + my * sect1->IyzNormal() + mz * sect1->IzzNormal() );
                    }
                    *yRet += - fcd * sect2->SyNormal();
                    *zRet -= - fcd * sect2->SzNormal();
                } else if( concrete()->modelfEpsSection->valueNormal() == Concrete::SectionStressBlock ){
                    QList<double> eVal; eVal << -concrete()->epsCU3->valueNormal() << -concrete()->epsC4->valueNormal();
                    QList<Point2DModel *> sects; sects << NULL << sect2 << NULL;
                    m_pointsModel->calculateSects( l, my, mz, eVal, sects );

                    *yRet = - concrete()->fcd->valueNormal() * sect2->SyNormal();
                    *zRet = 0.0;
                    *zRet -= - concrete()->fcd->valueNormal() * sect2->SzNormal();
                }
                if( sects ){
                    sects->append( sect1 );
                    sects->append( sect2 );
                } else {
                    delete sect1;
                    delete sect2;
                }
            }
        }
    }
}

void SectionCncr::setRectangular( DoublePlus * b, DoublePlus * h ){
    setRectangularNormal( b->valueNormal(), h->valueNormal());
}

void SectionCncr::setRectangularNormal( double b, double h){
    suspendSignals( true );

    m_pointsModel->clear();
    m_pointsModel->insertRows(0,4);

    m_pointsModel->P(0)->y->setValueNormal( b/2.0 );
    m_pointsModel->P(0)->z->setValueNormal( h/2.0 );

    m_pointsModel->P(1)->y->setValueNormal( -b/2.0 );
    m_pointsModel->P(1)->z->setValueNormal( h/2.0 );

    m_pointsModel->P(2)->y->setValueNormal( -b/2.0 );
    m_pointsModel->P(2)->z->setValueNormal( -h/2.0 );

    m_pointsModel->P(3)->y->setValueNormal( b/2.0 );
    m_pointsModel->P(3)->z->setValueNormal( -h/2.0 );

    suspendSignals( false );
}
