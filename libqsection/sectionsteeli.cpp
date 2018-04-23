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
#include "sectionsteeli.h"

#include "steel.h"
#include "point2dmodel.h"
#include "point2dplus.h"
#include "doubleplus.h"
#include "structmath.h"

#include <QList>
#include <QPolygonF>
#include <QGraphicsPathItem>
#include <QBrush>
#include <QObject>
#include <cmath>

#define ARC_PREC 8

class SectionSteelIPrivate {
public:
    SectionSteelIPrivate( DoublePlus ** p ){
        np = 5;

        h = p[0];
        h->setToolTip( QObject::trUtf8("Altezza del profilo"));
        h->setRichName( QObject::trUtf8("h"));
        h->setUnitMeasure( UnitMeasure::sectL );
        h->setEnabled( true );

        b = p[1];
        b->setToolTip( QObject::trUtf8("Larghezza del profilo"));
        b->setRichName( QObject::trUtf8("b"));
        b->setUnitMeasure( UnitMeasure::sectL );
        b->setEnabled( true );

        tw = p[2];
        tw->setToolTip( QObject::trUtf8("Spessore dell'anima"));
        tw->setRichName( QObject::trUtf8("t<span style=\" vertical-align:sub;\">w</span>"));
        tw->setUnitMeasure( UnitMeasure::sectL );
        tw->setEnabled( true );

        tf = p[3];
        tf->setToolTip( QObject::trUtf8("Spessore delle ali"));
        tf->setRichName( QObject::trUtf8("t<span style=\" vertical-align:sub;\">f</span>"));
        tf->setUnitMeasure( UnitMeasure::sectL );
        tf->setEnabled( true );

        r = p[4];
        r->setToolTip( QObject::trUtf8("Raggio di raccordo tra anima e ali"));
        r->setRichName( QObject::trUtf8("r"));
        r->setUnitMeasure( UnitMeasure::sectL );
        r->setEnabled( true );

        qGraphicsItem = NULL;
    }
    ~SectionSteelIPrivate(){
        if( qGraphicsItem ){
            delete qGraphicsItem;
        }
    }

    // numero parametri
    int np;
    DoublePlus * h;
    DoublePlus * b;
    DoublePlus * tw;
    DoublePlus * tf;
    DoublePlus * r;

    QGraphicsPathItem * qGraphicsItem;
};

SectionSteelI::SectionSteelI( DoublePlus ** p, Steel * st, UnitMeasure * ump ):
    SectionSteelProfile( st, ump ),
    m_d( new SectionSteelIPrivate(p)){
    connect( m_d->h, &DoublePlus::valueChanged, this, &SectionSteelI::updateSectionSteelProfile );
    connect( m_d->b, &DoublePlus::valueChanged, this, &SectionSteelI::updateSectionSteelProfile );
    connect( m_d->tw, &DoublePlus::valueChanged, this, &SectionSteelI::updateSectionSteelProfile );
    connect( m_d->tf, &DoublePlus::valueChanged, this, &SectionSteelI::updateSectionSteelProfile );
    connect( m_d->r, &DoublePlus::valueChanged, this, &SectionSteelI::updateSectionSteelProfile );
}

SectionSteelI::~SectionSteelI(){
    delete m_d;
}

SectionSteel::SectionShape SectionSteelI::sectionShape() {
    return SectionSteel::sectionI;
}

int SectionSteelI::np(){
    return m_d->np;
}

double SectionSteelI::ANormal(){
    return ( 2.0 * m_d->b->valueNormal() * m_d->tf->valueNormal() + \
             (m_d->h->valueNormal() - 2.0 * m_d->tf->valueNormal()) * m_d->tw->valueNormal() + \
             (4.0 - M_PI) * pow(m_d->r->valueNormal(), 2.0) );
}

double SectionSteelI::SyNormal(){
    return 0.0;
}

double SectionSteelI::SzNormal(){
    return 0.0;
}

double SectionSteelI::IyyNormal(){
    return  ( 2.0 * (m_d->b->valueNormal() * pow(m_d->tf->valueNormal(), 3.0) / 12 + m_d->b->valueNormal() * m_d->tf->valueNormal() * pow((m_d->h->valueNormal() - m_d->tf->valueNormal()) / 2.0, 2.0) )
              + m_d->tw->valueNormal() * pow(m_d->h->valueNormal() - 2.0 * m_d->tf->valueNormal(), 3.0) / 12.0
              + 4.0 * ( pow(m_d->r->valueNormal(), 4.0) / 12.0 + pow(m_d->r->valueNormal(), 2.0)*pow(m_d->h->valueNormal() / 2.0 - m_d->tf->valueNormal() - m_d->r->valueNormal() / 2.0, 2.0)
                        - ( M_PI * pow(m_d->r->valueNormal(), 4.0) / 16.0 + 2.0 / 3.0 * (m_d->h->valueNormal() / 2.0 - m_d->tf->valueNormal() - m_d->r->valueNormal()) * pow(m_d->r->valueNormal(), 3.0) + M_PI * pow(m_d->r->valueNormal(), 2.0) * pow(m_d->h->valueNormal() / 2.0 - m_d->tf->valueNormal() - m_d->r->valueNormal(), 2.0) / 4.0 ) ));
}

