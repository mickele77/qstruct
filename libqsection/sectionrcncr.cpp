/*
            Copyright (C) 2014-2017 Michele Mocciola

            This file is part of QStruct.

            QStruct is free software: you can redistribute it and/or modify
            it under the terms of the GNU General Public License as published by
            the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.

            QStruct is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
            GNU General Public License for more details.

            You should have received a copy of the GNU General Public License
            along with QStruct.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "sectionrcncr.h"

#include "sectionloadphasemodel.h"
#include "sectionloadphase.h"
#include "multisectioncncrmodel.h"
#include "multisectionsteelcncrmodel.h"
#include "multisectionsteelmodel.h"
#include "multisectionfrpmodel.h"

#include "intplus.h"
#include "doubleplus.h"
#include "point2dplus.h"
#include "point2dmodel.h"
#include "qstringplus.h"
#include "uintplus.h"

#include "unitmeasure.h"
#include "structmath.h"

#include <QXmlStreamWriter>
#include <QString>
#include <QGraphicsItemGroup>
#include <QPolygonF>
#include <QPointF>

#include <cmath>

#define MAX_ITER 1000

class SectionRCncrPrivate{
public:
    SectionRCncrPrivate( UnitMeasure * ump, MaterialModel * matModel, QObject * parent ):
        materialModel( matModel ),
        loadPhaseModel( NULL ),
        sectionsCncr( new MultiSectionCncrModel(ump, matModel, parent ) ),
        sectionsSteelCncr( new MultiSectionSteelCncrModel(ump, matModel, parent ) ),
        sectionsSteel( new MultiSectionSteelModel(ump, matModel, parent ) ),
        sectionsFRP( new MultiSectionFRPModel(ump, matModel, parent ) ),
        qGraphicsItem(NULL){
    }
    ~SectionRCncrPrivate(){
        delete sectionsCncr;
        delete sectionsSteelCncr;
        delete sectionsSteel;
        delete sectionsFRP;
        if( qGraphicsItem ){
            delete qGraphicsItem;
            qGraphicsItem = NULL;
        }
    }

    MaterialModel * materialModel;
    SectionLoadPhaseModel * loadPhaseModel;
    MultiSectionCncrModel * sectionsCncr;
    MultiSectionSteelCncrModel * sectionsSteelCncr;
    MultiSectionSteelModel * sectionsSteel;
    MultiSectionFRPModel * sectionsFRP;
    QGraphicsItemGroup * qGraphicsItem;
};

SectionRCncr::SectionRCncr( UnitMeasure * ump,
                            MaterialModel * matModel,
                            const QString & nn,
                            QObject * parent):
    Section( ump, NULL, nn, parent ),
    m_d( new SectionRCncrPrivate( ump, matModel, this ) ){
    initVar();
}

SectionRCncr::SectionRCncr(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *matModel, SectionLoadPhaseModel *lModel, QObject *parent):
    Section( ump, NULL, "", parent ),
    m_d( new SectionRCncrPrivate( ump, matModel, this ) ){
    initVar();
    loadFromXML( attrs, matModel, lModel );
}

SectionRCncr::~SectionRCncr(){
    emit aboutToBeDeleted();
    delete m_d;
}

SectionRCncr & SectionRCncr::operator=( SectionRCncr & val){
    if( this != &val ){
        this->Section::operator=( val );
        *(m_d->sectionsCncr) = *(val.m_d->sectionsCncr);
        *(m_d->sectionsSteelCncr) = *(val.m_d->sectionsSteelCncr);
        *(m_d->sectionsSteel) = *(val.m_d->sectionsSteel);
        *(m_d->sectionsFRP) = *(val.m_d->sectionsFRP);
        m_d->loadPhaseModel = val.m_d->loadPhaseModel;
    }
    return *this;
}

Section & SectionRCncr::operator=( Section & val){
    if( this != &val ){
        SectionRCncr * sectRCncr = dynamic_cast<SectionRCncr *>(&val);
        if( sectRCncr ){
            *this = *sectRCncr;
        }
        else {
            this->Section::operator=( val );
        }
    }
    return *this;
}

void SectionRCncr::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement(*m_typeNameInternal);
    for( QMap<QString, VarPlus *>::iterator i = m_varContainer->begin(); i != m_varContainer->end(); ++i) {
        writer->writeAttribute( (*i)->xmlAttribute() );
    }
    if( m_material ){
        writer->writeAttribute( "material", m_material->id->valueNormalStr() );
    }

    m_d->sectionsCncr->writeXml( writer );
    m_d->sectionsSteelCncr->writeXml( writer );
    m_d->sectionsSteel->writeXml( writer );
    m_d->sectionsFRP->writeXml( writer );

    writer->writeEndElement();
}

void SectionRCncr::suspendSignals(bool susp) {
    if( susp ){
        m_d->sectionsCncr->suspendSignals( true );
        m_d->sectionsSteelCncr->suspendSignals( true );
        m_d->sectionsSteel->suspendSignals( true );
        m_d->sectionsFRP->suspendSignals( true );
        VarPlusContainer::suspendSignals( true );
    } else {
        VarPlusContainer::suspendSignals( false );
        m_d->sectionsCncr->suspendSignals( false );
        m_d->sectionsSteelCncr->suspendSignals( false );
        m_d->sectionsSteel->suspendSignals( false );
        m_d->sectionsFRP->suspendSignals( false );
    }
}

void SectionRCncr::updateReadOnlyVars() {
    if( m_d->sectionsCncr != NULL ){
        m_d->sectionsCncr->updateReadOnlyVars();
    }
    if( m_d->sectionsSteelCncr != NULL ){
        m_d->sectionsSteelCncr->updateReadOnlyVars();
    }
    if( m_d->sectionsSteel != NULL ){
        m_d->sectionsSteel->updateReadOnlyVars();
    }
    if( m_d->sectionsFRP != NULL ){
        m_d->sectionsFRP->updateReadOnlyVars();
    }
    VarPlusContainer::updateReadOnlyVars();
}

void SectionRCncr::readXml(QXmlStreamReader *reader) {
    QXmlStreamReader::TokenType t = reader->tokenType();
    QString n = reader->name().toString().toUpper();
    QString myName = m_typeNameInternal->toUpper();

    // Ci mettiamo sul tag iniziale
    while( !(n == myName && t == QXmlStreamReader::StartElement) &&
           !(reader->hasError()) &&
           (t != QXmlStreamReader::EndDocument) ){
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }

    // Andiamo sul tag finale. Nel frattempo leggiamo i modelli che incontriamo
    while( !(t == QXmlStreamReader::EndElement && n == myName) &&
           !(reader->hasError() ) &&
           (t != QXmlStreamReader::EndDocument) ){
        if( t == QXmlStreamReader::StartElement ){
            if( n == m_d->sectionsCncr->modelName().toUpper() ){
                m_d->sectionsCncr->readXml(reader);
            }
            if( n == m_d->sectionsSteelCncr->modelName().toUpper() ){
                m_d->sectionsSteelCncr->readXml(reader);
            }
            if( n == m_d->sectionsSteel->modelName().toUpper() ){
                m_d->sectionsSteel->readXml(reader);
            }
            if( n == m_d->sectionsFRP->modelName().toUpper() ){
                m_d->sectionsFRP->readXml(reader);
            }
        }
        t = reader->readNext();
        n = reader->name().toString().toUpper();
    }
}

void SectionRCncr::initVar(){
    *m_typeNameInternal = "SectionRCncr";
    typeName->setValue( trUtf8("C.A."));

    A->setReadOnly( true );
    Sy->setReadOnly( true );
    Sz->setReadOnly( true );
    Iyy->setReadOnly( true );
    Izz->setReadOnly( true );
    Iyz->setReadOnly( true );
    Sy3DyDz->setReadOnly( true );
    Sz3DyDz->setReadOnly( true );
    Syz2DyDz->setReadOnly( true );
    Sy2zDyDz->setReadOnly( true );

    connect( m_d->sectionsCncr, &MultiSectionCncrModel::modelChanged, this, &SectionRCncr::updateReadOnlyVars );
    connect( m_d->sectionsSteelCncr, &MultiSectionSteelCncrModel::modelChanged, this, &SectionRCncr::updateReadOnlyVars );
    connect( m_d->sectionsSteel, &MultiSectionSteelModel::modelChanged, this, &SectionRCncr::updateReadOnlyVars );
    connect( m_d->sectionsFRP, &MultiSectionSteelModel::modelChanged, this, &SectionRCncr::updateReadOnlyVars );

    connect( m_d->sectionsCncr, &MultiSectionCncrModel::modelChanged, this, &SectionRCncr::sectionChanged );
    connect( m_d->sectionsSteelCncr, &MultiSectionSteelCncrModel::modelChanged, this, &SectionRCncr::sectionChanged );
    connect( m_d->sectionsSteel, &MultiSectionSteelModel::modelChanged, this, &SectionRCncr::sectionChanged );
    connect( m_d->sectionsFRP, &MultiSectionFRPModel::modelChanged, this, &SectionRCncr::sectionChanged );

    GCncr = new Point2DPlus("GCncr", "yGCncr", 0.0, "zGCncr", 0.0, m_unitMeasure, UnitMeasure::sectL, true );
    GCncr->setRichName(trUtf8("Coordinate del baricentro della sezione in cls"));
    GCncr->y->setToolTip( trUtf8("Ascissa del baricentro della sezione in cls"));
    GCncr->z->setToolTip( trUtf8("Ordinata del baricentro della sezione in cls"));
    GCncr->setRichName( "G<span style=\"vertical-align:sub;\">c</span>" );
    GCncr->y->setToolTip( trUtf8("Ascissa del baricentro della sezione in cls"));
    GCncr->z->setToolTip( trUtf8("Ordinata del baricentro della sezione in cls"));
    addVarToContainer( GCncr );
    GCncr->y->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionRCncr::setGCncrY) );
    GCncr->z->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionRCncr::setGCncrZ) );
}

void SectionRCncr::setGCncrY( bool emitAuto ) {
    double a = m_d->sectionsCncr->ANormal();
    if( a != 0.0 ){
        a = m_d->sectionsCncr->SzNormal() / a;
    }
    GCncr->y->setValueNormal( a, emitAuto );
}

void SectionRCncr::setGCncrZ( bool emitAuto ) {
    double a = m_d->sectionsCncr->ANormal();
    if( a != 0.0 ){
        a = m_d->sectionsCncr->SyNormal() / a;
    }
    GCncr->z->setValueNormal( a, emitAuto );
}

void SectionRCncr::translateNormal( double yTrans, double zTrans, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }

    m_d->sectionsCncr->translateNormal( yTrans, zTrans );
    m_d->sectionsSteelCncr->translateNormal( yTrans, zTrans );
    m_d->sectionsSteel->translateNormal( yTrans, zTrans );
    m_d->sectionsFRP->translateNormal( yTrans, zTrans );

    if( suspSignals ){
        updateReadOnlyVars();
        suspendSignals( false );
    }
}

void SectionRCncr::rotateNormal( double rot, bool suspSignals ){
    if( suspSignals ){
        suspendSignals( true );
    }

    m_d->sectionsCncr->rotateNormal( rot );
    m_d->sectionsSteelCncr->rotateNormal( rot );
    m_d->sectionsSteel->rotateNormal( rot );
    m_d->sectionsFRP->rotateNormal( rot );

    if( suspSignals ){
        updateReadOnlyVars();
        suspendSignals( false );
    }
}

QString SectionRCncr::typeSectionNormal(){
    return QString("SectionRCncr");
}

QString SectionRCncr::typeSection(){
    return trUtf8("Sezione in C.A.");
}

MultiSectionCncrModel *SectionRCncr::sectionsCncr(){
    return m_d->sectionsCncr;
}

MultiSectionSteelCncrModel * SectionRCncr::sectionsSteelCncr(){
    return m_d->sectionsSteelCncr;
}

MultiSectionSteelModel * SectionRCncr::sectionsSteel(){
    return m_d->sectionsSteel;
}

MultiSectionFRPModel *SectionRCncr::sectionsFRP() {
    return m_d->sectionsFRP;
}

bool SectionRCncr::lmP1P2( double *l, double *a,
                           double s1, double e1, double s2, double e2 ){
    if( s1 != s2 ){
        /*       l + (ny * y1 + nz*z1) * a = e1
                 l + (ny * y2 + nz*z2) * a = e2 =>
           1.0 * l +       s1          * a = e1
           1.0 * l +       s2          * a = e2 */

        *a = (e2-e1) / (s2-s1);
        // trick per evitare problemi di approssimazione che porterebbero
        // le armatura al di fuori del limite di accettabilità
        double l1 = e1 - *a * s1;
        double l2 = e2 - *a * s2;
        if( fabs(l1) < fabs(l2) ){
            (*l) = l1;
        } else {
            (*l) = l2;
        }
        return true;
    } else
        return false;
}

