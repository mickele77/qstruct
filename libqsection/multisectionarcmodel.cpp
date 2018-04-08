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
#include "multisectionarcmodel.h"

#include "sectionarc.h"
#include "materialmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "qstringplus.h"

#include "tablemodelplusprivate.h"
#include "structmath.h"

#include <QGraphicsWidget>

#include <cmath>

MultiSectionArcModel::MultiSectionArcModel( UnitMeasure * ump, Material * mat, double t, QObject * parent):
    MultiSectionModelBase( "MultiSectionArcModel", ump, NULL, parent ),
    m_material(mat),
    m_t(t){
    m_materialColumn = -1;
    m_loadPhaseColumn = -1;
    updateHeaders();
    connect( m_d->unitMeasure, &UnitMeasure::stringsChanged, this, &MultiSectionArcModel::updateHeaders );
}

Section * MultiSectionArcModel::createSection(){
    SectionArc * sect = new SectionArc( m_d->unitMeasure, m_material, "sez" );
    sect->t->setValueNormal( m_t );
    sect->t->setReadOnly( true );
    connect( sect->t, &DoublePlus::readOnlyChanged, this, &MultiSectionArcModel::setTNormalSignal );
    if( m_qGraphicsItem ){
        m_qGraphicsItem->addToGroup( sect->qGraphicsItem() );
    }
    return sect;
}

MultiSectionArcModel & MultiSectionArcModel::operator=( MultiSectionArcModel &val) {
    removeRows( 0, m_sectionContainer->size() );
    insertRows( 0, val.m_sectionContainer->size() );

    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    QList<Section *>::iterator iterVal = val.m_sectionContainer->begin() ;
    while( (iter != m_sectionContainer->end()) && (iterVal != val.m_sectionContainer->end())){
        SectionArc * arc = dynamic_cast<SectionArc *>(*iter);
        SectionArc * valArc = dynamic_cast<SectionArc *>(*iterVal);
        if( (arc != NULL) && valArc != NULL ) {
            *arc = *valArc;
        }
        iter++;
        iterVal++;
    }
    return *this;
}

SectionArc * MultiSectionArcModel::sectionArc( int i ){
    if( i >= 0 && i < m_sectionContainer->size() ){
        return dynamic_cast<SectionArc *>(m_sectionContainer->at(i));
    } else {
        return NULL;
    }
}

SectionArc * MultiSectionArcModel::sectionArcId( int i ){
    return dynamic_cast<SectionArc *> (sectionId( i ));
}

void MultiSectionArcModel::insertSection( Section * addedSectGen, int position ){
    if (position < 0 || position > m_sectionContainer->size() )
        position = m_sectionContainer->size();

    SectionArc * addedSect = dynamic_cast<SectionArc *>(addedSectGen);
    if( addedSect ){
        insertSectionPrivate( addedSect, position );

        insertRowsPrivate( position );

        setVar( DisplayValueRO, position, 0, addedSect->t);
        setVar( DisplayValue, position, 1, addedSect->R);
        setVar( DisplayValue, position, 2, addedSect->C->y);
        setVar( DisplayValue, position, 3, addedSect->C->z);
        setVar( DisplayValue, position, 4, addedSect->startAngle);
        setVar( DisplayValue, position, 5, addedSect->sweepAngle);
        connect( addedSect, SIGNAL(sectionChanged()), this, SIGNAL(modelChanged()) );
        connect( addedSect->t, &DoublePlus::readOnlyChanged, this, &MultiSectionArcModel::setTNormalSignal );

        if( m_qGraphicsItem != NULL ){
            addedSect->qGraphicsItem()->setParentItem( m_qGraphicsItem );
        }

        emit modelChanged();
    }
}

