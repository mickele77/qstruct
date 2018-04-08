#include "sectionfrp.h"

#include "sectionloadphasemodel.h"
#include "frp.h"
#include "concrete.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "uintplus.h"
#include "qstringplus.h"
#include "structmath.h"

#include <QXmlStreamWriter>
#include <QPointF>
#include <cmath>

class SectionFRPPrivate {
public:
    SectionFRPPrivate( Concrete * cncr ):
        concrete(cncr){
    }
    ~SectionFRPPrivate() {

    }
    Concrete * concrete;
};

SectionFRP::SectionFRP(UnitMeasure * ump, FRP * frp, Concrete *cncr, const QString & nn, QObject * parent ):
    SectionLine( ump, frp, nn, parent ),
    m_d(new SectionFRPPrivate(cncr) ){
    initVar();
}

SectionFRP::SectionFRP(UnitMeasure *ump, const QXmlStreamAttributes &attrs,
                       MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent):
    SectionLine( ump, NULL, "", parent ),
    m_d(new SectionFRPPrivate(NULL) ) {
    initVar();
    loadFromXML( attrs, mModel, lModel );
}

SectionFRP::~SectionFRP(){
    if( m_d->concrete != NULL ){
        m_d->concrete->removeIsUsedBy( this );
    }
    delete m_d;
}

SectionFRP & SectionFRP::operator=( SectionFRP & v ){
    if( this != &v ){
        this->Section::operator=( v );
        setConcrete( v.m_d->concrete );
    }
    return *this;
}

Section & SectionFRP::operator=( Section & v ){
    if( this != &v ){
        this->Section::operator=( v );
        SectionFRP * val = dynamic_cast<SectionFRP *>(&v);
        if( val ){
            setConcrete( val->m_d->concrete );
        }
    }
    return *this;
}

void SectionFRP::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement(*m_typeNameInternal);
    for( QMap<QString, VarPlus *>::iterator i = m_varContainer->begin(); i != m_varContainer->end(); ++i) {
        writer->writeAttribute( (*i)->xmlAttribute() );
    }

    if( m_material ){
        writer->writeAttribute( "material", m_material->id->valueNormalStr() );
    }
    if( m_d->concrete ){
        writer->writeAttribute( "concrete", m_d->concrete->id->valueNormalStr() );
    }

    writer->writeEndElement();
}

void SectionFRP::loadFromXML( const QXmlStreamAttributes &attrs, MaterialModel * mModel, SectionLoadPhaseModel * loadModel  ){
    for ( int i=0; i<attrs.count(); ++i ) {
        for( QMap<QString, VarPlus *>::iterator iter = m_varContainer->begin(); iter != m_varContainer->end(); ++iter ){
            if ( attrs.at(i).name().toString().toUpper() == (*iter)->name().toUpper() ) {
                (*iter)->setValueNormal( attrs.at(i).value().toString(), false );
            } else if ( attrs.at(i).name().toString().toUpper() == "MATERIAL" ) {
                setMaterial( mModel->materialId( attrs.at(i).value().toString().toUInt() ), false );
            } else if ( attrs.at(i).name().toString().toUpper() == "CONCRETE" ) {
                setConcrete( dynamic_cast<Concrete *>(mModel->materialId( attrs.at(i).value().toString().toUInt() )), false );
            } else if ( attrs.at(i).name().toString().toUpper() == "LOADPHASE" ) {
                if( loadModel != NULL ){
                    setLoadPhase( loadModel->loadPhaseId( attrs.at(i).value().toString().toUInt() ), false );
                }
            }
        }
    }
    updateReadOnlyVars();
}

