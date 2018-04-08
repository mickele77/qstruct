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
#include "multisectionfrpmodel.h"

#include "tablemodelplusprivate.h"
#include "sectionfrp.h"
#include "frp.h"
#include "concrete.h"
#include "materialmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "intplus.h"
#include "qstringplus.h"

#include <QGraphicsWidget>

MultiSectionFRPModel::MultiSectionFRPModel(UnitMeasure * ump, MaterialModel *matModel, QObject * parent):
    MultiSectionModelBase( "MultiSectionFRPModel", ump, matModel, parent ) {
    m_materialColumn = 1;
    m_loadPhaseColumn = 2;
    updateHeaders();
    connect( ump, &UnitMeasure::stringsChanged, this, &MultiSectionFRPModel::updateHeaders );
}

int MultiSectionFRPModel::materialColumn() {
    return 1;
}

int MultiSectionFRPModel::loadPhaseColumn() {
    return 2;
}

Section * MultiSectionFRPModel::createSection(){
    Material * frpMat = m_materialModel->firstMaterial( MaterialModel::FRPMaterial );
    Material * cncrMat = m_materialModel->firstMaterial( MaterialModel::ConcreteMaterial );
    if( frpMat && cncrMat ){
        FRP * frp = dynamic_cast<FRP *>(frpMat);
        Concrete * cncr = dynamic_cast<Concrete *>(cncrMat);
        if( frp ){
            return new SectionFRP( m_d->unitMeasure, frp, cncr, trUtf8("Sezione FRP") );
        }
    }
    return NULL;
}

MultiSectionFRPModel & MultiSectionFRPModel::operator=( MultiSectionFRPModel &val) {
    removeRows( 0, m_sectionContainer->size() );
    insertRows( 0, val.m_sectionContainer->size() );

    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    QList<Section *>::iterator iterVal = val.m_sectionContainer->begin() ;
    while( (iter != m_sectionContainer->end()) && (iterVal != val.m_sectionContainer->end())){
        SectionFRP * frpSect = dynamic_cast<SectionFRP *>(*iter);
        SectionFRP * valFrpSect = dynamic_cast<SectionFRP *>(*iterVal);
        if( (frpSect != NULL) && (valFrpSect != NULL) ) {
            *frpSect = *valFrpSect;
        }
        iter++;
        iterVal++;
    }
    return *this;
}

SectionFRP * MultiSectionFRPModel::sectionFRP( int i ){
    if( i >= 0 && i < m_sectionContainer->size() ){
        return dynamic_cast<SectionFRP *>(m_sectionContainer->at(i));
    } else {
        return NULL;
    }
}

SectionFRP * MultiSectionFRPModel::sectionFRPId( int i ){
    return dynamic_cast<SectionFRP *>(sectionId( i ));
}


void MultiSectionFRPModel::insertSection( Section * addedSectGen, int position ){
    if (position < 0 || position > m_sectionContainer->size() )
        position = m_sectionContainer->size();

    SectionFRP * addedSect = dynamic_cast<SectionFRP *>(addedSectGen);
    if( addedSect ){
        insertSectionPrivate( addedSect, position );

        insertRowsPrivate( position );

        setVar( DisplayValue, position, 0, addedSect->t);
        connect( addedSect, &SectionFRP::sectionChanged, this, &MultiSectionFRPModel::modelChanged );

        if( m_qGraphicsItem != NULL ){
            addedSect->qGraphicsItem()->setParentItem( m_qGraphicsItem );
        }

        emit modelChanged();
    }
}

