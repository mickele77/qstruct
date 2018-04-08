#ifndef QWIDGETPLUS_H
#define QWIDGETPLUS_H

class VarPlus;

class QWidgetPlus
{
public:
    QWidgetPlus( VarPlus * v=0 );

    virtual void setVar( VarPlus * v);
    VarPlus * var();

protected:
    VarPlus * m_var;
};

#endif // QWIDGETPLUS_H
