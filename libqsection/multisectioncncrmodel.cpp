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
#include "multisectioncncrmodel.h"

#include "sectioncncr.h"
#include "materialmodel.h"
#include "concrete.h"

#include "tablemodelplusprivate.h"
#include "point2dplusmodel.h"
#include "doubleplus.h"
#include "uintplus.h"
#include "qstringplus.h"

MultiSectionCncrModel::MultiSectionCncrModel(UnitMeasure * ump,
                                             MaterialModel * matModel,
                                             QObject * parent ):
    MultiSectionModelBase( "MultiSectionCncrModel", ump, matModel, parent){
    m_materialColumn = 1;
    m_loadPhaseColumn = 2;
    updateHeaders();
    connect( m_d->unitMeasure, &UnitMeasure::stringsChanged, this, &MultiSectionCncrModel::updateHeaders );
}

void MultiSectionCncrModel::fMinSLSNormal(QList<double> *fMinCncr, double l, double my, double mz) {
    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); iter++ ){
        SectionCncr * sectCncr = dynamic_cast<SectionCncr *>(*iter);
        if( sectCncr ){
            fMinCncr->append( sectCncr->fMinSLSNormal( l, my, mz ) );
        }
    }
}

int MultiSectionCncrModel::materialColumn() {
    return 1;
}

int MultiSectionCncrModel::loadPhaseColumn() {
    return 2;
}

void MultiSectionCncrModel::updateHeaders(){
    QList<QString> headerList;
    headerList << trUtf8("Nome")
               << trUtf8("Materiale")
               << trUtf8("Fase");
    setHeaders( headerList );
}

Section * MultiSectionCncrModel::createSection(){
    Concrete * cncr = dynamic_cast<Concrete *>(m_materialModel->firstMaterial( MaterialModel::ConcreteMaterial ));
    if( cncr != NULL ){
        return new SectionCncr( m_d->unitMeasure, cncr, trUtf8("Cls") );
    }
    return NULL;
}

Section * MultiSectionCncrModel::createSection( const QXmlStreamAttributes& attrs, SectionLoadPhaseModel * lModel ){
    return new SectionCncr( m_d->unitMeasure, attrs, m_materialModel, lModel );
}

SectionCncr * MultiSectionCncrModel::sectionCncr( int i ){
    return dynamic_cast<SectionCncr *> (section(i));
}

SectionCncr * MultiSectionCncrModel::sectionCncrId( unsigned int i){
    return dynamic_cast<SectionCncr *> (sectionId(i));
}

double MultiSectionCncrModel::yMaxNormal(){
    double maxRet = 0.0;
    bool wasMexRetSet = false;

    // cerchiamo la prima SectionCncr
    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); iter++ ){
        SectionCncr * sectCncr = dynamic_cast<SectionCncr *>(*iter);
        if(  sectCncr != NULL ){
            if( sectCncr->pointsModel()->pCount() > 0 ){
                if( wasMexRetSet ){
                    double maxCmp = sectCncr->pointsModel()->yMaxNormal();
                    if( maxCmp > maxRet ){
                        maxRet = maxCmp;
                    }
                } else {
                    maxRet = sectCncr->pointsModel()->yMaxNormal();
                    wasMexRetSet = true;
                }
                break;
            }
        }
    }

    // finito
    return maxRet;
}

double MultiSectionCncrModel::yMinNormal(){
    double minRet = 0.0;
    bool wasMinSet = false;

    // cerchiamo la prima SectSteelCncr
    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); iter++ ){
        SectionCncr * sectCncr = dynamic_cast<SectionCncr *>(*iter);
        if( sectCncr ){
            if( sectCncr->pointsModel()->pCount() > 0 ){
                if( wasMinSet ){
                    double minCmp = sectCncr->pointsModel()->yMinNormal();
                    if( minCmp < minRet ){
                        minRet = minCmp;
                    }
                } else {
                    minRet = sectCncr->pointsModel()->yMinNormal();
                    wasMinSet = true;
                }
            }
        }
    }

    // finito
    return minRet;
}

double MultiSectionCncrModel::zMaxNormal(){
    double maxRet = 0.0;
    bool wasMaxRet = false;

    // cerchiamo la prima SectSteelCncr
    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); iter++ ){        SectionCncr * sectCncr = dynamic_cast<SectionCncr *>(*iter);
        if( sectCncr ){
            if( sectCncr->pointsModel()->pCount() > 0 ){
                if( wasMaxRet) {
                    double maxCmp = sectCncr->pointsModel()->zMaxNormal();
                    if( maxCmp > maxRet ){
                        maxRet = maxCmp;
                    }
                } else {
                    maxRet = sectCncr->pointsModel()->zMaxNormal();
                    wasMaxRet = true;
                }
            }
        }
    }

    // finito
    return maxRet;
}

