#include "simplesectionrcncrrectgui.h"
#include "ui_simplesectionrcncrrectgui.h"

#include "qcustomplotdialog.h"

#include "simplesectionrcncrrect.h"
#include "steelamodel.h"
#include "steelfepsmodel.h"
#include "slssteelsigmamodel.h"

#include "varpluscontainer.h"
#include "doubleplus.h"
#include "varplusguiutility.h"

#include <QClipboard>

#include <cmath>

class SimpleSectionRCncrRectGUIPrivate: public VarPlusContainer {
public:
    SimpleSectionRCncrRectGUIPrivate( UnitMeasure * ump ):
        VarPlusContainer( ump, "SimpleSectionRCncrRectGUIPrivate", ""),
        ui(new Ui::SimpleSectionRCncrRectGUI) {
        MRdNEd = new DoublePlus( 0.0, "MRdNEd", m_unitMeasure, UnitMeasure::loadF, true );
        MRdNEd->setRichName( QObject::tr("N<span style=\" vertical-align:sub;\">Ed</span>") );
        MRdNEd->setToolTip( QObject::tr("Verifica a pressoflessione - Sforzo normale agente") );
        MRdNEd->setReadOnly( false );
        addVarToContainer(MRdNEd);

        MChiNEd = new DoublePlus( 0.0, "MChiNEd", m_unitMeasure, UnitMeasure::loadF, true );
        MChiNEd->setRichName( QObject::tr("N<span style=\" vertical-align:sub;\">Ed</span>") );
        MChiNEd->setToolTip( QObject::tr("Diagramma momento-curvatura - Sforzo normale agente") );
        MChiNEd->setReadOnly( false );
        addVarToContainer(MChiNEd);

        MRdPlus = new DoublePlus( 0.0, "MRdPlus", m_unitMeasure, UnitMeasure::loadM, true );
        MRdPlus->setRichName( QObject::tr("M<span style=\" vertical-align:sub;\">Rd,+</span>") );
        MRdPlus->setToolTip( QObject::tr("Verifica a pressoflessione - Momento resistente positivo"));
        MRdPlus->setReadOnly( true );
        addVarToContainer(MRdPlus);

        xMRdPlus = new DoublePlus( 0.0, "xMRdPlus", m_unitMeasure, UnitMeasure::sectL, true );
        xMRdPlus->setRichName( QObject::tr("x<span style=\" vertical-align:sub;\">MRd,+</span>") );
        xMRdPlus->setToolTip( QObject::tr("Verifica a pressoflessione - Distanza dell'asse neutro dal lembo superiore compresso"));
        xMRdPlus->setReadOnly( true );
        addVarToContainer(xMRdPlus);

        MRdMinus = new DoublePlus( 0.0, "MRdMinus", m_unitMeasure, UnitMeasure::loadM, true );
        MRdMinus->setRichName( QObject::tr("M<span style=\" vertical-align:sub;\">Rd,-</span>") );
        MRdMinus->setToolTip( QObject::tr("Verifica a pressoflessione - Momento resistente negativo"));
        MRdMinus->setReadOnly( true );
        addVarToContainer(MRdMinus);

        xMRdMinus = new DoublePlus( 0.0, "xMRdMinus", m_unitMeasure, UnitMeasure::sectL, true );
        xMRdMinus->setRichName( QObject::tr("x<span style=\" vertical-align:sub;\">MRd,-</span>") );
        xMRdMinus->setToolTip( QObject::tr("Verifica a pressoflessione - Distanza dell'asse neutro dal lembo inferiore compresso"));
        xMRdMinus->setReadOnly( true );
        addVarToContainer(xMRdMinus);

        bVULS = new DoublePlus( 0.0, "bVULS", m_unitMeasure, UnitMeasure::sectL, true );
        bVULS->setRichName( QObject::tr("b<span style=\" vertical-align:sub;\">w</span>") );
        bVULS->setToolTip( QObject::tr("Verifica a taglio - Larghezza minima dell'area tesa"));
        addVarToContainer(bVULS);

        dVULS = new DoublePlus( 0.0, "dVULS", m_unitMeasure, UnitMeasure::sectL, true );
        dVULS->setRichName( QObject::tr("d") );
        dVULS->setToolTip( QObject::tr("Verifica a taglio - Altezza utile"));
        addVarToContainer(dVULS);

        sigmaCPNoSteelVULS = new DoublePlus( 0.0, "sigmaCPNoSteelVULS", m_unitMeasure, UnitMeasure::tension );
        sigmaCPNoSteelVULS->setRichName( QObject::tr("σ<span style=\" vertical-align:sub;\">cp</span>") );
        sigmaCPNoSteelVULS->setToolTip( QObject::tr("Verifica a taglio in sezione priva di armatura a taglio - Tensione di compressione"));
        addVarToContainer(sigmaCPNoSteelVULS);

        ASLNoSteelVULS = new DoublePlus( 0.0, "sigmaCPNoSteelVULS", m_unitMeasure, UnitMeasure::sectL2 );
        ASLNoSteelVULS->setRichName( QObject::tr("A<span style=\" vertical-align:sub;\">sl</span>") );
        ASLNoSteelVULS->setToolTip( QObject::tr("Verifica a taglio in sezione priva di armatura a taglio - Armatura longitudinale"));
        addVarToContainer(ASLNoSteelVULS);

        VRdNoSteelVULS = new DoublePlus( 0.0, "VRdNoSteelVULS", m_unitMeasure, UnitMeasure::loadF, true );
        VRdNoSteelVULS->setRichName( QObject::tr("V<span style=\" vertical-align:sub;\">Rd,c</span>") );
        VRdNoSteelVULS->setToolTip( QObject::tr("Verifica a taglio in sezione priva di armatura a taglio - Taglio resistente"));
        addVarToContainer(VRdNoSteelVULS);

        // staffe 4 bracci diam. 8
        ASWSteelVULS = new DoublePlus( M_PI * 1.60e-5 * 4.0, "ASWSteelVULS", m_unitMeasure, UnitMeasure::sectL2 );
        ASWSteelVULS->setRichName( QObject::tr("A<span style=\" vertical-align:sub;\">sw</span>") );
        ASWSteelVULS->setToolTip( QObject::tr("Verifica a taglio in sezione con armatura a taglio - Armatura trasversale"));
        addVarToContainer(ASWSteelVULS);

        sSteelVULS = new DoublePlus( 0.20, "sSteelVULS", m_unitMeasure, UnitMeasure::sectL );
        sSteelVULS->setRichName( QObject::tr("s<span style=\" vertical-align:sub;\">V</span>") );
        sSteelVULS->setToolTip( QObject::tr("Verifica a taglio in sezione con armatura a taglio - Passo staffe"));
        addVarToContainer(sSteelVULS);

        thetaVULS = new DoublePlus( M_PI/4.0, "thetaVULS", m_unitMeasure, UnitMeasure::angle, true );
        thetaVULS->setRichName( QObject::tr("θ<span style=\" vertical-align:sub;\">V</span>") );
        thetaVULS->setToolTip( QObject::tr("Verifica a taglio in sezione con armatura a taglio - Inclinazione bielle compresse rispetto ad asse trave"));
        addVarToContainer(thetaVULS);

        alphaVULS = new DoublePlus( M_PI/2.0, "alphaVULS", m_unitMeasure, UnitMeasure::angle );
        alphaVULS->setRichName( QObject::tr("α") );
        alphaVULS->setToolTip( QObject::tr("Verifica a taglio in sezione con armatura a taglio - Inclinazione armatura taglio rispetto ad asse trave"));
        addVarToContainer(alphaVULS);

        VRdMaxSteelVULS = new DoublePlus( 0.0, "VRdMaxSteelVULS", m_unitMeasure, UnitMeasure::loadF, true );
        VRdMaxSteelVULS->setRichName( QObject::tr("V<span style=\" vertical-align:sub;\">Rd,max</span>") );
        VRdMaxSteelVULS->setToolTip( QObject::tr("Verifica a taglio in sezione con armatura a taglio - Taglio resistente lato cls"));
        addVarToContainer(VRdMaxSteelVULS);

        VRdSteelSteelVULS = new DoublePlus( 0.0, "VRdSteelSteelVULS", m_unitMeasure, UnitMeasure::loadF, true );
        VRdSteelSteelVULS->setRichName( QObject::tr("V<span style=\" vertical-align:sub;\">Rd,s</span>") );
        VRdSteelSteelVULS->setToolTip( QObject::tr("Verifica a taglio in sezione con armatura a taglio - Taglio resistente lato acciaio"));
        addVarToContainer(VRdSteelSteelVULS);

        VRdSteelVULS = new DoublePlus( 0.0, "VRdSteelVULS", m_unitMeasure, UnitMeasure::loadF, true );
        VRdSteelVULS->setRichName( QObject::tr("V<span style=\" vertical-align:sub;\">Rd</span>") );
        VRdSteelVULS->setToolTip( QObject::tr("Verifica a taglio in sezione con armatura a taglio - Taglio resistente"));
        addVarToContainer(VRdSteelVULS);

        ASWTULS = new DoublePlus( M_PI * 1.60e-5 * 4.0, "ASWTULS", m_unitMeasure, UnitMeasure::sectL2 );
        ASWTULS->setRichName( QObject::tr("A<span style=\" vertical-align:sub;\">sw</span>") );
        ASWTULS->setToolTip( QObject::tr("Verifica a torsione in sezione con armatura a torsione - Armatura trasversale staffe"));
        addVarToContainer(ASWTULS);

        sTULS = new DoublePlus( 0.20, "sSteelVULS", m_unitMeasure, UnitMeasure::sectL );
        sTULS->setRichName( QObject::tr("s<span style=\" vertical-align:sub;\">T</span>") );
        sTULS->setToolTip( QObject::tr("Verifica a torsione in sezione con armatura a torsione - Passo staffe"));
        addVarToContainer(sTULS);

        thetaTULS = new DoublePlus( M_PI/4.0, "thetaTULS", m_unitMeasure, UnitMeasure::angle, false );
        thetaTULS->setRichName( QObject::tr("θ<span style=\" vertical-align:sub;\">T</span>") );
        thetaTULS->setToolTip( QObject::tr("Verifica a torsione in sezione con armatura a torsione - Inclinazione bielle compresse rispetto ad asse trave"));
        addVarToContainer(thetaTULS);

        TRdCncrTULS = new DoublePlus( 0.0, "TRdCncrTULS", m_unitMeasure, UnitMeasure::loadM, true );
        TRdCncrTULS->setRichName( QObject::tr("T<span style=\" vertical-align:sub;\">Rd,c</span>") );
        TRdCncrTULS->setToolTip( QObject::tr("Verifica a torsione in sezione con armatura a torsione - Torsione resistente lato cls"));
        addVarToContainer(TRdCncrTULS);

        TRdSteelTULS = new DoublePlus( 0.0, "TRdSteelTULS", m_unitMeasure, UnitMeasure::loadM, true );
        TRdSteelTULS->setRichName( QObject::tr("T<span style=\" vertical-align:sub;\">Rd,sw</span>") );
        TRdSteelTULS->setToolTip( QObject::tr("Verifica a torsione in sezione con armatura a torsione - Torsione resistente lato staffe in acciaio"));
        addVarToContainer(TRdSteelTULS);

        TRdTULS = new DoublePlus( 0.0, "TRdTULS", m_unitMeasure, UnitMeasure::loadM, true );
        TRdTULS->setRichName( QObject::tr("T<span style=\" vertical-align:sub;\">Rd</span>") );
        TRdTULS->setToolTip( QObject::tr("Verifica a torsione in sezione con armatura a torsione - Torsione resistente"));
        addVarToContainer(TRdTULS);

        SLSNEk = new DoublePlus( 0.0, "SLSNEk", m_unitMeasure, UnitMeasure::loadF, false );
        SLSNEk->setRichName( QObject::tr("N<span style=\" vertical-align:sub;\">Ek</span>") );
        SLSNEk->setToolTip( QObject::tr("Verifica a pressoflessione SLE - Sforzo normale agente"));
        addVarToContainer(SLSNEk);

        SLSMEk = new DoublePlus( 0.0, "SLSMEk", m_unitMeasure, UnitMeasure::loadM, false );
        SLSMEk->setRichName( QObject::tr("M<span style=\" vertical-align:sub;\">Ek</span>") );
        SLSMEk->setToolTip( QObject::tr("Verifica a pressoflessione SLE - Momento agente"));
        addVarToContainer(SLSMEk);

        SLSAlphaE = new DoublePlus( 15.0, "SLSAlphaE", m_unitMeasure, UnitMeasure::noDimension, false );
        SLSAlphaE->setRichName( QObject::tr("α<span style=\" vertical-align:sub;\">e</span>") );
        SLSAlphaE->setToolTip( QObject::tr("Verifica a pressoflessione SLE - Coefficiente di omogeneizzazione"));
        addVarToContainer(SLSAlphaE);

        SLSSigmaCUp = new DoublePlus( 0.0, "SLSSigmaCUp", m_unitMeasure, UnitMeasure::tension, true );
        SLSSigmaCUp->setRichName( QObject::tr("σ<span style=\" vertical-align:sub;\">c,sup</span>") );
        SLSSigmaCUp->setToolTip( QObject::tr("Verifica a pressoflessione SLE - Tensione minima nel cls"));
        SLSSigmaCUp->setReadOnly( true );
        addVarToContainer(SLSSigmaCUp);

        SLSSigmaCDown = new DoublePlus( 0.0, "SLSSigmaCDown", m_unitMeasure, UnitMeasure::tension, true );
        SLSSigmaCDown->setRichName( QObject::tr("σ<span style=\" vertical-align:sub;\">c,inf</span>") );
        SLSSigmaCDown->setToolTip( QObject::tr("Verifica a pressoflessione SLE - Tensione massima nel cls"));
        SLSSigmaCDown->setReadOnly( true );
        addVarToContainer(SLSSigmaCDown);

        SLSx = new DoublePlus( 0.0, "SLSx", m_unitMeasure, UnitMeasure::sectL, true );
        SLSx->setRichName( QObject::tr("x") );
        SLSx->setToolTip( QObject::tr("Verifica a pressoflessione SLE - Distanza dell'asse neutro dal lembo superiore compresso"));
        SLSx->setReadOnly( true );
        addVarToContainer(SLSx);
    }
    ~SimpleSectionRCncrRectGUIPrivate(){
        delete ui;
    }

