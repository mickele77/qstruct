#include "sectionthincontsteel.h"

#include "sectionthincontprivate.h"
#include "sectionthinprivate.h"

#include "multisectionarcmodel.h"
#include "multisectionlinemodel.h"
#include "sectionthincontpointsmodel.h"
#include "sectionarc.h"
#include "sectionline.h"
#include "qstringplus.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "boolplus.h"

#include "steel.h"

#include "structmath.h"
#include <QVector2D>
#include <cmath>

class SectionThinContSteelPrivate {
public:
    SectionThinContSteelPrivate(UnitMeasure * ump, Steel * st ):
        NSectionEff( new SectionThin(ump, st, "")),
        MyPlusSectionEff( new SectionThin(ump, st, "MyPlusSectionEff")),
        MyMinusSectionEff( new SectionThin(ump, st, "MyMinusSectionEff")),
        MzPlusSectionEff( new SectionThin(ump, st, "MzPlusSectionEff")),
        MzMinusSectionEff( new SectionThin(ump, st, "MzMinusSectionEff")){
    }
    ~SectionThinContSteelPrivate(){
        delete NSectionEff;
        delete MyPlusSectionEff;
        delete MyMinusSectionEff;
        delete MzPlusSectionEff;
        delete MzMinusSectionEff;
    }

    SectionThin * NSectionEff;
    SectionThin * MyPlusSectionEff;
    SectionThin * MyMinusSectionEff;
    SectionThin * MzPlusSectionEff;
    SectionThin * MzMinusSectionEff;
};

SectionThinContSteel::SectionThinContSteel( UnitMeasure * ump, Steel * st, const QString &nn, QObject * parent ):
    SectionThinCont(ump, st, nn, parent),
    m_ddd( new SectionThinContSteelPrivate(ump, st )){
    initVar();
}

SectionThinContSteel::SectionThinContSteel(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *matModel, SectionLoadPhaseModel *lModel, QObject *parent):
    SectionThinCont( ump, NULL, "", parent ),
    m_ddd(new SectionThinContSteelPrivate( ump, NULL )){
    initVar();
    loadFromXML( attrs, matModel, lModel );
    updateSectionModels();
}

SectionThinContSteel::~SectionThinContSteel() {
    delete m_ddd;
}

// debug
/* QGraphicsItem * SectionThinContSteel::qGraphicsItem() {
    return m_ddd->MzPlusSectionEff->qGraphicsItem();
}*/

void SectionThinContSteel::bEffInternal(
        double *bEff1, double *bEff2,
        double E, double nu, double fy,
        double b, double t,
        double s1, double s2 ) {

    // se s1Eff è invertito rispetto a s1
    bool inverted = false;
    // s1Eff -> bordo compresso maggiore in modulo
    // s2Eff -> bordo compresso/teso
    double s1Eff = s1;
    double s2Eff = s2;
    if( s1Eff > s2Eff ){
        s1Eff = s2;
        s2Eff = s1;
        inverted = true;
    }
    if( s1Eff < 0.0 ){
        double psi = s2Eff / s1Eff;
        double ks = 0.0;
        if( psi >= 1.0 ) {
            ks = 4.0;
        } else if( psi >= 0.0 ) {
            ks = (8.2 / (1.05 + psi));
        } else if( psi >= -1.0 ) {
            ks = 7.81 - 6.29 * psi + 9.78 * psi * psi;
        } else if( psi >= -3.0 ) {
            ks = (5.98 * pow(1.0 - psi, 2.0) );
        } else {
            ks = (5.98 * 16.0);
        }
        double scr = ks * pow(M_PI, 2.0) * E * pow(t,2.0) / (12.0 * (1.0-pow(nu,2.0)) * pow(b,2.0) );
        double lp = sqrt( fy / scr );
        double rho = 1.0;
        if( lp > (0.50+sqrt(0.085-0.055*psi)) ) {
            rho = (lp - 0.055 * (3.0+psi)) / (lp*lp);
            if( rho > 1.0 ){
                rho = 1.0;
            }
        }
        if( s2Eff < 0.0 ){
            double bEff = rho * b;
            if( inverted ){
                *bEff2 = 2.0 * bEff / (5.0 - psi);
                *bEff1 = bEff - *bEff2;
            } else {
                *bEff1 = 2.0 * bEff / (5.0 - psi);
                *bEff2 = bEff - *bEff1;
            }
        } else {
            double bc = s1Eff / (s1Eff-s2Eff) * b; // tratto compresso
            double bt = b - bc;                    // tratto teso
            double bEff = rho * bc;
            if( inverted ) {
                *bEff2 = 0.4 * bEff;
                *bEff1 = 0.6 * bEff + bt;
            } else {
                *bEff1 = 0.4 * bEff;
                *bEff2 = 0.6 * bEff + bt;
            }
        }
        return;
    }
    *bEff1 = 0.5 * b;
    *bEff2 = *bEff1;
}

