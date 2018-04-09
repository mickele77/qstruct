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
#ifndef SECTIONRCNCR_H
#define SECTIONRCNCR_H

#include "section.h"

class IntPlus;
class UnitMeasure;
class MaterialModel;
class MultiSectionCncrModel;
class MultiSectionSteelCncrModel;
class MultiSectionSteelModel;
class MultiSectionFRPModel;

class QGraphicsItemGroup;
class QPolygonF;

class SectionRCncrPrivate;

class EXPORT_SECTION_LIB_OPT SectionRCncr: public Section {
    Q_OBJECT

private:
    SectionRCncrPrivate * m_d;

    /** calcola lambda, muy  muz in funzione di due punti di passaggio del piano di deformazione */
    bool lmP1P2(double *l, double *a, double s1, double e1, double s2, double e2);

    /** Inizializza le variabili proprie della sezione */
    void initVar();

    void setGCncrY( bool emitAuto );
    void setGCncrZ( bool emitAuto );

public:
    /** Costruttore */
    SectionRCncr(UnitMeasure *,
                 MaterialModel *matModel,
                 const QString & mm,
                 QObject * = 0);

    /** Costruttore per cerazione da attributi XML */
    SectionRCncr(UnitMeasure * ump,
                 const QXmlStreamAttributes & attrs,
                 MaterialModel * matModel,
                 SectionLoadPhaseModel * lModel,
                 QObject * parent = 0);

    ~SectionRCncr();

    SectionRCncr & operator=( SectionRCncr &);
    Section & operator=( Section &);

    /** Legge da flusso XML i dati necessari per la sezione. */
    void readXml( QXmlStreamReader * reader );

    SectionLoadPhaseModel *loadPhaseModel();

    /** Scrive la sezione su flusso XML */
    void writeXml(QXmlStreamWriter *writer);

    void suspendSignals( bool susp = true );
    void updateReadOnlyVars();

    /** baricentro della sezione in cls */
    Point2DPlus * GCncr;
    /** baricentro della sezione in acciaio */
    Point2DPlus * GSteelCncr;

    void translateNormal(double, double, bool suspSignals = true);
    void rotateNormal( double, bool suspSignals = true);

    /** La denominzione del materiale all'interno del programma (Concrete, SteelCncr, etc)
    @return la denominazione */
    QString typeSectionNormal();

    /** La denominzione del materiale per l'utente (Calcestruzzo, Acciaio per c.a., etc)
    @return la denominazione */
    QString typeSection();

    /** Sezione in cls */
    MultiSectionCncrModel * sectionsCncr();
    /** Sezione in acciaio per c.a. */
    MultiSectionSteelCncrModel *sectionsSteelCncr();
    /** Sezione in acciaio da carpenteria */
    MultiSectionSteelModel *sectionsSteel();
    /** Sezione in FRP */
    MultiSectionFRPModel *sectionsFRP();

    double ANormal();
    double SyNormal();
    double SzNormal();
    double IyyNormal();
    double IzzNormal();
    double IyzNormal();
    double Sz3DyDzNormal();
    double Sy3DyDzNormal();
    double Syz2DyDzNormal();
    double Sy2zDyDzNormal();
    double pWNormal();

    void fSLSNormal( QList<double> *fMinCncr, QList<double> *fSteelCncr,
                     QList<double> *fMinSteel, QList<double> *fMaxSteel, QList<double> *fMinFRP, QList<double> *fMaxFRP,
                     double l, double my, double mz );

    void NMSLSNormal( double *NRet, double *MyRet, double *MzRet,
                      double l, double my, double mz, QList<Point2DModel *> *sects=NULL );

    /** Calcola la sezione allo SLE
     * @param l parametro di ritorno descrittivo dello stato deformativo
     * @param my parametro di ritorno descrittivo dello stato deformativo
     * @param mz parametro di ritorno descrittivo dello stato deformativo
     * @param N sforzo normale applicato
     * @param My momento My applicato
     * @param Mz momento Mz applicato
     * @param yN y del punto di applicazione dello sforzo normale N
     * @param zN z del punto di applicazione dello sforzo normale N
     * @param maxIter numero massimo di iterazioni
     * @param prec livello di precisione */
    bool NMSLSNormal( double *l, double *my, double *mz,
                      double N, double My, double Mz,
                      double yN, double zN,
                      int maxIter=5000, double prec=1.0e-4);

    /** Calcola la sezione allo SLE
     * @param l parametro di ritorno descrittivo dello stato deformativo
     * @param my parametro di ritorno descrittivo dello stato deformativo
     * @param mz parametro di ritorno descrittivo dello stato deformativo
     * @param N sforzo normale applicato
     * @param My momento My applicato
     * @param Mz momento Mz applicato
     * @param yN y del punto di applicazione dello sforzo normale N
     * @param zN z del punto di applicazione dello sforzo normale N
     * @param maxIter numero massimo di iterazioni
     * @param prec livello di precisione */
    bool NMSLS(DoublePlus * l, DoublePlus * my, DoublePlus * mz,
               DoublePlus * N, DoublePlus * My, DoublePlus * Mz,
               Point2DPlus *NCen,
               IntPlus * maxIter=NULL, DoublePlus * prec=NULL);

    /** Calcola lo stato deformativo della sezione sotto l'azione di (N, My, Mz) applicati in
     * (yM, zM)
     * @param l lambda sezione deformata come risultante dal calcolo
     * @param my my sezione deformata come risultante dal calcolo
     * @param mz mz sezione deformata come risultante dal calcolo
     * @return restituisce true se si raggiunge la convergenza, false altrimenti */
    bool NMULSNormal( double *l, double *my, double *mz,
                      double N, double My, double Mz,
                      double yN, double zN ,
                      int maxIter=500, double prec=1.0e-4);

