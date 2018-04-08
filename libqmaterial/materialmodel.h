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
#ifndef MATERIALMODEL_H
#define MATERIALMODEL_H

class UnitMeasure;
class Material;
class QXmlStreamAttributes;

class MaterialModelPrivate;

#include "material_export.h"
#include "tablemodelplus.h"

class  EXPORT_MATERIAL_LIB_OPT MaterialModel : public TableModelPlus {
public:
    enum MaterialType{
        GenericMaterial,
        ConcreteMaterial,
        SteelCncrMaterial,
        SteelMaterial,
        TimberMaterial,
        FRPMaterial
    };

    explicit MaterialModel(UnitMeasure * ump, QObject *parent = 0);

    /** Scrive il modello su flusso XML */
    void writeXml(QXmlStreamWriter * writer);
    /** Legge il modello da flusso XML */
    void readXml(QXmlStreamReader * reader);

    /** Numero di materiali di tipo t */
    int count( MaterialType t = GenericMaterial );
    /** Puntatore al materiale i-esimo */
    Material *material(int i);
    Material *material(int) const;
    Material *materialId( unsigned int);
    Material *firstMaterial( MaterialType mt=GenericMaterial );

    /** Inserisce il materiale t */
    void insertRows( MaterialType t , int position, int count = 1 );
    /** Inserisce il materiale di nome matType e attributi attrs nella posizione
        position. Se position è negativo, aggiunge il materiale in coda. */
    void insertRows( QString matType, const QXmlStreamAttributes &attrs, int position = -1, int count = 1 );
    /** Rimuove il materiale */
    void removeRows(int position, int count = 1 );

    /** Restituisce una lista dei materiali contenuti nel modello.
        @return lista dei puntatori dei materiali di tipo mt contenuti nel modello */
    QList< Material *> materialsList(MaterialType mt);

private:
    MaterialModelPrivate * m_dd;

    /** Crea il materiale di tipo MaterialType */
    Material * createMaterial( UnitMeasure * ump, MaterialType );

    /** Crea il materiale di tipo matType, con attributi attrs */
    Material * createMaterial( UnitMeasure * ump, QString matType, const QXmlStreamAttributes &attrs );

    /** Inserisce il materiale addedMat nella posizione position
        Se position è negativo il valore viene aggiunto in coda .*/
    void insertMaterial( Material * addedMat, int position=-1 );
};

#endif // MATERIALMODEL_H
