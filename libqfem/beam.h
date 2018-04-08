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
#ifndef BEAM_H
#define BEAM_H

#include "element.h"

class Vertex;
class Section;
class BeamPrivate;
class DoublePlus;
class BeamFModel;
class BeamQModel;
class BeamType;

/**
* @class Beam
*
* @brief Classe usata per rappresentare l'elemento Beam
*
* Questa classe viene usata per rappresentare l'elemento finito Beam
*
* @author Michele Mocciola
*/

class Beam: public Element {
    Q_OBJECT
public:
    enum Type {
        Bernoulli,
        Timoshenko,
        Winkler
    };
    /** Costruttore
        @param iidd l'id della trave
        @param sect la sezione
        @param vert1 il primo vertice
        @param vert2 il secondo vertice
        @param ump unità di misura impiegate
        @param parent oggetto genitore */
    Beam( unsigned int iidd,
          Section * sect,
          Vertex * vert1,
          Vertex * vert2,
          Hypothesis * hyp,
          UnitMeasure * ump,
          QObject * parent = NULL );

    /** Puntatore ad oggetto BeamType contenente le informazioni sul tipo di teoria
         utilizzata per il calcolo dell'elemento */
    BeamType * bType;

    /** @return la sezione della trave */
    Section * section();
    /** Imposta la sezione della trave
        @param s la sezione da impostare */
    void setSection( Section * s );

    /** Forze concetrate sui nodi sulla trave, orientate secondo il riferimento locale,
        ottenute sommando tutte le forze concentrate contenute in fModel().
        Per aggiornare questo parametro bisogna usare il metodo setFLocal();
        @param i componente */
    DoublePlus * fAppLocal( int i );

    /** Modello delle forze concentrate applicate sui nodi della trave */
    BeamFModel * fAppModel();

    /** Forze distribuite sulla trave, orientate secondo il riferimento locale,
        ottenute sommando le altre forze distribuite.
        Per aggiornare questo parametro bisogna usare il metodo setQLocal();
        @param i componente */
    DoublePlus * qAppLocal( int i );

    /** Modello delle forze distribuite applicate sulla trave */
    BeamQModel * qAppModel();

    /** Lunghezza della trave */
    DoublePlus * L;

    /** Imposta il primo vertice (wrapper di Element::setVertex(0, vert)) */
    void setVertex1( Vertex * vert );
    /** Imposta il secondo vertice (wrapper di Element::setVertex(1, vert)) */
    void setVertex2( Vertex * vert );

    /** Stringa associata univocamente all'elemento (per uso interno al programma
        non va riportata all'utente) */
    QString typeElement();

    /** Calcola la matrice del sistema di rotazione del sistema di riferimento locale [LCS].
        E' una matrice 2x2 (caso 2D) o 3x3 (caso 3d) contenente le basi del sistema di riferimento
        locale disposte per righe.
        La prima base è orientata dal primo al secondo vertice.
        La seconda base è ottenuta dal prodotto vettoriale della prima per la base k (asse z).
        La terza base (solo per il caso tridimensionale) è ottenuta dal prodotto vettoriale
        della prima e della seconda.
        Tutte le tre basi sono normalizzate (modulo unitario).
        Calcola inoltre {LCSOrigin} posto uguale al primo vertice. */
    void setLCS();

    /** Spostamenti interni all'elemento
        @param coordinate x locali del punto in cui si calcolano gli spostamenti
        @param name nome degli spostamenti
        @param val valore degli spostamenti
        @param n numero dei punti in cui si calcolano gli spostamenti */
    void etaPLocal(QString * xName, QVector< double > * xVect, QVector<QString> * valNames, QVector<QVector< double> > * val, int n, bool scale = true );

    /** Nomi delle caratteristiche di sollecitazione date alcune ipotesi */
    static QVector<QString> FPLocalNames(Hypothesis *hyp);

    /** Caratteristiche di sollecitazione interne alla trave
     *  @param xName denominazione e UdM delle ascisse
        @param xVect ascisse locali del punto in cui si calcolano le caratterisitche di sollecitazione
        @param valNames nome delle caratteristiche di sollecitazione
        @param valVect vettore delle caratterisitche di sollecitazione
        @param nP numero delle ascisse in cui si effettua il calcolo */
    void FPLocal(QString * xName, QVector< double> * xVect, QVector<QString> * valNames, QVector<QVector< double> > * valVect, int nP );

    /** @return Oggetto di tipo QGraphicsItem associato all'elemento */
    QGraphicsItem * qGraphicsItem();

    /** @return Oggetto di tipo GLItem associato all'elemento */
    GLItem * glItem();

    /** Spostamenti di punti interni all'elemento nel sistema di riferimento globale
        @param nP numero di punti su cui viene calcolato lo spostamento (estremi inclusi)
        @param xPUndeformed vettore di dimensione n*spaceDim contenente le coordinate globali dei
               punti della struttura indeformata
        @param xPDeformed vettore di dimensione n*spaceDim contenente le coordinate globali dei
               punti della struttura deformata */
    GLItem * glItemDeformed( int nP, double scale );

    /** Caratteristiche di sollecitazione della trave */
    GLItem * glItemFPLocal( int nFP, int nP, double scale );

public slots:
    /** Calcola la matrice di rigidezza locale ed il vettore delle forze nodali equivalenti */
    void setKF0Local();
    /** Calcola la risultante delle forze nodali applicate */
    void setFAppLocal();
    /** Calcola la risultante dei carichi distribuiti applicati */
    void setQAppLocal();

private:
    /** Oggetto privato contenente proprietà che non devono essere accessibili all'esterno */
    BeamPrivate * m_dd;
    /** Inizializza la variabili associate */
    void init();

