#ifndef VARPLUS_EXPORT_H
#define VARPLUS_EXPORT_H

#include <QtGlobal>

#ifdef BUILD_SHARED_WIN
    #ifdef BUILD_VARPLUS_LIB
        #define EXPORT_VARPLUS_LIB_OPT Q_DECL_EXPORT
    #else
        #define EXPORT_VARPLUS_LIB_OPT Q_DECL_IMPORT
    #endif
#else
    #define EXPORT_VARPLUS_LIB_OPT
#endif

#endif // VARPLUS_EXPORT_H
