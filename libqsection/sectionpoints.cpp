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
#include "sectionpoints.h"

#include "material.h"
#include "boolplus.h"
#include "doubleplus.h"
#include "point2dplusmodel.h"
#include "point2dplus.h"
#include "qstringplus.h"
#include "uintplus.h"
#include "unitmeasure.h"
#include "structmath.h"
#include "point2dmodel.h"

#include <QXmlStreamWriter>
#include <QGraphicsPolygonItem>
#include <QList>

#include <cmath>

SectionPoints::SectionPoints( UnitMeasure * ump, Material *mat, const QString & n, QObject * parent ):
    Section( ump, mat, n, parent ){
    initVar();
}

SectionPoints::SectionPoints( UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent):
    Section( ump, NULL, "", parent ){
    initVar();
    loadFromXML( attrs, mModel, lModel );
}

SectionPoints::~SectionPoints() {
    if( m_qGraphicsItem ){
        delete m_qGraphicsItem;
    }
    Point2DPlusModel::deleteModel( m_pointsModel );
}

SectionPoints & SectionPoints::operator=( SectionPoints & val){
    if( this != &val ){
        this->Section::operator=( ( Section & ) val );

        *m_pointsModel = *(val.m_pointsModel);
    }
    return *this;
}

Section & SectionPoints::operator=( Section & val){
    if( this != &val ){
        SectionPoints * sectPoints = dynamic_cast<SectionPoints *> (&val);
        if( sectPoints ){
            *this = *sectPoints;
        } else {
            this->Section::operator=( val );
        }
    }
    return *this;
}

void SectionPoints::writeXml(QXmlStreamWriter * writer){
    writer->writeStartElement(*m_typeNameInternal);
    for( QMap<QString, VarPlus *>::iterator i = m_varContainer->begin(); i != m_varContainer->end(); ++i) {
        writer->writeAttribute( (*i)->xmlAttribute() );
    }
    if( m_material ){
        writer->writeAttribute( "material", m_material->id->valueNormalStr() );
    }
    m_pointsModel->writeXml( writer );
    writer->writeEndElement();
}

void SectionPoints::readXml(QXmlStreamReader *reader) {
    QXmlStreamReader::TokenType t = reader->tokenType();
    QString n = reader->name().toString().toUpper();
    QString myName = m_typeNameInternal->toUpper();

    // Ci mettiamo sul tag iniziale
    while( !(n == myName && t == QXmlStreamReader::StartElement) &&
           !(reader->hasError()) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }

    m_pointsModel->readXml( reader );

    // Ci mettiamo sul tag finale
    while( !(n == myName && t == QXmlStreamReader::EndElement) &&
           !(reader->hasError()) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void SectionPoints::suspendSignals(bool susp) {
    if( susp ){
        m_pointsModel->suspendSignals( true );
        VarPlusContainer::suspendSignals( true );
    } else {
        VarPlusContainer::suspendSignals( false );
        m_pointsModel->suspendSignals( false );
    }
}

void SectionPoints::translateNormal( double yTrans, double zTrans, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }

    m_pointsModel->translateNormal( yTrans, zTrans );

    if( suspSignals ){
        suspendSignals( false );
    }
}

void SectionPoints::rotateNormal( double rot, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }

    m_pointsModel->rotateNormal( rot );

    if( suspSignals ){
        suspendSignals( false );
    }
}

void SectionPoints::initVar(){
    *m_typeNameInternal = "SectionPoints";
    typeName->setValue( trUtf8("Punti"));

    m_pointsModel = new Point2DPlusModel(m_unitMeasure, UnitMeasure::sectL, QString("y"), QString("z"), this);

    m_qGraphicsItem =  NULL;

    A->setReadOnly( true );
    A->addConnectedTableModel( m_pointsModel );

    Sy->setReadOnly( true );
    Sy->addConnectedTableModel( m_pointsModel );

    Sz->setReadOnly( true );
    Sz->addConnectedTableModel( m_pointsModel );

    Iyy->setReadOnly( true );
    Iyy->addConnectedTableModel( m_pointsModel );

    Izz->setReadOnly( true );
    Izz->addConnectedTableModel( m_pointsModel );

    Iyz->setReadOnly( true );
    Iyz->addConnectedTableModel( m_pointsModel );

    Sy3DyDz->setReadOnly( true );
    Sy3DyDz->addConnectedTableModel( m_pointsModel );

    Sz3DyDz->setReadOnly( true );
    Sz3DyDz->addConnectedTableModel( m_pointsModel );

    Syz2DyDz->setReadOnly( true );
    Syz2DyDz->addConnectedTableModel( m_pointsModel );

    Sy2zDyDz->setReadOnly( true );
    Sy2zDyDz->addConnectedTableModel( m_pointsModel );

    connect( m_pointsModel, &Point2DPlusModel::modelChanged, this, &SectionPoints::updateQGraphics );
    connect( m_pointsModel, &Point2DPlusModel::modelChanged, this, &SectionPoints::sectionChanged );
}

