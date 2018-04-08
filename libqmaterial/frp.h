/*
    Copyright (C) 2014-2017 Michele Mocciola

    This file is part of QStruct.

    QStruct is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QStruct is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QStruct.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef FRP_H
#define FRP_H

#include "material.h"

class FRPExposition;
class FRPFiberType;
class FRPApplicationType;
class DoublePlus;

class EXPORT_MATERIAL_LIB_OPT FRP: public Material{
    Q_OBJECT
public:
    enum Exposition {
        internal,
        external,
        aggressive
    };
    enum FiberType {
        epoxyGlass,
        epoxyAramid,
        epoxyCarbon
    };
    enum ResistenceModel {
        normalBending,
        shearTorsion,
        confinement
    };
    enum ApplicationType {
        A,
        B
    };
    enum LoadMode {
        persistent,
        cyclic
    };


    FRP(UnitMeasure * ump, const QString &n = QString(), QObject * parent = 0 );

    FRP(UnitMeasure * ump, const QXmlStreamAttributes &attrs, QObject * parent = 0 );

    MaterialModel::MaterialType materialType();

    /// Tensione a rottura caratteristica
    DoublePlus * ffk;

    /// Deformazione a rottura caratteristica
    DoublePlus * epsfk;

    /// fattore di conversione ambientale
    DoublePlus * etaa;

    /// coefficiente parziale a rottura per i materiali
    DoublePlus * gammaf;

    /// coefficiente parziale a distacco per i materiali
    DoublePlus * gammafd;

    /// coefficiente parziale per i modelli di resistenza
    double gammaRd( FRP::ResistenceModel resModel );

    /// Fattore di conversione per effetti di lunga durata
    double etal( FRP::LoadMode lMode );

    /// Esposizione ambientale
    FRPExposition * exposition;

    /// Tipo di fibra
    FRPFiberType * fiberType;

    /// Tipo di applicazione (A/B)
    FRPApplicationType * applicationType;

private:
    void initVar();

    void setEtaa( bool emitAuto );
    void setGammaf( bool emitAuto );
    void setGammafd( bool emitAuto );
    void setEpsfk( bool emitAuto );
};

#include "enumplus.h"

class FRPExpositionPrivate;

class FRPExposition: public EnumPlus {
    Q_OBJECT
public:
    FRPExposition( FRP::Exposition vv,
                   const QString & nn,
                   bool ro = false);

    FRPExposition(FRPExposition &val);
    ~FRPExposition();
    QString typeVar();
    FRPExposition &operator =(FRPExposition &val);
    VarPlus &operator =(VarPlus &val);
    FRP::Exposition value();
    FRP::Exposition valueNormal();
    QString valueNormalStr();
    QString valueStr();
    int enumCount();
    QString enumStr(int v);
    int valueNormalInt();

public slots:
    void setValueNormal(FRP::Exposition vv, bool emitAuto = true );
    void setValueNormal(const QString &vv, bool emitAuto = true );
    void setValueInt(int i, bool emitAuto = true );
    void setValue(FRP::Exposition vv, bool emitAuto = true );
    void setValue(const QString &vv, bool emitAuto = true );

private:
    FRPExpositionPrivate * m_d;
};

class FRPFiberTypePrivate;

class FRPFiberType: public EnumPlus {
    Q_OBJECT
public:
    FRPFiberType( FRP::FiberType vv,
                  const QString & nn,
                  bool ro = false );

    FRPFiberType(FRPFiberType &val);
    ~FRPFiberType();
    QString typeVar();
    FRPFiberType &operator =(FRPFiberType &val);
    VarPlus &operator =(VarPlus &val);
    FRP::FiberType value();
    FRP::FiberType valueNormal();
    QString valueNormalStr();
    QString valueStr();
    int enumCount();
    QString enumStr(int v);
    int valueNormalInt();

public slots:
    void setValueNormal(FRP::FiberType vv, bool emitAuto = true );
    void setValueNormal(const QString &vv, bool emitAuto = true );
    void setValueInt(int i, bool emitAuto = true );
    void setValue(FRP::FiberType vv, bool emitAuto = true );
    void setValue(const QString &vv, bool emitAuto = true );

private:
    FRPFiberTypePrivate * m_d;
};

class FRPApplicationTypePrivate;

class FRPApplicationType: public EnumPlus {
    Q_OBJECT
public:
    FRPApplicationType( FRP::ApplicationType vv,
                  const QString & nn,
                  bool ro = false );

    FRPApplicationType(FRPApplicationType &val);
    ~FRPApplicationType();
    QString typeVar();
    FRPApplicationType &operator =(FRPApplicationType &val);
    VarPlus &operator =(VarPlus &val);
    FRP::ApplicationType value();
    FRP::ApplicationType valueNormal();
    QString valueNormalStr();
    QString valueStr();
    int enumCount();
    QString enumStr(int v);
    int valueNormalInt();

public slots:
    void setValueNormal(FRP::ApplicationType vv, bool emitAuto = true );
    void setValueNormal(const QString &vv, bool emitAuto = true );
    void setValueInt(int i, bool emitAuto = true );
    void setValue(FRP::ApplicationType vv, bool emitAuto = true );
    void setValue(const QString &vv, bool emitAuto = true );

private:
    FRPApplicationTypePrivate * m_d;
};

class FRPResistenceModelPrivate;

class FRPResistenceModel: public EnumPlus {
    Q_OBJECT
public:
    FRPResistenceModel( FRP::ResistenceModel vv,
                  const QString & nn,
                  bool ro = false );

    FRPResistenceModel(FRPResistenceModel &val);
    ~FRPResistenceModel();
    QString typeVar();
    FRPResistenceModel &operator =(FRPResistenceModel &val);
    VarPlus &operator =(VarPlus &val);
    FRP::ResistenceModel value();
    FRP::ResistenceModel valueNormal();
    QString valueNormalStr();
    QString valueStr();
    int enumCount();
    QString enumStr(int v);
    int valueNormalInt();

public slots:
    void setValueNormal(FRP::ResistenceModel vv, bool emitAuto = true );
    void setValueNormal(const QString &vv, bool emitAuto = true );
    void setValueInt(int i, bool emitAuto = true );
    void setValue(FRP::ResistenceModel vv, bool emitAuto = true );
    void setValue(const QString &vv, bool emitAuto = true );

private:
    FRPResistenceModelPrivate * m_d;
};

class FRPLoadModePrivate;

class FRPLoadMode: public EnumPlus {
    Q_OBJECT
public:
    FRPLoadMode( FRP::LoadMode vv,
                  const QString & nn,
                  bool ro = false );

    FRPLoadMode(FRPLoadMode &val);
    ~FRPLoadMode();
    QString typeVar();
    FRPLoadMode &operator =(FRPLoadMode &val);
    VarPlus &operator =(VarPlus &val);
    FRP::LoadMode value();
    FRP::LoadMode valueNormal();
    QString valueNormalStr();
    QString valueStr();
    int enumCount();
    QString enumStr(int v);
    int valueNormalInt();

public slots:
    void setValueNormal(FRP::LoadMode vv, bool emitAuto = true );
    void setValueNormal(const QString &vv, bool emitAuto = true );
    void setValueInt(int i, bool emitAuto = true );
    void setValue(FRP::LoadMode vv, bool emitAuto = true );
    void setValue(const QString &vv, bool emitAuto = true );

private:
    FRPLoadModePrivate * m_d;
};

#endif // FRP_H