    /** Fattori matrice di rigidezza locale
        Si preoccupa di richiamare la teoria scelta dall'utente */
    double KLocal(int i, int j, double E, double G, double chi, double A, double J, double l);

    /** Fattori matrice di rigidezza calcolata secondo teoria di Bernoulli */
    double KBernoulli(int i, int j, double E, double A, double J, double l);

    /** Fattori matrice di rigidezza calcolata secondo teoria di Thimoshenko */
    double KTimoshenko(int i, int j, double E, double G, double chi, double A, double J, double l);

    /** Spostamenti nodali trave semplicemente appoggiata secondo teoria di Thimoshenko */
    double etaSSBernoulli(int i, double n1, double v1, double m1, double n2, double v2, double m2, double E, double A, double J, double l);

    /** Spostamenti nodali trave semplicemente appoggiata secondo teoria di Thimoshenko */
    double etaSSTimoshenko(int i, double n1, double v1, double m1, double n2, double v2, double m2, double E, double G, double chi, double A, double J, double l);

    /** Forze nodali trave semplicemente appoggiata */
    double fSS(int i, double n1, double v1, double m1, double n2, double v2, double m2, double l);

    /** Calcola gli spostamenti assiali interni di un insieme di punti le cui ascisse sono
     *  contenute in un vettore xVect. Il calcolo viene eseguito secondo la teoria
     *  elastica del primo ordine */
    void etaPAx(QVector<double> *xVect, QVector<double> *valVect,
                double eta1, double eta4,
                double p1, double p2,
                double E, double A, double l);

    /** Calcola gli spostamenti interni in un insieme di punti le cui ascisse sono
     *  contenute in un vettore xVect. Il calcolo viene eseguito secondo la teoria
     *  di Eulero-Bernoulli. Le grandezze vengono fornite in unità di misura normali
     *  (interne al programma) che vano poi trasformate in unità di misura utente
        @param xVect vettore delle ascisse dei punti di cui si vogliono calcolare gli spostamenti
        @param wVect vettore su cui vengono scritti gli spostamenti trasversali
        @param thetaVect vettore su cui vengono scritte le rotazioni
        @param eta2 spostamento trasversale nodo 1
        @param eta3 rotazione nodo 1
        @param eta5 spostamento trasversale nodo 2
        @param eta6 rotazione nodo 2
        @param q1 carico distribuito trasversale al nodo 1
        @param m1 coppia distribuita al nodo 1
        @param q2 carico distribuito trasversale al nodo 2
        @param m2 coppia distribuita al nodo 2
        @param E modulo di elasticità normale
        @param J momento di inerzia della sezione della trave
        @param l lunghezza della trave */
    void etaPBernoulli(QVector< double> * xVect, QVector< double> * wVect, QVector< double> * thetaVect,
                       double eta2, double eta3, double eta5, double eta6,
                       double q1, double m1, double q2, double m2,
                       double E, double J, double l);

    /** Calcola gli spostamenti interni in un insieme di punti le cui ascisse sono
     *  contenute in un vettore xVect. Il calcolo viene eseguito secondo la teoria
     *  di Timoshenko. Le grandezze vengono fornite in unità di misura normali
     *  (interne al programma) che vano poi trasformate in unità di misura utente
        @param xVect vettore delle ascisse dei punti di cui si vogliono calcolare gli spostamenti
        @param wVect vettore su cui vengono scritti gli spostamenti trasversali
        @param thetaVect vettore su cui vengono scritte le rotazioni
        @param eta2 spostamento trasversale nodo 1
        @param eta3 rotazione nodo 1
        @param eta5 spostamento trasversale nodo 2
        @param eta6 rotazione nodo 2
        @param q1 carico distribuito trasversale al nodo 1
        @param m1 coppia distribuita al nodo 1
        @param q2 carico distribuito trasversale al nodo 2
        @param m2 coppia distribuita al nodo 2
        @param E modulo di elasticità normale
        @param G modulo di elasticità trasversale
        @param chi fattore di taglio
        @param A area della sezione
        @param J momento di inerzia della sezione della trave
        @param l lunghezza della trave */
    void etaPTimoshenko(QVector< double> * xVect, QVector< double> * wVect, QVector< double> * thetaVect,
                        double eta2, double eta3, double eta5, double eta6,
                        double q1, double m1, double q2, double m2,
                        double E, double G, double chi, double A, double J, double l);

private slots:
    /** Calcola la launghezza della trave */
    void setL();
    /** Aggiorna l'oggetto QGraphicsItem */
    void updateQGraphicsItem();
    /** Aggiorna l'oggetto GLItem */
    void updateGLItem();
    /** Reimposta le connessioni per l'aggiornamento dell'oggetto GLItem associato */
    void reconnectVertexToGLItem( int index, Vertex * oldVert, Vertex * newVert);
};


#include "enumplus.h"

class BeamTypePrivate;

/**
* @class BeamType
*
* @brief Classe usata per identificare la tipologia di teoria applicata all'elemento Beam
*
* Questa classe viene impiegata per determinare con quale teoria viene calcolate l'elemento
* Beam
*
* @author Michele Mocciola
*
*/

class BeamType : public EnumPlus
{
    Q_OBJECT
public:
    BeamType( Beam::Type bt,
              const QString & nn,
              bool ro = false );

    QString typeVar();

    BeamType & operator=( BeamType &);
    VarPlus & operator=( VarPlus &);

    Beam::Type value();
    Beam::Type valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( Beam::Type, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( Beam::Type, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    BeamTypePrivate * m_d;

    ~BeamType();
};


#endif // BEAM_H
