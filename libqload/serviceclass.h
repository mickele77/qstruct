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
#ifndef SERVICECLASS_H
#define SERVICECLASS_H

#include "enumplus.h"

class ServiceClassPrivate;

/**
* @class ServiceClass
*
* @brief Classe usata per accedere alle diverse tipologie di classi di servizio
*
* Questa classe viene impiegata per definire le diverse tipologie di classi di servizio
*
* @author Michele Mocciola
*
*/

class ServiceClass : public EnumPlus
{
    Q_OBJECT
public:
    /** Classe di servizio */
    enum Class {
        SC1,             /**< classe di servizio 1 */
        SC2,             /**< classe di servizio 2 */
        SC3              /**< classe di servizio 3 */
    };

    ServiceClass( ServiceClass::Class tt,
                 const QString & nn,
                 bool ro = false);

    QString typeVar();

    ServiceClass & operator=( ServiceClass &);
    VarPlus & operator=( VarPlus &);

    ServiceClass::Class value();
    QString valueStr();
    ServiceClass::Class valueNormal();
    int valueNormalInt();
    QString valueNormalStr();

    int enumCount();
    QString enumStr(int v);
public slots:
    void setValue( ServiceClass::Class, bool emitAuto = true );
    void setValue(const QString &vv, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );
    void setValueNormal( ServiceClass::Class, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );

private:
    ServiceClassPrivate * m_d;

    ~ServiceClass();
};

#endif // SERVICECLASS_H
