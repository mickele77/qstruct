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
#include "sectionprofile.h"

#include "point2dplus.h"
#include "doubleplus.h"
#include "qstringplus.h"

#include <cmath>

SectionProfile::SectionProfile(UnitMeasure *ump, Material *mat, const QString &n, QObject *parent) :
    Section( ump, mat, n, parent) {
    initVar();
}

SectionProfile::SectionProfile(UnitMeasure *ump, const QXmlStreamAttributes &attrs,
                               MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent):
    Section( ump, NULL, "", parent ) {
    initVar();
    loadFromXML( attrs, mModel, lModel );
}

SectionSpace::SectionType SectionProfile::sectionType() {
    return SectionSpace::ProfileSection;
}

double SectionProfile::SyNormal() {
    double a = -thetaD->valueNormal();
    // double dy = -D->y->valueNormal();
    double dz = -D->z->valueNormal();

    double SYRot = - SDz->valueNormal() * sin(a) + SDy->valueNormal() * cos(a);
    return (SYRot - A->valueNormal() * dz);
}

double SectionProfile::SzNormal() {
    double a = -thetaD->valueNormal();
    double dy = -D->y->valueNormal();
    // double dz = -D->z->valueNormal();

    double SZRot = SDz->valueNormal() * cos(a) + SDy->valueNormal() * sin(a);
    return (SZRot - A->valueNormal() * dy);
}

double SectionProfile::IyyNormal() {
    double a = -thetaD->valueNormal();
    // double dy = -D->y->valueNormal();
    double dz = -D->z->valueNormal();

    double SYRot =  - SDz->valueNormal() * sin(a) + SDy->valueNormal() * cos(a);;
    double IYYRot = IDyy->valueNormal() * pow(cos(a), 2.0) - 2.0 * IDyz->valueNormal() * sin(a) * cos(a) + IDzz->valueNormal() * pow(sin(a), 2.0);
    return (IYYRot - 2 * dz * SYRot + A->valueNormal() * dz * dz);
}

double SectionProfile::IzzNormal() {
    double a = -thetaD->valueNormal();
    double dy = -D->y->valueNormal();
    // double dz = -D->z->valueNormal();

    double SZRot = SDz->valueNormal() * cos(a) + SDy->valueNormal() * sin(a);
    double IZZRot = IDyy->valueNormal() * pow(sin(a), 2.0) + 2.0 * IDyz->valueNormal() * sin(a) * cos(a) + IDzz->valueNormal() * pow(cos(a), 2.0);
    return (IZZRot - 2 * dy * SZRot + A->valueNormal() * dy * dy );
}

double SectionProfile::IyzNormal() {
    double a = -thetaD->valueNormal();
    double dy = -D->y->valueNormal();
    double dz = -D->z->valueNormal();

    double SYRot =  - SDz->valueNormal() * sin(a) + SDy->valueNormal() * cos(a);
    double SZRot = SDz->valueNormal() * cos(a) + SDy->valueNormal() * sin(a);
    double IYZRot = (IDyy->valueNormal() - IDzz->valueNormal()) * sin(2.0*a) / 2.0 + IDyz->valueNormal() * cos(2.0*a);
    return (IYZRot - dy * SYRot - dz * SZRot + A->valueNormal() * dy * dz );
}

double SectionProfile::Sy3DyDzNormal() {
    double a = -thetaD->valueNormal();
    double dy = -D->y->valueNormal();
    // double dz = -D->z->valueNormal();

    double sina = sin(a);
    double cosa = cos(a);

    double Sy3DyDzRot = pow(cosa,3.0) * SDy3DyDz->valueNormal() + 3.0 * sina * cosa * cosa * SDy2zDyDz->valueNormal() + 3.0 * sina * sina * cosa * SDyz2DyDz->valueNormal() + pow(sina, 3.0) * SDz3DyDz->valueNormal();
    double IZZRot = IDyy->valueNormal() * sina * sina + 2.0 * IDyz->valueNormal() * sina * cosa + IDzz->valueNormal() * cosa * cosa;
    double SZRot = SDz->valueNormal() * cosa + SDy->valueNormal() * sina;
    return Sy3DyDzRot - 3.0 * dy * IZZRot + 3.0 * dy * dy * SZRot - pow(dy,3.0) * A->valueNormal();
}

