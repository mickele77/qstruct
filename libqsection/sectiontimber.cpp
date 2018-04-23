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
#include "sectiontimber.h"

#include "sectionloadphasemodel.h"
#include "timber.h"
#include "doubleplus.h"
#include "point2dplus.h"
#include "point2dplusmodel.h"
#include "qstringplus.h"
#include "unitmeasure.h"

#include <QXmlStreamAttributes>
#include <QComboBox>
#include <cmath>

SectionTimber::SectionTimber(UnitMeasure * um, Timber * c, const QString & n, QObject * parent ):
    SectionPoints( um, c, n, parent ){
    initVar();
}

SectionTimber::SectionTimber(UnitMeasure * ump, const QXmlStreamAttributes & attrs, MaterialModel * mModel, SectionLoadPhaseModel *lModel, QObject * parent ):
    SectionPoints( ump, NULL, "", parent ){
    initVar();
    loadFromXML( attrs, mModel, lModel );
}

SectionTimber::~SectionTimber(){
}

void SectionTimber::loadFromXML(const QXmlStreamAttributes &attrs, MaterialModel * mModel, SectionLoadPhaseModel *loadModel ){
    for ( int i=0; i<attrs.count(); ++i ) {
        for( QMap<QString, VarPlus *>::iterator iter = m_varContainer->begin(); iter != m_varContainer->end(); ++iter ){
            if ( attrs.at(i).name().toString().toUpper() == (*iter)->name().toUpper() ) {
                (*iter)->setValueNormal( attrs.at(i).value().toString() );
            } else if ( attrs.at(i).name().toString().toUpper() == "MATERIAL" ) {
                Timber * timb = dynamic_cast<Timber *>(mModel->materialId( attrs.at(i).value().toString().toUInt() ));
                if( timb == NULL ){
                    timb = dynamic_cast<Timber *>(mModel->firstMaterial( MaterialModel::TimberMaterial ) );
                }
                if( timb != NULL ){
                    setMaterial( timb );
                }
            } else if ( attrs.at(i).name().toString().toUpper() == "LOADPHASE" ) {
                if( loadModel != NULL ){
                    setLoadPhase( loadModel->loadPhaseId( attrs.at(i).value().toString().toUInt() ), false );
                }
            }
        }
    }
    updateReadOnlyVars();
}

void SectionTimber::initVar(){
    *m_typeNameInternal = "SectionTimber";
    typeName->setValue( trUtf8("Legno"));

    m_np = 0;
    for( int i=0; i < N_PAR; i++ ){
        m_p[i] = new DoublePlus(0.0, "p" + QString::number(i), m_unitMeasure, UnitMeasure::noDimension );
        m_p[i]->setRichName( "" );
        m_p[i]->setReadOnly( false );
        addVarToContainer( m_p[i] );
        m_p[i]->setEnabled( false );
    }

    sectionShape = new SectionTimberShape( SectionTimber::rectangular, "sectionShape" );
    sectionShape->setRichName( trUtf8("Forma"));
    addVarToContainer( sectionShape );
    sectionShape->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionTimber::updateParameters) );

    A->setReadOnly( true );
    A->addConnectedVars( 3, sectionShape, m_p[0], m_p[1] );

    Sy->setReadOnly( true );
    Sy->addConnectedVars( 3, sectionShape, m_p[0], m_p[1] );

    Sz->setReadOnly( true );
    Sz->addConnectedVars( 3, sectionShape, m_p[0], m_p[1] );

    Iyy->setReadOnly( true );
    Iyy->addConnectedVars( 3, sectionShape, m_p[0], m_p[1] );

    Izz->setReadOnly( true );
    Izz->addConnectedVars( 3, sectionShape, m_p[0], m_p[1] );

    Iyz->setReadOnly( true );

    kh = new DoublePlus(1.0, "kh", m_unitMeasure, UnitMeasure::noDimension );
    kh->setReadOnly( true );
    kh->setToolTip( trUtf8("Coefficiente amplificativo per sforzi normali di trazione paralleli alle fibre"));
    kh->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">h</span>"));
    addVarToContainer( kh );
    kh->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionTimber::setkh) );
    kh->addConnectedVars( 3, sectionShape, m_p[0], m_p[1] );

    khy = new DoublePlus(1.0, "khy", m_unitMeasure, UnitMeasure::noDimension );
    khy->setReadOnly( true );
    khy->setToolTip( trUtf8("Coefficiente amplificativo per sforzi normali di trazione paralleli alle fibre"));
    khy->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">h,y</span>"));
    addVarToContainer( khy );
    khy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionTimber::setkhy) );
    khy->addConnectedVars( 3, sectionShape, m_p[0], m_p[1] );

    khz = new DoublePlus(1.0, "khz", m_unitMeasure, UnitMeasure::noDimension );
    khz->setReadOnly( true );
    khz->setToolTip( trUtf8("Coefficiente amplificativo per sforzi normali di trazione paralleli alle fibre"));
    khz->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">h,z</span>"));
    addVarToContainer( khz );
    khz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionTimber::setkhz) );
    khz->addConnectedVars( 2, sectionShape, m_p[0] );

    ksh = new DoublePlus(1.0, "ksh", m_unitMeasure, UnitMeasure::noDimension );
    ksh->setReadOnly( true );
    ksh->setToolTip( trUtf8("Coefficiente amplificativo per sforzi tangenziali derivanti da torsione"));
    ksh->setRichName( trUtf8("k<span style=\" vertical-align:sub;\">sh</span>"));
    addVarToContainer( ksh );
    ksh->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionTimber::setksh) );
    ksh->addConnectedVars( 3, sectionShape, m_p[0], m_p[1] );

    Wy = new DoublePlus(0.0, "Wy", m_unitMeasure, UnitMeasure::sectL3 );
    Wy->setReadOnly( true );
    Wy->setToolTip( trUtf8("Modulo resistente elastico, asse y"));
    Wy->setRichName( trUtf8("W<span style=\" vertical-align:sub;\">y</span>"));
    addVarToContainer( Wy );
    Wy->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionTimber::setWy) );
    Wy->addConnectedVars( 3, sectionShape, m_p[0], m_p[1] );

    Wz = new DoublePlus(0.0, "Wz", m_unitMeasure, UnitMeasure::sectL3 );
    Wz->setReadOnly( true );
    Wz->setToolTip( trUtf8("Modulo resistente elastico, asse z"));
    Wz->setRichName( trUtf8("W<span style=\" vertical-align:sub;\">z</span>"));
    addVarToContainer( Wz );
    Wz->setUpdateValueMethod( this, static_cast<void(VarPlusContainer::*)(bool)>(&SectionTimber::setWz) );
    Wz->addConnectedVars( 3, sectionShape, m_p[0], m_p[1] );

    updateParameters( false );
}

