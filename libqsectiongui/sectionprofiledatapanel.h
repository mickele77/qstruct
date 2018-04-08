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
#ifndef SECTIONPROFILEDATAPANEL_H
#define SECTIONPROFILEDATAPANEL_H

class SectionProfile;

class SectionProfileDataPanelPrivate;

#include <QWidget>

class SectionProfileDataPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SectionProfileDataPanel(QWidget *parent = 0);
    ~SectionProfileDataPanel();

    SectionProfile * section();

public slots:
    void setSection(SectionProfile * );

private:
    SectionProfileDataPanelPrivate * m_d;

};

#endif // SECTIONPROFILEDATAPANEL_H
