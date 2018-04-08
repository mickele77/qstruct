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

#ifndef TIMBERCONNECTION_H
#define TIMBERCONNECTION_H


class ServiceClass;
class LoadDurationClass;
class LSType;
class Timber;
class Steel;
class TimberConnector;
class DoublePlus;
class UIntPlus;
class QStringPlus;
class VarPlus;
class MaterialModel;
class QXmlAttributes;
class TimberConnectionType;
class TimberConnectionPrivate;

#include "timberconnection_export.h"

#include "varpluscontainer.h"
#include <QObject>

/**
* @class TimberConnection
*
* @brief Classe usata per modellizzare i collegamenti di strutture lignee
*
* Questa classe viene impiegata per modellizzare i collegamenti con connettori cilindrici all'interno di strutture lignee.
* Considera le connessioni legno-legno e quelle legno-acciaio.
*
* @author Michele Mocciola
*
*/

class EXPORT_TIMBERCONNECTION_LIB_OPT TimberConnection: public QObject, public VarPlusContainer
{
    Q_OBJECT
public:
    /** Diverse tipologie di connessione analizzate dalla classe. */
    enum ConnectionType {
        TTSingleShear,
        TTDoubleShear,
        TSSingleShear,
        TSDoubleShearCentralSteel,
        TSDoubleShearOuterSteel
    };

    /** Costruttore */
    TimberConnection( UnitMeasure * ump,
                      const QString &nPar,
                      Timber *timb,
                      ServiceClass * sc = 0,
                      LoadDurationClass * ld = 0,
                      LSType * ls = 0,
                      QObject * parent = 0 );

    /** Costruttore da flusso XML */
    TimberConnection(UnitMeasure *ump,
                     const QXmlStreamAttributes &attrs,
                     MaterialModel *mModel,
                     ServiceClass *sc = 0,
                     LoadDurationClass *ld = 0,
                     LSType *ls = 0,
                     QObject *parent = 0);


    /** Distruttore */
    ~TimberConnection();

    TimberConnection & operator=( TimberConnection & val);

    ServiceClass * sClass();
    LoadDurationClass * ldClass();
    LSType * lsType();

    TimberConnector * connector();
    void setConnector( TimberConnector * );

    /** Nel caso del calcolo del carico di taglio in connessioni legno-acciaio, è il legno di cui si consdierano le caratteristiche meccaniche.
        Nel caso del calcolo del carico di taglio in connessioni legno-legno, è uno dei legni di cui si consdierano le caratteristiche meccaniche.
        Per il calcolo del carico di estrazione, è il legno di cui si considerano le caratteristiche meccaniche */
    Timber * timber1();
    /** Imposta il legno 1 */
    void setTimber1( Timber * );
    /** Spessore legno 1 */
    DoublePlus * tTimber1;

    /** Nel caso del calcolo del carico di taglio in connessioni legno-legno, è uno dei legni di cui si consdierano le caratteristiche meccaniche. */
    Timber * timber2();
    /** Imposta il legno 2 */
    void setTimber2( Timber * );
    /** Spessore legno 2 */
    DoublePlus * tTimber2;

    Steel * steel();
    void setSteel( Steel * );
    /** Spessore lastra acciaio */
    DoublePlus * tSteel;

    /** Resistenza caratteristica del collegamento per carichi assiali */
    DoublePlus * FAxRk;
    /** Resistenza di progetto del collegamento per carichi assiali */
    DoublePlus * FAxRd;
    /** Nel caso di chiodi, valore caratteristico della tensione di estrazione del legno 1 (lato infissione).
        Nel caso di viti, valore caratteristico della tensione di estrazione, secondo direzione perpendicolare alle fibre di legno.
        Nel caso di bulloni, valore caratteristico della tensione massima sotto la rondella, legno 1.*/
    DoublePlus * fAxk1A;
    /** Nel caso di viti, valore caratteristico della tensione di estrazione, secondo direzione inclinata di alpha rispetto alle fibre di legno.*/
    DoublePlus * fAxk1B;
    /** Nel caso di chiodi, valore caratteristico della tensione di estrazione del legno 2 (legno attraversato).
        Nel caso di bulloni, valore caratteristico della tensione massima sotto la rondella, legno 2. */
    DoublePlus * fAxk2A;
    /** Nel caso di chiodi, valore caratteristico della tensione di estrazione sotto la testa del chiodo.*/
    DoublePlus * fAxk2B;