void SectionTimber::updateParameters( bool emitAuto ){
    for( int i=0; i < N_PAR; i++ ){
        m_p[i]->setEnabled( false );
        m_p[i]->setValueNormal( 0.0, emitAuto );
        m_p[i]->setRichName("");
        m_p[i]->setUnitMeasure( UnitMeasure::noDimension );
    }

    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular : {
        m_np = 2;
        m_p[0]->setToolTip( trUtf8("Base della sezione"));
        m_p[0]->setRichName( trUtf8("b"));
        m_p[0]->setUnitMeasure( UnitMeasure::sectL );
        m_p[0]->setEnabled( true );

        m_p[1]->setToolTip( trUtf8("Altezza della sezione"));
        m_p[1]->setRichName( trUtf8("h"));
        m_p[1]->setUnitMeasure( UnitMeasure::sectL );
        m_p[1]->setEnabled( true );

        break; }
    case SectionTimber::circular : {
        m_np = 1;

        m_p[0]->setToolTip( trUtf8("Diametro"));
        m_p[0]->setRichName( trUtf8("d"));
        m_p[0]->setEnabled( true );

        break; }
    default: {
        break; }
    }
}

Section & SectionTimber::operator=( Section & val ){
    SectionTimber * sectTimber = dynamic_cast<SectionTimber *>(&val);
    if( sectTimber ){
        this->SectionTimber::operator=( *sectTimber );
    } else {
        SectionPoints * sectPoints = dynamic_cast<SectionPoints *>(&val);
        if( sectPoints ){
            this->SectionPoints::operator=( *sectPoints );
        } else {
            this->Section::operator=( val );
        }
    }
    return *this;
}

SectionTimber & SectionTimber::operator=( SectionTimber & sect ){
    this->SectionPoints::operator=( sect );
    m_material = sect.m_material;
    return *this;
}

int SectionTimber::np(){
    return m_np;
}

int SectionTimber::npAll(){
    return N_PAR;
}

DoublePlus * SectionTimber::p( int i ){
    if( i >= 0 && i < N_PAR ){
        return m_p[i];
    } else {
        return NULL;
    }
}


Timber * SectionTimber::timber(){
    return dynamic_cast<Timber *> (m_material);
}

void SectionTimber::setMaterial( Material * mat, bool emitAuto ){
    setTimber( dynamic_cast<Timber *>(mat), emitAuto );
}

