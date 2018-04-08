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
#include "sectionthincont.h"

#include "sectionthinprivate.h"

#include "sectionthincontpointsmodel.h"
#include "multisectionarcmodel.h"
#include "multisectionlinemodel.h"
#include "material.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "doubleplus.h"
#include "boolplus.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <cmath>

#include "sectionthincontprivate.h"

SectionThinCont::SectionThinCont(UnitMeasure * ump, Material * mat, const QString & nn, QObject *parent) :
    SectionThin( ump, mat, nn, parent ),
    m_dd(new SectionThinContPrivate( ump )){
    initVar();
}

SectionThinCont::SectionThinCont(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *matModel, SectionLoadPhaseModel *lModel, QObject *parent):
    SectionThin( ump, NULL, "", parent ),
    m_dd(new SectionThinContPrivate( ump )){
    initVar();
    loadFromXML( attrs, matModel, lModel );
}

SectionThinCont::~SectionThinCont(){
    delete m_dd;
}

SectionThinContPointsModel *SectionThinCont::pointsModel() {
    return m_dd->pointsModel;
}

void SectionThinCont::initVar(){
    *m_typeNameInternal = "SectionThinCont";
    typeName->setValue( trUtf8("Sottile Continua"));

    isClosed = new BoolPlus( false, "isClosed" );
    isClosed->setRichName( trUtf8("Sezione chiusa") );
    isClosed->setToolTip( trUtf8("Il parametro specifica se la sezione sottile è chiusa o meno"));
    addVarToContainer( isClosed );
    connect( isClosed, SIGNAL(valueChanged(QString)), this, SLOT(updateSectionModels()));

    connect( t, &DoublePlus::valueChanged, this, &SectionThinCont::setTNormal );

    connect( t, &DoublePlus::valueChanged, this, &SectionThinCont::updateSectionModels );
    connect( m_dd->pointsModel, &SectionThinContPointsModel::modelChanged, this, &SectionThinCont::updateSectionModels );
}

void SectionThinCont::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement(*m_typeNameInternal);
    for( QMap<QString, VarPlus *>::iterator i = m_varContainer->begin(); i != m_varContainer->end(); ++i) {
        writer->writeAttribute( (*i)->xmlAttribute() );
    }
    if( m_material ){
        writer->writeAttribute( "material", m_material->id->valueNormalStr() );
    }

    m_dd->pointsModel->writeXml( writer );

    writer->writeEndElement();
}

void SectionThinCont::readXml(QXmlStreamReader *reader) {
    QXmlStreamReader::TokenType token = reader->readNext();
    QString n = reader->name().toString().toUpper();
    QString myName = m_typeNameInternal->toUpper();
    while( !(token == QXmlStreamReader::EndElement && n == myName) &&
           !(reader->hasError() ) &&
           (token != QXmlStreamReader::EndDocument) ){
        if( token == QXmlStreamReader::StartElement ){
            if( n == m_dd->pointsModel->modelName().toUpper() ){
               m_dd->pointsModel->readXml(reader);
            }
        }
        token = reader->readNext();
        n = reader->name().toString().toUpper();
    }
    setTNormal();
    updateSectionModels();
}

#include "sectionline.h"
#include "sectionarc.h"
#include "point2dplus.h"

#include <QVector2D>

