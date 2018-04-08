#ifndef SECTIONRCNCRRECTPANEL_H
#define SECTIONRCNCRRECTPANEL_H

#include <QWidget>

namespace Ui {
    class SectionRCncrRectPanel;
}
class UnitMeasure;
class ConcreteTableModel;
class SteelCncrTableModel;
class SectionRCncrRect;
class SectionSteelCncrTableModel;

class SectionRCncrRectPanel : public QWidget
{
    Q_OBJECT

public:
    explicit SectionRCncrRectPanel( SectionRCncrRect *, ConcreteTableModel *, SteelCncrTableModel *, QWidget * = 0  );
    ~SectionRCncrRectPanel();

private:
    Ui::SectionRCncrRectPanel *m_ui;
    ConcreteTableModel * m_cncrModel;
    SteelCncrTableModel * m_steelCncrModel;
    SectionRCncrRect * m_section;

private slots:
    void setConcrete( int );

    void addSectSteelCncr(void);
    void removeSectSteelCncr(void);
};

#endif // SECTIONRCNCRRECTPANEL_H
