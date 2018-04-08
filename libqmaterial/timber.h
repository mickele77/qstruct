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

#ifndef TIMBER_H
#define TIMBER_H

#include "material.h"

#include "sectiontimber.h"

#include "serviceclass.h"
#include "lstype.h"
#include "loaddurationclass.h"

class TimberPrivate;
class TimberTypePlus;
class WoodEssencePlus;

class DoublePlus;
class BoolPlus;

/**
* @class Timber
*
* @brief Classe usata per modellizzare il materiale legno da costruzioni
*
* Questa classe viene impiegata per modellizzare il legno, inteso come materiale di sezioni
*
* @author Michele Mocciola
*
*/

class EXPORT_MATERIAL_LIB_OPT Timber: public Material
{
    Q_OBJECT

public:
    /** Tipologia di legno */
    enum TimberType {
        solid,             /**< legno massiccio */
        glulam,            /**< legno lamellare incollato */
        LVL,               /**< microlammellare */
        plywood_1,         /**< compensato - EN 636 parte 1 */
        plywood_2,         /**< compensato - EN 636 parte 2 */
        plywood_3,         /**< compensato - EN 636 parte 3 */
        OSB_2,             /**< scaglie orientate - EN 300 OSB/2) */
        OSB_3,             /**< scaglie orientate - EN 300 OSB/3) */
        OSB_4,             /**< scaglie orientate - EN 300 OSB/4) */
        particle_4,        /**< particelle (truciolare) - EN 312 parte 4 */
        particle_5,        /**< particelle (truciolare) - EN 312 parte 5 */
        particle_6,        /**< particelle (truciolare) - EN 312 parte 6 */
        particle_7,        /**< particelle (truciolare) - EN 312 parte 7 */
        HBLA,              /**< fibre alta densita' - EN 622-2 HB.LA */
        HBLA1,             /**< fibre alta densita' - EN 622-2 HB.LA1 */
        HBLA2,             /**< fibre alta densita' - EN 622-2 HB.LA2 */
        MBHLA1,            /**< fibre media densita' - EN 622-3 MBH.LA1 */
        MBHLA2,            /**< fibre media densita' - EN 622-3 MBH.LA2 */
        MBHHLS1,           /**< fibre media densita' - EN 622-3 MBH.HLS1 */
        MBHHLS2,           /**< fibre media densita' - EN 622-3 MBH.HLS2 */
        MDFLA,             /**< fibre - EN 622-5 MDF.LA */
        MDFHLS             /**< fibre - EN 622-5 MDF.HLS */
    };

    /** Origine del legno (latifoglie o conifera) */
    enum WoodEssence {
        softwoods,   /**< confiere */
        hardwoods    /**< latifoglie */
    };

    /** Costruttore standard */
    Timber( UnitMeasure *, const QString & = QString(), QObject * parent = 0 );
    /** Costruttore da file XML */
    Timber( UnitMeasure * ump, const QXmlStreamAttributes & attrs, QObject * parent = 0  );

    /** Tipologia di materiale */
    MaterialModel::MaterialType materialType();

    /** Peso specifico caratteristico */
    DoublePlus * gammaWk;
    /** Resistenza a trazione in caso di flessione */
    DoublePlus * fmk;
    /** Resistenza a trazione parallela alle fibre */
    DoublePlus * ft0k;
    /** Resistenza a trazione perpendicolare alle fibre */
    DoublePlus * ft90k;
    /** Resistenza a compressione parallela alle fibre */
    DoublePlus * fc0k;
    /** Resistenza a compressione perpendicolare alle fibre */
    DoublePlus * fc90k;
    /** Resistenza a taglio */
    DoublePlus * fvk;
    /** Modulo di elasticita' normale parallelo alle fibre, valore medio */
    DoublePlus * E0m;
    /** Modulo di elasticita' normale parallelo alle fibre, valore caratteristico */
    DoublePlus * E0k;
    /** Modulo di elasticita' perpendicolare alle fibre, valore medio */
    DoublePlus * E90m;
    /** Modulo di elasticita' tangenziale, valore medio */
    DoublePlus * Gm;
    /** Coefficiente di Poisson, valore medio */
    DoublePlus * num;
    /** Tipologia di legno  */
    TimberTypePlus * timberType;
    /** Tipologia di legno  */
    WoodEssencePlus * woodEssence;

