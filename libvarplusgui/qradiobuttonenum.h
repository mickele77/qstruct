#ifndef QRADIOBUTTONENUM_H
#define QRADIOBUTTONENUM_H

class QRadioButtonEnumPrivate;

#include "varplusgui_export.h"

#include <QRadioButton>
#include "qwidgetenumplus.h"

class EXPORT_VARPLUSGUI_LIB_OPT QRadioButtonEnum: public QRadioButton, public QWidgetEnumPlus {
    Q_OBJECT

public:
    QRadioButtonEnum( QWidget * parent = 0, EnumPlus * v = nullptr );
    ~QRadioButtonEnum();

    void setVar(EnumPlus *v, int vv = 0);

private slots:
    void setVarValue();
    void setCheckedInt( int );
    void resetVar();

private:
    int m_value;
    void connectVar();
    void disconnectVar();
};

#endif // QRADIOBUTTONENUM_H
