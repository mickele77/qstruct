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

#endif // QCUSTOMPLOT_EXPORT_H