double SectionProfile::Sz3DyDzNormal() {
    double a = -thetaD->valueNormal();
    // double dy = -D->y->valueNormal();
    double dz = -D->z->valueNormal();

    double sina = sin(a);
    double cosa = cos(a);

    double Sz3DyDzRot = - pow(sina,3.0) * SDy3DyDz->valueNormal() + 3.0 * sina * sina * cosa * SDy2zDyDz->valueNormal() - 3.0 * sina * cosa * cosa * SDyz2DyDz->valueNormal() + pow(cosa, 3.0) * SDz3DyDz->valueNormal();
    double IYYRot = IDyy->valueNormal() * cosa * cosa - 2.0 * IDyz->valueNormal() * sina * cosa + IDzz->valueNormal() * sina * sina;
    double SYRot = - SDz->valueNormal() * sina + SDy->valueNormal() * cosa;
    return Sz3DyDzRot - 3.0 * dz * IYYRot + 3.0 * dz * dz * SYRot - pow(dz,3.0) * A->valueNormal();
}

double SectionProfile::Sy2zDyDzNormal() {
    double a = -thetaD->valueNormal();
    double dy = -D->y->valueNormal();
    double dz = -D->z->valueNormal();

    double sina = sin(a);
    double cosa = cos(a);

    double Sy2zDyDzRot = - sina * cosa * cosa * SDy3DyDz->valueNormal() - 2.0 * sina * sina * cosa * SDy2zDyDz->valueNormal() - pow(sina,3.0) * SDyz2DyDz->valueNormal() \
            + pow(cosa, 3.0) * SDy2zDyDz->valueNormal() + 2.0 * sina * cosa * cosa * SDyz2DyDz->valueNormal() + sina * sina * cosa * SDz3DyDz->valueNormal();
    double IYZRot = (IDyy->valueNormal() - IDzz->valueNormal() )* sina * cosa + IDyz->valueNormal() * ( cosa * cosa - sina * sina );
    double IZZRot = IDyy->valueNormal() * sina * sina + 2.0 * IDyz->valueNormal() * sina * cosa + IDzz->valueNormal() * cosa * cosa;
    double SYRot = - SDz->valueNormal() * sina + SDy->valueNormal() * cosa;
    double SZRot = SDz->valueNormal() * cosa + SDy->valueNormal() * sina;
    return (Sy2zDyDzRot - 2.0 * dy * IYZRot - dz * IZZRot + dy * dy * SYRot + 2.0 * dy * dz * SZRot - dy * dy * dz * A->valueNormal());
}

double SectionProfile::Syz2DyDzNormal() {
    double a = -thetaD->valueNormal();
    double dy = -D->y->valueNormal();
    double dz = -D->z->valueNormal();

    double sina = sin(a);
    double cosa = cos(a);

    double Syz2DyDzRot = sina * sina * cosa * SDy3DyDz->valueNormal() - 2.0 * sina * cosa * cosa * SDy2zDyDz->valueNormal() + pow(cosa,3.0) * SDyz2DyDz->valueNormal() \
            + pow(sina, 3.0) * SDy2zDyDz->valueNormal() - 2.0 * sina * sina * cosa * SDyz2DyDz->valueNormal() + sina * cosa * cosa * SDz3DyDz->valueNormal();
    double IYZRot = (IDyy->valueNormal() - IDzz->valueNormal() )* sina * cosa + IDyz->valueNormal() * ( cosa * cosa - sina * sina );
    double IYYRot  = IDyy->valueNormal() * cosa * cosa - 2.0 * IDyz->valueNormal() * sina * cosa + IDzz->valueNormal() * sina * sina;
    double SYRot = - SDz->valueNormal() * sina + SDy->valueNormal() * cosa;
    double SZRot = SDz->valueNormal() * cosa + SDy->valueNormal() * sina;
    return (Syz2DyDzRot - 2.0 * dz * IYZRot - dy * IYYRot + dz * dz * SZRot + 2.0 * dy * dz * SYRot - dy * dz * dz * A->valueNormal());
}

