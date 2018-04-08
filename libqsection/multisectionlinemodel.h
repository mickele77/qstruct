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
#ifndef MULTISECTIONLINEMODEL_H
#define MULTISECTIONLINEMODEL_H

#include "multisectionmodelbase.h"

class SectionLine;
class Material;

/**
* @class MultipleSectionLineModel (
*
* @brief Classe usata per modellizzare un insieme di sezioni sottili rettilinee
*
* Questa classe viene impiegata per modellizzare un insieme di sezioni sottili rettilinee
* (i.e. spessore molto minore delle dimensioni trasversali della sezion)
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT MultiSectionLineModel : public MultiSectionModelBase {
    Q_OBJECT

public:
    MultiSectionLineModel(UnitMeasure *ump, Material *mat, double t, QObject *parent = 0 );

    MultiSectionLineModel & operator=( MultiSectionLineModel & );

    SectionLine * sectionLine( int );
    SectionLine * sectionLineId( int );

    /** Restituisce il materiale delle sezioni presenti nel  modell */
    Material * material();
    /** Imposta il materiale delle sezioni presenti nel modelli
      @param mat il nuovo materiale
      @param emitAuto emetti il segnale modelChanged */
    void setMaterial( Material * mat, bool emitAuto = true );

    /** Imposta il valore dello spessore, se la variabile t è automatic */
    void setTNormal( double val, bool emitAuto = true );

    /** Calcola il fattore di rigidezza torsionale della sezione (effettua il
        calcolo sommando i valori delle sezioni, non restituisce un valore in memoria)  */
    double ITNormal();
    /** Massima y delle sezioni costituenti il modello (effettua il
        calcolo sommando i valori delle sezioni, non restituisce un valore in memoria)  */
    double yMaxNormal();
    /** Minima y delle sezioni costituenti il modello (effettua il
        calcolo sommando i valori delle sezioni, non restituisce un valore in memoria)  */
    double yMinNormal();
    /** Massima z delle sezioni costituenti il modello (effettua il
        calcolo sommando i valori delle sezioni, non restituisce un valore in memoria)  */
    double zMaxNormal();
    /** Minima z delle sezioni costituenti il modello (effettua il
        calcolo sommando i valori delle sezioni, non restituisce un valore in memoria)  */
    double zMinNormal();

    /** Taglia il modello di linee in due modelli di linee, sectPlus e sectMinus.
     *  su sectPlus l + my * y + mz * z è maggiore o uguale a t,
        su sectMinus l + my * y + mz * z è minore o uguale a t
        @param sectPlus la sezione su cui viene copiata la porzione di sezione con l + my * y + mz * z >= t
               se è uguale al puntatore NULL i valori calcolati vengono persi
        @param sectMinus la sezione su cui viene copiata la porzione di sezione con l + my * y + mz * z <= t
               se è uguale al puntatore NULL i valori calcolati vengono persi */
    void cut( MultiSectionLineModel * sectPlus,
              MultiSectionLineModel * sectMinus,
              double l,
              double my,
              double mz,
              double t = 0.0);


    /** Proiezione massima e minima della sezione lungo una retta passante per
     * l'origine e parallela al versore (ny, nz)
     @return restituisce falso se il modello non contiene sezioni */
    bool proj(double *tSup, double *tInf, double ny, double nz );

    void insertSection(Section *addedSectGen, int position);

signals:
    void setTNormalSignal();

private:
    Material * m_material;
    double m_t;

    Section * createSection();

private slots:
    void updateHeaders();
};


#endif // MULTISECTIONLINEMODEL_H