double SectionSteelI::IzzNormal(){
    return m_d->tf->valueNormal() * pow(m_d->b->valueNormal(), 3.0) / 6.0 + pow(m_d->tw->valueNormal(), 3.0) * (m_d->h->valueNormal() - 2.0 * m_d->tf->valueNormal()) / 12.0
            + 4.0 * ( (pow(m_d->r->valueNormal(), 4) / 12.0 + pow(m_d->r->valueNormal(), 2) * pow(m_d->tw->valueNormal() + m_d->r->valueNormal(), 2.0) / 4.0 )
                      - (  M_PI * pow(m_d->r->valueNormal(), 4.0) / 16.0 - 2.0 / 3.0 * (m_d->tw->valueNormal() / 2.0 + m_d->r->valueNormal()) * pow(m_d->r->valueNormal(), 3.0) + M_PI * pow(m_d->r->valueNormal(), 2.0)*pow(m_d->tw->valueNormal() / 2.0 + m_d->r->valueNormal(), 2.0) / 4.0) );
}

double SectionSteelI::IyzNormal(){
    return 0.0;
}

double SectionSteelI::Sz3DyDzNormal(){
    return 0.0;
}

double SectionSteelI::Sy3DyDzNormal(){
    return 0.0;
}

double SectionSteelI::Syz2DyDzNormal(){
    return 0.0;
}

double SectionSteelI::Sy2zDyDzNormal(){
    return 0.0;
}

double SectionSteelI::maxTNormal(){
    if( m_d->tf->valueNormal() > m_d->tw->valueNormal() ){
        return m_d->tf->valueNormal();
    } else {
        return m_d->tw->valueNormal();
    }
}

double SectionSteelI::ITNormal(){
    return 2.0 * (m_d->b->valueNormal() - 0.63*m_d->tf->valueNormal()) * pow(m_d->tf->valueNormal(), 3.0) / 3.0 + \
            (m_d->h->valueNormal() - 2.0 * m_d->tf->valueNormal()) * pow(m_d->tw->valueNormal(),3.0) / 3.0 + \
            2.0 * m_d->tw->valueNormal() / m_d->tf->valueNormal() * (0.145 + 0.1 * m_d->r->valueNormal() / m_d->tf->valueNormal()) * pow( (pow((m_d->r->valueNormal()+m_d->tw->valueNormal() / 2.0), 2.0) +  pow((m_d->r->valueNormal()+m_d->tf->valueNormal()),2.0) - pow( m_d->r->valueNormal(),2.0) ) / (2*m_d->r->valueNormal()+m_d->tf->valueNormal()), 4.0);
}


double SectionSteelI::IwNormal(){
    return m_d->tf->valueNormal() * pow(m_d->b->valueNormal(), 3.0) * pow((m_d->h->valueNormal()-m_d->tf->valueNormal()),2.0) / 24.0;
}

double SectionSteelI::tyNormal(){
    return 2.3;
}

double SectionSteelI::tzNormal(){
    return 2.3;
}

double SectionSteelI::tyzNormal(){
    return 0.0;
}

double SectionSteelI::WElMyNormal(){
    if( m_d->h->valueNormal() == 0.0 ){
        return 0.0;
    } else {
        return ( IyyNormal() * 2.0 / m_d->h->valueNormal());
    }
}

double SectionSteelI::WElMzNormal(){
    if( m_d->b->valueNormal() == 0 ){
        return 0.0;
    } else {
        return ( IzzNormal() * 2.0 / m_d->b->valueNormal());
    }
}

double SectionSteelI::WPlMyNormal(){
    return 2.0 * ( m_d->b->valueNormal() * m_d->tf->valueNormal() * (m_d->h->valueNormal() - m_d->tf->valueNormal()) / 2.0 + m_d->tw->valueNormal() * pow(m_d->h->valueNormal() / 2.0 - m_d->tf->valueNormal(), 2.0) / 2.0
                   + 2.0 * (pow(m_d->r->valueNormal(), 2.0)* (m_d->h->valueNormal() / 2.0 - m_d->tf->valueNormal() - m_d->r->valueNormal() / 2.0) - pow(m_d->r->valueNormal(), 3.0) / 3.0 - M_PI* pow(m_d->r->valueNormal(), 2.0)*(m_d->h->valueNormal() / 2.0 - m_d->tf->valueNormal() - m_d->r->valueNormal()) / 4.0 ) );
}

double SectionSteelI::WPlMzNormal(){
    return ( 2.0 * ( m_d->tf->valueNormal() * pow(m_d->b->valueNormal(), 2.0) / 4.0 + (m_d->h->valueNormal() - 2.0 * m_d->tf->valueNormal()) * pow(m_d->tw->valueNormal(), 2.0) / 8.0
                     + 2.0 *(pow(m_d->r->valueNormal(), 2.0)*(m_d->tw->valueNormal() + m_d->r->valueNormal()) / 2.0 + pow(m_d->r->valueNormal(), 3.0) / 3.0 - M_PI*(pow(m_d->r->valueNormal(), 2.0)*(m_d->tw->valueNormal() / 2.0 + m_d->r->valueNormal())) / 4.0) ) );
}

