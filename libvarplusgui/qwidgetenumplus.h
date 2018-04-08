#ifndef QWIDGETENUMPLUS_H
#define QWIDGETENUMPLUS_H

class EnumPlus;

class QWidgetEnumPlus
{
public:
    QWidgetEnumPlus( EnumPlus * v=0 );

    EnumPlus * var();

protected:
    EnumPlus * m_var;
};

#endif // QWIDGETENUMPLUS_H
