#include "simplesectionrcncrfrpgui.h"
#include "ui_simplesectionrcncrfrpgui.h"

SimpleSectionRCncrFRPGUI::SimpleSectionRCncrFRPGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimpleSectionRCncrFRPGUI)
{
    ui->setupUi(this);
}

SimpleSectionRCncrFRPGUI::~SimpleSectionRCncrFRPGUI()
{
    delete ui;
}