void SectionSteelI::EffNMinusNormal( double * AEff, double * yGEff, double * zGEff, double){
    double eps = 0.0;
    if( fykNormal() > 0.0  ){
        eps = pow( 2.35e+8 /  fykNormal(), 0.50 );
    }

    double lf = 0.50 * (m_d->b->valueNormal() - m_d->tw->valueNormal() - 2.0 * m_d->r->valueNormal());
    if( m_d->tf->valueNormal() != 0.0 ){
        double lp = lf / m_d->tf->valueNormal();
        if( lp > 14.0 * eps ){
            lp /= (28.40 * eps * sqrt(0.43) );
            double rho = 1.0;
            if( lp > 0.748 ){
                rho = (lp - 0.188) / pow( lp,2.0 );
                if( rho > 1.0 ){
                    rho = 1.0;
                }
            }
            lf *= rho;
        }
    }
    double lw = m_d->h->valueNormal() - 2.0 * (m_d->tf->valueNormal() + m_d->r->valueNormal());
    if( m_d->tw->valueNormal() != 0.0 ){
        double lp = lw / m_d->tw->valueNormal();
        if( lp > 42.0 * eps ){
            lp /= (28.40 * eps * 2.0 );
            double rho = 1.0;
            if( lp > 0.673 ){
                rho = (lp - 0.055 * 4.0) / pow( lp,2.0);
            }
            if( rho > 1.0 ){
                rho = 1.0;
            }
            lw *= rho;
        }
    }

    *AEff = 4.0 * m_d->tf->valueNormal() * lf +
            2.0 * ( ( 2.0 * m_d->r->valueNormal() + m_d->tw->valueNormal() ) * ( m_d->r->valueNormal() + m_d->tf->valueNormal() ) - M_PI * pow(m_d->r->valueNormal(),2.0) / 2.0 )
            + m_d->tw->valueNormal() * lw;
    *yGEff = 0.0;
    *zGEff = 0.0;
}

double SectionSteelI::WEffMyMaxNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelI::WEffMyMinNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelI::WEffMzMaxNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelI::WEffMzMinNormal( double ){
    // TODO
    return 0.0;
}

double SectionSteelI::AVyNormal(){
    return ( ANormal() - (m_d->h->valueNormal() - 2.0 * m_d->tf->valueNormal()) * m_d->tw->valueNormal() );
}

double SectionSteelI::AVzNormal(){
    return ( ANormal() - 2.0 * m_d->b->valueNormal() * m_d->tf->valueNormal() + ( m_d->tf->valueNormal() + 2.0 * m_d->r->valueNormal() ) * m_d->tf->valueNormal() );
}

QString SectionSteelI::NClass(){
    int wClass = internalPartClass( m_d->h->valueNormal() - 2.0 * (m_d->tf->valueNormal() + m_d->r->valueNormal()), m_d->tw->valueNormal(), 1.0,1.0);;
    int fClass = outstandFlangeClass( (m_d->b->valueNormal() - 2.0 * m_d->r->valueNormal() - m_d->tw->valueNormal()) / 2.0, m_d->tf->valueNormal(), 1.0,1.0);

    if( fClass == wClass ){
        return QString::number(fClass);
    } else {
        if( fClass > wClass ){
            return ( QString::number(fClass) + QString("f") );
        } else {
            return ( QString::number(wClass) + QString("w") );
        }
    }
}

QString SectionSteelI::MyClass(){
    int wClass = internalPartClass( m_d->h->valueNormal() - 2.0 * (m_d->tf->valueNormal() + m_d->r->valueNormal()), m_d->tw->valueNormal(), -1.0,1.0);;
    int fClass = outstandFlangeClass( (m_d->b->valueNormal() - 2.0 * m_d->r->valueNormal() - m_d->tw->valueNormal()) / 2.0, m_d->tf->valueNormal(), 1.0, 1.0);
    if( fClass == wClass ){
        return ( QString::number(fClass));
    } else {
        if( fClass > wClass ){
            return ( QString::number(fClass) + QString("f") );
        } else {
            return ( QString::number(wClass) + QString("w") );
        }
    }
}

QString SectionSteelI::MyMaxClass(){
    return MyClass();
}

QString SectionSteelI::MyMinClass(){
    return MyClass();
}

double SectionSteelI::MyClassLimitNormal(){
    double l1 = 0.25 * fydNormal(0) * ANormal();
    double hw = m_d->h->valueNormal() - 2.0 * m_d->tf->valueNormal() - 2.0 * m_d->r->valueNormal();
    double l2 = 0.50 * hw * m_d->tw->valueNormal() * fydNormal(0);
    if( l1 < l2 ){
        return ( -l1 );
    } else {
        return ( -l2 );
    }
}

QString SectionSteelI::MzClass(){
    int fClass = outstandFlangeClass( (m_d->b->valueNormal() - 2.0 * m_d->r->valueNormal() - m_d->tw->valueNormal()) / 2.0, m_d->tf->valueNormal(), 0.0,1.0);
    return QString::number(fClass);
}

QString SectionSteelI::MzMaxClass(){
    return MzClass();
}

QString SectionSteelI::MzMinClass(){
    return MzClass();
}

double SectionSteelI::MzClassLimitNormal(){
    double hw = m_d->h->valueNormal() - 2.0 * m_d->tf->valueNormal() - 2.0 * m_d->r->valueNormal();
    double l = hw * m_d->tw->valueNormal() * fydNormal(0);
    return ( -l );
}

QGraphicsItem * SectionSteelI::qGraphicsItem(){
    if( m_d->qGraphicsItem == NULL ){
        m_d->qGraphicsItem = new QGraphicsPathItem();
        m_d->qGraphicsItem->setBrush( QBrush( Qt::lightGray ));
        updateQGraphics();
    }
    return m_d->qGraphicsItem;
}

