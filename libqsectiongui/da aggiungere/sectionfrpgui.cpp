#include "sectionfrpgui.h"
#include "ui_sectionfrpgui.h"

SectionFRPGUI::SectionFRPGUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SectionFRPGUI)
{
    ui->setupUi(this);
}

SectionFRPGUI::~SectionFRPGUI()
{
    delete ui;
}

void SectionFRPGUI::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
