/*
            Copyright (C) 2014-2017 Michele Mocciola

            This file is part of QStruct.

            QStruct is free software: you can redistribute it and/or modify
            it under the terms of the GNU General Public License as published by
            the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.

            QStruct is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
            GNU General Public License for more details.

            You should have received a copy of the GNU General Public License
            along with QStruct.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "qstructuregui.h"

#include "qstructure.h"

#include "unitmeasuregui.h"

#include "structmodule.h"
#include "materialmodule.h"
#include "sectionmodule.h"
#include "fem2dmodule.h"
#include "timberconnectionmodule.h"
#include "steelconnectionmodule.h"
#include "soilmodule.h"
#include "retainingwallmodule.h"
#include "soilsheetmodule.h"
#include "easymodule.h"
#include "easycncrmodule.h"

#include <QApplication>
#include <QMenu>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QAction>
#include <QActionGroup>
#include <QString>
#include <QList>
#include <QGridLayout>
#include <QCloseEvent>
#include <QSettings>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class QStructureGUIPrivate{
public:
    QStructureGUIPrivate( QWidget * parent ):
        structure(new QStructure( parent )) {

        modulesList = new QList<StructModule *>;
        modulesActionGroup = new QActionGroup( parent );
        currentModule = NULL;

        centralWidget = new QWidget(parent);
        centralWidget->setLayout( new QGridLayout( centralWidget ) );
    }
    ~QStructureGUIPrivate(){
    }

    /** File contenente le impostazioni */
    QString settingsFile;

    /** Puntatore alla classe QStructure contenente le informazioni sulla struttura */
    QStructure * structure;

    // Moduli
    /** Lista dei moduli */
    QList<StructModule *> * modulesList;
    /** Gruppo con le azioni associate ai moduli */
    QActionGroup * modulesActionGroup;
    /** Puntatore al modulo attivo */
    StructModule * currentModule;


    // *** GUI ***
    // File corrente
    QFile currentFile;
    // Elenco file recenti
    QStringList recentFiles;

    // Widget centrale
    QWidget * centralWidget;
    // Menu
    QMenu * fileMenu;
    QMenu * viewMenu;
    QMenu * toolsMenu;
    QMenu * helpMenu;
    // Toolbar
    QToolBar * modulesToolBar;
    QToolBar * fileToolBar;
    QToolBar * materialsToolBar;
    // Azioni
    QAction * newFileAct;
    QAction * openFileAct;
    QAction * saveAct;
    QAction * saveAsAct;
    QAction * separatorAction;
    QAction * recentFileActions[QStructureGUI::MaxRecentFiles];
    QAction * exitAct;
    QAction * editUnitMeasureAct;
    QAction * aboutQtAct;
    QAction * aboutAct;
};


QStructureGUI::QStructureGUI( QWidget * parent ):
    QMainWindow( parent ){

    m_d =  new QStructureGUIPrivate( this );

#ifdef _WIN32
    m_d->settingsFile = QApplication::applicationDirPath() + "/QStruct.ini";
#else
    m_d->settingsFile = "QStruct";
#endif

    setCentralWidget( m_d->centralWidget );

    populateModulesList();

    createActions();
    createMenus();
    createToolBars();

    changeCurrentModule();

    readSettings();
    setWindowIcon(QIcon(":/images/icon.svg"));
    setCurrentFile("");

    connect( m_d->structure, &QStructure::structureChanged, this, static_cast<void(QStructureGUI::*)()>(&QStructureGUI::setModified) );
}