void SectionFRP::setMaterial(Material *mat, bool emitAuto) {
    FRP * frp = dynamic_cast<FRP *>(m_material);
    if( frp ){
        led->removeConnectedVar( frp->E );
        epsfdd->removeConnectedVar( frp->E );
        ffdd->removeConnectedVar( frp->E );
        ffdd->removeConnectedVar( frp->gammafd );
        ffdd2->removeConnectedVar( frp->E );
        ffdd2->removeConnectedVar( frp->gammafd );
        epsfd->removeConnectedVar( frp->epsfk );
        epsfd->removeConnectedVar( frp->gammaf );
        epsfd->removeConnectedVar( frp->etaa );
    }

    frp = dynamic_cast<FRP *>(mat);
    Section::setMaterial( frp, emitAuto );

    if( frp ){
        led->addConnectedVar( frp->E );
        epsfdd->addConnectedVar( frp->E );
        ffdd->addConnectedVar( frp->E );
        ffdd->addConnectedVar( frp->gammafd );
        ffdd2->addConnectedVar( frp->E );
        ffdd2->addConnectedVar( frp->gammafd );
        epsfd->addConnectedVar( frp->epsfk );
        epsfd->addConnectedVar( frp->gammaf );
        epsfd->addConnectedVar( frp->etaa );
    } else {
        led->setValueNormal(0.0);
        epsfdd->setValueNormal( 0.0 );
        ffdd->setValueNormal( 0.0 );
        ffdd2->setValueNormal( 0.0 );
        epsfd->setValueNormal( 0.0 );
    }
}

void SectionFRP::setFRP(FRP *frp, bool emitAuto) {
    setMaterial( frp, emitAuto );
}

Concrete *SectionFRP::concrete() {
    return m_d->concrete;
}

void SectionFRP::setConcrete(Concrete *cncr, bool emitAuto) {
    if( m_d->concrete != cncr ){
        if( m_d->concrete != NULL ){
            m_d->concrete->removeIsUsedBy( this );
            disconnect( m_d->concrete, &Material::aboutToBeDeleted, this, &SectionFRP::setConcreteNULL );
            LFd->removeConnectedVar( m_d->concrete->fctm );
            LFd->removeConnectedVar( m_d->concrete->fcm );
            LFd->removeConnectedVar( m_d->concrete->FC );
            ffdd2->removeConnectedVar( m_d->concrete->fcm);
            ffdd2->removeConnectedVar( m_d->concrete->fctm);
            ffdd2->removeConnectedVar( m_d->concrete->FC);
        }

        Concrete * oldConcrete = m_d->concrete;
        m_d->concrete = cncr;

        if( m_d->concrete != NULL ){
            m_d->concrete->setIsUsedBy( this );
            connect( m_d->concrete, &Material::aboutToBeDeleted, this, &SectionFRP::setConcreteNULL );
            LFd->addConnectedVar( m_d->concrete->fctm );
            LFd->addConnectedVar( m_d->concrete->fcm );
            LFd->addConnectedVar( m_d->concrete->FC );
            ffdd2->addConnectedVar( m_d->concrete->fcm);
            ffdd2->addConnectedVar( m_d->concrete->fctm);
            ffdd2->addConnectedVar( m_d->concrete->FC);
        } else {
            LFd->setValueNormal( 0.0 );
            ffdd2->setValueNormal( 0.0 );
        }

        if( emitAuto ){
            emit concreteChanged( oldConcrete, m_d->concrete );
            setLFd( emitAuto );
            setFfdd2( emitAuto );
        }
    }
}

void SectionFRP::setConcreteNULL() {
    setConcrete( NULL );
}

