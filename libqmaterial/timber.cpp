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

#include "timber.h"

#include "qstringplus.h"
#include "doubleplus.h"
#include "boolplus.h"

#include <QString>
#include <QXmlAttributes>
#include <QMap>
#include <QList>
#include <QClipboard>
#include <QGuiApplication>
#include <cmath>

class TimberPrivate {
public:
    TimberPrivate() {
        initNTC08Values();
    };
    void initNTC08Values(){
        if( kModNTC08 == 0 ){
            kModNTC08 = new QList<QList<QList<double> *> *>;

            kModNTC08->append( new QList<QList<double> *>); // Timber::solid
            kModNTC08->at( Timber::solid )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::solid )->at( ServiceClass::SC1 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::solid )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::solid )->at( ServiceClass::SC2 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::solid )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::solid )->at( ServiceClass::SC3 )) << 0.50 << 0.55 << 0.65 << 0.70 << 0.90;

            kModNTC08->append( new QList<QList<double> *>); // Timber::glulam
            kModNTC08->at( Timber::glulam )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::glulam )->at( ServiceClass::SC1 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::glulam )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::glulam )->at( ServiceClass::SC2 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::glulam )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::glulam )->at( ServiceClass::SC3 )) << 0.50 << 0.55 << 0.65 << 0.70 << 0.90;

            kModNTC08->append( new QList<QList<double> *>); // Timber::LVL
            kModNTC08->at( Timber::LVL )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::LVL )->at( ServiceClass::SC1 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::LVL )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::LVL )->at( ServiceClass::SC2 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::LVL )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::LVL )->at( ServiceClass::SC3 )) << 0.50 << 0.55 << 0.65 << 0.70 << 0.90;

            kModNTC08->append( new QList<QList<double> *>); // Timber::plywood_1
            kModNTC08->at( Timber::plywood_1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::plywood_1 )->at( ServiceClass::SC1 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::plywood_1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::plywood_1 )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
            kModNTC08->at( Timber::plywood_1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::plywood_1 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::plywood_2
            kModNTC08->at( Timber::plywood_2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::plywood_2 )->at( ServiceClass::SC1 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::plywood_2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::plywood_2 )->at( ServiceClass::SC2 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::plywood_2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::plywood_2 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::plywood_3
            kModNTC08->at( Timber::plywood_3 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::plywood_3 )->at( ServiceClass::SC1 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::plywood_3 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::plywood_3 )->at( ServiceClass::SC2 )) << 0.60 << 0.70 << 0.80 << 0.90 << 1.00;
            kModNTC08->at( Timber::plywood_3 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::plywood_3 )->at( ServiceClass::SC3 )) << 0.50 << 0.55 << 0.65 << 0.70 << 0.90;

            kModNTC08->append( new QList<QList<double> *>); // Timber::OSB_2
            kModNTC08->at( Timber::OSB_2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::OSB_2 )->at( ServiceClass::SC1 )) << 0.30 << 0.45 << 0.65 << 0.85 << 1.00;
            kModNTC08->at( Timber::OSB_2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::OSB_2 )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
            kModNTC08->at( Timber::OSB_2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::OSB_2 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::OSB_3
            kModNTC08->at( Timber::OSB_3 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::OSB_3 )->at( ServiceClass::SC1 )) << 0.40 << 0.50 << 0.70 << 0.90 << 1.00;
            kModNTC08->at( Timber::OSB_3 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::OSB_3 )->at( ServiceClass::SC2 )) << 0.30 << 0.40 << 0.55 << 0.70 << 0.90;
            kModNTC08->at( Timber::OSB_3 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::OSB_3 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::OSB_4
            kModNTC08->at( Timber::OSB_4 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::OSB_4 )->at( ServiceClass::SC1 )) << 0.40 << 0.50 << 0.70 << 0.90 << 1.00;
            kModNTC08->at( Timber::OSB_4 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::OSB_4 )->at( ServiceClass::SC2 )) << 0.30 << 0.40 << 0.55 << 0.70 << 0.90;
            kModNTC08->at( Timber::OSB_4 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::OSB_4 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::particle_4
            kModNTC08->at( Timber::particle_4 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_4 )->at( ServiceClass::SC1 )) << 0.30 << 0.45 << 0.65 << 0.85 << 1.00;
            kModNTC08->at( Timber::particle_4 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_4 )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
            kModNTC08->at( Timber::particle_4 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_4 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::particle_5
            kModNTC08->at( Timber::particle_5 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_5 )->at( ServiceClass::SC1 )) << 0.30 << 0.45 << 0.65 << 0.85 << 1.00;
            kModNTC08->at( Timber::particle_5 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_5 )->at( ServiceClass::SC2 )) << 0.20 << 0.30 << 0.45 << 0.60 << 0.80;
            kModNTC08->at( Timber::particle_5 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_5 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::particle_6
            kModNTC08->at( Timber::particle_6 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_6 )->at( ServiceClass::SC1 )) << 0.40 << 0.50 << 0.70 << 0.90 << 1.00;
            kModNTC08->at( Timber::particle_6 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_6 )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
            kModNTC08->at( Timber::particle_6 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_6 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::particle_7
            kModNTC08->at( Timber::particle_7 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_7 )->at( ServiceClass::SC1 )) << 0.40 << 0.50 << 0.70 << 0.90 << 1.00;
            kModNTC08->at( Timber::particle_7 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_7 )->at( ServiceClass::SC2 )) << 0.30 << 0.40 << 0.55 << 0.70 << 0.90;
            kModNTC08->at( Timber::particle_7 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::particle_7 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::HBLA
            kModNTC08->at( Timber::HBLA )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::HBLA )->at( ServiceClass::SC1 )) << 0.30 << 0.45 << 0.65 << 0.85 << 1.00;
            kModNTC08->at( Timber::HBLA )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::HBLA )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
            kModNTC08->at( Timber::HBLA )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::HBLA )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::HBLA1
            kModNTC08->at( Timber::HBLA1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::HBLA1 )->at( ServiceClass::SC1 )) << 0.30 << 0.45 << 0.65 << 0.85 << 1.00;
            kModNTC08->at( Timber::HBLA1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::HBLA1 )->at( ServiceClass::SC2 )) << 0.20 << 0.30 << 0.45 << 0.60 << 0.80;
            kModNTC08->at( Timber::HBLA1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::HBLA1 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::HBLA2
            kModNTC08->at( Timber::HBLA2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::HBLA2 )->at( ServiceClass::SC1 )) << 0.30 << 0.45 << 0.65 << 0.85 << 1.00;
            kModNTC08->at( Timber::HBLA2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::HBLA2 )->at( ServiceClass::SC2 )) << 0.20 << 0.30 << 0.45 << 0.60 << 0.80;
            kModNTC08->at( Timber::HBLA2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::HBLA2 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::MBHLA1
            kModNTC08->at( Timber::MBHLA1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHLA1 )->at( ServiceClass::SC1 )) << 0.20 << 0.40 << 0.60 << 0.80 << 1.00;
            kModNTC08->at( Timber::MBHLA1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHLA1 )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
            kModNTC08->at( Timber::MBHLA1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHLA1 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::MBHLA2
            kModNTC08->at( Timber::MBHLA2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHLA2 )->at( ServiceClass::SC1 )) << 0.20 << 0.40 << 0.60 << 0.80 << 1.00;
            kModNTC08->at( Timber::MBHLA2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHLA2 )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
            kModNTC08->at( Timber::MBHLA2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHLA2 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::MBHHLS1
            kModNTC08->at( Timber::MBHHLS1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHHLS1 )->at( ServiceClass::SC1 )) << 0.20 << 0.40 << 0.60 << 0.80 << 1.00;
            kModNTC08->at( Timber::MBHHLS1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHHLS1 )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.45 << 0.80;
            kModNTC08->at( Timber::MBHHLS1 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHHLS1 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::MBHHLS2
            kModNTC08->at( Timber::MBHHLS2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHHLS2 )->at( ServiceClass::SC1 )) << 0.20 << 0.40 << 0.60 << 0.80 << 1.00;
            kModNTC08->at( Timber::MBHHLS2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHHLS2 )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.45 << 0.80;
            kModNTC08->at( Timber::MBHHLS2 )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MBHHLS2 )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::MDFLA
            kModNTC08->at( Timber::MDFLA )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MDFLA )->at( ServiceClass::SC1 )) << 0.20 << 0.40 << 0.60 << 0.80 << 1.00;
            kModNTC08->at( Timber::MDFLA )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MDFLA )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
            kModNTC08->at( Timber::MDFLA )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MDFLA )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

            kModNTC08->append( new QList<QList<double> *>); // Timber::MDFHLS
            kModNTC08->at( Timber::MDFHLS )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MDFHLS )->at( ServiceClass::SC1 )) << 0.20 << 0.40 << 0.60 << 0.80 << 1.00;
            kModNTC08->at( Timber::MDFHLS )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MDFHLS )->at( ServiceClass::SC2 )) << 0.0 << 0.0 << 0.0 << 0.45 << 0.80;
            kModNTC08->at( Timber::MDFHLS )->append( new QList<double>); // ServiceClass::SC1
            * (kModNTC08->at( Timber::MDFHLS )->at( ServiceClass::SC3 )) << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
        }
        if( gammaMNTC08 == 0 ) {
            gammaMNTC08 = new QList<QList<double> *>;
            gammaMNTC08->append( new QList<double> );
            * (gammaMNTC08->at( LSType::ULSFundamental )) << 1.50 << 1.45 << 1.45 <<            // solid, glulam, LVL
                                                             1.40 << 1.40 << 1.40 <<            // plywood_1, plywood_2, plywood_3
                                                             1.40 << 1.40 << 1.40 <<            // OSB_2, OSB_3, OSB_4
                                                             1.50 << 1.50 << 1.50 << 1.50 <<    // particle_4, particle_5, particle_6, particle_7
                                                             1.50 << 1.50 << 1.50 <<            // HBLA, HBLA1, HBLA2
                                                             1.50 << 1.50 << 1.50 << 1.50 <<    // MBHLA1, MBHLA2, MBHHLS1, MBHHLS2
                                                             1.50 << 1.50;                      // MDFLA, MDFHLS
            gammaMNTC08->append( new QList<double> );
            * (gammaMNTC08->at( LSType::ULSExceptional )) << 1.0 << 1.0 << 1.0 <<            // solid, glulam, LVL
                                                             1.0 << 1.0 << 1.0 <<            // plywood_1, plywood_2, plywood_3
                                                             1.0 << 1.0 << 1.0 <<            // OSB_2, OSB_3, OSB_4
                                                             1.0 << 1.0 << 1.0 << 1.0 <<     // particle_4, particle_5, particle_6, particle_7
                                                             1.0 << 1.0 << 1.0 <<            // HBLA, HBLA1, HBLA2
                                                             1.0 << 1.0 << 1.0 << 1.0 <<     // MBHLA1, MBHLA2, MBHHLS1, MBHHLS2
                                                             1.0 << 1.0;                     // MDFLA, MDFHLS
            gammaMNTC08->append( new QList<double> );
            * (gammaMNTC08->at( LSType::ELS )) << 1.0 << 1.0 << 1.0 <<            // solid, glulam, LVL
                                                  1.0 << 1.0 << 1.0 <<            // plywood_1, plywood_2, plywood_3
                                                  1.0 << 1.0 << 1.0 <<            // OSB_2, OSB_3, OSB_4
                                                  1.0 << 1.0 << 1.0 << 1.0 <<     // particle_4, particle_5, particle_6, particle_7
                                                  1.0 << 1.0 << 1.0 <<            // HBLA, HBLA1, HBLA2
                                                  1.0 << 1.0 << 1.0 << 1.0 <<     // MBHLA1, MBHLA2, MBHHLS1, MBHHLS2
                                                  1.0 << 1.0;                     // MDFLA, MDFHLS
        }
    };
    /** Coefficienti gammaM, sono pari al numero di stati limite contenuti in LSType */
    QList<DoublePlus *> * gammaM;
    /** Coefficienti kmod, sono pari al numero di classi di servizio x il numero di calssi di carico */
    QList<QList <DoublePlus *> *> * kmod;


    /** Coefficienti kmod da NTC 08 */
    static QList<QList<QList<double> *> *> * kModNTC08;
    /** Coefficienti gammaM da NTC 08 */
    static QList<QList<double> *> * gammaMNTC08;

};

