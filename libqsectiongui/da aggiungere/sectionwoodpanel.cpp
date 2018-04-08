#include "sectionwoodpanel.h"
#include "ui_sectionwoodpanel.h"

SectionWoodPanel::SectionWoodPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SectionWoodPanel)
{
    ui->setupUi(this);
}

SectionWoodPanel::~SectionWoodPanel()
{
    delete ui;
}

void SectionWoodPanel::changeEvent(QEvent *e)
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
