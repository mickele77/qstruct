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
#ifndef MULTISECTIONFRPMODEL_H
#define MULTISECTIONFRPMODEL_H

#include "multisectionmodelbase.h"

class SectionFRP;

/**
* @class MultipleSectionFRPModel (
*
* @brief Classe usata per modellizzare un insieme di sezioni in FRP
*
* Questa classe viene impiegata per modellizzare un insieme di sezioni in FRP
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT MultiSectionFRPModel : public MultiSectionModelBase {
    Q_OBJECT

public:
    MultiSectionFRPModel( UnitMeasure *ump,
                          MaterialModel *matModel,
                          QObject *parent = 0 );

    MultiSectionFRPModel & operator=( MultiSectionFRPModel & );

    SectionFRP * sectionFRP( int );
    SectionFRP * sectionFRPId( int );

    /** Imposta il valore dello spessore, se la vairabile t è automatic */
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
      * calcolo controllando i valori delle sezioni, non restituisce un valore in memoria)  */
    double zMaxNormal(double *corrEpsfd= NULL );
    /** Minima z delle sezioni costituenti il modello (effettua il
        calcolo sommando i valori delle sezioni, non restituisce un valore in memoria)  */
    double zMinNormal();
    bool sMinNormal(double *ret, double my, double mz);
    bool sMaxNormal(double *ret, double my, double mz, double *corrEpsfd = NULL );

    /** Taglia il modello di lineearchi in due modelli di linee, sectPlus e sectMinus.
     *  su sectPlus l + my * y + mz * z è maggiore o uguale a t,
        su sectMinus l + my * y + mz * z è minore o uguale a t
        @param sectPlus la sezione su cui viene copiata la porzione di sezione con l + my * y + mz * z >= t
               se è uguale al puntatore NULL i valori calcolati vengono persi
        @param sectPlus la sezione su cui viene copiata la porzione di sezione con l + my * y + mz * z <= t
               se è uguale al puntatore NULL i valori calcolati vengono persi */
    void cut( MultiSectionFRPModel * sectPlus,
              MultiSectionFRPModel * sectMinus,
              double l,
              double my,
              double mz,
              double t = 0.0);


    /** Proezione massima e minima della sezione lungo una retta passante per
     * l'origine e parallela al versore (ny, nz)
     @return restituisce falso se il modello non contiene sezioni */
    bool proj(double *tSup, double *tInf, double ny, double nz );

    void insertSection(Section *addedSectGen, int position);

    /** Restituisce il minimo valore non nuello di epsfd */
    double epsfdMin() ;


    static int materialColumn();
    static int loadPhaseColumn();

private:
    Section * createSection();

    void updateHeaders();
};


#endif // MULTISECTIONFRPMODEL_H
