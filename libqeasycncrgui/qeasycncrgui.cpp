#include "qeasycncrgui.h"
#include "ui_qeasycncrgui.h"

#include "simplesectionrcncrrectgui.h"
#include "simplesectionrcncrcompgui.h"

class QEasyCncrGUIPrivate{
public:
    QEasyCncrGUIPrivate():
        ui(new Ui::QEasyCncrGUI) {
    }
    ~QEasyCncrGUIPrivate(){
        delete ui;
    }

    Ui::QEasyCncrGUI * ui;
};

QEasyCncrGUI::QEasyCncrGUI(UnitMeasure * ump, QWidget *parent) :
    QWidget(parent),
    m_d(new QEasyCncrGUIPrivate()) {
    m_d->ui->setupUi(this);

    QGridLayout * simpleSectionRCncrLayout = new QGridLayout( m_d->ui->simpleSectionRCncrTab );
    simpleSectionRCncrLayout->addWidget( new SimpleSectionRCncrRectGUI( ump, this ) );

    QGridLayout * simpleSectionRCncrCompLayout = new QGridLayout( m_d->ui->simpleSectRCncrCompTab );
    simpleSectionRCncrCompLayout->addWidget( new SimpleSectionRCncrCompGUI( ump, this ) );
}

QEasyCncrGUI::~QEasyCncrGUI() {
    delete m_d;
}
