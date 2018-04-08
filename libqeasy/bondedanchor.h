#ifndef BONDEDANCHOR_H
#define BONDEDANCHOR_H

class BAnchorConcrete;
class BAnchorInternalReinforcement;
class BAnchorEdgeReinforcement;
class DoublePlus;
class QStringPlus;
class UnitMeasure;

#include "easy_export.h"
#include "varpluscontainerplus.h"
#include <QObject>

class EXPORT_EASY_LIB_OPT BondedAnchor: public VarPlusContainerPlus {
    Q_OBJECT
public:
    enum InternalReinforcement {
        Dense,
        Rare
    };
    enum EdgeReinforcement {
        EdgeNotReinforced,
        EdgeReinforced,
        EdgeReinforcedStirrups
    };
    enum Concrete {
        Cracked,
        NotCracked
    };

    BondedAnchor( UnitMeasure * ump );
    
    DoublePlus * d;
    DoublePlus * d0;

    UIntPlus * nX;
    UIntPlus * nY;
    UIntPlus * nTot;

    DoublePlus * cXMinus;
    DoublePlus * sX;
    DoublePlus * cXPlus;
    DoublePlus * cYMinus;
    DoublePlus * sY;
    DoublePlus * cYPlus;

    DoublePlus * As;

    DoublePlus * fyks;
    DoublePlus * fuks;
    DoublePlus * fckCube;

    DoublePlus * h;
    DoublePlus * hEff;

    BAnchorConcrete * concreteCrack;
    BAnchorInternalReinforcement * internalReinforcement;
    BAnchorEdgeReinforcement * edgeReinforcement;

    // dati certificazione ETAG
    DoublePlus * hEffMin;
    DoublePlus * hEffMax;
    DoublePlus * hMin;
    DoublePlus * cMin;
    DoublePlus * sMin;
    DoublePlus * tauRkCr;
    DoublePlus * tauRkUCr;
    DoublePlus * tauRkUCr25;
    DoublePlus * ratioCCrSpOvEff;
    DoublePlus * ratioSCrSpOvEff;

    // coefficienti sicurezza
    DoublePlus * gammaC;
    DoublePlus * gamma2;

    DoublePlus * gammaMsN;
    DoublePlus * gammaMpN;
    DoublePlus * gammaMcN;
    DoublePlus * gammaMSpN;

    // valori di resistenza carateristici e di progetto
    DoublePlus * NRks;
    DoublePlus * NhRds;

    // rottura combinata cono cls e pull-out
    DoublePlus * cCrNp;
    DoublePlus * sCrNp;
    DoublePlus * ApN;
    DoublePlus * A0pN;
    DoublePlus * psiSNp;
    DoublePlus * psiGNp;
    DoublePlus * psiEcNpX;
    DoublePlus * psiEcNpY;
    DoublePlus * psiReNp;
    DoublePlus * N0Rkp;
    DoublePlus * NRkp;
    DoublePlus * NgRdp;

    // rottura cono cls
    DoublePlus * cCrN;
    DoublePlus * sCrN;
    DoublePlus * AcN;
    DoublePlus * A0cN;
    DoublePlus * psiSN;
    DoublePlus * psiEcNX;
    DoublePlus * psiEcNY;
    DoublePlus * psiReN;
    DoublePlus * N0Rkc;
    DoublePlus * NRkc;
    DoublePlus * NgRdc;

    // splitting
    DoublePlus * cCrSp;
    DoublePlus * sCrSp;
    DoublePlus * psiHSp;
    DoublePlus * NRkSp;
    DoublePlus * NgRdSp;

    // tagio acciaio
    DoublePlus * M0Rks;
    DoublePlus * MRks;
    DoublePlus * alphaM;
    DoublePlus * VRks;
    DoublePlus * gammaMsV;
    DoublePlus * VhRds;

    // pry-out
    DoublePlus * kCp;
    DoublePlus * VRkCp;
    DoublePlus * gammaMcV;
    DoublePlus * VgRdCp;

    DoublePlus * k1C;