QList<QList<QList<double> *> *> * TimberPrivate::kModNTC08 = 0;
QList<QList<double> *> * TimberPrivate::gammaMNTC08 = 0;

Timber::Timber( UnitMeasure * ump, const QString &n, QObject * parent ):
    Material( ump, n, parent ),
    m_d( new TimberPrivate() ){
    initVar();

    name->setValue("GL24h");
    timberType->setValueNormal(Timber::glulam);
    fmk->setValueNormal( 2.40e+7);
    ft0k->setValueNormal( 1.65e+7);
    ft90k->setValueNormal( 0.40e+6);
    fc0k->setValueNormal(2.40e+7);
    fc90k->setValueNormal( 2.7e+6 );
    fvk->setValueNormal( 2.7e+6);
    E0m->setValueNormal( 1.16e+10 );
    E0k->setValueNormal( 9.4e+9 );
    E90m->setValueNormal( 3.90e+8 );
    Gm->setValueNormal( 7.20e+8 );
    gammaWk->setValueNormal( 3.8e+3 );
    gammaW->setValueNormal( 3.8e+3 );
}

Timber::Timber( UnitMeasure * ump, const QXmlStreamAttributes & attrs, QObject * parent ):
    Material(ump, "", parent),
    m_d(new TimberPrivate()){
    initVar();
    loadFromXML( attrs );
}

