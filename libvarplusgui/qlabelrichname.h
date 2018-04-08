#ifndef QLABELRICHNAME_H
#define QLABELRICHNAME_H

#include <QLabel>
#include "qwidgetplus.h"

/**
* @class QLabelRichName
*
* @brief Classe usata per rappresentare il richName() di un oggetto VarPlus
*
* Questa classe viene impiegata per rappresentare il richName di un oggetto VarPlus.
* Si aggiorna automaticamente al variare dell'oggetto VarPlus.
*
* @author Michele Mocciola
*
*/

class QLabelRichName : public QLabel, public QWidgetPlus {
    Q_OBJECT
public:
    explicit QLabelRichName(QWidget *parent = 0, VarPlus * v = 0 );
    ~QLabelRichName();

    void setVar( VarPlus * v);

private:
    void connectVar();
    void disconnectVar();

private slots:
    void resetVar();
    void setToolTip(const QString &newTT);
};

#endif // QLABELRICHNAME_H
