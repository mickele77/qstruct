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
#include "multisectionsteelmodel.h"

#include "sectionsteel.h"
#include "materialmodel.h"
#include "sectionloadphase.h"
#include "steel.h"

#include "tablemodelplusprivate.h"
#include "point2dplusmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "intplus.h"
#include "qstringplus.h"

#include <QGraphicsWidget>

MultiSectionSteelModel::MultiSectionSteelModel(UnitMeasure * ump,
                                               MaterialModel * matModel,
                                               QObject * parent ):
    MultiSectionModelBase( "MultiSectionSteelModel", ump, matModel, parent){
    m_materialColumn = 1;
    m_loadPhaseColumn = 5;
    updateHeaders();
}

int MultiSectionSteelModel::materialColumn() {
    return 1;
}

int MultiSectionSteelModel::loadPhaseColumn() {
    return 5;
}

Section * MultiSectionSteelModel::createSection(){
    Steel * steel = dynamic_cast<Steel *>(m_materialModel->firstMaterial( MaterialModel::SteelMaterial ));
    if( steel != NULL ){
        return new SectionSteel( m_d->unitMeasure, steel, SectionSteel::sectionRolled, SectionSteel::sectionI, trUtf8("Acciaio") );
    }
    return NULL;
}

void MultiSectionSteelModel::insertSection( Section * addedSectGen, int position ){
    if (position < 0 || position > m_sectionContainer->size() )
        position = m_sectionContainer->size();

    SectionSteel * addedSect = dynamic_cast<SectionSteel *>(addedSectGen);
    if( addedSect ){
        insertSectionPrivate( addedSect, position );

        insertRowsPrivate( position );

        setVarValue( position, 0, addedSect->name);
        setVarValue( position, 2, addedSect->rotationO );
        setVarValue( position, 3, addedSect->translationO->y );
        setVarValue( position, 4, addedSect->translationO->z );

        connect( addedSect, SIGNAL(sectionChanged()), this, SIGNAL(modelChanged()) );
        connect( addedSect, SIGNAL(materialChanged(Material*,Material*)), this, SLOT(updateMaterial()) );

        if( m_qGraphicsItem != NULL ){
            addedSect->qGraphicsItem()->setParentItem( m_qGraphicsItem );
        }

        emit modelChanged();
    }
}

