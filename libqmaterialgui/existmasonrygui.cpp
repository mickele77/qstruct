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
#include "existmasonrygui.h"
#include "ui_existmasonrygui.h"

ExistMasonryGUI::ExistMasonryGUI(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ExistMasonryGUI)
{
#ifdef WIN32
    setStyleSheet("* {font-size: 14px}");
#endif

    m_ui->setupUi(this);
}

ExistMasonryGUI::~ExistMasonryGUI()
{
    delete m_ui;
}
