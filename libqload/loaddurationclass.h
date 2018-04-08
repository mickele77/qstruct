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
#ifndef LOADDURATIONCLASS_H
#define LOADDURATIONCLASS_H

#include "enumplus.h"
class LoadDurationClassPrivate;

/**
* @class LoadDurationClass
*
* @brief Classe usata per accedere alle diverse tipologie di classi di durata del carico
*
* Questa classe viene impiegata per definire le diverse tipologie di classi di durata del carico
*
* @author Michele Mocciola
*
*/

class LoadDurationClass : public EnumPlus
{
    Q_OBJECT
public:
    /** Classe di durata del carico */
    enum Class {
        permanent,        /**< permanente */
        longTerm,         /**< lunga */
        mediumTerm,       /**< media */
        shortTerm,        /**< breve */
        instantaneous     /**< istantanea */
    };

    LoadDurationClass( Class tt,
                          const QString & nn,
                          bool ro = false);

    QString typeVar();

    LoadDurationClass & operator=( LoadDurationClass &);
    VarPlus & operator=( VarPlus &);

    static int nVal();
    static Class val( int );
    static QString valStr( int );

    Class value();
    Class valueNormal();
    QString valueNormalStr();
    int valueNormalInt();
    QString valueStr();

    int enumCount();
    QString enumStr(int v);

public slots:
    void setValue( Class, bool = true );
    void setValue( const QString &vv, bool = true );
    void setValueInt( int i, bool emitAuto = true );
    void setValueNormal( Class, bool = true );
    void setValueNormal( const QString &, bool = true );

private:
    LoadDurationClassPrivate * m_d;

    ~LoadDurationClass();
};

#endif // LOADDURATIONCLASS_H
