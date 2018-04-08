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
#ifndef SOIL_H
#define SOIL_H

class UIntPlus;
class QStringPlus;
class DoublePlus;
class BoolPlus;
class SoilTypePlus;
class SoilKTheoryPlus;
class SoilEQCategory;
class SoilPrivate;

#include <QObject>
#include "varpluscontainer.h"

/**
* @class Soil
*
* @brief Classe usata per modellazzare il suolo
*
* Questa classe viene impiegata per modellizzare il suolo
*
* @author Michele Mocciola*/

class Soil: public QObject, public VarPlusContainer
{
public:
    enum SoilType {
        clay,             /**< argilla */
        sand             /**< sabbia */
    };
    enum SoilKTheory {
        Coulomb,             /**< teoria di Coulomb (superfici di rottura piane) */
        Lancellotta          /**< Lancellotta */
    };
    enum EQCategory {
        A,
        B,
        C,
        D,
        E
    };

    /** Costruttore */
    Soil( UnitMeasure * ump, const QString &n, SoilType st, QObject * parent = 0 );

    /** Costruttore da flusso XML */
    Soil(UnitMeasure *ump, const QXmlStreamAttributes &attrs, QObject *parent = 0);

    /** Indica se il materiale e' stato impiegato da qualche sezione */
    // bool isUsed();
    /** Metodo chiamato dalle sezioni se vogliono che il materiale non venga cancellato */
    // void addObjectUsing( VarPlusContainer * );
    /** Metodo chiamato dalle sezioni se vogliono che il materiale non venga cancellato */
    // void removeObjectUsing( VarPlusContainer * );

    SoilTypePlus * soilType;
    SoilKTheoryPlus * KaTheory;
    SoilKTheoryPlus * KpTheory;
    DoublePlus * phiPrimeK;
    DoublePlus * cPrimeK;
    DoublePlus * cUK;
    DoublePlus * gammaW;
    SoilEQCategory * soilEQCategory;

    double LPrimeNormal( double L, double M, double V );
    void LPrime( DoublePlus * ret, DoublePlus * L, DoublePlus * M, DoublePlus * V );

