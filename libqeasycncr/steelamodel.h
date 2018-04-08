#ifndef STEELAMODEL_H
#define STEELAMODEL_H

class SteelAModelPrivate;
class SteelA;
class DoublePlus;

#include "easycncr_export.h"
#include "tablemodelplus.h"
#include "unitmeasure.h"

class EXPORT_EASYCNCR_LIB_OPT SteelAModel: public TableModelPlus{
    Q_OBJECT
private:
    SteelAModelPrivate * m_dd;
    void insertSteelA(SteelA *addedA, int position);
    void removeSteelA(int p);
    void writeXml(QXmlStreamWriter * writer);
    void readXml(QXmlStreamReader * reader);

public slots:
    void updateDMax();
    void updateDMin();

private slots:
    void updateHeaders(UnitMeasure::unitMeasure um );

signals:
    void dMaxChanged(double);
    void dMinChanged(double);

public:
    SteelAModel( UnitMeasure * ump, QObject *parent = 0);
    void insertRows( int position, int count = 1);
    void removeRows(int position, int count = 1);
    DoublePlus * d( int i );
    DoublePlus * A( int i );
    int count();
    double SANormal();
    double SAdNormal();
    double SAd2Normal();
};

#endif // STEELAMODEL_H