MaterialModel::MaterialType Timber::materialType() {
    return MaterialModel::TimberMaterial;
}

void Timber::initVar(){
    *m_typeNameInternal = "Timber";
    typeName->setValue( trUtf8("Legno"));

    gammaWk = new DoublePlus( 0.0, "gammaWk", m_unitMeasure, UnitMeasure::loadFVolume );
    gammaWk->setRichName( trUtf8("γ<span style=\" vertical-align:sub;\">W,k</span>") );
    gammaWk->setToolTip( trUtf8("Peso specifico caratteristico"));
    addVarToContainer( gammaWk );

    gammaW->setRichName( trUtf8("γ<span style=\" vertical-align:sub;\">W,m</span>") );
    gammaW->setToolTip( trUtf8("Peso specifico medio"));

    fmk = new DoublePlus( 0.0, "fmk", m_unitMeasure, UnitMeasure::tension );
    fmk->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">m,k</span>") );
    addVarToContainer( fmk );

    ft0k = new DoublePlus( 0.0, "ft0k", m_unitMeasure, UnitMeasure::tension );
    ft0k->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">t,0,k</span>") );
    addVarToContainer( ft0k );

    ft90k = new DoublePlus( 0.0, "ft90k", m_unitMeasure, UnitMeasure::tension );
    ft90k->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">t,90,k</span>") );
    addVarToContainer( ft90k );

    fc0k = new DoublePlus( 0.0, "fc0k", m_unitMeasure, UnitMeasure::tension );
    fc0k->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">c,0,k</span>") );
    addVarToContainer( fc0k );

    fc90k = new DoublePlus( 0.0, "fc90k", m_unitMeasure, UnitMeasure::tension );
    fc90k->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">c,90,k</span>") );
    addVarToContainer( fc90k );

    fvk = new DoublePlus( 0.0, "fvk", m_unitMeasure, UnitMeasure::tension );
    fvk->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">v,k</span>") );
    addVarToContainer( fvk );

    E0m = E;
    E0m->setName( "E0m" );
    E0m->setRichName( trUtf8("E<span style=\" vertical-align:sub;\">0,m</span>") );

    E0k = new DoublePlus( 0.0, "E0k", m_unitMeasure, UnitMeasure::tension );
    E0k->setRichName( trUtf8("E<span style=\" vertical-align:sub;\">0,k</span>") );
    addVarToContainer( E0k );

    E90m = new DoublePlus( 0.0, "E90m", m_unitMeasure, UnitMeasure::tension );
    E90m->setRichName( trUtf8("E<span style=\" vertical-align:sub;\">90,m</span>") );
    addVarToContainer( E90m );

    Gm = G;
    Gm->setName("Gm");
    Gm->setRichName( trUtf8("G<span style=\" vertical-align:sub;\">m</span>") );

    num = new DoublePlus( 0.0, "num", m_unitMeasure, UnitMeasure::noDimension );
    num->setReadOnly( true );
    num->setRichName( trUtf8("G<span style=\" vertical-align:sub;\">m</span>") );
    addVarToContainer( num );
    setNum();
    connect( E0m, SIGNAL(valueChanged(QString)), this, SLOT(setNum()) );
    connect( Gm, SIGNAL(valueChanged(QString)), this, SLOT(setNum()) );

    timberType = new TimberTypePlus( Timber::solid, "timberType" );
    timberType->setRichName("Tipologia legno");
    addVarToContainer( timberType );

    woodEssence = new WoodEssencePlus( Timber::softwoods, "woodEssence" );
    woodEssence->setRichName("Essenza legnosa");
    addVarToContainer( woodEssence );

    m_d->kmod = new QList<QList<DoublePlus *> *>;
    for(int i=0; i < 3; i++ ){
        m_d->kmod->append( new QList<DoublePlus *> );
        for(int j=0; j < 5; j++ ){
            QString name = "kmod_" + QString::number(i) + "_" + QString::number(j);
            m_d->kmod->at( i )->append( new DoublePlus(1.0, name, m_unitMeasure, UnitMeasure::noDimension, true ) );
            m_d->kmod->at( i )->at(j)->setRichName("k<span style=\" vertical-align:sub;\">mod</span>");
            m_d->kmod->at( i )->at(j)->setReadOnly();
            addVarToContainer( m_d->kmod->at( i )->at(j) );
            connect( m_d->kmod->at( i )->at(j), SIGNAL(readOnlyChanged(bool)), this, SLOT(setKmod()) );
        }
    }
    setKmod( false );

    m_d->gammaM = new QList<DoublePlus *>;
    for(int i=0; i < 3; i++ ){
        QString name = "gammaM_" + QString::number(i);
        m_d->gammaM->append( new DoublePlus(1.0, name, m_unitMeasure, UnitMeasure::noDimension, true ) );
        m_d->gammaM->at( i )->setRichName(trUtf8("γ<span style=\" vertical-align:sub;\">M</span>"));
        m_d->gammaM->at( i )->setReadOnly();
        addVarToContainer( m_d->gammaM->at( i ) );
        connect( m_d->gammaM->at( i ), SIGNAL(readOnlyChanged(bool)), this, SLOT(setGammaM()) );
    }
    setGammaM( false );
}

