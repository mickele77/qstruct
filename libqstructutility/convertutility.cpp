#include "convertutility.h"

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <cmath>

double convertPhi( const QString & expr ){
    QStringList list = expr.split('f');
    if( list.count() == 1 ){
        return expr.toDouble();
    } else if(list.count() > 1 ){
        return list.at(0).toDouble() * M_PI * pow( list.at(1).toDouble()/2.0, 2.0 );
    }
    return 0.0;
}

double convertArea( const QString & expr ){
    QString n;
    double ret = 0.0;
    double sign = 0.0;
    for( int i=0; i< expr.count(); i++){
        QChar c = expr.at(i).toLatin1();
        if( c == '+' ){
            if( sign == 0.0 ){
                ret = convertPhi( n );
            } else {
                ret += sign * convertPhi( n );
            }
            n.clear();
            sign = 1.0;
            continue;
        } else if( c == '-'){
            if( sign == 0.0 ){
                ret = convertPhi( n );
            } else {
                ret += sign * convertPhi( n );
            }
            n.clear();
            sign = -1.0;
            continue;
        }
        n.append(c);
    }
    if( sign == 0.0 ){
        ret = convertPhi( n );
    } else {
        ret += sign * convertPhi( n );
    }
    return ret;
}

QString escapedXmlAttribute(const QString &str) {
    QString ret = str;
    ret.replace("&", "&amp;");
    ret.replace("<", "&lt;");
    ret.replace(">", "&gt;");
    ret.replace("\"", "&quot;");
    return ret;
}
