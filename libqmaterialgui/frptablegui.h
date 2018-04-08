#ifndef FRPTABLEGUI_H
#define FRPTABLEGUI_H

#include <QDialog>

namespace Ui {
    class TableModelGUI;
}

class FRPTableGUI : public QDialog
{
    Q_OBJECT

public:
    explicit FRPTableGUI(QWidget *parent = 0);
    ~FRPTableGUI();

private:
    Ui::TableModelGUI *m_ui;
};

#endif // FRPTABLEGUI_H