void SectionFRP::initVar() {
    *m_typeNameInternal = "SectionFRP";
    typeName->setValue( trUtf8("FRP"));

    connect( this, &SectionFRP::concreteChanged, this, &Section::sectionChanged );

    t->setRichName( trUtf8("t<span style=\" vertical-align:sub;\">f</span>") );
    t->setToolTip( trUtf8("Spessore del rinforzo"));
    t->setReadOnly( false );

    L->setRichName( trUtf8("b<span style=\" vertical-align:sub;\">f</span>") );
    L->setToolTip( trUtf8("Larghezza del rinforzo in FRP"));

    b = new DoublePlus( 0.0, "b", m_unitMeasure, UnitMeasure::sectL );
    b->setRichName( trUtf8("b") );
    b->setToolTip( trUtf8("Larghezza della trave rinforzata"));
    addVarToContainer( b );

    kb = new DoublePlus( 0.0, "kb", m_unitMeasure, UnitMeasure::noDimension, true );
    kb->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">b</span>") );
    kb->setToolTip( trUtf8("Fattore geomtrico funzione della larghezza della trave rinforzata e di quella del rinforzo"));
    addVarToContainer( kb );
    kb->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionFRP::setKb) );
    kb->addConnectedVars( 2, L, b );

    kG = new DoublePlus( 0.000037, "kG", m_unitMeasure, UnitMeasure::sectL );
    kG->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">G</span>") );
    kG->setToolTip( trUtf8("Coefficiente correttivo per calcolo energia specifica di frattura (0.023 mm per composti preformati - 0.037 mm per compositi impregnati in situ)" ));
    addVarToContainer( kG );

    su = new DoublePlus( 0.00025, "kG", m_unitMeasure, UnitMeasure::sectL );
    su->setRichName( trUtf8("s<span style=\" vertical-align:sub;\">u</span>") );
    su->setToolTip( trUtf8("Valore ultimo dello scorrimento tra FRP e supporto"));
    addVarToContainer( su );

    LFd = new DoublePlus( 1.35, "LFd", m_unitMeasure, UnitMeasure::SCE, true );
    LFd->setRichName( trUtf8("Γ<span style=\" vertical-align:sub;\">Fd</span>") );
    LFd->setToolTip( trUtf8("Valore di progetto dell'energia specifica di frattura"));
    addVarToContainer( LFd );
    LFd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionFRP::setLFd) );
    LFd->addConnectedVars( 2, kG, kb );

    kq = new DoublePlus( 1.0, "kq", m_unitMeasure, UnitMeasure::noDimension );
    kq->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">q</span>") );
    kq->setToolTip( trUtf8("Coefficiente correttivo funzione delle condizioni di carico (1.25 carichi distribuiti - 1 negli altri casi)"));
    addVarToContainer( kq );

    kG2 = new DoublePlus( 0.0001, "kG2", m_unitMeasure, UnitMeasure::sectL );
    kG2->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">G,2</span>") );
    kG2->setToolTip( trUtf8("Coefficiente correttivo per calcolo tensione di delaminazione tipo 2 (pari a 0,10 mm)"));
    addVarToContainer( kG2 );

    fbd = new DoublePlus( 0.0, "fbd", m_unitMeasure, UnitMeasure::tension, true );
    fbd->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">bd</span>") );
    fbd->setToolTip( trUtf8(""));
    addVarToContainer( fbd );
    fbd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionFRP::setFbd) );
    fbd->addConnectedVars( 2, su, LFd );

    led = new DoublePlus( 0.0, "led", m_unitMeasure, UnitMeasure::sectL, true );
    led->setRichName( trUtf8("l<span style=\" vertical-align:sub;\">ed</span>" ) );
    led->setToolTip( trUtf8("Lunghezza ottimale di ancoraggio"));
    addVarToContainer( led );
    led->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionFRP::setLed) );
    led->addConnectedVars( 3, t, fbd, LFd );

    ffdd = new DoublePlus( 0.0, "ffdd", m_unitMeasure, UnitMeasure::tension, true );
    ffdd->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">fdd</span>") );
    ffdd->setToolTip( trUtf8("Tensione di progetto del rinforzo"));
    addVarToContainer( ffdd );
    ffdd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionFRP::setFfdd) );
    ffdd->addConnectedVars( 2, t, LFd );

    ffdd2 = new DoublePlus( 0.0, "ffdd2", m_unitMeasure, UnitMeasure::tension, true );
    ffdd2->setRichName( trUtf8("f<span style=\" vertical-align:sub;\">fdd,2</span>") );
    ffdd2->setToolTip( trUtf8("Tensione di progetto del rinforzo"));
    addVarToContainer( ffdd2 );
    ffdd2->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionFRP::setFfdd2) );
    ffdd2->addConnectedVars( 4, t, kG2, kb, kq );

    epsfdd = new DoublePlus( 0.0, "epsfdd", m_unitMeasure, UnitMeasure::deformation, true );
    epsfdd->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">fdd</span>") );
    epsfdd->setToolTip( trUtf8("Deformazione di delaminazione del rinforzo"));
    addVarToContainer( epsfdd );
    epsfdd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionFRP::setEpsfdd) );
    epsfdd->addConnectedVar( ffdd2 );

    epsfd = new DoublePlus( 0.0, "epsfd", m_unitMeasure, UnitMeasure::deformation, true );
    epsfd->setRichName( trUtf8("ε<span style=\" vertical-align:sub;\">fd</span>") );
    epsfd->setToolTip( trUtf8("Deformazione massima di progetto"));
    addVarToContainer( epsfd );
    epsfd->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionFRP::setEpsfd) );
    epsfd->addConnectedVar( epsfdd );
}