double SectionRCncr::NULSMinNormal(){
    if( m_d->sectionsCncr->count() > 0 ){
        // *** sezione con cls ***
        return NULSNormal( -m_d->sectionsCncr->epsCUMin(), 0.0);
    } else if (m_d->sectionsSteelCncr->count() > 0) {
        // *** sezione senza cls, solo armature ***
        switch( m_d->sectionsSteelCncr->steelCncrModelfEps() ){
        case SteelCncr::inclinedTopBranch:
            return NULSNormal( -m_d->sectionsSteelCncr->epsUdMin(), 0.0);
        default:
            return NULSNormal( -m_d->sectionsSteelCncr->epsYdMax(), 0.0);
        }
    } else if (m_d->sectionsSteel->count() > 0) {
        return NULSNormal( - m_d->sectionsSteelCncr->epsYdMax(), 0.0);
    }
    return 0.0;
}

double SectionRCncr::NULSMaxNormal( double *eps ){
    if( m_d->sectionsFRP->count() > 0 ){
        if( m_d->sectionsFRP->ANormal() > 0.0 ){
            double e = m_d->sectionsFRP->epsfdMin();
            if( eps != NULL ){
                *eps = e;
            }
            return NULSNormal( e, 0.0, 0.0);
        }
    }

    if( m_d->sectionsSteelCncr->count() > 0 ){
        if( m_d->sectionsSteelCncr->ANormal() > 0.0 ){
            switch( m_d->sectionsSteelCncr->steelCncrModelfEps() ){
            case SteelCncr::inclinedTopBranch:{
                double e = m_d->sectionsSteelCncr->epsUdMin();
                if( eps != NULL ){
                    *eps = e;
                }
                return NULSNormal( e, 0.0, 0.0);
            }
            default: {
                double e = m_d->sectionsSteelCncr->epsYdMax();
                if( eps != NULL ){
                    *eps = e;
                }
                return NULSNormal( e, 0.0, 0.0);
            }
            }
        }
    }

    if (m_d->sectionsSteel->count() > 0) {
        if (m_d->sectionsSteel->ANormal() > 0) {
            double e = m_d->sectionsSteel->epsYdMax();
            if( eps != NULL ){
                *eps = e;
            }
            return NULSNormal( e, 0.0, 0.0);
        }
    }

    if( eps != NULL ){
        *eps = 0.0;
    }
    return 0.0;
}