void SectionThinContSteel::bEffOutstand(
        double *bEff1, double *bEff2,
        double E, double nu, double fy,
        double b, double t,
        double s1, double s2 ) {
    // se s1Eff è invertito rispetto a s1
    bool inverted = false;
    // s1Eff -> bordo compresso maggiore di s2Eff
    // s2Eff -> bordo compresso/teso
    double s1Eff = s1;
    double s2Eff = s2;
    if( s1Eff > s2Eff ){
        s1Eff = s2;
        s2Eff = s1;
        inverted = true;
    }
    if( s1Eff < 0.0 ){
        double psi = s2Eff / s1Eff;
        double ks = 0.0;
        if( inverted ){
            if( psi >= 1.0 ) {
                ks = 0.43;
            } else if( psi >= 0.0 ) {
                ks = 0.578 / (psi + 0.34);
            } else if( psi >= -1.0 ) {
                ks = 1.7 - 5.0 * psi + 17.1 * psi + 0.07 * pow(psi, 2.0);
            } else {
                ks = 23.8;
            }
        } else {
            if( psi >= 1.0 ) {
                ks = 0.43;
            } else if( psi >= 0.0 ) {
                ks = 0.57 + (0.43-0.57) * psi;
            } else if( psi >= -1.0 ) {
                ks = 0.85 + (0.57-0.85) * (psi+1);
            } else if( psi >= -3.0 ) {
                ks = 0.57 - 0.21 * psi + 0.07 * pow(psi,2);
            } else {
                ks = (5.98 * 16.0);
            }
        }
        double eps = sqrt( 235.0e+6/fy);
        double scr = ks * pow(M_PI, 2.0) * E * pow(t,2.0) / (12.0 * (1.0-pow(nu,2.0)) * pow(b,2.0) );
        double lp = sqrt( fy / scr ) / eps;
        double rho = 1.0;
        if( lp > 0.748 ){
            rho = (lp - 0.188) / (lp*lp);
            if( rho > 1.0 ){
                rho = 1.0;
            }
        }
        if( inverted ) {
            // riquadro superiore tabella 4.2
            *bEff1 = 0.0;
            if( s2Eff < 0.0 ) {
                double bEff = rho * b;
                *bEff2 = bEff;
                *bEff1 = 0.0;
            } else {
                double bc = s1Eff / (s1Eff-s2Eff) * b; // tratto compresso
                double bt = b - bc;                    // tratto teso
                double bEff = rho * bc;
                *bEff2 = bt + bEff;
                *bEff1 = 0.0;
            }
        } else {
            // riquadro inferiore tabella 4.2
            if( s2Eff < 0.0 ) {
                double bEff = rho * b;
                *bEff2 = bEff;
                *bEff1 = 0.0;
            } else {
                double bc = s1Eff / (s1Eff-s2Eff) * b; // tratto compresso
                double bt = b - bc;                    // tratto teso
                double bEff = rho * bc;
                *bEff2 = bEff;
                *bEff1 = bt;
            }
        }
        return;
    }
    *bEff1 = 0.5 * b;
    *bEff2 = *bEff1;
}

QVector2D SectionThinContSteel::arcMidPoint( int i ) {
    if( i < m_d->sectionArcModel->count() && i >= 0 ){
        double a = (m_d->sectionArcModel->sectionArc( i )->startAngle->valueNormal() + m_d->sectionArcModel->sectionArc( i )->sweepAngle->valueNormal() * 0.5);
        double r = m_d->sectionArcModel->sectionArc( i )->R->valueNormal();
        return QVector2D( m_d->sectionArcModel->sectionArc( i )->C->y->valueNormal() + r * cos(a),
                          m_d->sectionArcModel->sectionArc( i )->C->z->valueNormal() + r * sin(a) );
    }
    return QVector2D();
}

