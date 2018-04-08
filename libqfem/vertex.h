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
#ifndef VERTEX_H
#define VERTEX_H

class GLItem;
class DoublePlus;
class BoolPlus;
class UIntPlus;
class UnitMeasure;
class Hypothesis;
class VertexPrivate;

#include "varpluscontainer.h"
#include <QObject>

/**
* @class Vertex
*
* @brief Classe usata per modellizzare i vertici del nostro modello
*
* Questa classe viene impiegata per modellizzare i vertici del modello alla base del problema
*
* @author Michele Mocciola
*
*/

class Vertex : public QObject, public VarPlusContainer
{
    Q_OBJECT
public:
    /** Costruttore della classe Vertex
        @param hyp ipotesi alla base del problema in analisi
        @param ump unità di misura adottate
        @param parent oggetto genitore */
    Vertex(Hypothesis * hyp, UnitMeasure * ump, QObject * parent=0 );

    /** Costruttore della classe Vertex da attributo XML */
    Vertex(Hypothesis * hyp, UnitMeasure * ump, const QXmlStreamAttributes & attrs, QObject * parent = 0);

    /** Distruttore della classe.
        Si occupa di elminare dalla memoria le variabili create. */
    ~Vertex();

    /** Dimensione dell'array contenente le coordinate del vertice */
    int xGlobalSize();
    /** Coordinata i-esima del vertice */
    DoublePlus * xGlobal(int);

    /** Ci dice se  lo spostamento è stato imposto */
    BoolPlus * isEtaSet(int);
    /** Spostamento del grado di libertà i-esimo del vertice */
    DoublePlus * etaGlobal(int);
    /** Il valore delle reazioni associate al grado di libertà i-esimo */
    DoublePlus * fGlobal(int);

    /** Ipotesi associate al vertice
    @return puntatore alla classe Hypotehesis contenenti le ipotesi alla base del vertice */
    Hypothesis * hypothesis();

    /** @return Puntatore all'unità di misura*/
    UnitMeasure * unitMeasure();

    /** @return Puntatore all'oggetto grafico openGL associato al vertice
        Se non esiste, lo crea. */
    GLItem * glItem();

private:
    VertexPrivate * m_d;

    void initVar();
private slots:
    void updateGLItem();

signals:
    void vertexChanged();
};

#endif // VERTEX_H
