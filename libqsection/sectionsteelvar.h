#ifndef SECTIONSTEELVAR_H
#define SECTIONSTEELVAR_H

#include "sectionsteel.h"
#include "enumplus.h"

class SectionSteelManufactTypePrivate;

/**
* @class SectionSteelManufactType
*
* @brief Classe usata per accedere alle diverse modalità di realizzazione delle sezioni
*
* Questa classe viene impiegata per definire modalità di realizzazione delle sezioni
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionSteelManufactType : public EnumPlus
{
    Q_OBJECT
public:
    SectionSteelManufactType( SectionSteel::ManufactType mt,
                              const QString & nn,
                              bool ro = false );

    QString typeVar();

    SectionSteelManufactType & operator=( SectionSteelManufactType &);
    VarPlus & operator=( VarPlus &);

    SectionSteel::ManufactType value();
    SectionSteel::ManufactType valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( SectionSteel::ManufactType, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( SectionSteel::ManufactType, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    SectionSteelManufactTypePrivate * m_d;

    ~SectionSteelManufactType();
};

class SectionSteelShapePrivate;

/**
* @class SectionSteelShape
*
* @brief Classe usata per accedere alle diverse tipologie di sezione (forma geometrica)
*
* Questa classe viene impiegata per definire le diverse tipologie di sezione (forma geometrica)
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionSteelShape : public EnumPlus
{
    Q_OBJECT
public:
    SectionSteelShape( SectionSteel::SectionShape st,
                       const QString & nn,
                       bool ro = false );

    QString typeVar();

    SectionSteelShape & operator=( SectionSteelShape &);
    VarPlus & operator=( VarPlus &);

    SectionSteel::SectionShape value();
    SectionSteel::SectionShape valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( SectionSteel::SectionShape, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( SectionSteel::SectionShape, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    SectionSteelShapePrivate * m_d;

    ~SectionSteelShape();
};

class SectionSteelMomentDistributionPrivate;

/**
* @class SectionSteelMomentDistribution
*
* @brief Classe usata per accedere alle diverse tipologie di distribuzinoe del momento
*
* Questa classe viene impiegata per definire le diverse tipologie di distribuzinoe del momento
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionSteelMomentDistribution : public EnumPlus
{
    Q_OBJECT
public:
    SectionSteelMomentDistribution( SectionSteel::MomentDistribution st,
                                    const QString & nn,
                                    bool ro = false );

    QString typeVar();

    SectionSteelMomentDistribution & operator=( SectionSteelMomentDistribution &);
    VarPlus & operator=( VarPlus &);

    SectionSteel::MomentDistribution value();
    SectionSteel::MomentDistribution valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( SectionSteel::MomentDistribution, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( SectionSteel::MomentDistribution, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    SectionSteelMomentDistributionPrivate * m_d;

    ~SectionSteelMomentDistribution();
};
class SectionSteelTorsionalDeformabilityPrivate;

/**
* @class SectionSteelTorsionalDeformability
*
* @brief Classe usata per sapere se una sezione è o meno deformabile torionalmente
*
* Questa classe viene impiegata per sapere se una sezione è o meno deformabile torionalmente
*
* @author Michele Mocciola
*
*/

class EXPORT_SECTION_LIB_OPT SectionSteelTorsionalDeformability : public EnumPlus
{
    Q_OBJECT
public:
    SectionSteelTorsionalDeformability( SectionSteel::TorsionalDeformability st,
                                        const QString & nn,
                                        bool ro = false );

    QString typeVar();

    SectionSteelTorsionalDeformability & operator=( SectionSteelTorsionalDeformability &);
    VarPlus & operator=( VarPlus &);

    SectionSteel::TorsionalDeformability value();
    SectionSteel::TorsionalDeformability valueNormal();
    QString valueNormalStr();
    QString valueStr();

    int enumCount();
    QString enumStr( int );

    int valueNormalInt();

public slots:
    void setValue( SectionSteel::TorsionalDeformability, bool emitAuto = true );
    void setValue( const QString &, bool emitAuto = true );
    void setValueNormal( SectionSteel::TorsionalDeformability, bool emitAuto = true );
    void setValueNormal( const QString &, bool emitAuto = true );
    void setValueInt( int, bool emitAuto = true );

private:
    SectionSteelTorsionalDeformabilityPrivate * m_d;

    ~SectionSteelTorsionalDeformability();
};

#endif // SECTIONSTEELVAR_H