void SectionThinContSteel::initVar(){
    *m_typeNameInternal = "SectionThinContSteel";
    typeName->setValue( trUtf8("Sezione piegata a freddo acciaio"));

    ANMinusEff = new DoublePlus( 0.0, "ANMinusEff", m_unitMeasure, UnitMeasure::sectL2, true );
    ANMinusEff->setToolTip( trUtf8("Area della sezione efficace nel caso di compressione semplice"));
    ANMinusEff->setRichName( trUtf8("A<span style=\"vertical-align:sub;\">N-,eff</span>") );
    addVarToContainer( ANMinusEff );

    eGNMinusEff = new Point2DPlus("GNMinusEff", "yGNMinusEff", 0.0, "zGNMinusEff", 0.0, m_unitMeasure, UnitMeasure::sectL, true );
    eGNMinusEff->setReadOnly( true );
    eGNMinusEff->y->setReadOnly( true );
    eGNMinusEff->z->setReadOnly( true );
    eGNMinusEff->setRichName(trUtf8("Eccentricità del baricentro della della sezione efficace rispetto a quello della sezione lorda nel caso di compressione semplice"));
    eGNMinusEff->y->setToolTip( trUtf8("Eccentricità lungo y del baricentro della sezione efficace nel caso di compressione semplice"));
    eGNMinusEff->z->setToolTip( trUtf8("Eccentricità lungo z del baricentro della sezione efficace nel caso di compressione semplice"));
    eGNMinusEff->setRichName( "GNMinusEff");
    eGNMinusEff->y->setRichName( trUtf8("e<span style=\"vertical-align:sub;\">Ny</span>") );
    eGNMinusEff->z->setRichName( trUtf8("e<span style=\"vertical-align:sub;\">Nz</span>") );
    addVarToContainer( eGNMinusEff );

    IMyyPlusEff = new DoublePlus( 0.0, "IMyyPlusEff", m_unitMeasure, UnitMeasure::sectL4, true );
    IMyyPlusEff->setToolTip( trUtf8("Momento centrale di inerzia della sezione efficace nel caso di flessione positiva attorno asse Y"));
    IMyyPlusEff->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">eff,YY+</span>") );
    addVarToContainer( IMyyPlusEff );

    WMyyPlusEff = new DoublePlus( 0.0, "WMyyPlusEff", m_unitMeasure, UnitMeasure::sectL3, true );
    WMyyPlusEff->setToolTip( trUtf8("Modulo elastico di resistenza flessionale della sezione efficace nel caso di flessione positiva attorno asse Y"));
    WMyyPlusEff->setRichName( trUtf8("W<span style=\"vertical-align:sub;\">eff,Y+</span>") );
    addVarToContainer( WMyyPlusEff );

    thetaCMyyPlusEff = new DoublePlus( 0.0, "thetaCMyyPlusEff", m_unitMeasure, UnitMeasure::angle, true );
    thetaCMyyPlusEff->setToolTip( trUtf8("Rotazione degli assi centrali di inerzia della sezione efficace nel caso di flessione positiva attorno asse Y"));
    thetaCMyyPlusEff->setRichName( trUtf8("θ<span style=\" vertical-align:sub;\">C,eff,YY+</span>"));
    addVarToContainer( thetaCMyyPlusEff );

    IMyyMinusEff = new DoublePlus( 0.0, "IMyyMinusEff", m_unitMeasure, UnitMeasure::sectL4, true );
    IMyyMinusEff->setToolTip( trUtf8("Momento centrale di inerzia della sezione efficace nel caso di flessione negativa attorno attorno asse Y"));
    IMyyMinusEff->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">eff,Y-</span>") );
    addVarToContainer( IMyyMinusEff );

    WMyyMinusEff = new DoublePlus( 0.0, "WMyyMinusEff", m_unitMeasure, UnitMeasure::sectL3, true );
    WMyyMinusEff->setToolTip( trUtf8("Modulo elastico di resistenza flessionale della sezione efficace nel caso di flessione negativa attorno asse Y"));
    WMyyMinusEff->setRichName( trUtf8("W<span style=\"vertical-align:sub;\">eff,Y-</span>") );
    addVarToContainer( WMyyMinusEff );

    thetaCMyyMinusEff = new DoublePlus( 0.0, "thetaCMyyMinusEff", m_unitMeasure, UnitMeasure::angle, true );
    thetaCMyyMinusEff->setToolTip( trUtf8("Rotazione degli assi centrali di inerzia della sezione efficace nel caso di flessione negativa attorno asse Y"));
    thetaCMyyMinusEff->setRichName( trUtf8("θ<span style=\" vertical-align:sub;\">C,eff,Y-</span>"));
    addVarToContainer( thetaCMyyMinusEff );

    IMzzPlusEff = new DoublePlus( 0.0, "IMzzPlusEff", m_unitMeasure, UnitMeasure::sectL4, true );
    IMzzPlusEff->setToolTip( trUtf8("Momento centrale di inerzia della sezione efficace nel caso di flessione negativa attorno asse Z"));
    IMzzPlusEff->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">eff,Z+</span>") );
    addVarToContainer( IMzzPlusEff );

    WMzzPlusEff = new DoublePlus( 0.0, "WMzzPlusEff", m_unitMeasure, UnitMeasure::sectL3, true );
    WMzzPlusEff->setToolTip( trUtf8("Modulo elastico di resistenza flessionale della sezione efficace nel caso di flessione attorno positiva asse Z"));
    WMzzPlusEff->setRichName( trUtf8("W<span style=\"vertical-align:sub;\">eff,Z+</span>") );
    addVarToContainer( WMzzPlusEff );

    thetaCMzzPlusEff = new DoublePlus( 0.0, "thetaCMzzPlusEff", m_unitMeasure, UnitMeasure::angle, true );
    thetaCMzzPlusEff->setToolTip( trUtf8("Rotazione degli assi centrali di inerzia della sezione efficace nel caso di flessione attorno positiva asse Z"));
    thetaCMzzPlusEff->setRichName( trUtf8("θ<span style=\" vertical-align:sub;\">C,eff,Z+</span>"));
    addVarToContainer( thetaCMzzPlusEff );

    IMzzMinusEff = new DoublePlus( 0.0, "IMzzMinusEff", m_unitMeasure, UnitMeasure::sectL4, true );
    IMzzMinusEff->setToolTip( trUtf8("Momento centrale di inerzia della sezione efficace nel caso di flessione negativa attorno asse Z"));
    IMzzMinusEff->setRichName( trUtf8("I<span style=\"vertical-align:sub;\">eff,ZZ-</span>") );
    addVarToContainer( IMzzMinusEff );

    WMzzMinusEff = new DoublePlus( 0.0, "WMzzMinusEff", m_unitMeasure, UnitMeasure::sectL3, true );
    WMzzMinusEff->setToolTip( trUtf8("Modulo elastico di resistenza flessionale della sezione efficace nel caso di flessione negativa attorno asse Z"));
    WMzzMinusEff->setRichName( trUtf8("W<span style=\"vertical-align:sub;\">eff,Z-</span>") );
    addVarToContainer( WMzzMinusEff );

    thetaCMzzMinusEff = new DoublePlus( 0.0, "thetaCMzzMinusEff", m_unitMeasure, UnitMeasure::angle, true );
    thetaCMzzMinusEff->setToolTip( trUtf8("Rotazione degli assi centrali di inerzia della sezione efficace nel caso di flessione negativa attorno asse Z"));
    thetaCMzzMinusEff->setRichName( trUtf8("θ<span style=\" vertical-align:sub;\">C,eff,Z-</span>"));
    addVarToContainer( thetaCMzzMinusEff );

    WMyyMinEff = new DoublePlus( 0.0, "WMyyMinusEff", m_unitMeasure, UnitMeasure::sectL3, true );
    WMyyMinEff->setToolTip( trUtf8("Modulo elastico di resistenza flessionale minimo della sezione efficace nel caso di flessione attorno asse Y"));
    WMyyMinEff->setRichName( trUtf8("W<span style=\"vertical-align:sub;\">eff,Y,min</span>") );
    addVarToContainer( WMyyMinEff );

    thetaCMyyMinEff = new DoublePlus( 0.0, "thetaCMyyMinEff", m_unitMeasure, UnitMeasure::angle, true );
    thetaCMyyMinEff->setToolTip( trUtf8("Rotazione degli assi centrali di inerzia della sezione efficace associato al modulo di resistenza flessinoale minimo attorno asse Y"));
    thetaCMyyMinEff->setRichName( trUtf8("θ<span style=\" vertical-align:sub;\">C,eff,Y,min</span>"));
    addVarToContainer( thetaCMyyMinEff );

    WMzzMinEff = new DoublePlus( 0.0, "WMzzMinusEff", m_unitMeasure, UnitMeasure::sectL3, true );
    WMzzMinEff->setToolTip( trUtf8("Modulo elastico di resistenza flessionale minimo della sezione efficace nel caso di flessione attorno asse Z"));
    WMzzMinEff->setRichName( trUtf8("W<span style=\"vertical-align:sub;\">eff,Z,min</span>") );
    addVarToContainer( WMzzMinEff );

    thetaCMzzMinEff = new DoublePlus( 0.0, "thetaCMzzMinEff", m_unitMeasure, UnitMeasure::angle, true );
    thetaCMzzMinEff->setToolTip( trUtf8("Rotazione degli assi centrali di inerzia della sezione efficace associato al modulo di resistenza flessinoale minimo attorno asse Z"));
    thetaCMzzMinEff->setRichName( trUtf8("θ<span style=\" vertical-align:sub;\">C,eff,Z,min</span>"));
    addVarToContainer( thetaCMzzMinEff );

    connect( t, &VarPlus::valueChanged, this, static_cast<void(SectionThinContSteel::*)()>(&SectionThinContSteel::updateSectionEff) );
}