void QStructureGUI::createActions(){
    m_d->newFileAct = new QAction(tr("&Nuovo"), this);
    m_d->newFileAct->setShortcut(tr("Ctrl+N"));
    m_d->newFileAct->setStatusTip(tr("Crea un nuovo progetto"));
    connect(m_d->newFileAct, SIGNAL(triggered()), this, SLOT(newFile()) );

    m_d->openFileAct = new QAction(tr("&Apri"), this);
    m_d->openFileAct->setShortcut(tr("Ctrl+A"));
    m_d->openFileAct->setStatusTip(tr("Apre un file esistente"));
    connect(m_d->openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));

    m_d->saveAct = new QAction(tr("&Salva"), this);
    m_d->saveAct->setShortcut(tr("Ctrl+S"));
    m_d->saveAct->setStatusTip(tr("Salva il progetto corrente"));
    connect(m_d->saveAct, SIGNAL(triggered()), this, SLOT(save()));

    m_d->saveAsAct = new QAction(tr("Salva con &nome..."), this);
    m_d->saveAsAct->setShortcut(tr("Ctrl+Shift+S"));
    m_d->saveAsAct->setStatusTip(tr("Salva il progetto corrente specificando il file di destinazione"));
    connect(m_d->saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        m_d->recentFileActions[i] = new QAction(this);
        m_d->recentFileActions[i]->setVisible(false);
        connect(m_d->recentFileActions[i], &QAction::triggered, this, &QStructureGUI::openRecentFile );
    }

    m_d->exitAct = new QAction(tr("&Esci"), this);
    m_d->exitAct->setShortcut(tr("Ctrl+Q"));
    m_d->exitAct->setStatusTip(tr("Esci dall'applicazione"));
    connect(m_d->exitAct, SIGNAL(triggered()), this, SLOT(close()));

    m_d->editUnitMeasureAct = new QAction( trUtf8("&Unità di misura"), this );
    m_d->editUnitMeasureAct->setStatusTip(trUtf8("Modifica le unitò dmi misura"));
    connect( m_d->editUnitMeasureAct, SIGNAL(triggered()), this, SLOT(editUnitMeasure()) );

    m_d->aboutQtAct = new QAction(tr("Informazioni sulle Qt"), this);
    m_d->aboutQtAct->setStatusTip(tr("Mostra informazioni sulle librerie Qt"));
    connect(m_d->aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    m_d->aboutAct = new QAction(tr("&Informazioni su QStruct"), this);
    m_d->aboutAct->setStatusTip(tr("Mostra informazioni su QStruct"));
    connect(m_d->aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void QStructureGUI::updateRecentFileActions() {
    int i = 0;
    while( i < m_d->recentFiles.size() ){
        QFileInfo fileInfo(m_d->recentFiles.at(i));
        if( fileInfo.exists() && fileInfo.isFile() ){
            i++;
        } else {
            m_d->recentFiles.removeAt(i);
        }
    }

    for (int j = 0; j < MaxRecentFiles; ++j) {
        bool actionSet = false;
        if (j < m_d->recentFiles.count()) {
            if( QFileInfo(m_d->recentFiles.at(j)).exists() ){
                QString text = tr("&%1 %2")
                        .arg(j + 1)
                        .arg( QFileInfo( m_d->recentFiles[j]).fileName() );
                m_d->recentFileActions[j]->setText(text);
                m_d->recentFileActions[j]->setData( m_d->recentFiles[j] );
                actionSet = true;
                m_d->recentFileActions[j]->setVisible(true);
            }
        }
        m_d->recentFileActions[j]->setVisible(actionSet);
    }

    m_d->separatorAction->setVisible(!m_d->recentFiles.isEmpty());
}

void QStructureGUI::createMenus(){
    m_d->fileMenu = menuBar()->addMenu(trUtf8("&File"));
    m_d->fileMenu->addAction( m_d->newFileAct );
    m_d->fileMenu->addAction( m_d->openFileAct );
    m_d->fileMenu->addAction( m_d->saveAct );
    m_d->fileMenu->addAction( m_d->saveAsAct );
    m_d->separatorAction = m_d->fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i){
        m_d->fileMenu->addAction(m_d->recentFileActions[i]);
    }
    m_d->fileMenu->addSeparator();
    m_d->fileMenu->addAction( m_d->exitAct );

    m_d->viewMenu = menuBar()->addMenu( trUtf8("Visualizza"));

    m_d->toolsMenu = menuBar()->addMenu( trUtf8("Strumenti"));
    m_d->toolsMenu->addAction( m_d->editUnitMeasureAct );

    m_d->helpMenu = menuBar()->addMenu(trUtf8("&Aiuto"));
    m_d->helpMenu->addAction( m_d->aboutQtAct );
    m_d->helpMenu->addAction( m_d->aboutAct );
}

void QStructureGUI::createToolBars(){
    m_d->modulesToolBar = new QToolBar(trUtf8("Moduli"), this );
    m_d->modulesToolBar->setObjectName( "Modules");
    addToolBar( Qt::LeftToolBarArea, m_d->modulesToolBar );
    m_d->modulesToolBar->setFloatable( false );
    m_d->modulesToolBar->setMovable( false);
    m_d->modulesToolBar->setAllowedAreas( Qt::LeftToolBarArea );
    m_d->modulesToolBar->setIconSize( QSize(32,32));
    m_d->modulesToolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    m_d->modulesToolBar->addActions( m_d->modulesActionGroup->actions() );
}

void QStructureGUI::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        if( m_d->currentFile.isOpen() ){
            m_d->currentFile.close();
        }
        event->accept();
    } else {
        event->ignore();
    }
    writeSettings();
}

