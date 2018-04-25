#include "simplesectionrcncrcompgui.h"
#include "ui_simplesectionrcncrcompgui.h"

#include "qcustomplotdialog.h"

#include "simplesectionrcncrcomp.h"
#include "cncrrectmodel.h"
#include "steelamodel.h"
#include "steelfepsmodel.h"
#include "slssteelsigmamodel.h"

#include "varpluscontainer.h"
#include "doubleplus.h"
#include "varplusguiutility.h"

class SimpleSectionRCncrCompGUIPrivate: public VarPlusContainer {
public:
    SimpleSectionRCncrCompGUIPrivate( UnitMeasure * ump ):
        VarPlusContainer( ump, "SimpleSectionRCncrCompGUIPrivate", ""),
        ui(new Ui::SimpleSectionRCncrCompGUI) {
        MRdNEd = new DoublePlus( 0.0, "MRdNEd", m_unitMeasure, UnitMeasure::loadF, true );
        MRdNEd->setRichName( QObject::trUtf8("N<span style=\" vertical-align:sub;\">Ed</span>") );
        MRdNEd->setToolTip( QObject::trUtf8("Verifica a pressoflessione - Sforzo normale agente") );
        MRdNEd->setReadOnly( false );
        addVarToContainer(MRdNEd);

        MChiNEd = new DoublePlus( 0.0, "MChiNEd", m_unitMeasure, UnitMeasure::loadF, true );
        MChiNEd->setRichName( QObject::trUtf8("N<span style=\" vertical-align:sub;\">Ed</span>") );
        MChiNEd->setToolTip( QObject::trUtf8("Diagramma momento-curvatura - Sforzo normale agente") );
        MChiNEd->setReadOnly( false );
        addVarToContainer(MChiNEd);

        MRdPlus = new DoublePlus( 0.0, "MRdPlus", m_unitMeasure, UnitMeasure::loadM, true );
        MRdPlus->setRichName( QObject::trUtf8("M<span style=\" vertical-align:sub;\">Rd,+</span>") );
        MRdPlus->setToolTip( QObject::trUtf8("Verifica a pressoflessione - Momento resistente positivo"));
        MRdPlus->setReadOnly( true );
        addVarToContainer(MRdPlus);

        xMRdPlus = new DoublePlus( 0.0, "xMRdPlus", m_unitMeasure, UnitMeasure::sectL, true );
        xMRdPlus->setRichName( QObject::trUtf8("x<span style=\" vertical-align:sub;\">MRd,+</span>") );
        xMRdPlus->setToolTip( QObject::trUtf8("Verifica a pressoflessione - Distanza dell'asse neutro dal lembo superiore compresso"));
        xMRdPlus->setReadOnly( true );
        addVarToContainer(xMRdPlus);

        MRdMinus = new DoublePlus( 0.0, "MRdMinus", m_unitMeasure, UnitMeasure::loadM, true );
        MRdMinus->setRichName( QObject::trUtf8("M<span style=\" vertical-align:sub;\">Rd,-</span>") );
        MRdMinus->setToolTip( QObject::trUtf8("Verifica a pressoflessione - Momento resistente negativo"));
        MRdMinus->setReadOnly( true );
        addVarToContainer(MRdMinus);

        xMRdMinus = new DoublePlus( 0.0, "xMRdMinus", m_unitMeasure, UnitMeasure::sectL, true );
        xMRdMinus->setRichName( QObject::trUtf8("x<span style=\" vertical-align:sub;\">MRd,-</span>") );
        xMRdMinus->setToolTip( QObject::trUtf8("Verifica a pressoflessione - Distanza dell'asse neutro dal lembo inferiore compresso"));
        xMRdMinus->setReadOnly( true );
        addVarToContainer(xMRdMinus);

        SLSNEk = new DoublePlus( 0.0, "SLSNEk", m_unitMeasure, UnitMeasure::loadF, false );
        SLSNEk->setRichName( QObject::trUtf8("N<span style=\" vertical-align:sub;\">Ek</span>") );
        SLSNEk->setToolTip( QObject::trUtf8("Verifica a pressoflessione SLE - Sforzo normale agente"));
        addVarToContainer(SLSNEk);

        SLSMEk = new DoublePlus( 0.0, "SLSMEk", m_unitMeasure, UnitMeasure::loadM, false );
        SLSMEk->setRichName( QObject::trUtf8("M<span style=\" vertical-align:sub;\">Ek</span>") );
        SLSMEk->setToolTip( QObject::trUtf8("Verifica a pressoflessione SLE - Momento agente"));
        addVarToContainer(SLSMEk);

        SLSAlphaE = new DoublePlus( 15.0, "SLSAlphaE", m_unitMeasure, UnitMeasure::noDimension, false );
        SLSAlphaE->setRichName( QObject::trUtf8("α<span style=\" vertical-align:sub;\">e</span>") );
        SLSAlphaE->setToolTip( QObject::trUtf8("Verifica a pressoflessione SLE - Coefficiente di omogeneizzazione"));
        addVarToContainer(SLSAlphaE);

        SLSSigmaCUp = new DoublePlus( 0.0, "SLSSigmaCUp", m_unitMeasure, UnitMeasure::tension, true );
        SLSSigmaCUp->setRichName( QObject::trUtf8("σ<span style=\" vertical-align:sub;\">c,sup</span>") );
        SLSSigmaCUp->setToolTip( QObject::trUtf8("Verifica a pressoflessione SLE - Tensione minima nel cls"));
        SLSSigmaCUp->setReadOnly( true );
        addVarToContainer(SLSSigmaCUp);

        SLSSigmaCDown = new DoublePlus( 0.0, "SLSSigmaCDown", m_unitMeasure, UnitMeasure::tension, true );
        SLSSigmaCDown->setRichName( QObject::trUtf8("σ<span style=\" vertical-align:sub;\">c,inf</span>") );
        SLSSigmaCDown->setToolTip( QObject::trUtf8("Verifica a pressoflessione SLE - Tensione massima nel cls"));
        SLSSigmaCDown->setReadOnly( true );
        addVarToContainer(SLSSigmaCDown);

        SLSx = new DoublePlus( 0.0, "SLSx", m_unitMeasure, UnitMeasure::sectL, true );
        SLSx->setRichName( QObject::trUtf8("x") );
        SLSx->setToolTip( QObject::trUtf8("Verifica a pressoflessione SLE - Distanza dell'asse neutro dal lembo superiore compresso"));
        SLSx->setReadOnly( true );
        addVarToContainer(SLSx);

        bVULS = new DoublePlus( 0.0, "bVULS", m_unitMeasure, UnitMeasure::sectL, true );
        bVULS->setRichName( QObject::trUtf8("b<span style=\" vertical-align:sub;\">w</span>") );
        bVULS->setToolTip( QObject::trUtf8("Verifica a taglio - Larghezza minima dell'area tesa"));
        addVarToContainer(bVULS);

        dVULS = new DoublePlus( 0.0, "dVULS", m_unitMeasure, UnitMeasure::sectL, true );
        dVULS->setRichName( QObject::trUtf8("d") );
        dVULS->setToolTip( QObject::trUtf8("Verifica a taglio - Altezza utile"));
        addVarToContainer(dVULS);
    }
    ~SimpleSectionRCncrCompGUIPrivate(){
        delete ui;
    }

