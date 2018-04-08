#ifndef SECTIONFRPGUI_H
#define SECTIONFRPGUI_H

#include <QWidget>

namespace Ui {
    class SectionFRPGUI;
}

class SectionFRPGUI : public QWidget {
    Q_OBJECT
public:
    SectionFRPGUI(QWidget *parent = 0);
    ~SectionFRPGUI();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SectionFRPGUI *ui;
};

#endif // SECTIONFRPGUI_H
