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
#include "ssbeamtimber.h"

#include "sectiontimber.h"
#include "serviceclass.h"
#include "doubleplus.h"

SSBeamTimber::SSBeamTimber( UnitMeasure * ump, SectionTimber * ss ):
    SSBeam( ump, ss ) {
    initVar();
}

void SSBeamTimber::initVar() {
    lRelLT = new DoublePlus(0.0, "lRelLT", m_unitMeasure, UnitMeasure::noDimension );
    lRelLT->setRichName( trUtf8("Î»<span style=\" vertical-align:sub;\">rel,LT</span>"));
    lRelLT->setToolTip( trUtf8("Snellezza relativa per instabilità  flessotorsionale"));
    lRelLT->setReadOnly( true );

    kCrLT = new DoublePlus(0.0, "kCrLT", m_unitMeasure, UnitMeasure::noDimension );
    kCrLT->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">crit,LT</span>"));
    kCrLT->setToolTip( trUtf8("Fattore per instabilità  flessotorsionale"));
    kCrLT->setReadOnly( true );

    serviceClass = new ServiceClass( ServiceClass::SC2, "serviceClass" );
    serviceClass->setRichName( trUtf8("Classe di servizio"));
    serviceClass->setToolTip( trUtf8("Classe di servizio (EC5 par. 2.3.1.3)"));
    serviceClass->setReadOnly( false );
}

