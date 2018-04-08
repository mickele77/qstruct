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
#ifndef SECTIONMODEL_H
#define SECTIONMODEL_H

class SectionModelPrivate;
class MaterialModel;
class Material;
class Section;
class QXmlStreamAttributes;
class QComboBox;

#include "section_export.h"
#include "section_def.h"
#include "tablemodelplus.h"
#include "unitmeasure.h"

class EXPORT_SECTION_LIB_OPT SectionModel : public TableModelPlus {
    Q_OBJECT
public:

    /** Costruttore */
    explicit SectionModel(UnitMeasure * ump, MaterialModel * matModel, QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /** La colonna del modello che rappresenta il materiale della sezione. */
    static int materialColumn();

    /** Scrive su file XML il contenuto del modello. */
    void writeXml(QXmlStreamWriter *writer);
    /** Legge da flusso XML i dati necessari per il modello */
    void readXml( QXmlStreamReader * reader );

    /** Restituisce la sezione i */
    Section *section(int i);
    /** Restituisce la sezione i */
    Section *section(int i) const;
    /** Restituisce la sezione di id i. */
    Section *sectionId( unsigned int i );
    /** Restituisce la prima sezione di tipo st. */
    Section *firstSection( SectionSpace::SectionType st=SectionSpace::GenericSection );
    /** Restituisce l'ultima sezione di tipo st. */
    Section *lastSection( SectionSpace::SectionType st=SectionSpace::GenericSection );

    /** Numero complessivo di sezioni presenti */
    int count();
    /** Numero di sezioni di tipo t */
    int count( SectionSpace::SectionType t );
    /** Inserisce la sezione di tipo t */
    void insertRows( SectionSpace::SectionType t, int position, int count = 1);
    /** Rimuove la sezione */
    void removeRows(int position, int count = 1 );

    /** Crea una sezione */
    static Section *createSection(UnitMeasure *ump, MaterialModel *matModel, const QString &sectType, const QXmlStreamAttributes &attrs);

    /** Restituisce una lista delle sezioni contenute nel modello.
        @return lista dei puntatori delle sezioni di tipo st contenuti nel modello */
    QList< Section *> sectionsList(SectionSpace::SectionType st);

private slots:
    void updateHeaders();
    void updateHeadersUM(UnitMeasure::unitMeasure u);
    void updateMaterial();

private:
    SectionModelPrivate * m_dd;

    /** Crea una sezione */
    Section *createSection( SectionSpace::SectionType );
    /** Inserisce addedSect nella posizione position */
    void insertSection(Section *addedSect, int position);
};

#endif // SECTIONMODEL_H
