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
#include "sectionthinsimple.h"

#include "doubleplus.h"

SectionThinSimple::SectionThinSimple(UnitMeasure * ump, Material * mat, const QString &n, QObject *parent) :
    Section( ump, mat, n, parent) {
    initVar();
}

void SectionThinSimple::initVar() {
    *m_typeNameInternal = "SectionSimple";

    t = new DoublePlus( 0.0, "delta", m_unitMeasure, UnitMeasure::sectL ) ;
    t->setRichName( trUtf8("δ"));
    t->setToolTip( trUtf8("Spessore della sezione"));
    addVarToContainer( t );

    L = new DoublePlus( 0.0, "L", m_unitMeasure, UnitMeasure::sectL );
    L->setRichName( trUtf8("L"));
    L->setToolTip( trUtf8("Perimetro della sezione"));
    addVarToContainer( L );
    L->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionThinSimple::setL) );

    A->setReadOnly( true );
    A->addConnectedVars( 2, t, L );
}

double SectionThinSimple::ANormal() {
    return ( t->valueNormal() * L->valueNormal() );
}

void SectionThinSimple::setL( bool emitAuto ){
    if( L->readOnly() ){
        L->setValueNormal( LNormal(), emitAuto );
    }
}
