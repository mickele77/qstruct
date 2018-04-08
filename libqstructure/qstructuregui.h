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
#ifndef QSTRUCTUREGUI_H
#define QSTRUCTUREGUI_H

#include <QMainWindow>

class QStructureGUIPrivate;
class StructModule;
class QFile;

/**
* @class QStructureGUI
*
* @brief Finestra principale dell'applicazione QStruct
*
* Finestra principale dell'applicazione QStruc
*
* @author Michele Mocciola
*
*/

class QStructureGUI : public QMainWindow
{
    Q_OBJECT
public:
    /** Costruttore */
    QStructureGUI( QWidget * = 0 );

    void readSettings();
    void writeSettings();
private slots:
    void setModified();

    /** Apre un file esistente */
    void openFile();
    /** Apre un file recente */
    void openRecentFile();
    /** Crea un file nuovo */
    void newFile();
    /** Salva il progetto corrente */
    bool save();
    /** Salva il progetto corrente specificando il nome del file */
    bool saveAs();

    /** Attiva il form per cambiare le unità di misura */
    void editUnitMeasure();
    /** Attiva una finestra con le informazioni generiche sul programma */
    void about();

    /** Cambia il modulo attivo */
    void changeCurrentModule();

private:
    /** Numero massimo di file recenti sisualizzati nel menu file */
    enum { MaxRecentFiles = 5 };

    friend class QStructureGUIPrivate;

    /** Class privata contenente gli attributi */
    QStructureGUIPrivate * m_d;

    /** Alla chiusura dell'applicazione verifica che tutto sia a posto */
    void closeEvent(QCloseEvent *event);

    void setModified(bool v );
    /** Verifica se il modello è stato modificato dall'ultimo salvataggio.
        Se è stato modificato chiede se si vogliono salvare i cambiamenti. */
    bool okToContinue();
    /** Salva il modello in file */
    bool saveCurrentFile();
    /** Imposta il file corrente */
    bool setCurrentFile(const QString &fileName, bool readContent = false );

    // GUI
    void createActions();
    void updateRecentFileActions();
    void createMenus();
    void createToolBars();

    // Moduli
    void populateModulesList();
    void addModule( StructModule * );
};

#endif // QSTRUCTUREGUI_H