void SectionFRP::setLFd(bool emitAuto) {
    double val = 0.0;
    if( m_d->concrete ){
        if( m_d->concrete->FC->valueNormal() != 0.0 ){
            val = kb->valueNormal() * kG->valueNormal() * \
                    sqrt(m_d->concrete->fcm->valueNormal() * m_d->concrete->fctm->valueNormal()) / m_d->concrete->FC->valueNormal();
        }
    }

    LFd->setValueNormal( val, emitAuto );
}

void SectionFRP::setLed(bool emitAuto) {
    double val = 0.0;
    FRP * frp = dynamic_cast<FRP *>(m_material );
    if( frp ){
        double rad = M_PI * M_PI * frp->E->valueNormal() * t->valueNormal() * LFd->valueNormal() / 2.0;
        if( rad > 0.0 && fbd->valueNormal() != 0.0 ){
            val = sqrt( rad ) / ( fbd->valueNormal() * 1.25 );
        }
        if( val > 0.2 ){
            val = 0.2;
        }
    }

    led->setValueNormal( val, emitAuto );
}

void SectionFRP::setKb(bool emitAuto) {
    double val = 0.0;
    if( b->valueNormal() != 0.0 && L->valueNormal() != 0.0 ){
        double bfOb = L->valueNormal() / b->valueNormal();
        if( bfOb > 0.0 ) {
            if( bfOb < 0.33 ){
                bfOb = 0.33;
            }
            double rad = (2.0 - bfOb) / (1.0 + L->valueNormal() * 1.0e+3 / 400);
            if( rad > 0.0 ){
                val = pow(rad, 0.5);
                if( val <= 1.0 ){
                    val = 1.0;
                }
            }
        }
    }
    kb->setValueNormal( val, emitAuto );
}

void SectionFRP::setFbd(bool emitAuto) {
    double val = 0.0;
    if( su->valueNormal() != 0.0 ){
        val = LFd->valueNormal() / su->valueNormal();
    }
    fbd->setValueNormal( val, emitAuto );
}

void SectionFRP::setFfdd(bool emitAuto) {
    double val = 0.0;
    FRP * frp = dynamic_cast<FRP *>(m_material);
    if( frp ){
        if( t->valueNormal() != 0.0 ){
            double rad = 2.0 * frp->E->valueNormal() * LFd->valueNormal() / t->valueNormal();
            if( rad > 0.0 && frp->gammafd->valueNormal() != 0.0 ){
                val = sqrt(rad) / frp->gammafd->valueNormal();
            }
        }
    }
    ffdd->setValueNormal( val, emitAuto );
}

