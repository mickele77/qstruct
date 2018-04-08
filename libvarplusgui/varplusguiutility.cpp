#include "varplusguiutility.h"

#include "enumplus.h"
#include "boolplus.h"

#include "qlabelrichname.h"
#include "qlabelunitmeasure.h"
#include "qlineeditvalue.h"
#include "qcheckboxreadonly.h"
#include "qcheckboxbool.h"
#include "qcomboboxenum.h"
#include "qradiobuttonenum.h"

#include <QList>

void VarPlusGUI::connectVar( VarPlus * var, QLabelRichName * lrn, QLineEditValue * lev, QLabelUnitMeasure * lum, QCheckBoxReadOnly * cba ){
    if( var ){
        if( lrn ){
            lrn->setVar( var );
        }
        if( lum ){
            lum->setVar( var );
        }
        if( lev ){
            lev->setVar(var);
        }
        if( cba ){
            cba->setVar( var );
        }
    }
}

void VarPlusGUI::disconnectVar( QLabelRichName * lrn, QLineEditValue * lev, QLabelUnitMeasure * lum, QCheckBoxReadOnly * cba ){
    if( lrn ){
        lrn->setVar( NULL );
    }
    if( lum ){
        lum->setVar( NULL );
    }
    if( lev ){
        lev->setVar( NULL );
    }
    if( cba ){
        cba->setVar( NULL );
    }
}

void VarPlusGUI::connectEnumVar(EnumPlus * var, QLabelRichName * lrn, QComboBoxEnum * cb){
    if( lrn ){
        lrn->setVar( var );
    }
    if(cb){
        cb->setVar( var );
    }
}

void VarPlusGUI::disconnectEnumVar(QLabelRichName * lrn, QComboBoxEnum * cb){
    if( lrn ){
        lrn->setVar( NULL );
    }
    if( cb ){
        cb->setVar( NULL );
    }
}

void VarPlusGUI::connectEnumVar( EnumPlus * var, QList<QRadioButtonEnum * > & listQRB){
    for( int i=0; i < listQRB.size(); i++ ){
        listQRB.at(i)->setVar( var, i );
    }
}

void VarPlusGUI::disconnectEnumVar(QList<QRadioButtonEnum *> & listQRB){
    for( int i=0; i < listQRB.size(); i++ ){
        listQRB.at(i)->setVar( NULL );
    }
}


void VarPlusGUI::connectVar(BoolPlus *var, QLabelRichName *lrn, QCheckBoxBool *cb) {
    if( var ){
        if( lrn ){
            lrn->setVar( var );
        }
        if( cb ){
            cb->setVar( var );
        }
    }
}

void VarPlusGUI::disconnectVar( QLabelRichName * lrn, QCheckBoxBool *cb){
    if( lrn ){
        lrn->setVar( NULL );
    }
    if( cb ){
        cb->setVar( NULL );
    }
}

