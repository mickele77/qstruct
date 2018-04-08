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
#include "multisectionsteelcncrmodel.h"

#include "sectionsteelcncr.h"
#include "materialmodel.h"

#include "tablemodelplusprivate.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "convertutility.h"

#include <QGraphicsWidget>

MultiSectionSteelCncrModel::MultiSectionSteelCncrModel( UnitMeasure * ump,
                                                        MaterialModel * matModel,
                                                        QObject * parent):
    MultiSectionModelBase(  "MultiSectionSteelCncrModel", ump, matModel, parent ){
    m_materialColumn = 1;
    m_loadPhaseColumn = 4;
    updateHeaders();
}

int MultiSectionSteelCncrModel::materialColumn() {
    return 1;
}

int MultiSectionSteelCncrModel::loadPhaseColumn() {
    return 4;
}

void MultiSectionSteelCncrModel::insertSection( Section * addedSectGen, int position ){
    if (position < 0 || position > m_sectionContainer->size() )
        position = m_sectionContainer->size();

    SectionSteelCncr * addedSect = dynamic_cast<SectionSteelCncr *>(addedSectGen);
    if( addedSect ){
        insertSectionPrivate( addedSect, position );

        insertRowsPrivate( position );

        setVarValue( position, 0, addedSect->A);
        setVarValue( position, 2, addedSect->P->y);
        setVarValue( position, 3, addedSect->P->z);
        connect( addedSect, &Section::sectionChanged, this, &MultiSectionModelBase::modelChanged );
        connect( addedSect, SIGNAL(materialChanged(Material*,Material*)), this, SLOT(updateMaterial()) );

        if( m_qGraphicsItem != NULL ){
            addedSect->qGraphicsItem()->setParentItem( m_qGraphicsItem );
        }

        emit modelChanged();
    }
}