void SectionFRP::setFfdd2(bool emitAuto) {
    double val = 0.0;
    FRP * frp = dynamic_cast<FRP *>(m_material);
    if( frp && m_d->concrete ){
        double rad = m_d->concrete->fcm->valueNormal() * m_d->concrete->fctm->valueNormal();
        if( t->valueNormal() != 0.0 && m_d->concrete->FC->valueNormal() != 0.0 && rad > 0.0 ){
            rad = frp->E->valueNormal() * 2.0 * kb->valueNormal() * kG2->valueNormal() * sqrt(rad) / (t->valueNormal() * m_d->concrete->FC->valueNormal());
            if( rad > 0.0 && frp->gammafd->valueNormal() != 0.0 ){
                val = kq->valueNormal() * sqrt(rad) / frp->gammafd->valueNormal();
            }
        }
    }
    ffdd2->setValueNormal( val, emitAuto );
}

void SectionFRP::setEpsfdd(bool emitAuto) {
    double val = 0.0;
    FRP * frp = dynamic_cast<FRP *>(m_material);
    if( frp ){
        if( frp->E->valueNormal() != 0.0 ){
            val = ffdd2->valueNormal() / frp->E->valueNormal();
        }
    }
    epsfdd->setValueNormal( val, emitAuto );
}

void SectionFRP::setEpsfd(bool emitAuto) {
    double val = 0.0;
    FRP * frp = dynamic_cast<FRP *>(m_material);
    if( frp ){
        val = qMin( frp->etaa->valueNormal() * frp->epsfk->valueNormal() / frp->gammaf->valueNormal(), epsfdd->valueNormal() );
    }
    epsfd->setValueNormal( val, emitAuto );
}

void SectionFRP::MULSNormal(double *yRet,
                            double *zRet,
                            double l,
                            double my,
                            QList<Point2DModel *> *sects ){
    MULSNormal( yRet, zRet, l, my, 0.0, sects );
}

double SectionFRP::NULSNormal(double l,
                              double my,
                              QList<Point2DModel *> *sects){
    return NULSNormal( l, my, 0.0, sects );
}

double SectionFRP::NULSNormal(double l,
                              double my,
                              double mz,
                              QList<Point2DModel *> *sects){
    Q_UNUSED(sects);

    if( A->valueNormal() == 0.0 ){
        return 0.0;
    }

    // coordinate di P1
    double y1 = P1->y->valueNormal(), z1 = P1->z->valueNormal();
    // coordinate di P2
    double y2 = P2->y->valueNormal(), z2 = P2->z->valueNormal();
    // deformazioni in 1 e 2
    double v1 = l + my * z1 + mz * y1, v2 = l + my * z2 + mz * y2;

    if( (v1 < 0.0 && v2 < 0.0) || (v1 > epsfd->valueNormal() && v2 > epsfd->valueNormal() ) ){
        return 0.0;
    }

    // se le deformazioni sono uguali e contenute nel range di esistenza il calcolo e' semplice
    if( v1 == v2 ){
        return m_material->E->valueNormal() * v1 * A->valueNormal();
    }

    // v1 deve essere minore di v2
    if( v1 > v2 ){
        double tmp = v1; v1 = v2; v2 = tmp;
        tmp = y1; y1 = y2; y2 = tmp;
        tmp = z1; z1 = z2; z2 = tmp;
    }

    // v1 deve essere maggiore di 0
    if( v1 < 0.0 ){
        y1 = y2 + (v2/(v2-v1)) * (y1-y2);
        z1 = z2 + (v2/(v2-v1)) * (z1-z2);
        v1 = 0.0;
    }

    // v2 deve essere minore di epsfd
    if( v2 > epsfd->valueNormal() ){
        y2 = y1 + ((epsfd->valueNormal()-v1)/(v2-v1)) * (y2-y1);
        z2 = z1 + ((epsfd->valueNormal()-v1)/(v2-v1)) * (z2-z1);
        v2 = epsfd->valueNormal();
    }

    // area
    double A = t->valueNormal() * length2D( y1, z1, y2, z2 );
    // momento statico asso y
    double Sy = A * (z1 + z2) * 0.50;
    double Sz = A * (y1 + y2) * 0.50;
    return ( m_material->E->valueNormal() * (l * A + my * Sy + mz * Sz) );
}

