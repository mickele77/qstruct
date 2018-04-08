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
#ifndef MULTISECTIONMODELBASE_H
#define MULTISECTIONMODELBASE_H

class Section;
class MaterialModel;
class Material;
class Point2DModel;

class QGraphicsItemGroup;
class QGraphicsItem;
class QXmlStreamAttributes;

#include "section_export.h"
#include "tablemodelplus.h"

class EXPORT_SECTION_LIB_OPT MultiSectionModelBase : public TableModelPlus {
    Q_OBJECT
public:
    /** Costrutture. */
    MultiSectionModelBase( const QString &modName, UnitMeasure * ump, MaterialModel * matModel, QObject * parent = 0 );
    ~MultiSectionModelBase();

    virtual MultiSectionModelBase & operator=( MultiSectionModelBase & );

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);


    /** Scrive il modello su file XML. Scrive solo il contenuto (la classe di riferimento inserisce
        il contesto (apertura < nome > e chiusura  < /nome > ) */
    void writeXml(QXmlStreamWriter *writer);
    /** Legge il modello da file XML */
    void readXml( QXmlStreamReader * reader );

    /** Trasla tutte le sezioni. */
    void translateNormal( double yTrans, double zTrans, bool suspSignals = true );
    /** Ruota tutte le sezioni. */
    void rotateNormal(double rot, bool suspSignals = true );

    /** Numero di sezioni elementari presenti */
    int count();
    /** @return puntatore alla sezione i-esima */
    Section * section(int i);
    /** @return puntatore alla sezione di id i */
    Section * sectionId( unsigned int);
    /** Puntatore alla prima sezione. */
    Section * firststSection();
    /** Puntatore all'ultima sezione. */
    Section * lastSection();

    /** Calcola la somma dei prodotti del modulo di Young e dell'area della sezione
     * (effettua il calcolo sommando i valori delle sezioni, non restituisce un valore in memoria)  */
    double EANormal();
    /** Calcola la somma dei prodotti del modulo di Young e del momento di inerzia della sezione
     * rispetto all'asse y-y, con centro in (yCen, zCen)
     * (effettua il calcolo sommando i valori delle sezioni, non restituisce un valore in memoria)  */
    double EIyyNormal(double yCen, double zCen);
    /** Calcola la somma dei prodotti del modulo di Young e del momento di inerzia della sezione
     * rispetto all'asse z-z, con centro in (yCen, zCen)
     * (effettua il calcolo sommando i valori delle sezioni, non restituisce un valore in memoria)  */
    double EIzzNormal(double yCen, double zCen);

    /** Calcola l'area della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double ANormal();
    /** Calcola Sy della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double SyNormal();
    /** Calcola Sz della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double SzNormal();
    /** Calcola Iy della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double IyyNormal();
    /** Calcola Iz della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double IzzNormal();
    /** Calcola Iyz della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double IyzNormal();
    /** Calcola Sy3DyDz della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double Sy3DyDzNormal();
    /** Calcola Sz3DyDz della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double Sz3DyDzNormal();
    /** Calcola Syz2DyDz della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double Syz2DyDzNormal();
    /** Calcola Sy2zDyDz della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double Sy2zDyDzNormal();
    /** Calcola pW della sezione (effettua il calcolo sommando i valori delle sezioni,
        non restituisce un valore in memoria)  */
    double pWNormal();

    /**
    * Momento allo SLE con deformazione data (deformazione piana e = l + my * z + mz * y), unità di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità di misura interne al programma)
    */
    void NMSLSNormal( double *NRet, double *MyRet, double * MzRet,
                      double l, double my, double mz,
                      QList<Point2DModel *> * sects = NULL );

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unità di misura interne al programma
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo (unità di misura interne del programma)
    */
    double NULSNormal(double l, double my, double mz, QList<Point2DModel *> *sects = NULL );

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unità di misura interne al programma
    * @param phase fase di applicazione del carico in cui si valuta lo sforzo normale
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo (unità di misura interne del programma)
    */
    double NULSNormal(int phase, QList<double> *l, QList<double> *my, QList<double> *mz, QList<Point2DModel *> *sects = NULL );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unità di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità di misura interne al programma)
    */
    void MULSNormal( double *yRet, double * zRet,
                     double l, double my, double mz,
                     QList<Point2DModel *> * sects = NULL );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unità di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param phase fase di applicazione del carico in cui viene valutato il momento
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità di misura interne al programma)
    */
    void MULSNormal(double *yRet, double * zRet,
                    int phase,
                    QList<double> *l, QList<double> *my, QList<double> *mz,
                    QList<Point2DModel *> * sects = NULL );

    /** Inserisce una sezione. */
    bool insertRows( int first, int count=1, const QModelIndex &parent = QModelIndex() );
    /** Inserisce una sezione in coda */
    bool appendRows( int count = 1 );
    /** Rimuove la sezione */
    void removeRows(int position, int count = 1 );

    /** Inserisce la sezione addedSect nel modello */
    virtual void insertSection(Section *addedSect, int position);

    /** Restituisce l'oggetto grafico che rappresenta la sezione */
    QGraphicsItem * qGraphicsItem();

    void suspendSignals( bool susp = true );

    void updateReadOnlyVars();

protected:
    /** Colonna del materiale. Se il materiale non è visualizzato è -1 */
    int m_materialColumn;
    /** Colonna della fase di carico. Se la fase non è visualizzata è -1 */
    int m_loadPhaseColumn;
    /** Contenitore sezioni */
    QList<Section *> * m_sectionContainer;
    /** Contenitore sezioni */
    QMap<unsigned int, Section *> * m_sectionMap;
    /** Inserisce le sezioni */
    void insertSectionPrivate(Section *, int p );
    /** Rimuove la sezione, null'altro  */
    void removeSectionPrivate( int position );
    /** Modello dei materiali */
    MaterialModel * m_materialModel;
    /** Oggetto grafico contenente le rappresentazioni delle sezioni */
    QGraphicsItemGroup * m_qGraphicsItem;
    /** Crea la sezione. Da reimplementare per modelli composti da sezioni omogenee.
        Di base restituisce il valore NULL. */
    virtual Section * createSection();
    /** Crea la sezione di tipo sectType.
        @param sectType tipo della sezione
        @param ump unità di misura
        @param attr attributi della sezione
        @param matModel modello dei materiali
        @param parent oggetto genitore
        @return sezione creata */
    Section * createSection(const QString & sectType, const QXmlStreamAttributes &attrs );


protected slots:
    /** Slot usate per cambiare il nome del materiale nella tabella nel caso il materiale
        di una sezione cambi. */
    virtual void updateMaterial();
};

#endif // MULTISECTIONMODELBASE_H