void MultiSectionArcModel::updateHeaders(){
    QList<QString> headerList;
    headerList << trUtf8("t [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL) );
    headerList << trUtf8("R [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL) );
    headerList << trUtf8("yC [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL) );
    headerList << trUtf8("zC [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL) );
    headerList << trUtf8("αi [%1]").arg( m_d->unitMeasure->string(UnitMeasure::angle) );
    headerList << trUtf8("Δα [%1]").arg( m_d->unitMeasure->string(UnitMeasure::angle) );
    setHeaders( headerList );
}

Material * MultiSectionArcModel::material(){
    return m_material;
}

void MultiSectionArcModel::setMaterial( Material * mat, bool emitAuto ){
    if( mat != m_material ){
        m_material = mat;
        QList<Section *>::iterator iter = m_sectionContainer->begin() ;
        while( iter != m_sectionContainer->end() ){
            (*iter)->setMaterial( m_material, emitAuto );
            iter++;
        }
        if( emitAuto ){
            emit modelChanged();
        }
    }
}

void MultiSectionArcModel::setTNormal( double val, bool emitAuto ){
    m_t = val;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        SectionThinSimple * sect = dynamic_cast<SectionThinSimple *>( *iter );
        if( sect ){
            if( sect->t->readOnly()  ){
                sect->t->setValueNormal( val, emitAuto );
            }
        }
        iter++;
    }
    if( emitAuto ){
        emit modelChanged();
    }
}

double MultiSectionArcModel::ITNormal(){
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        SectionThinSimple * sect = dynamic_cast<SectionThinSimple *>( *iter );
        if( sect ){
            ret += sect->ITNormal();
        }
        iter++;
    }
    return ret;
}

double MultiSectionArcModel::yMaxNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionArc * sect = dynamic_cast<SectionArc *>( *iter );
        if( sect ){
            double y1 = sect->C->y->valueNormal() + sect->R->valueNormal() * cos( sect->startAngle->valueNormal() );
            double y2 = sect->C->y->valueNormal() + sect->R->valueNormal() * cos( sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal() );
            double max = y1 >= y2 ? y1: y2;
            for( int i = -1; i <= 1; i++ ){
                if( (2.0 * i * M_PI > sect->startAngle->valueNormal()) && (2.0 * i * M_PI < (sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal())) ){
                    double yMax = sect->C->y->valueNormal() + sect->R->valueNormal();
                    if( yMax > max )
                        max = yMax;
                }
            }
            if( !flag ){
                flag = true;
                ret = max;
            } else {
                if( max > ret ){
                    ret = max;
                }
            }
        }
        iter++;
    }
    return ret;
}

double MultiSectionArcModel::yMinNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionArc * sect = dynamic_cast<SectionArc *>( *iter );
        if( sect ){
            double y1 = sect->C->y->valueNormal() + sect->R->valueNormal() * cos( sect->startAngle->valueNormal() );
            double y2 = sect->C->y->valueNormal() + sect->R->valueNormal() * cos( sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal() );
            double min = y1 <= y2 ? y1: y2;
            for( int i = -1; i <= 1; i++ ){
                if( ((2.0 * i * M_PI + M_PI) > sect->startAngle->valueNormal()) && ((2.0 * i * M_PI + M_PI) < (sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal())) ){
                    double yMin = sect->C->y->valueNormal() - sect->R->valueNormal();
                    if( yMin < min )
                        min = yMin;
                }
            }
            if( !flag ){
                flag = true;
                ret = min;
            } else {
                if( min < ret ){
                    ret = min;
                }
            }
        }
        iter++;
    }
    return ret;
}

double MultiSectionArcModel::zMaxNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionArc * sect = dynamic_cast<SectionArc *>( *iter );
        if( sect ){
            double z1 = sect->C->z->valueNormal() + sect->R->valueNormal() * sin( sect->startAngle->valueNormal() );
            double z2 = sect->C->z->valueNormal() + sect->R->valueNormal() * sin( sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal() );
            double max = z1 >= z2 ? z1: z2;
            for( int i = -1; i <= 1; i++ ){
                if( ((2.0 * i * M_PI + M_PI*0.50) > sect->startAngle->valueNormal()) && ((2.0 * i * M_PI + M_PI*0.50) < (sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal())) ){
                    double zMax = sect->C->z->valueNormal() + sect->R->valueNormal();
                    if( zMax > max )
                        max = zMax;
                }
            }
            if( !flag ){
                flag = true;
                ret = max;
            } else {
                if( max > ret ){
                    ret = max;
                }
            }
        }
        iter++;
    }
    return ret;
}

double MultiSectionArcModel::zMinNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionArc * sect = dynamic_cast<SectionArc *>( *iter );
        if( sect ){
            double z1 = sect->C->z->valueNormal() + sect->R->valueNormal() * sin( sect->startAngle->valueNormal() );
            double z2 = sect->C->z->valueNormal() + sect->R->valueNormal() * sin( sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal() );
            double min = z1 <= z2 ? z1: z2;
            for( int i = -1; i <= 1; i++ ){
                if( ((2.0 * i * M_PI + M_PI*1.50) > sect->startAngle->valueNormal()) && ((2.0 * i * M_PI + M_PI*1.50) < (sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal())) ){
                    double zMin = sect->C->z->valueNormal() - sect->R->valueNormal();
                    if( zMin < min )
                        min = zMin;
                }
            }
            if( !flag ){
                flag = true;
                ret = min;
            } else {
                if( min < ret ){
                    ret = min;
                }
            }
        }
        iter++;
    }
    return ret;
}