    DoublePlus * A0CVXPlus;
    DoublePlus * ACVXPlus;
    DoublePlus * V0RkCXPlus;
    DoublePlus * VRkCXPlus;
    DoublePlus * VgRdCXPlus;

    DoublePlus * A0CVXMinus;
    DoublePlus * ACVXMinus;
    DoublePlus * V0RkCXMinus;
    DoublePlus * VRkCXMinus;
    DoublePlus * VgRdCXMinus;

    DoublePlus * A0CVYPlus;
    DoublePlus * ACVYPlus;
    DoublePlus * V0RkCYPlus;
    DoublePlus * VRkCYPlus;
    DoublePlus * VgRdCYPlus;

    DoublePlus * A0CVYMinus;
    DoublePlus * ACVYMinus;
    DoublePlus * V0RkCYMinus;
    DoublePlus * VRkCYMinus;
    DoublePlus * VgRdCYMinus;

    DoublePlus * lVEd;
    DoublePlus * VEdX;
    DoublePlus * VEdY;
    DoublePlus * TEd;

    DoublePlus * eNEdX;
    DoublePlus * eNEdY;
    DoublePlus * NEd;

private:
    void initVar();

    void setNTot( bool emitAuto = true );

    void setAs(bool emitAuto = true);

    void setNRks(bool emitAuto = true);
    void setGammaMsN(bool emitAuto = true);
    void setNhRds(bool emitAuto);

    // moltiplicatore di sCrNp, cCrNp e hEff
    // 5.2.2.3g (special cases)
    double moltNP();
    void setApN(bool emitAuto = true);
    void setA0pN(bool emitAuto = true);
    void setCCrNp(bool emitAuto);
    void setSCrNp(bool emitAuto);
    void setPsiSNp(bool emitAuto = true);
    void setPsiGNp(bool emitAuto = true);
    void setPsiEcNpX(bool emitAuto = true);
    void setPsiEcNpY(bool emitAuto = true);
    void setPsiReNp(bool emitAuto = true);
    void setN0Rkp(bool emitAuto = true);
    void setNRkp(bool emitAuto = true);
    void setGammaMpN(bool emitAuto = true);
    void setNgRdp(bool emitAuto = true);

    // moltiplicatore di sCrN, cCrN e hEff
    // 5.2.2.4f (special cases)
    double moltN();
    void setCCrN(bool emitAuto);
    void setSCrN(bool emitAuto);
    void setAcN(bool emitAuto = true);
    void setA0cN(bool emitAuto = true);
    void setPsiSN(bool emitAuto = true);
    void setPsiEcNX(bool emitAuto = true);
    void setPsiEcNY(bool emitAuto = true);
    void setPsiReN(bool emitAuto = true);
    void setN0Rkc(bool emitAuto = true);
    void setNRkc(bool emitAuto = true);
    void setGammaMcN(bool emitAuto = true);
    void setNgRdc(bool emitAuto = true);

    void setCCrSp(bool emitAuto = true);
    void setSCrSp(bool emitAuto = true);
    void setPsiHSp(bool emitAuto = true);
    void setNRkSp(bool emitAuto = true);
    void setGammaMSpN(bool emitAuto = true);
    void setNgRdSp(bool emitAuto = true);

    void setMRks(bool emitAuto = true);
    void setVRks(bool emitAuto = true);
    void setGammaMsV(bool emitAuto = true);
    void setVhRds(bool emitAuto);

    void setKCp(bool emitAuto = true);
    void setVRkCp(bool emitAuto = true);
    void setGammaMcV(bool emitAuto = true);
    void setVgRdCp(bool emitAuto = true);

    void setK1C(bool emitAuto = true);

    void setA0CVXPlus(bool emitAuto = true);
    void setACVXPlus(bool emitAuto = true);
    void setV0RkCXPlus(bool emitAuto = true);
    void setVRkCXPlus(bool emitAuto = true);
    void setVgRdCXPlus(bool emitAuto = true);

    void setA0CVXMinus(bool emitAuto = true);
    void setACVXMinus(bool emitAuto = true);
    void setV0RkCXMinus(bool emitAuto = true);
    void setVRkCXMinus(bool emitAuto = true);
    void setVgRdCXMinus(bool emitAuto = true);