void SectionTimber::setTimber( Timber * c, bool emitAuto ){
    Timber * oldTimber = dynamic_cast<Timber *>(m_material);
    if( oldTimber ){
        kh->removeConnectedVarContainer( oldTimber );
        khy->removeConnectedVarContainer( oldTimber );
        khz->removeConnectedVarContainer( oldTimber );
        ksh->removeConnectedVarContainer( oldTimber );
    }

    Section::setMaterial( c, emitAuto );

    Timber * newTimber = dynamic_cast<Timber *>(m_material);
    if( newTimber ){
        kh->addConnectedVarContainer( newTimber );
        khy->addConnectedVarContainer( newTimber );
        khz->addConnectedVarContainer( newTimber );
        ksh->addConnectedVarContainer( newTimber );
    }
}

QString SectionTimber::typeSectionNormal(){
    return QString("SectionTimber");
}

QString SectionTimber::typeSection(){
    return trUtf8("Sezione in Legno");
}

double SectionTimber::NULSPlusNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    if( timber() ){
        return ( A->valueNormal() * timber()->ft0dNormal( lsType, sClass, ldClass, kh->valueNormal() ) );
    }
    return 0.0;
}

double SectionTimber::NULSMinusNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    if( timber() ){
        return ( - A->valueNormal() * timber()->fc0dNormal( lsType, sClass, ldClass ) );
    }
    return 0.0;
}

double SectionTimber::VULSNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    switch( sectionShape->valueNormal() ){
    case SectionTimber::rectangular :{
        return ( m_p[0]->valueNormal() * m_p[1]->valueNormal() * timber()->fvdNormal( lsType, sClass, ldClass) * 2.0 / 3.0 );
        break;}
    case SectionTimber::circular :{
        return ( timber()->fvdNormal( lsType, sClass, ldClass) * M_PI * 3.0 * pow(m_p[0]->valueNormal() / 2.0,2.0) / 4.0 );
        break;}
    }
    return 0.0;
}

double SectionTimber::VULSYNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    return VULSNormal( lsType, sClass, ldClass );
}

double SectionTimber::VULSZNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    return VULSNormal( lsType, sClass, ldClass );
}

double SectionTimber::MULSXNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    if( timber() ){
        switch(sectionShape->valueNormal()){
        case SectionTimber::rectangular :{
            double b = m_p[0]->valueNormal();
            if( b > m_p[1]->valueNormal()){
                b = m_p[1]->valueNormal();
            }
            if( b != 0.0 ){
                return timber()->fvdNormal(lsType, sClass, ldClass) * ksh->valueNormal() * IT->valueNormal() / b;
            }
            break;}
        case SectionTimber::circular :{
            double r = m_p[0]->valueNormal() / 2.0;
            if( r != 0.0 ){
                return timber()->fvdNormal( lsType, sClass, ldClass) * ksh->valueNormal() * IT->valueNormal() / r ;
            }
            break;}
        }
    }
    return 0.0;
}

double SectionTimber::MULSYNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    if( timber() ){
        return Wy->valueNormal() * timber()->fmdNormal( lsType, sClass, ldClass, khy->valueNormal() );
    }
    return 0.0;
}

double SectionTimber::MULSZNormal(LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    if( timber() ){
        return Wz->valueNormal() * timber()->fmdNormal( lsType, sClass, ldClass, khz->valueNormal() );
    }
    return 0.0;
}

void SectionTimber::NULSPlus(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( NULSPlusNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal()));
}

void SectionTimber::NULSMinus(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( NULSMinusNormal( lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal() ));
}

void SectionTimber::VULS(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( VULSNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal() ));
}

void SectionTimber::VULSY(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( VULSYNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal() ));
}

void SectionTimber::VULSZ(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( VULSZNormal(lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal() ));
}

void SectionTimber::MULSX(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( MULSXNormal( lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal()));
}

void SectionTimber::MULSY(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( MULSYNormal( lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal()));
}

void SectionTimber::MULSZ(DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( MULSZNormal( lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal()));
}

double SectionTimber::betaCNormal(){
    Timber * timb = timber();
    if( timb ){
        switch(  timb->timberType->valueNormal() ){
        case Timber::solid : {
            return 0.2;
            break; }
        case Timber::glulam : {
            return 0.1;
            break; }
        default : {
            return 0.0;
            break; }
        }

    }
    return 0.0;
}

double SectionTimber::beamNCrYNormal( double Ly ){
    if( Ly != 0.0 ){
        return pow(M_PI, 2.0) * timber()->E0k->valueNormal() * Iyy->valueNormal() / pow( Ly, 2.0 );
    } else {
        return 0.0;
    }
}

