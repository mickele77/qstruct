#ifndef SECTIONFRP_H
#define SECTIONFRP_H

class FRP;
class Concrete;
class SectionFRPPrivate;

#include "sectionline.h"

class EXPORT_SECTION_LIB_OPT SectionFRP : public SectionLine {
    Q_OBJECT
public:
    explicit SectionFRP( UnitMeasure * ump, FRP * frp, Concrete * cncr, const QString & nn, QObject * parent = 0 );
    explicit SectionFRP(UnitMeasure *ump, const QXmlStreamAttributes &attrs, MaterialModel *mModel, SectionLoadPhaseModel * lModel, QObject *parent = 0 );

    ~SectionFRP();

    /** Scrive la sezione su flusso XML */
    void writeXml(QXmlStreamWriter *writer);

    /** Carica i valori degli attributi pubblici da un oggetto di tipo QXmlAttributes
    @param attrs gli attributi da analizzare */
    void loadFromXML(const QXmlStreamAttributes &attrs, MaterialModel * mModel , SectionLoadPhaseModel *loadModel);

    void setMaterial( Material * mat, bool emitAuto = true );
    void setFRP( FRP * frp, bool emitAuto = true );

    Concrete * concrete();
    void setConcrete( Concrete * cncr, bool emitAuto = true );

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + my * z), unità  di misura interne al programma
    * @param l
    * @param my
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo (unità  di misura interne del programma)
    */
    double NULSNormal(double l, double my, QList<Point2DModel *> * sects = NULL);

    /**
    * Calcola lo sforzo normale allo SLU con deformazione data (deformazione piana e = l + my * z + mz * y), unità  di misura interne al programma
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return lo sforzo normale allo stato limite ultimo (unità  di misura interne del programma)
    */
    double NULSNormal(double l, double my, double mz, QList<Point2DModel *> * sects = NULL);

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z), unità  di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente x del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param l
    * @param my
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità  di misura interne al programma)
    */
    void MULSNormal( double *yRet, double * zRet,
                     double l, double my,
                     QList<Point2DModel *> * sects = NULL );

    /**
    * Momento allo SLU con deformazione data (deformazione piana e = l + my * z), unità  di misura interne al programma
    * @param yRet puntatore alla variabile su cui viene scritta la componente x del momento allo SLU
    * @param zRet puntatore alla variabile su cui viene scritta la componente y del momento allo SLU
    * @param l
    * @param my
    * @param mz
    * @param sect1 zona con andamento parabolico delle tensioni
    * @param sect2 zona con andamento costante delle tensioni
    * @return Il momento allo stato limite ultimo con deformazione data di parametri l e m (unità  di misura interne al programma)
    */
    void MULSNormal( double *yRet, double * zRet,
                     double l, double my, double mz,
                     QList<Point2DModel *> * sects = NULL );

    /**
     * @brief larghezza della trave rinforzata
     */
    DoublePlus * b;

    /**
     * @brief larghezza ottimale di ancoraggio
     */
    DoublePlus * led;

    /**
     * @brief Valore di progetto dell'energia specifica di frattura
     */
    DoublePlus * LFd;

    /**
     * @brief Valore ultimo scorrimento tra FRP e supporto
     */
    DoublePlus * su;

    /**
     * @brief Fattore di tipo geometrico funziona della larghezza della trave rinforzata e del rinforzo
     */
    DoublePlus * kb;

    /**
     * @brief Coefficiente correttivo per calcolo energia rottura
     */
    DoublePlus * kG;

    /**
     * @brief Coefficiente correttivo per calcolo tensione di delaminazione pari a 0,2 mm
     */
    DoublePlus * kG2;

    /**
     * @brief Coefficiente correttivo per calcolo tensione di delaminazione
     */
    DoublePlus * kq;

    /**
     */
    DoublePlus * fbd;

    /** Tensione di progetto del rinforzo
     */
    DoublePlus * ffdd;

    /** Tensione di progetto del rinforzo per delaminazione di tipo 2
     */
    DoublePlus * ffdd2;

    /** Deformazione di progetto del rinforzo per delaminazione di tipo 2
     */
    DoublePlus * epsfdd;

    /** Deformazione di progetto del rinforzo
     */
    DoublePlus * epsfd;

    SectionFRP &operator =(SectionFRP &v);
    Section &operator =(Section &v);
signals:
    void concreteChanged( Concrete * oldConcrete, Concrete * newConcrete );

private slots:
    void setConcreteNULL();

private:
    SectionFRPPrivate * m_d;

    void initVar();

    void setLFd( bool emitAuto );
    void setLed( bool emitAuto );
    void setKb( bool emitAuto );
    void setFbd( bool emitAuto );
    void setFfdd( bool emitAuto );
    void setFfdd2( bool emitAuto );
    void setEpsfdd( bool emitAuto );
    void setEpsfd( bool emitAuto );

};

#endif // SECTIONFRP_H