    Ui::SimpleSectionRCncrCompGUI *ui;

    SimpleSectionRCncrComp * sect;

    DoublePlus * MRdNEd;
    DoublePlus * MRdPlus;
    DoublePlus * xMRdPlus;
    DoublePlus * MRdMinus;
    DoublePlus * xMRdMinus;
    DoublePlus * MChiNEd;

    DoublePlus * SLSNEk;
    DoublePlus * SLSMEk;
    DoublePlus * SLSAlphaE;
    DoublePlus * SLSSigmaCUp;
    DoublePlus * SLSSigmaCDown;
    DoublePlus * SLSx;
    SLSSteelSigmaModel * SLSSigmaSModel;

    DoublePlus * bVULS;
    DoublePlus * dVULS;
};

SimpleSectionRCncrCompGUI::SimpleSectionRCncrCompGUI(UnitMeasure * ump, QWidget *parent) :
    QWidget(parent),
    m_d( new SimpleSectionRCncrCompGUIPrivate( ump ) ) {

    m_d->sect = new SimpleSectionRCncrComp(ump, "", this);

    m_d->sect->cncrRectModel()->insertRows(0, 2);
    m_d->sect->cncrRectModel()->b(0)->setValue( 500.0 );
    m_d->sect->cncrRectModel()->t(0)->setValue( 50.0 );
    m_d->sect->cncrRectModel()->b(1)->setValue( 100.0 );
    m_d->sect->cncrRectModel()->t(1)->setValue( 200.0 );
    m_d->sect->steelAModel()->insertRows(0,1);
    m_d->sect->steelAModel()->d(0)->setValue( 220.0 );
    m_d->sect->steelAModel()->A(0)->setValue( 2000.0 );

    m_d->SLSSigmaSModel = new SLSSteelSigmaModel( m_d->sect->steelAModel(), ump );

    m_d->ui->setupUi(this);
    connectVar();

    connect( m_d->ui->MChiPushButton, SIGNAL(clicked()), this, SLOT(calcMChi()));
    connect( m_d->ui->NRdMRdPushButton, SIGNAL(clicked()), this, SLOT(calcNRdMRd()));

    m_d->ui->cncrRectTableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    connect( m_d->ui->addCncrRectPushButton, SIGNAL(clicked()), this, SLOT(addCncrRect()));
    connect( m_d->ui->delCncrRectPushButton, SIGNAL(clicked()), this, SLOT(delCncrRect()));

    m_d->ui->steelATableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    connect( m_d->ui->addSteelAPushButton, SIGNAL(clicked()), this, SLOT(addSteelA()));
    connect( m_d->ui->delSteelAPushButton, SIGNAL(clicked()), this, SLOT(delSteelA()));

    connect( m_d->sect, SIGNAL(sectionChanged()), this, SLOT(setMRd()) );
    connect( m_d->MRdNEd, SIGNAL(valueChanged(QString)), this, SLOT(setMRd()) );
    setMRd();

    connect( m_d->sect, SIGNAL(sectionChanged()), this, SLOT(setSLSValues()) );
    connect( m_d->SLSMEk, SIGNAL(valueChanged(QString)), this, SLOT(setSLSValues()) );
    connect( m_d->SLSNEk, SIGNAL(valueChanged(QString)), this, SLOT(setSLSValues()) );
    connect( m_d->SLSAlphaE, SIGNAL(valueChanged(QString)), this, SLOT(setSLSValues()) );
    setSLSValues();
}