void Timber::setKmod(bool emitAuto){
    DoublePlus * var = dynamic_cast<DoublePlus *>( sender() );
    if(var){
        if( var->readOnly() ){
            for(int i=0; i < 3; i++ ){
                for( int j=0; j < 5; j++ ){
                    if( var == m_d->kmod->at( i )->at(j) ) {
                        m_d->kmod->at( i )->at(j)->setValueNormal( m_d->kModNTC08->at(timberType->valueNormal() )->at(i)->at(j), emitAuto );
                    }
                }
            }
        }
    } else {
        for(int i=0; i < 3; i++ ){
            for( int j=0; j < 5; j++ ){
                if( m_d->kmod->at( i )->at(j)->readOnly() ) {
                    m_d->kmod->at( i )->at(j)->setValueNormal( m_d->kModNTC08->at(timberType->valueNormal() )->at(i)->at(j), emitAuto );
                }
            }
        }
    }
}

void Timber::setKmod(){
    setKmod( true );
}

void Timber::setGammaM( bool emitAuto ){
    DoublePlus * var = dynamic_cast<DoublePlus *>( sender() );
    if( var ){
        if( var->readOnly() ){
            for(int i=0; i < 3; i++ ){
                if( var == m_d->gammaM->at( i ) ){
                    m_d->gammaM->at( i )->setValueNormal( m_d->gammaMNTC08->at( i )->at( timberType->valueNormal() ), emitAuto );
                }
            }
        }
    } else {
        for(int i=0; i < 3; i++ ){
            if( m_d->gammaM->at( i )->readOnly() ){
                m_d->gammaM->at( i )->setValueNormal( m_d->gammaMNTC08->at( i )->at( timberType->valueNormal() ), emitAuto );
            }
        }
    }
}