double SectionRCncr::NULSNormal(double l, double my, QList<Point2DModel *> *sects){
    double NRdRet = m_d->sectionsCncr->NULSNormal(l, my, sects);
    NRdRet += m_d->sectionsSteelCncr->NULSNormal(l, my, sects);
    NRdRet += m_d->sectionsSteel->NULSNormal(l, my, sects);
    NRdRet += m_d->sectionsFRP->NULSNormal(l, my, sects);
    return NRdRet;
}

double SectionRCncr::NULSNormal(double l, double my, double mz, QList<Point2DModel *> *sects){
    double NRdRet = m_d->sectionsCncr->NULSNormal(l, my, mz, sects);
    NRdRet += m_d->sectionsSteelCncr->NULSNormal(l, my, mz, sects);
    NRdRet += m_d->sectionsSteel->NULSNormal(l, my, mz, sects);
    NRdRet += m_d->sectionsFRP->NULSNormal(l, my, mz, sects);
    return NRdRet;
}

double SectionRCncr::NULSNormal( int phase,
                                 QList<double> *l, QList<double> * my, QList<double> * mz,
                                 QList<Point2DModel *> *sects  ) {
    double NRdRet = m_d->sectionsCncr->NULSNormal( phase, l, my, mz, sects);
    NRdRet += m_d->sectionsSteelCncr->NULSNormal( phase, l, my, mz, sects);
    NRdRet += m_d->sectionsSteel->NULSNormal( phase, l, my, mz, sects);
    NRdRet += m_d->sectionsFRP->NULSNormal( phase, l, my, mz, sects);
    return NRdRet;
}

void SectionRCncr::MULSNormal( double * MyRet, double * MzRet,
                               double l, double my, QList<Point2DModel *> * sects){
    *MyRet = 0.0; *MzRet = 0.0;
    double y=0.0, z=0.0;

    m_d->sectionsCncr->MULSNormal( &y, &z, l, my, sects);
    *MyRet += y; *MzRet += z;
    m_d->sectionsSteelCncr->MULSNormal( &y, &z, l, my, sects );
    *MyRet += y; *MzRet += z;
    m_d->sectionsSteel->MULSNormal( &y, &z, l, my, sects );
    *MyRet += y; *MzRet += z;
    m_d->sectionsFRP->MULSNormal( &y, &z, l, my, sects );
    *MyRet += y; *MzRet += z;
}

void SectionRCncr::MULSNormal(double * MyRet, double * MzRet,
                              double l, double my, double mz,
                              QList<Point2DModel *> * sects){
    *MyRet = 0.0; *MzRet = 0.0;
    double y=0.0, z=0.0;

    m_d->sectionsCncr->MULSNormal( &y, &z, l, my, mz, sects);
    *MyRet += y; *MzRet += z;
    m_d->sectionsSteelCncr->MULSNormal( &y, &z, l, my, mz, sects );
    *MyRet += y; *MzRet += z;
    m_d->sectionsSteel->MULSNormal( &y, &z, l, my, mz, sects );
    *MyRet += y; *MzRet += z;
    m_d->sectionsFRP->MULSNormal( &y, &z, l, my, mz, sects );
    *MyRet += y; *MzRet += z;
}

