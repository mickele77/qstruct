#ifndef LOADRESULTANTGUI_H
#define LOADRESULTANTGUI_H

class LoadResultantGUIPrivate;
class LoadResultant;
class UnitMeasure;

#include <QWidget>

class LoadResultantGUI : public QWidget
{
    Q_OBJECT
    
public:
    explicit LoadResultantGUI(UnitMeasure *ump, QWidget *parent = 0);
    ~LoadResultantGUI();
    
private slots:
    void insertLoad();
    void removeLoad();
    void loadDataFromFileTXT();
    void loadDataFromFileXML();
    void writeDataToFileXML();

private:
    LoadResultantGUIPrivate * m_d;

    void connectVar();
};

#endif // LOADRESULTANTGUI_H
