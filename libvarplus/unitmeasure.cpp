#include "unitmeasure.h"

#include "convertutility.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QMap>
#include <QList>

#include <cmath>

class UnitMeasureData{
public:
    UnitMeasureData( const QString & n,
                     UnitMeasure::unitMeasure um,
                     const QString & d,
                     double s,
                     const QString &str,
                     const QString & rStr) {
        name = n;
        unitMeasure = um;
        description = d;
        scale = s;
        string = str;
        richString = rStr;
    };
    UnitMeasureData & operator=(const UnitMeasureData & val){
        if( this != &val ){
            scale = val.scale;
            string = val.string;
            richString = val.richString;
        }
        return *this;
    }

    QString name;
    UnitMeasure::unitMeasure unitMeasure;
    QString description;
    double scale;
    QString string;
    QString richString;
};

class UnitMeasurePrivate{
public:
    UnitMeasurePrivate(){
        setDefaultData();
    };
    void addData( const QString & n,
                  UnitMeasure::unitMeasure um,
                  const QString & d,
                  double s,
                  const QString &str,
                  const QString & rStr ){
        UnitMeasureData * data = new UnitMeasureData(n, um, d, s, str, rStr);
        container.append( data );
        map.insert( um, data);
    };
    void setDefaultData(){
        addData( "noDimension",
                 UnitMeasure::noDimension,
                 QObject::tr("Grandezza adimensionale (-)"),
                 1.0,
                 QObject::tr("-"),
                 QObject::tr("-") );
        addData( "percent",
                 UnitMeasure::percent,
                 QObject::tr("Grandezza adimensionale percentuale (%)"),
                 100.0,
                 QObject::tr("%"),
                 QObject::tr("%") );
        addData( "length",
                 UnitMeasure::length,
                 QObject::tr("Lunghezza (m)" ),
                 1.0,
                 QObject::tr("m"),
                 QObject::tr("m") );
        addData( "area",
                 UnitMeasure::area,
                 QObject::tr("Area (m<span style=\" vertical-align:super;\">2</span>)" ),
                 1.0,
                 QObject::tr("m²"),
                 QObject::tr("m<span style=\" vertical-align:super;\">2</span>") );
        addData( "volume",
                 UnitMeasure::volume,
                 QObject::tr("Volume (m<span style=\" vertical-align:super;\">3</span>)"),
                 1.0,
                 QObject::tr("m³"),
                 QObject::tr("m<span style=\" vertical-align:super;\">3</span>") );
        addData( "angle",
                 UnitMeasure::angle,
                 QObject::tr("Angolo (rad)"),
                 180.0/M_PI,
                 QObject::tr("°"),
                 QObject::tr("°") );
        addData( "time",
                 UnitMeasure::time,
                 QObject::tr("Tempo (sec)"),
                 1.0,
                 QObject::tr("s"),
                 QObject::tr("s") );
        addData( "tension",
                 UnitMeasure::tension,
                 QObject::tr("Tensione (Pa)"),
                 1.0e-6,
                 QObject::tr("MPa"),
                 QObject::tr("MPa") );
        addData( "deformation",
                 UnitMeasure::deformation,
                 QObject::tr("Deformazione (-)"),
                 1.0e+2,
                 QObject::tr("%"),
                 QObject::tr("%") );
        addData( "curvature",
                 UnitMeasure::curvature,
                 QObject::tr("Curvatura (m<span style=\" vertical-align:super;\">-1</span>)"),
                 1.0e-1,
                 QObject::tr("% mm⁻¹"),
                 QObject::tr("% mm<span style=\" vertical-align:super;\">-1</span>") );
        addData( "sectL",
                 UnitMeasure::sectL,
                 QObject::tr("Dimensioni e coordinate sezione (in m)"),
                 1.0e+3,
                 QObject::tr("mm"),
                 QObject::tr("mm") );
        addData( "sectL2",
                 UnitMeasure::sectL2,
                 QObject::tr("Area sezione (m<span style=\" vertical-align:super;\">2</span>)"),
                 1.0e+6,
                 QObject::tr("mm²"),
                 QObject::tr("mm<span style=\" vertical-align:super;\">2</span>") );
        addData( "sectL3",
                 UnitMeasure::sectL3,
                 QObject::tr("Volume trave, momento statico sezione (m<span style=\" vertical-align:super;\">3</span>)"),
                 1.0e+9,
                 QObject::tr("mm³"),
                 QObject::tr("mm<span style=\" vertical-align:super;\">3</span>") );
        addData( "sectL4",
                 UnitMeasure::sectL4,
                 QObject::tr("Momento di inerzia sezione (m<span style=\" vertical-align:super;\">4</span>)"),
                 1.0e+12,
                 QObject::tr("mm⁴"),
                 QObject::tr("mm<span style=\" vertical-align:super;\">4</span>") );
        addData( "sectL5",
                 UnitMeasure::sectL5,
                 QObject::tr("Integrale del tipo ∫y3 dxdy su sezione (m<span style=\" vertical-align:super;\">5</span>)"),
                 1.0e+15,
                 QObject::tr("mm⁵"),
                 QObject::tr("mm<span style=\" vertical-align:super;\">5</span>") );
        addData( "sectL6",
                 UnitMeasure::sectL6,
                 QObject::tr("Costante di ingobbamento (m<span style=\" vertical-align:super;\">6</span>)"),
                 1.0e+18,
                 QObject::tr("mm⁶"),
                 QObject::tr("mm<span style=\" vertical-align:super;\">6</span>") );
        addData( "alphaTemp",
                 UnitMeasure::alphaTemp,
                 QObject::tr("Coefficiente dilatazione termica (K<span style=\" vertical-align:super;\">-1</span>)"),
                 1.0,
                 QObject::tr("K⁻¹"),
                 QObject::tr("K<span style=\" vertical-align:super;\">-1</span>") );
        addData( "loadF",
                 UnitMeasure::loadF,
                 QObject::tr("Forza (N)"),
                 1.0e-3,
                 QObject::tr("kN"),
                 QObject::tr("kN") );
        addData( "loadFLine",
                 UnitMeasure::loadFLine,
                 QObject::tr("Forza riferita a lunghezza (N/m)"),
                 1.0e-3,
                 QObject::tr("kN/m"),
                 QObject::tr("kN/m") );
        addData( "loadFArea",
                 UnitMeasure::loadFArea,
                 QObject::tr("Forza riferita a superficie (N/m<span style=\" vertical-align:super;\">2</span>)"),
                 1.0e-3,
                 QObject::tr("kN/m²"),
                 QObject::tr("kN/m<span style=\" vertical-align:super;\">2</span>") );
        addData( "loadFVolume",
                 UnitMeasure::loadFVolume,
                 QObject::tr("Forza riferita a volume (N/m<span style=\" vertical-align:super;\">3</span>)"),
                 1.0e-3,
                 QObject::tr("kN/m²"),
                 QObject::tr("kN/m<span style=\" vertical-align:super;\">2</span>") );
        addData( "loadM",
                 UnitMeasure::loadM,
                 QObject::tr("Coppia, momento (forza per lunghezza) (N m)"),
                 1.0e-3,
                 QObject::tr("kN m"),
                 QObject::tr("kN m") );
        addData( "loadMLine",
                 UnitMeasure::loadMLine,
                 QObject::tr("Coppia riferita a lunghezza (N m/m)"),
                 1.0e-3,
                 QObject::tr("kN m/m"),
                 QObject::tr("kN m/m") );
        addData( "SCE",
                 UnitMeasure::SCE,
                 QObject::tr("Energia specifica di frattura (N/m)"),
                 1.0e-3,
                 QObject::tr("N/mm"),
                 QObject::tr("N/mm") );
    };
    UnitMeasurePrivate & operator=(UnitMeasurePrivate & val){
        if( this != &val ){
            QList<UnitMeasureData *>::iterator iter1 = container.begin();
            while( iter1 != container.end()){
                QList<UnitMeasureData *>::iterator iter2 = val.container.begin();
                while( iter2 != val.container.end()){
                    if( ((*iter1)->name == (*iter2)->name) && ((*iter1)->unitMeasure == (*iter2)->unitMeasure) ){
                        **iter1 = **iter2;
                    }
                    iter2++;
                }
                iter1++;
            }
        }
        return *this;
    };
    void writeXml(QXmlStreamWriter * writer ){
        for( QList<UnitMeasureData *>::iterator i = container.begin(); i != container.end(); ++i ){
            writer->writeStartElement( (*i)->name);
            writer->writeAttribute( "scale", QString::number((*i)->scale) );
            writer->writeAttribute( "string", escapedXmlAttribute( (*i)->string ) );
            writer->writeAttribute( "richString", escapedXmlAttribute((*i)->richString ) );
            writer->writeEndElement();
        }
    }
    void setData( const QString & name, double scale, const QString & str, const QString & rStr ){
        QList<UnitMeasureData *>::iterator iter = container.begin();
        while( iter != container.end() ){
            if( (*iter)->name.toUpper() == name.toUpper() ){
                if( scale != 0.0 ){
                    (*iter)->scale = scale;
                }
                if( !str.isEmpty() ){
                    (*iter)->string = str;
                }
                if( !rStr.isEmpty() ){
                    (*iter)->richString = rStr;
                }
            }
            iter++;
        }
    }

