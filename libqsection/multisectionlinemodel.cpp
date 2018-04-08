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
#include "multisectionlinemodel.h"

#include "tablemodelplusprivate.h"
#include "sectionline.h"
#include "materialmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "qstringplus.h"

#include <QGraphicsWidget>

MultiSectionLineModel::MultiSectionLineModel( UnitMeasure * ump, Material * mat, double t, QObject * parent):
    MultiSectionModelBase( "MultiSectionLineModel", ump, NULL, parent ),
    m_material( mat ),
    m_t(t){
    m_materialColumn = -1;
    m_loadPhaseColumn = -1;
    updateHeaders();
    connect( m_d->unitMeasure, &UnitMeasure::stringsChanged, this, &MultiSectionLineModel::updateHeaders );
}

Section * MultiSectionLineModel::createSection(){
    SectionLine * sect = new SectionLine( m_d->unitMeasure, m_material, "sez" );
    sect->t->setValueNormal( m_t );
    sect->t->setReadOnly( true );
    connect( sect->t, &DoublePlus::readOnlyChanged, this, &MultiSectionLineModel::setTNormalSignal );
    if( m_qGraphicsItem ){
        m_qGraphicsItem->addToGroup( sect->qGraphicsItem() );
    }
    return sect;
}

MultiSectionLineModel & MultiSectionLineModel::operator=( MultiSectionLineModel &val) {
    removeRows( 0, m_sectionContainer->size() );
    insertRows( 0, val.m_sectionContainer->size() );

    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    QList<Section *>::iterator iterVal = val.m_sectionContainer->begin() ;
    while( (iter != m_sectionContainer->end()) && (iterVal != val.m_sectionContainer->end())){
        SectionLine * line = dynamic_cast<SectionLine *>(*iter);
        SectionLine * valLine = dynamic_cast<SectionLine *>(*iterVal);
        if( (line != NULL) && (valLine != NULL) ) {
            *line = *valLine;
        }
        iter++;
        iterVal++;
    }
    return *this;
}

SectionLine * MultiSectionLineModel::sectionLine( int i ){
    if( i >= 0 && i < m_sectionContainer->size() ){
        return dynamic_cast<SectionLine *>(m_sectionContainer->at(i));
    } else {
        return NULL;
    }
}

SectionLine * MultiSectionLineModel::sectionLineId( int i ){
    return dynamic_cast<SectionLine *>(sectionId( i ));
}


void MultiSectionLineModel::insertSection( Section * addedSectGen, int position ){
    if (position < 0 || position > m_sectionContainer->size() )
        position = m_sectionContainer->size();

    SectionLine * addedSect = dynamic_cast<SectionLine *>(addedSectGen);
    if( addedSect ){
        insertSectionPrivate( addedSect, position );

        insertRowsPrivate( position );

        setVar( DisplayValueRO, position, 0, addedSect->t);
        setVar( DisplayValue, position, 1, addedSect->P1->y);
        setVar( DisplayValue, position, 2, addedSect->P1->z);
        setVar( DisplayValue, position, 3, addedSect->P2->y);
        setVar( DisplayValue, position, 4, addedSect->P2->z);
        connect( addedSect, &SectionLine::sectionChanged, this, &MultiSectionLineModel::modelChanged );
        connect( addedSect->t, &DoublePlus::readOnlyChanged, this, &MultiSectionLineModel::setTNormalSignal );

        if( m_qGraphicsItem != NULL ){
            addedSect->qGraphicsItem()->setParentItem( m_qGraphicsItem );
        }

        emit modelChanged();
    }
}