    /** Condizioni di carico drenate - Coefficiente di capacità portante legato a gammaW */
    double NGammaWPrimeNormal( double phiD );
    /** Condizioni di carico drenate - Coefficiente di capacità portante legato a gammaW */
    void NGammaWPrime( DoublePlus * ret, DoublePlus * phiD );
    /** Condizioni di carico drenate -Coefficiente di forma legato a gammaW */
    double sGammaWPrimeNormal( double phiD, double B, double L, bool infinite = false );
    /** Condizioni di carico drenate - Coefficiente di forma legato a gammaW */
    void sGammaWPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite = NULL );
    /** Condizioni di carico drenate - Coefficiente di inclinazione del carico legato a gammaW */
    double iGammaWPrimeNormal( double phiD, double cd, double V, double H, double B, double L, bool LInfinite = false);
    /** Condizioni di carico drenate - Coefficiente di inclinazione del carico legato a gammaW */
    void iGammaWPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * cd, DoublePlus * V, DoublePlus * H, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite = NULL );
    /** Condizioni di carico drenate - Coefficiente di incliazione della fondazione legato a gammaW */
    double bGammaWPrimeNormal( double phiD, double alpha );
    /** Condizioni di carico drenate -  Coefficiente di inclinazione della fondazione legato a gammaW */
    void bGammaWPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * alpha );
    /** Condizioni di carico drenate -  Coefficiente di inclinazione del piano campagna legato a gammaW
        @param omega inclinazione del piano di campagna a monte
        @return il valore del coefficiente */
    double gGammaWPrimeNormal( double omega );
    /** Condizioni di carico drenate -  Coefficiente di inclinazione del piano campagna legato a gammaW
        @param omega inclinazione del piano di campagna a monte
        @param ret variabile su cui viene scritto il valore del coefficiente */
    void gGammaWPrime( DoublePlus * ret, DoublePlus * omega );

    /** Condizioni di carico drenate - Coefficiente di capacità portante legato a c */
    double NcPrimeNormal( double phiD );
    /** Condizioni di carico drenate - Coefficiente di capacità portante legato a c */
    void NcPrime( DoublePlus * ret, DoublePlus * phiD );
    /** Condizioni di carico drenate - Coefficiente di forma legato a c */
    double scPrimeNormal( double phiD, double B, double L, bool infinite = false );
    /** Condizioni di carico drenate - Coefficiente di forma legato a c */
    void scPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite = NULL );
    /** Condizioni di carico drenate - Coefficiente di inclinazione del carico legato a c */
    double icPrimeNormal( double phiD, double cd, double V, double H, double B, double L, bool LInfinite = false);
    /** Condizioni di carico drenate - Coefficiente di inclinazione del carico legato a c */
    void icPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * cd, DoublePlus * V, DoublePlus * H, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite = NULL);
    /** Condizioni di carico drenate -  Coefficiente di incliazione della fondazione legato a c */
    double bcPrimeNormal( double phiD, double alpha );
    /** Condizioni di carico drenate -  Coefficiente di incliazione della fondazione legato a c */
    void bcPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * alpha );
    /** Condizioni di carico drenate -  Coefficiente di incliazione del piano campagna legato a c */
    double gcPrimeNormal( double phiD, double omega );
    /** Condizioni di carico drenate -  Coefficiente di incliazione del piano campagna legato a c */
    void gcPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * omega );
    /** Condizioni di carico drenate -  Coefficiente di affondamento legato a c
    @param B base della fondazione
    @param D affondamento della fondazione
    @param phiD angolo di attrito interno di progetto
    @return il coefficiente di affondamento */
    double dcPrimeNormal( double phiD, double D, double B, double L, bool LInfinite );
    /** Condizioni di carico drenate -  Coefficiente di affondamento legato a c
    @param B base della fondazione
    @param D affondamento della fondazione
    @param phiD angolo di attrito interno di progetto
    @param ret variabile su cui viene scritto il coefficiente di affondamento */
    void dcPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * D, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite );

    /** Condizioni di carico drenate - Coefficiente di capacità portante legato a q */
    double NqPrimeNormal( double phiD );
    /** Condizioni di carico drenate - Coefficiente di capacità portante legato a q */
    void NqPrime( DoublePlus * ret, DoublePlus * phiD );
    /** Coefficiente di forma legato a q */
    double sqPrimeNormal( double phiD, double B, double L, bool infinite = false );
    /** Coefficiente di forma legato a q */
    void sqPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite = NULL );
    /** Coefficiente di inclinazione del carico legato a q */
    double iqPrimeNormal( double phiD, double cd, double V, double H, double B, double L, bool LInfinite = false);
    /** Coefficiente di inclinazione del carico legato a q */
    void iqPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * cd, DoublePlus * V, DoublePlus * H, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite = NULL );
    /** Condizioni di carico drenate -  Coefficiente di incliazione della fondazione legato a q */
    double bqPrimeNormal( double phiD, double alpha );
    /** Condizioni di carico drenate -  Coefficiente di incliazione della fondazione legato a q */
    void bqPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * alpha );
    /** Condizioni di carico drenate -  Coefficiente di incliazione del piano campagna legato a q */
    double gqPrimeNormal( double omega );
    /** Condizioni di carico drenate -  Coefficiente di incliazione del piano campagna legato a q */
    void gqPrime( DoublePlus * ret, DoublePlus * omega );
    /** Condizioni di carico drenate -  Coefficiente di affondamento legato a q
    @param Bp base della fondazione
    @param D affondamento della fondazione
    @param phiD angolo di attrito interno di progetto
    @return il coefficiente di affondamento */
    double dqPrimeNormal( double phiD, double D, double Bp, double Lp, bool LInfinite );
    /** Condizioni di carico drenate -  Coefficiente di affondamento legato a q
    @param B base della fondazione
    @param D affondamento della fondazione
    @param phiD angolo di attrito interno di progetto
    @param ret variabile su cui viene scritto il coefficiente di affondamento */
    void dqPrime( DoublePlus * ret, DoublePlus * phiD, DoublePlus * D, DoublePlus * B, DoublePlus * L, BoolPlus * LInfinite );

    double qLimPrimeNormal( double gammaWDes, double cDes, double qDes, double BDes, double LDes, bool LDesInfinite,
                            double NGammaWDes, double iGammaWDes, double sGammaWDes, double bGammaWDes, double gGammaWDes,
                            double NcDes, double icDes, double scDes, double bcDes, double gcDes, double dcDes,
                            double NqDes, double iqDes, double sqDes, double bqDes, double gqDes, double dqDes );
    void qLimPrime( DoublePlus * ret,
                    DoublePlus * gammaWDes, DoublePlus * cDes, DoublePlus * qDes, DoublePlus * BDes, DoublePlus * LDes, BoolPlus * LDesInfinite,
                    DoublePlus * NGammaWDes, DoublePlus * iGammaWDes, DoublePlus * sGammaWDes, DoublePlus * bGammaWDes, DoublePlus * gGammaWDes,
                    DoublePlus * NcDes, DoublePlus * icDes, DoublePlus * scDes, DoublePlus * bcDes, DoublePlus * gcDes, DoublePlus * dcDes,
                    DoublePlus * NqDes, DoublePlus * iqDes, DoublePlus * sqDes, DoublePlus * bqDes, DoublePlus * gqDes, DoublePlus * dqDes );

    double QLimVRdPrimeNormal( double qLimD, double B, double L );
    void QLimVRdPrime( DoublePlus * ret, DoublePlus * qLimD, DoublePlus * B, DoublePlus * L );

    /** Condizioni di carico non drenate */
    DoublePlus * Nc0;
    void icNotDrained( DoublePlus * ret );
    void scNotDrained( DoublePlus * ret );
    void bcNotDrained( DoublePlus * ret );
    void dcNotDrained( DoublePlus * ret );
    void gcNotDrained( DoublePlus * ret );
    void tgNotDrained( DoublePlus * ret );
    void qlimNotDrained( DoublePlus * ret );

    double phiPrimeDNormal( double gammaTanPhiPrime );
    void phiPrimeD( DoublePlus * ret, DoublePlus *gammaTanPhiPrime);

    double cPrimeDNormal( double gammaCPrime);
    void cPrimeD( DoublePlus * ret, DoublePlus *gammaCPrime);

    double cUD( double gammaCU);
    void cUD( DoublePlus * ret, DoublePlus * gammaCU);

    double gammaWDNormal( double gammaGammaW );
    void gammaWD( DoublePlus * ret, DoublePlus *gammaGammaW );

    double KaCoulombNormal( double phi,
                            double i,
                            double beta,
                            double delta = 0.0 );
    double KaLancellottaNormal( double phi,
                                double delta );
    /** Coefficiente di spinta attiva */
    double KaNormal( double phiD,
                     double i,
                     double beta,
                     double delta );
    void Ka( DoublePlus * ret,
             DoublePlus * phiD,
             DoublePlus * i = NULL,
             DoublePlus * beta = NULL,
             DoublePlus * delta = NULL );

    /** Coefficiente di spinta attiva in condizioni dinamiche */
    double KaeNormal( double theta,
                    double phiD,
                    double i,
                    double beta,
                    double delta );

    /** Coefficiente di spinta attiva in condizioni dinamiche */
    void Kae( DoublePlus * ret,
              DoublePlus * theta,
              DoublePlus * phiD,
              DoublePlus * i = NULL,
              DoublePlus * beta = NULL,
              DoublePlus * delta = NULL );

    double KpCoulombNormal( double phi,
                            double i,
                            double beta,
                            double delta = 0.0 );
    double KpLancellottaNormal( double phi,
                                double delta );
    double KpNormal( double phiD,
                     double i,
                     double beta,
                     double delta );
    void Kp( DoublePlus * ret,
             DoublePlus * phiD,
             DoublePlus * i = NULL,
             DoublePlus * beta = NULL,
             DoublePlus * delta = NULL);

    /** Coefficiente di spinta attiva in condizioni dinamiche */
    double KpeNormal( double theta,
                    double phiD,
                    double i,
                    double beta,
                    double delta );

    /** Coefficiente di spinta attiva in condizioni dinamiche */
    void Kpe( DoublePlus * ret,
              DoublePlus * theta,
              DoublePlus * phiD,
              DoublePlus * i = NULL,
              DoublePlus * beta = NULL,
              DoublePlus * delta = NULL );

