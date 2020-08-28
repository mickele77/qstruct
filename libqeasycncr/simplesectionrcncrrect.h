#ifndef SIMPLESECTIONRCNCRRECT_H
#define SIMPLESECTIONRCNCRRECT_H

class SteelAModel;
class SteelA;
class SLSSteelSigmaModel;
class DoublePlus;

class SimpleSectionRCncrRectPrivate;
class SteelFEpsModel;

#include "easycncr_export.h"
#include "unitmeasure.h"

#include "varpluscontainerplus.h"

class EXPORT_EASYCNCR_LIB_OPT SimpleSectionRCncrRect : public VarPlusContainerPlus {
    Q_OBJECT
public:
    enum ModelSteelFEps {
        horizontalTopBranch,
        inclinedTopBranch
    };
    explicit SimpleSectionRCncrRect(UnitMeasure * ump, const QString & n, QObject * parent = nullptr);

    /** Base */
    DoublePlus * b;
    /** Altezza */
    DoublePlus * h;
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

    SteelAModel * steelAModel();

    QVector<QPointF> NEpsConfigurations1( int nP = 600 );
    QVector<QPointF> NEpsConfigurations23(int nP = 600 );
    QVector<QPointF> NEpsConfigurations4(int nP = 600 );
    void MRd(DoublePlus *ret, DoublePlus * NEd, DoublePlus *x, bool inverted = false);
    QVector<QPointF> MChiDiagram(DoublePlus *NEdPlus, int nP=600, double prec=1.0e-4, int maxIter=1000);
    QVector<QPointF> NRdMRd( int nP = 100 );

    /** Calcola la torsione resistente della sezione con armatura a torsione.
     * Calcola theta in modo da massimizzare il valore della resistenza torsionale
     * @param TRd variabile su cui scrivere il valore della resistenza torsionale massima
     * */
    void ULSTMax(DoublePlus *TRd, DoublePlus *TRdc, DoublePlus *TRds, DoublePlus *Asw, DoublePlus *s, DoublePlus *theta);

    /** Calcola la torsione resistente della sezione con armatura a torsione
     * Calcola theta in modo da massimizzare il valore della resistenza torsionale
     * @return la resistenza torsionale massima
     * */
    double ULSTMaxNormal(double *TRdc, double *TRds, double Asw, double s, double *theta);

    /** Calcola la torsione resistente della sezione con armatura a torsione
     * @param TRdc resistenza a torsione cls
     * @param TRds resistenza a torsione lato acciaio
     * @return resistenza a torsione effettiva della sezione
     *  */
    void ULST( DoublePlus * T, DoublePlus * TRdc, DoublePlus * TRds,
               DoublePlus * Asw, DoublePlus * s, DoublePlus * theta );

    /** Calcola la torsione resistente della sezione con armatura a torsione
     * @param TRdmax resistenza a torsione cls
     * @param TRds resistenza a torsione lato acciaio
     * @return resistenza a torsione effettiva della sezione
     *  */
    double ULSTNormal(double *TRdc, double *TRds, double Asw, double s, double theta );

    /** calcola il taglio resistente della sezione senza staffe
     * @param dw altezza utile
     * @param bw larghezza sezione
     * @param Asl armatura longitudinale  */
    double ULSVNoSteelNormal(double bw, double dw, double Asl , double scp = 0.0);
    void ULSVNoSteel(DoublePlus *V, DoublePlus *bw, DoublePlus *dw, DoublePlus *Asl, DoublePlus *scp);

    /** calcola il taglio resistente della sezione con staffe
     * @param VRdmax resistenza a taglio cls
     * @param VRds resistenza a taglio lato acciaio
     * @return resistenza a taglio effettiva della sezione
     *  */
    double ULSVSteelNormal( double *VRdmax, double *VRds, double bw, double dw, double Asw, double s, double theta, double alpha);
    void ULSVSteel( DoublePlus * V, DoublePlus * VMax, DoublePlus * Vs,
                    DoublePlus * bw, DoublePlus * dw, DoublePlus * Asw, DoublePlus * s, DoublePlus * theta, DoublePlus * alpha);

    /** Calcola il taglio resistente della sezione con armatura a taglio
     * Fa variare theta (inclinazione bielle cls) di modo da massimizzare il taglio
     * @param VRdmax resistenza a taglio cls
     * @param VRds resistenza a taglio lato acciaio
     * @return resistenza a taglio effettiva della sezione
     *  */
    double ULSVSteelMaxNormal( double *VRdmax, double *VRds,
                               double bw, double dw, double Asw, double s, double * theta, double alpha);

    void ULSVSteelMax( DoublePlus * V, DoublePlus * VMax, DoublePlus * Vs,
                       DoublePlus * bw, DoublePlus * dw, DoublePlus * Asw, DoublePlus * s, DoublePlus * theta, DoublePlus * alpha);

    /** calcola la sezione sotto ipotesi elastico lineari
     * @param alphaE coefficiente di omogeneizzazione
     *  */
    void SLSNMNormal( double N, double M, double alphaE, double sigmaS[], double * sigmaCUp, double * sigmaCDown, double * x);

    void SLSNM(DoublePlus *N, DoublePlus *M, DoublePlus *alphaE, SLSSteelSigmaModel *sigmaSModel, DoublePlus *sigmaCUp, DoublePlus *sigmaCDown, DoublePlus *x);

signals:
    void sectionChanged();

private:
    SimpleSectionRCncrRectPrivate * m_d;

    void initVar();

    double beta1Normal(double epsC );
    double beta2Normal(double epsC );
    double fCncrNormal(double eC);
    double fSteelNormal(double eS);

    double NNormal(double eS, double eC, bool inverted = false );
    double MNormal(double eS, double eC, bool inverted = false );

    double MRdNormal(double NEd, double *x, bool inverted = false, double prec=1.0e-7, int maxIter=1000);

    double calcSigmaNormal(double *sigmaCUp, double *sigmaCDown, double sigmaS[], double N, double M, double x, double alphaE, bool invert);

    void setFcd( bool emitAuto = true );
    void setEpsC2( bool emitAuto = true  );
    void setEpsCU2( bool emitAuto = true  );
    void setNC( bool emitAuto = true  );

    void setEpsSUd( bool emitAuto = true );
    void setFSyd( bool emitAuto = true );
    void setFSud( bool emitAuto = true );

    void setNRdMin( bool emitAuto = true );
    void setNRdMax( bool emitAuto = true );

private slots:
    void setDMax( double newVal );
    void setDMin( double newVal );
};

#endif // SIMPLESECTIONRCNCRRECT_H
