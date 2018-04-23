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
#ifndef VARPLUS_H
#define VARPLUS_H

#include "varplus_export.h"
#include <QObject>

class QString;
class QXmlStreamAttribute;

#include "tablemodelplus.h"
#include "varpluscontainerplus.h"
#include "varpluscontainer.h"
#include "unitmeasure.h"

/**
* @class VarPlus
*
* @brief Classe base usata per modellizzare grandezze fisiche rappresentate con un valore
*
* Questa classe viene impiegata per modellizzare grandezze fisiche rappresentabili con un singolo valore
*
* @author Michele Mocciola
*
*/

class EXPORT_VARPLUS_LIB_OPT VarPlus: public QObject {
    Q_OBJECT

public:
    /**
    Costruttore della classe
    @param nn Il nome della variabile
    @param ump il puntatore ad un oggetto della classe UnitMeasure contenente le informazioni sulle unita' di misura
    @param um l'unita' di misura della variabile
    @param em le modalita' di accesso al valore della variabile (valore in input, in output o entrambi)
    @param parent l'oggetto della classe QObject genitore
    */
    VarPlus( const QString & nn,
             UnitMeasure * ump,
             UnitMeasure::unitMeasure um,
             bool rOnly = false);

    VarPlus( VarPlus & );

    /** Operatore di assegnazione */
    virtual VarPlus & operator=( VarPlus &);


    QString name();     /**< @return Il nome della variabile */

    QString richName(); /**< @return Il nome della variabile in formato HTML */

    QString toolTip();  /**< @return Il suggerimento associato alla variabile */

    bool readOnly();  /**< @return Se la variabile e in sola lettura */

    bool enabled(); /**< @return Se la variabile è abilitata */

    /** Restituisce l'unita' di misura della variabile
    @return L'unita' di misura*/
    UnitMeasure::unitMeasure unitMeasureValue();

    /** Restituisce un puntatore alla classe contenente le unita' di misura della variabile
    @return puntatore alla classe contenente le unita' di misura */
    UnitMeasure * unitMeasurePointer();

    QXmlStreamAttribute xmlAttribute();

    /** Il tipo di variabile contenuto
    @return Il valore sotto forma di stringa
    */
    virtual QString typeVar() = 0;

    /** Il valore contenuto nella variabile
    @ return Il valore sotto forma di stringa
    */
    virtual QString valueNormalStr() = 0;

    /** Il valore contenuto nella variabile
    @ return Il valore sotto forma di stringa
    */
    virtual QString valueStr() = 0;

    /** Funzione statica che richiama il distruttore della classe
    @param var la variabile da terminare
    */
    static void deleteVarPlus( VarPlus* var );

    /** Aggiunge il contenitore all'elenco di quelli connessi */
    void addConnectedTableModel(TableModelPlus *tModel);

    /** Rimuove il contenitore all'elenco di quelli connessi */
    void removeConnectedTableModel(TableModelPlus * tModel);

    /** Pulisce l'elenco dei contenitori connessi */
    void clearConnectedTableModels();

    /** Aggiunge il contenitore all'elenco di quelli connessi */
    void addConnectedVarContainer(VarPlusContainerPlus *varCont, bool emitAuto = true );

    /** Rimuove il contenitore all'elenco di quelli connessi */
    void removeConnectedVarContainer(VarPlusContainerPlus *varCont, bool emitAuto = true );

    /** Pulisce l'elenco dei contenitori connessi */
    void clearConnectedVarContainers();

    /** Aggiunge la variabile all'elenco di quelle connesse */
    void addConnectedVar( VarPlus * var );

    /** Considera le variabili indicate dal secondo argomento in poi come connesse
     * @param nVars numero delle variabili aggiunte
    */
    void addConnectedVars(int nVars,...);

    /** Rimuove la variabile dall'elenco di quelle connesse */
    void removeConnectedVar( VarPlus * var );

    /** Pulisce l'elenco delle variabili connesse */
    void clearConnectedVars();

    /**
    * @return elenco delle variabili connesse
    */
    QList<VarPlus *> connectedVars();

    /** Aggiorna le variabili connesse con la variabile corrente, se non sono
      * gia' presenti in updatedVars */
    void updateConnectedVarsValue(QList<VarPlus *> *updatedVars);

    /** Associa alla variabile il metodo che ne calcola automaticamente il valore */
    void setUpdateValueMethod( VarPlusContainer * cont, void (VarPlusContainer::*method)(bool emitAuto) );

    /** Aggiorna la variabile chiamando il metodo che ne calcola in valore */
    virtual void updateValue( bool emitAuto );

    /** Disabilita l'aggiornamento ad ogni variazione delle variabili collegate */
    void suspendSignals( bool susp = true );

    /** Ci dice se e' disabilitato l'aggiornamento ad ogni variazione delle variabili collegate */
    bool signalsSuspended() const;

    /** Disabilita l'aggiornamento ad ogni variazione delle variabili collegate */
    void suspendUpdate( bool susp = true );

    /** Copia in un attributo privato valueStr() */
    void storeValue();

    /** Confronta il valore copiato con storeLastValue() con valueStr().
     * Se sono diversi emette il segnale valueChanged() */
    void checkStoredValue();

public slots:
    /** Imposta il nome della variabile
    @param nn il nuovo nome
    */
    void setName( const QString & nn);

