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
#include "hypothesis.h"

#include <QList>

class HypothesisPrivate{
public:
    HypothesisPrivate( int dim,
                       int dof):
        spaceDim(dim),
        nDOFVert(dof) {
        for( int i=0; i < dof; i++ ){
            unitMeasureEtaContainer.append( UnitMeasure::noDimension );
            unitMeasureFContainer.append( UnitMeasure::noDimension );
        }
    }
    /** Dimensione dello spazio (numero di coordinate di un vertice) */
    int spaceDim;
    /** Gradi di libertà di un vertice */
    int nDOFVert;
    /** Unità di misura dello spostamento associato a ciascun grado di libertà */
    QList<UnitMeasure::unitMeasure> unitMeasureEtaContainer;
    /** Unità di misura della forza associata a ciascun grado di libertà */
    QList<UnitMeasure::unitMeasure> unitMeasureFContainer;
};

Hypothesis::Hypothesis( int dim,
                        int dof ):
    m_d( new HypothesisPrivate(dim, dof ) ){
}

Hypothesis::~Hypothesis(){
    delete m_d;
}

int Hypothesis::spaceDim(){
    return m_d->spaceDim;
}

QString Hypothesis::dimCaption(int v){
    if( v < 0 || v >= m_d->spaceDim ){
        return QString();
    } else {
        QList<QString> listDim;
        if( m_d->spaceDim <= 3 ){
            listDim << "x" << "y" << "z";
        } else {
            for( int i=0; i < m_d->spaceDim ; i++ ){
                listDim << "v" + QString::number(i+1);
            }
        }
        return listDim.at(v);
    }
}

int Hypothesis::nDOFVert(){
    return m_d->nDOFVert;
}

UnitMeasure::unitMeasure Hypothesis::unitMeasureEta(int i ) {
    if( i < 0 || i >= m_d->unitMeasureEtaContainer.size() ){
        return UnitMeasure::noDimension;
    } else {
        return m_d->unitMeasureEtaContainer.at(i);
    }
}

void Hypothesis::setUnitMeasureEta(int i, UnitMeasure::unitMeasure um) {
    if( i >= 0 && i < m_d->unitMeasureEtaContainer.size() ){
        m_d->unitMeasureEtaContainer[i] = um;
    }
}

UnitMeasure::unitMeasure Hypothesis::unitMeasureF(int i ) {
    if( i < 0 || i >= m_d->unitMeasureFContainer.size() ){
        return UnitMeasure::noDimension;
    } else {
        return m_d->unitMeasureFContainer.at(i);
    }
}

void Hypothesis::setUnitMeasureF(int i, UnitMeasure::unitMeasure um) {
    if( i >= 0 && i < m_d->unitMeasureFContainer.size() ){
        m_d->unitMeasureFContainer[i] = um;
    }
}