void SectionFRP::MULSNormal( double *yRet, double *zRet,
                             double l, double my, double mz,
                             QList<Point2DModel *> *sects ){
    Q_UNUSED(sects);

    if( A->valueNormal() == 0.0 ){
        *yRet = 0.0;
        *zRet = 0.0;
        return;
    }

    // coordinate di P1
    double y1 = P1->y->valueNormal(), z1 = P1->z->valueNormal();
    // coordinate di P2
    double y2 = P2->y->valueNormal(), z2 = P2->z->valueNormal();
    // deformazioni in 1 e 2
    double v1 = l + my * z1 + mz * y1, v2 = l + my * z2 + mz * y2;

    if( (v1 < 0.0 && v2 < 0.0) || (v1 > epsfd->valueNormal() && v2 > epsfd->valueNormal() ) ){
        *yRet = 0.0;
        *zRet = 0.0;
        return;
    }

    // se le deformazioni sono uguali e contenute nel range di esistenza il calcolo e' semplice
    if( v1 == v2 ){
        double Dy = y2 - y1, Dz = z2 - z1;
        double len = sqrt( Dy*Dy+Dz*Dz );
        // area
        double A = t->valueNormal() * len;
        // momento statico asso y
        double Sy = A * (z1 + z2) * 0.50;
        // momento statico asso z
        double Sz = A * (y1 + y2) * 0.50;

        *yRet = m_material->E->valueNormal() * v1 * Sy;
        *zRet = - m_material->E->valueNormal() * v1 * Sz;
        return;
    }

    // v1 deve essere minore di v2
    if( v1 > v2 ){
        double tmp = v1; v1 = v2; v2 = tmp;
        tmp = y1; y1 = y2; y2 = tmp;
        tmp = z1; z1 = z2; z2 = tmp;
    }

    // v1 deve essere maggiore di 0
    if( v1 < 0.0 ){
        y1 = y2 + (v2/(v2-v1)) * (y1-y2);
        z1 = z2 + (v2/(v2-v1)) * (z1-z2);
        v1 = 0.0;
    }

    // v2 deve essere minore di epsfd
    if( v2 > epsfd->valueNormal() ){
        y2 = y1 + ((epsfd->valueNormal()-v1)/(v2-v1)) * (y2-y1);
        z2 = z1 + ((epsfd->valueNormal()-v1)/(v2-v1)) * (z2-z1);
        v2 = epsfd->valueNormal();
    }

    double Dy = y2 - y1, Dz = z2 - z1;
    double len = sqrt( Dy*Dy+Dz*Dz );
    // area
    double A = t->valueNormal() * len;
    // momento statico asso y
    double Sy = A * (z1 + z2) * 0.50;
    // momento statico asso z
    double Sz = A * (y1 + y2) * 0.50;
    // momento di inerzia rispetto asse y
    double Iyy = ( t->valueNormal() * len * (pow(z1,2.0) + Dz * (z1 + Dz / 3.0 )) );
    // momento di inerzia centrifugo rispetto assi y e z
    double Iyz = ( t->valueNormal() * len * ( y1 * z1 + (y1 * Dz + z1 * Dy ) / 2.0 + Dy * Dz / 3.0 ) );
    // momento di inerzia rispetto asse z
    double Izz = ( t->valueNormal() * len * (pow(y1,2.0) + Dy * (y1 + Dy / 3.0 )) );

    *yRet = m_material->E->valueNormal() * (l * Sy + my * Iyy + mz * Iyz);
    *zRet = - m_material->E->valueNormal() * (l * Sz + my * Iyz + mz * Izz);
}