double SectionTimber::beamNCrZNormal( double Lz ){
    if( Lz != 0.0 ){
        return pow(M_PI, 2.0) * timber()->E0k->valueNormal() * Izz->valueNormal() / pow( Lz, 2.0 );
    } else {
        return 0.0;
    }
}

double SectionTimber::beamlRelBucklNormal( double l ){
    if( timber() ){
        if( timber()->E0k->valueNormal() != 0.0 ){
            return (l * pow( timber()->fc0k->valueNormal() / timber()->E0k->valueNormal(), 0.50 ) / M_PI);
        }
    }
    return 0.0;
}

double SectionTimber::beamkBucklNormal( double lRel ){
    return ( 0.50 * ( 1.0 + betaCNormal() * ( lRel - 0.30) + pow(lRel,2.0) ) );
}

double SectionTimber::beamkCrBucklNormal( double k, double lRel ){
    if( lRel <= 0.30 ){
        return 1.0;
    } else {
        double R = pow(k,2.0) - pow(lRel, 2.0);
        if( R >= 0.0 ){
            double D = k + pow( R,0.50);
            if( D != 0.0 ){
                return 1.0 / D;
            }
        }
    }
    return 0.0;
}

double SectionTimber::beamNULSBucklNormal( double kCr,  LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    return (kCr * NULSMinusNormal(lsType, sClass, ldClass ) );
}

void SectionTimber::beamlRelBuckl( DoublePlus * ret, DoublePlus * l ){
    ret->setValueNormal( beamlRelBucklNormal(l->valueNormal() ));
}

void SectionTimber::beamkBuckl( DoublePlus * ret, DoublePlus * lRel ){
    ret->setValueNormal( beamkBucklNormal(lRel->valueNormal()));
}

void SectionTimber::beamkCrBuckl( DoublePlus * ret, DoublePlus * k, DoublePlus * lRel ){
    ret->setValueNormal( beamkCrBucklNormal(k->valueNormal(), lRel->valueNormal()));
}

void SectionTimber::beamNULSBuckl( DoublePlus * ret, DoublePlus * kCr, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( beamNULSBucklNormal( kCr->valueNormal(), lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal()));
}

double SectionTimber::beamlRelBucklLTYNormal( double Ly ){
    if((sectionShape->valueNormal() == SectionTimber::rectangular) && (m_p[0]->valueNormal() < m_p[1]->valueNormal())){
        if( timber() ){
            if( timber()->E0m->valueNormal() != 0.0 && Ly != 0.0 ){
                double sCr = M_PI * pow(m_p[0]->valueNormal(),2.0) * timber()->E0k->valueNormal() * pow( timber()->Gm->valueNormal() / timber()->E0m->valueNormal(), 0.50) / (Ly * m_p[1]->valueNormal());
                if (sCr != 0.0 ){
                    return pow( fabs(timber()->fmk->valueNormal() ) / sCr, 0.50);
                }
            }
        }
    }
    return 0.0;
}

double SectionTimber::beamlRelBucklLTZNormal( double Lz ){
    if((sectionShape->valueNormal() == SectionTimber::rectangular) && (m_p[0]->valueNormal() > m_p[1]->valueNormal()) ){
        if( timber() ){
            if( timber()->E0m->valueNormal() != 0.0 && Lz != 0.0 ){
                double sCr = M_PI * pow(m_p[1]->valueNormal(),2.0) * timber()->E0k->valueNormal() * pow( timber()->Gm->valueNormal() / timber()->E0m->valueNormal(), 0.50) / (Lz * m_p[0]->valueNormal());
                if (sCr != 0.0 ){
                    return pow( fabs(timber()->fmk->valueNormal() ) / sCr, 0.50);
                }
            }
        }
    }
    return 0.0;
}

double SectionTimber::beamkCrBucklLTNormal( double lRel ){
    if( sectionShape->valueNormal() == SectionTimber::rectangular ){
        if( lRel <= 0.75 ){
            return 1.0;
        } else if( lRel <= 1.4 ){
            return 1.56 - 0.75 * lRel;
        } else { // lRel > 1.4
            return 1.0 / pow(lRel, 2.0);
        }
    } else {
        return 1.0;
    }
}

void SectionTimber::beamlRelBucklLTY( DoublePlus * ret, DoublePlus * Ly ){
    ret->setValueNormal( beamlRelBucklLTYNormal( Ly->valueNormal() ));
}

void SectionTimber::beamlRelBucklLTZ( DoublePlus * ret, DoublePlus * Lz ){
    ret->setValueNormal( beamlRelBucklLTZNormal( Lz->valueNormal() ));
}

void SectionTimber::beamkCrBucklLT( DoublePlus  * ret, DoublePlus * lRel ){
    ret->setValueNormal( beamkCrBucklLTNormal( lRel->valueNormal() ));
}

