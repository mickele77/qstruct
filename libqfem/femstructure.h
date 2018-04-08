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
#ifndef FEMSTRUCTURE_H
#define FEMSTRUCTURE_H

#include <QObject>

#include <Eigen/Core>

class VertexModel;
class Vertex;
class ElementModel;
class SectionModel;
class Section;
class Hypothesis;
class UnitMeasure;
class QGraphicsItem;
class QGraphicsPointItem;
class QGraphicsLineItem;
class QRectF;
class GLItem;

class FEMStructurePrivate;

/**
* @class FEMStructure
*
* @brief Classe usata per modellizzare la struttura
*
* Questa classe viene impiegata per modellizzare l'intera struttura oggetto della nostra analisi
*
* @author Michele Mocciola
*
*/

class FEMStructure: public QObject
{
    Q_OBJECT
public:
    /** Costruttore nel caso di elementi con più sezioni
    @param sm modello delle sezioni (sezioni diverse per gli elementi della struttura)
    @param hyp ipotesi alla base dello studio
    @param ump unità di misura
    @param parent genitore dell'oggetto */
    FEMStructure( SectionModel * sm, Hypothesis * hyp, UnitMeasure * ump, QObject * parent = 0 );

    /** Costruttore nel caso di elementi con un0unica sezione
    @param sm sezione che verrà usata per la struttura (c'è una sola sezione per tutti gli elementi)
    @param hyp ipotesi alla base dello studio
    @param ump unità di misura
    @param parent genitore dell'oggetto */
    FEMStructure( Section * s, Hypothesis * hyp, UnitMeasure * ump, QObject * parent = 0 );

    ~FEMStructure();

    /** Modello dei vertici */
    VertexModel * vertexModel();
    /** Modello degli elementi */
    ElementModel * elementModel();

    /** Gradi di libertà della struttura
    @return numero di gradi di libertà complessivi della struttura */
    int nDOF();

    /** Ci dice se il vertice è usato o meno
    @param vert vertice di cui ci si interroga se è usato o meno
    @return risultato della ricerca */
    bool isVertexUsed( Vertex * vert );

    /**
    @return puntatore all'oggetto grafico associato alla struttura */
    QGraphicsItem * graphicsItem();
    /** Rettangolo che inscrive l'oggetto grafico */
    QRectF graphicsItemBoudingRect();

    GLItem * glItemDeformed( double scale=10.0);
    GLItem * glItemBeamFPLocal( int nFP, double scale=1.0);

    Hypothesis *hypothesis();

public slots:
    void setProblem();
    void solveProblem();

    // Grafica
    void setVertexRadius( double );

private:
    FEMStructurePrivate * m_d;

    /** Calcola la lista dei vertici effettivamente usati */
    void setUsedVertexList();

private slots:
    // Grafica
    void updateQGraphics( Vertex * );

};

#endif // FEMSTRUCTURE_H
