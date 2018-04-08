#ifndef VARPLUSGUIUTILITY_H
#define VARPLUSGUIUTILITY_H

class VarPlus;
class BoolPlus;
class EnumPlus;
class QLabelRichName;
class QLabelUnitMeasure;
class QLineEditValue;
class QCheckBoxReadOnly;
class QCheckBoxBool;
class QComboBoxEnum;
class QRadioButtonEnum;
template <typename T> class QList;

namespace VarPlusGUI{
void connectVar(VarPlus * var, QLabelRichName * lrn, QLineEditValue * lev, QLabelUnitMeasure * lum = 0, QCheckBoxReadOnly * cba = 0 );
void disconnectVar( QLabelRichName * lrn, QLineEditValue * lev, QLabelUnitMeasure * lum = 0, QCheckBoxReadOnly * cba = 0 );
void connectEnumVar(EnumPlus * var, QLabelRichName * lrn, QComboBoxEnum * cb);
void disconnectEnumVar(QLabelRichName * lrn, QComboBoxEnum * cb);
void connectEnumVar(EnumPlus * var, QList<QRadioButtonEnum *> & listQRB);
void disconnectEnumVar(QList<QRadioButtonEnum *> &listQRB );
void connectVar(BoolPlus * var, QLabelRichName * lrn, QCheckBoxBool * cb = 0 );
void disconnectVar(QLabelRichName * lrn, QCheckBoxBool * cb );
}

#endif // VARPLUSGUIUTILITY_H
