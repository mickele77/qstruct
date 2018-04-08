#include "sectionrcncrrectpanel.h"
#include "ui_sectionrcncrrectpanel.h"

#include "convertutility.h"
#include "concretetablemodel.h"
#include "steelcncrtablemodel.h"
#include "sectioncncrtablemodel.h"
#include "sectionsteelcncrtablemodel.h"
#include "sectionrcncrrect.h"

#include "sectionsteelcncrdelegate.h"


SectionRCncrRectPanel::SectionRCncrRectPanel( SectionRCncrRect * sect,
                                              ConcreteTableModel * cncrMod,
                                              SteelCncrTableModel * steelCncrMod,
                                              QWidget *parent ) :
QWidget(parent),
m_ui(new Ui::SectionRCncrRectPanel),
m_cncrModel(cncrMod),
m_steelCncrModel( steelCncrMod),
m_section( sect ) {

    m_ui->setupUi(this);

    // nome
    m_ui->nameLEdit->setText( m_section->name() );

    // tab dati generali
    m_ui->sectSteelCncrTableView->setModel( m_section->sectSteelCncrModel() );
    m_ui->sectSteelCncrTableView->setItemDelegate( new SectionSteelCncrDelegate(m_steelCncrModel, this));
    m_ui->bLEdit->setText( m_section->bStr());
    m_ui->hLEdit->setText( m_section->hStr());
    m_ui->ACncrLEdit->setText( m_section->sectCncrModel()->AStr() );
    m_ui->ASlLEdit->setText( m_section->sectSteelCncrModel()->AStr() );

    // tab taglio
    m_ui->dLEdit->setText( m_section->dStr() );
    m_ui->bwLEdit->setText( m_section->bwStr());
    m_ui->NEdLEdit->setText( m_section->NEdStr() );
    m_ui->kLEdit->setText( m_section->kStr());
    m_ui->rholLEdit->setText( m_section->rholStr());
    m_ui->scpLEdit->setText( m_section->scpStr() );
    m_ui->vminLEdit->setText( m_section->vminStr() );
    m_ui->VRdLEdit->setText( m_section->VRdStr());

    m_ui->concreteComboBox->setModel( m_cncrModel );
    m_ui->concreteComboBox->setModelColumn( 0 );
    int index = 0;
    for( int i = 0; i < m_cncrModel->rowCount(); i++){
        if( m_section->concrete() == m_cncrModel->concrete(i)){
            index = i;
        }
    }
    m_section->setConcrete( m_cncrModel->concrete(index ));
    m_ui->concreteComboBox->setCurrentIndex( index );
    connect( m_ui->concreteComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setConcrete( int )));

    // nome
    connect( m_ui->nameLEdit, SIGNAL(textChanged(QString)), m_section, SLOT(setName(QString)) );

    // Dati sezione
    connect( m_ui->bLEdit, SIGNAL(textChanged(QString)), m_section, SLOT(setB(QString)));
    connect( m_ui->hLEdit, SIGNAL(textChanged(QString)), m_section, SLOT(setH(QString)));
    connect( m_ui->dLEdit, SIGNAL(textChanged(QString)), m_section, SLOT(setD(QString)));
    connect( m_ui->NEdLEdit, SIGNAL(textChanged(QString)), m_section, SLOT(setNEd(QString)));
    connect( m_ui->newSectSteelCncrPButton, SIGNAL(clicked()), this, SLOT(addSectSteelCncr()) );
    connect( m_ui->delSectSteelCncrPButton, SIGNAL(clicked()), this, SLOT(removeSectSteelCncr()) );

    connect( m_section->sectCncrModel(), SIGNAL(AChanged(QString)), m_ui->ACncrLEdit, SLOT(setText(QString)));
    connect( m_section->sectSteelCncrModel(), SIGNAL(AChanged(QString)), m_ui->ASlLEdit, SLOT(setText(QString)));
    connect( m_section, SIGNAL(bwChanged(QString)), m_ui->bwLEdit, SLOT(setText(QString)));
    connect( m_section, SIGNAL(kChanged(QString)), m_ui->kLEdit, SLOT(setText(QString)));
    connect( m_section, SIGNAL(rholChanged(QString)), m_ui->rholLEdit, SLOT(setText(QString)));
    connect( m_section, SIGNAL(scpChanged(QString)), m_ui->scpLEdit, SLOT(setText(QString)));
    connect( m_section, SIGNAL(vminChanged(QString)), m_ui->vminLEdit, SLOT(setText(QString)));
    connect( m_section, SIGNAL(VRdChanged(QString)), m_ui->VRdLEdit, SLOT(setText(QString)));
}

SectionRCncrRectPanel::~SectionRCncrRectPanel(){
    delete m_ui;
}

void SectionRCncrRectPanel::setConcrete(int index){
    m_section->setConcrete( m_cncrModel->concrete( index ));
}

void SectionRCncrRectPanel::addSectSteelCncr(void){
    if( m_steelCncrModel->nSteelCncr() > 0 ){
        if( m_steelCncrModel->defaultSteelCncr() != 0 ){
            int row = m_ui->sectSteelCncrTableView->selectionModel()->currentIndex().row();
            if( row < 0 || row >= m_section->nSectionSteelCncr() )
                row = m_section->nSectionSteelCncr() - 1 ;
            m_section->insertSectionSteelCncr( m_steelCncrModel->defaultSteelCncr(), (row+1) );
        }
    }
}

void SectionRCncrRectPanel::removeSectSteelCncr(void){
    int row = m_ui->sectSteelCncrTableView->selectionModel()->currentIndex().row();
    if( row >= 0 && row < m_section->nSectionSteelCncr() )
        m_section->removeSectionSteelCncr( row );
}