    /** Coefficiente di sicurezza per proprietà materiale */
    double gammaMNormal( LSType::Type lsType );
    /** Coefficiente di sicurezza per proprietà materiale */
    DoublePlus * gammaM( LSType * lsType );
    /** kmodNormal */
    double kmodNormal( ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    /** kmod */
    DoublePlus * kmod( ServiceClass * sClass, LoadDurationClass * ldClass );

    /** Coefficiente di sicurezza per proprietà materiale */
    void setGammaM( DoublePlus * var, LSType * lsType );
    /** kmod */
    void setKmod( DoublePlus * var, ServiceClass * sClass, LoadDurationClass * ldClass );

    /** Coefficiente amplificativo per sforzi normali di trazione paralleli alle fibre per tener conto dell'effetto volume
    @param h larghezza della sezione tesa (larghezza è la massima dimensione della sezione) o l'altezza della sezione inflessa
    @return il valore del parametro (UdM interne al programma) */
    double khNormal( double h = 0.0 );
    /** Coefficiente amplificativo per sforzi normali di trazione perpendicolari alle fibre per tener conto dell'effetto volume
    @param V volume uniformemente sollecitato
    @return il valore del parametro (UdM interne al programma) */
    double kVNormal( double V = 0.0 ) ;
    /** Coefficiente di correlazione tra resistenza a taglio e resistenza a torsione
    @param sType tipo di sezione
    @param b larghezza della base della sezione (solo per sezione rettangolare)
    @param h altezza della sezione (solo per sezione rettangolare)
    @return il valore del parametro (UdM interne al programma) */
    double kshNormal( SectionTimber::Shape sType, double b = 0.0, double h = 0.0 );
    /** Coefficiente di disomogeneita' per verifica a flessione deviata
    @param sType tipo di sezione
    @return il valore del parametro (UdM interne al programma) */
    double kmNormal( SectionTimber::Shape sType );
    /** lunghezza efficace per verifica a compressione perpendicolare alle fibre
    @param h altezza della sezione
    @param l lunghezza del tratto di trave su cui agisce F90,d
    @param l1conf lunghezza di confinamento lato 1
    @param l2conf lunghezza di confinamento lato 2
    @return il valore del parametro (UdM interne al programma) */
    double lEffNormal ( double h, double l, double l1Conf = 0.0, double l2Conf = 0.0 );

    /** Resistenza a trazione parallela alle fibre
    @param lsType tipo stato limite
    @param sClass classe di servizio
    @param ldClass classe di durata del carico
    @param khVal Coefficiente amplificativo per sforzi normali di trazione paralleli alle fibre
    @return il valore di resistenza (UdM interne al programma) */
    double ft0dNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass, double khVal );
    /** Resistenza a trazione perpendicolare alle fibre
    @param lsType tipo stato limite
    @param sClass classe di servizio
    @param ldClass classe di durata del carico
    @param kVVal coefficiente amplificativo per sforzi normali perpendicolari alle fibre
    @return il valore di resistenza (UdM interne al programma) */
    double ft90dNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass, double kVVal );
    /** Resistenza a compressione parallela alle fibre
    @param lsType tipo stato limite
    @param sClass classe di servizio
    @param ldClass classe di durata del carico
    @return il valore di resistenza (UdM interne al programma) */
    double fc0dNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    /** Resistenza a compressione perpendicolare alle fibre
    @param lsType tipo stato limite
    @param sClass classe di servizio
    @param ldClass classe di durata del carico
    @return il valore di resistenza (UdM interne al programma) */
    double fc90dNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    /** Resistenza a trazione inclinata di angle rispetto alle fibre
    @param lsType tipo stato limite
    @param sClass classe di servizio
    @param ldClass classe di durata del carico
    @param angle inclinazione dello sforzo agente rispetto alle fibre
    @return il valore di resistenza (UdM interne al programma) */
    double fcdNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass, double angle);
    /** Resistenza a trazione per flessione, parallela alle fibre
    @param lsType tipo stato limite
    @param sClass classe di servizio
    @param ldClass classe di durata del carico
    @param khVal coefficiente amplificativo
    @return il valore di resistenza (UdM interne al programma) */
    double fmdNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass, double khVal );
    /** Resistenza a taglio
    @param lsType tipo stato limite
    @param sClass classe di servizio
    @param ldClass classe di durata del carico
    @return il valore di resistenza (UdM interne al programma) */
    double fvdNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass );
    /** Resistenza a torsione
    @param lsType puntatore alla variabile contenente i dati sul tipo stato limite
    @param sClass puntatore alla variabile contenente il valore della classe di servizio
    @param ldClass puntatore alla variabile contenente il valore della classe di durata del carico
    @param kshVal Coefficiente di correlazione tra resistenza a taglio e resistenza a torsione
    @return il valore di resistenza (UdM interne al programma) */
    double ftorsdNormal( LSType::Type lsType, ServiceClass::Class sClass, LoadDurationClass::Class ldClass, double kshVal);

    /** Coefficiente amplificativo per sforzi normali di trazione paralleli alle fibre per tener conto dell'effetto volume
    @param h puntatore alla variabile contenente il valore della larghezza della sezione tesa (larghezza è la massima dimensione della sezione) o l'altezza della sezione inflessa
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void kh( DoublePlus * ret, DoublePlus * h );
    /** Coefficiente amplificativo per sforzi normali di trazione perpendicolari alle fibre per tener conto dell'effetto volume
    @param V puntatore alla variabile contenente il volume uniformemente sollecitato
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void kV( DoublePlus * ret, DoublePlus * V ) ;
    /** Coefficiente di correlazione tra resistenza a taglio e resistenza a torsione
    @param sType puntatore alla variabile contenente i dati sul tipo di sezione
    @param b puntatore alla variabile contenente i dati sulla larghezza della base della sezione (solo per sezione rettangolare)
    @param h puntatore alla variabile contenente i dati sull'altezza della sezione (solo per sezione rettangolare)
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void ksh( DoublePlus * ret, SectionTimber::Shape sType, DoublePlus * b, DoublePlus * h);
    /** Coefficiente di disomogeneita' per verifica a flessione deviata
    @param sType tipo di sezione
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void km( DoublePlus * ret, SectionTimber::Shape sType );
    /** lunghezza efficace per verifica a compressione perpendicolare alle fibre
    @param h puntatore alla variabile contenete i sati sull'altezza della sezione
    @param l puntatore alla variabile contenente i dati sulla lunghezza del tratto di trave su cui agisce F90,d
    @param lsc1 puntatore alla variabile contenente i dati sulla lunghezza di confinamento su lato 1
    @param lsc2 puntatore alla variabile contenente i dati sulla lunghezza di confinamento su lato 2
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void lEff ( DoublePlus * ret, DoublePlus * h, DoublePlus * l, DoublePlus * lsc1, DoublePlus * lsc2);

    /** Resistenza a trazione parallela alle fibre
    @param lsType puntatore alla variabile contenente i dati sul tipo stato limite
    @param sClass puntatore alla variabile contenente il valore della classe di servizio
    @param ldClass puntatore alla variabile contenente il valore della classe di durata del carico
    @param khVal puntatore alla variabile contenente il coefficiente di amplificazione kh
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void ft0d( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass, DoublePlus * khVal );
    /** Resistenza a trazione perpendicolare alle fibre
    @param lsType puntatore alla variabile contenente i dati sul tipo stato limite
    @param sClass puntatore alla variabile contenente il valore della classe di servizio
    @param ldClass puntatore alla variabile contenente il valore della classe di durata del carico
    @param kVVal puntatore alla variabile contenente il coefficiente amplificativo
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void ft90d( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass, DoublePlus * kVVal );
    /** Resistenza a compressione parallela alle fibre
    @param lsType puntatore alla variabile contenente i dati sul tipo stato limite
    @param sClass puntatore alla variabile contenente il valore della classe di servizio
    @param ldClass puntatore alla variabile contenente il valore della classe di durata del carico
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void fc0d( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );
    /** Resistenza a compressione perpendicolare alle fibre
    @param lsType puntatore alla variabile contenente i dati sul tipo stato limite
    @param sClass puntatore alla variabile contenente il valore della classe di servizio
    @param ldClass puntatore alla variabile contenente il valore della classe di durata del carico
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void fc90d( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass);
    /** Resistenza a trazione inclinata di angle rispetto alle fibre
    @param lsType puntatore alla variabile contenente i dati sul tipo stato limite
    @param sClass puntatore alla variabile contenente il valore della classe di servizio
    @param ldClass puntatore alla variabile contenente il valore della classe di durata del carico
    @param angle inclinazione dello sforzo agente rispetto alle fibre
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void fcd( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass, DoublePlus * angle);
    /** Resistenza a trazione per flessione, parallela alle fibre
    @param lsType puntatore alla variabile contenente i dati sul tipo stato limite
    @param sClass puntatore alla variabile contenente il valore della classe di servizio
    @param ldClass puntatore alla variabile contenente il valore della classe di durata del carico
    @param h puntatore alla variabile contenente il valore dell'altezza della sezione inflessa
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void fmd( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass, DoublePlus * khVal );
    /** Resistenza a taglio
    @param lsType puntatore alla variabile contenente i dati sul tipo stato limite
    @param sClass puntatore alla variabile contenente il valore della classe di servizio
    @param ldClass puntatore alla variabile contenente il valore della classe di durata del carico
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void fvd( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass );
    /** Resistenza a torsione
    @param lsType puntatore alla variabile contenente i dati sul tipo stato limite
    @param sClass puntatore alla variabile contenente il valore della classe di servizio
    @param ldClass puntatore alla variabile contenente il valore della classe di durata del carico
    @param ret puntatore alla variabile su cui viene scritto il valore di ritorno */
    void ftorsd( DoublePlus * ret, LSType * lsType, ServiceClass * sClass, LoadDurationClass * ldClass, DoublePlus * kshVal );