void SectionRCncr::MULSNormal(double * MyRet, double * MzRet,
                              int phase,
                              QList<double> * l, QList<double> * my, QList<double> * mz,
                              QList<Point2DModel *> * sects){
    *MyRet = 0.0; *MzRet = 0.0;
    double y=0.0, z=0.0;

    m_d->sectionsCncr->MULSNormal( &y, &z, phase, l, my, mz, sects);
    *MyRet += y; *MzRet += z;
    m_d->sectionsSteelCncr->MULSNormal( &y, &z, phase, l, my, mz, sects );
    *MyRet += y; *MzRet += z;
    m_d->sectionsSteel->MULSNormal( &y, &z, phase, l, my, mz, sects );
    *MyRet += y; *MzRet += z;
    m_d->sectionsFRP->MULSNormal( &y, &z, phase, l, my, mz, sects );
    *MyRet += y; *MzRet += z;
}

bool SectionRCncr::MULSNormal( double * MyRet, double *MzRet,
                               double NEd,
                               double yCen, double zCen,
                               double rot,
                               QList<Point2DModel *> *sects,
                               double accuracy){
    // Valore di partenza di MRd
    // se ci sono problemi è il valore che verra restituito dalla funzione
    *MyRet = 0.0;
    *MzRet = 0.0;

    // Verifica se ci sono sezioni in cls o armature
    // Nella sezione ci devono essere almeno una sezione in cls ed un'armatura di area non nulla
    double AFRP = m_d->sectionsFRP->ANormal();
    double ASteelCncr = m_d->sectionsSteelCncr->ANormal();
    double ASteel = m_d->sectionsSteel->ANormal();
    double ACncr = m_d->sectionsCncr->ANormal();

    if( (ASteelCncr > 0.0 || ASteel > 0.0 || AFRP > 0.0) && ACncr > 0.0 ){

        // vettore normale all'asse neutro della sezione (asse neutro: retta con deformazione nulla)
        double ny = cos( M_PI/2.0 + rot );
        double nz = sin( M_PI/2.0 + rot );

        // Verifica se le armature più in alto del lembo inferiore del cls

        // calcolo minima s nella sezine in cls, e corrispondenti epsC ed epsCU
        double sCMin = 0.0;
        double epsC = 0.0, epsCU = 0.0;
        if( !m_d->sectionsCncr->sMinNormal( &sCMin, ny, nz, &epsC, &epsCU ) ) {
            return false;
        }
        epsC *= -1.0; epsCU *= -1.0;

        // calcolo s massima nelle varie armature
        double sMax = 0.0;
        bool sMaxWasSet = false;
        if( ASteelCncr > 0.0 ){
            sMaxWasSet = m_d->sectionsSteelCncr->sMaxNormal( &sMax, ny, nz );
        }
        if( ASteel > 0.0 ){
            if( sMaxWasSet ) {
                double sCmp;
                if( m_d->sectionsSteel->sMaxNormal( &sCmp, ny, nz ) ){
                    if( sCmp > sMax ){
                        sMax = sCmp;
                    }
                }
            } else {
                sMaxWasSet = m_d->sectionsSteel->sMaxNormal( &sMax, ny, nz );
            }
        }
        if( AFRP > 0.0 ){
            if( sMaxWasSet ) {
                double sCmp;
                if( m_d->sectionsFRP->sMaxNormal( &sCmp, ny, nz ) ){
                    if( sCmp > sMax ){
                        sMax = sCmp;
                    }
                }
            } else {
                sMaxWasSet = m_d->sectionsFRP->sMaxNormal( &sMax, ny, nz );
            }
        }

        /* se le armature sono più in basso del lembo inferiore del cls
           o in caso di problemi nel calcolo delle s, usciamo */
        if( !sMaxWasSet || sCMin >= sMax ){
            return false;
        }

        // **** CALCOLO ****

        // calcolo massima s nella sezione in cls
        double sCMax = 0.0;
        if( !m_d->sectionsCncr->sMaxNormal( &sCMax, ny, nz ) ){
            return false;
        }

        // NRd1: sezione uniformemente compressa
        double NRd1 = NULSMinNormal();

        // NRd2: lembo inferiore cls compresso, lembo superiore cls deformazione nulla
        double lRd2, myRd2, mzRd2, aRd2;
        lmP1P2(&lRd2, &aRd2, sCMin, epsCU, sCMax, 0.0 );
        myRd2 = aRd2 * nz;
        mzRd2 = aRd2 * ny;
        double NRd2 = NULSNormal( lRd2, myRd2, mzRd2 );

        // NRd4: sezione uniformemente in trazione
        double epsNRd4 = 0.0;
        double NRd4 = NULSMaxNormal( &epsNRd4 );

        // verifica ammissibilita' di NEd
        if( NEd < NRd1 || NEd > NRd4 ){
            qWarning("NEd: %f N al di fuori di intervallo ammissibilita' di NRd (%f N, %f N)", NEd, NRd1, NRd4 );
            return false;
        } else if( NEd == NRd1 ) {
            // sezione uniformemente compressa, deformazione epsC
            MULSNormal( MyRet, MzRet, epsC, 0.0, 0.0, sects );
            // Riportiamo i valori di MRd in (yCen, zCen)
            (*MyRet) -= NEd * zCen;
            (*MzRet) += NEd * yCen;
            return true;
        } else if( NEd == NRd2 ){
            MULSNormal( MyRet, MzRet, lRd2, myRd2, mzRd2, sects );
            // Riportiamo i valori di MRd in (yCen, zCen)
            (*MyRet) -= NEd * zCen;
            (*MzRet) += NEd * yCen;
            return true;
        } else if( NEd == NRd4 ) {
            MULSNormal( MyRet, MzRet, epsNRd4, 0.0, 0.0, sects );
            // Riportiamo i valori di MRd in (yCen, zCen)
            (*MyRet) -= NEd * zCen;
            (*MzRet) += NEd * yCen;
            return true;
        } else {
            // Non siamo stati fortunati: procediamo
            double sPivot = 0.0, ePivot = 0.0;
            bool pivotOnSteel = false;
            double sP = 0.0, eP = 0.0;
            double eps1 = 0.0; double eps2 = 0.0;
            if( NEd < NRd2 ){ // if ( NRd1 < NSd < NRd2 )
                sPivot = sCMax - epsC * ( sCMax - sCMin ) / epsCU;
                ePivot = epsC;
                sP = sCMax;
                eps1 = epsC;
                eps2 = 0.0;
                // N1 = NRd1;
                // N2 = NRd2;
            } else { // if ( NRd2 < NSd < NRd4 )
                if( AFRP > 0.0  ||
                        (ASteelCncr > 0.0 && (m_d->sectionsSteelCncr->steelCncrModelfEps() == SteelCncr::inclinedTopBranch)) ){
                    // Calcolo NRd3: lembo inferiore cls compresso, armatura superiore deformazione massima
                    double s1 = sCMin, e1 = epsCU;
                    double sRd3, eRd3;
                    double lRd3, myRd3, mzRd3, aRd3, NRd3;

                    if( AFRP > 0.0 ){
                        sRd3 = m_d->sectionsFRP->sMaxNormal( &eRd3, ny, nz );
                    } else {
                        sRd3 = m_d->sectionsSteelCncr->sMaxNormal( &eRd3, ny, nz);
                    }

                    lmP1P2(&lRd3, &aRd3, s1, e1, sRd3, eRd3);
                    myRd3 = aRd3 * nz;
                    mzRd3 = aRd3 * ny;
                    NRd3 = NULSNormal(lRd3, myRd3, mzRd3 );

                    if( NEd == NRd3 ){
                        MULSNormal( MyRet, MzRet, lRd3, myRd3, mzRd3, sects );
                        // Riportiamo i valori di MRd in (yCen, zCen)
                        (*MyRet) -= NEd * zCen;
                        (*MzRet) += NEd * yCen;
                        return true;
                    } else if (NEd < NRd3 ){ // if (NRd2 < NSd < NRd3)
                        sPivot = sCMin;
                        ePivot = epsCU;
                        sP = sCMax;
                        eps1 = 0.0;
                        eps2 = lRd3 + myRd3 * sP;
                        // N1 = NRd2;
                        // N2 = NRd3;
                    } else { // if (NRd3 < NSd < NRd4)
                        sPivot = sRd3;
                        ePivot = eRd3;
                        sP = sCMin;
                        eps1 = epsCU;
                        eps2 = eRd3;
                        pivotOnSteel = true;
                        // N1 = NRd3;
                        // N2 = NRd4;
                    }
                } else {
                    // *** SteelCncr::horizontalTopBranch (elastico infinitamente plastico) e no FRP ***
                    sPivot = sCMin;
                    ePivot = epsCU;
                    eps1 = 0.0;
                    eps2 = epsNRd4;
                    sP = sCMax;
                    double l, my, mz, a;
                    lmP1P2(&l, &a, sPivot, ePivot, sP, eps2 );
                    my = a * nz;
                    mz = a * ny;
                    // N1 = NRd2;
                    // sforzo N in corrispondenza di e2
                    double N2 = NULSNormal( l, my, mz);
                    while( N2 < NEd ) {
                        eps2 *= 1.1;
                        lmP1P2(&l, &a, sPivot, ePivot, sP, eps2 );
                        my = a * nz;
                        mz = a * ny;
                        N2 = NULSNormal(l, my, mz);
                    }
                }
            }

            double absAccuracy = 0.0;
            if( NRd1 != 0.0 ){
                absAccuracy = fabs( accuracy * NRd1 );
            } else {
                absAccuracy = fabs( accuracy * NRd4 );
            }

            int i = 0; // contatore per verificare numero massimo iterazioni
            double l, my, mz, a;
            bool found = false;
            while ( !found ){
                // ciclo bisezione finché found = true
                eP = (eps1 + eps2) * 0.5;
                if( pivotOnSteel ){
                    lmP1P2( &l, &a, sP, eP, sPivot, ePivot);
                } else {
                    lmP1P2( &l, &a, sPivot, ePivot, sP, eP);
                }
                my = a * nz;
                mz = a * ny;
                double NCalc = NULSNormal(l, my, mz);
                if( fabs( NCalc - NEd ) < absAccuracy ){
                    found = true;
                } else if( NCalc > NEd ){
                    eps2 = eP;
                } else { // if( NCalc < NEd )
                    eps1 = eP;
                }
                i++;
                if( i > MAX_ITER ){
                    qWarning("Valore accuratezza troppo basso. N: %f N - acc: %f N", NEd, absAccuracy);
                    return false;
                }
            }
            // Restituisce il valore cercato
            MULSNormal( MyRet, MzRet, l, my, mz, sects);
            // Riportiamo i valori di MRd in (yCen, zCen)
            (*MyRet) -= NEd * zCen;
            (*MzRet) += NEd * yCen;

            return true;
        }
    }
    return false;
}

