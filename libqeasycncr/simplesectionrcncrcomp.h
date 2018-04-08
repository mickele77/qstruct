#ifndef SIMPLESECTIONRCNCRCOMP_H
#define SIMPLESECTIONRCNCRCOMP_H

class SteelAModelPrivate;
class SteelA;
class DoublePlus;

class CncrRectModel;
class SteelAModel;
class SteelFEpsModel;
class SLSSteelSigmaModel;
class SimpleSectionRCncrCompPrivate;

#include "easycncr_export.h"
#include "tablemodelplus.h"
#include "unitmeasure.h"

#include "varpluscontainerplus.h"

class EXPORT_EASYCNCR_LIB_OPT SimpleSectionRCncrComp : public VarPlusContainerPlus {
    Q_OBJECT
public:
    enum ModelSteelFEps {
        horizontalTopBranch,
        inclinedTopBranch
    };
    explicit SimpleSectionRCncrComp(UnitMeasure * ump, const QString & n, QObject * parent = 0);

    /** Altezza totale */
    DoublePlus * hTot;
    /** Altezza baricentro rispetto a bordo superiore */
    DoublePlus * hG;
    /** Altezza utile */
    DoublePlus * dMax;
    /** Altezza utile */
    DoublePlus * dMin;

    /** fck */
    DoublePlus * fck;
    /** alphaCC */
    DoublePlus * alphaCC;
    /** gammaC */
    DoublePlus * gammaC;
    /** fcd */
    DoublePlus * fcd;
    /** epsC2 */
    DoublePlus * epsC2;
    /** epsCU2 */
    DoublePlus * epsCU2;
    /** nC */
    DoublePlus * nC;

    /** epsUk */
    DoublePlus * Es;
    /** epsUk */
    DoublePlus * epsSUk;
    /** epsUd */
    DoublePlus * epsSUd;
    /** fyk */
    DoublePlus * fSyk;
    /** gammaS */
    DoublePlus * gammaS;
    /** fyd */
    DoublePlus * fSyd;
    /** kS */
    DoublePlus * kS;
    /** fSud */
    DoublePlus * fSud;
    /** Modello sigma/epsilon acciaio */
    SteelFEpsModel * steelFEpsModel;
    /** NRdMin */
    DoublePlus * NRdMin;
    /** NRdMin */
    DoublePlus * NRdMax;

    CncrRectModel * cncrRectModel();
    SteelAModel * steelAModel();

    QVector<QPointF> NEpsConfigurations1( int nP = 600 );
    QVector<QPointF> NEpsConfigurations23(int nP = 600 );
    QVector<QPointF> NEpsConfigurations4(int nP = 600 );
    void MRd(DoublePlus *ret, DoublePlus * NEd, DoublePlus *x, bool inverted = false);
    QVector<QPointF> MChiDiagram(DoublePlus *NEdPlus, int nP=600, double prec=1.0e-4, int maxIter=1000);
    QVector<QPointF> NRdMRd( int nP = 1000 );

    /** calcola la sezione sotto ipotesi elastico lineari
     * @param alphaE coefficiente di omogeneizzazione
     *  */
    void SLSNMNormal( double N, double M, double alphaE, double sigmaS[], double * sigmaCUp, double * sigmaCDown, double * x);

    void SLSNM(DoublePlus *N, DoublePlus *M, DoublePlus *alphaE, SLSSteelSigmaModel *sigmaSModel, DoublePlus *sigmaCUp, DoublePlus *sigmaCDown, DoublePlus *x);


    /** calcola il taglio resistente della sezione senza staffe
     * @param dw altezza utile
     * @param bw larghezza sezione
     * @param Asl armatura longitudinale  */
    double ULSVNoSteelNormal(double bw, double dw, double Asl , double scp = 0.0);
    void ULSVNoSteel(DoublePlus *V, DoublePlus *bw, DoublePlus *dw, DoublePlus *Asl, DoublePlus *scp);

signals:
    void sectionChanged();

private slots:
    void setFcd();
    void setEpsC2();
    void setEpsCU2();
    void setNC();

    void setEpsSUd();
    void setFSyd();
    void setFSud();

    void setNRdMin();
    void setNRdMax();

private:
    SimpleSectionRCncrCompPrivate * m_d;

    void initVar();

    double beta1Normal(double epsC );
    double beta2Normal(double epsC );
    double fCncrNormal(double eC);
    double fSteelNormal(double eS);

    double NNormal(double eS, double eC, bool inverted = false );
    double MNormal(double eS, double eC, bool inverted = false );

    double MRdNormal(double NEd, double *x, bool inverted = false, double prec=1.0e-7, int maxIter=1000);

    double calcSigmaNormal(double *sigmaCUp, double *sigmaCDown, double sigmaS[], double N, double M, int sectI, double xi, double alphaE, bool invert);
};

#endif // SIMPLESECTIONRCNCRCOMP_H