void MultiSectionFRPModel::updateHeaders(){
    QList<QString> headerList;
    headerList << trUtf8("t [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL)  );
    headerList << trUtf8("FRP");
    headerList << trUtf8("Fase" );
    setHeaders( headerList );
}

double MultiSectionFRPModel::ITNormal(){
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

double MultiSectionFRPModel::yMaxNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionFRP * sect = dynamic_cast<SectionFRP *>( *iter );
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

double MultiSectionFRPModel::yMinNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionFRP * sect = dynamic_cast<SectionFRP *>( *iter );
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

double MultiSectionFRPModel::zMaxNormal(double *corrEpsfd){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool wasMaxSet = false;
    double maxRet = 0.0;
    if( corrEpsfd != NULL ){
        *corrEpsfd = 0;
    }
    while( iter != m_sectionContainer->end() ){
        SectionFRP * sect = dynamic_cast<SectionFRP *>( *iter );
        if( sect != NULL ){
            double z1 = sect->P1->z->valueNormal();
            double z2 = sect->P2->z->valueNormal();
            double max = qMax( z1, z2);
            if( wasMaxSet ){
                if( max > maxRet ){
                    maxRet = max;
                    if( corrEpsfd != NULL ){
                        *corrEpsfd = sect->epsfd->valueNormal();
                    }
                }
            } else {
                maxRet = max;
                if( corrEpsfd != NULL ){
                    *corrEpsfd = sect->epsfd->valueNormal();
                }
                wasMaxSet = true;
            }
        }
        iter++;
    }
    return maxRet;
}

double MultiSectionFRPModel::zMinNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool flag = false;
    double ret = 0.0;
    while( iter != m_sectionContainer->end() ){
        SectionFRP * sect = dynamic_cast<SectionFRP *>( *iter );
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

bool MultiSectionFRPModel::sMaxNormal(double * ret, double my, double mz, double *corrEpsfd){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool wasMaxSet = false;
    if( corrEpsfd != NULL ){
        *corrEpsfd = 0;
    }
    while( iter != m_sectionContainer->end() ){
        SectionFRP * sect = dynamic_cast<SectionFRP *>( *iter );
        if( sect != NULL ){
            double s1 = my * sect->P1->y->valueNormal() + mz * sect->P1->z->valueNormal();
            double s2 = my * sect->P2->y->valueNormal() + mz * sect->P2->z->valueNormal();
            double sMax = qMax( s1, s2);
            if( wasMaxSet ){
                if( sMax > *ret ){
                    *ret = sMax;
                    if( corrEpsfd != NULL ){
                        *corrEpsfd = sect->epsfd->valueNormal();
                    }
                }
            } else {
                *ret = sMax;
                if( corrEpsfd != NULL ){
                    *corrEpsfd = sect->epsfd->valueNormal();
                }
                wasMaxSet = true;
            }
        }
        iter++;
    }
    return wasMaxSet;
}

bool MultiSectionFRPModel::sMinNormal( double *ret, double my, double mz ){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool wasRetSet = false;
    while( iter != m_sectionContainer->end() ){
        SectionFRP * sect = dynamic_cast<SectionFRP *>( *iter );
        if( sect ){
            double s1 = my * sect->P1->y->valueNormal() + mz * sect->P1->z->valueNormal();
            double s2 = my * sect->P2->y->valueNormal() + mz * sect->P2->z->valueNormal();
            double sMin = qMin(s1, s2);
            if( wasRetSet ) {
                if( sMin < *ret ){
                    *ret = sMin;
                }
            } else {
                *ret = sMin;
                wasRetSet = true;
            }
        }
        iter++;
    }
    return wasRetSet;
}

void MultiSectionFRPModel::cut( MultiSectionFRPModel * sectPlus, MultiSectionFRPModel * sectMinus, double l, double my, double mz, double t){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    while( iter != m_sectionContainer->end() ){
        SectionFRP * sect = dynamic_cast<SectionFRP *>( *iter );
        if( sect ){
            double d1 = l + my * sect->P1->y->valueNormal() + mz * sect->P1->z->valueNormal() - t;
            double d2 = l + my * sect->P2->y->valueNormal() + mz * sect->P2->z->valueNormal() - t;
            if( d1 * d2 >= 0.0 ){
                // d1 e d2 concordi o almeno uno dei due nullo
                SectionFRP * sc = NULL;

                // se uno dei due è non nullo
                if( (d1 > 0.0 || d2 > 0.0) && (sectPlus != NULL)){
                    sectPlus->appendRows();
                    sc = dynamic_cast<SectionFRP *>( sectPlus->lastSection() );;
                } else if( (d1 < 0.0 || d2 < 0.0) && (sectMinus != NULL)){
                    sectMinus->appendRows();
                    sc = dynamic_cast<SectionFRP *>( sectMinus->lastSection() );
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
                        SectionFRP * sc = dynamic_cast<SectionFRP *>( sectPlus->lastSection() );
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
                        SectionFRP * sc = dynamic_cast<SectionFRP *>( sectMinus->lastSection() );
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

bool MultiSectionFRPModel::proj( double * tSup, double * tInf, double ny, double nz ){
    QList<Section *>::iterator iter = m_sectionContainer->begin() ;
    bool found = false;

    while( iter != m_sectionContainer->end() ){
        SectionFRP * sect = dynamic_cast<SectionFRP *>( *iter );
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

double MultiSectionFRPModel::epsfdMin() {
    // il valore che restituiamo
    double ret = 0.0;

    QList<Section *>::iterator iter = m_sectionContainer->begin();

    SectionFRP * currSectFRP = NULL;

    // cerchiamo la prima SectFRP
    while( iter != m_sectionContainer->end() ){
        currSectFRP = dynamic_cast<SectionFRP *>(*iter);
        if( currSectFRP ){ // != NULL
            double epsfd = currSectFRP->epsfd->valueNormal();
            if( epsfd > 0.0 ){
                if( ret == 0.0 ){
                    ret = epsfd;
                } else if( epsfd < ret ){
                    ret = epsfd;
                }
            }
        }
        iter++;
    }

    // finito
    return ret;
}