void SectionRCncr::MULS( DoublePlus * MyRet, DoublePlus *MzRet,
                         DoublePlus * NSd, Point2DPlus * cen, DoublePlus * rot,
                         QList<Point2DModel *> *sects, DoublePlus * accuracy){
    // Valore restituito
    double yM = 0.0, zM = 0.0;

    // Accuratezza. Se non è stata inserita in input assume valore di default
    double acc = 1.0e-6;
    if( accuracy != NULL ){
        acc = accuracy->valueNormal();
    }

    // Polo per calcolo momento. Se non è stato inserito assume (0.0,0.0)
    double yCen = 0.0, zCen = 0.0;
    if( cen ){
        yCen = cen->y->valueNormal();
        zCen = cen->z->valueNormal();
    }

    //  Rotazione. Se non è stata definito assume 0.0.
    double r = 0.0;
    if( rot ){
        r = rot->valueNormal();
    }

    // Calcola momento resistente
    MULSNormal( &yM, &zM, NSd->valueNormal(), yCen, zCen, r, sects, acc);

    // Assegna il valore calcolato al puntatore restituito
    MyRet->setValueNormal( yM );
    MzRet->setValueNormal( zM );
}

QPolygonF SectionRCncr::MULSyMULSz( DoublePlus * NSd, Point2DPlus * cen, int propNP){
    int nP = propNP > 4 ? propNP: 4;
    double dRot = 2.0 * M_PI / nP;

    double yRet = 0.0, zRet = 0.0;
    double rot = 0.0;

    SectionRCncr sectRCncrRot(m_unitMeasure, m_d->materialModel, "", parent() );
    sectRCncrRot.suspendSignals( true );
    sectRCncrRot = *this;
    sectRCncrRot.translateNormal( -(cen->y->valueNormal()), -(cen->z->valueNormal()) );
    sectRCncrRot.updateReadOnlyVars();

    QVector<QPointF> polygon;
    for( int i=0; i< (nP+1); i++){
        sectRCncrRot.MULSNormal( &yRet, &zRet, NSd->valueNormal(), 0.0, 0.0, 0.0 );
        rotate2D( &yRet, &zRet, rot );
        polygon << QPointF( yRet * m_unitMeasure->scale(UnitMeasure::loadM), zRet * m_unitMeasure->scale(UnitMeasure::loadM));

        sectRCncrRot.rotateNormal( -dRot);
        sectRCncrRot.updateReadOnlyVars();
        rot += dRot;
    }
    return polygon;
}

