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
#include "beamdeformationgui.h"

#include "beam.h"

#include "qcustomplot.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QList>

#include <algorithm>

class BeamDeformationGUIPrivate{
public:
    BeamDeformationGUIPrivate( BeamDeformationGUI * p ):
        beam( NULL ),
        parent(p),
        globalLayout( new QGridLayout() ),
        scrollArea( new QScrollArea(p) ),
        scrollContents( new QWidget() ),
        scrollLayout( new QGridLayout(scrollContents) ){

        scrollArea->setWidgetResizable(true);
        scrollContents->setLayout( scrollLayout );
        scrollArea->setWidget(scrollContents);
        globalLayout->addWidget( scrollArea, 0,0 );
    };
    ~BeamDeformationGUIPrivate(){
    }
    void addPlots(){
        if( beam != NULL ){
            beam->etaPLocal( &xName, &x, &etaNames, &eta, 9);
            if( x.size() > 0 ){
                for( int i=0; i < eta.size(); i++ ){
                    plotList.append( new QCustomPlot(scrollContents) );
                    QCPGraph *graph = plotList.last()->addGraph();
                    graph->setName( etaNames.at(i) );
                    graph->setData( x, eta.at(i));
                    plotList.last()->xAxis->setRange( x.first(), x.last() );
                    plotList.last()->yAxis->setRange( *std::min_element(eta.at(i).constBegin(), eta.at(i).constEnd() ),
                                                      *std::max_element(eta.at(i).constBegin(), eta.at(i).constEnd() ) );
                    plotList.last()->replot();
                    scrollLayout->addWidget( plotList.last(), i, 0 );
                }
            }
        }
    }

    void removePlots(){
        while( plotList.size() > 0 ){
            scrollLayout->removeWidget( plotList.last() );
            plotList.removeLast();
        }
    }

    void updatePlots(){
        if( beam != NULL ){
            beam->etaPLocal( &xName, &x, &etaNames, &eta, 19);
            if( x.size() > 0 ){
                for(int i=0; i < plotList.size(); i++){
                    if( plotList.at(i)->graphCount() > 0 ){
                        QCPGraph * graph = plotList.at(i)->graph( 0 );
                        if( graph ){
                            graph->setData( x, eta.at(i));
                        }
                        plotList.at(i)->xAxis->setRange( x.first(), x.last() );
                        plotList.at(i)->yAxis->setRange( *std::min_element(eta.at(i).constBegin(), eta.at(i).constEnd() ),
                                                          *std::max_element(eta.at(i).constBegin(), eta.at(i).constEnd() ) );
                        plotList.at(i)->replot();
                    }
                }
            }
        }
    }

    void setBeam( Beam * b ){
        Beam * oldBeam = beam;
        beam = b;
        if( oldBeam != NULL ){
            updatePlots();
        } else {
            addPlots();
        }
    }

    Beam * beam;
    BeamDeformationGUI * parent;
    QGridLayout * globalLayout;
    QScrollArea * scrollArea;
    QWidget * scrollContents;
    QGridLayout * scrollLayout;
    QList<QCustomPlot *> plotList;

    QString xName;
    QVector<double> x;
    QVector<QString> etaNames;
    QVector<QVector<double> > eta;
};

BeamDeformationGUI::BeamDeformationGUI( QWidget *parent) :
    QWidget(parent),
    m_d( new BeamDeformationGUIPrivate( this) ){
    setLayout( m_d->globalLayout );
}

BeamDeformationGUI::~BeamDeformationGUI() {
    delete m_d;
}

void BeamDeformationGUI::setBeam(Beam * b){
    m_d->setBeam( b );
}

void BeamDeformationGUI::updateData(){
    m_d->updatePlots();
}