    /** Calcola lo stato deformativo della sezione sotto l'azione di (N, My, Mz) applicati in
     * (yM, zM)
     * @param l lambda sezione deformata come risultante dal calcolo
     * @param my my sezione deformata come risultante dal calcolo
     * @param mz mz sezione deformata come risultante dal calcolo
     * @param phase fase di applicazione de carico
     * @return restituisce true se si raggiunge la convergenza, false altrimenti */
    bool NMULSNormal(QList<double> *l, QList<double> *my, QList<double> *mz,
                     int phase,
                     double N, double My, double Mz, double yN, double zN , int maxIter=200, double prec=1.0e-6);

    /** Calcola lo stato deformativo della sezione sotto l'azione di (N, My, Mz) applicati
     * in NCen
     * @param l lambda sezione deformata come risultante dal calcolo
     * @param my my sezione deformata come risultante dal calcolo
     * @param mz mz sezione deformata come risultante dal calcolo
     * @return restituisce true se si raggiunge la convergenza, false altrimenti */
    bool NMULS( DoublePlus * l, DoublePlus * my, DoublePlus * mz,
                DoublePlus * N, DoublePlus * My, DoublePlus * Mz, Point2DPlus * Ncen,
                IntPlus * maxIter=NULL, DoublePlus * prec=NULL);

    /** Calcola lo stato deformativo della sezione sotto l'azione di (N, My, Mz) applicati
     * in NCen
     * @param phase fase di applicazione del carico in cui si effettua il calcolo
     * @param l lambda sezione deformata come risultante dal calcolo
     * @param my my sezione deformata come risultante dal calcolo
     * @param mz mz sezione deformata come risultante dal calcolo
     * @return restituisce true se si raggiunge la convergenza, false altrimenti */
    bool NMULS(QList<DoublePlus *> * l, QList<DoublePlus *> *my, QList<DoublePlus *> * mz,
               SectionLoadPhase *phase, DoublePlus *N, DoublePlus *My, DoublePlus *Mz, Point2DPlus *Ncen,
               IntPlus * maxIter=NULL, DoublePlus * prec=NULL);

    /** massima sollecitazione di compressione nella sezione (con segno negativo) */
    double NULSMinNormal();
    /** massima sollecitazione di trazione nella sezione
     *  @param eps deformazione corrispondente */
    double NULSMaxNormal( double * eps = NULL );

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + m * y), unitÃ  di misura interne al programma
    * @param l
    * @param my
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    * @return lo sforzo normale allo stato limite ultimo (unitÃ  di misura interne del programma)
    */
    double NULSNormal( double l, double my, double mz,
                       QList<Point2DModel *> * sects = NULL );

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + m * y), unitÃ  di misura interne al programma
    * @param l
    * @param my
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    * @return lo sforzo normale allo stato limite ultimo (unitÃ  di misura interne del programma)
    */
    double NULSNormal(int phase,
                      QList<double> * l, QList<double> * my, QList<double> * mz,
                      QList<Point2DModel *> * sects = NULL );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unita'  di misura interne al programma
    * Il momento è valutato rispetto a (0, 0).
    * @param MyRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param MzRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    */
    void MULSNormal(double *MyRet, double * MzRet,
                    double l, double my, double mz,
                    QList<Point2DModel *> *sects = NULL );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unita'  di misura interne al programma
    * @param MyRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param MzRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param phase fase di carico in cui si effettua il calcolo
    * @param l
    * @param my
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    */
    void MULSNormal(double *MyRet, double * MzRet,
                    int phase,
                    QList<double> *l, QList<double> *my, QList<double> *mz,
                    QList<Point2DModel *> *sects = NULL );

    /** Momento resistente alla stato limite ultimo
    * @param MyRet puntatore alla variabile su cui viene scritta la componente x del momento allo SLU
    * @param MzRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param NSd sforzo normale agente
    * @param xCen ascissa punto attorno a cui si calcola il momento (udm interne)
    * @param yCen ordinata punto attorno a cui si calcola il momento (udm interne)
    * @param rot rotazione della sezione (in radianti
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    * @param accuracy accuratezza di calcolo del momento
    */
    bool MULSNormal(double * MyRet, double *MzRet,
                    double NEd,
                    double xCen, double yCen,
                    double rot,
                    QList<Point2DModel *> * sects = NULL,
                    double accuracy = 1.0e-7);

    /** Momento resistente alla stato limite ultimo
    * @param MyRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param MzRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param NSd sforzo normale agente
    * @param cen punto attorno a cui si calcola il momento (udm interne)
    * @param rot rotazione della sezione (in radianti)
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    * @param accuracy accuratezza di calcolo del momento
    */
    void MULS(DoublePlus * MyRet, DoublePlus * MzRet,
              DoublePlus * NSd,
              Point2DPlus * cen,
              DoublePlus * rot,
              QList<Point2DModel *> * sects = NULL,
              DoublePlus * accuracy = NULL);

    /** Diagramma di interazione
    * @param NSd sforzo normale agente
    * @param cen polo per calcolo momento
    * @param propNP numero punti diagramma
    * @return i punti del diagramma di interazione */
    QPolygonF MULSyMULSz( DoublePlus * NSd, Point2DPlus * cen, int propNP);

    /** Diagramma di interazione
    * @param NSd sforzo normale agente
    * @param cen polo per calcolo momento
    * @param propNP numero punti diagramma
    * @param rot rotazione della sezione
    * @return i punti del diagramma di interazione */
    QPolygonF MULSN( Point2DPlus * cen, DoublePlus * rot, int propNP);

    // **** GUI ****
    QGraphicsItem * qGraphicsItem();
};

#endif // SECTIONRCNCR_H