QPolygonF SectionRCncr::MULSN( Point2DPlus * cen, DoublePlus * rot, int propNP){
    double lRd, mRd;
    QPolygonF polygon;

    int nP = propNP > 1 ? propNP: 1;

    // calcolo epsC ed epsCU
    double epsCU = -m_d->sectionsCncr->epsCUMin();

    // calcolo epsYd ed epsUd
    double epsUd = m_d->sectionsSteelCncr->epsUdMin();

    SectionRCncr sectRCncrRot(m_unitMeasure, m_d->materialModel, "" );
    sectRCncrRot.suspendSignals( true );

    sectRCncrRot = (*this);

    sectRCncrRot.translateNormal( -(cen->y->valueNormal()), -(cen->z->valueNormal()) );
    sectRCncrRot.rotateNormal( - rot->valueNormal() );
    sectRCncrRot.updateReadOnlyVars();

    // calcolo ySup e yInf nella sezione in cls
    double zCMax = sectRCncrRot.m_d->sectionsCncr->zMaxNormal();
    double zCMin = sectRCncrRot.m_d->sectionsCncr->zMinNormal();

    // calcolo ySup nelle armature in acciaiosezione in cls
    double zSMax = sectRCncrRot.m_d->sectionsSteelCncr->zMaxNormal();

    double NRd[4];
    NRd[0] = sectRCncrRot.NULSMinNormal();
    lmP1P2(&lRd, &mRd, zCMin, epsCU, zCMax, 0.0 );
    NRd[1] = sectRCncrRot.NULSNormal( lRd, mRd);
    lmP1P2(&lRd, &mRd, zCMin, epsCU, zSMax, epsUd);
    NRd[2] = sectRCncrRot.NULSNormal(lRd, mRd);
    NRd[3] = sectRCncrRot.NULSMaxNormal();

    for( int i=0; i < 3; i++){
        double yRet = 0.0, zRet = 0.0;
        bool found = sectRCncrRot.MULSNormal( &yRet, &zRet, NRd[i], 0.0, 0.0, 0.0 );
        if( found ) {
            rotate2D( &yRet, &zRet, rot->valueNormal() );
            polygon << QPointF( NRd[i] * m_unitMeasure->scale(UnitMeasure::loadF), (yRet * cos(rot->valueNormal()) + zRet * sin(rot->valueNormal())) * m_unitMeasure->scale(UnitMeasure::loadM) );
        }
        for( int j=0; j < nP; j++){
            double N = NRd[i] + (NRd[i+1] - NRd[i]) * (j+1) / (nP+1);
            found = sectRCncrRot.MULSNormal( &yRet, &zRet, N, 0.0, 0.0, 0.0 );
            if( found ){
                rotate2D( &yRet, &zRet, rot->valueNormal() );
                polygon << QPointF( N * m_unitMeasure->scale(UnitMeasure::loadF), (yRet * cos(rot->valueNormal()) + zRet * sin(rot->valueNormal())) *  m_unitMeasure->scale(UnitMeasure::loadM) );
            }
        }
    }

    sectRCncrRot.rotateNormal( - M_PI );
    sectRCncrRot.updateReadOnlyVars();

    // calcolo ySup e yInf nella sezione in cls
    zCMax = sectRCncrRot.m_d->sectionsCncr->zMaxNormal();
    zCMin = sectRCncrRot.m_d->sectionsCncr->zMinNormal();

    // calcolo ySup nelle armature in acciaiosezione in cls
    zSMax = sectRCncrRot.m_d->sectionsSteelCncr->zMaxNormal();

    NRd[0] = sectRCncrRot.NULSMinNormal();
    lmP1P2(&lRd, &mRd, zCMin, epsCU, zCMax, 0.0 );
    NRd[1] = sectRCncrRot.NULSNormal( lRd, mRd);
    lmP1P2(&lRd, &mRd, zCMin, epsCU, zSMax, epsUd);
    NRd[2] = sectRCncrRot.NULSNormal(lRd, mRd);
    NRd[3] = sectRCncrRot.NULSMaxNormal();

    for( int i=3; i > 0; i--){
        double yRet = 0.0, zRet = 0.0;
        bool found = sectRCncrRot.MULSNormal( &yRet, &zRet, NRd[i], 0.0, 0.0, 0.0 );
        if( found ){
            rotate2D( &yRet, &zRet, rot->valueNormal() + M_PI );
            polygon << QPointF( NRd[i] * m_unitMeasure->scale(UnitMeasure::loadF), (yRet * cos(rot->valueNormal()) + zRet * sin(rot->valueNormal())) * m_unitMeasure->scale(UnitMeasure::loadM));
        }
        for( int j=0; j < nP; j++){
            double N = NRd[i] + (NRd[i-1] - NRd[i]) * (j+1) / (nP+1);
            found = sectRCncrRot.MULSNormal( &yRet, &zRet, N, 0.0, 0.0, 0.0 );
            if( found ){
                rotate2D( &yRet, &zRet, rot->valueNormal() + M_PI );
                polygon << QPointF( N * m_unitMeasure->scale(UnitMeasure::loadF), (yRet * cos(rot->valueNormal()) + zRet * sin(rot->valueNormal())) * m_unitMeasure->scale(UnitMeasure::loadM));
            }
        }
    }

    if( !polygon.isEmpty() ){
        polygon << polygon.at(0);
    }

    return polygon;
}

