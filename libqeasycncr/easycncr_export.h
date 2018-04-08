#ifndef EASYCNCR_EXPORT_H
#define EASYCNCR_EXPORT_H

#include <QtGlobal>

#ifdef BUILD_SHARED_WIN
    #ifdef BUILD_EASYCNCR_LIB
        #define EXPORT_EASYCNCR_LIB_OPT Q_DECL_EXPORT
    #else
        #define EXPORT_EASYCNCR_LIB_OPT Q_DECL_IMPORT
    #endif
#else
    #define EXPORT_EASYCNCR_LIB_OPT
#endif

#endif // EASYCNCR_EXPORT_H