void MultiSectionLineModel::updateHeaders(){
    QList<QString> headerList;
    headerList << trUtf8("t [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL)  );
    headerList << trUtf8("y₁ [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL) );
    headerList << trUtf8("z₁ [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL) );
    headerList << trUtf8("y₂ [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL) );
    headerList << trUtf8("z₂ [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL) );
    setHeaders( headerList );
}

Material * MultiSectionLineModel::material(){
    return m_material;
}

void MultiSectionLineModel::setMaterial( Material * mat, bool emitAuto ){
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

void MultiSectionLineModel::setTNormal( double val, bool emitAuto ){
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

double MultiSectionLineModel::ITNormal(){
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

double MultiSectionLineModel::yMaxNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionLine * sect = dynamic_cast<SectionLine *>( *iter );
        if( sect ){
            double y1 = sect->P1->y->valueNormal();
            double y2 = sect->P2->y->valueNormal();
            double max = y1 >= y2 ? y1: y2;
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

double MultiSectionLineModel::yMinNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionLine * sect = dynamic_cast<SectionLine *>( *iter );
        if( sect ){
            double y1 = sect->P1->y->valueNormal();
            double y2 = sect->P2->y->valueNormal();
            double min = y1 <= y2 ? y1: y2;
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

double MultiSectionLineModel::zMaxNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionLine * sect = dynamic_cast<SectionLine *>( *iter );
        if( sect ){
            double z1 = sect->P1->z->valueNormal();
            double z2 = sect->P2->z->valueNormal();
            double max = z1 >= z2 ? z1: z2;
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

double MultiSectionLineModel::zMinNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionLine * sect = dynamic_cast<SectionLine *>( *iter );
        if( sect ){
            double z1 = sect->P1->z->valueNormal();
            double z2 = sect->P2->z->valueNormal();
            double min = (z1 <= z2) ? z1: z2;
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

void MultiSectionLineModel::cut( MultiSectionLineModel * sectPlus, MultiSectionLineModel * sectMinus, double l, double my, double mz, double t){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        SectionLine * sect = dynamic_cast<SectionLine *>( *iter );
        if( sect ){
            double d1 = l + my * sect->P1->y->valueNormal() + mz * sect->P1->z->valueNormal() - t;
            double d2 = l + my * sect->P2->y->valueNormal() + mz * sect->P2->z->valueNormal() - t;
            if( d1 * d2 >= 0.0 ){
                // d1 e d2 concordi o almeno uno dei due nullo
                SectionLine * sc = NULL;

                // se uno dei due è non nullo
                if( (d1 > 0.0 || d2 > 0.0) && (sectPlus != NULL)){
                    sectPlus->appendRows();
                    sc = dynamic_cast<SectionLine *>( sectPlus->lastSection() );;
                } else if( (d1 < 0.0 || d2 < 0.0) && (sectMinus != NULL)){
                    sectMinus->appendRows();
                    sc = dynamic_cast<SectionLine *>( sectMinus->lastSection() );
                }
                if( sc != NULL ){
                    sc->t->setValueNormal( sect->t->valueNormal() );
                    sc->P1->y->setValueNormal( sect->P1->y->valueNormal() );
                    sc->P1->z->setValueNormal( sect->P1->z->valueNormal() );
                    sc->P2->y->setValueNormal( sect->P2->y->valueNormal() );
                    sc->P2->z->setValueNormal( sect->P2->z->valueNormal() );
                }
            } else {
                // d1 e d2 discordi e diversi da zero
                double Dy = sect->P2->y->valueNormal() - sect->P1->y->valueNormal();
                double Dz = sect->P2->z->valueNormal() - sect->P1->z->valueNormal();
                double a = my * Dy + mz * Dz;
                if( a != 0.0 ){
                    //  equazione lineare del tipo: a * s + b = 0
                    // a = my * Dy + mz * Dz
                    // b = l - t + my * y1 + mz * z1
                    double b = l + my * sect->P1->y->valueNormal() + mz * sect->P1->z->valueNormal() - t;
                    double s = - b / a;
                    double ym = sect->P1->y->valueNormal() + Dy * s;
                    double zm = sect->P1->z->valueNormal() + Dz * s;
                    if( sectPlus != NULL ){
                        sectPlus->appendRows();
                        SectionLine * sc = dynamic_cast<SectionLine *>( sectPlus->lastSection() );
                        sc->t->setValueNormal( sect->t->valueNormal());
                        if( d1 > 0.0 ){
                            sc->P1->y->setValueNormal( sect->P1->y->valueNormal() );
                            sc->P1->z->setValueNormal( sect->P1->z->valueNormal() );
                            sc->P2->y->setValueNormal( ym );
                            sc->P2->z->setValueNormal( zm );
                        }
                        if( d2 > 0.0 ){
                            sc->P1->y->setValueNormal( ym );
                            sc->P1->z->setValueNormal( zm );
                            sc->P2->y->setValueNormal( sect->P2->y->valueNormal() );
                            sc->P2->z->setValueNormal( sect->P2->z->valueNormal() );
                        }
                    }
                    if( sectMinus != NULL ){
                        sectMinus->appendRows();
                        SectionLine * sc = dynamic_cast<SectionLine *>( sectMinus->lastSection() );
                        sc->t->setValueNormal( sect->t->valueNormal());
                        if( d1 < 0.0 ){
                            sc->P1->y->setValueNormal( sect->P1->y->valueNormal() );
                            sc->P1->z->setValueNormal( sect->P1->z->valueNormal() );
                            sc->P2->y->setValueNormal( ym );
                            sc->P2->z->setValueNormal( zm );
                        }
                        if( d2 < 0.0 ){
                            sc->P1->y->setValueNormal( ym );
                            sc->P1->z->setValueNormal( zm );
                            sc->P2->y->setValueNormal( sect->P2->y->valueNormal() );
                            sc->P2->z->setValueNormal( sect->P2->z->valueNormal() );
                        }
                    }
                }
            }
        }
        iter++;
    }
}

bool MultiSectionLineModel::proj( double * tSup, double * tInf, double ny, double nz ){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool found = false;

    while( iter != m_sectionContainer->end() ){
        SectionLine * sect = dynamic_cast<SectionLine *>( *iter );
        if( sect ){
            // calcoliamo le proiezioni e supponiamo che una sia maggiore dell'altra
            double tMin = sect->P1->y->valueNormal() * ny + sect->P1->z->valueNormal() * nz;
            double tMax = sect->P2->y->valueNormal() * ny + sect->P2->z->valueNormal() * nz;
            // verifichiamo se la supposizione sull'ordinamento è esatta
            // altrimenti provvdiamo
            if( tMin > tMax ){
                double tmp = tMin;
                tMin = tMax;
                tMax = tmp;
            }
            if( !found ){
                // se è la prima sezione analizzata, tInf = tMin e tSup = tMax
                found = true;
                *tInf = tMin;
                *tSup = tMax;
            } else {
                // se non è la prima sezione analizzata, confrontiano tInf con tMin e tSup con tMax
                if( tMin < *tInf ){
                    *tInf = tMin;
                }
                if( tMax > *tSup ){
                    *tSup = tMax;
                }
            }
        }
        iter++;
    }

    return found;
}