    /** Per collegamenti legno-legno, singolo piano di taglio: EC5 (8.6a)
        Per collegamenti legno-legno, doppio piano di taglio: EC5 (8.7g)
        Per collegamenti legno-acciaio, singolo piano di taglio, lastra sottile: EC5 (8.9a) */
    DoublePlus * FVRk1A;
    /** Per collegamenti legno-legno, singolo piano di taglio, EC5 (8.6b)
        Per collegamenti legno-legno, doppio piano di taglio, EC5 (8.7h)
        Per collegamenti legno-acciaio, singolo piano di taglio, lastra spessa: EC5 (8.10e)
        Per collegamenti legno-acciaio, doppio piano di taglio, piastra centrale: EC5 (8.11f)
        Per collegamenti legno-acciaio, doppio piano di taglio, piastre esterne: EC5 (8.12j) e (8.13l) */
    DoublePlus * FVRk1B;
    /** Per collegamenti legno-legno, singolo piano di taglio: EC5 (8.6c) */
    DoublePlus * FVRk1C;
    /** Per collegamenti legno-legno, singolo piano di taglio: EC5 (8.6d)
        Per collegamenti legno-legno, doppio piano di taglio: EC5 (8.7j)
        Per collegamenti legno-acciaio, singolo piano di taglio, lastra sottile: EC5 (8.9b)
        Per collegamenti legno-acciaio, doppio piano di taglio, piastre esterne, sottili: EC5 (8.12k) */
    DoublePlus * FVRk2A;
    /** Per collegamenti legno-legno, singolo piano di taglio, EC5 (8.6e)
        Per collegamenti legno-acciaio, singolo piano di taglio, lastra spessa: EC5 (8.10c)
        Per collegamenti legno-acciaio, doppio piano di taglio, piastra centrale: EC5 (8.11g) */
    DoublePlus * FVRk2B;
    /** Per collegamenti legno-legno, singolo piano di taglio, EC5 (8.6f)
        Per collegamenti legno-legno, doppio piano di taglio, EC5 (8.7k)
        Per collegamenti legno-acciaio, singolo piano di taglio, lastra spessa: EC5 (8.10d)
        Per collegamenti legno-acciaio, doppio piano di taglio, piastra centrale: EC5 (8.11h)
        Per collegamenti legno-acciaio, doppio piano di taglio, piastre esterne, spesse: EC5 (8.13m) */
    DoublePlus * FVRk3;
    /** Resistenza caratteristica a taglio del collegamento */
    DoublePlus * FVRk;
    /** Resistenza di progetto a taglio del collegamento */
    DoublePlus * FVRd;
    /** Valore caratteristico di rifollamento, secondo direzione fibre legno, legno 1 */
    DoublePlus * fh0k1;
    /** Valore caratteristico di rifollamento, legno 1 */
    DoublePlus * fhAlphak1;
    /** Valore caratteristico di rifollamento, legno 2 */
    DoublePlus * fhAlphak2;
    /** Valore caratteristico di rifollamento, secondo direzione fibre legno, legno 2 */
    DoublePlus * fh0k2;
    /** Rapporto tra fh2k e fh1k */
    DoublePlus * beta;
    /** Coefficiente di sicurezza per proprietà materiale */
    DoublePlus * gammaM;
    /** kmod */
    DoublePlus * kmod;
    /** Tipo di collegamento */
    TimberConnectionType * connectionType;

    /** Spaziatura dei connettori parallelamente alle fibre di legno */
    DoublePlus * a1;

