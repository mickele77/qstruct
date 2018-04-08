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
#ifndef CONNECTOR_H
#define CONNECTOR_H

class TimberConnectorType;
class TimberPredrilledHole;
class DoublePlus;
class VarPlus;
class UnitMeasure;

class QString;
class QXmlAttributes;
template <class Key, class T> class QMap;

#include "timberconnection_export.h"

#include "varpluscontainer.h"
#include <QObject>

class EXPORT_TIMBERCONNECTION_LIB_OPT TimberConnector: public QObject, public VarPlusContainer
{
    Q_OBJECT
public:
    enum ConnectorType {
        Bolt,               /**< bulloni-spinotti */
        Screw,              /**< viti */
        SmoothRoundNail,    /**< chiodi a gambo liscio, rotondi */
        SmoothSquareNail,   /**< chiodi a gambo liscio, quadrati */
        OtherRoundNail,     /**< chiodi non a gambo liscio, rotondi */
        OtherSquareNail     /**< chiodi non a gambo liscio, quadrati */
    };
    enum PredrilledHole {
        WithPredrilledHole,      /**< con preforo */
        WithoutPredrilledHole    /**< senza preforo */
    };

    TimberConnector( UnitMeasure * ump, double ffyk, double ffuk, double dd, QObject * parent = 0 );

    /** Tensione caratteristica di snervamento del connettore */
    DoublePlus * fyk;
    /** Tensione caratteristica ultima del connettore */
    DoublePlus * fuk;
    /** Diametro del connettore.
        Nel caso di viti è il diametro esterno misurato sulla filettatura */
    DoublePlus * d;
    /** Per viti, diametro effettivo della vite.
        Se il diametro del gambo è uguale al diametro esterno sulla dilettatura, è uguale a tale valore,
        a condizione che la vite entri nel legno per almeno 4 diametri. Negli altri casi è uguale a 1.1 volte
        il diametro interno (più piccolo) della filettatura. Il programma di default assume il diamtro effttivo
        pari a 0.9 il diamtro esterno */
    DoublePlus * dEf;
    /** Nel caso di chiodi, è il diametro della testa del chiodo,
        nel caso di bulloni o viti è il diametro della rondella */
    DoublePlus * dHead;
    /** Nel caso di viti, è la lunghezza della porzione filettata meno un diametro. */
    DoublePlus * lEf;
    /** Inclinazione del connettore rispetto alle fibre di legno  */
    DoublePlus * alpha;
    /** Tipologia di connettore */
    TimberConnectorType * connectorType;
    /** Con/senza preforo */
    TimberPredrilledHole * predrilledHole;
    /** Momento caratteristico di snervamento del connettore */
    DoublePlus * MyRk;

private:
    /** Inizializza gli attributi pubblici (del tipo VarPlus) presenti nella classe */
    void initVar( double ffyk, double ffuk, double dd );

private slots:
    /** Calcola il momento caratteristico di senrvamento.
        Per i chiodi applica EC5 8.14 */
    void setMyRk();

    /** Imposta le variabili visibili */
    void setVisibleVar();

    void setDEf();
};


#include "enumplus.h"
class TimberConnectorTypePrivate;

/**
* @class TimberConnectorType
*
* @brief Classe usata per accedere alle diverse tipologie di connettori per legno
*
* Questa classe viene impiegata per definire le diverse tipologie di connettori per legno
*
* @author Michele Mocciola
*
*/

class TimberConnectorType : public EnumPlus {
    Q_OBJECT
public:
    TimberConnectorType( TimberConnector::ConnectorType tt,
                         const QString & nn,
                         bool ro = false );

    QString typeVar();

    TimberConnectorType & operator=( TimberConnectorType &);
    VarPlus & operator=( VarPlus &);

    TimberConnector::ConnectorType value();
    TimberConnector::ConnectorType valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( TimberConnector::ConnectorType, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( TimberConnector::ConnectorType, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
     TimberConnectorTypePrivate * m_d;

    ~TimberConnectorType();
};

class TimberPredrilledHolePrivate;

/**
* @class PredrilledHole
*
* @brief Classe usata per definire o meno il preforo
*
* Questa classe viene impiegata per definire o meno il preforo
*
* @author Michele Mocciola
*
*/

class TimberPredrilledHole : public EnumPlus {
    Q_OBJECT
public:
    TimberPredrilledHole( TimberConnector::PredrilledHole tt,
                    const QString & nn,
                    bool ro = false );

    QString typeVar();

    TimberPredrilledHole & operator=( TimberPredrilledHole &);
    VarPlus & operator=( VarPlus &);

    TimberConnector::PredrilledHole value();
    TimberConnector::PredrilledHole valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( TimberConnector::PredrilledHole, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( TimberConnector::PredrilledHole, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    TimberPredrilledHolePrivate * m_d;

    ~TimberPredrilledHole();
};

#endif // CONNECTOR_H