void SectionThinCont::updateSectionModels(){
    if( m_dd->pointsModel->size() == 0 ){
        m_d->sectionArcModel->clear();
        m_d->sectionLineModel->clear();
    } else if( m_dd->pointsModel->size() == 1 ){
        m_d->sectionArcModel->clear();
        m_d->sectionLineModel->clear();
        m_d->sectionLineModel->insertRows(1);
        *(m_d->sectionLineModel->sectionLine(0)->P1) = *(m_dd->pointsModel->P(0));
        *(m_d->sectionLineModel->sectionLine(0)->P2) = *(m_dd->pointsModel->P(0));
    } else {
        int nLines = m_dd->pointsModel->size() - 1;
        if( isClosed->valueNormal() ){
            nLines += 1;
        }
        int nLinesExst = m_d->sectionLineModel->count();
        if( nLines > nLinesExst ){
            m_d->sectionLineModel->insertRows( nLinesExst, nLines - nLinesExst);
        } else if( nLines < nLinesExst ){
            m_d->sectionLineModel->removeRows( nLinesExst, nLinesExst - nLines);
        }

        int nArcs = m_dd->pointsModel->size() - 2;
        if( isClosed->valueNormal() ){
            nArcs += 2;
        }
        int nArcsExst = m_d->sectionArcModel->count();
        if( nArcs > nArcsExst ){
            m_d->sectionArcModel->insertRows( nArcsExst, nArcs - nArcsExst);
        } else if( nLines < nLinesExst ){
            m_d->sectionArcModel->removeRows( nArcsExst, nArcsExst - nArcs);
        }

        for( int i=0; i < m_dd->pointsModel->size(); ++i){
            if( !(isClosed->valueNormal()) && i == 0 ){
                *(m_d->sectionLineModel->sectionLine(i)->P1) = *(m_dd->pointsModel->P(i));
            } else if( !(isClosed->valueNormal()) && i == (m_dd->pointsModel->size()-1) ){
                *(m_d->sectionLineModel->sectionLine(i-1)->P2) = *(m_dd->pointsModel->P(i));
            } else {
                // ((i > 0) && ( i < (m_dd->pointsModel->size()-1) ) )) && ! isClosed
                // isClosed

                int iPrev = i-1;
                if( iPrev < 0 ){ // && isClosed->valueNormal()
                    iPrev = m_dd->pointsModel->size()-1;
                }

                int iNext = i+1;
                if( iNext >= m_dd->pointsModel->size() ){ // && isClosed->valueNormal()
                    iNext = 0;
                }

                int iArc = i-1;
                if( isClosed->valueNormal() ){
                    iArc = i;
                }

                QVector2D u( m_dd->pointsModel->P(iPrev)->y->valueNormal() - m_dd->pointsModel->P(i)->y->valueNormal(),
                             m_dd->pointsModel->P(iPrev)->z->valueNormal() - m_dd->pointsModel->P(i)->z->valueNormal() );
                u.normalize();
                QVector2D v( m_dd->pointsModel->P(iNext)->y->valueNormal() - m_dd->pointsModel->P(i)->y->valueNormal(),
                             m_dd->pointsModel->P(iNext)->z->valueNormal() - m_dd->pointsModel->P(i)->z->valueNormal() );
                v.normalize();
                double sint = u.x() * v.y() - u.y() * v.x();
                if( (u.length() > 0.0) && (v.length() > 0.0) && (sint != 0.0) ){
                    double r = m_dd->pointsModel->r(i)->valueNormal();
                    double cost = u.x() * v.x() + u.y() * v.y();
                    double tgt2 = (1-cost) / sint;
                    double rp = fabs(r / tgt2);
                    m_d->sectionLineModel->sectionLine(iPrev)->P2->y->setValueNormal( m_dd->pointsModel->P(i)->y->valueNormal() + u.x() * rp );
                    m_d->sectionLineModel->sectionLine(iPrev)->P2->z->setValueNormal( m_dd->pointsModel->P(i)->z->valueNormal() + u.y() * rp );
                    m_d->sectionLineModel->sectionLine(i)->P1->y->setValueNormal( m_dd->pointsModel->P(i)->y->valueNormal() + v.x() * rp );
                    m_d->sectionLineModel->sectionLine(i)->P1->z->setValueNormal( m_dd->pointsModel->P(i)->z->valueNormal() + v.y() * rp );

                    double rc = pow( pow(rp, 2.0) + pow(r, 2.0), 0.5 );
                    QVector2D w = u + v;
                    w.normalize();
                    m_d->sectionArcModel->sectionArc(iArc)->C->y->setValueNormal( m_dd->pointsModel->P(i)->y->valueNormal() + w.x() * rc );
                    m_d->sectionArcModel->sectionArc(iArc)->C->z->setValueNormal( m_dd->pointsModel->P(i)->z->valueNormal() + w.y() * rc );
                    m_d->sectionArcModel->sectionArc(iArc)->R->setValueNormal( r );

                    u = QVector2D( u.x() * rp - w.x() * rc, u.y() * rp - w.y() * rc );
                    v = QVector2D( v.x() * rp - w.x() * rc, v.y() * rp - w.y() * rc );
                    u.normalize();
                    v.normalize();
                    double ai = atan2(u.y(), u.x());
                    m_d->sectionArcModel->sectionArc(iArc)->startAngle->setValueNormal(ai);
                    double Da = atan2(v.y(), v.x()) - ai;
                    while(Da < - M_PI){
                        Da += 2.0 * M_PI;
                    }
                    while(Da > M_PI){
                        Da -= 2.0 * M_PI;
                    }
                    m_d->sectionArcModel->sectionArc(iArc)->sweepAngle->setValueNormal(Da);
                } else {
                    *(m_d->sectionLineModel->sectionLine(iPrev)->P2) = *(m_dd->pointsModel->P(i));
                    *(m_d->sectionArcModel->sectionArc(iArc)->C) = *(m_dd->pointsModel->P(i));
                    m_d->sectionArcModel->sectionArc(iArc)->R->setValueNormal(0.0);
                    m_d->sectionArcModel->sectionArc(iArc)->startAngle->setValueNormal(0.0);
                    m_d->sectionArcModel->sectionArc(iArc)->sweepAngle->setValueNormal(0.0);
                    *(m_d->sectionLineModel->sectionLine(i)->P1) = *(m_dd->pointsModel->P(i));
                }
            }
        }
        /*for( int i=0; i < m_d->sectionLineModel->count(); ++i ){
            qWarning("P1 - %f, %f", m_d->sectionLineModel->sectionLine(i)->P1->y->valueNormal(), m_d->sectionLineModel->sectionLine(i)->P1->z->valueNormal() );
            qWarning("P2 - %f, %f", m_d->sectionLineModel->sectionLine(i)->P2->y->valueNormal(), m_d->sectionLineModel->sectionLine(i)->P2->z->valueNormal() );
        }*/
    }
}

