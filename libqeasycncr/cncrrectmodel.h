#ifndef CNCRRECTMODEL_H
#define CNCRRECTMODEL_H

class CncrRectModelPrivate;
class CncrRect;
class DoublePlus;

#include "easycncr_export.h"
#include "tablemodelplus.h"
#include "unitmeasure.h"

class EXPORT_EASYCNCR_LIB_OPT CncrRectModel: public TableModelPlus{
    Q_OBJECT
private:
    CncrRectModelPrivate * m_dd;
    void insertCncrRect(CncrRect *addedSect, int position);
    void removeCncrRect(int p);
    void writeXml(QXmlStreamWriter * writer);
    void readXml(QXmlStreamReader * reader);

private slots:
    void updateHeaders(UnitMeasure::unitMeasure um );
    void updateHG();
    void updateHTot();

signals:
    void hTotChanged(double);
    void hGChanged(double);

public:
    CncrRectModel( UnitMeasure * ump, QObject *parent = 0);
    void insertRows( int position, int count = 1);
    void removeRows(int position, int count = 1);
    DoublePlus * b( int i );
    DoublePlus * t( int i );
    int count();

    double ANormal();
    double SxNormal();
    double hGNormal();
};

#endif // CNCRRECTMODEL_H
