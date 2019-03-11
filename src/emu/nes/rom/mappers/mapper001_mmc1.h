//
// Created by Forrest Jones on 2019-02-23.
//

#ifndef TERMU_MAPPER001_MMC1_H
#define TERMU_MAPPER001_MMC1_H

#include "mapper.h"

class mapper001_mmc1 : public mapper {
    enum prg_bank_mode {
        switch_32k,
        switch_last_16k,
        switch_first_16k
    };
    
    enum chr_bank_mode {
        switch_8k,
        switch_4k,
    };
    
    mapper001_mmc1::prg_bank_mode prg_bank_mode;
    mapper001_mmc1::chr_bank_mode chr_bank_mode;

    uint8_t *chr;
    uint8_t prg_bank;
    uint8_t chr_bank_0;
    uint8_t chr_bank_1;

    uint8_t shift;
    uint8_t shift_amount;

    uint64_t last_write_cycle;

    uint8_t &map_prg(uint16_t addr, bool write) override;
    uint8_t &map_chr(uint16_t addr, bool write) override;

    void write_control(uint8_t b);
    void write_chr_bank_0(uint8_t b);
    void write_chr_bank_1(uint8_t b);
    void write_prg_bank(uint8_t b);
    
public:
    explicit mapper001_mmc1(nes_rom *rom);

    void set_prg(uint16_t addr, uint8_t b) override;
};

#endif //TERMU_MAPPER001_MMC1_H
