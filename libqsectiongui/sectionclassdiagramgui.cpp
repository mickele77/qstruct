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
#include "sectionclassdiagramgui.h"
#include "ui_sectionclassdiagramgui.h"

#include "sectionsteel.h"
#include <QPolygonF>

SectionClassDiagramGUI::SectionClassDiagramGUI(SectionSteel * sect,
                                               QWidget *parent ):
    QDialog(parent),
    m_ui(new Ui::SectionClassDiagramGUI) {
    m_ui->setupUi(this);

    QList<QPolygonF * > * polygonList = sect->classDiagram();
    for( int i=0; i < polygonList->size(); i++ ){
        QCPGraph * curve = m_ui->plot->addGraph();
        switch(i){
        case 0:
            curve->setPen( QPen(Qt::green ));
            curve->setBrush( QBrush(Qt::green));
            break;
        case 1:
            curve->setPen( QPen(Qt::cyan ));
            curve->setBrush( QBrush(Qt::cyan));
            break;
        case 2:
            curve->setPen( QPen(Qt::red ));
            curve->setBrush( QBrush(Qt::red));
            break;
        case 3:
            curve->setPen( QPen(Qt::magenta ));
            curve->setBrush( QBrush(Qt::magenta));
            break;
        }

        QVector<double> xData;
        QVector<double> yData;
        for( QPolygonF::iterator p = polygonList->at(i)->begin(); p != polygonList->at(i)->end(); ++p ){
            xData.append( p->x() );
            yData.append( p->y() );
        }
        curve->addData( xData, yData );
    }
    m_ui->plot->replot();
}

SectionClassDiagramGUI::~SectionClassDiagramGUI() {
    delete m_ui;
}