void MultiSectionSteelCncrModel::updateHeaders(){
    QList<QString> headerList;
    headerList << trUtf8("A [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL2 ) );
    headerList << trUtf8("Materiale");
    headerList << trUtf8("y [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL ) );
    headerList << trUtf8("z [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL ) );
    headerList << trUtf8("Fase");
    setHeaders( headerList );
}

Section * MultiSectionSteelCncrModel::createSection(){
    Material * steelCncrMat = m_materialModel->firstMaterial( MaterialModel::SteelCncrMaterial );
    if( steelCncrMat != NULL ){
        SteelCncr * steelCncr = dynamic_cast<SteelCncr *>(steelCncrMat);
        if( steelCncr ){
            return new SectionSteelCncr( m_d->unitMeasure, steelCncr, trUtf8("Acciaio") );
        }
    }
    return NULL;
}

double MultiSectionSteelCncrModel::yMaxNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    double ret = 0.0;

    // cerchiamo la prima SectSteelCncr
    while( iter != m_sectionContainer->end() ){
        if( dynamic_cast<SectionSteelCncr *>(*iter) ){ // != NULL
            break;
        }
        iter++;
    }

    // prendiamo il valore di x del primo punto della sezione
    if( iter != m_sectionContainer->end() ){
        ret = (dynamic_cast<SectionSteelCncr *>(*iter))->P->y->valueNormal();
        iter++;
    }

    // confrontiamo il valore trovato con le x degli altri punti
    // se sono maggiori, aggiorniamo ret
    while( iter != m_sectionContainer->end() ){
        SectionSteelCncr * sectSCncr = dynamic_cast<SectionSteelCncr *>(*iter);
        if( sectSCncr ){
            if( sectSCncr->P->y->valueNormal() > ret ){
                ret = sectSCncr->P->y->valueNormal();
            }
            iter++;
        }
    }

    // finito
    return ret;
}

double MultiSectionSteelCncrModel::yMinNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    double ret = 0.0;

    // cerchiamo la prima SectSteelCncr
    while( iter != m_sectionContainer->end() ){
        if( dynamic_cast<SectionSteelCncr *>(*iter) ){ // != NULL
            break;
        }
        iter++;
    }

    // prendiamo il valore di x del primo punto della sezione
    if( iter != m_sectionContainer->end() ){
        ret = (dynamic_cast<SectionSteelCncr *>(*iter))->P->y->valueNormal();
        iter++;
    }

    // confrontiamo il valore trovato con le x degli altri punti
    // se sono maggiori, aggiorniamo ret
    while( iter != m_sectionContainer->end() ){
        SectionSteelCncr * sectSCncr = dynamic_cast<SectionSteelCncr *>(*iter);
        if( sectSCncr ){
            if( sectSCncr->P->y->valueNormal() < ret ){
                ret = sectSCncr->P->y->valueNormal();
            }
            iter++;
        }
    }

    // finito
    return ret;
}

double MultiSectionSteelCncrModel::zMaxNormal( double * corrEpsUd ){
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    double ret = 0.0;
    bool wasRetSet = false;

    if( corrEpsUd != NULL ){
        *corrEpsUd = 0.0;
    }

    // cerchiamo la prima SectSteelCncr
    while( iter != m_sectionContainer->end() ){
        SectionSteelCncr * sectSCncr = dynamic_cast<SectionSteelCncr *>(*iter);
        if( sectSCncr ){
            if( wasRetSet ){
                double zCmp = sectSCncr->P->z->valueNormal();
                if( zCmp > ret ){
                    ret = zCmp;
                    if( corrEpsUd != NULL ){
                        if( sectSCncr->steelCncr() ){
                            *corrEpsUd = sectSCncr->steelCncr()->epsUd->valueNormal();
                        }
                    }
                }
            } else {
                ret = sectSCncr->P->z->valueNormal();
                wasRetSet = true;
                if( corrEpsUd != NULL ){
                    if( sectSCncr->steelCncr() ){
                        *corrEpsUd = sectSCncr->steelCncr()->epsUd->valueNormal();
                    }
                }
            }
        }
        iter++;
    }

    // finito
    return ret;
}

double MultiSectionSteelCncrModel::zMinNormal(){
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    double ret = 0.0;
    bool wasRetSet = false;

    while( iter != m_sectionContainer->end() ){
        SectionSteelCncr * sectSCncr = dynamic_cast<SectionSteelCncr *>(*iter);
        if( sectSCncr ){
            if( wasRetSet ){
                double zCmp = sectSCncr->P->z->valueNormal();
                if( zCmp < ret ){
                    ret = zCmp;
                }
            } else {
                ret = sectSCncr->P->z->valueNormal();
                wasRetSet = true;
            }
        }
        iter++;
    }

    // finito
    return ret;
}

bool MultiSectionSteelCncrModel::sMaxNormal( double *ret, double my, double mz, double * corrEpsUd ){
    *ret = 0.0;
    if( corrEpsUd != NULL ){
        *corrEpsUd = 0.0;
    }
    bool wasRetSet = false;

    QList<Section *>::iterator iter = m_sectionContainer->begin();
    while( iter != m_sectionContainer->end() ){
        SectionSteelCncr * sectSteelCncr = dynamic_cast<SectionSteelCncr *>(*iter);
        if( sectSteelCncr != NULL ){
            if( wasRetSet ){
                double sMaxCmp = my * sectSteelCncr->P->y->valueNormal() + mz * sectSteelCncr->P->z->valueNormal();
                if( sMaxCmp > *ret ) {
                    *ret = sMaxCmp;
                    if( sectSteelCncr->steelCncr() != NULL ){
                        if( corrEpsUd != NULL ){
                            *corrEpsUd = sectSteelCncr->steelCncr()->epsUd->valueNormal();
                        }
                    }
                }
            } else {
                *ret = my * sectSteelCncr->P->y->valueNormal() + mz * sectSteelCncr->P->z->valueNormal();
                wasRetSet = true;
                if( sectSteelCncr->steelCncr() != NULL ){
                    if( corrEpsUd != NULL ){
                        *corrEpsUd = sectSteelCncr->steelCncr()->epsUd->valueNormal();
                    }
                }
            }
        }
        iter++;
    }

    // finito
    return wasRetSet;
}

bool MultiSectionSteelCncrModel::sMinNormal( double * ret, double my, double mz  ){
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    *ret = 0.0;
    bool wasRetSet = false;

    while( iter != m_sectionContainer->end() ){
        SectionSteelCncr * sectSCncr = dynamic_cast<SectionSteelCncr *>(*iter);
        if( sectSCncr ){
            if( wasRetSet ){
                double sCmp = my * sectSCncr->P->y->valueNormal() + mz * sectSCncr->P->z->valueNormal();
                if( sCmp < *ret ){
                    *ret = sCmp;
                }
            } else {
                *ret = my * sectSCncr->P->y->valueNormal() + mz * sectSCncr->P->z->valueNormal();
                wasRetSet = true;
            }
        }
        iter++;
    }

    // finito
    return wasRetSet;
}

double MultiSectionSteelCncrModel::epsYdMax(){
    // il valore che restituiamo
    double ret = 0.0;
    bool wasRetSet = false;

    // iteriamo su tutti i valori di epsYd per cercare il maggiore
    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); iter++ ){
        SectionSteelCncr * sectSCncr = dynamic_cast<SectionSteelCncr *>(*iter);
        if( sectSCncr != NULL ){
            if( sectSCncr->steelCncr() != NULL ){
                if( wasRetSet ){
                    double retCmp = sectSCncr->steelCncr()->epsYd->valueNormal();
                    if( retCmp > ret ){
                        ret = retCmp;
                    }
                } else {
                    ret = sectSCncr->steelCncr()->epsYd->valueNormal();
                    wasRetSet = true;
                }
            }
        }
    }

    // finito
    return ret;
}