    Ui::SimpleSectionRCncrRectGUI *ui;

    SimpleSectionRCncrRect * sect;

    DoublePlus * MRdNEd;
    DoublePlus * MRdPlus;
    DoublePlus * xMRdPlus;
    DoublePlus * MRdMinus;
    DoublePlus * xMRdMinus;
    DoublePlus * MChiNEd;

    DoublePlus * bVULS;
    DoublePlus * dVULS;
    DoublePlus * sigmaCPNoSteelVULS;
    DoublePlus * ASLNoSteelVULS;
    DoublePlus * VRdNoSteelVULS;
    DoublePlus * ASWSteelVULS;
    DoublePlus * sSteelVULS;
    DoublePlus * alphaVULS;
    DoublePlus * thetaVULS;
    DoublePlus * VRdMaxSteelVULS;
    DoublePlus * VRdSteelSteelVULS;
    DoublePlus * VRdSteelVULS;

    DoublePlus * ASWTULS;
    DoublePlus * sTULS;
    DoublePlus * thetaTULS;
    DoublePlus * TRdCncrTULS;
    DoublePlus * TRdSteelTULS;
    DoublePlus * TRdTULS;

    DoublePlus * SLSNEk;
    DoublePlus * SLSMEk;
    DoublePlus * SLSAlphaE;
    DoublePlus * SLSSigmaCUp;
    DoublePlus * SLSSigmaCDown;
    DoublePlus * SLSx;
    SLSSteelSigmaModel * SLSSigmaSModel;
};

