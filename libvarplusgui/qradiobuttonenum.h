#ifndef QRADIOBUTTONENUM_H
#define QRADIOBUTTONENUM_H

class QRadioButtonEnumPrivate;

#include <QRadioButton>
#include "qwidgetenumplus.h"

class QRadioButtonEnum: public QRadioButton, public QWidgetEnumPlus {
    Q_OBJECT

public:
    QRadioButtonEnum( QWidget * parent = 0, EnumPlus * v = 0 );
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