void MultiSectionSteelModel::updateHeaders(){
    QList<QString> headerList;
    headerList << trUtf8("Nome");
    headerList << trUtf8("Mat.");
    headerList << trUtf8("α [%1]").arg( m_d->unitMeasure->string(UnitMeasure::angle ) );
    headerList << trUtf8("y [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL ) );
    headerList << trUtf8("z [%1]").arg( m_d->unitMeasure->string(UnitMeasure::sectL ) );
    headerList << trUtf8("Fase" );
    setHeaders( headerList );
}

Section * MultiSectionSteelModel::createSection( const QXmlStreamAttributes& attrs, SectionLoadPhaseModel * lModel ){
    return new SectionSteel( m_d->unitMeasure, attrs, m_materialModel, lModel );
}

SectionSteel * MultiSectionSteelModel::sectionSteel( int i ){
    return dynamic_cast<SectionSteel *> (section(i));
}

SectionSteel * MultiSectionSteelModel::sectionSteelId( unsigned int i){
    return dynamic_cast<SectionSteel *> (sectionId(i));
}

double MultiSectionSteelModel::yMaxNormal() {
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    if( iter != m_sectionContainer->end() ){
        SectionSteel * sect = dynamic_cast<SectionSteel *>(*iter);
        while( sect == NULL ){
            sect = dynamic_cast<SectionSteel *>(*iter);
            iter++;
        }
        if( sect != NULL ){
            ret = sect->yMaxNormal();
        }
        if( iter != m_sectionContainer->end() ){
            iter++;
        }
        while( iter != m_sectionContainer->end() ){
            SectionSteel * sect = dynamic_cast<SectionSteel *>(*iter);
            if( sect != NULL ){
                if( sect->yMaxNormal() > ret ){
                    ret = sect->yMaxNormal();
                }
            }
            iter++;
        }
    }
    return ret;
}

double MultiSectionSteelModel::yMinNormal() {
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    if( iter != m_sectionContainer->end() ){
        SectionSteel * sect = dynamic_cast<SectionSteel *>(*iter);
        while( sect == NULL ){
            sect = dynamic_cast<SectionSteel *>(*iter);
            iter++;
        }
        if( sect != NULL ){
            ret = sect->yMinNormal();
        }
        if( iter != m_sectionContainer->end() ){
            iter++;
        }
        while( iter != m_sectionContainer->end() ){
            SectionSteel * sect = dynamic_cast<SectionSteel *>(*iter);
            if( sect != NULL ){
                if( sect->yMinNormal() < ret ){
                    ret = sect->yMinNormal();
                }
            }
            iter++;
        }
    }
    return ret;
}

double MultiSectionSteelModel::zMaxNormal() {
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    if( iter != m_sectionContainer->end() ){
        SectionSteel * sect = dynamic_cast<SectionSteel *>(*iter);
        while( sect == NULL ){
            sect = dynamic_cast<SectionSteel *>(*iter);
            iter++;
        }
        if( sect != NULL ){
            ret = sect->zMaxNormal();
        }
        if( iter != m_sectionContainer->end() ){
            iter++;
        }
        while( iter != m_sectionContainer->end() ){
            SectionSteel * sect = dynamic_cast<SectionSteel *>(*iter);
            if( sect != NULL ){
                if( sect->zMaxNormal() > ret ){
                    ret = sect->zMaxNormal();
                }
            }
            iter++;
        }
    }
    return ret;
}

double MultiSectionSteelModel::zMinNormal() {
    double ret = 0.0;
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    if( iter != m_sectionContainer->end() ){
        SectionSteel * sect = dynamic_cast<SectionSteel *>(*iter);
        while( sect == NULL ){
            sect = dynamic_cast<SectionSteel *>(*iter);
            iter++;
        }
        if( sect != NULL ){
            ret = sect->zMinNormal();
        }
        if( iter != m_sectionContainer->end() ){
            iter++;
        }
        while( iter != m_sectionContainer->end() ){
            SectionSteel * sect = dynamic_cast<SectionSteel *>(*iter);
            if( sect != NULL ){
                if( sect->zMinNormal() < ret ){
                    ret = sect->zMinNormal();
                }
            }
            iter++;
        }
    }
    return ret;
}

bool MultiSectionSteelModel::sMaxNormal( double * ret, double my, double mz ){
    QList<Section *>::iterator iter = m_sectionContainer->begin();
    *ret = 0.0;
    bool sMaxWasSet = false;

    while( iter != m_sectionContainer->end() ){
        SectionSteel * sectSteel = dynamic_cast<SectionSteel *>(*iter);
        if( sectSteel != NULL ){
            if( sMaxWasSet ) {
                double sMaxCmp;
                if( sectSteel->sMaxNormal( &sMaxCmp, my, mz ) ){
                    if( sMaxCmp > *ret ) {
                        *ret = sMaxCmp;
                    }
                }
            } else {
                sMaxWasSet= sectSteel->sMaxNormal(ret, my, mz );
            }
        }
        iter++;
    }

    // finito
    return sMaxWasSet;
}

bool MultiSectionSteelModel::sMinNormal(double *ret,  double my, double mz ){
    *ret = 0.0;
    bool sMinWasSet = false;

    QList<Section *>::iterator iter = m_sectionContainer->begin();
    while( iter != m_sectionContainer->end() ){
        SectionSteel * sectSteel = dynamic_cast<SectionSteel *>(*iter);
        if( sectSteel != NULL ){
            if( sMinWasSet ) {
                double sMinCmp;
                if( sectSteel->sMinNormal( &sMinCmp, my, mz ) ){
                    if( sMinCmp < *ret ) {
                        *ret = sMinCmp;
                    }
                }
            } else {
                sMinWasSet = sectSteel->sMinNormal( ret, my, mz );
            }
        }
        iter++;
    }

    // finito
    return sMinWasSet;
}

double MultiSectionSteelModel::epsYdMax() {
    // il valore che restituiamo
    double ret = 0.0;

    QList<Section *>::iterator iter = m_sectionContainer->begin();

    SectionSteel * currSect = NULL;

    // cerchiamo la prima SectSteel
    while( iter != m_sectionContainer->end() ){
        currSect = dynamic_cast<SectionSteel *>(*iter);
        if( currSect != NULL  ){
            if( currSect->steel() != NULL ){
                if( currSect->steel()->E->valueNormal() != 0.0 ){
                    double epsYd = currSect->steel()->fyd(0)->valueNormal() / currSect->steel()->E->valueNormal();
                    if( epsYd > ret ){
                        ret = epsYd;
                    }
                }
            }
        }
        iter++;
    }

    // finito
    return ret;
}