void SectionThinContSteel::setMaterial( Material * mat) {
    if( mat == NULL || dynamic_cast<Steel *>(mat) ){
        SectionThin::setMaterial( mat );
    }
}

Steel * SectionThinContSteel::steel() {
    return dynamic_cast<Steel *>(m_material);
}

void SectionThinContSteel::updateSectionEff( SectionThin * sect,
                                             double l, double my, double mz ) {
    sect->clear();

    sect->t->setValueNormal( t->valueNormal() );

    *( sect->sectionArcModel() ) = *(m_d->sectionArcModel);

    int currentI = 0;
    sect->sectionLineModel()->clear();

    for( int i=0; i < m_d->sectionLineModel->count(); ++i ){
        int j = i+1;
        if( j >= m_dd->pointsModel->size() ) {
            j = 0;
        }

        QVector2D Oi( m_dd->pointsModel->P(i)->y->valueNormal(), m_dd->pointsModel->P(i)->z->valueNormal() );
        QVector2D Oj( m_dd->pointsModel->P(j)->y->valueNormal(), m_dd->pointsModel->P(j)->z->valueNormal() );
        QVector2D ui( m_dd->pointsModel->P(j)->y->valueNormal() - Oi.x(), m_dd->pointsModel->P(j)->z->valueNormal() - Oi.y() );
        ui.normalize();
        QVector2D uj( m_dd->pointsModel->P(i)->y->valueNormal() - Oj.x(), m_dd->pointsModel->P(i)->z->valueNormal() - Oj.y() );
        uj.normalize();
        QVector2D Pi0, Pj0;
        if( !(isClosed->valueNormal()) && (i == 0) ) {
            // sezione aperta e primo segmento
            QVector2D vj = arcMidPoint(j-1) - Oj;
            Pi0 = Oi;
            Pj0 = Oj + QVector2D::dotProduct(vj, uj) * uj;
        } else if( !(isClosed->valueNormal()) && (i == (m_d->sectionLineModel->count()-1)) ) {
            // sezione aperta e ultimo segmento
            QVector2D vi = arcMidPoint(i-1) - Oi;
            Pi0 = Oi + QVector2D::dotProduct(vi, ui) * ui;
            Pj0 = Oj;
        } else {
            // segmento interno
            QVector2D vi = arcMidPoint(i-1) - Oi;
            QVector2D vj = arcMidPoint(j-1) - Oj;
            Pi0 = Oi + QVector2D::dotProduct(vi, ui) * ui;
            Pj0 = Oj + QVector2D::dotProduct(vj, uj) * uj;
        }
        QVector2D bVec = Pj0 - Pi0;

        double yPi = Pi0.x(), zPi = Pi0.y();
        double ei = l + my * zPi + mz * yPi;
        double yPj = Pj0.x(), zPj = Pj0.y();
        double ej = l + my * zPj + mz * yPj;

        // calcoliamo larghezze collaboranti
        double bEffi = 0.0, bEffj = 0.0;
        if( ei > 0.0 && ej > 0.0 ){
            bEffi = 0.0;
            bEffj = bVec.length();
        } else {
            if( !(isClosed->valueNormal()) && (i == 0) ) {
                // sezione aperta, primo segmento
                // bordo vincolato è j
                bEffOutstand( &bEffi, &bEffj,
                              m_material->E->valueNormal(), m_material->nu->valueNormal(), steel()->fyk->valueNormal(),
                              bVec.length(), t->valueNormal(), ei, ej );
            } else if( !(isClosed->valueNormal()) && (i == (m_d->sectionLineModel->count()-1)) ) {
                // sezione aperta, ultimo segmento
                // invertiamo i e j perché il bordo vincolato è i
                bEffOutstand( &bEffj, &bEffi,
                              m_material->E->valueNormal(), m_material->nu->valueNormal(), steel()->fyk->valueNormal(),
                              bVec.length(), t->valueNormal(), ej, ei );
            } else {
                // segmento interno
                bEffInternal( &bEffi, &bEffj,
                              m_material->E->valueNormal(), m_material->nu->valueNormal(), steel()->fyk->valueNormal(),
                              bVec.length(), t->valueNormal(), ei, ej );
            }
        }

        // aggiorniamo la sezione in base ai risultati
        if ((bEffi + bEffj) >= bVec.length() ){
            sect->sectionLineModel()->insertRows( currentI );
            SectionLine * sectL = sect->sectionLineModel()->sectionLine(currentI);
            sectL->t->setValueNormal( t->valueNormal() );
            sectL->P1->y->setValueNormal( m_d->sectionLineModel->sectionLine(i)->P1->y->valueNormal() );
            sectL->P1->z->setValueNormal( m_d->sectionLineModel->sectionLine(i)->P1->z->valueNormal() );
            sectL->P2->y->setValueNormal( m_d->sectionLineModel->sectionLine(i)->P2->y->valueNormal() );
            sectL->P2->z->setValueNormal( m_d->sectionLineModel->sectionLine(i)->P2->z->valueNormal() );
            currentI++;
        } else {
            if( bEffi > 0.0 ) {
                QVector2D Pi1 = Pi0 + bEffi * ui;
                sect->sectionLineModel()->insertRows( currentI );
                SectionLine * sectL = sect->sectionLineModel()->sectionLine(currentI);
                sectL->t->setValueNormal( t->valueNormal() );
                sectL->P1->y->setValueNormal( m_d->sectionLineModel->sectionLine(i)->P1->y->valueNormal() );
                sectL->P1->z->setValueNormal( m_d->sectionLineModel->sectionLine(i)->P1->z->valueNormal() );
                sectL->P2->y->setValueNormal( Pi1.x() );
                sectL->P2->z->setValueNormal( Pi1.y() );
                currentI++;
            }
            if( bEffj > 0.0 ) {
                QVector2D Pj1 = Pj0 + bEffj * uj;
                sect->sectionLineModel()->insertRows( currentI );
                SectionLine * sectL = sect->sectionLineModel()->sectionLine(currentI);
                sectL->t->setValueNormal( t->valueNormal() );
                sectL->P1->y->setValueNormal( Pj1.x() );
                sectL->P1->z->setValueNormal( Pj1.y() );
                sectL->P2->y->setValueNormal( m_d->sectionLineModel->sectionLine(i)->P2->y->valueNormal() );
                sectL->P2->z->setValueNormal( m_d->sectionLineModel->sectionLine(i)->P2->z->valueNormal() );
                currentI++;
            }
        }
    }
}

