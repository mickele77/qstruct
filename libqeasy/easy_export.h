#ifndef EASY_EXPORT_H
#define EASY_EXPORT_H

#include <QtGlobal>

#ifdef BUILD_SHARED_WIN
    #ifdef BUILD_EASY_LIB
        #define EXPORT_EASY_LIB_OPT Q_DECL_EXPORT
    #else
        #define EXPORT_EASY_LIB_OPT Q_DECL_IMPORT
    #endif
#else
    #define EXPORT_EASY_LIB_OPT
#endif

#endif // EASY_EXPORT_H
