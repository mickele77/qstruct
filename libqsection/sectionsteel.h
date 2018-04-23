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
#ifndef SECTIONSTEEL_H
#define SECTIONSTEEL_H

class SectionSteelPrivate ;

class SectionSteelManufactType;
class SectionSteelShape;
class SectionSteelMomentDistribution;
class SectionSteelTorsionalDeformability;

class Steel;
class Point2DPlusModel;
class QStringPlus;

class QGraphicsItemGroup;

class QPolygonF;

#include "section.h"

/**
* @class SectionSteel
*
* @brief Classe usata per modellizzare una sezione d'acciaio
*
* Questa classe viene impiegata per modellizzare una sezione composta di acciaio
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionSteel : public Section
{
    Q_OBJECT
public:
    enum MomentDistribution{
        momentLinear = 0,
        momentBiLinear = 1,
        momentParabolic = 2
    };
    enum ManufactType{
        sectionRolled,
        sectionWelded,
        sectionColdFormed,
        sectionManufactTypeOther
    };
    enum SectionShape{
        sectionI,
        sectionL,
        sectionCirc,
        sectionRect,
        sectionOmega
    };
    enum TorsionalDeformability{
        torsDeformable,
        torsNotDeformable
    };

    /** Costruttore */
    explicit SectionSteel( UnitMeasure * um, Steel * mat, SectionSteel::ManufactType manType, SectionSteel::SectionShape sectShape, const QString & n, QObject * parent=0 );
    explicit SectionSteel(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent = 0 );

    /** Distruttore */
    ~SectionSteel();

    /** Sospende l'emissione dei segnali delle variabili contenute */
    void suspendSignals( bool susp = true );

    QString typeSection();

    /** L'acciaio di cui si compone la sezione. Se alla sezione è stato erronamente
        attribuito un materiale che non è acciaio, restituisce il puntatore nullo. */
    Steel * steel();
    /** Imposta il materiale di cui è composta la sezione. Il materiale deve essere
        un acciaio, altrimenti il metodo non produce risutlati */
    void setMaterial( Material *, bool emitAuto = true );
    /** Imposta il materiale di cui è composta la sezione. */
    void setSteel( Steel *, bool emitAuto = true );

    /** Numero dei parametri necessari per descrivere la tipologia di sezione scelta */
    int np();
    /** Numero massimo dei parametri necessari per descrivere la tipologia di sezione scelta */
    static int npMax();
    /** Parametro i-esimo per descrivere la tipologia di sezione scelta
        @paramt numero del parametro
        @return puntatore al parametro */
    DoublePlus * p( int i );

    /** Tipologia realizzativa della sezione */
    SectionSteelManufactType * manufactType;
    /** Forma della sezione */
    SectionSteelShape * sectionShape;

    /** traslazione della sezione */
    Point2DPlus * translationO;
    /** Rotazione della sezione rispetto al baricentro */
    DoublePlus * rotationO;

    /** Costante di ingobbamento */
    DoublePlus * Iw;

    /** Modulo resistente flessionale elastico, asse y-y */
    DoublePlus * WElMy;
    /** Modulo resistente flessionale elastico, asse z-z */
    DoublePlus * WElMz;
    /** Modulo resistente flessionale plastico, asse y-y */
    DoublePlus * WPlMy;
    /** Modulo resistente flessionale plastico, asse z-z */
    DoublePlus * WPlMz;

    /** Classificazione della sezione per sforzo normale */
    QStringPlus * NClass;
    /** Classificazione della sezione per momento positivo lungo asse y */
    QStringPlus * MyMaxClass;
    /** Classificazione della sezione per momento negativo lungo asse y */
    QStringPlus * MyMinClass;
    /** Carico limite per applicazione classe flessione pura y-y (6.2.9.1 (4) dell'EC3) */
    DoublePlus * MyClassLimit;
    /** Classificazione della sezione per momento positivo lungo asse z */
    QStringPlus * MzMaxClass;
    /** Classificazione della sezione per momento negativo lungo asse z */
    QStringPlus * MzMinClass;
    /** Carico limite per applicazione classe flessione pura y-y (6.2.9.1 (4) dell'EC3) */
    DoublePlus * MzClassLimit;

    /** Area efficace in presenza di sforzo normale di compressione puro allo SLU */
    DoublePlus * AEffN;
    /** Baricentro della sezione efficace in presenza di sforzo normale di compressione puro allo SLU */
    Point2DPlus * GEffN;
    /** Modulo resistente massimo della sezione efficace, asse y-y */
    DoublePlus * WEffMyMax;
    /** Modulo resistente minimo della sezione efficace, asse y-y */
    DoublePlus * WEffMyMin;
    /** Modulo resistente massimo della sezione efficace, asse z-z */
    DoublePlus * WEffMzMax;
    /** Modulo resistente minimo della sezione efficace, asse z-z */
    DoublePlus * WEffMzMin;
    /** Area resistente a taglio agente lungo asse y-y */
    DoublePlus * AVy;
    /** Area resistente a taglio agente lungo asse z-z */
    DoublePlus * AVz;

    /** Sforzo normale resistente massimo allo SLU */
    DoublePlus * NULSMax;
    /** Sforzo normale minimo resistente allo SLU */
    DoublePlus * NULSMin;
    /** Momento resistente massimo allo SLU, asse y-y */
    DoublePlus * MULSyMax;
    /** Momento resistente minimo allo SLU, asse y-y */
    DoublePlus * MULSyMin;
    /** Momento resistente massimo allo SLU, asse z-z */
    DoublePlus * MULSzMax;
    /** Momento resistente minimo allo SLU, asse z-z */
    DoublePlus * MULSzMin;
    /** Taglio resistente massimo in modulo allo SLU, asse y-y */
    DoublePlus * VULSy;
    /** Taglio resistente massimo in modulo allo SLU, asse z-z */
    DoublePlus * VULSz;

    /** @return area della sezione */
    double ANormal();
    /** @return momento statico rispetto asse y-y della sezione */
    double SyNormal();
    /** @return momento statico rispetto asse z-z della sezione */
    double SzNormal();
    /** @return momento di inerzia rispetto asse y-y della sezione */
    double IyyNormal();
    /** @return momento di inerzia rispetto asse z-z della sezione */
    double IzzNormal();
    /** @return momento di inerzia centrifugo della sezione */
    double IyzNormal();
    /** @return integrale di z^3 sulla sezione */
    double Sz3DyDzNormal();
    /** @return integrale di y^3 sulla sezione */
    double Sy3DyDzNormal();
    /** @return integrale di y*z^2 sulla sezione */
    double Syz2DyDzNormal();
    /** @return integrale di y^2*z sulla sezione */
    double Sy2zDyDzNormal();
    /** @return coefficiente di taglio asse y */
    double tyNormal();
    /** @return coefficiente di taglio asse z */
    double tzNormal();
    /** @return coefficiente di taglio assi y e z*/
    double tyzNormal();

    /** Resistenza a rottura della sezione netta in corrispondenza dei fori per il collegamento */
    double NNetULSMaxNormal( double ANet );

    /** Tensione massima con segno nella sezione in presenza di sforzo normale e flessione deviata
    * @param N sforzo normale
    * @param My momento piano xy
    * @param Mz momento piano xz
    * @return la tensione massima con segno (positiva in presenza di trazione, negativa in presenza di compressione */
    double fElNormal( double N, double My, double Mz);

    /** Verifica allo SLU della sezione */
    bool verifyULSNormal( double lRelY,
                          double chiY,
                          double lRelZ,
                          double chiZ,
                          double N,
                          SectionSteel::TorsionalDeformability torsDef,
                          double chiLT,
                          SectionSteel::MomentDistribution myd,
                          double MyH1,
                          double MyS,
                          double MyH2,
                          SectionSteel::MomentDistribution mzd,
                          double MzH1,
                          double MzS,
                          double MzH2,
                          SectionSteel::MomentDistribution mltd,
                          double MltH1,
                          double MltS,
                          double MltH2,
                          double yCen,
                          double zCen,
                          double My,
                          double Mz,
                          double Vy,
                          double Vz,
                          double * res,
                          QString * clString = NULL,
                          QString * messages = NULL,
                          bool calcDomMyMz = false );

    /** Verifica allo SLU della sezione */
    bool verifyULS( DoublePlus * lRelY,
                    DoublePlus * chiY,
                    DoublePlus * lRelZ,
                    DoublePlus * chiZ,
                    DoublePlus * N,
                    SectionSteelTorsionalDeformability * torsDef,
                    DoublePlus * chiLT,
                    SectionSteelMomentDistribution * myd,
                    DoublePlus * MyH1,
                    DoublePlus * MyS,
                    DoublePlus * MyH2,
                    SectionSteelMomentDistribution * mzd,
                    DoublePlus * MzH1,
                    DoublePlus * MzS,
                    DoublePlus * MzH2,
                    SectionSteelMomentDistribution * mltd,
                    DoublePlus * MltH1,
                    DoublePlus * MltS,
                    DoublePlus * MltH2,
                    Point2DPlus * cen,
                    DoublePlus * My,
                    DoublePlus * Mz,
                    DoublePlus * Vy,
                    DoublePlus * Vz,
                    DoublePlus * res,
                    QString * clString = NULL,
                    QString * messages = NULL );

    /** Snellezza relativa minima */
    double beamlRelBuckl0Normal();

    /** Fattore di imperfezione, asse y-y */
    double alphaBucklYNormal();
    /** Fattore di imperfezione, asse z-z */
    double alphaBucklZNormal();
    /** Snellezza relativa trave, asse y-y */
    double beamlRelBucklNormal( double NCrInput );

    /** Fattore di riduzione per instabilita' compressione, asse y-y */
    double beamChiBucklYNormal( double lRelY );
    /** Fattore di riduzione per instabilita' compressione, asse z-z */
    double beamChiBucklZNormal( double lRelZ );

    /** Compressione massima per instabilita' a compressione
    param chiY fattore di riduzione */
    double beamNULSBucklNormal( double chi );

    /** Snellezza relativa trave, compressione, piano y-y */
    void beamlRelBuckl( DoublePlus * ret, DoublePlus * NCrInput );

    /** Fattore di riduzione per instabilita' compressione, asse y-y */
    void beamChiBucklY( DoublePlus * ret, DoublePlus * lRelY );
    /** Fattore di riduzione per instabilita' compressione, asse z-z */
    void beamChiBucklZ( DoublePlus * ret, DoublePlus * lRelZ );

    /** Compressione massima per instabilita' a compressione
    param chiY fattore di riduzione */
    void beamNULSBuckl( DoublePlus * ret, DoublePlus * chi );

    /** Fattore di imperfezione per instabilita' flessotorsionale */
    double alphaBucklLTNormal();
    /** Momento critico instabilita' flessotorsionale */
    double beamMCrLTNormal( double Ly, double MH1, double MH2 );
    /** Snellezza relativa isntabilita' flessotorsionale */
    double beamlRelBucklLTNormal( double MCrInput );
    /** Snellezza relativa minima instabilita' flessotorsionale */
    double beamlRelBucklLT0Normal();
    /** Fattore di riduzione instabilita' flessotorsionale */
    double beamChiBucklLTNormal( double MH1, double MH2, SectionSteel::MomentDistribution md , double lRelY );
    /** Momento massimo per instabilita' flessotorsionale
    param Ly distanza ritegni torsionali secondo asse y */
    double beamMULSBucklLTMaxNormal( double chiY );
    /** Momento minimo per instabilita' flessotorsionale
    param Ly distanza ritegni torsionali secondo asse y */
    double beamMULSBucklLTMinNormal( double chiY );

    /** Momento critico instabilita' flessotorsionale */
    void beamMCrLT( DoublePlus *ret, DoublePlus * Ly, DoublePlus * MH1, DoublePlus * MH2 );
    /** Snellezza relativa isntabilita' flessotorsionale */
    void beamlRelBucklLT( DoublePlus *ret, DoublePlus * MCrInput );
    /** Snellezza relativa minima instabilita' flessotorsionale */
    void beamlRelBucklLT0( DoublePlus *ret );
    /** Fattore di riduzione instabilita' flessotorsionale */
    void beamChiBucklLT( DoublePlus * ret, DoublePlus * MH1, DoublePlus * MH2, SectionSteelMomentDistribution * md , DoublePlus * lRelY );
    /** Momento massimo per instabilita' flessotorsionale
        @param chiY fattore di riduzione */
    void beamMULSBucklLTMax( DoublePlus * ret, DoublePlus * chiY );
    /** Momento minimo per instabilita' flessotorsionale
        @param chiY fattore di riduzione */
    void beamMULSBucklLTMin( DoublePlus * ret, DoublePlus * chiY );

    /** Ordinata del diagramma delle classi (vedi Rugarli) */
    double Theta( double N, double My, double Mz);
    /** Ascissa del diagramma delle classi (vedi Rugarli) */
    double xi( double My, double Mz);
    /** Restituisce il diagramma delle classi
        @return una lista di poligoni (QPolygonF) chiusi corrispondente al dominio di ciasuna classe */
    QList<QPolygonF *> * classDiagram();

    /**
    Classe della sezione per una sollecitazione composta
    @param N sforzo normale agente
    @param My componente y del momento
    @param Mz componente z del momento
    @param yCen coordinata y del polo attorno a cui si calcola il momento
    @param zCen coordinata z del polo attorno a cui si calcola il momento
    @param simpleCalc se la determinazione della classe avviene o no in maniera semplificata, vale a dire se si impiega il diagramma delle classi o meno
    @return la classe della sezione per la sollecitazione composta data */
    int classNormal( double N, double My, double Mz, double yCen = 0.0, double zCen = 0.0, bool simpleCalc = false );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + m * y), unità di misura interne al programma
    * @param WyRet puntatore alla variabile su cui viene scritta la componente x del momento allo SLU
    * @param WzRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param l
    * @param my
    * @param mz
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    */
    void WNMULSPlastNormal( double *WNRet, double *WMyRet, double * WMzRet,
                            double l, double my, double mz,
                            double yCen, double zCen,
                            QList<Point2DPlusModel *> * sects = NULL );

    /**
    * Sforzo normale e momento allo SLU con deformazione data (deformazione piana e = l + m * y), unità di misura interne al programma
    * @param NRet puntatore alla variabile su cui viene scritto lo sforzo normale allo SLU
    * @param MyRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param MzRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l deformazione assiale in (0,0)
    * @param my curvatura
    * @param mz curvatura
    */
    void NMULSPlastNormal( double * NRet, double *MyRet, double * MzRet,
                           double l, double my, double mz,
                           double yCen, double zCen);

    /**
    * Calcola momento e sforzo normale allo SLU con deformazione data (deformazione piana e = l + m * y), unità di misura interne al programma.
    * Nel calcolo considera un comportamento elasto-perfettamente plastico del materiale.
    * @param NRet puntatore alla variabile su cui viene scritto lo sforzo normale allo SLU
    * @param MyRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param MzRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l deformazione assiale in (0,0)
    * @param my curvatura
    * @param mz curvatura
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    */
    void NMULSElastPlastNormal( double * NRet, double *MyRet, double * MzRet,
                                double l, double my, double mz,
                                double yCen, double zCen );

    /** Momento resistente alla stato limite ultimo
    * @param xRet puntatore alla variabile su cui viene scritta la componente x del momento allo SLU
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param NSd sforzo normale agente
    * @param xCen ascissa punto attorno a cui si calcola il momento (udm interne)
    * @param yCen ordinata punto attorno a cui si calcola il momento (udm interne)
    * @param rot rotazione della sezione (in radianti)
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    * @param accuracy accuratezza di calcolo del momento
    */
    void MULSNormal( double * MyRet, double *MzRet,
                     double NSd,
                     double yCen, double zCen,
                     double rot,
                     double accuracy = 1.0e-6);

    /** Momento resistente alla stato limite ultimo
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param NSd sforzo normale agente
    * @param cen punto attorno a cui si calcola il momento (udm interne)
    * @param rot rotazione della sezione (in radianti
    * @param sects elenco delle sezioni definite nel corso del calcolo ed utili al fine della verifica del risultato
    * @param accuracy accuratezza di calcolo del momento
    */
    void MULS( DoublePlus * yRet, DoublePlus *zRet,
               DoublePlus * NSd,
               Point2DPlus * cen,
               DoublePlus * rot,
               DoublePlus * accuracy = NULL);

    /** Diagramma di interazione plastico
    * @param NSd sforzo normale agente
    * @param yCen y del polo per calcolo momento
    * @param zCen y del polo per calcolo momento
    * @param propNP numero punti diagramma
    * @return i punti del diagramma di interazione */
    QPolygonF MULSyMULSzPlastNormal( double NSd, double yCen, double zCen, int propNP);

    /** Diagramma di interazione plastico
    * @param NSd sforzo normale agente
    * @param cen polo per calcolo momento
    * @param propNP numero punti diagramma
    * @return i punti del diagramma di interazione */
    QPolygonF MULSyMULSzPlast( DoublePlus * NSd, Point2DPlus * cen, int propNP);

    /** Diagramma di interazione elastico
    * @param NSd sforzo normale agente
    * @param yCen y del polo per calcolo momento
    * @param zCen y del polo per calcolo momento
    * @param propNP numero punti diagramma
    * @return i punti del diagramma di interazione */
    QPolygonF MULSyMULSzElastNormal( double NSd, double yCen, double zCen, int propNP);

    /** Diagramma di interazione elastico
    * @param NSd sforzo normale agente
    * @param cen polo per calcolo momento
    * @param propNP numero punti diagramma
    * @return i punti del diagramma di interazione */
    QPolygonF MULSyMULSzElast( DoublePlus * NSd, Point2DPlus * cen, int propNP);

    /** Diagramma di interazione
    * @param NSd sforzo normale agente
    * @param cen polo per calcolo momento
    * @param propNP numero punti diagramma
    * @return i punti del diagramma di interazione */
    QPolygonF MULSyMULSz( DoublePlus * NSd, Point2DPlus * cen, int propNP);

    /** Diagramma di interazione
    * @param NSd sforzo normale agente
    * @param cen polo per calcolo momento
    * @param rot rotazione
    * @param propNP numero punti diagramma
    * @return i punti del diagramma di interazione */
    QPolygonF MULSN( Point2DPlus * cen, DoublePlus * rot, int propNP);

    /** Restituisce l'oggetto grafico associato alla sezione
        @return l'oggetto grafico associato alla sezione */
    QGraphicsItem * qGraphicsItem();

    /** Carica gli attributi XML. */
    void loadFromXML(const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel *loadModel);

    double NULSNormal(double l, double my, QList<Point2DModel *> *sects = NULL);
    void MULSNormal(double *yRet, double *zRet, double l, double my, QList<Point2DModel *> *sects = NULL);

    double NULSNormal(double l, double my, double mz, QList<Point2DModel *> *sects = NULL);
    void MULSNormal(double *yRet, double *zRet, double l, double my, double mz, QList<Point2DModel *> *sects = NULL);

    /** Restituisce la costante torsionale */
    double ITNormal();

    /** Calcola la costante di ingobbamento */
    void setIw( bool emitAuto );
    /** Calcola il modulo resistente flessionale elastico massimo lungo l'asse y-y */
    void setWElMy( bool emitAuto );
    /** Calcola il modulo resistente flessionale elastico massimo lungo l'asse z-z */
    void setWElMz( bool emitAuto );
    /** Calcola il modulo resistente flessionale plastico lungo l'asse y-y */
    void setWPlMy( bool emitAuto );
    /** Calcola il modulo resistente flessionale plastico lungo l'asse z-z */
    void setWPlMz( bool emitAuto );
    /** Calcola area e baricentro della sezione efficace in presenza di sforzo
        normale di compressione */
    void setEffN( bool emitAuto );
    /** Calcola il modulo resistente flessionale efficace massimo lungo l'asse y-y */
    void setWEffMyMax( bool emitAuto );
    /** Calcola il modulo resistente flessionale efficace minimo lungo l'asse y-y */
    void setWEffMyMin( bool emitAuto );
    /** Calcola il modulo resistente flessionale efficace massimo lungo l'asse z-z */
    void setWEffMzMax( bool emitAuto );
    /** Calcola il modulo resistente flessionale efficace minimo lungo l'asse z-z */
    void setWEffMzMin( bool emitAuto );
    /** Calcola l'area resistente a talgio lungo l'asse y-y */
    void setAVy( bool emitAuto );
    /** Calcola l'area resistente a talgio lungo l'asse z-z */
    void setAVz( bool emitAuto );

    /** Calcola la classe della sezione per sforzo di compressione */
    void setNClass( bool emitAuto );
    /** Calcola la classe della sezione per flessione pura positiva lungo l'asse y-y */
    void setMyMaxClass( bool emitAuto );
    /** Calcola la classe della sezione per flessione pura negativa lungo l'asse y-y */
    void setMyMinClass( bool emitAuto );
    void setMyClassLimit( bool emitAuto );
    /** Calcola la classe della sezione per flessione pura positiva lungo l'asse z-z */
    void setMzMaxClass( bool emitAuto );
    /** Calcola la classe della sezione per flessione pura negativa lungo l'asse z-z */
    void setMzMinClass( bool emitAuto );
    void setMzClassLimit( bool emitAuto );

    /** Calcola la resistenza massima a trazione della sezione */
    void setNULSMax( bool emitAuto );
    /** Calcola la resistenza massima a compressione della sezione */
    void setNULSMin( bool emitAuto );
    /** Calcola il momento resistente massimo della sezione lungo l'asse y-y */
    void setMULSyMax( bool emitAuto );
    /** Calcola il momento resistente minimo della sezione lungo l'asse y-y */
    void setMULSyMin( bool emitAuto );
    /** Calcola il momento resistente massimo della sezione lungo l'asse z-z */
    void setMULSzMax(bool emitAuto);
    /** Calcola il momento resistente minimo della sezione lungo l'asse z-z */
    void setMULSzMin( bool emitAuto );
    /** Calcola la resistenza a talgio massima a SLU lungo l'asse y-y */
    void setVULSy( bool emitAuto );
    /** Calcola la resistenza a talgio massima a SLU lungo l'asse z-z */
    void setVULSz( bool emitAuto );

    double yMaxNormal();
    double yMinNormal();
    double zMaxNormal();
    double zMinNormal();
    bool sMinNormal(double *ret, double my, double mz);
    bool sMaxNormal(double *ret, double my, double mz);

    void updateQGraphics();

