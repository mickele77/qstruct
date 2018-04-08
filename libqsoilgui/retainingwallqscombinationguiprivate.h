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
#ifndef RETAININGWALLQSCOMBINATIONGUIPRIVATE_H
#define RETAININGWALLQSCOMBINATIONGUIPRIVATE_H

#include "varpluscontainer.h"

class Point2DPlus;
class DoublePlus;
class BoolPlus;
class Point2DPlus;

class RetainingWallQSCombinationGUIPrivate: public VarPlusContainer  {
private:
    void initVar();

public:
    RetainingWallQSCombinationGUIPrivate( UnitMeasure * ump, const QString & cn,
                                        DoublePlus *gk2, DoublePlus *qk,
                                        DoublePlus * gTanPhiPrime, DoublePlus * gCPrime,
                                        DoublePlus *gG1Min, DoublePlus *gG1Max,
                                        DoublePlus *gG2Min, DoublePlus *gG2Max,
                                        DoublePlus *gQMin, DoublePlus *gQMax,
                                        DoublePlus *gSliding, DoublePlus *gQLim  );

    int index;
    RetainingWall * retainingWall;
    QString combinationName;

    DoublePlus * Gk2;
    DoublePlus * Qk;
    DoublePlus * gammaTanPhiPrime;
    DoublePlus * gammaCPrime;
    DoublePlus *gammaG1Min;
    DoublePlus *gammaG1Max;
    DoublePlus *gammaG2Min;
    DoublePlus *gammaG2Max;
    DoublePlus *gammaQMin;
    DoublePlus *gammaQMax;
    DoublePlus *gammaSliding;
    DoublePlus *gammaQLim;

    DoublePlus * SoilDownPhiPrimeD;
    DoublePlus * SoilDownCPrimeD;
    DoublePlus * SoilDownDeltaD;
    DoublePlus * SoilUpPhiPrimeD;
    DoublePlus * SoilUpCPrimeD;
    DoublePlus * SoilUpDeltaD;
    DoublePlus * Ka;
    DoublePlus * Kp;
    DoublePlus * ChiKp;
    DoublePlus * KpRed;
    /** Verifica a scorrimento */
    DoublePlus * SlidingVEd;
    DoublePlus * SlidingHEd;
    DoublePlus * SlidingHRdP;
    BoolPlus * SlidingApplyKp;
    DoublePlus * SlidingHRd;
    /** Verifica capacità portante */
    DoublePlus * QLimSoilDownGammaWPrimeD;
    DoublePlus * QLimVEd;
    BoolPlus * QLimApplyKp;
    DoublePlus * QLimHEd;
    DoublePlus * QLimHRdPMax;
    DoublePlus * QLimHRdP;
    Point2DPlus * QLimMEdC;
    DoublePlus * QLimMEd;
    DoublePlus * QLimQEd;
    DoublePlus * QLimBPrime;
    DoublePlus * QLimLPrime;
    DoublePlus * QLimD;
    BoolPlus * QLimLInfinite;
    DoublePlus * QLimNGammaW;
    DoublePlus * QLimSGammaW;
    DoublePlus * QLimIGammaW;
    DoublePlus * QLimBGammaW;
    DoublePlus * QLimGGammaW;
    DoublePlus * QLimNC;
    DoublePlus * QLimSC;
    DoublePlus * QLimIC;
    DoublePlus * QLimBC;
    DoublePlus * QLimGC;
    DoublePlus * QLimDC;
    DoublePlus * QLimNQ;
    DoublePlus * QLimSQ;
    DoublePlus * QLimIQ;
    DoublePlus * QLimBQ;
    DoublePlus * QLimGQ;
    DoublePlus * QLimDQ;
    DoublePlus * qLimD;
    DoublePlus * QLimVRd;
};

#endif // RETAININGWALLQSCOMBINATIONGUIPRIVATE_H