SimpleSectionRCncrCompGUI::~SimpleSectionRCncrCompGUI() {
    delete m_d;
}

void SimpleSectionRCncrCompGUI::calcMChi() {
    QCustomPlotDialog d( trUtf8("Momento/Curvatura"), this );
    d.setCurveData( 0, m_d->sect->MChiDiagram( m_d->MChiNEd ) );
    d.exec();
}

void SimpleSectionRCncrCompGUI::calcNRdMRd() {
    QCustomPlotDialog d( trUtf8("Diagramma N-M"), this );
    d.setCurveData( 0, m_d->sect->NRdMRd() );
    d.setXAxisInverted( true );
    d.exec();
}

void SimpleSectionRCncrCompGUI::addCncrRect() {
    if( m_d->ui->cncrRectTableView->selectionModel() ){
        QList<int> selRows;
        QModelIndexList selectedIndexes = m_d->ui->cncrRectTableView->selectionModel()->selectedIndexes();
        for( QModelIndexList::iterator i = selectedIndexes.begin(); i != selectedIndexes.end(); ++i ){
            if( !selRows.contains((*i).row()) ){
                selRows.append( (*i).row() );
            }
        }
        qSort( selRows );
        if( selRows.isEmpty() ){
            m_d->sect->cncrRectModel()->insertRows( 0, 1 );
        } else {
            m_d->sect->cncrRectModel()->insertRows( selRows.last()+1, selRows.size() );
        }
    }
}


void SimpleSectionRCncrCompGUI::delCncrRect() {
    if( m_d->ui->cncrRectTableView->selectionModel() ){
        QModelIndexList selectedRows = m_d->ui->cncrRectTableView->selectionModel()->selectedRows();
        if( selectedRows.size() > 0 ){
            int p = m_d->sect->cncrRectModel()->count();
            for( int i=0; i < selectedRows.size(); ++i ){
                if( (selectedRows.at(i).isValid()) && (selectedRows.at(i).row() < p) ){
                    p = selectedRows.at(i).row();
                }
            }
            m_d->sect->cncrRectModel()->removeRows( p, selectedRows.size() );
        }
    }
}

