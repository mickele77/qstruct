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
#include "changerichnamedialog.h"
#include "ui_changerichnamedialog.h"

ChangeRichNameDialog::ChangeRichNameDialog(QString * name, QWidget *parent) :
    QDialog(parent),
    m_name(name),
    m_ui(new Ui::ChangeRichNameDialog) {
    m_ui->setupUi(this);

    m_ui->lineEdit->setText( *m_name );
    m_ui->label->setText( *m_name );

    connect( m_ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(changeName(QString)));
}

ChangeRichNameDialog::~ChangeRichNameDialog() {
    delete m_ui;
}

void ChangeRichNameDialog::changeName( const QString & val ){
    *m_name = val;
    m_ui->label->setText( *m_name );
}