void Timber::setGammaM(){
    setGammaM( true );
}

void Timber::setNum( bool emitAuto ){
    if( num->readOnly() ){
        num->setValueNormal( E0m->valueNormal() / (2.0 * Gm->valueNormal() ) - 1.0, emitAuto );
    }
}

void Timber::setNum(){
    setNum( true );
}

double Timber::gammaMNormal( LSType::Type lsType ){
    if( lsType < m_d->gammaM->size() )
        return m_d->gammaM->at( (int) lsType )->valueNormal();
    else if( m_d->gammaM->size() > 0 )
        return m_d->gammaM->at( 0 )->valueNormal();
    else
        return 1.0;
}

DoublePlus * Timber::gammaM( LSType * lsType ){
    if( lsType->valueNormal() < m_d->gammaM->size() )
        return m_d->gammaM->at( (int) lsType->valueNormal() );
    else if( m_d->gammaM->size() > 0 )
        return m_d->gammaM->at( 0 );
    else
        return NULL;
}

double Timber::kmodNormal( ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    if( m_d->kmod->size() == 0)
        return 0;
    if( m_d->kmod->at(0)->size() == 0)
        return 0;
    int sc = 0;
    if( sClass < m_d->kmod->size() ){
        sc = sClass;
    }
    int ldc = 0;
    if( sc < m_d->kmod->size() ){
        if( ldClass < m_d->kmod->at(sc)->size() ){
            ldc = ldClass;
        }
    }
    if( sc < m_d->kmod->size() ){
        if( ldc < m_d->kmod->at(sc)->size() ){
            return m_d->kmod->at( sc )->at( ldc )->valueNormal();
        }
    }
    return 0.0;
}

DoublePlus * Timber::kmod( ServiceClass* sClass, LoadDurationClass * ldClass ){
    if( m_d->kmod->size() == 0)
        return 0;
    if( m_d->kmod->at(0)->size() == 0)
        return 0;
    ServiceClass::Class sc = ServiceClass::SC1;
    if( sClass->value() < m_d->kmod->size() ){
        sc = sClass->valueNormal();
    }
    LoadDurationClass::Class ldc = LoadDurationClass::permanent;
    if( ldClass->value() < m_d->kmod->at(sc)->size() ){
        ldc = ldClass->valueNormal();
    }
    return m_d->kmod->at( sc )->at( ldc );
}

double Timber::khNormal( double h ){
    if( h > 0 ){
        if( timberType->valueNormal() == Timber::solid ) {
            if( h < 0.150 ){
                double ret = pow(0.150/h, 0.20);
                if( ret > 1.30 ){
                    return 1.30;
                } else {
                    return ret;
                }
            }
        } else if( timberType->valueNormal() == Timber::glulam ) {
            if( h < 0.600 ){
                double ret = pow(0.600/h, 0.20);
                if( ret > 1.15 ){
                    return 1.15;
                } else {
                    return ret;
                }
            }
        }
    }
    return 1.0;
}

double Timber::kVNormal( double V ){
    if( V > 0.0 ){
        return pow(0.01 / V, 0.20);
    }
    return 1.0;
}

double Timber::kshNormal( SectionTimber::Shape sType, double b, double h ){
    if( sType == SectionTimber::circular ){
        return 1.20;
    } else if( sType == SectionTimber::rectangular ){
        if( h != 0.0 ){
            return (1.0 + 0.50 * b / h);
        }
    }
    return 1.0;
}

