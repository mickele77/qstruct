#ifndef QLABELUNITMEASURE_H
#define QLABELUNITMEASURE_H

#include "varplusgui_export.h"

#include <QLabel>
#include "qwidgetplus.h"

/**
* @class QLabelUnitMeasure
*
* @brief Classe usata per rappresentare la unitMeasure() di un oggetto VarPlus
*
* Questa classe viene impiegata per rappresentare la unitMeasure() di un oggetto VarPlus.
* Si aggiorna automaticamente al variare dell'oggetto VarPlus.
*
* @author Michele Mocciola
*
*/

class EXPORT_VARPLUSGUI_LIB_OPT QLabelUnitMeasure : public QLabel, public QWidgetPlus {
    Q_OBJECT
public:
    explicit QLabelUnitMeasure(QWidget *parent = 0, VarPlus * v = nullptr );
    ~QLabelUnitMeasure();

    void setVar( VarPlus * v);

private:
    void connectVar();
    void disconnectVar();

private slots:
    void updateText();
    void resetVar();
};

#endif // QLABELUNITMEASURE_H
