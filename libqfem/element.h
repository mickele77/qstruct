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
#ifndef ELEMENT_H
#define ELEMENT_H

#include <QObject>

#include <Eigen/Core>

class ElementPrivate;
class Vertex;
class VertexModel;
class ElementVertexModel;
class Hypothesis;
class UnitMeasure;
class BoolPlus;
class QGraphicsItem;
class GLItem;
template <typename T> class QList;
template <typename T> class QVector;


/**
* @class VertexObject
*
* @brief Classe contenente le informazioni necessarie per ciascun vertice di un elemento
*
* Questa classe permette di memorizzare ed accedere a tutte le informazioni dei
* vertici di un elemento
*
* @author Michele Mocciola
*/

class VertexElement{
public:
    VertexElement( Vertex * );
    Vertex * vertex;
    QList<BoolPlus *> * vertexConnection;
};

/**
* @class Element
*
* @brief Classe genitore di qualunque elemento
*
* Questa classe viene impiegata come classe base di ciascun elemento introdotto
*
* @author Michele Mocciola
*
*/

class Element: public QObject {

    Q_OBJECT

public:
    /**
    @param iidd id dell'elemento
    @param hyp ipotesi alla base dell'elemento
    @param ump unità di misura
    @param parent oggetto genitore */
    Element( unsigned int iidd, Hypothesis * hyp, UnitMeasure * ump,  QObject * parent = NULL );

    /**
    Distruttore della classe. Elimina gli attributi creati tramite puntatori. */
    ~Element();


    /** Puntatore all'ggeto della classe Hypothesis associata all'elemento */
    Hypothesis * hypothesis();

    /** Numero di gradi di libertà complessivi dell'elemento
    (numero di gradi di libertà di un vertice * numero dei vertici) */
    int nDOF();

    /** Incrementa l'id della classe usando come riferimento una variabile statica
        che quindi è comune a tutti gli oggetti di questa classe */
    void nextId();
    /** Restituisce l'id dell'elemento, un numero univoco che lo identifica
    @return l'id dell'elemento */
    unsigned int id();

    /**
    @return la denominazione dell'elemento */
    virtual QString typeElement() = 0;

    /** Numero di vertici */
    int nVertexes();
    Vertex * vertex(int index);
    /** Imposta i vertici contenuti nella lista come vertici dell'elemento
    @param vertList list dei vertici da impostare */
    void setVertexes( const QList<Vertex *> & vertList );
    /** Imposta il vertice vert dato come vertice i-esimo dell'elemento
    @param vert il vertice da impostare
    @param i l'indice del vertice nell'elemento */
    void setVertex( int i, Vertex * vert);
    /** Ci dice se il vertice vert è o meno un vertice dell'elemento
    @param vert il vertice su cui viene effettuata la ricerca */
    bool isVertexUsed( Vertex * vert );
    /** Gradi di connessione dell'elemento al vertice
    @param vert indice del vertice di cui si irestituiscono le connessioni */
    QList<BoolPlus *> *vertexConnection( int vert );
    /** Connessione dell'elemento al vertice
    @param vert indice del vertice di cui si restituiscono le connessioni
    @param index indice della connessione restituita  */
    BoolPlus * vertexConnection( int vert, int index );

    /** Crea il modello dei vertici dell'elemento */
    void createVertexModel(VertexModel *vm);
    /**
    @return il modello dei vertici dell'elemento */
    ElementVertexModel * vertexModel();

    /** Sottomatrice di rigidezza:
        forze applicate nel vertice vert1 a seguito di spostamenti nel vertice vert2 */
    Eigen::MatrixXd KGlobalVert( Vertex * vert1, Vertex * vert2 );
    /** Sottovettore dei carichi nodali equivalenti:
        forze applicate nel vertice vert */
    Eigen::VectorXd f0GlobalVert( Vertex * vert );

    /**
    @return oggetto grafico di tipo QGraphicsItem associato all'elemento */
    virtual QGraphicsItem * qGraphicsItem() = 0;

    /**
    @return oggetto grafico di tipo GLItem associato all'elemento */
    virtual GLItem * glItem() = 0;

    /**
    @return oggetto grafico di tipo GLItem usato per rappresentare l'elemento deformato */
    virtual GLItem * glItemDeformed( int nP, double scale ) = 0;

public slots:
    /** Calcola la matrice per il cambio di sistema di riferimento (globale -> locale) */
    virtual void setLCS() = 0;
    /** Calcola la matrice di rotazione dell'elemento */
    void setN();

    /** Calcola la matrice di rigidezza e i carichi nodali equivalenti, nel sistema
        di riferimento locale */
    virtual void setKF0Local() = 0;
    /** Calcola la matrice di rigidezza e i carichi nodali equivalenti, nel sistema
        di riferimento globale */
    void setKF0Global();

    /** Calcola il vettore delle forze nodali ed il vettore degli spostamenti nodali,
        nel sistema di riferimento locale */
    void setFEtaLocal();

    virtual void updateQGraphicsItem() = 0;

private:
    unsigned int m_id;
    static unsigned int s_nextId;

protected:
    Hypothesis * m_hypothesis;
    UnitMeasure * m_unitMeasure;

    /** Vertici dell'elemento */
    QList<VertexElement *> * m_vertexes;

    /** Modello dei vertici dell'elemento */
    ElementVertexModel * m_vertexModel;

    /** Dimensione della matrice m_LCS */
    int m_LCSSize;

    /** Matrice di rototralsazione del sistema di riferimento locale dell'elemento
        Dimensione = (m_hypothesis->nDOFVert() + 1) * (m_hypothesis->nDOFVert() + 1)
        {r'} = vettore posizione del punto nel sistema di riferimento locale
        {r} = vettore posizione del punto nel sistema di riferimento globale
        {r'} = {LCSOrigin} + [LCS] * {r} */
    double * m_LCS;
    double * m_LCSOrigin;

    /** Dimensione dei vettori dei carichi e degli spostamenti nodali */
    int m_nDOF;

    /** Matrice di rotazione degli spostamenti nodali
        Dimensione = m_arraySize * m_arraySize
        {eta'} = spostamenti nodali nel sistema di riferimento locale
        {eta} = spostamentio nodali nel sistema di riferimento globale
        {eta'} = [N] * {eta} */
    double * m_N;

    /** Vettore dei carichi nodali equivalenti, nel sistema di riferimento locale, teorici
        (senza tener conto dei vincoli effettivi) */
    double * m_f0Local;
    /** Vettore dei carichi nodali equivalenti, nel sistema di riferimento globale */
    double * m_f0Global;

    /** Matrice di rigidezza locale teorica (senza tener conto dei vincoli effettivi) */
    double * m_KLocal;
    /** Matrice di rigidezza globale */
    double * m_KGlobal;

    /** Vettore degli spostamenti nodali, sistema di riferimento locale */
    double * m_etaLocal;

    /** Vettore dei carichi nodali, sistema di riferimento locale */
    double * m_fLocal;

    /** Inizializzazione attributi comuni a tutti i memebri della classe Element.
        Questo metodo deve essere richiamato nel costruttore di ciascuna classe derivata. */
    void commonInit();

signals:
    /** Segnale emesso quando cambia uno dei vertici dell'elemento */
    void vertexChanged( int index, Vertex * oldVert, Vertex * newVert);
};

#endif // ELEMENT_H