void SimpleSectionRCncrCompGUI::addSteelA() {
    if( m_d->ui->steelATableView->selectionModel() ){
        QList<int> selRows;
        QModelIndexList selectedIndexes = m_d->ui->steelATableView->selectionModel()->selectedIndexes();
        for( QModelIndexList::iterator i = selectedIndexes.begin(); i != selectedIndexes.end(); ++i ){
            if( !selRows.contains((*i).row()) ){
                selRows.append( (*i).row() );
            }
        }
        qSort( selRows );
        if( selRows.isEmpty() ){
            m_d->sect->steelAModel()->insertRows( 0, 1 );
        } else {
            m_d->sect->steelAModel()->insertRows( selRows.last()+1, selRows.size() );
        }
    }
}


void SimpleSectionRCncrCompGUI::delSteelA() {
    if( m_d->ui->steelATableView->selectionModel() ){
        QModelIndexList selectedRows = m_d->ui->steelATableView->selectionModel()->selectedRows();
        if( selectedRows.size() > 0 ){
            int p = m_d->sect->steelAModel()->count();
            for( int i=0; i < selectedRows.size(); ++i ){
                if( (selectedRows.at(i).isValid()) && (selectedRows.at(i).row() < p) ){
                    p = selectedRows.at(i).row();
                }
            }
            m_d->sect->steelAModel()->removeRows( p, selectedRows.size() );
        }
    }
}

void SimpleSectionRCncrCompGUI::setMRd() {
    m_d->sect->MRd( m_d->MRdPlus, m_d->MRdNEd, m_d->xMRdPlus );
    m_d->sect->MRd( m_d->MRdMinus, m_d->MRdNEd, m_d->xMRdMinus, true );
}

void SimpleSectionRCncrCompGUI::setSLSValues(){
    m_d->sect->SLSNM( m_d->SLSNEk, m_d->SLSMEk, m_d->SLSAlphaE, m_d->SLSSigmaSModel, m_d->SLSSigmaCUp, m_d->SLSSigmaCDown, m_d->SLSx );
}

