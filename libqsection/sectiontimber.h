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
#ifndef SECTIONTIMBER_H
#define SECTIONTIMBER_H

#include "sectionpoints.h"

#define N_PAR 5
#define N_POINTS 16

class Timber;

#include "serviceclass.h"
#include "lstype.h"
#include "loaddurationclass.h"

class SectionTimberShape;
class UnitMeasure;
class Point2D;

class EXPORT_SECTION_LIB_OPT SectionTimber : public SectionPoints {
    Q_OBJECT

public:
    /** Tipo di sezione (usata per definire le proprieta' del materiale che dipendono al tipo di sezione) */
    enum Shape {
        rectangular,     /**< rettangolare */
        circular         /**< circolare */
    };

    /** Costruttore. */
    SectionTimber( UnitMeasure *, Timber *, const QString &, QObject * = 0 );

    /** Costruttore per creazione da file XML. */
    SectionTimber(UnitMeasure *ump, const QXmlStreamAttributes &attrs,
                  MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent = 0 );

    virtual ~SectionTimber();
    Section & operator=( Section &);
    SectionTimber & operator=( SectionTimber & );

    int np();
    int npAll();
    DoublePlus * p( int );

    DoublePlus * kh;
    DoublePlus * khy;
    DoublePlus * khz;
    DoublePlus * ksh;
    DoublePlus * Wy;
    DoublePlus * Wz;

    Timber * timber();
    void setMaterial( Material *, bool emitAuto = true );
    void setTimber( Timber *, bool emitAuto = true );

    SectionTimberShape * sectionShape;

    /** La denominzione del materiale all'interno del programma (Timber, SteelCncr, etc)
    @return la denominazione */
    QString typeSectionNormal();

    /** La denominzione del materiale per l'utente (Calcestruzzo, Acciaio per c.a., etc)
    @return la denominazione */
    QString typeSection();

    double NULSPlusNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    double NULSMinusNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    double VULSNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    double VULSYNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    double VULSZNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    double MULSXNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    double MULSYNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    double MULSZNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );

    void NULSPlus(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );
    void NULSMinus(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );
    void VULS(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );
    void VULSY(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );
    void VULSZ(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );
    void MULSX(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );
    void MULSY(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );
    void MULSZ(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );

    /** Fattore di imperfezione per instabilita' a compressione */
    double betaCNormal();

    /** Carico critico euleriano trave, asse y-y */
    double beamNCrYNormal( double Ly );
    /** Carico critico euleriano trave, asse z-z */
    double beamNCrZNormal( double Lz );

    /** Snellezza relativa trave
    @param ly snellezza trave, asse y-y
    @param lz snellezza trave, asse z-z
    @return snellezza relativa massima */
    double beamlRelBucklNormal( double l );
    /** Parametro k per calcolo coefficiente di tensione critica */
    double beamkBucklNormal( double lRel );
    /** Coefficiente di tensione critica */
    double beamkCrBucklNormal( double k, double lRelY);
    /** Compressione massima per instabilita' a compressione
    param k coefficiente di tensione critica */
    double beamNULSBucklNormal( double k,  LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );

    /** Snellezza relativa trave, compressione
    @param l snellezza */
    void beamlRelBuckl( DoublePlus * ret, DoublePlus * l );
    /** Coefficiente per calcolo coefficiente di tensione critica */
    void beamkBuckl( DoublePlus * ret, DoublePlus * lRel );
    /** coefficiente di tensione critica */
    void beamkCrBuckl( DoublePlus * ret, DoublePlus * k, DoublePlus * lRel );
    /** Compressione massima per instabilita' a compressione
    param kCr coefficiente di tensione critica
    param lRel snellezza relativa */
    void beamNULSBuckl(  DoublePlus * ret, DoublePlus * kCr, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );

    /** Snellezza relativa trave flessotorsione, asse Y-Y
    @param l lunghezza trave, asse Y-Y */
    double beamlRelBucklLTYNormal( double Ly );
    /** Snellezza relativa trave flessotorsione, asse Y-Y
    @param l lunghezza trave, asse Y-Y */
    void beamlRelBucklLTY( DoublePlus * ret, DoublePlus * Ly );
    /** Snellezza relativa trave flessotorsione, asse Z-Z
    @param l lunghezza trave, asse Z-Z */
    double beamlRelBucklLTZNormal( double Lz );
    /** Snellezza relativa trave flessotorsione, asse Y-Y
    @param l lunghezza trave, asse Z-Z */
    void beamlRelBucklLTZ( DoublePlus * ret, DoublePlus * Lz );

    /** Coefficiente di tensione critica flessotorsione */
    double beamkCrBucklLTNormal( double lRel );
    /** Coefficiente di tensione critica flessotorsione */
    void beamkCrBucklLT( DoublePlus  * ret, DoublePlus * lRel );

    /** Momento resistente massimo per instabilita' flessotorsionale, asse Y-Y
    param k coefficiente di sbantamento laterale, asse Y-Y */
    double beamMULSBucklLTYNormal( double k,  LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    /** Momento resistente massimo per instabilita' flessotorsionale, asse Z-Z
    param k coefficiente di sbantamento laterale, asse Z-Z */
    double beamMULSBucklLTZNormal( double k,  LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );

    /** Momento massimo per instabilita' flessotorsionale, asse Z-Z
    param kCrit coefficiente di sbantamento laterale, asse Y-Y */
    void beamMULSBucklLTY( DoublePlus * ret, DoublePlus * kCr, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );
    /** Momento massimo per instabilita' flessotorsionale, asse Z-Z
    param kCrit coefficiente di sbantamento laterale, asse Z-Z */
    void beamMULSBucklLTZ( DoublePlus * ret, DoublePlus * kCr, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + m * z), unitÃ  di misura interne al programma
    * @param l
    * @param m
    * @param sects zone con andamento parabolico e costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo (unitÃ  di misura interne del programma)
    */
    double NULSNormal(double l, double m, QList<Point2DModel *> *sects = NULL );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z), unita'  di misura interne al programma
    * @param xRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param yRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param sects zona con andamento parabolico e costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unitÃ  di misura interne al programma)
    */
    void MULSNormal(double *yRet, double * zRet, double l, double my, QList<Point2DModel *> * sects = NULL );


    bool verifyULSNormal(LSType::Type lsType,
                         ServiceClass::Class sClass,
                         LoadDurationClass::Class ldClass ,
                         double kCritNy,
                         double kCritNz,
                         double N,
                         double yM,
                         double zM,
                         double Mx,
                         double kCritMy,
                         double MyInput,
                         double kCritMz,
                         double MzInput,
                         double Vy,
                         double Vz,
                         double * res,
                         QString * messages = NULL );

    bool verifyULS(LSType * lsType,
                   ServiceClass * sClass,
                   LoadDurationClass * ldClass ,
                   DoublePlus * kCritNy,
                   DoublePlus * kCritNz,
                   DoublePlus * N,
                   Point2DPlus * cenM,
                   DoublePlus * Mx,
                   DoublePlus * kCritMy,
                   DoublePlus * MyInput,
                   DoublePlus * kCritMz,
                   DoublePlus * MzInput,
                   DoublePlus * Vy,
                   DoublePlus * Vz,
                   DoublePlus * res,
                   QString * messages = NULL );

    /** Carica gli attributi XML. */
    void loadFromXML(const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel);