double SectionTimber::beamMULSBucklLTYNormal( double k,  LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    return k * MULSYNormal( lsType, sClass, ldClass );
}

double SectionTimber::beamMULSBucklLTZNormal( double k,  LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass ){
    return k * MULSZNormal( lsType, sClass, ldClass );
}

void SectionTimber::beamMULSBucklLTY( DoublePlus * ret, DoublePlus * kCr, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( beamMULSBucklLTYNormal( kCr->valueNormal(), lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal() ));
}

void SectionTimber::beamMULSBucklLTZ( DoublePlus * ret, DoublePlus * kCr, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass ){
    ret->setValueNormal( beamMULSBucklLTZNormal( kCr->valueNormal(), lsType->valueNormal(), sClass->valueNormal(), ldClass->valueNormal() ));
}

void SectionTimber::MULSNormal(double *yRet,
                               double *zRet,
                               double l,
                               double my,
                               QList<Point2DModel *> *){
    *yRet = 0.0;
    *zRet = 0.0;
    if( m_pointsModel->pCount() > 2 && timber() != NULL ) {
        if( my == 0.0 ){
            ;
        } else {
            ;
        }
    }
}

double SectionTimber::NULSNormal( double l,
                                  double m,
                                  QList<Point2DModel *> * ){
    double NRet = 0.0;
    // TODO
    if( m_pointsModel->pCount() > 2 ){
        if( m == 0.0 ){
            ;
        } else {
            ;
        }
    }
    return NRet;
}

bool SectionTimber::verifyULSNormal(LSType::Type lsType,
                                    ServiceClass::Class sClass,
                                    LoadDurationClass::Class ldClass ,
                                    double kCritNy,
                                    double kCritNz,
                                    double N,
                                    double yM,
                                    double zM,
                                    double Mx,
                                    double kCritMy,
                                    double MyInput,
                                    double kCritMz,
                                    double MzInput,
                                    double Vy,
                                    double Vz,
                                    double * res,
                                    QString * messages ){
    if( messages ){
        messages->clear();
    }
    bool result = true;

    double tTor = 100.0;
    double MTRd = MULSXNormal( lsType, sClass, ldClass);
    if( MTRd != 0.0 ){
        tTor = Mx / MTRd;
    }

    double tShear = 100.0;
    double V = pow( pow(Vy,2.0) + pow(Vz,2.0), 0.50 );
    double VRd = VULSNormal( lsType, sClass, ldClass );
    if( VRd != 0.0 ){
        tShear = V / VRd;
    }
    double t = tShear;
    if( tTor != 0.0 ){
        t = tTor + pow(tShear,2.0);
    }
    if( t > 1.0 ){
        if( messages ){
            messages->append( trUtf8("Rottura per sforzo tangenziale (T e/o MT) (%imT: ") + QString::number(t) + QString(")\n"));
        }
        result = false;
    } else {
        if( messages ){
            messages->append( trUtf8("ImpT: ") + QString::number(t) + QString("\n"));
        }
    }


    double My = MyInput - N * yM;
    double Mz = MzInput + N * zM;

    double nn = 100.0;
    double nmy = 100.0;
    double nmz = 100.0;
    if( N < 0.0 ){
        double NRd = qMin( kCritNy, kCritNz) * NULSMinusNormal( lsType, sClass, ldClass );
        if( NRd != 0.0 ){
            nn = N / NRd;
        }
        nn = pow(nn, 2.0);
    } else { // N >= 0.0
        double NRd = NULSPlusNormal( lsType, sClass, ldClass );
        if( NRd != 0.0 ){
            nn = fabs( N / NRd );
        }
    }
    double MYRd = MULSYNormal( lsType, sClass, ldClass );
    if( MYRd != 0.0 ){
        nmy = fabs( My / MYRd );
    }
    double MZRd = MULSZNormal( lsType, sClass, ldClass );
    if( MZRd != 0.0 ){
        nmz = fabs( Mz / MZRd );
    }

    double n = 100.0;
    double km = 1.0;
    if( sectionShape->valueNormal() == SectionTimber::rectangular ){
        km = 0.70;
    }
    if( kCritMy != 0.0 && kCritMz != 0.0){
        double ImpN1 = nn + nmy * km + nmz / kCritMz;
        double ImpN2 = nn + nmy / kCritMy + nmz * km;
        if( ImpN1 > ImpN2 ){
            n = ImpN1;
        } else {
            n = ImpN2;
        }
        if( messages ){
            messages->append( trUtf8("ImpN1: ") + QString::number(ImpN1) + QString("\n"));
            messages->append( trUtf8("ImpN2: ") + QString::number(ImpN2) + QString("\n"));
        }
    }

    if( n > 1.0 ){
        if( messages ){
            messages->append( trUtf8("Rottura per sforzo normale (N, My e/o Mz) (%imNM: ") + QString::number(n) + QString(")\n"));
        }
        result = false;
    }
    if( t > n )
        *res = t;
    else
        *res = n;

    return result;

}

