#ifndef VARPLUSGUI_EXPORT_H
#define VARPLUSGUI_EXPORT_H

#include <QtGlobal>

#ifdef BUILD_SHARED_WIN
    #ifdef BUILD_VARPLUSGUI_LIB
        #define EXPORT_VARPLUSGUI_LIB_OPT Q_DECL_EXPORT
    #else
        #define EXPORT_VARPLUSGUI_LIB_OPT Q_DECL_IMPORT
    #endif
#else
    #define EXPORT_VARPLUSGUI_LIB_OPT
#endif

#endif // VARPLUSGUI_EXPORT_H
