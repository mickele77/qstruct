#ifndef LOADRESULTANT_H
#define LOADRESULTANT_H

class DoublePlus;
class QStringPlus;
class UnitMeasure;

#include "easy_export.h"
#include "varpluscontainer.h"
#include <QObject>

class EXPORT_EASY_LIB_OPT LoadSimple: public QObject, public VarPlusContainer{
    Q_OBJECT
public:
    LoadSimple( UnitMeasure * ump,
                double PxInp=0.0, double PyInp=0.0, double PzInp=0.0,
                double FxInp=0.0, double FyInp=0.0, double FzInp=0.0,
                double MxInp=0.0, double MyInp=0.0, double MzInp=0.0);
    QStringPlus * tag;
    DoublePlus * xP;
    DoublePlus * yP;
    DoublePlus * zP;
    DoublePlus * Fx;
    DoublePlus * Fy;
    DoublePlus * Fz;
    DoublePlus * Mx;
    DoublePlus * My;
    DoublePlus * Mz;
signals:
    void loadChanged();
};

class QFile;
class LoadResultantPrivate;

#include "tablemodelplus.h"

class EXPORT_EASY_LIB_OPT LoadResultant : public TableModelPlus {
    Q_OBJECT
public:
    LoadResultant(UnitMeasure *ump, QObject *parent = 0);

    /** Inserisce un carico */
    void insertRows( int position, int count = 1);
    void appendRows( int count = 1 );
    /** Rimuove un carico */
    void removeRows(int position, int count = 1 );

    void writeXml(QXmlStreamWriter * writer);
    void readXml(QXmlStreamReader * reader);

    LoadSimple * R;

    void loadFromFileTXT(QFile &file);

private slots:
    void updateR();
    void updateHeaders();

private:
    LoadResultantPrivate * m_dd;
};

#endif // LOADRESULTANT_H