    QList<UnitMeasureData *> container;
    QMap<UnitMeasure::unitMeasure, UnitMeasureData *> map;

};

UnitMeasure::UnitMeasure( QObject * parent):
    QObject(parent),
    m_d( new UnitMeasurePrivate() ){
}

UnitMeasure::UnitMeasure( UnitMeasure & val, QObject * parent ):
    QObject(parent),
    m_d( new UnitMeasurePrivate() ) {
    *this = val;
}

UnitMeasure & UnitMeasure::operator=(const UnitMeasure & val){
    if( this != &val ){
        *m_d = *(val.m_d);
    }
    return *this;
}

void UnitMeasure::writeXml(QXmlStreamWriter *writer) {
    writer->writeStartElement("UnitMeasure");
    m_d->writeXml( writer );
    writer->writeEndElement();
}

bool UnitMeasure::readXml(QXmlStreamReader *reader) {
    QString name = "UnitMeasure";
    if( reader->isStartElement() && reader->name().toString().toUpper() == name.toUpper() ){
        while( !(reader->atEnd()) ){
            reader->readNext();
            if( reader->isEndElement() || reader->name().toString().toUpper() == name.toUpper() ){
                return true;
            }
            QString name = reader->name().toString();
            double scale = 0.0;
            QString str, rStr;
            for( int i=0; i< reader->attributes().size(); i++ ){
                if( reader->attributes().at(i).name().toString().toUpper() == "SCALE" ){
                    scale = reader->attributes().at(i).value().toString().toDouble();
                }
                if( reader->attributes().at(i).name().toString().toUpper() == "STRING" ){
                    str = reader->attributes().at(i).value().toString();
                }
                if( reader->attributes().at(i).name().toString().toUpper() == "RICHSTRING" ){
                    rStr = reader->attributes().at(i).value().toString();
                }
                m_d->setData( name, scale, str, rStr );
            }
        }
    }
    return false;
}