public slots:
    void copyfkToClipBoard();
    void copyEToClipBoard();

private:
    TimberPrivate * m_d;

    /** Inizializza le variabili membro */
    void initVar();

    /** Nel caso di chiamata dello slot da un connect(), imposta i valori del puntatore kmod chiamante,
        usando i valori previsti dalle NTC08, se la variabile è readOnly.
        Se è chiamato direttamente, effettua l'operazione su tutti i valori di kMod. */
    void setKmod( bool emitAuto );
    /** Nel caso di chiamata dello slot da un connect(), imposta i valori del puntatore gammaM chiamante,
        usando i valori previsti dalle NTC08, se la variabile è readOnly.
        Se è chiamato direttamente, effettua l'operazione su tutti i valori di gammaM. */
    void setGammaM(bool emitAuto );
    /** Imposta nell'attributo num i valori di default consigliati dalla normativa */
    void setNum(bool emitAuto );

private slots:
    void setKmod();
    void setGammaM();
    void setNum();
};

#include "enumplus.h"

class TimberTypePlusPrivate;

/**
* @class TimberTypePlus
*
* @brief Classe usata per accedere alle diverse tipologie di legno da costruzione
*
* Questa classe viene impiegata per definire le diverse tipologie di legno della classe Timber
*
* @author Michele Mocciola
*
*/

