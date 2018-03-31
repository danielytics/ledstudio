#ifndef PHYSICALLEDSTRIP_H_
#define PHYSICALLEDSTRIP_H_

#include "LedStrip.h"

class PhysicalLedStrip : public LedStrip {
public:
	PhysicalLedStrip();
    ~PhysicalLedStrip();

    void init();
    void term();
    void reset();
    void refresh(const std::vector<NativeColor>& pixels);
};

#endif
