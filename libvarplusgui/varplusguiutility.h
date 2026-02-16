#ifndef VARPLUSGUIUTILITY_H
#define VARPLUSGUIUTILITY_H

#include "varplusgui_export.h"

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
EXPORT_VARPLUSGUI_LIB_OPT void connectVar(VarPlus * var, QLabelRichName * lrn, QLineEditValue * lev, QLabelUnitMeasure * lum = 0, QCheckBoxReadOnly * cba = 0 );
EXPORT_VARPLUSGUI_LIB_OPT void disconnectVar( QLabelRichName * lrn, QLineEditValue * lev, QLabelUnitMeasure * lum = 0, QCheckBoxReadOnly * cba = 0 );
EXPORT_VARPLUSGUI_LIB_OPT void connectEnumVar(EnumPlus * var, QLabelRichName * lrn, QComboBoxEnum * cb);
EXPORT_VARPLUSGUI_LIB_OPT void disconnectEnumVar(QLabelRichName * lrn, QComboBoxEnum * cb);
EXPORT_VARPLUSGUI_LIB_OPT void connectEnumVar(EnumPlus * var, QList<QRadioButtonEnum *> & listQRB);
EXPORT_VARPLUSGUI_LIB_OPT void disconnectEnumVar(QList<QRadioButtonEnum *> &listQRB );
EXPORT_VARPLUSGUI_LIB_OPT void connectVar(BoolPlus * var, QLabelRichName * lrn, QCheckBoxBool * cb = 0 );
EXPORT_VARPLUSGUI_LIB_OPT void disconnectVar(QLabelRichName * lrn, QCheckBoxBool * cb );
}

#endif // VARPLUSGUIUTILITY_H