    /** Numero dei connettori parallelo alle fibre */
    DoublePlus * nPar;
    /** Numero dei connettori perpendicolare alle fibre */
    DoublePlus * nPerp;
    /** Numero efficace dei connettori per sforzo di taglio, parallelo alle fibre */
    DoublePlus * nVEf;
    /** Resistenza di progetto complessiva del collegamento per carichi trasversali
        (resistenza del singolo connettore * numero efficace) */
    DoublePlus * FVRdTot;
    /** Numero efficace dei connettori per sforzo di estrazione */
    DoublePlus * nAxEf;
    /** Resistenza di progetto complessiva del collegamento per carichi assiali
        (resistenza del singolo connettore * numero efficace) */
    DoublePlus * FAxRdTot;

    /** Verifica a carica combinato, assiale e trasversale, usando EC5 (8.27) e EC5 (8.28) */
    bool verifyNormal( double FAxEd, double FVEd, double * res, QString * messages = NULL );
    bool verify( DoublePlus * FAxEd, DoublePlus * FVEd, DoublePlus * res, QString * messages = NULL );

    /** Scrive le variabili contenute nel contenitore sul flusso XML. */
    void writeXml(QXmlStreamWriter *writer);

    /** Carica i valori degli attributi pubblici da un oggetto di tipo QXmlAttributes
    @param attrs gli attributi da analizzare */
    void loadFromXML(const QXmlStreamAttributes &attrs, MaterialModel *mModel);
public slots:
    void setFAxk1A();
    void setFAxk1B();
    void setFAxk2A();
    void setFAxk2B();

    void setFAxRk();
    void setFAxRd();

    void setFh0k1();
    void setFhAlphak1();
    void setFh0k2();
    void setFhAlphak2();
    void setBeta();

    void setFVRk1A();
    void setFVRk1B();
    void setFVRk1C();
    void setFVRk2A();
    void setFVRk2B();
    void setFVRk3();
    void setFVRk();
    void setFVRd();
    /** Calcola il coefficiente di sicurezza per proprietà materiale */
    void setGammaM();
    /** calcola kmod */
    void setKmod();

    /** Pone uguale al puntatore NULL il puntatore al legno 1 */
    void setTimber1NULL();
    /** Pone uguale al puntatore NULL il puntatore al legno 2 */
    void setTimber2NULL();
    /** Pone uguale al puntatore NULL il puntatore all'acciaio */
    void setSteelNULL();

    void setNVEf();
    void setFVRdTot();
    void setNAxEf();
    void setFAxRdTot();

private:
    TimberConnectionPrivate * m_d;

    /** Coefficienti gammaM, sono pari al numero di stati limite contenuti in LSType */
    QList<double> * m_gammaM;

    /** Inizializza gli attributi pubblici (del tipo VarPlus) presenti nella classe */
    void initVar();

    /** Massimo rapporto del contributo della resistenza assiale al taglio (EC5 8.2.2 (2) )*/
    double maxVAxApport();

    double fh0kNormal( Timber * );
    double fhAlphakNormal( Timber * );

private slots:
    void setVisibleVar();
    void setVisibleVarConnector();

signals:
    void timber1Changed();
    void timber2Changed();
    void steelChanged();
};

class TimberConnectionTypePrivate;

#include "enumplus.h"

class TimberConnectionType : public EnumPlus
{
    Q_OBJECT
public:
    TimberConnectionType( TimberConnection::ConnectionType tt,
                              const QString & nn,
                              bool ro = false );

    TimberConnectionType & operator=( TimberConnectionType & );
    VarPlus & operator=( VarPlus &);

    QString typeVar();

    TimberConnection::ConnectionType value();
    TimberConnection::ConnectionType valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( TimberConnection::ConnectionType, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( TimberConnection::ConnectionType, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    TimberConnectionTypePrivate * m_d;

    ~TimberConnectionType();
};

#endif // TIMBERCONNECTION_H