bool QStructureGUI::okToContinue()
{
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, trUtf8("QStruct"),
                                     trUtf8("Il progetto è stato modificato.\n"
                                            "Vuoi salvare i cambiamenti?"),
                                     QMessageBox::Yes | QMessageBox::No
                                     | QMessageBox::Cancel);
        if (r == QMessageBox::Yes) {
            return save();
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void QStructureGUI::newFile() {
    if (okToContinue()) {
        m_d->structure->clear();
        setCurrentFile("");
    }
}

void QStructureGUI::openFile() {
    if (okToContinue()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        trUtf8("Apri il progetto"), ".",
                                                        trUtf8("File progetto QStruct(*.qst)"));
        setCurrentFile( fileName, true );
    }
}
void QStructureGUI::openRecentFile() {
    if (okToContinue()) {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action){
            setCurrentFile( action->data().toString(), true );
        }
    }
}

void QStructureGUI::setModified(bool v) {
    m_d->saveAct->setEnabled( v );
    setWindowModified( v );
}

void QStructureGUI::setModified() {
    setModified( true );
}

bool QStructureGUI::save(){
    if ( m_d->currentFile.isOpen() ) {
        return saveCurrentFile();
    } else {
        return saveAs();
    }
}

bool QStructureGUI::saveCurrentFile(){
    if( m_d->currentFile.isOpen() && m_d->currentFile.isWritable() ){
        m_d->currentFile.resize(0);
        QXmlStreamWriter writer(&m_d->currentFile);
        m_d->structure->writeXml( &writer );
        m_d->currentFile.flush();
        setModified( false );
        return true;
    }
    return false;
}

bool QStructureGUI::saveAs(){
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    trUtf8("Salva il progetto"), ".",
                                                    trUtf8("File progetto QStruct(*.qst)"));

    if (fileName.isEmpty()){
        return false;
    } else {
        QString fileNameUp = fileName.toUpper();
        if( !(fileNameUp.endsWith( ".QST")) ){
            fileName.append( ".qst");
        }
        setCurrentFile( fileName );
        return saveCurrentFile();
    }
}

bool QStructureGUI::setCurrentFile(const QString &fileName, bool readContent ) {
    if( !(fileName.isEmpty()) ) {
        // chiude il file esistente
        if( m_d->currentFile.exists() ){
            if( m_d->currentFile.isOpen() ){
                m_d->currentFile.close();
            }
        }

        // imposta il nuovo file
        m_d->currentFile.setFileName( fileName );

        // apre e legge il nuovo file
        if( !m_d->currentFile.fileName().isEmpty() ){
            if (!m_d->currentFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
                return false;
            }
            // legge il contenuto del file se richiesto
            if( readContent ){
                m_d->structure->clear();
                QXmlStreamReader reader( &m_d->currentFile );
                m_d->structure->readXml( &reader );
            }
        }

        // aggiorna il titolo della finestra
        QString shownName = trUtf8("Senza titolo");
        if( m_d->currentFile.isOpen() ){
            shownName = m_d->currentFile.fileName();
            m_d->recentFiles.removeAll(fileName);
            QFileInfo fileInfo(fileName);
            fileInfo.makeAbsolute();
            m_d->recentFiles.prepend( fileInfo.filePath() );
            updateRecentFileActions();
        }
        setWindowTitle(tr("%1[*] - %2").arg(shownName)
                       .arg(tr("QStruct")));
        setModified(false);
        return true;
    } else {
        setWindowTitle(QString("%1[*] - %2").arg(trUtf8("Senza titolo"))
                       .arg(trUtf8("QStruct")));
        setModified(false);
        return false;
    }
}