class EXPORT_MATERIAL_LIB_OPT TimberTypePlus : public EnumPlus
{
    Q_OBJECT
public:
    TimberTypePlus( Timber::TimberType tt,
                    const QString & nn,
                    bool ro = false );

    QString typeVar();

    TimberTypePlus & operator=( TimberTypePlus &);
    VarPlus & operator=( VarPlus &);

    Timber::TimberType value();
    Timber::TimberType valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValueNormal( Timber::TimberType, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValue( Timber::TimberType, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    TimberTypePlusPrivate * m_d;

    ~TimberTypePlus();
};

class WoodEssencePlusPrivate;

/**
* @class WoodEssencePlus
*
* @brief Classe usata per accedere alle diverse essenze di legno
*
* Questa classe viene impiegata per definire le diverse essenze di legno
*
* @author Michele Mocciola
*
*/

class EXPORT_MATERIAL_LIB_OPT WoodEssencePlus : public EnumPlus
{
    Q_OBJECT
public:
    WoodEssencePlus( Timber::WoodEssence tt,
                     const QString & nn,
                     bool ro = false );

    QString typeVar();

    WoodEssencePlus & operator=( WoodEssencePlus &);
    VarPlus & operator=( VarPlus &);

    Timber::WoodEssence value();
    Timber::WoodEssence valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValueNormal( Timber::WoodEssence, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValue( Timber::WoodEssence, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    WoodEssencePlusPrivate * m_d;

    ~WoodEssencePlus();
};

#endif // TIMBER_H
