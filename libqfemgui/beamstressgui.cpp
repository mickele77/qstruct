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
#include "beamstressgui.h"

#include "beam.h"

#include "qcustomplot.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QList>

#include <algorithm>

class BeamStressGUIPrivate{
public:
    BeamStressGUIPrivate( BeamStressGUI * p ):
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
    }
    ~BeamStressGUIPrivate(){
    }
    void addPlots(){
        if( beam != NULL ){
            beam->FPLocal( &xName, &x, &fNames, &f, 19);
            if( x.size() > 0 ){
                for( int i=0; i < f.size(); i++ ){
                    plotList.append( new QCustomPlot(scrollContents) );
                    QCPGraph * graph = plotList.last()->addGraph();
                    graph->setName( fNames.at(i) );
                    graph->setData( x, f.at(i));
                    plotList.last()->xAxis->setRange( x.first(), x.last() );
                    plotList.last()->yAxis->setRange( *std::min_element(f.at(i).constBegin(), f.at(i).constEnd() ),
                                                      *std::max_element(f.at(i).constBegin(), f.at(i).constEnd() ) );
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
            beam->FPLocal( &xName, &x, &fNames, &f, 19);
            if( x.size() > 0 ){
                for(int i=0; i < plotList.size(); i++){
                    if( plotList.at(i)->graphCount() > 0 ){
                        QCPGraph * graph = plotList.at(i)->graph( 0 );
                        if( graph ){
                            graph->setData( x, f.at(i));
                        }
                        plotList.at(i)->xAxis->setRange( x.first(), x.last() );
                        plotList.at(i)->yAxis->setRange( *std::min_element(f.at(i).constBegin(), f.at(i).constEnd() ),
                                                          *std::max_element(f.at(i).constBegin(), f.at(i).constEnd() ) );
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
    BeamStressGUI * parent;
    QGridLayout * globalLayout;
    QScrollArea * scrollArea;
    QWidget * scrollContents;
    QGridLayout * scrollLayout;
    QList<QCustomPlot *> plotList;

    QString xName;
    QVector<double> x;
    QVector<QString> fNames;
    QVector<QVector<double> > f;
};

BeamStressGUI::BeamStressGUI( QWidget *parent) :
    QWidget(parent),
    m_d( new BeamStressGUIPrivate( this) ){
    setLayout( m_d->globalLayout );
}

BeamStressGUI::~BeamStressGUI() {
    delete m_d;
}

void BeamStressGUI::setBeam(Beam * b){
    m_d->setBeam( b );
    // updateResults();
}

void BeamStressGUI::updateData(){
    m_d->updatePlots();
}