signals:
    void geometryChanged();

private:
    int m_np;
    DoublePlus * m_p[N_PAR];

    void initVar();

    double ANormal();
    double SyNormal();
    double SzNormal();
    double IyyNormal();
    double IzzNormal();
    double IyzNormal();
    double Sz3DyDzNormal();
    double Sy3DyDzNormal();
    double Syz2DyDzNormal();
    double Sy2zDyDzNormal();
    double ITNormal();

    void updateReadOnlyVars();

    void setWy(bool emitAuto);
    void setWz(bool emitAuto);

    void setkh(bool emitAuto);
    void setkhy(bool emitAuto);
    void setkhz(bool emitAuto);
    void setksh(bool emitAuto);

    void updateParameters(bool emitAuto);
};

class SectionTimberShapePrivate;

/**
* @class SectionTimberShape
*
* @brief Classe usata per accedere alle diverse tipologie di sezioni
*
* Questa classe viene impiegata per definire le diverse tipologie di sezioni
* ai fini dell'influenza sulle caratteristiche del materiale Timber
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionTimberShape : public EnumPlus {
    Q_OBJECT
public:
    SectionTimberShape( SectionTimber::Shape tt,
                       const QString & nn,
                       bool ro = false );

    QString typeVar();

    SectionTimberShape & operator=( SectionTimberShape &);
    VarPlus & operator=( VarPlus &);

    SectionTimber::Shape valueNormal();
    QString valueNormalStr();

    SectionTimber::Shape value();
    QString valueStr();
    int enumCount();
    QString enumStr(int v);
    int valueNormalInt();

public slots:
    void setValue( SectionTimber::Shape, bool emitAuto = true );
    void setValueNormal( SectionTimber::Shape, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValue(const QString &vv, bool emitAuto = true );
    void setValueInt(int i, bool emitAuto = true );

private:
    SectionTimberShapePrivate * m_d;

    ~SectionTimberShape();
};

#endif // SECTIONTIMBER_H