public slots:
    void updateClassDiagram();


private:
    SectionSteelPrivate * m_d;

    /** Inizializza le variabili */
    void initVar( SectionSteel::ManufactType manType = sectionRolled, SectionSteel::SectionShape sectType = sectionI );

    /** Tensione caratteristica della sezione (tiene conto dello spessore massimo della sezione)
    @return il valore della tensione carattersitica */
    double fykNormal();

    /** Tensione di progetto della sezione (tiene conto dello spessore massimo della sezione)
    @return il valore della tensione di progetto */
    double fydNormal( int );

    /** Coefficienti tabella B.3 EC3 */
    double Cm( SectionSteel::MomentDistribution md, double MH1, double MS, double MH2 );

    /** Imposta i valori delle variabili readOnly. Di default il metodo è vuoto.
        Dopo aver caricato i valori da file viene usato per verificare le grandezze
        dichiarate auto. */
    void updateReadOnlyVars();

    /** Annulla la possibilità di traslare la sezione. Occorre usare trans */
    virtual void translateNormal(double yTrans, double zTrans, bool suspSignals= true);

    /** Annulla la possibilità di traslare la sezione. Occorre usare rotate */
    virtual void rotateNormal( double rot, bool suspSignals= true);


    void calculateSects( const double &l, const double &my,
                         const double &epsPlus, const double &epsMinus,
                         Point2DModel *sectPlus, Point2DModel *sectEl, Point2DModel *sectMinus);

    int where(double z, double zPlus, double zMinus);


private slots:
    void updateSectionSteelProfile();
    void updateSectionPoints();
};

#endif // SECTIONSTEEL_H
