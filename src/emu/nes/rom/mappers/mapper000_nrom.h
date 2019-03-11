//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_MAPPER000_NROM_H
#define TERMU_MAPPER000_NROM_H

#include "mapper.h"

class mapper000_nrom : public mapper {
public:
    explicit mapper000_nrom(nes_rom *rom);

    uint8_t &map_prg(uint16_t addr, bool write) override;
    uint8_t &map_chr(uint16_t addr, bool write) override;
};

#endif //TERMU_MAPPER000_NROM_H