SimpleSectionRCncrRectGUI::SimpleSectionRCncrRectGUI(UnitMeasure * ump, QWidget *parent) :
    QWidget(parent),
    m_d( new SimpleSectionRCncrRectGUIPrivate( ump ) ) {
    m_d->sect = new SimpleSectionRCncrRect(ump, "", this);
    m_d->sect->b->setValue( 400.0 );
    m_d->sect->h->setValue( 500.0 );
    m_d->sect->steelAModel()->insertRows(0,2);
    m_d->sect->steelAModel()->d(0)->setValue( 50.0 );
    m_d->sect->steelAModel()->A(0)->setValue( 314.0 );
    m_d->sect->steelAModel()->d(1)->setValue( 450.0 );
    m_d->sect->steelAModel()->A(1)->setValue( 314.0 );

    m_d->SLSSigmaSModel = new SLSSteelSigmaModel( m_d->sect->steelAModel(), ump );

    m_d->ui->setupUi(this);
    connectVar();

    connect( m_d->ui->MChiPushButton, &QPushButton::clicked, this, &SimpleSectionRCncrRectGUI::calcMChi );
    connect( m_d->ui->MChiCopyToClipboardPushButton, &QPushButton::clicked, this, &SimpleSectionRCncrRectGUI::copyToClipboardMChi );
    connect( m_d->ui->NRdMRdPushButton, &QPushButton::clicked, this, &SimpleSectionRCncrRectGUI::calcNRdMRd );
    connect( m_d->ui->NRdMRdCopyToClipboardPushButton, &QPushButton::clicked, this, &SimpleSectionRCncrRectGUI::copyToClipboardNRdMRd );

    m_d->ui->steelATableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    connect( m_d->ui->addSteelAPushButton, &QPushButton::clicked, this, &SimpleSectionRCncrRectGUI::addSteelA );
    connect( m_d->ui->delSteelAPushButton, &QPushButton::clicked, this, &SimpleSectionRCncrRectGUI::delSteelA );

    connect( m_d->sect, &SimpleSectionRCncrRect::sectionChanged, this, &SimpleSectionRCncrRectGUI::setMRd );
    connect( m_d->MRdNEd, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setMRd );
    setMRd();

    connect( m_d->sect, &SimpleSectionRCncrRect::sectionChanged, this, &SimpleSectionRCncrRectGUI::setSLSValues );
    connect( m_d->SLSMEk, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setSLSValues );
    connect( m_d->SLSNEk, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setSLSValues );
    connect( m_d->SLSAlphaE, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setSLSValues );
    setSLSValues();

    connect( m_d->sect->b, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setBVULS );
    connect( m_d->bVULS, &DoublePlus::readOnlyChanged, this, &SimpleSectionRCncrRectGUI::setBVULS );
    setBVULS();

    connect( m_d->sect->dMax, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setDVULS );
    connect( m_d->dVULS, &DoublePlus::readOnlyChanged, this, &SimpleSectionRCncrRectGUI::setDVULS );
    setDVULS();

    connect( m_d->sect, &SimpleSectionRCncrRect::sectionChanged, this, &SimpleSectionRCncrRectGUI::setVULSNoSteel );
    connect( m_d->bVULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setVULSNoSteel );
    connect( m_d->dVULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setVULSNoSteel );
    connect( m_d->ASLNoSteelVULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setVULSNoSteel );
    connect(  m_d->sigmaCPNoSteelVULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setVULSNoSteel );
    setVULSNoSteel();

    connect( m_d->sect, &SimpleSectionRCncrRect::sectionChanged, this, &SimpleSectionRCncrRectGUI::setVULSSteel );
    connect( m_d->bVULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setVULSSteel );
    connect( m_d->dVULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setVULSSteel );
    connect( m_d->ASWSteelVULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setVULSSteel );
    connect( m_d->sSteelVULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setVULSSteel );
    connect( m_d->thetaVULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setVULSSteel );
    connect( m_d->thetaVULS, &DoublePlus::readOnlyChanged, this, &SimpleSectionRCncrRectGUI::setVULSSteel );
    connect( m_d->alphaVULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setVULSSteel );
    setVULSSteel();

    connect( m_d->sect, &SimpleSectionRCncrRect::sectionChanged, this, &SimpleSectionRCncrRectGUI::setTULS );
    connect( m_d->ASWTULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setTULS );
    connect( m_d->sTULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setTULS );
    connect( m_d->thetaTULS, &DoublePlus::valueChanged, this, &SimpleSectionRCncrRectGUI::setTULS );
    connect( m_d->thetaTULS, &DoublePlus::readOnlyChanged, this, &SimpleSectionRCncrRectGUI::setTULS );
    setTULS();
}

