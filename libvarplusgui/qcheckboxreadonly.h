#ifndef QCHECKBOXREADONLY_H
#define QCHECKBOXREADONLY_H

#include <QCheckBox>
#include "qwidgetplus.h"

/**
* @class QCheckBoxReadOnly
*
* @brief Classe usata per rappresentare lo stato della proprietà readOnly() di un oggetto VarPlus
*
* Questa classe viene impiegata per rappresentare lo stato della proprietà readOnly() di un oggetto VarPlus.
* Si aggiorna automaticamente al variare della proprietà dell'oggetto VarPlus.
*
* @author Michele Mocciola
*
*/

class QCheckBoxReadOnly : public QCheckBox, public QWidgetPlus {
    Q_OBJECT
public:
    explicit QCheckBoxReadOnly(QWidget *parent = 0, VarPlus * v = 0 );
    ~QCheckBoxReadOnly();

    void setVar( VarPlus * v);
    
private slots:
    void resetVar();

private:
    void connectVar();
    void disconnectVar();
};

#endif // QCHECKBOXREADONLY_H
