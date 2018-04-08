#ifndef SECTIONTHINCONTSTEEL_H
#define SECTIONTHINCONTSTEEL_H

class Steel;
class QVector2D;

class SectionThinContSteelPrivate;

#include "sectionthincont.h"

class EXPORT_SECTION_LIB_OPT SectionThinContSteel: public SectionThinCont {
    Q_OBJECT
public:
    SectionThinContSteel( UnitMeasure * ump,
                          Steel * st,
                          const QString &nn,
                          QObject * parent = 0 );
    SectionThinContSteel( UnitMeasure *ump,
                          const QXmlStreamAttributes &attrs,
                          MaterialModel *matModel,
                          SectionLoadPhaseModel * lModel,
                          QObject *parent = 0 );
    ~SectionThinContSteel();

    // Debug
    // QGraphicsItem *qGraphicsItem();

    /** Ridefinizione del metdo virtuale. Questa classe è valida solo nel caso
    * di sezione in acciaio (Steel). In caso contrario non reimposta la sezione.
    */
    void setMaterial(Material *mat);

    /**
    * @return restituisce il materiale della sezione () vedi nota metodo precedente) */
    Steel *steel();

    /** Area sezione efficace nel caso di compressione semplice */
    DoublePlus * ANMinusEff;
    /** Baricentro sezione efficace nel caso di compressione semplice */
    Point2DPlus * eGNMinusEff;

    /** Momento centrale di inerzia della sezione efficace nel caso di flessione positiva asse y-y */
    DoublePlus * IMyyPlusEff;
    /** Modulo elastico di resistenza flessionale della sezione efficace nel caso di flessione positiva asse y-y */
    DoublePlus * WMyyPlusEff;
    /** Rotazione assi centrali di inerzia della sezione efficace nel caso di flessione positiva asse y-y */
    DoublePlus * thetaCMyyPlusEff;

    /** Momento principale di inerzia della sezione efficace nel caso di flessione negativa asse y-y */
    DoublePlus * IMyyMinusEff;
    /** Modulo elastico di resistenza flessionale della sezione efficace nel caso di flessione negativa asse y-y */
    DoublePlus * WMyyMinusEff;
    /** Rotazione assi centrali di inerzia della sezione efficace nel caso di flessione negativa asse y-y */
    DoublePlus * thetaCMyyMinusEff;

    /** Momento principale di inerzia della sezione efficace nel caso di flessione positiva asse z-z */
    DoublePlus * IMzzPlusEff;
    /** Modulo elastico di resistenza flessionale della sezione efficace nel caso di flessione positiva asse z-z */
    DoublePlus * WMzzPlusEff;
    /** Rotazione assi centrali di inerzia della sezione efficace nel caso di flessione positiva asse z-z */
    DoublePlus * thetaCMzzPlusEff;

    /** Momento principale di inerzia della sezione efficace nel caso di flessione negativa asse z-z */
    DoublePlus * IMzzMinusEff;
    /** Modulo elastico di resistenza flessionale della sezione efficace nel caso di flessione negativa asse z-z */
    DoublePlus * WMzzMinusEff;
    /** Rotazione assi centrali di inerzia della sezione efficace nel caso di flessione negativa asse z-z */
    DoublePlus * thetaCMzzMinusEff;

    /** Modulo elastico di resistenza flessionale minimo della sezione efficace nel caso di flessione asse y-y */
    DoublePlus * WMyyMinEff;
    /** Rotazione assi centrali di inerzia della sezione efficace nel caso di flessione asse y-y */
    DoublePlus * thetaCMyyMinEff;

    /** Modulo elastico di resistenza flessionale minimo della sezione efficace nel caso di flessione asse z-z */
    DoublePlus * WMzzMinEff;
    /** Rotazione assi centrali di inerzia della sezione efficace nel caso di flessione asse z-z */
    DoublePlus * thetaCMzzMinEff;

private:
    SectionThinContSteelPrivate * m_ddd;

    /** Inizializza */
    void initVar();

    /** Calcola la lerghezza collaborante di un segmento interno
    * @param s1 la tensione su uno degli estremi del tratto di sezione
    * @param s2 la tensione su uno degli estremi del tratto di sezione */
    void bEffInternal(double *bEff1, double *bEff2, double E, double nu, double fy, double b, double t, double s1, double s2);

    /** Calcola la larghezza collaborante di un segmento esterno
    * @param bEff1 larghezza collaborante tratto lato bordo libero
    * @param bEff2 larghezza collaborante tratto lato bordo vincolato
    * #param b larghezza complessiva tratto
    * @param s1 la tensione sul bordo libero
    * @param s2 la tensione sul bordo vincolato */
    void bEffOutstand( double *bEff1, double *bEff2,
                       double E, double nu, double fy,
                       double b, double t,
                       double s1, double s2 );

    /** Punto medio dell'arco i-esimo */
    QVector2D arcMidPoint(int i);

    /** Ricalcola la sezione efficace sect, soggetta alle caratteristiche (N, My, Mz)
     * @param N sforzo normale applicato alla sezione
     * @param My momento y applicato alla sezione
     * @param Mz momento z applicato alla sezione
     * @param A area della sezione calcolata
     * @param Iyy momento di inerzia della sezione calcolata
     * @param Izz momento di inerzia della sezione calcolata
     * @param alpha rotazione del sistema di riferimento centrale sella sezione calcolata
     * @param y0 ascissa del baricentro della sezione calcolata
     * @param z0 ordinata del baricentro della sezione calcolata
     * */
    void updateSectionEff(SectionThin *sect,
                           double l, double my, double mz);

    /** Ricalcola i modelli di tutte le sezioni efficaci */
    void updateSectionEff();

    void updateSectionModels();

    void setTNormal();

    /** Ricalcola le coordinate di (yp, zp) a seguito di una rototraslazione del sistema di riferimento
     * @param alpha rotazione del nuovo sistema di riferimento (positivo se antiorario)
     * @param y0 prima coordinata del centro del nuovo sistema di riferimento
     * @param z0 seconda coordinata del centro del nuovo sistema di riferimento */
    void transRot( double * yRet, double * zRet,
                   double yP, double zP,
                   double alpha, double y0, double z0 );
};

#endif // SECTIONTHINCONTSTEEL_H
