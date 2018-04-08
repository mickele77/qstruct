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
#ifndef SECTIONTRANSFORMPANEL_H
#define SECTIONTRANSFORMPANEL_H

class SectionTransformPanelPrivate;
class UnitMeasure;
class Section;

#include <QWidget>

/**
* @class SectionTransformPanel
*
* @brief Classe usata per applicare trasformazioni geometriche a sezioni
*
* Questa classe fornisce l'interfaccia per applicare trasformazioni geometriche a sezioni
*
* @author Michele Mocciola
*
*/

class SectionTransformPanel : public QWidget {
    Q_OBJECT
    
public:
    explicit SectionTransformPanel(UnitMeasure *ump, QWidget *parent = 0);
    ~SectionTransformPanel();
    
    Section * section();

public slots:
    void setSection( Section * );

private:
    SectionTransformPanelPrivate * m_d;

private slots:
    void rotate();
    void translate();
    void GTranslate();
    void PTransRot();
};

#endif // SECTIONTRANSFORMPANEL_H
