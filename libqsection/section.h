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
#ifndef SECTION_H
#define SECTION_H


class SectionLoadPhaseModel;
class SectionLoadPhase;
class Point2DPlus;
class Point2DModel;
class BoolPlus;
class DoublePlus;
class UnitMeasure;

class QGraphicsItem;
class QString;
class QPolygonF;
template <typename T> class QList;

#include "section_export.h"
#include "sectionmodel.h"
#include "varpluscontainerplus.h"
#include <QObject>

/**
* @class Section
*
* @brief Classe usata per modellizzare una generica sezione
*
* Questa classe viene impiegata per modellizzare una generica sezione
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT Section: public VarPlusContainerPlus {
    Q_OBJECT

protected:
    /** materiale di cui è composta la sezione */
    Material * m_material;

    /** materiale di cui è composta la sezione */
    SectionLoadPhase * m_loadPhase;

    /** Inizializza tutte le variabili */
    void initVar();

    /** Aggiorna il valore dell'area della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    virtual void setA( bool emitAuto );
    /** Aggiorna il valore del momento statico rispetto all'asse y
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    virtual void setSy( bool emitAuto );
    /** Aggiorna il valore del momento statico rispetto all'asse z
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    virtual void setSz( bool emitAuto );
    /** Aggiorna il valore del momento di inerzia rispetto all'asse y sulla superficie della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    virtual void setIyy( bool emitAuto );
    /** Aggiorna il valore del momento di inerzia rispetto all'asse z sulla superficie della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    virtual void setIzz( bool emitAuto );
    /** Aggiorna il valore del momento di inerzia misto sulla superficie della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    virtual void setIyz( bool emitAuto );
    /** Aggiorna il valore dell'integrale di y^3 sulla superficie della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    virtual void setSy3DyDz( bool emitAuto );
    /** Aggiorna il valore dell'integrale di z^3 sulla superficie della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    virtual void setSz3DyDz( bool emitAuto );
    /** Aggiorna il valore dell'integrale di y^2z sulla superficie della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    virtual void setSy2zDyDz( bool emitAuto );
    /** Aggiorna il valore dell'integrale di yz^2 sulla superficie dellasezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    virtual void setSyz2DyDz( bool emitAuto );
    /** Aggiorna il valore del peso specifico unitario della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setPW( bool emitAuto );
    /** Aggiorna il valore del momento di inerzia rispetto all'asse X della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setIxx( bool emitAuto );
    /** Aggiorna il valore della y del baricentro della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setGy( bool emitAuto );
    /** Aggiorna il valore della z del baricentro della sezione
      * @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setGz( bool emitAuto );
    /** Aggiorna il valore del momento di inerzia baricentrico rispetto ad un asse parallelo
     *  all'asse y
     *  @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setIGyy( bool emitAuto );
    /** Aggiorna il valore del momento di inerzia baricentrico rispetto ad un asse parallelo
     *  all'asse z
     *  @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setIGzz( bool emitAuto );
    /** Aggiorna il valore del momento di inerzia baricentrico misto rispetto a due assi
     *  paralleli all'asse y e all'asse z
     *  @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setIGyz( bool emitAuto );
    /** Aggiorna il valore del momento di inerzia baricentrico rispetto ad un asse parallelo
     *  all'asse x
     *  @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setIGxx( bool emitAuto );
    /** Aggiorna il valore dell'angolo di inclinazione dell'asse pricipale di inerzia rispetto
     * all'asse y
     *  @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setThetaC( bool emitAuto );
    /** Aggiorna il valore di uno dei due momenti principali di inerzia
     *  @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setICyy( bool emitAuto );
    /** Aggiorna il valore di uno dei due momenti principali di inerzia
     *  @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setICzz( bool emitAuto );
    /** Aggiorna il valore del momento di inerzia centrifugo rispetto ai due assi princiapali di inerzia.
     * E' un calcolo che facciamo per verificare la correttezza degli altri parametri, perché per definizione
     * di assi principali di inerzia dovrebbe essere nullo
     *  @param emitAuto se vero, viene emesso il segnale valueChanged() */
    void setICyz( bool emitAuto );
    void setiy( bool emitAuto );
    void setiz( bool emitAuto );
    void settz( bool emitAuto );
    void setty( bool emitAuto );
    void settyz( bool emitAuto );
    void setIT(bool emitAuto);