int UnitMeasure::size(){ return m_d->container.size(); }

QString UnitMeasure::description( unitMeasure u ){ return m_d->map.value(u)->description;}
double UnitMeasure::scale( unitMeasure u ){ return m_d->map.value(u)->scale;}
QString UnitMeasure::string( unitMeasure u ){ return m_d->map.value(u)->string;}
QString UnitMeasure::richString( unitMeasure u ){ return m_d->map.value(u)->richString;}

QString UnitMeasure::description( int u ){ return m_d->container.at( u )->description;}
double UnitMeasure::scale( int u ){ return m_d->container.at( u )->scale;}
QString UnitMeasure::string( int u ){ return m_d->container.at(u)->string;}
QString UnitMeasure::richString( int u ){ return m_d->container.at(u)->richString;}

void UnitMeasure::setScale( unitMeasure u, double val){
    if( m_d->map.value(u)->scale != val ){
        m_d->map.value(u)->scale = val;
        emit scalesChanged( u );
    }
}

void UnitMeasure::setString( unitMeasure u, const  QString & str){
    if( m_d->map.value(u)->string != str ){
        m_d->map.value(u)->string = str;
        emit stringsChanged( u );
    }
}

void UnitMeasure::setRichString( unitMeasure u, const  QString & str){
    if( m_d->map.value(u)->richString != str ){
        m_d->map.value(u)->richString = str;
        emit richStringsChanged( u );
    }
}

void UnitMeasure::setScale( int u, double val){
    if( u >= 0 && u < m_d->container.size() ){
        if( m_d->container.at(u)->scale != val ){
            m_d->container.at(u)->scale = val;
            emit scalesChanged( (unitMeasure) (u) );
        }
    }
}
void UnitMeasure::setString( int u, const  QString & str){
    if( u >= 0 && u < m_d->container.size() ){
        if( m_d->container.at(u)->string != str ){
            m_d->container.at(u)->string = str;
            emit stringsChanged( (unitMeasure) (u) );
        }
    }
}

void UnitMeasure::setRichString( int u, const  QString & str){
    if( u >= 0 && u < m_d->container.size() ){
        if( m_d->container.at(u)->richString != str ){
            m_d->container.at(u)->richString = str;
            emit richStringsChanged( (unitMeasure) (u) );
        }
    }
}