QGraphicsItem * SectionRCncr::qGraphicsItem(){
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsItemGroup;
        if( m_d->sectionsCncr->qGraphicsItem() ){
            m_d->qGraphicsItem->addToGroup( m_d->sectionsCncr->qGraphicsItem() );
        }
        if( m_d->sectionsSteelCncr->qGraphicsItem() ){
            m_d->qGraphicsItem->addToGroup( m_d->sectionsSteelCncr->qGraphicsItem() );
        }
        if( m_d->sectionsSteel->qGraphicsItem() ){
            m_d->qGraphicsItem->addToGroup( m_d->sectionsSteel->qGraphicsItem() );
        }
        if( m_d->sectionsFRP->qGraphicsItem() ){
            m_d->qGraphicsItem->addToGroup( m_d->sectionsFRP->qGraphicsItem() );
        }
    }
    return m_d->qGraphicsItem;
}

double SectionRCncr::ANormal(){
    return m_d->sectionsCncr->ANormal();
}

double SectionRCncr::SyNormal(){
    return m_d->sectionsCncr->SyNormal();
}

double SectionRCncr::SzNormal(){
    return m_d->sectionsCncr->SzNormal();
}

double SectionRCncr::IyyNormal(){
    return m_d->sectionsCncr->IyyNormal();
}

double SectionRCncr::IzzNormal(){
    return m_d->sectionsCncr->IzzNormal();
}

double SectionRCncr::IyzNormal(){
    return m_d->sectionsCncr->IyzNormal();
}

double SectionRCncr::Sz3DyDzNormal(){
    return m_d->sectionsCncr->Sz3DyDzNormal();
}

double SectionRCncr::Sy3DyDzNormal(){
    return m_d->sectionsCncr->Sy3DyDzNormal();
}

double SectionRCncr::Syz2DyDzNormal(){
    return m_d->sectionsCncr->Syz2DyDzNormal();
}

double SectionRCncr::Sy2zDyDzNormal(){
    return m_d->sectionsCncr->Sy2zDyDzNormal();
}

double SectionRCncr::pWNormal(){
    return m_d->sectionsCncr->pWNormal();
}

bool SectionRCncr::NMSLSNormal( double *l, double *my, double *mz,
                                double N, double My, double Mz,
                                double yN, double zN ,
                                int maxIter, double prec ){
    double EA = m_d->sectionsCncr->EANormal() + m_d->sectionsSteelCncr->EANormal() + m_d->sectionsSteel->EANormal() + m_d->sectionsFRP->EANormal();
    double EIyy = m_d->sectionsCncr->EIyyNormal( yN, zN ) + m_d->sectionsSteelCncr->EIyyNormal( yN, zN ) + m_d->sectionsSteel->EIyyNormal( yN, zN ) + m_d->sectionsFRP->EIyyNormal( yN, zN );
    double EIzz = m_d->sectionsCncr->EIzzNormal( yN, zN ) + m_d->sectionsSteelCncr->EIzzNormal( yN, zN ) + m_d->sectionsSteel->EIzzNormal( yN, zN ) + m_d->sectionsFRP->EIzzNormal( yN, zN );
    double DNLim = prec * qMax( NULSMaxNormal(), fabs(NULSMinNormal()) );

    double MyEff = My + N * zN;
    double MzEff = Mz - N * yN;

    if( EA != 0.0 && EIyy != 0.0 &&EIzz != 0.0 ){
        int iter = 0;

        *l = N / EA;
        *my = My / EIyy;
        *mz = - Mz / EIzz;
        double DN = N - NSLSNormal( *l, *my, *mz );
        double MyRet = 0.0, MzRet = 0.0;
        MSLSNormal( &MyRet, &MzRet, *l, *my, *mz );
        double DMy = MyEff - MyRet;
        double DMz = MzEff - MzRet;

        while( fabs(DN) > DNLim && iter < maxIter ) {
            *l += DN / EA;
            *my += DMy / EIyy;
            *mz += - DMz / EIzz;

            DN = N - NSLSNormal( *l, *my, *mz );

            MyRet = 0.0; MzRet = 0.0;
            MSLSNormal( &MyRet, &MzRet, *l, *my, *mz );
            DMy = MyEff - MyRet;
            DMz = MzEff - MzRet;

            iter++;
        }
        if( DN < DNLim && iter < maxIter ){
            return true;
        }
    }
    *l = 0.0;
    *my = 0.0;
    *mz = 0.0;
    return false;
}

bool SectionRCncr::NMULSNormal(double *l, double *my, double *mz,
                               double N, double My, double Mz, double yN, double zN,
                               int maxIter, double prec ) {
    double EA = m_d->sectionsCncr->EANormal() + m_d->sectionsSteelCncr->EANormal() + m_d->sectionsSteel->EANormal() + m_d->sectionsFRP->EANormal();
    double EIyy = m_d->sectionsCncr->EIyyNormal( yN, zN ) + m_d->sectionsSteelCncr->EIyyNormal( yN, zN ) + m_d->sectionsSteel->EIyyNormal( yN, zN ) + m_d->sectionsFRP->EIyyNormal( yN, zN );
    double EIzz = m_d->sectionsCncr->EIzzNormal( yN, zN ) + m_d->sectionsSteelCncr->EIzzNormal( yN, zN ) + m_d->sectionsSteel->EIzzNormal( yN, zN ) + m_d->sectionsFRP->EIzzNormal( yN, zN );
    double DNLim = prec * qMax( NULSMaxNormal(), fabs(NULSMinNormal()) );

    double MyEff = My + N * zN;
    double MzEff = Mz - N * yN;

    if( EA != 0.0 && EIyy != 0.0 &&EIzz != 0.0 ){
        int iter = 0;

        *l = N / EA;
        *my = My / EIyy;
        *mz = - Mz / EIzz;
        double DN = N - NULSNormal( *l, *my, *mz );
        double MyRet = 0.0, MzRet = 0.0;
        MULSNormal( &MyRet, &MzRet, *l, *my, *mz );
        double DMy = MyEff - MyRet;
        double DMz = MzEff - MzRet;

        while( fabs(DN) > DNLim && iter < maxIter ) {
            *l += DN / EA;
            *my += DMy / EIyy;
            *mz += - DMz / EIzz;

            DN = N - NULSNormal( *l, *my, *mz );

            MyRet = 0.0; MzRet = 0.0;
            MULSNormal( &MyRet, &MzRet, *l, *my, *mz );
            DMy = MyEff - MyRet;
            DMz = MzEff - MzRet;

            iter++;
        }
        if( DN < DNLim && iter < maxIter ){
            return true;
        }
    }
    *l = 0.0;
    *my = 0.0;
    *mz = 0.0;
    return false;
}