void MultiSectionArcModel::cut( MultiSectionArcModel * sectPlus, MultiSectionArcModel * sectMinus, double l, double my, double mz, double t){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        SectionArc * sect = dynamic_cast<SectionArc *>( *iter );
        if( sect ){
            double theta[2]; // angoli di intersezione della retta con la circonferenza
            // numero intersezioni della retta con la circonferenza
            int nTheta = solveTrigonometric( theta,
                                             (mz * sect->R->valueNormal()),
                                             (my * sect->R->valueNormal()),
                                             (l-t) + my * sect->C->y->valueNormal() + mz * sect->C->z->valueNormal() );

            // angoli frontiera
            int nPhi = 0;
            double phi[4];

            phi[nPhi] = sect->startAngle->valueNormal();
            nPhi++;
            for( int i=0; (i < nTheta) && (i < 2); i++ ){
                for(int k=-1; k < 2; k++){
                    if( (theta[i]+2.0*k*M_PI) > sect->startAngle->valueNormal() && (theta[i]+2.0*k*M_PI) < (sect->startAngle->valueNormal()+ sect->sweepAngle->valueNormal()) ){
                        if( nPhi < 3){ // per sicurezza
                            phi[ nPhi ] = theta[i] +2.0*k*M_PI;
                            nPhi++;
                        }
                    }
                }
            }
            phi[nPhi] = sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal();
            nPhi++;

            if( nPhi > 3 ){
                // ordiniamo gli angoli phi[1] e phi[2]
                if( phi[1] > phi[2]){
                    double tmp = phi[1];
                    phi[1] = phi[2];
                    phi[2] = tmp;
                }
            }

            for( int i=0; i < (nPhi-1); i++){
                double a = phi[i] + (phi[i+1] - phi[i]) * 0.50;
                double y = sect->C->y->valueNormal() + sect->R->valueNormal() * cos( a );
                double z = sect->C->z->valueNormal() + sect->R->valueNormal() * sin( a );
                double v = l + my * y + mz * z - t;
                SectionArc * sc = NULL;
                if((v > 0.0) && (sectPlus != NULL)){
                    sectPlus->appendRows();
                    sc = dynamic_cast<SectionArc *>( sectPlus->lastSection() );
                } else if((v < 0.0) && (sectMinus != NULL)){
                    sectMinus->appendRows();
                    sc = dynamic_cast<SectionArc *>( sectMinus->lastSection() );
                }
                if( sc != NULL ){
                    sc->t->setValueNormal( sect->t->valueNormal() );
                    sc->C->y->setValueNormal( sect->C->y->valueNormal() );
                    sc->C->z->setValueNormal( sect->C->z->valueNormal() );
                    sc->R->setValueNormal( sect->R->valueNormal() );
                    sc->startAngle->setValueNormal( phi[i] );
                    sc->sweepAngle->setValueNormal( phi[i+1] - phi[i]);
                }
            }
        }
        iter++;
    }
}

bool MultiSectionArcModel::proj(double *tSup, double *tInf, double ny, double nz ){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool found = false;

    while( iter != m_sectionContainer->end() ){
        SectionArc * sect = dynamic_cast<SectionArc *>( *iter );
        if( sect ){
            double t[4];
            int nt = 0;

            // proiezione primo estremo
            double y = sect->C->y->valueNormal() + sect->R->valueNormal() * cos( sect->startAngle->valueNormal() );
            double z = sect->C->z->valueNormal() + sect->R->valueNormal() * sin( sect->startAngle->valueNormal() );
            t[nt++] = ny * y + nz * z;
            // proiezione secondo estremo
            y = sect->C->y->valueNormal() + sect->R->valueNormal() * cos( sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal() );
            z = sect->C->z->valueNormal() + sect->R->valueNormal() * sin( sect->startAngle->valueNormal() + sect->sweepAngle->valueNormal() );
            t[nt++] = ny * y + nz * z;

            // proiezione raggio parallelo a theta
            double ai = normalAngle( sect->startAngle->valueNormal() );
            double af = ai + sect->sweepAngle->valueNormal();
            double theta = normalAngle( atan2( nz, ny ), ai );
            if( ai < theta && theta < af ){
                y = sect->C->y->valueNormal() + sect->R->valueNormal() * cos( theta );
                z = sect->C->z->valueNormal() + sect->R->valueNormal() * sin( theta );
                t[nt++] = ny * y + nz * z;
            }
            theta += M_PI;
            if( ai < theta && theta < af ){
                y = sect->C->y->valueNormal() + sect->R->valueNormal() * cos( theta );
                z = sect->C->z->valueNormal() + sect->R->valueNormal() * sin( theta );
                t[nt++] = ny * y + nz * z;
            }
            int f = 0;
            if( !found ){
                found = true;
                *tSup = t[0];
                *tInf = t[0];
                f = 1;
            }
            for( int i=f; i < nt; i++){
                if( t[i] < *tInf){
                    *tInf = t[i];
                }
                if( t[i] > *tSup ){
                    *tSup = t[i];
                }
            }
        }
        iter++;
    }

    return found;
}
