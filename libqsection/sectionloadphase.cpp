#include "sectionloadphase.h"

#include "sectionloadphasemodel.h"
#include "sectionrcncr.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "qstringplus.h"

class SectionLoadPhasePrivate {
public:
    SectionLoadPhasePrivate(SectionLoadPhaseModel * lModel):
        loadPhaseModel(lModel){
    }

    SectionLoadPhaseModel * loadPhaseModel;
};

SectionLoadPhase::SectionLoadPhase(UnitMeasure * ump, SectionLoadPhaseModel * lmodel, const QString &n, QObject * parent ):
    VarPlusContainerPlus( ump, "SectionLoadPhase", n, parent ),
    m_d(new SectionLoadPhasePrivate(lmodel) ){
    initVar();
}

int SectionLoadPhase::phaseNum() {
    if( m_d->loadPhaseModel != NULL ){
        return m_d->loadPhaseModel->loadPhaseNumber( this );
    }
    return 0;
}

void SectionLoadPhase::initVar() {
    name = new QStringPlus( trUtf8("Fase"), "typeName", false );
    name->setRichName( trUtf8("Fase") );
    addVarToContainer( name );

    NEd = new DoublePlus(0.0, "NEd", m_unitMeasure, UnitMeasure::loadF, false );
    NEd->setToolTip( QObject::trUtf8("Sforzo normale agente"));
    NEd->setRichName( QObject::trUtf8("N<span style=\"vertical-align:sub;\">Ed</span>"));
    addVarToContainer( NEd );

    MEdY = new DoublePlus( 0.0, "MEdY", m_unitMeasure, UnitMeasure::loadM, false );
    MEdY->setRichName( trUtf8("M<span style=\"vertical-align:sub;\">Ed,y</span>") );
    addVarToContainer( MEdY );

    MEdZ = new DoublePlus( 0.0, "MEdZ", m_unitMeasure, UnitMeasure::loadM, false );
    MEdZ->setRichName( trUtf8("M<span style=\"vertical-align:sub;\">Ed,z</span>") );
    addVarToContainer( MEdZ );

    NCen  = new Point2DPlus( "NCen", "NCenY", 0.0, "NCenZ", 0.0, m_unitMeasure, UnitMeasure::sectL);
    addVarToContainer( NCen );
    NCen->y->setToolTip( QObject::trUtf8("Ascissa del polo attorno a cui si calcola il momento"));
    NCen->y->setRichName( QObject::trUtf8("y<span style=\"vertical-align:sub;\">N</span>"));
    NCen->y->setReadOnly( true );
    NCen->z->setToolTip( QObject::trUtf8("Ordinata del polo attorno a cui si calcola il momento"));
    NCen->z->setRichName( QObject::trUtf8("z<span style=\"vertical-align:sub;\">N</span>"));
    NCen->z->setReadOnly( true );

    l = new DoublePlus(0.0, "l", m_unitMeasure, UnitMeasure::deformation, true );
    l->setToolTip( QObject::trUtf8("Deformazione assiale corrispondente"));
    l->setRichName( QObject::trUtf8("λ"));
    addVarToContainer( l );

    my = new DoublePlus(0.0, "my", m_unitMeasure, UnitMeasure::curvature, true );
    my->setToolTip( QObject::trUtf8("Curvatura rispetto ad asse y"));
    my->setRichName( QObject::trUtf8("μ<span style=\"vertical-align:sub;\">y</span>"));
    addVarToContainer( my );

    mz = new DoublePlus(0.0, "LSPlusmzULS", m_unitMeasure, UnitMeasure::curvature, true );
    mz->setToolTip( QObject::trUtf8("Curvatura rispetto ad asse z"));
    mz->setRichName( QObject::trUtf8("μ<span style=\"vertical-align:sub;\">z</span>"));
    addVarToContainer( mz );
}