double Timber::kmNormal( SectionTimber::Shape sType ){
    if( sType == SectionTimber::rectangular ){
        return 0.70;
    }
    return 1.0;
}

double Timber::lEffNormal ( double h, double l, double l1Conf, double l2Conf ){
    if( l1Conf > 0.0 && l2Conf > 0.0 ){
        double ret[3];
        ret[0] = l + h / 3.0;
        ret[1] = l + l1Conf + l2Conf;
        ret[2] = 2.0 * l;
        double min = ret[0];
        for(int i = 1; i < 3; i++ ){
            if( min > ret[i])
                min = ret[i];
        }
        return min;
    } else if( l1Conf > 0.0 ){
        double ret[3];
        ret[0] = l + h / 6.0;
        ret[1] = l + l1Conf;
        ret[2] = 1.50 * l;
        double min = ret[0];
        for(int i = 1; i < 3; i++ ){
            if( min > ret[i])
                min = ret[i];
        }
        return min;
    } else if( l2Conf > 0.0 ){
        double ret[3];
        ret[0] = l + h / 6.0;
        ret[1] = l + l2Conf;
        ret[2] = 1.50 * l;
        double min = ret[0];
        for(int i = 1; i < 3; i++ ){
            if( min > ret[i])
                min = ret[i];
        }
        return min;
    }
    return l;
}

double Timber::ft0dNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass, double khVal ){
    return khVal * kmodNormal(sClass, ldClass) * ft0k->valueNormal() / gammaMNormal(lsType);
}


double Timber::ft90dNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass, double kVVal ){
    return kVVal * kmodNormal(sClass, ldClass) * ft90k->valueNormal() / gammaMNormal(lsType);
}

double Timber::fc0dNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    return kmodNormal(sClass, ldClass) * fc0k->valueNormal() / gammaMNormal(lsType);
}

double Timber::fc90dNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass){
    return kmodNormal(sClass, ldClass) * fc90k->valueNormal() / gammaMNormal(lsType);
}

double Timber::fcdNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass, double angle){
    return (fc0dNormal(lsType,sClass,ldClass) / \
            ( (fc0dNormal(lsType,sClass,ldClass) / fc90dNormal(lsType,sClass,ldClass) ) * pow(sin(angle),2.0)+ pow(cos(angle),2.0)));
}

double Timber::fmdNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass, double khVal ){
    return khVal * kmodNormal(sClass, ldClass) * fmk->valueNormal() / gammaMNormal(lsType);
}


double Timber::fvdNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    return kmodNormal(sClass, ldClass) * fvk->valueNormal() / gammaMNormal(lsType);
}

double Timber::ftorsdNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass, double kshVal){
    return kshVal * kmodNormal(sClass, ldClass) * fvk->valueNormal() / gammaMNormal(lsType);
}

void Timber::kh( DoublePlus * ret, DoublePlus * h ){
    ret->setValueNormal( khNormal( h->valueNormal() ));
}

void Timber::kV( DoublePlus * ret, DoublePlus * V ){
    ret->setValueNormal( kVNormal(V->valueNormal()));
}

void Timber::ksh( DoublePlus * ret, SectionTimber::Shape sType, DoublePlus * b, DoublePlus * h ){
    ret->setValueNormal( kshNormal(sType, b->valueNormal(), h->valueNormal()));
}

void Timber::km( DoublePlus * ret, SectionTimber::Shape sType ){
    ret->setValueNormal( kmNormal(sType ));
}

void Timber::lEff ( DoublePlus * ret, DoublePlus * h, DoublePlus * l, DoublePlus * lsc1, DoublePlus * lsc2 ){
    ret->setValueNormal( lEffNormal(h->valueNormal(), l->valueNormal(), lsc1->valueNormal(), lsc2->valueNormal()));
}

void Timber::ft0d( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass, DoublePlus * khVal ){
    ret->setValueNormal(ft0dNormal(lsType->valueNormal(),sClass->valueNormal(),ldClass->valueNormal(),khVal->valueNormal()));
}

void Timber::ft90d( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass, DoublePlus * kVVal ){
    ret->setValueNormal( ft90dNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal(), kVVal->valueNormal()));
}

void Timber::fc0d( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( fc0dNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal()));
}

void Timber::fc90d( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass){
    ret->setValueNormal( fc90dNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal()));
}

void Timber::fcd( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass, DoublePlus * angle){
    ret->setValueNormal( fcdNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal(), angle->valueNormal()));
}

void Timber::fmd( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass, DoublePlus * khVal ){
    ret->setValueNormal(fmdNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal(), khVal->valueNormal()));
}


void Timber::fvd( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( fvdNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal()));
}

void Timber::ftorsd( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass, DoublePlus * kshVal ){
    ret->setValueNormal( ftorsdNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal(), kshVal->valueNormal() ));
}