void QStructureGUI::editUnitMeasure(){
    UnitMeasureGUI gui( m_d->structure->unitMeasure(), this );
    gui.exec();
}

void QStructureGUI::about() {
    QMessageBox::about(this, trUtf8("Informazioni su QStruct"),
                       trUtf8("<h2>QStruct</h2>"
                              "<p>Copyright &copy; 2014-2017 Michele Mocciola."
                              "<p>Un software per il calcolo strutturale"
                              " rilasciato sotto licenza GPL v3." ));
}

void QStructureGUI::changeCurrentModule(){
    int currIndex = m_d->modulesActionGroup->actions().indexOf( m_d->modulesActionGroup->checkedAction() );
    if( m_d->currentModule != m_d->modulesList->at(currIndex) ){
        if ( m_d->currentModule  != NULL ){
            m_d->currentModule->setState( saveState() );
            m_d->currentModule->hide();
            QList<QDockWidget *> listDocks = m_d->currentModule->panels();
            for( int i=0; i < listDocks.size(); i++ ){
                listDocks.at(i)->hide();
                removeDockWidget( listDocks.at(i) );
            }
        }
        m_d->currentModule = m_d->modulesList->at(currIndex);
        if ( m_d->currentModule  != NULL ){
            m_d->currentModule->show();
            QList<QDockWidget *> listDocks = m_d->currentModule->panels();
            for( int i=0; i < listDocks.size(); i++ ){
                addDockWidget( Qt::RightDockWidgetArea, listDocks.at(i) );
                listDocks.at(i)->show();
            }
            restoreState( m_d->currentModule->state() );
        }
    }
}

void QStructureGUI::populateModulesList(){
    m_d->modulesActionGroup->setExclusive( true );

    addModule( new MaterialModule(m_d->structure->unitMeasure(), m_d->structure->materialModel(), m_d->settingsFile, m_d->centralWidget ) );
    addModule( new SectionModule(m_d->structure->unitMeasure(), m_d->structure->materialModel(), m_d->structure->sectionModel(), m_d->settingsFile, this ) );
    addModule( new Fem2DModule(m_d->structure->unitMeasure(), m_d->structure->sectionModel(), this ) );
    addModule( new TimberConnectionModule(m_d->structure->unitMeasure(), m_d->structure->materialModel(), m_d->structure->timberConnectionModel(), this ) );
    addModule( new SteelConnectionModule( m_d->structure->unitMeasure(), this ) );
    addModule( new SoilModule(m_d->structure->unitMeasure(), m_d->structure->soilModel(), this ) );
    addModule( new RetainingWallModule(m_d->structure->unitMeasure(), m_d->structure->retainingWallModel(), m_d->structure->soilModel(), this ) );
    addModule( new SoilSheetModule(m_d->structure->unitMeasure(), m_d->structure->soilSheetModel(), m_d->structure->soilModel(), this ) );
    addModule( new EasyCncrModule(m_d->structure->unitMeasure(), this ) );
    addModule( new EasyModule(m_d->structure->unitMeasure(), this ) );

    connect( m_d->modulesActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeCurrentModule()) );

    m_d->modulesActionGroup->actions().at(0)->setChecked( true );
}

void QStructureGUI::addModule( StructModule * module ){
    module->hide();
    m_d->centralWidget->layout()->addWidget( module );

    m_d->modulesList->append( module );
    QAction * actionModule = new QAction( module->icon(), module->richName(), this );
    actionModule->setCheckable( true );
    m_d->modulesActionGroup->addAction( actionModule );
}

void QStructureGUI::readSettings() {
#ifdef _win32
    QSettings settings( m_d->settingsFile, QSettings::IniFormat );
#else
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope, m_d->settingsFile );
#endif
    restoreGeometry(settings.value("geometry").toByteArray());
    m_d->recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();
}

void QStructureGUI::writeSettings() {
#ifdef _win32
    QSettings settings( m_d->settingsFile, QSettings::IniFormat );
#else
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope, m_d->settingsFile );
#endif

    settings.setValue("geometry", saveGeometry());
    settings.setValue("recentFiles", m_d->recentFiles);
}
