#ifndef SECTIONWOODPANEL_H
#define SECTIONWOODPANEL_H

#include <QWidget>

namespace Ui {
    class SectionWoodPanel;
}

class SectionWoodPanel : public QWidget
{
    Q_OBJECT

public:
    explicit SectionWoodPanel(QWidget *parent = 0);
    ~SectionWoodPanel();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SectionWoodPanel *ui;
};

#endif // SECTIONWOODPANEL_H