void SectionSteelI::classDiagram( QList<QPolygonF *> * clDiagram, double NULSMax, double MyULSMax, double MzULSMax ){
    // verifica la dimensione di clDiagram
    while( clDiagram->size() > 4 ){
        delete clDiagram->at(0);
        clDiagram->removeAt(0);
    }
    while( clDiagram->size() < 4 ){
        clDiagram->append(new QPolygonF );
    }
    // azzera clDIagram
    for(int i=0; i < clDiagram->size(); i++){
        clDiagram->at(i)->clear();
    }

    if( m_steel ){
        // analizziamo l'anima
        QList<QPolygonF *> * diagramListPr = new QList<QPolygonF *>;
        diagramListPr->append( new QPolygonF );
        diagramListPr->append( new QPolygonF );
        diagramListPr->append( new QPolygonF );

        int classWN = internalPartClass( m_d->h->valueNormal() - 2.0 * (m_d->tf->valueNormal() + m_d->r->valueNormal()), m_d->tw->valueNormal(), 1.0,1.0);
        int classWM = internalPartClass( m_d->h->valueNormal() - 2.0 * (m_d->tf->valueNormal() + m_d->r->valueNormal()), m_d->tw->valueNormal(), -1.0,1.0);
        int classWMin = classWM;
        int classWMax =  classWN;
        if( classWMin > classWMax ){
            classWMin = classWN;
            classWMax =  classWM;
        }
        int cl = classWMin;
        double c = m_d->h->valueNormal() - 2.0 * (m_d->tf->valueNormal() + m_d->r->valueNormal());
        double t = m_d->tw->valueNormal();
        double eps = pow( 2.35e+8 /  fykNormal(), 0.50 );
        if( c != 0.0 && t != 0.0 ){
            while( cl < classWMax ){
                switch(cl){
                case 1:{
                    (*diagramListPr->at(0)) << QPointF( M_PI/2.0, 0.0 ) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( 0.0, M_PI/2.0 );
                    double alpha = 1.0;
                    double a = (1.0 + 396.0 * eps / (c/t) ) / 13.0;
                    if( a >= 0.5 && a <= 1.0 ){
                        alpha = a;
                    } else {
                        a = 36.0 * eps / (c/t);
                        if( a >= 0.0 && a <= 0.5 ){
                            alpha = a;
                        }
                    }
                    if( alpha > 0.0 && alpha < 1.0 ){
                        int nP = 99;
                        double yCen = 0.0;
                        double zCen = - c/2.0 + alpha * c;

                        double theta = 0.0;
                        double dTheta = (M_PI/2.0) / nP;
                        double l, my, mz;
                        double N, My, Mz;
                        for( int i=0; i < (nP+1); i++ ){
                            my = cos(theta);
                            mz = sin(theta);
                            l = - mz * yCen - my * zCen;
                            NMULSPlastNormal( &N, &My, &Mz,l, my, mz, 0.0, 0.0);
                            QPointF P( xi( My, Mz, MyULSMax, MzULSMax), Theta(N, My, Mz, NULSMax, MyULSMax, MzULSMax));
                            if( diagramListPr->at(0)->last() != P ){
                                (*(diagramListPr->at(0))) << P;
                            }
                            if( i < nP ){
                                theta += dTheta;
                            } else {
                                theta = M_PI / 2.0;
                            }
                        }
                    }
                    break;}
                case 2:{
                    (*diagramListPr->at(1)) << QPointF( M_PI/2.0, 0.0 ) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( 0.0, M_PI/2.0 );
                    double alpha = 1.0;
                    double a = (1.0 + 456.0 * eps / (c/t) ) / 13.0;
                    if( a >= 0.5 && a <= 1.0 ){
                        alpha = a;
                    } else {
                        a = 41.5 * eps / (c/t);
                        if( a >= 0.0 && a <= 0.5 ){
                            alpha = a;
                        }
                    }
                    if( alpha > 0.0 && alpha < 1.0 ){
                        int nP = 99;
                        double yCen = 0.0;
                        double zCen = - c/2.0 + alpha * c;

                        double theta = 0.0;
                        double dTheta = (M_PI/2.0) / nP;
                        double l, my, mz;
                        double N, My, Mz;
                        for( int i=0; i < (nP+1); i++ ){
                            my = cos(theta);
                            mz = sin(theta);
                            l = - mz * yCen - my * zCen;
                            NMULSPlastNormal( &N, &My, &Mz,l, my, mz, 0.0, 0.0);
                            QPointF P( xi(My, Mz, MyULSMax, MzULSMax), Theta(N, My, Mz, NULSMax, MyULSMax, MzULSMax));
                            if( diagramListPr->at(1)->last() != P )
                                (*(diagramListPr->at(1))) << P;
                            if( i < nP ){
                                theta += dTheta;
                            } else {
                                theta = M_PI / 2.0;
                            }
                        }
                    }
                    break;}
                case 3:{
                    (*diagramListPr->at(2)) << QPointF( M_PI/2.0, 0.0 ) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( 0.0, M_PI/2.0 );
                    double phi = 1.0;
                    double p = (42.0 * eps / (c/t) - 0.67 ) / 0.33;
                    if( p > -1.0 ){
                        phi = p;
                    } else {
                        if( solveCardano( &p, -1.0, (c/t)/ 62.0 * eps) ){
                            p = - pow(p, 2.0);
                            if( p <= 1.0 ){
                                phi = p;
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    }
                    if( phi != 1.0  ){
                        double alpha = 1.0 / (1.0 - phi);
                        int nP = 99;
                        double yCen = 0.0;
                        double zCen = - c/2.0 + alpha * c;

                        double theta = 0.0;
                        double dTheta = (M_PI/2.0) / nP;
                        double l, my, mz;
                        double N, My, Mz;
                        for( int i=0; i < (nP+1); i++ ){
                            my = cos(theta);
                            mz = sin(theta);
                            l = - mz * yCen - my * zCen;
                            double eMin = 0.0;
                            if( m_sectionPoints->nPoints() > 0 ){
                                eMin = l + mz * m_sectionPoints->yNormal(0) + my * m_sectionPoints->zNormal(0);
                                for( int  j = 1; j < m_sectionPoints->nPoints(); j++){
                                    double e = l + mz * m_sectionPoints->yNormal(j) + my * m_sectionPoints->zNormal(j);
                                    if( e < eMin ){
                                        eMin = e;
                                    }
                                }
                            }
                            if( eMin != 0.0 && steel() != NULL ){
                                double ey = fydNormal(0) / m_steel->E->valueNormal();
                                l = l * ey / fabs(eMin);
                                mz = mz * ey / fabs(eMin);
                                my = my * ey / fabs(eMin);
                            }
                            NMULSElastPlastNormal( &N, &My, &Mz,l, my, mz, 0.0, 0.0);

                            QPointF P( xi( My, Mz, MyULSMax, MzULSMax), Theta(N, My, Mz, NULSMax, MyULSMax, MzULSMax));
                            if( diagramListPr->at(2)->last() != P ){
                                (*(diagramListPr->at(2))) << P;
                            }
                            if( i < nP ){
                                theta += dTheta;
                            } else {
                                theta = M_PI / 2.0;
                            }
                        }
                    }
                    break;}
                }
                cl++;
            }
        }
        for( int i=0; i < 3; i++){
            if( !diagramListPr->at(i)->isClosed() && diagramListPr->at(i)->size() > 0 ){
                (*diagramListPr->at(i)) << (diagramListPr->at(i)->at(0));
            }
        }

        QList<QPolygonF *> * diagramListW = new QList<QPolygonF *>;
        diagramListW->append( new QPolygonF );
        diagramListW->append( new QPolygonF );
        diagramListW->append( new QPolygonF );
        diagramListW->append( new QPolygonF );
        if( classWMax > 0 && classWMin > 0 ){
            if( classWMax == classWMin ){
                *(diagramListW->at(classWMax-1)) << QPointF( 0.0, M_PI/2.0 ) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( M_PI/2.0, - M_PI/2.0 ) << QPointF( 0.0, - M_PI/2.0 ) << QPointF( 0.0, M_PI/2.0 );
            } else {
                *(diagramListW->at(classWMin-1)) = *(diagramListPr->at(classWMin-1));
                for( int cl = classWMin; cl < (classWMax-1); cl++ ){
                    *(diagramListW->at(cl)) = diagramListPr->at(cl)->subtracted( *(diagramListPr->at(cl-1)) );
                }
                QPolygonF rect;
                rect << QPointF( 0.0, M_PI/2.0 ) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( M_PI/2.0, - M_PI/2.0 ) << QPointF( 0.0, - M_PI/2.0 );
                *(diagramListW->at(classWMax-1)) = rect.subtracted( *(diagramListPr->at(classWMax-2)) );
            }
        }

        // analizziamo le ali
        for( int i=0; i < 3; i++){
            diagramListPr->at(i)->clear();
        }
        c = (m_d->b->valueNormal() - 2.0 * m_d->r->valueNormal() - m_d->tw->valueNormal()) / 2.0;
        t = m_d->tf->valueNormal();
        int classFN = outstandFlangeClass( c, t, 1.0,1.0);
        int classFM = outstandFlangeClass( c, t, -1.0e+5,1.0);
        int classFMin = classFM;
        int classFMax =  classFN;
        if( classFMin > classFMax ){
            classFMin = classFN;
            classFMax =  classFM;
        }
        cl = classFMin;
        if( c != 0.0 && t != 0.0){
            while( cl < classFMax ){
                switch(cl){
                case 1:{
                    *(diagramListPr->at(0)) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( 0.0, M_PI/2.0 );
                    double l, my, mz;
                    double N, My, Mz;
                    int nP = 99;
                    double theta = -M_PI;
                    double dTheta = (M_PI/2.0) / nP;
                    double yCen = 0.0;
                    double zCen = (m_d->h->valueNormal() - m_d->tf->valueNormal()) / 2.0;
                    for( int i=0; i < (nP+1); i++ ){
                        my = cos(theta);
                        mz = sin(theta);
                        l = - mz * yCen - my * zCen;
                        if( mz > 0.0 ){
                            double alpha = pow( 9.0 * eps / (c/t), 2.0/3.0 );
                            if( alpha >= 0.0 && alpha <= 1.0 ){
                                yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal() + alpha * c;
                            } else {
                                yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal();
                            }
                        } else if( mz < 0.0 ){
                            double alpha = 9.0 * eps / (c/t);
                            if( alpha >= 0.0 && alpha <= 1.0 ){
                                yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal() + (1.0 - alpha) * c;
                            } else {
                                yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal();
                            }
                        } else {
                            yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal();
                        }

                        l = - mz * yCen - my * zCen;
                        NMULSPlastNormal( &N, &My, &Mz,l, my, mz, 0.0, 0.0);
                        QPointF P( xi( My, Mz, MyULSMax, MzULSMax), Theta(N, My, Mz, NULSMax, MyULSMax, MzULSMax));
                        if( diagramListPr->at(0)->last() != P ){
                            (*(diagramListPr->at(0))) << P;
                        }
                        if( i < nP ){
                            theta += dTheta;
                        } else {
                            theta = - M_PI / 2.0;
                        }
                    }
                    break;}
                case 2:{
                    *(diagramListPr->at(1)) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( 0.0, M_PI/2.0 ) ;
                    double l, my, mz;
                    double N, My, Mz;
                    int nP = 99;
                    double theta = -M_PI;
                    double dTheta = (M_PI/2.0) / nP;
                    double yCen = 0.0;
                    double zCen = (m_d->h->valueNormal() - m_d->tf->valueNormal()) / 2.0;
                    for( int i=0; i < (nP+1); i++ ){
                        my = cos(theta);
                        mz = sin(theta);
                        l = - mz * yCen - my * zCen;
                        if( mz > 0.0 ){
                            double alpha = pow( 10.0 * eps / (c/t), 2.0/3.0 );
                            if( alpha >= 0.0 && alpha <= 1.0 ){
                                yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal() + alpha * c;
                            } else {
                                yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal();
                            }
                        } else if( mz < 0.0 ){
                            double alpha = 10.0 * eps / (c/t);
                            if( alpha >= 0.0 && alpha <= 1.0 ){
                                yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal() + (1.0 - alpha) * c;
                            } else {
                                yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal();
                            }
                        } else {
                            yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal();
                        }

                        l = - mz * yCen - my * zCen;
                        NMULSPlastNormal( &N, &My, &Mz,l, my, mz, 0.0, 0.0);
                        QPointF P( xi(My, Mz, MyULSMax, MzULSMax), Theta(N, My, Mz, NULSMax, MyULSMax, MzULSMax));
                        if( diagramListPr->at(1)->last() != P ){
                            (*(diagramListPr->at(1))) << P;
                        }
                        if( i < nP ){
                            theta += dTheta;
                        } else {
                            theta = - M_PI / 2.0;
                        }
                    }
                    break;}
                case 3:{
                    *(diagramListPr->at(2)) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( 0.0, M_PI/2.0 );
                    double l, my, mz;
                    double N, My, Mz;
                    int nP = 99;
                    double theta = -M_PI;
                    double dTheta = (M_PI/2.0) / nP;
                    double yCen = 0.0;
                    double zCen = (m_d->h->valueNormal() - m_d->tf->valueNormal()) / 2.0;
                    for( int i=0; i < (nP+1); i++ ){
                        my = cos(theta);
                        mz = sin(theta);
                        l = - mz * yCen - my * zCen;
                        double ks = pow( c / (t * 21.0 * eps), 2.0);
                        double phi = 1.0;
                        if( mz > 0.0 ){
                            double k = ks;
                            if( k < 0.43)
                                k = 0.43;
                            else if( k > 23.8)
                                k = 23.8;

                            if( k <= 0.43 ){
                                phi = 1.0;
                            } else if( k <= 1.7 ){
                                phi = 0.578 / k - 0.34;
                            } else if( k <= 23.8 ){
                                phi = (5.0 - pow((25.0-4.0*17.1*(1.7-k)),0.5)) / (2.0*17.1);
                            }
                        } else if( mz < 0.0 ){
                            double k = ks;
                            if( k < 0.43)
                                k = 0.43;
                            else if( k > 1.83)
                                k = 1.83;
                            phi = (3.0- pow((pow(3.0,2.0)-4.0*(0.57-k)/0.07),0.5) )*0.5;
                        }
                        if( phi != 1.0  ){
                            double alpha = 1.0 / (1.0 - phi);
                            yCen = m_d->tw ->valueNormal() / 2.0 + m_d->r->valueNormal() + (1.0 - alpha) * c;

                            double eMin = 0.0;
                            if( m_sectionPoints->nPoints() > 0 ){
                                eMin = l + mz * m_sectionPoints->yNormal(0) + my * m_sectionPoints->zNormal(0);
                                for( int  j = 1; j < m_sectionPoints->nPoints(); j++){
                                    double e = l + mz * m_sectionPoints->yNormal(j) + my * m_sectionPoints->zNormal(j);
                                    if( e < eMin ){
                                        eMin = e;
                                    }
                                }
                            }
                            if( eMin != 0.0 && steel() != NULL ){
                                double ey = fydNormal(0) / m_steel->E->valueNormal();
                                l = l * ey / fabs(eMin);
                                mz = mz * ey / fabs(eMin);
                                my = my * ey / fabs(eMin);
                            }
                            NMULSElastPlastNormal( &N, &My, &Mz,l, my, mz, 0.0, 0.0);
                            QPointF P( xi(My, Mz, MyULSMax, MzULSMax), Theta(N, My, Mz, NULSMax, MyULSMax, MzULSMax));
                            if( diagramListPr->at(2)->last() != P ){
                                (*(diagramListPr->at(2))) << P;
                            }
                            if( i < nP ){
                                theta += dTheta;
                            } else {
                                theta = - M_PI / 2.0;
                            }
                        }
                    }
                    break;}
                }
                cl++;
            }
        }
        for( int i=0; i < 3; i++){
            if( !diagramListPr->at(i)->isClosed() && diagramListPr->at(i)->size() > 0 ){
                *(diagramListPr->at(i)) << (diagramListPr->at(i)->at(0));
            }
        }

        QList<QPolygonF *> * diagramListF = new QList<QPolygonF *>;
        diagramListF->append( new QPolygonF );
        diagramListF->append( new QPolygonF );
        diagramListF->append( new QPolygonF );
        diagramListF->append( new QPolygonF );
        if( classFMin > 0 && classFMax > 0){
            if( classFMax == classFMin ){
                *(diagramListF->at(classFMax-1)) << QPointF( 0.0, M_PI/2.0 ) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( M_PI/2.0, - M_PI/2.0 ) << QPointF( 0.0, - M_PI/2.0 ) << QPointF( 0.0, M_PI/2.0 );
            } else {
                *(diagramListF->at(classFMin-1)) = *(diagramListPr->at(classFMin-1));
                for( int cl = classFMin; cl < (classFMax-1); cl++ ){
                    if( diagramListPr->at(cl)->size() > 0 ){
                        *(diagramListF->at(cl)) =  diagramListPr->at(cl)->subtracted( *(diagramListPr->at(cl-1)));
                    }
                }
                QPolygonF rect;
                rect << QPointF( 0.0, M_PI/2.0 ) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( M_PI/2.0, - M_PI/2.0 ) << QPointF( 0.0, - M_PI/2.0 );
                *(diagramListF->at(classFMax-1)) = rect.subtracted( *(diagramListPr->at(classFMax-2)) );
            }
        }

        int classMin = classWMin;
        if( classFMin < classMin )
            classMin = classFMin;
        int classMax = classWMax;
        if( classFMax > classMax )
            classMax = classFMax;

        if( classMax > 0 && classMin > 0 ){
            if( classMax == classMin ){
                *(clDiagram->at(classMax-1)) << QPointF( 0.0, M_PI/2.0 ) << QPointF( M_PI/2.0, M_PI/2.0 ) << QPointF( M_PI/2.0, - M_PI/2.0 ) << QPointF( 0.0, - M_PI/2.0 ) << QPointF( 0.0, M_PI/2.0 );
            } else {
                for( int i=classMax; i >= classMin; i--){
                    *(clDiagram->at(i-1)) = diagramListW->at(i-1)->united( (*diagramListF->at(i-1)) );
                    for( int j=classMax; j>i; j--){
                        *(clDiagram->at(i-1)) = clDiagram->at(i-1)->subtracted( *(clDiagram->at(j-1)) );
                    }
                }
            }
        }

        // clears memory
        qDeleteAll( diagramListPr->begin(), diagramListPr->end());
        delete diagramListPr;
        qDeleteAll( diagramListF->begin(), diagramListF->end());
        delete diagramListF;
        qDeleteAll( diagramListW->begin(), diagramListW->end());
        delete diagramListW;
    }
}

void SectionSteelI::updateSectionSteelProfile() {
    updateSectionPoints();
    updateQGraphics();

    if( !m_signalsSuspended ){
        emit sectionSteelProfileChanged();
    }
}

void SectionSteelI::updateQGraphics(){
    if( m_d->qGraphicsItem ){
        QPainterPath path;
        if( m_d->r->value() > 0.0 ){
            {
                QPolygonF upperF;
                upperF << QPointF( -(m_d->tw->value()/2.0 +m_d->r->value()), (m_d->h->value()/2.0 - m_d->tf->value()) );
                upperF << QPointF( -m_d->b->value()/2.0, (m_d->h->value()/2.0 - m_d->tf->value()) );
                upperF << QPointF( -m_d->b->value()/2.0, m_d->h->value()/2.0 );
                upperF << QPointF( m_d->b->value()/2.0, m_d->h->value()/2.0 );
                upperF << QPointF( m_d->b->value()/2.0, (m_d->h->value()/2.0 - m_d->tf->value()) );
                upperF << QPointF( m_d->tw->value()/2.0 + m_d->r->value(), (m_d->h->value()/2.0 - m_d->tf->value()) );
                path.addPolygon( upperF );
            }
            path.arcTo( m_d->tw->value()/2.0, (m_d->h->value()/2.0 - m_d->tf->value()), 2.0 * m_d->r->value(), - 2.0 * m_d->r->value(), 90.0, 90.0 );
            path.arcTo( m_d->tw->value()/2.0, -(m_d->h->value()/2.0 - m_d->tf->value()), 2.0 * m_d->r->value(), 2.0 * m_d->r->value(), 180.0, -90.0 );
            {
                QPolygonF lowerF;
                lowerF << QPointF( m_d->tw->value()/2.0 +m_d->r->value(), -(m_d->h->value()/2.0 - m_d->tf->value()) );
                lowerF << QPointF( m_d->b->value()/2.0, -(m_d->h->value()/2.0 - m_d->tf->value()) );
                lowerF << QPointF( m_d->b->value()/2.0, -m_d->h->value()/2.0 );
                lowerF << QPointF( -m_d->b->value()/2.0, -m_d->h->value()/2.0 );
                lowerF << QPointF( -m_d->b->value()/2.0, -(m_d->h->value()/2.0 - m_d->tf->value()) );
                lowerF << QPointF( -(m_d->tw->value()/2.0 +m_d->r->value()), -(m_d->h->value()/2.0 - m_d->tf->value()) );
                path.addPolygon( lowerF );
            }
            path.arcTo( -m_d->tw->value()/2.0, -(m_d->h->value()/2.0 - m_d->tf->value()), -2.0 * m_d->r->value(), 2.0 * m_d->r->value(), 90.0, 90.0 );
            path.arcTo( -m_d->tw->value()/2.0, (m_d->h->value()/2.0 - m_d->tf->value()), -2.0 * m_d->r->value(), -2.0 * m_d->r->value(), 180.0, -90.0 );
        } else if( m_d->r->value() == 0.0 ) {
            QPolygonF pol;
            pol << QPointF( -m_d->tw->value()/2.0, (m_d->h->value()/2.0 - m_d->tf->value()) );
            pol << QPointF( -m_d->b->value()/2.0, (m_d->h->value()/2.0 - m_d->tf->value()) );
            pol << QPointF( -m_d->b->value()/2.0, m_d->h->value()/2.0 );
            pol << QPointF( m_d->b->value()/2.0, m_d->h->value()/2.0 );
            pol << QPointF( m_d->b->value()/2.0, (m_d->h->value()/2.0 - m_d->tf->value()) );
            pol << QPointF( m_d->tw->value()/2.0, (m_d->h->value()/2.0 - m_d->tf->value()) );
            pol << QPointF( m_d->tw->value()/2.0, -(m_d->h->value()/2.0 - m_d->tf->value()) );
            pol << QPointF( m_d->b->value()/2.0, -(m_d->h->value()/2.0 - m_d->tf->value()) );
            pol << QPointF( m_d->b->value()/2.0, -m_d->h->value()/2.0 );
            pol << QPointF( -m_d->b->value()/2.0, -m_d->h->value()/2.0 );
            pol << QPointF( -m_d->b->value()/2.0, -(m_d->h->value()/2.0 - m_d->tf->value()) );
            pol << QPointF( -m_d->tw->value()/2.0, -(m_d->h->value()/2.0 - m_d->tf->value()) );
            pol << QPointF( -m_d->tw->value()/2.0, (m_d->h->value()/2.0 - m_d->tf->value()) );
            path.addPolygon( pol );
        }
        m_d->qGraphicsItem->setPath( path );
    }
}

void SectionSteelI::updateSectionPoints(){
    if( m_sectionPoints == NULL ){
        m_sectionPoints = new Point2DModel();
    }

    int arc_prec = 0;
    if( m_d->r->valueNormal() > 0.0 ){
        arc_prec = ARC_PREC;
    }
    m_sectionPoints->resize(2 * 6 + 4 * arc_prec);

    double a, y0, z0;
    double da = M_PI / ( 2.0 * ARC_PREC );

    double h = m_d->h->valueNormal();
    double b = m_d->b->valueNormal();
    double tw = m_d->tw->valueNormal();
    double tf = m_d->tf->valueNormal();
    double r = m_d->r->valueNormal();
    m_sectionPoints->setYZNormal( 0, tw/2.0 + r, h/2.0 - tf );
    m_sectionPoints->setYZNormal( 1, b/2.0, h/2.0 - tf );
    m_sectionPoints->setYZNormal( 2, b/2.0, h/2.0 );
    m_sectionPoints->setYZNormal( 3, -b/2.0, h/2.0 );
    m_sectionPoints->setYZNormal( 4, -b/2.0, h/2.0 - tf );

    a = M_PI/2.0;
    y0 = - ( tw/2.0 + r);
    z0 = (h/2.0 - tf - r);
    for( int i=0; i < arc_prec; i++ ){
        m_sectionPoints->setYZNormal( 5+i, y0 + m_d->r->valueNormal() * cos(a), z0 + m_d->r->valueNormal() * sin(a) );
        a -= da;
    }
    m_sectionPoints->setYZNormal( 5+arc_prec, - (tw/2.0), (h/2.0 - tf - r) );
    z0 = -z0;
    for( int i=0; i < arc_prec; i++ ){
        m_sectionPoints->setYZNormal( 6+arc_prec+i, y0 + m_d->r->valueNormal() * cos(a), z0 + m_d->r->valueNormal() * sin(a) );
        a -= da;
    }
    m_sectionPoints->setYZNormal( 6 + 2 * arc_prec, -(tw/2.0 +r), -(h/2.0 - tf) );
    m_sectionPoints->setYZNormal( 6 + 2 * arc_prec + 1, -(b/2.0), -(h/2.0 - tf) );
    m_sectionPoints->setYZNormal( 6 + 2 * arc_prec + 2, -(b/2.0), -(h/2.0) );
    m_sectionPoints->setYZNormal( 6 + 2 * arc_prec + 3, b/2.0, -(h/2.0) );
    m_sectionPoints->setYZNormal( 6 + 2 * arc_prec + 4, b/2.0, -(h/2.0 - tf) );
    y0 = -y0;
    for( int i=0; i < arc_prec; i++ ){
        m_sectionPoints->setYZNormal( 6+5+2*arc_prec+i, y0 + r * cos(a), z0 + r * sin(a) );
        a -= da;
    }
    m_sectionPoints->setYZNormal( 6+5+3*arc_prec, (tw/2.0), -(h/2.0 - tf - r ) );
    z0 = -z0;
    for( int i=0; i < arc_prec; i++ ){
        m_sectionPoints->setYZNormal( 2*6+3*arc_prec+i, y0 + r * cos(a), z0 + r * sin(a) );
        a -= da;
    }
}

void SectionSteelI::point( int i, double *y, double *z ){
    if( i < m_sectionPoints->nPoints() ){
        *y = m_sectionPoints->yNormal( i );
        *z = m_sectionPoints->zNormal( i );
    }
}

int SectionSteelI::nPoints(){
    return m_sectionPoints->nPoints();
}

