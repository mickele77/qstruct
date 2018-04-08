#ifndef QCOMBOBOXENUM_H
#define QCOMBOBOXENUM_H

#include <QComboBox>
#include "qwidgetenumplus.h"

/**
* @class QComboBoxEnum
*
* @brief Classe usata per rappresentare il valore di un oggetto EnumPlus
*
* Questa classe viene impiegata per rappresentare il valore di un oggetto EnumPlus
* Si aggiorna automaticamente al variare della proprietà dell'oggetto VarPlus.
*
* @author Michele Mocciola
*
*/

class QComboBoxEnum: public QComboBox, public QWidgetEnumPlus {
    Q_OBJECT
public:
    QComboBoxEnum( QWidget * parent = 0, EnumPlus * v = 0);
    ~QComboBoxEnum();

    void setVar( EnumPlus * v);

private slots:
    void resetVar();

private:
    void connectVar();
    void disconnectVar();
};

#endif // QCOMBOBOXENUM_H