SimpleSectionRCncrRectGUI::~SimpleSectionRCncrRectGUI() {
    delete m_d;
}

void SimpleSectionRCncrRectGUI::setBVULS() {
    if( m_d->bVULS->readOnly() ){
        m_d->bVULS->setValueNormal( m_d->sect->b->valueNormal() );
    }
}

void SimpleSectionRCncrRectGUI::setDVULS() {
    if( m_d->dVULS->readOnly() ){
        m_d->dVULS->setValueNormal( m_d->sect->dMax->valueNormal() );
    }
}

void SimpleSectionRCncrRectGUI::calcMChi() {
    QCustomPlotDialog d( tr("Momento/Curvatura"), this );
    d.setCurveData( 0, m_d->sect->MChiDiagram( m_d->MChiNEd ) );
    d.exec();
}

void SimpleSectionRCncrRectGUI::copyToClipboardMChi() {
    QVector<QPointF> data = m_d->sect->MChiDiagram( m_d->MChiNEd );
    QString text;
    for( QPolygonF::iterator i = data.begin(); i != data.end(); ++i ){
        text += QString::number( i->x() ) + QString("\t") + QString::number( i->y() ) + QString("\n");
    }
    QApplication::clipboard()->setText( text);
}

void SimpleSectionRCncrRectGUI::calcNRdMRd() {
    QCustomPlotDialog d( tr("Diagramma N-M"), this );
    d.setCurveData( 0, m_d->sect->NRdMRd() );
    d.setXAxisInverted( true );
    d.exec();
}