Point2DPlusModel * SectionPoints::pointsModel(){
    return m_pointsModel;
}

QGraphicsItem * SectionPoints::qGraphicsItem(){
    if( m_qGraphicsItem == NULL ){
        m_qGraphicsItem = new QGraphicsPolygonItem;
        updateQGraphics();
    }
    return m_qGraphicsItem;
}

void SectionPoints::updateQGraphics(){
    if( m_qGraphicsItem != NULL ){
        m_qGraphicsItem->setPolygon( m_pointsModel->qPolygonF() );
        emit graphicsChanged();
    }
}

double SectionPoints::ANormal(){
    return m_pointsModel->ANormal();
}

double SectionPoints::SyNormal(){
    double ret = m_pointsModel->SyNormal();
    if( applyRoundOff->valueNormal() )
        ret = MRoundOff( ret, 3.0 );
    return ret;
}

double SectionPoints::SzNormal(){
    double ret = m_pointsModel->SzNormal();
    if( applyRoundOff->valueNormal() )
        ret = MRoundOff( ret, 3.0 );
    return ret;
}

double SectionPoints::IyyNormal(){
    double ret = m_pointsModel->IyyNormal();
    if( applyRoundOff->valueNormal() )
        ret = MRoundOff( ret, 4.0 );
    return ret;
}

double SectionPoints::IzzNormal(){
    double ret = m_pointsModel->IzzNormal();
    if( applyRoundOff->valueNormal() )
        ret = MRoundOff( ret, 4.0 );
    return ret;
}

double SectionPoints::IyzNormal(){
    double ret = m_pointsModel->IyzNormal();
    if( applyRoundOff->valueNormal() )
        ret = MRoundOff( ret, 4.0 );
    return ret;
}

double SectionPoints::Sz3DyDzNormal(){
    double ret = m_pointsModel->Sz3DyDzNormal();
    if( applyRoundOff->valueNormal() )
        ret = MRoundOff( ret, 5.0 );
    return ret;
}

double SectionPoints::Sy3DyDzNormal(){
    double ret = m_pointsModel->Sy3DyDzNormal();
    if( applyRoundOff->valueNormal() )
        ret = MRoundOff( ret, 5.0 );
    return ret;
}

double SectionPoints::Syz2DyDzNormal(){
    double ret = m_pointsModel->Syz2DyDzNormal();
    if( applyRoundOff->valueNormal() )
        ret = MRoundOff( ret, 5.0 );
    return ret;
}

double SectionPoints::Sy2zDyDzNormal(){
    double ret = m_pointsModel->Sy2zDyDzNormal();
    if( applyRoundOff->valueNormal() )
        ret = MRoundOff( ret, 5.0 );
    return ret;
}