SectionThinCont & SectionThinCont::operator=( SectionThinCont & val ){
    if( this != &val ){
        this->SectionThin::operator=( val );
        *(m_dd->pointsModel) = *(val.m_dd->pointsModel);
    }
    return *this;
}

SectionThin & SectionThinCont::operator=( SectionThin & val ){
    if( this != &val ){
        SectionThinCont * sectionThinCont =dynamic_cast<SectionThinCont *>(&val);
        if( sectionThinCont ){
            this->SectionThinCont::operator=( *sectionThinCont );
            return *this;
        }
        this->SectionThin::operator=( *sectionThinCont );
    }
    return *this;
}

Section & SectionThinCont::operator=( Section & val ){
    if( this != &val ){
        SectionThinCont * sectionThinCont =dynamic_cast<SectionThinCont *>(&val);
        if( sectionThinCont ){
            this->SectionThinCont::operator=( *sectionThinCont );
            return *this;
        }

        SectionThin * sectionThin =dynamic_cast<SectionThin *>(&val);
        if( sectionThin ){
            this->SectionThin::operator=( *sectionThin );
            return *this;
        }

        this->Section::operator=( val );
    }
    return *this;
}

double SectionThinCont::LNormal(){
    // TODO
    return 0.0;
}

double SectionThinCont::ITNormal(){
    // TODO
    return 0.0;
}
