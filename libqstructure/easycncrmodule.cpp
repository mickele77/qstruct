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
#include "easycncrmodule.h"

#include "qeasycncrgui.h"

#include <QGridLayout>
#include <QIcon>

class EasyCncrModulePrivate {
public:
    EasyCncrModulePrivate( UnitMeasure * ump, QWidget * parent ):
        gui(new QEasyCncrGUI( ump, parent )){
    }
    ~EasyCncrModulePrivate(){
    }

    QEasyCncrGUI * gui;
};


EasyCncrModule::EasyCncrModule( UnitMeasure * ump,
                          QWidget *parent) :
    StructModule( ump, parent ),
    m_d( new EasyCncrModulePrivate( ump, this) ){

    setLayout( new QGridLayout(this));
    layout()->addWidget( m_d->gui );
}

EasyCncrModule::~EasyCncrModule(){
    delete m_d;
}

QString EasyCncrModule::name(){
    return QString("EasyCncr");
}

QString EasyCncrModule::richName(){
    return trUtf8("Easy C.A.");
}

QIcon EasyCncrModule::icon(){
    return QIcon(":/images/easycncr.svg");
}
