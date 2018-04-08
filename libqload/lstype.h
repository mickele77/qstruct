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
#ifndef LSTYPE_H
#define LSTYPE_H

#include "enumplus.h"

class LSTypePrivate;

/**
* @class LSType
*
* @brief Classe usata per accedere alle diverse tipologie di stati limite
*
* Questa classe viene impiegata per definire le diverse tipologie di stati limite
*
* @author Michele Mocciola
*
*/

class LSType : public EnumPlus {
    Q_OBJECT
public:
    enum Type {
        ULSFundamental,     /**< Stato Limite Ultimo - combinazioni fondamentali */
        ULSExceptional,     /**< Stato Limite Ultimo - combinazioni eccezionali */
        ELS                 /**< Stato Limite di Esercizio */
    };

    LSType( Type tt,
            const QString & nn,
            bool ro = false);

    QString typeVar();

    LSType & operator=( LSType &);
    VarPlus & operator=( VarPlus &);

    Type value();
    Type valueNormal();
    QString valueNormalStr();
    int valueNormalInt();
    QString valueStr();

    int enumCount();
    QString enumStr(int v);

public slots:
    void setValue( Type, bool emitAuto = true );
    void setValue(const QString &vv, bool emitAuto = true );
    void setValueInt(int i, bool emitAuto = true );
    void setValueNormal( Type, bool emitAuto = true );
    void setValueNormal( const QString & v, bool emitAuto = true );

private:
    LSTypePrivate * m_d;

    ~LSType();
};


#endif // LSTYPE_H
