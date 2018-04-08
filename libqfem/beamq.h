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
#ifndef BEAMQ_H
#define BEAMQ_H

#include "beam.h"

#include <QObject>
#include "varpluscontainer.h"

class BeamQPrivate;
class Hypothesis;
class BeamQSysRef;
class BeamQSysRefGlobal;
class DoublePlus;
class UIntPlus;

/**
* @class BeamQ
*
* @brief Classe usata per definire un carico distribuito su un elemento Beam
*
* Questa classe viene impiegata per definire un crico distribuito su un elemento
* Beam
*
* @author Michele Mocciola
*
*/

class BeamQ : public QObject, public VarPlusContainer
{
public:
    enum qSysRef {
        qLocalSR,
        qGlobalLocalSR,
        qGlobalSR
    };

    BeamQ( Hypothesis * hyp, UnitMeasure * ump, const QString & n, QObject * parent = 0);

    static int nVert();

    BeamQSysRef * sysRef();
    BeamQSysRefGlobal *sysRefGlobal();

    double * qVal() const;
    DoublePlus * q(int);

    void nextId();

private:
    BeamQPrivate * m_d;
};



#include "enumplus.h"

class BeamQSysRefPrivate;

/**
* @class BeamQSysRef
*
* @brief Classe usata il sistema di riferimento rispetto a cui è fissato il carico esterno agente
*
* Questa classe viene impiegata per determinare il sistema di riferimento rispetto a cui viene
* riferito il carico distribuito agente
*
* @author Michele Mocciola
*
*/

class BeamQSysRef : public EnumPlus
{
    Q_OBJECT
public:
    BeamQSysRef( BeamQ::qSysRef sr,
                 const QString & nn,
                 bool ro = false );

    QString typeVar();

    BeamQSysRef & operator=( BeamQSysRef &);
    VarPlus & operator=( VarPlus &);

    BeamQ::qSysRef value();
    BeamQ::qSysRef valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    static int enumCountStatic();
    static QString enumStrStatic( int );

    int valueNormalInt();

public slots:
    void setValue( BeamQ::qSysRef, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( BeamQ::qSysRef, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    BeamQSysRefPrivate * m_d;

    ~BeamQSysRef();
};

class BeamQSysRefGlobalPrivate;

/**
* @class BeamQSysRefGlobal
*
* @brief Classe usata per definire il piano rispetto a cui è riferito il carico distribuito nel caso
* di sistema di riferimento globale
*
* Questa classe viene impiegata per determinare il piano rispetto a cui è riferito il carico distribuito nel caso
* di sistema di riferimento globale. E' utile nel caso di carichi riferiti al piano orizzontale (carico neve).
*
* @author Michele Mocciola
*
*/

class BeamQSysRefGlobal : public EnumPlus
{
    Q_OBJECT
public:
    BeamQSysRefGlobal( int v,
                       Hypothesis * hyp,
                       const QString & nn,
                       bool ro = false );

    QString typeVar();

    BeamQSysRefGlobal & operator=( BeamQSysRefGlobal &);
    VarPlus & operator=( VarPlus &);

    int value();
    int valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( int, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( int, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    BeamQSysRefGlobalPrivate * m_d;

    ~BeamQSysRefGlobal();
};

#endif // BEAMQ_H
