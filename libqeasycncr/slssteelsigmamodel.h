#ifndef SLSSTEELSIGMAMODEL_H
#define SLSSTEELSIGMAMODEL_H

class SteelAModel;
class SLSSteelSigmaModelPrivate;

#include "easycncr_export.h"
#include "tablemodelplus.h"
#include "unitmeasure.h"

class EXPORT_EASYCNCR_LIB_OPT  SLSSteelSigmaModel: public TableModelPlus{
    Q_OBJECT
public:
    SLSSteelSigmaModel( SteelAModel * steelModel, UnitMeasure * ump, QObject *parent = 0);

    void writeXml(QXmlStreamWriter * writer);
    void readXml(QXmlStreamReader * reader);

    // annulla tutti i valori
    void resetValues();
    void setValueNormal(double v, int i);

private slots:
    void insertRows(const QModelIndex &parent, int start, int end);
    void removeRows(const QModelIndex &parent, int start, int end);
    void updateHeaders(UnitMeasure::unitMeasure um );
private:
    SLSSteelSigmaModelPrivate * m_dd;
    void removeRows(int position, int count);
};

#endif // SLSSTEELSIGMAMODEL_H