public:
    /** Costruttore */
    Section( UnitMeasure * ump, Material * mat, const QString & n, QObject * parent=0 );
    /** Costruttore da attributi XML */
    Section(UnitMeasure * ump, const QXmlStreamAttributes & attrs, MaterialModel * mModel, SectionLoadPhaseModel *sModel, QObject * parent = 0);

    /** Distruttore */
    virtual ~Section();

    /** Overloading dell'operatore di assegnazione */
    virtual Section & operator=( Section &);

    /** Carica i valori degli attributi pubblici da un oggetto di tipo QXmlAttributes
    @param attrs gli attributi da analizzare */
    virtual void loadFromXML(const QXmlStreamAttributes &attrs, MaterialModel * mModel , SectionLoadPhaseModel *loadModel);

    /** Scrive la sezione su flusso XML */
    void writeXml(QXmlStreamWriter *writer);

    /** Legge da flusso XML i dati necessari per la sezione. Non vengono caricati gli attributi,
        che invece sono gestiti direttamente dal costruttore, ma si riferisce al caso di sezioni che
        ne contengono altre (SectionRCncr, SectionThin). Di default non fa nulla. */
    virtual void readXml( QXmlStreamReader * reader );

    /** La denominzione della sezione all'interno del programma (Section, SectionCncr, etc)
    @return la denominazione */
    virtual SectionSpace::SectionType sectionType();

    /** @return Restituisce il materiale di cui e' composta la sezione */
    Material * material();

    /** Imposta il materiale della sezione
      @param mat il nuovo materiale
      @param emitAuto emetti il segnale modelChanged */
    virtual void setMaterial( Material * mat, bool emitAuto = true );

    /** @return Restituisce il materiale di cui e' composta la sezione */
    SectionLoadPhase * loadPhase();

    /** Imposta il materiale della sezione
      @param mat il nuovo materiale
      @param emitAuto emetti il segnale modelChanged */
    void setLoadPhase( SectionLoadPhase * lp, bool emitAuto = true );

    /** Il nome del tipo di sezione visualizzato all'utente */
    QStringPlus * typeName;
    /** Applica o meno l'arrotondamento nel calcolo */
    BoolPlus * applyRoundOff;
    /** Peso specifico unitario */
    DoublePlus * pW;
    /** Area */
    DoublePlus * A;
    /** Momento statico rispetto all'asse y */
    DoublePlus * Sy;
    /** Momento statico rispetto all'asse z */
    DoublePlus * Sz;
    /** Momento di inerzia rispetto all'asse x (e.g. momento di inerzia polare) */
    DoublePlus * Ixx;
    /** Momento di inerzia rispetto all'asse y */
    DoublePlus * Iyy;
    /** Momento di inerzia rispetto all'asse z */
    DoublePlus * Izz;
    /** Momento centrifugo rispetto agli assi y e z */
    DoublePlus * Iyz;
    /** Integrale di y^3 sulla superficie della sezione */
    DoublePlus * Sy3DyDz;
    /** Integrale di z^3 sulla superficie della sezione */
    DoublePlus * Sz3DyDz;
    /** Integrale di y^2z sulla superficie della sezione */
    DoublePlus * Sy2zDyDz;
    /** Integrale di yz^2 sulla superficie della sezione */
    DoublePlus * Syz2DyDz;
    /** Baricentro della sezione */
    Point2DPlus * G;
    /** Raggio di inerzia - asse Y */
    DoublePlus * iy;
    /** Raggio di inerzia - asse Z */
    DoublePlus * iz;
    /** Momento di inerzia rispetto all'asse x nel baricentro (e.g. momento di inerzia polare rispetto al baricentro) */
    DoublePlus * IGxx;
    /** Momento di inerzia rispetto all'asse y nel baricentro*/
    DoublePlus * IGyy;
    /** Momento di inerzia rispetto all'asse z nel baricentro*/
    DoublePlus * IGzz;
    /** Momento di inerzia centrifugo rispetto agli assi y e z, nel baricentro*/
    DoublePlus * IGyz;
    /** Angolo di inclinazione dell'asse centrale di inerzia 1 */
    DoublePlus * thetaC;
    /** Momento centrale di inerzia - asse 1 */
    DoublePlus * ICyy;
    /** Momento centrale di inerzia - asse 2 */
    DoublePlus * ICzz;
    /** Momento centrifugo rispetto agli assi centrali di inerzia,
    * dovrebbe essere sempre nullo, lo calcoliamo per controllo */
    DoublePlus * ICyz;
    /** Fattore di taglio - asse Y */
    DoublePlus * ty;
    /** Fattore di taglio - asse Z */
    DoublePlus * tz;
    /** Fattore di taglio misto assi Y Z */
    DoublePlus * tyz;
    /** Modulo di rigidezza torsionale */
    DoublePlus * IT;

    /** Trasla la sezione, ricalcolando momenti statici, momenti di inerzia etc
    * @param trans componenti della traslazione */
    void translate( Point2DPlus * trans );

    /** Trasla la sezione, ricalcolando momenti statici, momenti di inerzia etc
    * @param yTrans componenti della traslazione
    * @param zTrans componenti della traslazione */
    virtual void translateNormal( double yTrans, double zTrans, bool suspSignals = true );

    /** Ruota la sezione attorno all'origine, ricalcolando i momenti statici, momenti di inerzia etc,
    * @param a angolo della rotazione */
    void rotate( DoublePlus * a );

    /** Ruota la sezione attorno all'orogine, ricalcolando i momenti statici, momenti di inerzia etc,
    * @param rot angolo della rotazione */
    virtual void rotateNormal(double rot , bool suspSignals = true );

    virtual double ANormal();
    virtual double SyNormal();
    virtual double SzNormal();
    virtual double IyyNormal();
    virtual double IzzNormal();
    virtual double IyzNormal();
    virtual double IxxNormal();
    virtual double Sy3DyDzNormal();
    virtual double Sz3DyDzNormal();
    virtual double Sy2zDyDzNormal();
    virtual double Syz2DyDzNormal();
    virtual double IGyyNormal();
    virtual double IGzzNormal();
    virtual double IGyzNormal();
    virtual double IGxxNormal();
    virtual double GyNormal();
    virtual double GzNormal();
    virtual double thetaCNormal();
    virtual double ICyyNormal();
    virtual double ICzzNormal();
    virtual double ICyzNormal();
    virtual double ICxxNormal();
    virtual double iyNormal();
    virtual double izNormal();
    virtual double tyNormal();
    virtual double tzNormal();
    virtual double tyzNormal();
    virtual double ITNormal();
    virtual double pWNormal();

    /** @return il momento statico rispetto all'asse y a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a */
    double SyTransRotNormal( double yTrans, double zTrans, double a );
    /** @return il momento statico rispetto all'asse z a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a */
    double SzTransRotNormal( double yTrans, double zTrans, double a );
    /** @return il momento di interzia rispetto all'asse y a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a */
    double IyyTransRotNormal( double yTrans, double zTrans, double rot);
    /** @return il momento di interzia rispetto all'asse z a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a */
    double IzzTransRotNormal( double yTrans, double zTrans, double a );
    /** @return il momento di interzia centrifugo rispetto agli assi y e z a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a */
    double IyzTransRotNormal( double yTrans, double zTrans, double a );
    /** @return l'integrale ∬ z3 dA a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a */
    double Sz3DyDzTransRotNormal( double yTrans, double zTrans, double a );
    /** @return l'integrale ∬ y3 dA a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a */
    double Sy3DyDzTransRotNormal( double yTrans, double zTrans, double a );
    /** @return l'integrale ∬ y z2 dA a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a */
    double Syz2DyDzTransRotNormal( double yTrans, double zTrans, double a );
    /** @return l'integrale ∬ y2 z dA a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a */
    double Sy2zDyDzTransRotNormal( double yTrans, double zTrans, double a );

    /** Calcola il momento statico rispetto all'asse y a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a
     * e copia il valore in ret */
    void SyTransRot( DoublePlus * ret, Point2DPlus * trans = NULL, DoublePlus * a = NULL, bool emitAuto = true  );
    /** Calcola il momento statico rispetto all'asse z a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a
     * e copia il valore in ret */
    void SzTransRot( DoublePlus * ret, Point2DPlus * trans = NULL, DoublePlus * a = NULL, bool emitAuto = true  );
    /** Calcola il momento di interzia rispetto all'asse y a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a
     * e copia il valore in ret */
    void IyyTransRot( DoublePlus * ret, Point2DPlus * trans = NULL, DoublePlus * a = NULL, bool emitAuto = true );
    /** Calcola il momento di interzia rispetto all'asse z a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a
     * e copia il valore in ret */
    void IzzTransRot( DoublePlus * ret, Point2DPlus * trans = NULL, DoublePlus * a = NULL, bool emitAuto = true );
    /** Calcola il momento di interzia centrifugo rispetto agli assi y e z a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a
     * e copia il valore in ret */
    void IyzTransRot( DoublePlus * ret, Point2DPlus * trans = NULL, DoublePlus * a = NULL, bool emitAuto = true );
    /** Calcola l'integrale ∬ z3 dA a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a
     * e copia il valore in ret */
    void Sz3DyDzTransRot( DoublePlus * ret, Point2DPlus * trans = NULL, DoublePlus * a = NULL, bool emitAuto = true );
    /** Calcola l'integrale ∬ y3 dA a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a
     * e copia il valore in ret */
    void Sy3DyDzTransRot( DoublePlus * ret, Point2DPlus * trans = NULL, DoublePlus * a = NULL, bool emitAuto = true );
    /** Calcola l'integrale ∬ y z2 dA a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a
     * e copia il valore in ret */
    void Syz2DyDzTransRot( DoublePlus * ret, Point2DPlus * trans = NULL, DoublePlus * a = NULL, bool emitAuto = true );
    /** Calcola l'integrale ∬ y2 z dA a seguito, nell'ordine,
     * di una traslazione di componenti (yTrans, zTrans) e una rotazione di angolo a
     * e copia il valore in ret */
    void Sy2zDyDzTransRot( DoublePlus * ret, Point2DPlus * trans = NULL, DoublePlus * a = NULL, bool emitAuto = true );

    /** Snellezza trave
    @param Ly lunghezza libera di inflessione, asse y-y
    @param Lz lunghezza libera di inflessione, asse z-z
    @return snellezza massima */
    virtual double beamlBucklNormal( double Ly, double Lz );

    /** Carico critico euleriano trave, asse y-y
    @param Ly lunghezza libera di inflessione, asse y-y
    @param Lz lunghezza libera di inflessione, asse z-z
    @return snellezza massima */
    virtual double beamNCrNormal( double Ly, double Lz );

    /** Snellezza trave, compressione, asse y-y
    @param Ly lunghezza libera di inflessione, asse y-y
    @param Lz lunghezza libera di inflessione, asse z-z */
    void beamlBuckl( DoublePlus * ret, DoublePlus * Ly, DoublePlus * Lz );

    /** Carico critico euleriano trave, asse y-y
    @param Ly lunghezza libera di inflessione, asse y-y
    @param Lz lunghezza libera di inflessione, asse z-z */
    void beamNCr( DoublePlus * ret, DoublePlus * Ly, DoublePlus * Lz );

    /**
    * Calcola lo sforzo normale allo SLE con deformazione data (deformazione piana e = l + m * z), unità di misura interne al programma
    * @param l
    * @param my
    * @param mz
    * @param sects sezioni reagenti
    * @return lo sforzo normale allo stato limite di esercizio con deformazione data di parametri l, my e mz (unità di misura interne al programma)
    */
    virtual double NSLSNormal(double l, double my, double mz, QList<Point2DModel *> *sectsOut = NULL );

    /**
    * Momento allo SLE con deformazione data (deformazione piana e = l + my * z), unità di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param mz
    * @param sects sezioni reagenti
    * @return il momento allo stato limite di esercizio con deformazione data di parametri l, my e mz (unità di misura interne al programma)
    */
    virtual void MSLSNormal(double *yRet, double * zRet, double l, double my, double mz, QList<Point2DModel *> *sects = NULL);

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + m * z), unità di misura interne al programma
    * @param l
    * @param my
    * @param sects sezioni reagenti
    * @return lo sforzo normale allo stato limite ultimo (unità di misura interne del programma)
    */
    virtual double NULSNormal(double l, double my, QList<Point2DModel *> * sects = NULL);

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + m * z)
    * @param ret il puntatore alla variabile su cui viene scritto lo sforzo normale allo SLU
    * @param l
    * @param m
    * @param sects sezioni reagenti
    */
    void NULS( DoublePlus * ret, double l, double m, QList<Point2DModel *> * sects = NULL);

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z), unità di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param sects sezioni reagenti
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e my (unità di misura interne al programma)
    */
    virtual void MULSNormal(double *yRet, double * zRet, double l, double my, QList<Point2DModel *> *sects = NULL);

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + m * z)
    * @param l
    * @param m
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m
    */
    void MULS( DoublePlus * yRet, DoublePlus * zRet, double l, double m, QList<Point2DModel *> * sects = NULL);

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + m * z), unità di misura interne al programma
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo con deformazione data di parametri l, my e mz (unità di misura interne al programma)
    */
    virtual double NULSNormal(double l, double my, double mz, QList<Point2DModel *> * sects = NULL);

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z), unità di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente z del momento allo SLU
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return il momento allo stato limite ultimo con deformazione data di parametri l, my e mz (unità di misura interne al programma)
    */
    virtual void MULSNormal(double *yRet, double * zRet, double l, double my, double mz, QList<Point2DModel *> *sects = NULL);

    /** Diagramma di interazione
    * @param NSd sforzo normale agente
    * @param cen polo per calcolo momento
    * @param propNP numero punti diagramma
    * @return i punti del diagramma di interazione */
    virtual QPolygonF MULSyMULSz( DoublePlus * NSd, Point2DPlus * cen, int propNP);

    /** Diagramma di interazione
    * @param NSd sforzo normale agente
    * @param cen polo per calcolo momento
    * @param rot rotazione
    * @param propNP numero punti diagramma
    * @return i punti del diagramma di interazione */
    virtual QPolygonF MULSN( Point2DPlus * cen, DoublePlus * rot, int propNP);


    /** Restituisce l'oggetto grafico che rappresenta la sezione
    Se non c'e' oggetto grafico specifico restituisce NULL */
    virtual QGraphicsItem * qGraphicsItem();

    /** Funzione per arrotondare i momenti */
    double MRoundOff( double, double );

    /** Aggiorna l'oggetto grafico legato alla sezione.
        Di default questo metodo non fa nulla. */
    virtual void updateQGraphics();

    /** Fase di carico (valore usato internamente dal programma range 0...) */
    int internalLoadPhase();

public slots:
    /** Trasla l'origine del sistema di riferimento nel baricentro della sezione. */
    void GTranslate();

    /** Sposta il sistema di riferimento nel sistema centrale di inerzia della sezione.
        In pratica trasla l'origine nel baricentro della sezione e ruota gli assi secondo
        le direzioni centrali di inerzia. */
    void PTransRot();

signals:
    /** Segnale emesso quando la sezione cambia.
        Questo segnale serve a comunicare alle classi esterne che la sezione è cambiata. */
    void sectionChanged();

    /** Segnale emesso quanto cambia il mteriale della sezione
        @param sectionChangingMaterial puntatore alla sezione (this)
        @param oldMaterial il nuovo materiale
        @param newMaterial il uovo materiale */
    void materialChanged( Material * oldMaterial, Material * newMaterial );

    /** Segnale emesso quanto cambia la fase di inserimento della sezione */
    void loadPhaseChanged();

    /** Segnale emesso se intervengono cambiamenti nella grafica della sezione */
    void graphicsChanged();

protected slots:
    void setMaterialNULL();
    void setLoadPhaseNULL();
};

#endif // SECTION_H