    void setA0CVYPlus(bool emitAuto = true);
    void setACVYPlus(bool emitAuto = true);
    void setV0RkCYPlus(bool emitAuto = true);
    void setVRkCYPlus(bool emitAuto = true);
    void setVgRdCYPlus(bool emitAuto = true);

    void setA0CVYMinus(bool emitAuto = true);
    void setACVYMinus(bool emitAuto = true);
    void setV0RkCYMinus(bool emitAuto = true);
    void setVRkCYMinus(bool emitAuto = true);
    void setVgRdCYMinus(bool emitAuto = true);

    // Special cases par. 5.2.3.4 lett. i
    double c1VEffVal(unsigned int nVal, double c1ValInput, double c21ValInput, double s2ValInput, double c22ValInput );
    double A0CVVal(unsigned int nVal, double c1ValInput, double c21ValInput, double s2ValInput, double c22ValInput);
    double ACVVal(unsigned int nVal, double c1ValInput, double c21ValInput, double s2ValInput, double c22ValInput);
    double V0RkCVal(unsigned int nVal, double c1ValInput, double c21ValInput, double s2ValInput, double c22ValInput);
    double VRkCVal(unsigned int nVal, double V0RkCVal, double ACVVal, double A0CVVal,
                   double c1Val, double c21ValInput, double s2ValInput, double c22ValInput );

    // calcolo valore di progetto da valore caratteristico e coeff. sicurezza
    double dVal( double kVal, double gamma );

};

#include "enumplus.h"

class BAnchorConcretePrivate;

class EXPORT_EASY_LIB_OPT BAnchorConcrete : public EnumPlus
{
    Q_OBJECT
public:
    BAnchorConcrete( BondedAnchor::Concrete vv,
                     const QString & nn,
                     bool ro = false );
    BAnchorConcrete( BAnchorConcrete & val );


    BAnchorConcrete & operator=( BAnchorConcrete &);
    VarPlus & operator=( VarPlus &);

    QString typeVar();

    BondedAnchor::Concrete value();
    BondedAnchor::Concrete valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( BondedAnchor::Concrete, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( BondedAnchor::Concrete, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    BAnchorConcretePrivate * m_d;

    ~BAnchorConcrete();
};

class BAnchorInternalReinforcementPrivate;

class EXPORT_EASY_LIB_OPT BAnchorInternalReinforcement : public EnumPlus {
    Q_OBJECT
public:
    BAnchorInternalReinforcement( BondedAnchor::InternalReinforcement vv,
                                  const QString & nn,
                                  bool ro = false );
    BAnchorInternalReinforcement( BAnchorInternalReinforcement & val );


    BAnchorInternalReinforcement & operator=( BAnchorInternalReinforcement &);
    VarPlus & operator=( VarPlus &);

    QString typeVar();

    BondedAnchor::InternalReinforcement value();
    BondedAnchor::InternalReinforcement valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( BondedAnchor::InternalReinforcement, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( BondedAnchor::InternalReinforcement, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    BAnchorInternalReinforcementPrivate * m_d;

    ~BAnchorInternalReinforcement();
};

class BAnchorEdgeReinforcementPrivate;

class EXPORT_EASY_LIB_OPT BAnchorEdgeReinforcement : public EnumPlus {
    Q_OBJECT
public:
    BAnchorEdgeReinforcement( BondedAnchor::EdgeReinforcement vv,
                              const QString & nn,
                              bool ro = false );
    BAnchorEdgeReinforcement( BAnchorEdgeReinforcement & val );


    BAnchorEdgeReinforcement & operator=( BAnchorEdgeReinforcement &);
    VarPlus & operator=( VarPlus &);

    QString typeVar();

    BondedAnchor::EdgeReinforcement value();
    BondedAnchor::EdgeReinforcement valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( BondedAnchor::EdgeReinforcement, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( BondedAnchor::EdgeReinforcement, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    BAnchorEdgeReinforcementPrivate * m_d;

    ~BAnchorEdgeReinforcement();
};

#endif // BONDEDANCHOR_H