double MultiSectionSteelCncrModel::epsUdMin(){
    // il valore che restituiamo
    double ret = 0.0;
    bool wasRetSet = false;

    // iteriamo su tutti i valori di epsUd per cercare il minore
    for( QList<Section *>::iterator iter = m_sectionContainer->begin(); iter != m_sectionContainer->end(); iter++ ){
        SectionSteelCncr * sectSCncr = dynamic_cast<SectionSteelCncr *>(*iter);
        if( sectSCncr != NULL ){
            if( sectSCncr->steelCncr() != NULL ){
                if( wasRetSet ){
                    double retCmp = sectSCncr->steelCncr()->epsUd->valueNormal();
                    if( retCmp < ret ){
                        ret = retCmp;
                    }
                } else {
                    ret = sectSCncr->steelCncr()->epsUd->valueNormal();
                    wasRetSet = true;
                }
            }
        }
    }

    // finito
    return ret;
}

SteelCncr::ModelFEps MultiSectionSteelCncrModel::steelCncrModelfEps(){
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    bool wasRetSet = false;

    while( iter != m_sectionContainer->end() ){
        SectionSteelCncr * sectSCncr = dynamic_cast<SectionSteelCncr *>(*iter);
        if( sectSCncr ){
            if( wasRetSet ){
                if( sectSCncr->steelCncr()->modelfEps->valueNormal() == SteelCncr::horizontalTopBranch ) {
                    return SteelCncr::horizontalTopBranch;
                }
            } else {
                if( sectSCncr->steelCncr()->modelfEps->valueNormal() == SteelCncr::horizontalTopBranch ) {
                    return SteelCncr::horizontalTopBranch;
                } else {
                    wasRetSet = true;
                }
            }
        }
        iter++;
    }
    if( wasRetSet ){
        return SteelCncr::inclinedTopBranch;
    }
    return SteelCncr::horizontalTopBranch;
}