void Timber::copyfkToClipBoard(){
    QClipboard *cb = QGuiApplication::clipboard();
    QString text;
    text = fmk->name() + QString(" = ") + fmk->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::tension ) + QString("\n");
    text += ft0k->name() + QString(" = ") + ft0k->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::tension ) + QString("\n");
    text += ft90k->name() + QString(" = ") + ft90k->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::tension ) + QString("\n");
    text += fc0k->name() + QString(" = ") + fc0k->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::tension ) + QString("\n");
    text += fc90k->name() + QString(" = ") + fc90k->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::tension ) + QString("\n");
    text += fvk->name() + QString(" = ") + fvk->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::tension ) + QString("\n");
    cb->setText(text);
}

void Timber::copyEToClipBoard(){
    QClipboard *cb = QGuiApplication::clipboard();
    QString text;
    text = E0k->name() + QString(" = ") + E0k->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::tension ) + QString("\n");
    text += E0m->name() + QString(" = ") + E0m->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::tension ) + QString("\n");
    text += E90m->name() + QString(" = ") + E90m->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::tension ) + QString("\n");
    text += Gm->name() + QString(" = ") + Gm->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::tension ) + QString("\n");
    text += num->name() + QString(" = ") + num->valueStr() + QString(" ") + m_unitMeasure->string( UnitMeasure::noDimension ) + QString("\n");
    cb->setText(text);
}

class TimberTypeEnumVal{
public:
    TimberTypeEnumVal(Timber::TimberType tt, const QString & ns, const QString & s):
        val(tt),
        normalStr(ns),
        str(s){
    };
    Timber::TimberType val;
    QString normalStr;
    QString str;
};

