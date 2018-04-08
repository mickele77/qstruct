#ifndef QCHECKBOXBOOL_H
#define QCHECKBOXBOOL_H

#include <QCheckBox>
#include "qwidgetplus.h"

/**
* @class QCheckBoxBOOL
*
* @brief Classe usata per rappresentare lo stato della proprietà readOnly() di un oggetto VarPlus
*
* Questa classe viene impiegata per rappresentare lo stato della proprietà readOnly() di un oggetto VarPlus.
* Si aggiorna automaticamente al variare della proprietà dell'oggetto VarPlus.
*
* @author Michele Mocciola
*
*/

class QCheckBoxBool : public QCheckBox, public QWidgetPlus {
    Q_OBJECT
public:
    explicit QCheckBoxBool(QWidget *parent = 0, VarPlus * v = 0 );
    ~QCheckBoxBool();

    void setVar( VarPlus * v);
    
private slots:
    void resetVar();

private:
    void connectVar();
    void disconnectVar();
};

#endif // QCHECKBOXBOOL_H
