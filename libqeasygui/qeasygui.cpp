#include "qeasygui.h"
#include "ui_qeasygui.h"

#include "loadresultantgui.h"
#include "bondedanchorgui.h"

class QEasyGUIPrivate{
public:
    QEasyGUIPrivate():
        ui(new Ui::QEasyGUI) {
    }
    ~QEasyGUIPrivate(){
        delete ui;
    }

    Ui::QEasyGUI * ui;
};

QEasyGUI::QEasyGUI(UnitMeasure * ump, QWidget *parent) :
    QWidget(parent),
    m_d(new QEasyGUIPrivate()) {
    m_d->ui->setupUi(this);

    QGridLayout * loadResultantLayout = new QGridLayout( m_d->ui->loadResultantTab );
    loadResultantLayout->addWidget( new LoadResultantGUI( ump, this ) );

    QGridLayout * bondedAnchorLayout = new QGridLayout( m_d->ui->bondedAnchorTab );
    bondedAnchorLayout->addWidget( new BondedAnchorGUI( ump, this ) );
}

QEasyGUI::~QEasyGUI() {
    delete m_d;
}