void SectionThinContSteel::updateSectionModels() {
    SectionThinCont::updateSectionModels();
    updateSectionEff();
}

void SectionThinContSteel::updateSectionEff() {
    if( A->valueNormal() > 0.0 &&
            ICyy->valueNormal() > 0.0 &&
            ICzz->valueNormal() > 0.0 ) {


        m_ddd->NSectionEff->clear( false );
        m_ddd->MyPlusSectionEff->clear( false );
        m_ddd->MyMinusSectionEff->clear( false );
        m_ddd->MzPlusSectionEff->clear( false );
        m_ddd->MzMinusSectionEff->clear( false );

        // ei: deformazione del punto (yi, zi)
        // ei = l + my * zi + mz * yi

        // *** sforzo normale ***
        double l = -1.0;
        double my = 0.0;
        double mz = 0.0;
        updateSectionEff( m_ddd->NSectionEff,
                          l, my, mz );

        // *** flessione ***
        // parametri controllo (differenza area tra due passaggi successivi)
        double APrec = A->valueNormal() * 1e-3;

        QList<QPair<SectionThin *, QPair<double, double> > > sectList;
        // flessione positiva yy
        sectList << qMakePair( m_ddd->MyPlusSectionEff,  qMakePair( 1.0, 0.0) );
        // flessione negativa yy
        sectList << qMakePair( m_ddd->MyMinusSectionEff, qMakePair(-1.0, 0.0) );
        // flessione positiva zz
        sectList << qMakePair( m_ddd->MzPlusSectionEff,  qMakePair( 0.0, 1.0) );
        // flessione negativa zz
        sectList << qMakePair( m_ddd->MzMinusSectionEff, qMakePair( 0.0,-1.0) );

        for( int i=0; i < sectList.size(); ++i ){
            my = sectList.at(i).second.first * cos( thetaC->valueNormal() ) + \
                    sectList.at(i).second.second * sin( thetaC->valueNormal() );
            mz = sectList.at(i).second.first * sin( thetaC->valueNormal() ) + \
                    sectList.at(i).second.second * cos( thetaC->valueNormal() );
            l  = -my * G->z->valueNormal() - mz * G->y->valueNormal();

            double Ai = 0.0;
            double Aj = A->valueNormal();
            double dA = fabs(Aj - Ai);
            int maxIter = 100;
            int iter = 0;

            while( dA > APrec && iter < maxIter ) {
                iter++;
                // aggiorna la sezione
                updateSectionEff( sectList.at(i).first,
                                  l, my, mz );

                my = sectList.at(i).second.first * cos( sectList.at(i).first->thetaC->valueNormal() ) + \
                        sectList.at(i).second.second * sin( sectList.at(i).first->thetaC->valueNormal() );
                mz = sectList.at(i).second.first * sin( sectList.at(i).first->thetaC->valueNormal() ) + \
                        sectList.at(i).second.second * cos( sectList.at(i).first->thetaC->valueNormal() );
                // scegliamo l in modo da annullare la deformazione ei nel baricentro
                l = - my * sectList.at(i).first->G->z->valueNormal() - mz * sectList.at(i).first->G->y->valueNormal();

//                qWarning("l: %f - my: %f - mz: %f", l, my, mz );
//                qWarning("yG: %f - zG: %f - thetaC: %f", sectList.at(i).first->G->y->valueNormal(), sectList.at(i).first->G->z->valueNormal(), sectList.at(i).first->thetaC->value() );
//                qWarning("Alines: %f - AArcs: %f", sectList.at(i).first->sectionLineModel()->ANormal(), sectList.at(i).first->sectionArcModel()->ANormal());

                // aggiorna l'area nella nuova sezione
                Ai = Aj;
                Aj = sectList.at(i).first->A->valueNormal();
                dA = fabs(Aj - Ai);
            }
        }
    }

    ANMinusEff->setValueNormal( m_ddd->NSectionEff->A->valueNormal() );
    eGNMinusEff->y->setValueNormal( roundOff( m_ddd->NSectionEff->G->y->valueNormal() - G->y->valueNormal(), pow(10.0,-6.0)) );
    eGNMinusEff->z->setValueNormal( roundOff( m_ddd->NSectionEff->G->z->valueNormal() - G->z->valueNormal(), pow(10.0, -6.0)) );

    IMyyPlusEff->setValueNormal( m_ddd->MyPlusSectionEff->ICyy->valueNormal() );
    thetaCMyyPlusEff->setValueNormal( roundOff( m_ddd->MyPlusSectionEff->thetaC->valueNormal(), pow(10, -3)) );
    double yG = m_ddd->MyPlusSectionEff->G->y->valueNormal();
    double zG = m_ddd->MyPlusSectionEff->G->z->valueNormal();
    m_ddd->MyPlusSectionEff->translateNormal( - yG, - zG );
    m_ddd->MyPlusSectionEff->rotateNormal( thetaCMyyPlusEff->valueNormal() );
    double zMax = qMax( fabs(m_ddd->MyPlusSectionEff->zMaxNormal()), fabs(m_ddd->MyPlusSectionEff->zMinNormal()) );
    if( zMax > 0.0 ){
        WMyyPlusEff->setValueNormal( IMyyPlusEff->valueNormal() / zMax );
    } else {
        WMyyPlusEff->setValueNormal( 0.0 );
    }

    IMyyMinusEff->setValueNormal( m_ddd->MyMinusSectionEff->ICyy->valueNormal() );
    thetaCMyyMinusEff->setValueNormal( roundOff( m_ddd->MyMinusSectionEff->thetaC->valueNormal(), pow(10,-3.0)) );
    yG = m_ddd->MyMinusSectionEff->G->y->valueNormal();
    zG = m_ddd->MyMinusSectionEff->G->z->valueNormal();
    m_ddd->MyMinusSectionEff->translateNormal( - yG, - zG );
    m_ddd->MyMinusSectionEff->rotateNormal( thetaCMyyMinusEff->valueNormal() );
    zMax = qMax( fabs(m_ddd->MyMinusSectionEff->zMaxNormal()), fabs(m_ddd->MyMinusSectionEff->zMinNormal()) );
    if( zMax > 0.0 ){
        WMyyMinusEff->setValueNormal( IMyyMinusEff->valueNormal() / zMax );
    } else {
        WMyyMinusEff->setValueNormal( 0.0 );
    }

    IMzzPlusEff->setValueNormal( m_ddd->MzPlusSectionEff->ICzz->valueNormal() );
    thetaCMzzPlusEff->setValueNormal( roundOff( m_ddd->MzPlusSectionEff->thetaC->valueNormal(), pow(10.0,-3.0)) );
    yG = m_ddd->MzPlusSectionEff->G->y->valueNormal();
    zG = m_ddd->MzPlusSectionEff->G->z->valueNormal();
    m_ddd->MzPlusSectionEff->translateNormal( - yG, - zG );
    m_ddd->MzPlusSectionEff->rotateNormal( thetaCMzzPlusEff->valueNormal() );
    double yMax = qMax( fabs(m_ddd->MzPlusSectionEff->yMaxNormal()), fabs(m_ddd->MzPlusSectionEff->yMinNormal()) );
    if( yMax > 0.0 ){
        WMzzPlusEff->setValueNormal( IMzzPlusEff->valueNormal() / yMax );
    } else {
        WMzzPlusEff->setValueNormal( 0.0 );
    }

    IMzzMinusEff->setValueNormal( m_ddd->MzMinusSectionEff->ICzz->valueNormal() );
    thetaCMzzMinusEff->setValueNormal( roundOff( m_ddd->MzMinusSectionEff->thetaC->valueNormal(), pow(10.0, -3.0)) );
    yG = m_ddd->MzMinusSectionEff->G->y->valueNormal();
    zG = m_ddd->MzMinusSectionEff->G->z->valueNormal();
    m_ddd->MzMinusSectionEff->translateNormal( - yG, - zG );
    m_ddd->MzMinusSectionEff->rotateNormal( thetaCMzzMinusEff->valueNormal() );
    yMax = qMax( fabs(m_ddd->MzMinusSectionEff->yMaxNormal()), fabs(m_ddd->MzMinusSectionEff->yMinNormal()) );
    if( yMax > 0.0 ){
        WMzzMinusEff->setValueNormal( IMzzMinusEff->valueNormal() / yMax );
    } else {
        WMzzMinusEff->setValueNormal( 0.0 );
    }

    if( WMyyMinusEff->valueNormal() < WMyyPlusEff->valueNormal() ){
        WMyyMinEff->setValueNormal( WMyyMinusEff->valueNormal() );
        thetaCMyyMinEff->setValueNormal( thetaCMyyMinusEff->valueNormal() );
    } else {
        WMyyMinEff->setValueNormal( WMyyPlusEff->valueNormal() );
        thetaCMyyMinEff->setValueNormal( thetaCMyyPlusEff->valueNormal() );
    }
    if( WMzzMinusEff->valueNormal() < WMzzPlusEff->valueNormal() ){
        WMzzMinEff->setValueNormal( WMzzMinusEff->valueNormal() );
        thetaCMzzMinEff->setValueNormal( thetaCMzzMinusEff->valueNormal() );
    } else {
        WMzzMinEff->setValueNormal( WMzzPlusEff->valueNormal() );
        thetaCMzzMinEff->setValueNormal( thetaCMzzPlusEff->valueNormal() );
    }
}

void SectionThinContSteel::setTNormal() {
    SectionThinCont::setTNormal();
    updateSectionEff();
}

void SectionThinContSteel::transRot(double *yRet, double *zRet, double yP, double zP, double alpha, double y0, double z0) {
    *yRet =   cos(alpha) * yP + sin(alpha) * zP + (-y0*cos(alpha)-z0*sin(alpha));
    *zRet = - sin(alpha) * yP + cos(alpha) * zP + ( y0*sin(alpha)-z0*cos(alpha));
}
