#ifndef SECTION_DEF_H
#define SECTION_DEF_H

namespace SectionSpace {
/** Lista dei tipi di sezioni contenute nel modello
 * Aggiungere qui le nuove tipologie di sezioni. */
enum SectionType{
    GenericSection,
    SteelCncrSection,
    CncrSection,
    RCncrSection,
    SteelSection,
    TimberSection,
    ThinSection,
    ArcSection,
    LineSection,
    ThinSectionCont,
    ThinSectionContSteel,
    ProfileSection,
    PointsSection
};
}

#endif // SECTION_DEF_H
