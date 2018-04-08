#ifndef TABLEMODELPLUSPRIVATE_H
#define TABLEMODELPLUSPRIVATE_H

#include "varplus.h"

#include <QList>
#include <QString>
#include <QSignalMapper>

class VarData{
public:
    VarData( VarPlus * v, int ddt ):
        pointer(v),
        displayedDataType(ddt){
    };
    VarData& operator= (const VarData &source) {
        pointer = source.pointer;
        displayedDataType = source.displayedDataType;
        return *this;
    }

    VarPlus  * pointer;
    int displayedDataType;
};

class TableModelPlusPrivate{
public:
    TableModelPlusPrivate( const QString & n, UnitMeasure * ump ):
        unitMeasure(ump),
        modelName(n){
    };
    void setHeaders( const QList<QString> & hC ){
        headerContainer = hC;
    };
    VarPlus * var( int r, int c){
        if( (r >= 0 && r < rowCount()) && (c >= 0 && c < colCount() )){
            return static_cast <VarPlus *>(varContainer.at(r).at(c).pointer);
        }
        return NULL;
    };
    void * pointer( int r, int c){
        if( (r >= 0 && r < rowCount()) && (c >= 0 && c < colCount() )){
            return varContainer.at(r).at(c).pointer;
        }
        return NULL;
    };
    int displayedDataType( int r, int c){
        if( (r >= 0 && r < rowCount()) && (c >= 0 && c < colCount() )){
            return varContainer.at(r).at(c).displayedDataType;
        }
        return -1;
    };
    void setVar( int dataType, int r, int c, VarPlus * var){
        if( (r >= 0 && r < rowCount()) && (c >= 0 && c < colCount() )){
            varContainer[r][c].pointer = var;
            varContainer[r][c].displayedDataType = dataType;
        }
    };
    void insertRows( int position, int count = 1  ){
        QList<VarData> listVar;
        for( int i=0; i<colCount(); ++i ){
            listVar.append( VarData(NULL, 0) );
        }
        for( int i=0; i < count; ++i ){
            varContainer.insert(position, listVar );
        }
    };
    void removeRows( int position, int count = 1  ){
        for( int i=0; i < count; ++i ){
            varContainer.removeAt( position );
        }
    };
    bool findVar( VarPlus * var, int * r = NULL, int * c = NULL  ){
        for( int i=0; i < varContainer.size(); ++i ){
            for( int j=0; j < varContainer.at(i).size(); ++j ){
                if( varContainer.at(i).at(j).pointer == var ){
                    if( r  )
                        * r = i;
                    if( c )
                        * c = j;
                    return true;
                }
            }
        }
        if( r  )
            * r = -1;
        if( c )
            * c = -1;
        return false;
    }
    int rowCount(){
        return varContainer.size();
    }
    int colCount(){
        return headerContainer.size();
    }
    QString header( int col ){
        if( col >= 0 && col < headerContainer.size() ){
            return headerContainer.at( col );
        }
        return QString();
    }

    UnitMeasure * unitMeasure;
    QString modelName;
    QList<QString> headerContainer;
    QList< QList<VarData> >varContainer;
    QSignalMapper mapper;
};

#endif // TABLEMODELPLUSPROVATE_H