void SimpleSectionRCncrRectGUI::copyToClipboardNRdMRd() {
    QVector<QPointF> data = m_d->sect->NRdMRd();
    QString text;
    for( QPolygonF::iterator i = data.begin(); i != data.end(); ++i ){
        text += QString::number( i->x() ) + QString("\t") + QString::number( i->y() ) + QString("\n");
    }
    QApplication::clipboard()->setText( text);
}

void SimpleSectionRCncrRectGUI::addSteelA() {
    if( m_d->ui->steelATableView->selectionModel() ){
        QList<int> selRows;
        QModelIndexList selectedIndexes = m_d->ui->steelATableView->selectionModel()->selectedIndexes();
        for( QModelIndexList::iterator i = selectedIndexes.begin(); i != selectedIndexes.end(); ++i ){
            if( !selRows.contains((*i).row()) ){
                selRows.append( (*i).row() );
            }
        }
        std::sort( selRows.begin(), selRows.end() );
        if( selRows.isEmpty() ){
            m_d->sect->steelAModel()->insertRows( 0, 1 );
        } else {
            m_d->sect->steelAModel()->insertRows( selRows.last()+1, selRows.size() );
        }
    }
}

void SimpleSectionRCncrRectGUI::delSteelA() {
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

void SimpleSectionRCncrRectGUI::setMRd() {
    qWarning("sectionCHanged");
    m_d->sect->MRd( m_d->MRdPlus, m_d->MRdNEd, m_d->xMRdPlus );
    m_d->sect->MRd( m_d->MRdMinus, m_d->MRdNEd, m_d->xMRdMinus, true );
}

void SimpleSectionRCncrRectGUI::setSLSValues(){
    m_d->sect->SLSNM( m_d->SLSNEk, m_d->SLSMEk, m_d->SLSAlphaE, m_d->SLSSigmaSModel, m_d->SLSSigmaCUp, m_d->SLSSigmaCDown, m_d->SLSx );
}

void SimpleSectionRCncrRectGUI::setVULSNoSteel(){
    m_d->sect->ULSVNoSteel( m_d->VRdNoSteelVULS, m_d->bVULS, m_d->dVULS, m_d->ASLNoSteelVULS, m_d->sigmaCPNoSteelVULS );
}

void SimpleSectionRCncrRectGUI::setVULSSteel(){
    if( m_d->thetaVULS->readOnly() ){
        m_d->sect->ULSVSteelMax( m_d->VRdSteelVULS, m_d->VRdMaxSteelVULS, m_d->VRdSteelSteelVULS, m_d->bVULS, m_d->dVULS, m_d->ASWSteelVULS, m_d->sSteelVULS, m_d->thetaVULS, m_d->alphaVULS );
    } else {
        m_d->sect->ULSVSteel( m_d->VRdSteelVULS, m_d->VRdMaxSteelVULS, m_d->VRdSteelSteelVULS, m_d->bVULS, m_d->dVULS, m_d->ASWSteelVULS, m_d->sSteelVULS, m_d->thetaVULS, m_d->alphaVULS );
    }
}

void SimpleSectionRCncrRectGUI::setTULS(){
    if( m_d->thetaTULS->readOnly() ){
        m_d->sect->ULSTMax( m_d->TRdTULS, m_d->TRdCncrTULS, m_d->TRdSteelTULS, m_d->ASWTULS, m_d->sTULS, m_d->thetaTULS );
    } else {
        m_d->sect->ULST( m_d->TRdTULS, m_d->TRdCncrTULS, m_d->TRdSteelTULS, m_d->ASWTULS, m_d->sTULS, m_d->thetaTULS );
    }
}

void SimpleSectionRCncrRectGUI::connectVar() {
    VarPlusGUI::connectVar( m_d->sect->b, m_d->ui->bLabel, m_d->ui->bLEdit, m_d->ui->bUMLabel );
    VarPlusGUI::connectVar( m_d->sect->h, m_d->ui->hLabel, m_d->ui->hLEdit, m_d->ui->hUMLabel );
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

    VarPlusGUI::connectVar( m_d->bVULS, m_d->ui->bVULSLabel, m_d->ui->bVULSLEdit, m_d->ui->bVULSUMLabel, m_d->ui->bVUlsCheckBox );
    VarPlusGUI::connectVar( m_d->dVULS, m_d->ui->dVULSLabel, m_d->ui->dVULSLEdit, m_d->ui->dVULSUMLabel, m_d->ui->dVUlsCheckBox );
    VarPlusGUI::connectVar( m_d->sigmaCPNoSteelVULS, m_d->ui->sigmaCPNoSteelVULSLabel, m_d->ui->sigmaCPNoSteelVULSLEdit, m_d->ui->sigmaCPNoSteelVULSUMLabel );
    VarPlusGUI::connectVar( m_d->ASLNoSteelVULS, m_d->ui->ASLNoSteelVULSLabel, m_d->ui->ASLNoSteelVULSLEdit, m_d->ui->ASLNoSteelVULSUMLabel );
    VarPlusGUI::connectVar( m_d->VRdNoSteelVULS, m_d->ui->VRdNoSteelVULSLabel, m_d->ui->VRdNoSteelVULSLEdit, m_d->ui->VRdNoSteelVULSUMLabel );
    VarPlusGUI::connectVar( m_d->ASWSteelVULS, m_d->ui->ASWSteelVULSLabel, m_d->ui->ASWSteelVULSLEdit, m_d->ui->ASWSteelVULSUMLabel );
    VarPlusGUI::connectVar( m_d->sSteelVULS, m_d->ui->sSteelVULSLabel, m_d->ui->sSteelVULSLEdit, m_d->ui->sSteelVULSUMLabel );
    VarPlusGUI::connectVar( m_d->alphaVULS, m_d->ui->alphaVULSLabel, m_d->ui->alphaVULSLEdit, m_d->ui->alphaVULSUMLabel );
    VarPlusGUI::connectVar( m_d->thetaVULS, m_d->ui->thetaVULSLabel, m_d->ui->thetaVULSLEdit, m_d->ui->thetaVULSUMLabel, m_d->ui->thetaVULSCheckBox );
    VarPlusGUI::connectVar( m_d->VRdMaxSteelVULS, m_d->ui->VRdMaxSteelVULSLabel, m_d->ui->VRdMaxSteelVULSLEdit, m_d->ui->VRdMaxSteelVULSUMLabel );
    VarPlusGUI::connectVar( m_d->VRdSteelSteelVULS, m_d->ui->VRdSteelSteelVULSLabel, m_d->ui->VRdSteelSteelVULSLEdit, m_d->ui->VRdSteelSteelVULSUMLabel );
    VarPlusGUI::connectVar( m_d->VRdSteelVULS, m_d->ui->VRdSteelVULSLabel, m_d->ui->VRdSteelVULSLEdit, m_d->ui->VRdSteelVULSUMLabel );

    VarPlusGUI::connectVar( m_d->ASWTULS, m_d->ui->ASWTULSLabel, m_d->ui->ASWTULSLEdit, m_d->ui->ASWTULSUMLabel );
    VarPlusGUI::connectVar( m_d->sTULS, m_d->ui->sTULSLabel, m_d->ui->sTULSLEdit, m_d->ui->sTULSUMLabel );
    VarPlusGUI::connectVar( m_d->thetaTULS, m_d->ui->thetaTULSLabel, m_d->ui->thetaTULSLEdit, m_d->ui->thetaTULSUMLabel, m_d->ui->thetaTULSCheckBox );
    VarPlusGUI::connectVar( m_d->TRdCncrTULS, m_d->ui->TRdCncrTULSLabel, m_d->ui->TRdCncrTULSLEdit, m_d->ui->TRdCncrTULSUMLabel );
    VarPlusGUI::connectVar( m_d->TRdSteelTULS, m_d->ui->TRdSteelTULSLabel, m_d->ui->TRdSteelTULSLEdit, m_d->ui->TRdSteelTULSUMLabel );
    VarPlusGUI::connectVar( m_d->TRdSteelTULS, m_d->ui->TRdTULSLabel, m_d->ui->TRdTULSLEdit, m_d->ui->TRdTULSUMLabel );

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
