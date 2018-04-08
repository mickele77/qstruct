#ifndef VARPLUSCONTAINERPLUS_H
#define VARPLUSCONTAINERPLUS_H

#include "varpluscontainer.h"
#include <QObject>

class EXPORT_VARPLUS_LIB_OPT VarPlusContainerPlus: public QObject, public VarPlusContainer {
Q_OBJECT
public:
    VarPlusContainerPlus( UnitMeasure * ump, const QString & t, const QString & n, QObject * parent = 0 );
    virtual ~VarPlusContainerPlus();

    void addVarToContainer(VarPlus *var);

public slots:
    void updateReadOnlyVars();

signals:
    void containerChanged();
    void aboutToBeDeleted();
};

#endif // VARPLUSCONTAINERPLUS_H
