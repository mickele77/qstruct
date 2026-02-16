#ifndef QCUSTOMPLOT_EXPORT_H
#define QCUSTOMPLOT_EXPORT_H

#include <QtGlobal>

#ifdef BUILD_SHARED_WIN
    #ifdef BUILD_QCUSTOMPLOT_LIB
        #define QCP_LIB_DECL Q_DECL_EXPORT
    #else
        #define QCP_LIB_DECL Q_DECL_IMPORT
    #endif
#else
    #define QCP_LIB_DECL
#endif

// define empty macro for Q_DECL_OVERRIDE if it doesn't exist (Qt < 5)
#ifndef Q_DECL_OVERRIDE
    #define Q_DECL_OVERRIDE
#endif

#endif // QCUSTOMPLOT_EXPORT_H