bool SectionTimber::verifyULS(LSType * lsType,
                              ServiceClass * sClass,
                              LoadDurationClass * ldClass ,
                              DoublePlus * kCritNy,
                              DoublePlus * kCritNz,
                              DoublePlus * N,
                              Point2DPlus * cenM,
                              DoublePlus * Mx,
                              DoublePlus * kCritMy,
                              DoublePlus * MyInput,
                              DoublePlus * kCritMz,
                              DoublePlus * MzInput,
                              DoublePlus * Vy,
                              DoublePlus * Vz,
                              DoublePlus * res,
                              QString * messages ){
    double yM = 0.0, zM = 0.0;
    if( cenM ){
        yM = cenM->y->valueNormal();
        zM = cenM->z->valueNormal();
    }

    double resVal;
    bool retVal = verifyULSNormal( lsType->valueNormal(),
                                   sClass->valueNormal(),
                                   ldClass->valueNormal(),
                                   kCritNy->valueNormal(),
                                   kCritNz->valueNormal(),
                                   N->valueNormal(),
                                   yM,
                                   zM,
                                   Mx->valueNormal(),
                                   kCritMy->valueNormal(),
                                   MyInput->valueNormal(),
                                   kCritMz->valueNormal(),
                                   MzInput->valueNormal(),
                                   Vy->valueNormal(),
                                   Vz->valueNormal(),
                                   &resVal,
                                   messages);
    res->setValueNormal( resVal );
    return retVal;
}

void SectionTimber::updateReadOnlyVars(){
    switch( sectionShape->valueNormal() ){
    case SectionTimber::rectangular: {
        int npts = m_pointsModel->pCount();
        if( npts < 4 ){
            m_pointsModel->insertRows( npts, (4 - npts));
        } else if( npts > 4 ){
            m_pointsModel->removeRows( 4, (npts - 4));
        }

        double b = m_p[0]->valueNormal();
        double h = m_p[1]->valueNormal();

        m_pointsModel->P(0)->y->setValueNormal( b/2.0 );
        m_pointsModel->P(0)->z->setValueNormal( h/2.0 );

        m_pointsModel->P(1)->y->setValueNormal( -b/2.0 );
        m_pointsModel->P(1)->z->setValueNormal( h/2.0 );

        m_pointsModel->P(2)->y->setValueNormal( -b/2.0 );
        m_pointsModel->P(2)->z->setValueNormal( -h/2.0 );

        m_pointsModel->P(3)->y->setValueNormal( b/2.0 );
        m_pointsModel->P(3)->z->setValueNormal( -h/2.0 );
        break;}
    case SectionTimber::circular: {
        int npts = m_pointsModel->pCount();
        if( npts < N_POINTS ){
            m_pointsModel->insertRows( npts, (N_POINTS - npts));
        } else if( npts > N_POINTS ){
            m_pointsModel->removeRows( N_POINTS, (npts - N_POINTS));
        }

        double r = m_p[0]->valueNormal() / 2.0 ;
        double DA = 2.0 * M_PI / N_POINTS;
        for(int i=0; i < N_POINTS; i++ ){
            m_pointsModel->P(i)->y->setValueNormal( r * cos( i * DA ));
            m_pointsModel->P(i)->z->setValueNormal( r * sin( i * DA ));
        }
        break; }
    }
    VarPlusContainer::updateReadOnlyVars();
}

double SectionTimber::ANormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        return ( m_p[0]->valueNormal() * m_p[1]->valueNormal() );
    case SectionTimber::circular :
        return ( M_PI * pow(m_p[0]->valueNormal()/2.0, 2.0) );
    default:
        return ( 0.0 );
    }
}

double SectionTimber::SyNormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        return ( 0.0 );
    case SectionTimber::circular :
        return ( 0.0 );
    default:
        return ( 0.0 );
    }
}

double SectionTimber::SzNormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        return ( 0.0 );
    case SectionTimber::circular :
        return ( 0.0 );
    default:
        return ( 0.0 );
    }
}

double SectionTimber::IyyNormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        return ( m_p[0]->valueNormal() *  pow(m_p[1]->valueNormal(), 3.0) / 12.0 );
    case SectionTimber::circular :
        return ( M_PI * pow( m_p[0]->valueNormal() / 2.0, 4.0 ) / 4.0 );
    default:
        return ( 0.0 );
    }
}