bool SectionRCncr::NMULSNormal(QList<double> *l, QList<double> *my, QList<double> *mz,
                               int phase,
                               double N, double My, double Mz, double yN, double zN,
                               int maxIter, double prec ) {
    if( (l->size() > phase) && (my->size() > phase) && (mz->size() > phase)){
        double EA = m_d->sectionsCncr->EANormal() + m_d->sectionsSteelCncr->EANormal() + m_d->sectionsSteel->EANormal() + m_d->sectionsFRP->EANormal();
        double EIyy = m_d->sectionsCncr->EIyyNormal( yN, zN ) + m_d->sectionsSteelCncr->EIyyNormal( yN, zN ) + m_d->sectionsSteel->EIyyNormal( yN, zN ) + m_d->sectionsFRP->EIyyNormal( yN, zN );
        double EIzz = m_d->sectionsCncr->EIzzNormal( yN, zN ) + m_d->sectionsSteelCncr->EIzzNormal( yN, zN ) + m_d->sectionsSteel->EIzzNormal( yN, zN ) + m_d->sectionsFRP->EIzzNormal( yN, zN );

        double DNLim = prec * qMax( NULSMaxNormal(), fabs(NULSMinNormal()) );

        double MyEff = My + N * zN;
        double MzEff = Mz - N * yN;

        if( EA != 0.0 && EIyy != 0.0 &&EIzz != 0.0 ){
            int iter = 0;

            (*l)[phase] = N / EA;
            (*my)[phase] = My / EIyy;
            (*mz)[phase] = - Mz / EIzz;

            double DN = N - NULSNormal( phase, l, my, mz );
            double MyRet = 0.0, MzRet = 0.0;
            MULSNormal( &MyRet, &MzRet, phase, l, my, mz );
            double DMy = MyEff - MyRet;
            double DMz = MzEff - MzRet;

            while( DN > DNLim && iter < maxIter ) {
                (*l)[phase] += DN / EA;
                (*my)[phase] += DMy / EIyy;
                (*mz)[phase] += - DMz / EIzz;

                DN = N - NULSNormal( phase, l, my, mz );

                MyRet = 0.0; MzRet = 0.0;
                MULSNormal( &MyRet, &MzRet, phase, l, my, mz );
                DMy = MyEff - MyRet;
                DMz = MzEff - MzRet;

                iter++;
            }
            if( DN < DNLim && iter < maxIter ){
                return true;
            }
        }
    }
    return false;
}

bool SectionRCncr::NMULS( DoublePlus *l, DoublePlus *my, DoublePlus *mz,
                          DoublePlus *N, DoublePlus *My, DoublePlus *Mz, Point2DPlus *Ncen,
                          IntPlus * maxIter, DoublePlus * prec) {
    double lRet, myRet, mzRet;
    double precVal = 1.0e-4;
    int maxIterVal = 500;
    if( prec != NULL ){
        precVal = prec->valueNormal();
    }
    if( maxIter != NULL ){
        maxIterVal = maxIter->valueNormal();
    }
    bool retVal = NMULSNormal( &lRet, &myRet, &mzRet,
                               N->valueNormal(), My->valueNormal(), Mz->valueNormal(),
                               Ncen->y->valueNormal(), Ncen->z->valueNormal(), maxIterVal, precVal );
    l->setValueNormal( lRet );
    my->setValueNormal( myRet );
    mz->setValueNormal( mzRet );
    return retVal;
}

bool SectionRCncr::NMULS( QList<DoublePlus *> * l, QList<DoublePlus *> *my, QList<DoublePlus *> * mz,
                          SectionLoadPhase * phase,
                          DoublePlus *N, DoublePlus *My, DoublePlus *Mz, Point2DPlus *Ncen,
                          IntPlus * maxIter, DoublePlus * prec ) {
    bool retVal = false;
    int phaseNum = phase->phaseNum();
    if( (l->size() > phaseNum) && (my->size() > phaseNum) && (mz->size() > phaseNum) ){
        // set precVal e maxIter
        double precVal = 1.0e-4;
        int maxIterVal = 500;
        if( prec != NULL ){
            precVal = prec->valueNormal();
        }
        if( maxIter != NULL ){
            maxIterVal = maxIter->valueNormal();
        }

        QList<double> lRet, myRet, mzRet;
        for( int i=0; i < phaseNum; i++ ){
            lRet << l->at(i)->valueNormal();
            myRet << my->at(i)->valueNormal();
            mzRet << mz->at(i)->valueNormal();
        }
        lRet << 0.0;  myRet << 0.0;  mzRet << 0.0;

        retVal = NMULSNormal( &lRet, &myRet, &mzRet,
                              phaseNum,
                              N->valueNormal(), My->valueNormal(), Mz->valueNormal(),
                              Ncen->y->valueNormal(), Ncen->z->valueNormal(), maxIterVal, precVal );

        l->at(phaseNum)->setValueNormal( lRet.at(phaseNum) );
        my->at(phaseNum)->setValueNormal( myRet.at(phaseNum) );
        mz->at(phaseNum)->setValueNormal( mzRet.at(phaseNum) );
    }
    return retVal;
}

SectionLoadPhaseModel * SectionRCncr::loadPhaseModel() {
    if( m_d->loadPhaseModel == NULL ){
        m_d->loadPhaseModel = new SectionLoadPhaseModel( m_unitMeasure );
    }
    return m_d->loadPhaseModel;
}
