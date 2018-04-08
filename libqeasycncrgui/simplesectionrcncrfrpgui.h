#ifndef SIMPLESECTIONRCNCRFRPGUI_H
#define SIMPLESECTIONRCNCRFRPGUI_H

#include <QWidget>

namespace Ui {
class SimpleSectionRCncrFRPGUI;
}

class SimpleSectionRCncrFRPGUI : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleSectionRCncrFRPGUI(QWidget *parent = 0);
    ~SimpleSectionRCncrFRPGUI();

private:
    Ui::SimpleSectionRCncrFRPGUI *ui;
};

#endif // SIMPLESECTIONRCNCRFRPGUI_H
