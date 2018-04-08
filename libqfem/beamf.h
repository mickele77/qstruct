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
#ifndef BEAMF_H
#define BEAMF_H

#include "beam.h"

#include <QObject>
#include "varpluscontainer.h"

class BeamFPrivate;
class Hypothesis;
class BeamFSysRef;
class BeamFSysRefGlobal;
class DoublePlus;
class UIntPlus;

/**
* @class BeamF
*
* @brief Classe usata per definire una carico concetrato su un elemento Beam
*
* Questa classe viene impiegata per definire un carico concetrato su un elemento
* Beam. Il carico distribuito può essere:
* - forza concetrata
* - coppia concetrata
*
* @author Michele Mocciola
*
*/

class BeamF : public QObject, public VarPlusContainer
{
public:
    /** fLocal -> la forza è definita nel sistema di riferimento locale
        fGlobal -> la forza è definita nel sistema di riferimento globale */
    enum fSysRef {
        fLocal,
        fGlobal
    };

    BeamF( Hypothesis * hyp, UnitMeasure * ump, const QString & n, QObject * parent = 0);

    static int nVert();

    BeamFSysRef * sysRef();

    double * fVal() const;
    DoublePlus * f(int);

    void nextId();

private:
    BeamFPrivate * m_d;
};



#include "enumplus.h"

class BeamFSysRefPrivate;

/**
* @class BeamFSysRef
*
* @brief Classe usata il sistema di riferimento rispetto a cui è fissato il carico esterno agente
*
* Questa classe viene impiegata per determinare il sistema di riferimento rispetto a cui viene
* riferito il carico concetrato agente definito con la classe BeamF
*
* @author Michele Mocciola
*
*/

class BeamFSysRef : public EnumPlus
{
    Q_OBJECT
public:
    BeamFSysRef( BeamF::fSysRef sr,
                 const QString & nn,
                 bool ro = false );

    QString typeVar();

    BeamFSysRef & operator=( BeamFSysRef &);
    VarPlus & operator=( VarPlus &);

    BeamF::fSysRef value();
    BeamF::fSysRef valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    static int enumCountStatic();
    static QString enumStrStatic( int );

    int valueNormalInt();

public slots:
    void setValue( BeamF::fSysRef, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( BeamF::fSysRef, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    BeamFSysRefPrivate * m_d;

    ~BeamFSysRef();
};

class BeamFSysRefGlobalPrivate;

#endif // BEAMF_H