void SectionProfile::initVar() {
    *m_typeNameInternal = "SectionProfile";
    typeName->setValue( trUtf8("Sezione Profilo"));

    D = new Point2DPlus("D", "yD", 0.0, "zD", 0.0, m_unitMeasure, UnitMeasure::sectL, false );
    D->setRichName(trUtf8("Coordinate del punto D rispetto a cui è definita la sezione"));
    D->y->setToolTip( trUtf8("Ascissa del punto D rispetto a cui è definita la sezione"));
    D->z->setToolTip( trUtf8("Ordinata del punto D rispetto a cui è definita la sezione"));
    D->setRichName( "D");
    D->y->setRichName( trUtf8("y<span style=\"vertical-align:sub;\">D</span>") );
    D->z->setRichName( trUtf8("z<span style=\"vertical-align:sub;\">D</span>") );
    addVarToContainer( D );
    connect( D, &VarPlus::valueChanged, this, &Section::sectionChanged );

    thetaD = new DoublePlus( 0.0, "thetaD", m_unitMeasure, UnitMeasure::angle, false );
    thetaD->setToolTip( trUtf8("Angolo di inclinazione dell'asse rispetto a cui è definita la sezione rispetto all'asse y"));
    thetaD->setRichName( trUtf8("θ<span style=\" vertical-align:sub;\">D,y</span>"));
    addVarToContainer( thetaD );
    connect( thetaD, &VarPlus::valueChanged, this, &Section::sectionChanged );

    SDy = new DoublePlus( 0.0, "SDy", m_unitMeasure, UnitMeasure::sectL3 );
    SDy->setToolTip( trUtf8("Momento statico rispetto all'asse yD"));
    SDy->setRichName( trUtf8("S<span style=\"vertical-align:sub;\">D,y</span>") );
    addVarToContainer( SDy );
    connect( SDy, &VarPlus::valueChanged, this, &Section::sectionChanged );

    SDz = new DoublePlus( 0.0, "SDz", m_unitMeasure, UnitMeasure::sectL3 );
    SDz->setToolTip( trUtf8("Momento statico rispetto all'asse zD"));
    SDz->setRichName( trUtf8("S<span style=\"vertical-align:sub;\">D,z</span>") );
    addVarToContainer( SDz );
    connect( SDz, &VarPlus::valueChanged, this, &Section::sectionChanged );

    Sy->setReadOnly( true );
    Sy->addConnectedVars( 5, D->z, thetaD, A, SDy, SDz );
    Sz->setReadOnly( true );
    Sz->addConnectedVars( 5, D->y, thetaD, A, SDy, SDz );

    IDyy = new DoublePlus( 0.0, "IDyy", m_unitMeasure, UnitMeasure::sectL4 );
    IDyy->setToolTip( trUtf8("Momento di inerzia rispetto all'asse yD"));
    IDyy->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">D,y</span>") );
    addVarToContainer( IDyy );
    connect( IDyy, &VarPlus::valueChanged, this, &Section::sectionChanged );

    IDzz = new DoublePlus( 0.0, "IDzz", m_unitMeasure, UnitMeasure::sectL4 );
    IDzz->setToolTip( trUtf8("Momento di inerzia rispetto all'asse zD"));
    IDzz->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">D,z</span>") );
    addVarToContainer( IDzz );
    connect( IDzz, &VarPlus::valueChanged, this, &Section::sectionChanged );

    IDyz = new DoublePlus( 0.0, "IDyz", m_unitMeasure, UnitMeasure::sectL4 );
    IDyz->setToolTip( trUtf8("Momento centrifugo rispetto agli assi yD e zD"));
    IDyz->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">D,yz</span>") );
    addVarToContainer( IDyz );
    connect( IDyz, &VarPlus::valueChanged, this, &Section::sectionChanged );

    Iyy->setReadOnly(true);
    Iyy->addConnectedVars( 8, D->z, thetaD, A, SDy, SDz, IDyy, IDzz, IDyz );
    Izz->setReadOnly(true);
    Izz->addConnectedVars( 8, D->y, thetaD, A, SDy, SDz, IDyy, IDzz, IDyz );
    Iyz->setReadOnly(true);
    Iyz->addConnectedVars( 9, D->y, D->z, thetaD, A, SDy, SDz, IDyy, IDzz, IDyz );

    SDy3DyDz = new DoublePlus( 0.0, "SDy3DyDz", m_unitMeasure, UnitMeasure::sectL5 );
    SDy3DyDz->setToolTip( trUtf8("Integrale di yD^3 sulla superficie della sezione"));
    SDy3DyDz->setRichName( trUtf8("∬ y<span style=\"vertical-align:sub;\">D</span><span style=\" vertical-align:super;\">3</span> dA") );
    addVarToContainer( SDy3DyDz );
    connect( SDy3DyDz, &VarPlus::valueChanged, this, &Section::sectionChanged );

    SDz3DyDz = new DoublePlus( 0.0, "SDz3DyDz", m_unitMeasure, UnitMeasure::sectL5 );
    SDz3DyDz->setToolTip( trUtf8("Integrale di zD^3 sulla superficie della sezione"));
    SDz3DyDz->setRichName( trUtf8("∬ z<span style=\"vertical-align:sub;\">D</span><span style=\" vertical-align:super;\">3</span> dA") );
    addVarToContainer( SDz3DyDz );
    connect( SDz3DyDz, &VarPlus::valueChanged, this, &Section::sectionChanged );

    SDy2zDyDz = new DoublePlus( 0.0, "SDy2zDyDz", m_unitMeasure, UnitMeasure::sectL5 );
    SDy2zDyDz->setToolTip( trUtf8("Integrale di yC^2*zC sulla superficie della sezione"));
    SDy2zDyDz->setRichName( trUtf8("∬ y<span style=\"vertical-align:sub;\">D</span><span style=\" vertical-align:super;\">2</span>z<span style=\"vertical-align:sub;\">D</span> dA") );
    addVarToContainer( SDy2zDyDz );
    connect( SDy2zDyDz, &VarPlus::valueChanged, this, &Section::sectionChanged );

    SDyz2DyDz = new DoublePlus( 0.0, "SDyz2DyDz", m_unitMeasure, UnitMeasure::sectL5 );
    SDyz2DyDz->setToolTip( trUtf8("Integrale di yD*zD^2 sulla superficie della sezione"));
    SDyz2DyDz->setRichName( trUtf8("∬ y<span style=\"vertical-align:sub;\">D</span>z<span style=\"vertical-align:sub;\">D</span><span style=\" vertical-align:super;\">2</span> dA") );
    addVarToContainer( SDyz2DyDz );
    connect( SDyz2DyDz, &VarPlus::valueChanged, this, &Section::sectionChanged );

    Sy3DyDz->setReadOnly(true);
    Sy3DyDz->addConnectedVars( 12, D->y, thetaD, A, SDy, SDz, IDyy, IDzz, IDyz, SDy3DyDz, SDz3DyDz, SDy2zDyDz, SDyz2DyDz );
    Sz3DyDz->setReadOnly(true);
    Sz3DyDz->addConnectedVars( 12, D->z, thetaD, A, SDy, SDz, IDyy, IDzz, IDyz, SDy3DyDz, SDz3DyDz, SDy2zDyDz, SDyz2DyDz );
    Sy2zDyDz->setReadOnly(true);
    Sy2zDyDz->addConnectedVars( 13, D->y, D->z, thetaD, A, SDy, SDz, IDyy, IDzz, IDyz, SDy3DyDz, SDz3DyDz, SDy2zDyDz, SDyz2DyDz );
    Syz2DyDz->setReadOnly(true);
    Syz2DyDz->addConnectedVars( 13, D->y, D->z, thetaD, A, SDy, SDz, IDyy, IDzz, IDyz, SDy3DyDz, SDz3DyDz, SDy2zDyDz, SDyz2DyDz );
}

void SectionProfile::translateNormal( double yTrans, double zTrans){
    D->y->setValueNormal( D->y->valueNormal() + yTrans );
    D->z->setValueNormal( D->z->valueNormal() + zTrans );
}

void SectionProfile::rotateNormal( double rot ){
    thetaD->setValueNormal( thetaD->valueNormal() + rot );
}
