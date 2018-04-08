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
#ifndef SECTIONTHINCONT_H
#define SECTIONTHINCONT_H

class SectionThinContPointsModel;
class SectionThinContPrivate;

#include "sectionthin.h"

class EXPORT_SECTION_LIB_OPT SectionThinCont : public SectionThin {
    Q_OBJECT
public:
    SectionThinCont( UnitMeasure * ump, Material * mat, const QString & nn, QObject *parent=0 );

    /** Costruttore per creazione da attributi XML */
    SectionThinCont(UnitMeasure * ump,
                    const QXmlStreamAttributes & attrs,
                    MaterialModel * matModel,
                    SectionLoadPhaseModel * lModel,
                    QObject * parent = 0);
    ~SectionThinCont();

    void writeXml(QXmlStreamWriter *writer);
    void readXml(QXmlStreamReader *reader);

    /** Modello dei punti */
    SectionThinContPointsModel * pointsModel();

    SectionThinCont &operator =(SectionThinCont &val);
    SectionThin &operator =(SectionThin &);
    Section &operator =(Section &);
    double LNormal();
    double ITNormal();

    BoolPlus * isClosed;

protected:
    SectionThinContPrivate * m_dd;

    void initVar();

protected slots:
    virtual void updateSectionModels();
};




#endif // SECTIONTHINCONT_H