class TimberTypePlusPrivate{
    Q_DECLARE_TR_FUNCTIONS(TimberTypePlusPrivate)
public:
    TimberTypePlusPrivate(Timber::TimberType tt):
        value(tt){
        enumList.append( TimberTypeEnumVal( Timber::solid, trUtf8("massiccio"), trUtf8("legno massiccio") ) );
        enumList.append( TimberTypeEnumVal( Timber::glulam, trUtf8("lamellare"), trUtf8("legno lamellare incollato") ) );
        enumList.append( TimberTypeEnumVal( Timber::LVL, trUtf8("microlamellare"), trUtf8("microlamellare") ) );
        enumList.append( TimberTypeEnumVal( Timber::plywood_1, trUtf8("compensato EN 636-1"), trUtf8("compensato - EN 636 parte 1") ) );
        enumList.append( TimberTypeEnumVal( Timber::plywood_2, trUtf8("compensato EN 636-2"), trUtf8("compensato - EN 636 parte 2") ) );
        enumList.append( TimberTypeEnumVal( Timber::plywood_3, trUtf8("compensato EN 636-3"), trUtf8("compensato - EN 636 parte 3") ) );
        enumList.append( TimberTypeEnumVal( Timber::OSB_2, trUtf8("OSB/2"), trUtf8("scaglie orientate - EN 300 OSB/2") ) );
        enumList.append( TimberTypeEnumVal( Timber::OSB_3, trUtf8("OSB/3"), trUtf8("scaglie orientate - EN 300 OSB/3") ) );
        enumList.append( TimberTypeEnumVal( Timber::OSB_4, trUtf8("OSB/4"), trUtf8("scaglie orientate - EN 300 OSB/4") ) );
        enumList.append( TimberTypeEnumVal( Timber::particle_4, trUtf8("truciolare EN 312-4"), trUtf8("particelle (truciolare) - EN 312 parte 4") ) );
        enumList.append( TimberTypeEnumVal( Timber::particle_5, trUtf8("truciolare EN 312-5"), trUtf8("particelle (truciolare) - EN 312 parte 5") ) );
        enumList.append( TimberTypeEnumVal( Timber::particle_6, trUtf8("truciolare EN 312-6"), trUtf8("particelle (truciolare) - EN 312 parte 6") ) );
        enumList.append( TimberTypeEnumVal( Timber::particle_7, trUtf8("truciolare EN 312-7"), trUtf8("particelle (truciolare) - EN 312 parte 7") ) );
        enumList.append( TimberTypeEnumVal( Timber::HBLA, trUtf8("HB.LA"), trUtf8("fibre alta densita' - EN 622-2 HB.LA") ) );
        enumList.append( TimberTypeEnumVal( Timber::HBLA1, trUtf8("HB.LA1"), trUtf8("fibre alta densita' - EN 622-2 HB.LA1") ) );
        enumList.append( TimberTypeEnumVal( Timber::HBLA2, trUtf8("HB.LA2"), trUtf8("fibre alta densita' - EN 622-2 HB.LA2") ) );
        enumList.append( TimberTypeEnumVal( Timber::MBHLA1, trUtf8("MBH.LA1"), trUtf8("fibre media densita' - EN 622-3 MBH.LA1") ) );
        enumList.append( TimberTypeEnumVal( Timber::MBHLA2, trUtf8("MBH.LA2"), trUtf8("fibre media densita' - EN 622-3 MBH.LA2") ) );
        enumList.append( TimberTypeEnumVal( Timber::MBHHLS1, trUtf8("MBH.HLS1"), trUtf8("fibre media densita' - EN 622-3 MBH.HLS1") ) );
        enumList.append( TimberTypeEnumVal( Timber::MBHHLS2, trUtf8("MBH.HLS2"), trUtf8("fibre media densita' - EN 622-3 MBH.HLS2") ) );
        enumList.append( TimberTypeEnumVal( Timber::MDFLA, trUtf8("MDF.LA"), trUtf8("fibre - EN 622-5 MDF.LA") ) );
        enumList.append( TimberTypeEnumVal( Timber::MDFHLS, trUtf8("MDF.HLS"), trUtf8("fibre - EN 622-5 MDF.HLS") ) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    Timber::TimberType value;
    QList<TimberTypeEnumVal> enumList;
};

TimberTypePlus::TimberTypePlus( Timber::TimberType tt,
                                const QString & nn,
                                bool ro):
    EnumPlus( nn, ro),
    m_d( new TimberTypePlusPrivate(tt) ){
}

TimberTypePlus::~TimberTypePlus(){
    delete m_d;
}

QString TimberTypePlus::typeVar(){
    return QString("TimberType");
}

TimberTypePlus & TimberTypePlus::operator=( TimberTypePlus & val ){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & TimberTypePlus::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        TimberTypePlus * valTimberTypePlus = dynamic_cast<TimberTypePlus *>(&val);
        if(valTimberTypePlus) {
            setValueNormal( valTimberTypePlus->m_d->value );
        }
    }
    return *this;
}

Timber::TimberType TimberTypePlus::value(){
    return m_d->value;
}

Timber::TimberType TimberTypePlus::valueNormal(){
    return m_d->value;
}

QString TimberTypePlus::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString TimberTypePlus::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

int TimberTypePlus::enumCount() {
    return m_d->enumList.size();
}

QString TimberTypePlus::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int TimberTypePlus::valueNormalInt() {
    return m_d->valueIndex();
}

void TimberTypePlus::setValueNormal( Timber::TimberType vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void TimberTypePlus::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void TimberTypePlus::setValue( Timber::TimberType vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void TimberTypePlus::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void TimberTypePlus::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}

class WoodEssenceEnumVal{
public:
    WoodEssenceEnumVal(Timber::WoodEssence we, const QString & ns, const QString & s):
        val(we),
        normalStr(ns),
        str(s){
    };
    Timber::WoodEssence val;
    QString normalStr;
    QString str;
};

class WoodEssencePlusPrivate{
    Q_DECLARE_TR_FUNCTIONS(WoodEssencePlusPrivate)
public:
    WoodEssencePlusPrivate(Timber::WoodEssence we):
        value(we){
        enumList.append( WoodEssenceEnumVal( Timber::softwoods, "SoftWoods", trUtf8("Conifere") ) );
        enumList.append( WoodEssenceEnumVal( Timber::hardwoods, "HardWoods", trUtf8("Latifoglie") ) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    }
    Timber::WoodEssence value;
    QList<WoodEssenceEnumVal> enumList;
};

WoodEssencePlus::WoodEssencePlus( Timber::WoodEssence we,
                                  const QString & nn,
                                  bool ro):
    EnumPlus( nn, ro),
    m_d( new WoodEssencePlusPrivate(we) ){
}

WoodEssencePlus::~WoodEssencePlus(){
    delete m_d;
}

QString WoodEssencePlus::typeVar(){
    return QString("WoodEssence");
}

WoodEssencePlus & WoodEssencePlus::operator=( WoodEssencePlus & val ){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( m_d->value );
    }
    return *this;
}

VarPlus & WoodEssencePlus::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        WoodEssencePlus * valWoodEssencePlus = dynamic_cast<WoodEssencePlus *>(&val);
        if(valWoodEssencePlus) {
            setValueNormal( valWoodEssencePlus->m_d->value );
        }
    }
    return *this;
}

Timber::WoodEssence WoodEssencePlus::value(){
    return valueNormal();
}

Timber::WoodEssence WoodEssencePlus::valueNormal(){
    return m_d->value;
}

QString WoodEssencePlus::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString WoodEssencePlus::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

int WoodEssencePlus::enumCount() {
    return m_d->enumList.size();
}

QString WoodEssencePlus::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int WoodEssencePlus::valueNormalInt() {
    return m_d->valueIndex();
}

void WoodEssencePlus::setValueNormal( Timber::WoodEssence vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void WoodEssencePlus::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void WoodEssencePlus::setValue( Timber::WoodEssence vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void WoodEssencePlus::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void WoodEssencePlus::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}