private:
    void initVar( Soil::SoilType st);
};


#include "enumplus.h"
class  SoilTypePlusPrivate;

/**
* @class SoilTypePlus
*
* @brief Classe usata per accedere alle diverse tipologie di terreno
*
* Questa classe viene impiegata per accedere alle diverse tipologie di terreno della classe Soil
*
* @author Michele Mocciola
*
*/

class SoilTypePlus : public EnumPlus
{
    Q_OBJECT
public:
    SoilTypePlus( Soil::SoilType tt,
                  const QString & nn,
                  bool ro = false );

    QString typeVar();

    SoilTypePlus & operator=( SoilTypePlus &);
    VarPlus & operator=( VarPlus &);

    Soil::SoilType value();
    Soil::SoilType valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( Soil::SoilType, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( Soil::SoilType, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    SoilTypePlusPrivate * m_d;

    ~SoilTypePlus();
};

class SoilKTheoryPlusPrivate;

/**
* @class SoilKTheoryPlus
*
* @brief Classe usata per indicare la teoria con cui è sono calcolati i coefficienti di spinta
*
* Questa classe viene impiegata per determinare la teoria con cui sono calcolati i coefficienti di
* spinta
*
* @author Michele Mocciola
*
*/

class SoilKTheoryPlus : public EnumPlus
{
    Q_OBJECT
public:
    SoilKTheoryPlus( Soil::SoilKTheory tt,
                     const QString & nn,
                     bool ro = false );

    QString typeVar();

    SoilKTheoryPlus & operator=( SoilKTheoryPlus &);
    VarPlus & operator=( VarPlus &);

    Soil::SoilKTheory value();
    Soil::SoilKTheory valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( Soil::SoilKTheory, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( Soil::SoilKTheory, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    SoilKTheoryPlusPrivate * m_d;

    ~SoilKTheoryPlus();
};

class SoilEQCategoryPrivate;

/**
* @class SoilEQCategory
*
* @brief Classe usata per indicare la categoria sismica di appartenenza del sottosuolo (NTC08 3.2.2)
*
* Questa classe viene impiegata per determinare la categoria sismica di appartenenza del sottosuolo.
* Per maggiori informazioni a proposito vedi il par. 3.2.2 delle NTC 08
*
* @author Michele Mocciola
*
*/

class SoilEQCategory : public EnumPlus
{
    Q_OBJECT
public:
    SoilEQCategory( Soil::EQCategory cc,
                     const QString & nn,
                     bool ro = false );

    QString typeVar();

    SoilEQCategory & operator=( SoilEQCategory &);
    VarPlus & operator=( VarPlus &);

    Soil::EQCategory value();
    Soil::EQCategory valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( Soil::EQCategory, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( Soil::EQCategory, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    SoilEQCategoryPrivate * m_d;

    ~SoilEQCategory();
};

#endif // SOIL_H