double MultiSectionCncrModel::zMinNormal( double *corrEpsC, double *corrEpsCU ){
    double zMinRet = 0.0;
    bool zMinWasSet = false;

    if( corrEpsC != NULL ){
        *corrEpsC = 0.0;
    }
    if( corrEpsCU != NULL ){
        *corrEpsCU = 0.0;
    }

    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); iter++ ){        SectionCncr * sectCncr = dynamic_cast<SectionCncr *>(*iter);
        if( sectCncr != NULL ){
            if( sectCncr->pointsModel()->pCount() > 0 ){
                if( zMinWasSet ) {
                    double zMinCmp = sectCncr->pointsModel()->zMinNormal();
                    if( zMinCmp < zMinRet ) {
                        zMinRet = zMinCmp;
                        if( sectCncr->concrete() != NULL ){
                            if( corrEpsC != NULL ){
                                *corrEpsC = sectCncr->concrete()->epsCNormal();
                            }
                            if( corrEpsCU != NULL ){
                                *corrEpsCU = sectCncr->concrete()->epsCUNormal();
                            }
                        }
                    }
                } else {
                    zMinRet = sectCncr->pointsModel()->zMinNormal();
                    if( sectCncr->concrete() != NULL ){
                        if( corrEpsC != NULL ){
                            *corrEpsC = sectCncr->concrete()->epsCNormal();
                        }
                        if( corrEpsCU != NULL ){
                            *corrEpsCU = sectCncr->concrete()->epsCUNormal();
                        }
                    }
                    zMinWasSet = true;
                }
            }
        }
    }

    // finito
    return zMinRet;
}

bool MultiSectionCncrModel::sMaxNormal( double * ret, double my, double mz ){
    *ret = 0.0;
    bool sMaxWasSet = false;

    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); iter++ ){        SectionCncr * sectCncr = dynamic_cast<SectionCncr *>(*iter);
        if( sectCncr != NULL ){
            if( sectCncr->pointsModel()->pCount() > 0 ){
                if( sMaxWasSet ) {
                    double sMaxCmp;
                    if( sectCncr->pointsModel()->sMaxNormal( &sMaxCmp, my, mz ) ){
                        if( sMaxCmp > *ret ) {
                            *ret = sMaxCmp;
                        }
                    }
                } else {
                    sMaxWasSet= sectCncr->pointsModel()->sMaxNormal(ret, my, mz );
                }
            }
        }
    }

    // finito
    return sMaxWasSet;
}

bool MultiSectionCncrModel::sMinNormal(double *ret,  double my, double mz, double *corrEpsC, double *corrEpsCU  ){
    *ret = 0.0;
    if( corrEpsC != NULL ){
        *corrEpsC = 0.0;
    }
    if( corrEpsCU != NULL ){
        *corrEpsCU = 0.0;
    }
    bool sMinWasSet = false;

    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); iter++ ){        SectionCncr * sectCncr = dynamic_cast<SectionCncr *>(*iter);
        if( sectCncr != NULL ){
            if( sectCncr->pointsModel()->pCount() > 0 ){
                if( sMinWasSet ) {
                    double sMinCmp;
                    if( sectCncr->pointsModel()->sMinNormal( &sMinCmp, my, mz ) ){
                        if( sMinCmp < *ret ) {
                            *ret = sMinCmp;
                            if( sectCncr->concrete() != NULL ){
                                if( corrEpsC != NULL ){
                                    *corrEpsC = sectCncr->concrete()->epsCNormal();
                                }
                                if( corrEpsCU != NULL ){
                                    *corrEpsCU = sectCncr->concrete()->epsCUNormal();
                                }
                            }
                        }
                    }
                } else {
                    sMinWasSet = sectCncr->pointsModel()->sMinNormal( ret, my, mz );
                    if( sMinWasSet ){
                        if( sectCncr->concrete() != NULL ){
                            if( corrEpsC != NULL ){
                                *corrEpsC = sectCncr->concrete()->epsCNormal();
                            }
                            if( corrEpsCU != NULL ){
                                *corrEpsCU = sectCncr->concrete()->epsCUNormal();
                            }
                        }
                    }
                }
            }
        }
    }

    // finito
    return sMinWasSet;
}

double MultiSectionCncrModel::epsCMin(){
    double ret = 0.0;
    bool wasRetSet = false;

    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); ++iter ){
        SectionCncr * sectCncr = dynamic_cast<SectionCncr *>(*iter);
        if( sectCncr ){
            if( sectCncr->concrete() ){
                double retCmp = sectCncr->concrete()->epsCNormal();
                if( retCmp > 0.0 ) {
                    if( wasRetSet ){
                        if( retCmp < ret ){
                            ret = retCmp;
                        }
                    } else {
                        ret = retCmp;
                        wasRetSet = true;
                    }
                }
            }
        }
    }
    return ret;
}

double MultiSectionCncrModel::epsCUMin(){
    double ret = 0.0;
    bool wasRetSet = false;

    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); ++iter ){
        SectionCncr * sectCncr = dynamic_cast<SectionCncr *>(*iter);
        if( sectCncr ){
            if( sectCncr->concrete() ){
                double retCmp = sectCncr->concrete()->epsCUNormal();
                if( retCmp > 0.0 ) {
                    if( wasRetSet ){
                        if( retCmp < ret ){
                            ret = retCmp;
                        }
                    } else {
                        ret = retCmp;
                        wasRetSet = true;
                    }
                }
            }
        }
    }
    return ret;
}
