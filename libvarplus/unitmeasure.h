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
#ifndef UNITMEASURE_H
#define UNITMEASURE_H

class UnitMeasurePrivate;
class QXmlStreamWriter;
class QXmlStreamReader;
class QXmlStreamAttributes;

#include "varplus_export.h"
#include <QObject>

/**
* @class UnitMeasure
*
* @brief Classe base usata per definire le unità di misura di un progetto
*
* Questa classe viene impiegata per la definizione delle unità di misura usate all'interno
* di un progetto
*
* @author Michele Mocciola
*
*/

class EXPORT_VARPLUS_LIB_OPT UnitMeasure: public QObject
{
    Q_OBJECT
private:
    UnitMeasurePrivate * m_d;

public:
    enum unitMeasure{
        noDimension,    /**< Grandezze adimensionali (-) */
        percent,        /**< Grandezze adimensionali, percentuali (%) */
        length,         /**< lunghezza (in m)*/
        area,           /**< area (in m^2) */
        volume,         /**< volume (in m^3)*/
        angle,          /**< Angoli (in rad)*/
        time,           /**< tempo (sec) */
        tension,        /**< Tensione (in Pa)*/
        deformation,    /**< Deformazione (-) */
        curvature,      /**< Curvatura (1/m) */
        sectL,          /**< Dimensioni e coordinate di sezioni (in m)*/
        sectL2,         /**< Area di sezioni (in m^2)*/
        sectL3,         /**< Volumi travi, momenti static (in m^3)*/
        sectL4,         /**< Momento di inerzia di sezioni (in m^4)*/
        sectL5,         /**< ∫y3 dxdy, ∫y3 dxdy, ∫xy2 dxdy, ∫x2y dxdy (in m^5)*/
        sectL6,         /**< Iw (in m^4)*/
        alphaTemp,      /**< Coefficiente dilatazione termica (in °K^-1)*/
        loadF,          /**< Forza (in N)*/
        loadFLine,      /**< Forza riferita a lunghezze (in N/m) */
        loadFArea,      /**< Forza riferita a superfici (in N/m^2) */
        loadFVolume,    /**< Forza riferita a volumi (in N/m^3) */
        loadM,          /**< Coppia, momento (forza per lunghezza) (in N m)*/
        loadMLine,      /**< Coppia riferita a lunghezze (in N m/m) */
        SCE             /**< Energia specifica di rottura (in N/m) */
    };

    UnitMeasure( QObject * = 0 );

    UnitMeasure( UnitMeasure & , QObject * = 0 );

    UnitMeasure & operator=(const UnitMeasure & val);

    /** Scrive il contenuto dell'oggetto nel formato Xml */
    void writeXml( QXmlStreamWriter * writer );

    /** Legge i dati sulle unità di misura da file Xml */
    bool readXml( QXmlStreamReader * reader );

    /** @return il numero complessivo di unità di misura definite */
    int size();

    /** @param u unità di misura di cui si cerca la descrizione
        @return la descrizione dell'unità di misura indicata*/
    QString description( unitMeasure u );

    /** @param u unità di misura di cui si cerca la scala
        @return la scala dell'unità di misura indicata*/
    double scale( unitMeasure u );

    /** @param u unità di misura di cui si cerca la stringa
        @return la stringa dell'unità di misura indicata*/
    QString string( unitMeasure u );

    /** @param u unità di misura di cui si cerca la stringa
        @return la stringa dell'unità di misura indicata*/
    QString richString( unitMeasure u );

    QString description( int );
    double scale( int );
    QString string( int );
    QString richString( int );

public slots:
    /** Imposta la scala di una unità di misura
        @param u l'unità di misura di cui su imposta la scala
        @param s la nuova scala */
    void setScale( unitMeasure u , double s );
    /** Imposta la stringa di una unità di misura
        @param u l'unità di misura di cui su imposta la stringa
        @param s la nuova stringa */
    void setString( unitMeasure, const QString & );
    /** Imposta la stringa HTML di una unità di misura
        @param u l'unità di misura di cui su imposta la stringa HTML
        @param s la nuova stringa HTML */
    void setRichString( unitMeasure, const QString & );

    void setScale( int, double );
    void setString( int, const  QString & );
    void setRichString( int, const  QString & );

signals:
    /** Segnale emesso quando cambia la scala di una unità di misura
        @param u l'unità di misura di cui è cambiata la scala */
    void scalesChanged( UnitMeasure::unitMeasure u );
    /** Segnale emesso quando cambia la stringa di una unità di misura
        @param u l'unità di misura di cui è cambiata la stringa */
    void stringsChanged(  UnitMeasure::unitMeasure u );
    /** Segnale emesso quando cambia la stringa HTML di una unità di misura
        @param u l'unità di misura di cui è cambiata la stringa HTML */
    void richStringsChanged( UnitMeasure::unitMeasure u );
};

#endif // UNITMEASURE_H
