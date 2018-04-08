#ifndef PILLAR_H
#define PILLAR_H

class UnitMeasure;
class SectionArea;
class Point2D;
template <typename T> class QVector;

class Pillar
{
protected:
    Point2D * m_P;
    SectionArea * m_section;
public:
    Pillar( UnitMeasure * );
    ~Pillar();

    Point2D P();
    void setP( const Point2D & );
};

#endif // PILLAR_H