double SectionTimber::IzzNormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        return ( m_p[1]->valueNormal() *  pow(m_p[0]->valueNormal(), 3.0) / 12.0 );
    case SectionTimber::circular :
        return ( M_PI * pow( m_p[0]->valueNormal() / 2.0, 4.0 ) / 4.0 );
    default:
        return ( 0.0 );
    }
}

double SectionTimber::IyzNormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        return ( 0.0 );
    case SectionTimber::circular :
        return ( 0.0 );
    default:
        return ( 0.0 );
    }
}

double SectionTimber::Sz3DyDzNormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        return ( 0.0 );
    case SectionTimber::circular :
        return ( 0.0 );
    default:
        return ( 0.0 );
    }
}

double SectionTimber::Sy3DyDzNormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        return ( 0.0 );
    case SectionTimber::circular :
        return ( 0.0 );
    default:
        return ( 0.0 );
    }
}

double SectionTimber::Syz2DyDzNormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        return ( 0.0 );
    case SectionTimber::circular :
        return ( 0.0 );
    default:
        return ( 0.0 );
    }
}

double SectionTimber::Sy2zDyDzNormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        return ( 0.0 );
    case SectionTimber::circular :
        return ( 0.0 );
    default:
        return ( 0.0 );
    }
}

double SectionTimber::ITNormal(){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular : {
        double b = m_p[0]->valueNormal();
        double h = m_p[1]->valueNormal();
        if( b > h ){
            double c = b;
            b = h;
            h = c;
        }

        if( h != 0.0 ){
            double D =1.0 + 0.60 * b / h;
            if( D != 0.0 ){
                return ( pow(b, 3.0) * h / (3.0 * D));
            }
        }
        return 0.0;
        break; }
    case SectionTimber::circular : {
        double r = m_p[0]->valueNormal() / 2.0;
        return ( M_PI * pow(r, 4.0) / 2.0 );
        break;}
    default:
        return 0.0;
        break;
    }
}

void SectionTimber::setWy( bool emitAuto ){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        Wy->setValueNormal( m_p[0]->valueNormal() * pow(m_p[1]->valueNormal(), 2) / 6.0, emitAuto );
        break;
    case SectionTimber::circular :
        Wy->setValueNormal( M_PI * pow( m_p[0]->valueNormal()/2.0, 3.0) / 4.0, emitAuto );
        break;
    default:
        Wy->setValueNormal( 0.0, emitAuto );
        break;
    }
}

void SectionTimber::setWz( bool emitAuto ){
    switch( sectionShape->valueNormal() ) {
    case SectionTimber::rectangular :
        Wz->setValueNormal( m_p[1]->valueNormal() * pow(m_p[0]->valueNormal(), 2) / 6.0, emitAuto );
        break;
    case SectionTimber::circular :
        Wz->setValueNormal( M_PI * pow( m_p[0]->valueNormal()/2.0, 3.0) / 4.0, emitAuto );
        break;
    default:
        Wz->setValueNormal( 0.0, emitAuto );
        break;
    }
}

void SectionTimber::setkh( bool emitAuto ){
    if( kh->readOnly() ){
        if( timber() ){
            switch( sectionShape->valueNormal() ) {
            case SectionTimber::rectangular :  {
                double h = m_p[0]->valueNormal();
                if( m_p[1]->valueNormal() > h ){
                    h = m_p[1]->valueNormal();
                }
                kh->setValueNormal( timber()->khNormal(h), emitAuto );
                return;
                break; }
            case SectionTimber::circular :  {
                kh->setValueNormal( timber()->khNormal( m_p[0]->valueNormal() ), emitAuto );
                return;
                break; }
            }
        }
        kh->setValueNormal( 1.0, emitAuto );
    }
}

void SectionTimber::setkhy( bool emitAuto ){
    if( khy->readOnly() ){
        if( timber() ){
            switch( sectionShape->valueNormal() ) {
            case SectionTimber::rectangular :  {
                khy->setValueNormal( timber()->khNormal( m_p[1]->valueNormal()), emitAuto );
                return;
                break; }
            case SectionTimber::circular :  {
                khy->setValueNormal( timber()->khNormal( m_p[0]->valueNormal()), emitAuto );
                return;
                break; }
            }
        }
        khy->setValueNormal( 1.0, emitAuto );
    }
}

