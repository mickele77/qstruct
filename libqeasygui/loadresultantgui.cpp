#include "loadresultantgui.h"
#include "ui_loadresultantgui.h"

#include "loadresultant.h"
#include "doubleplus.h"
#include "varplusguiutility.h"

#include <QXmlStreamWriter>
#include <QFileDialog>
#include <QMessageBox>

class LoadResultantGUIPrivate{
public:
    LoadResultantGUIPrivate():
        ui(new Ui::LoadResultantGUI) {
    };
    ~LoadResultantGUIPrivate(){
        delete ui;
    }
    Ui::LoadResultantGUI *ui;
    LoadResultant * loadModel;
};

LoadResultantGUI::LoadResultantGUI( UnitMeasure * ump, QWidget *parent) :
    QWidget(parent),
    m_d( new LoadResultantGUIPrivate() ) {
    m_d->loadModel = new LoadResultant(ump, parent);
    m_d->ui->setupUi(this);
    connectVar();
    connect( m_d->ui->addFPushButton, SIGNAL(clicked()), this, SLOT(insertLoad()) );
    connect( m_d->ui->delFPushButton, SIGNAL(clicked()), this, SLOT(removeLoad()) );
    connect( m_d->ui->dataFromFileTXTPushButton, SIGNAL(clicked()), this, SLOT(loadDataFromFileTXT()) );
    connect( m_d->ui->dataFromFileXMLPushButton, SIGNAL(clicked()), this, SLOT(loadDataFromFileXML()) );
    connect( m_d->ui->dataToFileXMLPushButton, SIGNAL(clicked()), this, SLOT(writeDataToFileXML()) );
}

LoadResultantGUI::~LoadResultantGUI() {
    delete m_d;
}

void LoadResultantGUI::connectVar() {
    m_d->ui->tableViewF->setModel( m_d->loadModel );
    VarPlusGUI::connectVar( m_d->loadModel->R->xP, m_d->ui->xPLabel, m_d->ui->xPLineEdit, m_d->ui->xPUMLabel );
    VarPlusGUI::connectVar( m_d->loadModel->R->yP, m_d->ui->yPLabel, m_d->ui->yPLineEdit, m_d->ui->yPUMLabel );
    VarPlusGUI::connectVar( m_d->loadModel->R->zP, m_d->ui->zPLabel, m_d->ui->zPLineEdit, m_d->ui->zPUMLabel );
    VarPlusGUI::connectVar( m_d->loadModel->R->Fx, m_d->ui->FxLabel, m_d->ui->FxLineEdit, m_d->ui->FxUMLabel );
    VarPlusGUI::connectVar( m_d->loadModel->R->Fy, m_d->ui->FyLabel, m_d->ui->FyLineEdit, m_d->ui->FyUMLabel );
    VarPlusGUI::connectVar( m_d->loadModel->R->Fz, m_d->ui->FzLabel, m_d->ui->FzLineEdit, m_d->ui->FzUMLabel );
    VarPlusGUI::connectVar( m_d->loadModel->R->Mx, m_d->ui->MxLabel, m_d->ui->MxLineEdit, m_d->ui->MxUMLabel );
    VarPlusGUI::connectVar( m_d->loadModel->R->My, m_d->ui->MyLabel, m_d->ui->MyLineEdit, m_d->ui->MyUMLabel );
    VarPlusGUI::connectVar( m_d->loadModel->R->Mz, m_d->ui->MzLabel, m_d->ui->MzLineEdit, m_d->ui->MzUMLabel );
}

void LoadResultantGUI::insertLoad() {
    QModelIndexList rowListSelected = m_d->ui->tableViewF->selectionModel()->selectedRows();
    QList<int> rowList;
    for( int i=0; i < rowListSelected.size(); i++ ){
        if( !rowList.contains(rowListSelected.at(i).row()) ){
            rowList.append( rowListSelected.at(i).row() );
        }
    }
    qSort( rowList.begin(), rowList.end() );

    if( rowList.size() > 0 ){
        m_d->loadModel->insertRows( rowList.first(), rowList.size());
    } else {
        int row = m_d->ui->tableViewF->currentIndex().row();
        if( row < 0 || row > m_d->loadModel->rowCount() )
            row = m_d->loadModel->rowCount() - 1;
        m_d->loadModel->insertRows( row + 1 );
    }
}

void LoadResultantGUI::removeLoad() {
    QModelIndexList rowListSelected = m_d->ui->tableViewF->selectionModel()->selectedRows();
    QList<int> rowList;
    for( int i=0; i < rowListSelected.size(); i++ ){
        if( !rowList.contains(rowListSelected.at(i).row()) ){
            rowList.append( rowListSelected.at(i).row() );
        }
    }
    qSort( rowList.begin(), rowList.end() );

    for( int i=(rowList.size()-1); i >= 0; i-- ){
        m_d->loadModel->removeRows( rowList.at(i) );
    }
}

void LoadResultantGUI::loadDataFromFileTXT(){
    QString fileName = QFileDialog::getOpenFileName(this, trUtf8("Apri file TXT"), "", trUtf8("File TXT(%1)").arg("*.txt") );
    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if( !m_d->ui->addDataCheckBox->isChecked() ){
                m_d->loadModel->clear();
            }
            m_d->loadModel->loadFromFileTXT( file );
            file.close();
        } else {
            QMessageBox::warning( this, trUtf8("QStruct"), trUtf8("Non riesco ad aprire il file file %1:\n%2.").arg(fileName).arg(file.errorString()));
            return;
        }
    }
}

void LoadResultantGUI::loadDataFromFileXML(){
    QString fileName = QFileDialog::getOpenFileName(this, trUtf8("Apri file XML"), "", trUtf8("File XML (%1)").arg("*.xml") );
    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if( !m_d->ui->addDataCheckBox->isChecked() ){
                m_d->loadModel->clear();
            }
            QXmlStreamReader reader( &file );
            m_d->loadModel->readXml( &reader );
            file.close();
        } else {
            QMessageBox::warning( this, trUtf8("QStruct"), trUtf8("Non riesco ad aprire il file %1:\n%2.").arg(fileName).arg(file.errorString()));
            return;
        }
    }

}

void LoadResultantGUI::writeDataToFileXML(){
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    trUtf8("Salva su file XML"), ".",
                                                    trUtf8("File XML (%1)").arg("*.xml"));
    if (!fileName.isEmpty()){
        QFile file( fileName );
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            file.resize(0);
            QXmlStreamWriter writer(&file);
            m_d->loadModel->writeXml( &writer );
            file.flush();
            file.close();
        } else {
            QMessageBox::warning( this, trUtf8("QStruct"), trUtf8("Non riesco ad aprire il file %1:\n%2.").arg(fileName).arg(file.errorString()));
            return;
        }
    }
}