void SectionPoints::calculateSects(const double & l,
                                   const double & m,
                                   const double & eps1,
                                   const double & eps2,
                                   Point2DModel * sect1,
                                   Point2DModel * sect2){
    if( m_pointsModel->pCount() > 0 ){
        double z0 = -l/m;
        double z1 = ( eps1 - l )/m;
        double z2 = ( eps2 - l )/m;
        int d1 = where( m_pointsModel->P(0)->z->valueNormal(), z0, z1, z2 );
        int d2;
        if( d1 == 1 ){
            sect1->appendPoint( m_pointsModel->P(0)->y->valueNormal(), m_pointsModel->P(0)->z->valueNormal() );
        } else if( d1 == 2 ){
            sect2->appendPoint( m_pointsModel->P(0)->y->valueNormal(), m_pointsModel->P(0)->z->valueNormal() );
        }
        for( int j = 0; j < m_pointsModel->pCount(); j++){
            int i = j+1;
            if( i == m_pointsModel->pCount() )
                i = 0;
            d2 = where( m_pointsModel->P(i)->z->valueNormal(), z0, z1, z2 );
            if( d1 == 0 ){
                if( d2 == 1 ){
                    double y = pointsModel()->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z0 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect1->appendPoint( y, z0 );
                    sect1->appendPoint( m_pointsModel->P(i)->y->valueNormal(), m_pointsModel->P(i)->z->valueNormal() );
                } else if( d2 == 2 ){
                    double y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z0 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect1->appendPoint( y, z0 );
                    y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z1 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect1->appendPoint( y, z1 );
                    sect2->appendPoint( y, z1 );
                    sect2->appendPoint( m_pointsModel->P(i)->y->valueNormal(), m_pointsModel->P(i)->z->valueNormal() );
                } else if( d2 == 3 ){
                    double y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z0 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect1->appendPoint( y, z0 );
                    y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z1 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect1->appendPoint( y, z1 );
                    sect2->appendPoint( y, z1 );
                    y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z2 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect2->appendPoint( y, z2 );
                }
            } else if( d1 == 1 ){
                if( d2 == 0 ){
                    double y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z0 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect1->appendPoint( y, z0 );
                } else if( d2 == 1 ){
                    sect1->appendPoint( m_pointsModel->P(i)->y->valueNormal(), m_pointsModel->P(i)->z->valueNormal() );
                } else if( d2 == 2 ){
                    double y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z1 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect1->appendPoint( y, z1 );
                    sect2->appendPoint( y, z1 );
                    sect2->appendPoint( m_pointsModel->P(i)->y->valueNormal(), m_pointsModel->P(i)->z->valueNormal() );
                } else if( d2 == 3 ){
                    double y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z1 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect1->appendPoint( y, z1 );
                    sect2->appendPoint( y, z1 );
                    y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z2 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect2->appendPoint( y, z2 );
                }
            } else if( d1 == 2 ){
                if( d2 == 0 ){
                    double y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z1 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect2->appendPoint( y, z1 );
                    sect1->appendPoint( y, z1 );
                    y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z0 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect1->appendPoint( y, z0 );
                } else if( d2 == 1 ){
                    double y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z1 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect2->appendPoint( y, z1 );
                    sect1->appendPoint( y, z1 );
                    sect1->appendPoint( m_pointsModel->P(i)->y->valueNormal(), m_pointsModel->P(i)->z->valueNormal() );
                } else if( d2 == 2 ){
                    sect2->appendPoint( m_pointsModel->P(i)->y->valueNormal(), m_pointsModel->P(i)->z->valueNormal() );
                } else if( d2 == 3 ){
                    double y
                            = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z2 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect2->appendPoint( y, z2 );
                }
            } else if( d1 == 3 ){
                if( d2 == 0 ){
                    double y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z2 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect2->appendPoint( y, z2 );
                    y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z1 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect2->appendPoint( y, z1 );
                    sect1->appendPoint( y, z1 );
                    y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z0 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect1->appendPoint( y, z0 );
                } else if( d2 == 1 ){
                    double y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z2 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect2->appendPoint( y, z2 );
                    y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z1 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect2->appendPoint( y, z1 );
                    sect1->appendPoint( y, z1 );
                    sect1->appendPoint( m_pointsModel->P(i)->y->valueNormal(), m_pointsModel->P(i)->z->valueNormal() );
                } else if( d2 == 2 ){
                    double y = m_pointsModel->P(j)->y->valueNormal() + \
                            (m_pointsModel->P(i)->y->valueNormal() - m_pointsModel->P(j)->y->valueNormal()) * \
                            (z2 - m_pointsModel->P(j)->z->valueNormal()) / (m_pointsModel->P(i)->z->valueNormal() - m_pointsModel->P(j)->z->valueNormal());
                    sect2->appendPoint( y, z2 );
                    sect2->appendPoint( m_pointsModel->P(i)->y->valueNormal(), m_pointsModel->P(i)->z->valueNormal() );
                }
            }
            d1 = d2;
        }
    }
}

int SectionPoints::where( double z, double z0, double z1, double z2){
    if( (z0 < z1) && (z1 < z2) ){
        if( z < z0 )
            return 0;
        else if( z < z1 )
            return 1;
        else if( z <= z2 )
            return 2;
        else
            return 3;
    } else if( (z0 > z1) && (z1 > z2) ){
        if( z < z2 )
            return 3;
        else if( z <= z1 )
            return 2;
        else if( z <= z0 )
            return 1;
        else
            return 0;
    } else {
        return 0;
    }
}
