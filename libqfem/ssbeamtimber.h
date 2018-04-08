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
#ifndef SSBEAMTIMBER_H
#define SSBEAMTIMBER_H

#include "ssbeam.h"

class SectionTimber;
class ServiceClass;

class SSBeamTimber : public SSBeam
{
public:
    SSBeamTimber( UnitMeasure * ump, SectionTimber * ss );

    DoublePlus * lRelLT;
    DoublePlus * kCrLT;
    ServiceClass * serviceClass;

private:
    void initVar();

};

#endif // SSBEAMTIMBER_H
