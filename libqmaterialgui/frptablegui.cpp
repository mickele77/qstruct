#include "frptablegui.h"
#include "ui_tablemodelgui.h"

FRPTableGUI::FRPTableGUI(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::TableModelGUI)
{
    m_ui->setupUi(this);
}

FRPTableGUI::~FRPTableGUI()
{
    delete m_ui;
}

