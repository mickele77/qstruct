#ifndef STEELFEPSMODEL_H
#define STEELFEPSMODEL_H

#include "easycncr_export.h"
#include "enumplus.h"
#include "simplesectionrcncrcomp.h"

class SteelFEpsModelPrivate;

class EXPORT_EASYCNCR_LIB_OPT SteelFEpsModel : public EnumPlus
{
    Q_OBJECT
public:
    SteelFEpsModel( SimpleSectionRCncrComp::ModelSteelFEps m,
              const QString & nn,
              bool ro = false );

    QString typeVar();

    SteelFEpsModel & operator=( SteelFEpsModel &);
    VarPlus & operator=( VarPlus &);

    SimpleSectionRCncrComp::ModelSteelFEps value();
    SimpleSectionRCncrComp::ModelSteelFEps valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( SimpleSectionRCncrComp::ModelSteelFEps m, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( SimpleSectionRCncrComp::ModelSteelFEps m, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    SteelFEpsModelPrivate * m_d;

    ~SteelFEpsModel();
};
#endif // STEELFEPSMODEL_H