void SimpleSectionRCncrCompGUI::connectVar() {
    m_d->ui->cncrRectTableView->setModel( m_d->sect->cncrRectModel() );
    VarPlusGUI::connectVar( m_d->sect->hTot, m_d->ui->hTotLabel, m_d->ui->hTotLEdit, m_d->ui->hTotUMLabel );
    VarPlusGUI::connectVar( m_d->sect->hG, m_d->ui->hGLabel, m_d->ui->hGLEdit, m_d->ui->hGUMLabel );
    VarPlusGUI::connectVar( m_d->sect->fck, m_d->ui->fckLabel, m_d->ui->fckLEdit, m_d->ui->fckUMLabel );
    VarPlusGUI::connectVar( m_d->sect->gammaC, m_d->ui->gammaCLabel, m_d->ui->gammaCLEdit, m_d->ui->gammaCUMLabel );
    VarPlusGUI::connectVar( m_d->sect->alphaCC, m_d->ui->alphaCCLabel, m_d->ui->alphaCCLEdit, m_d->ui->alphaCCUMLabel );
    VarPlusGUI::connectVar( m_d->sect->fcd, m_d->ui->fcdLabel, m_d->ui->fcdLEdit, m_d->ui->fcdUMLabel, m_d->ui->fcdCheckBox );
    VarPlusGUI::connectVar( m_d->sect->epsC2, m_d->ui->epsC2Label, m_d->ui->epsC2LEdit, m_d->ui->epsC2UMLabel, m_d->ui->epsC2CheckBox );
    VarPlusGUI::connectVar( m_d->sect->epsCU2, m_d->ui->epsCU2Label, m_d->ui->epsCU2LEdit, m_d->ui->epsCU2UMLabel, m_d->ui->epsCU2CheckBox );
    VarPlusGUI::connectVar( m_d->sect->nC, m_d->ui->nCLabel, m_d->ui->nCLEdit, m_d->ui->nCUMLabel, m_d->ui->nCCheckBox );

    VarPlusGUI::connectVar( m_d->sect->Es, m_d->ui->EsLabel, m_d->ui->EsLEdit, m_d->ui->EsUMLabel );
    VarPlusGUI::connectVar( m_d->sect->epsSUk, m_d->ui->epsSUkLabel, m_d->ui->epsSUkLEdit, m_d->ui->epsSUkUMLabel );
    VarPlusGUI::connectVar( m_d->sect->epsSUd, m_d->ui->epsSUdLabel, m_d->ui->epsSUdLEdit, m_d->ui->epsSUdUMLabel, m_d->ui->epsSUdCheckBox );
    VarPlusGUI::connectVar( m_d->sect->fSyk, m_d->ui->fSykLabel, m_d->ui->fSykLEdit, m_d->ui->fSykUMLabel );
    VarPlusGUI::connectVar( m_d->sect->gammaS, m_d->ui->gammaSLabel, m_d->ui->gammaSLEdit, m_d->ui->gammaSUMLabel );
    VarPlusGUI::connectVar( m_d->sect->fSyd, m_d->ui->fSydLabel, m_d->ui->fSydLEdit, m_d->ui->fSydUMLabel, m_d->ui->fSydCheckBox );
    VarPlusGUI::connectVar( m_d->sect->kS, m_d->ui->kSLabel, m_d->ui->kSLEdit, m_d->ui->kSUMLabel );
    VarPlusGUI::connectVar( m_d->sect->fSud, m_d->ui->fSudLabel, m_d->ui->fSudLEdit, m_d->ui->fSudUMLabel, m_d->ui->fSudCheckBox );
    VarPlusGUI::connectEnumVar( m_d->sect->steelFEpsModel, m_d->ui->steelFEpsModelLabel, m_d->ui->steelFEpsModelComboBox );

    VarPlusGUI::connectVar( m_d->sect->NRdMin, m_d->ui->NRdMinLabel, m_d->ui->NRdMinLEdit, m_d->ui->NRdMinUMLabel );
    VarPlusGUI::connectVar( m_d->sect->NRdMax, m_d->ui->NRdMaxLabel, m_d->ui->NRdMaxLEdit, m_d->ui->NRdMaxUMLabel );

    VarPlusGUI::connectVar( m_d->MRdNEd, m_d->ui->MRdNEdLabel, m_d->ui->MRdNEdLEdit, m_d->ui->MRdNEdUMLabel );
    VarPlusGUI::connectVar( m_d->MRdPlus, m_d->ui->MRdPlusLabel, m_d->ui->MRdPlusLEdit, m_d->ui->MRdPlusUMLabel );
    VarPlusGUI::connectVar( m_d->xMRdPlus, m_d->ui->xMRdPlusLabel, m_d->ui->xMRdPlusLEdit, m_d->ui->xMRdPlusUMLabel );
    VarPlusGUI::connectVar( m_d->MRdMinus, m_d->ui->MRdMinusLabel, m_d->ui->MRdMinusLEdit, m_d->ui->MRdMinusUMLabel );
    VarPlusGUI::connectVar( m_d->xMRdMinus, m_d->ui->xMRdMinusLabel, m_d->ui->xMRdMinusLEdit, m_d->ui->xMRdMinusUMLabel );

    VarPlusGUI::connectVar( m_d->MChiNEd, m_d->ui->MChiNEdLabel, m_d->ui->MChiNEdLEdit, m_d->ui->MChiNEdUMLabel );

    m_d->ui->steelATableView->setModel( m_d->sect->steelAModel() );
    VarPlusGUI::connectVar( m_d->sect->dMax, m_d->ui->dMaxLabel, m_d->ui->dMaxLEdit, m_d->ui->dMaxUMLabel );

    VarPlusGUI::connectVar( m_d->SLSNEk, m_d->ui->SLSNEkLabel, m_d->ui->SLSNEkLineEdit, m_d->ui->SLSNEkUMLabel );
    VarPlusGUI::connectVar( m_d->SLSMEk, m_d->ui->SLSMEkLabel, m_d->ui->SLSMEkLineEdit, m_d->ui->SLSMEkUMLabel );
    VarPlusGUI::connectVar( m_d->SLSAlphaE, m_d->ui->SLSAlphaELabel, m_d->ui->SLSAlphaELineEdit, m_d->ui->SLSAlphaEUMLabel );
    VarPlusGUI::connectVar( m_d->SLSx, m_d->ui->SLSxLabel, m_d->ui->SLSxLineEdit, m_d->ui->SLSxUMLabel );
    VarPlusGUI::connectVar( m_d->SLSSigmaCUp, m_d->ui->SLSSigmaCUpLabel, m_d->ui->SLSSigmaCUpLineEdit, m_d->ui->SLSSigmaCUpUMLabel );
    VarPlusGUI::connectVar( m_d->SLSSigmaCDown, m_d->ui->SLSSigmaCDownLabel, m_d->ui->SLSSigmaCDownLineEdit, m_d->ui->SLSSigmaCDownUMLabel );
    m_d->ui->SLSSteelSigmaTableView->setModel( m_d->SLSSigmaSModel );
}