void SectionTimber::setkhz( bool emitAuto ){
    if( khz->readOnly() ){
        if( timber() ){
            switch( sectionShape->valueNormal() ) {
            case SectionTimber::rectangular :  {
                khz->setValueNormal( timber()->khNormal( m_p[0]->valueNormal()), emitAuto );
                return;
                break; }
            case SectionTimber::circular :  {
                khz->setValueNormal( timber()->khNormal( m_p[0]->valueNormal()), emitAuto );
                return;
                break; }
            }
        }
        khz->setValueNormal( 1.0 );
    }
}

void SectionTimber::setksh( bool emitAuto ){
    if( ksh->readOnly() ){
        if( timber() ){
            ksh->setValueNormal( timber()->kshNormal( sectionShape->valueNormal(), m_p[0]->valueNormal(), m_p[1]->valueNormal() ),
                    emitAuto );
        }
        ksh->setValueNormal( 1.0 );
    }
}

class SectionTimberEnumVal{
public:
    SectionTimberEnumVal(SectionTimber::Shape sh, const QString & ns, const QString & s):
        val(sh),
        normalStr(ns),
        str(s){
    };
    SectionTimber::Shape val;
    QString normalStr;
    QString str;
};

#include <QApplication>

class SectionTimberShapePrivate {
    Q_DECLARE_TR_FUNCTIONS(SectionTimberShapePrivate)
public:
    SectionTimberShapePrivate(SectionTimber::Shape sh):
        value(sh){
        enumList.append( SectionTimberEnumVal(SectionTimber::rectangular, "rectangular", trUtf8("rettangolare")) );
        enumList.append( SectionTimberEnumVal(SectionTimber::circular, "circular", trUtf8("circolare")) );
    };
    int valueIndex(){
        for( int i=0; i < enumList.size(); ++i){
            if( enumList.at(i).val == value ){
                return i;
            }
        }
        return 0;
    };
    SectionTimber::Shape value;
    QList<SectionTimberEnumVal> enumList;
};

SectionTimberShape::SectionTimberShape( SectionTimber::Shape sh,
                                        const QString & nn,
                                        bool ro):
    EnumPlus( nn, ro),
    m_d( new SectionTimberShapePrivate(sh) ){
}

SectionTimberShape::~SectionTimberShape(){
    delete m_d;
}

QString SectionTimberShape::typeVar(){
    return QString("SectionShape");
}

SectionTimberShape & SectionTimberShape::operator=( SectionTimberShape & val ){
    this->VarPlus::operator=( (VarPlus & ) val );

    if( this != &val ){
        setValueNormal( val.m_d->value );
    }
    return *this;
}

VarPlus & SectionTimberShape::operator=( VarPlus & val ){
    this->VarPlus::operator=( val );

    if( this != &val ){
        SectionTimberShape * valSectionTimberShape = dynamic_cast<SectionTimberShape *>(&val);
        if(valSectionTimberShape) {
            setValueNormal( valSectionTimberShape->m_d->value );
        }
    }
    return *this;
}

SectionTimber::Shape SectionTimberShape::value(){
    return m_d->value;
}

SectionTimber::Shape SectionTimberShape::valueNormal(){
    return m_d->value;
}

QString SectionTimberShape::valueNormalStr(){
    return m_d->enumList.at( m_d->valueIndex() ).normalStr ;
}

QString SectionTimberShape::valueStr(){
    return m_d->enumList.at( m_d->valueIndex() ).str ;
}

int SectionTimberShape::enumCount() {
    return m_d->enumList.size();
}

QString SectionTimberShape::enumStr(int v ) {
    if( v < m_d->enumList.size() ){
        return m_d->enumList.at(v).str;
    }
    return QString();
}

int SectionTimberShape::valueNormalInt() {
    return m_d->valueIndex();
}

void SectionTimberShape::setValueNormal( SectionTimber::Shape vv, bool emitAuto ){
    if( vv != m_d->value ){
        m_d->value = vv;
        emit valueChangedInternal( valueStr() );
        emit valueIntChangedWidget( m_d->valueIndex() );
        if( emitAuto && !m_signalsSuspended ){
            emit valueChanged( valueStr() );
            emit valueIntChanged( m_d->valueIndex() );
        }
    }
}

void SectionTimberShape::setValueNormal( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).normalStr.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SectionTimberShape::setValue( SectionTimber::Shape vv, bool emitAuto ){
    setValueNormal( vv, emitAuto );
}

void SectionTimberShape::setValue( const QString & vv, bool emitAuto ){
    QString v = vv.toUpper();
    for( int i=0; i < m_d->enumList.size(); ++i){
        if( m_d->enumList.at(i).str.toUpper() == v ){
            setValueNormal( m_d->enumList.at(i).val, emitAuto );
            return;
        }
    }
}

void SectionTimberShape::setValueInt( int i, bool emitAuto ){
    setValueNormal( m_d->enumList.at(i).val, emitAuto );
}