    /** Setta il valore del flag booleano associato pari ad f */
    virtual void setReadOnly( bool f = true, bool emitAuto = true );
    void setReadOnlyWithEmit( bool f = true );

    /** Setta il valore del flag booleano associato pari al contrario di f*/
    virtual void setReadOnlyInv( bool f );

    /** Imposta il nome della variabile in formato HTML
    @param rnn il nuovo nome in formato HTML
    */
    void setRichName( const QString & rnn);

    /** Imposta il suggerimento associato alla variabile
    @param nn il nuovo suggerimento
    */
    void setToolTip( const QString & nn);

    /** Imposta l'unita' di misura della variabile
    @param uu la nuova unita' di misura*/
    void setUnitMeasure(UnitMeasure::unitMeasure uu);

    /** Imposta il valore della variabile in formato QString
    @param rnn il nuovo valore in formato QString
    */
    virtual void setValue( const QString &, bool emitAuto = true ) = 0;

    /** Imposta il valore della variabile in formato QString
    @param rnn il nuovo valore in formato QString
    */
    virtual void setValueNormal( const QString &, bool emitAuto = true ) = 0;

    /** Metodo usato per leggere il valore della variabile da attributo XML.
        xmlVal può essere uguale a "auto" (-> variabile readOnly) o essere
        uguale all'effettivo valore numerico. */
    void setValueXml( const QString & xmlVal, bool emitAuto = false );

    /** Per abilitare o disabilitare la variabile
    @param v se la variabile deve essere abilitata (true) o meno (false)
    */
    void setEnabled( bool v = true );

    void updateValue();

signals:
    void aboutToBeDeleted( VarPlus * );
    /** Segnale emesso quando il nome della variabile cambia
    @param name il nuovo nome */
    void nameChanged( const QString & name );
    /** Segnale emesso quando il flag readOnly cambia
    @param f il nuovo valore del flag readOnly */
    void readOnlyChanged( bool f );
    /** Segnale emesso quando il nome in formato HTML della variabile cambia
    @param name il nuovo nome in formato HTML */
    void richNameChanged( const QString & name );
    /** Segnale emesso quando il suggerimento associato alla variabile cambia
    @param name il nuovo suggerimento */
    void toolTipChanged( const QString & name );
    /** Segnale emesso quando cambia il valore dell'unita' di misura
    @param uu la nuova unita' di misura */
    void unitMeasureChanged( UnitMeasure::unitMeasure uu );
    /** Segnale emesso quando il valore associato alla variabile cambia */
    void valueChanged( const QString & );
    /** Segnale emesso quando il valore associato alla variabile cambia
        E diretto ai soli widget grafici */
    void valueChangedInternal( const QString & );
    /** Segnale emesso quando la variabile è abilitata/disabilitata */
    void enabledChanged( bool );
    /** Segnale emesso quando cambia la stringa della unità di misura
        Può cambiare perché è cambiata l'unità di misura o perché ne
        è cambiato il simbolo */
    void unitMeasureStringChanged( QString );
    /** Segnale emesso quando cambia la stringa "ricca" della unità di misura
        Può cambiare perché è cambiata l'unità di misura o perché ne
        è cambiato il simbolo */
    void unitMeasureRichStringChanged( QString );

protected:
    ~VarPlus();

    /** Il nome della variabile */
    QString * m_name;
    /** Variabile flag per sapere se la variabile è abilitata o no */
    bool m_enabled;
    /** Il nome della variabile in formato HTML */
    QString * m_richName;
    /** Il suggerimento visualizzato all'utente quando posiziona il puntatore sulla casella di testo */
    QString * m_toolTip;
    /** Un puntatore ad un oggetto UnitMeasure che contiene le informazioni sulle unita' di misura */
    UnitMeasure * m_unitMeasurePointer;
    /** L'unità di misura della variabile */
    UnitMeasure::unitMeasure m_unitMeasureValue;
    /** Ci dice se il valore è inserito dall'utente o se è calcolato in automatico dal software */
    bool m_readOnly;
    /** Ci dice se gli aggiornamenti sono sospesi */
    bool m_signalsSuspended;
    /** Contiene valueStr() quanto è chiamato il metodo storeValue() */
    QString m_storedValue;
    /** Lista dei modelli connessi */
    QList<TableModelPlus *> m_connectedTableModels;
    /** Lista delle variabili connesse */
    QList<VarPlusContainerPlus *> m_connectedVarContainers;
    /** Lista delle variabili connesse */
    QList<VarPlus *> m_connectedVars;
    /** Istanza del contenitore cui appartiene la variabile */
    VarPlusContainer * m_container;
    /** Puntatore allo slot che aggiorna il valore della variabile */
    void (VarPlusContainer::*m_updateValueMethod)(bool emitAuto);

protected slots:
    /** emette il segnale unitMeasureStringChanged() se è cambiata la stringa della unità di misura */
    void emitUnitMeasureStringChanged( UnitMeasure::unitMeasure u );
    /** emette il segnale unitMeasureRichStringChanged() se è cambiata la stringa HTML della
     *  unità di misura */
    void emitUnitMeasureRichStringChanged( UnitMeasure::unitMeasure u );
};

#endif
