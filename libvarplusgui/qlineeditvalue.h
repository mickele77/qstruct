#ifndef QLINEEDITVALUE_H
#define QLINEEDITVALUE_H

#include <QLineEdit>
#include "qwidgetplus.h"

/**
* @class QLineEditValue
*
* @brief Classe usata per rappresentare il richName() di un oggetto VarPlus
*
* Questa classe viene impiegata per rappresentare il richName di un oggetto VarPlus.
* Si aggiorna automaticamente al variare dell'oggetto VarPlus.
*
* @author Michele Mocciola
*
*/

class QLineEditValue : public QLineEdit, public QWidgetPlus {
    Q_OBJECT
public:
    explicit QLineEditValue(QWidget *parent = 0, VarPlus * v = 0 );
    ~QLineEditValue();

    void setVar( VarPlus * v);

    void update();
    
private slots:
    void setValue();
    void resetVar();
    void updateReadOnly();

private:
    void connectVar();
    void disconnectVar();
};

#endif // QLINEEDITVALUE_H
