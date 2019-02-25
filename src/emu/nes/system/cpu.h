//
// Created by Forrest Jones on 2019-02-24.
//

#ifndef TERMU_CPU_H
#define TERMU_CPU_H

#include "../../../utils/utils.h"
#include "ppu.h"
#include "mem.h"

class cpu {
    typedef void (cpu::*instr_func)(uint16_t addr);

    typedef uint16_t (cpu::*addr_func)(bool writes);

    class instruction {
        friend class cpu;

    private:
        instr_func instr;
        addr_func addressing_mode;

        bool writes;
        bool official;

    public:
        instruction();

        instruction(instr_func instr, addr_func addressing_mode, bool writes, bool official);
    };

    std::unique_ptr<spdlog::logger> log = utils::get_logger("cpu");

    // Registers
    uint16_t PC;
    uint8_t S;
    uint8_t A;
    uint8_t X;
    uint8_t Y;

    // Flags
    bool CF;
    bool ZF;
    bool IF;
    bool DF;
    bool BF;
    bool VF;
    bool NF;

    ppu *ppu;
    mem *mem;
    cpu::instruction instructions[256];

    bool running;
    uint64_t current_cycle;

    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point benchmark_time;
    uint64_t benchmark_cycles;

public:
    explicit cpu(::ppu *ppu, ::mem *mem);

    void start();

    void stop();

private:

    void run();

    /**
     * Cycles the CPU. Will block until the cycle lasted long enough.
     */
    void cycle(uint16_t addr, bool write);

    // region Instruction Creators

    instruction r_instr(instr_func instruction);

    instruction r_instr(instr_func instruction, bool official);

    instruction r_instr(instr_func instruction, addr_func mode);

    instruction r_instr(instr_func instruction, addr_func mode, bool official);

    instruction w_instr(instr_func instruction);

    instruction w_instr(instr_func instruction, addr_func mode);

    instruction w_instr(instr_func instruction, addr_func mode, bool official);

    instruction instr(instr_func instruction, addr_func mode, bool writes, bool official);

    // endregion

    void setup_instructions();

    // region Addressing Modes

    uint16_t accumulator(bool writes);

    uint16_t implied(bool writes);

    uint16_t immediate(bool writes);

    uint16_t absolute(bool writes);

    uint16_t absolute_jsr(bool writes);

    uint16_t absolute_x(bool writes);

    uint16_t absolute_y(bool writes);

    uint16_t zero_page(bool writes);

    uint16_t zero_page_x(bool writes);

    uint16_t zero_page_y(bool writes);

    uint16_t relative(bool writes);

    uint16_t indirect(bool writes);

    uint16_t indirect_x(bool writes);

    uint16_t indirect_y(bool writes);

    // endregion

    // region Instruction Helpers

    void set_byte(uint16_t addr, uint8_t b);

    uint8_t get_byte(uint16_t addr);

    uint16_t get_short(uint16_t addr);

    uint16_t get_paged_short(uint16_t addr);

    void set_zn(uint8_t b);

    void branch(bool branch, uint16_t addr);

    void compare(uint8_t a, uint8_t b);

    void accumulator_instr(bool cf, uint8_t b_old, uint8_t b_new, uint16_t addr);

    void push_byte(uint8_t b);

    void push_short(uint16_t s);

    uint8_t pop_byte();

    uint8_t pop_byte(bool cycle);

    uint16_t pop_short();

    uint16_t pop_short(bool cycle);

    uint8_t get_status();

    void set_status(uint8_t b);

    // endregion

    // region Official Instructions

    /**
     * Add with Carry
     */
    void adc(uint16_t addr);

    /**
     * Add with Carry op
     */
    void adc(uint8_t op);

    /**
     * Logical AND
     */
    void _and(uint16_t addr);

    /**
     * Arithmetic Shift Left
     */
    void asl(uint16_t addr);

    /**
     * Branch if Carry Clear
     */
    void bcc(uint16_t addr);

    /**
     * Branch if Carry Set
     */
    void bcs(uint16_t addr);

    /**
     * Branch if Equal
     */
    void beq(uint16_t addr);

    /**
     * Bit Test
     */
    void bit(uint16_t addr);

    /**
     * Branch if Minus
     */
    void bmi(uint16_t addr);

    /**
     * Branch if Not Equal
     */
    void bne(uint16_t addr);

    /**
     * Branch if Positive
     */
    void bpl(uint16_t addr);

    /**
     * Force Interrupt
     */
    void brk(uint16_t addr);

    /**
     * Branch if Overflow Clear
     */
    void bvc(uint16_t addr);

    /**
     * Branch if Overflow Set
     */
    void bvs(uint16_t addr);

    /**
     * Clear Carry Flag
     */
    void clc(uint16_t addr);

    /**
     * Clear Decimal Mode
     */
    void cld(uint16_t addr);

    /**
     * Clear Interrupt Disable
     */
    void cli(uint16_t addr);

    /**
     * Clear Overflow Flag
     */
    void clv(uint16_t addr);

    /**
     * Compare
     */
    void cmp(uint16_t addr);

    /**
     * Compare op
     */
    void cmp(uint8_t op);

    /**
     * Compare X Register
     */
    void cpx(uint16_t addr);

    /**
     * Compare Y Register
     */
    void cpy(uint16_t addr);

    /**
     * Decrement Memory
     */
    void dec(uint16_t addr);

    /**
     * Decrement X Register
     */
    void dex(uint16_t addr);

    /**
     * Decrement Y Register
     */
    void dey(uint16_t addr);

    /**
     * Exclusive OR
     */
    void eor(uint16_t addr);

    /**
     * Increment Memory
     */
    void inc(uint16_t addr);

    /**
     * Increment X Register
     */
    void inx(uint16_t addr);

    /**
     * Increment Y Register
     */
    void iny(uint16_t addr);

    /**
     * Jump
     */
    void jmp(uint16_t addr);

    /**
     * Jump to Subroutine
     */
    void jsr(uint16_t addr);

    /**
     * Load Accumulator
     */
    void lda(uint16_t addr);

    /**
     * Load X Register
     */
    void ldx(uint16_t addr);

    /**
     * Load Y Register
     */
    void ldy(uint16_t addr);

    /**
     * Logical Shift Right
     */
    void lsr(uint16_t addr);

    /**
     * No Operation
     */
    void nop(uint16_t addr);

    /**
     * Logical Inclusive OR
     */
    void ora(uint16_t addr);

    /**
     * Push Accumulator
     */
    void pha(uint16_t addr);

    /**
     * Push Processor Status
     */
    void php(uint16_t addr);

    /**
     * Pull Accumulator
     */
    void pla(uint16_t addr);

    /**
     * Pull Processor Status
     */
    void plp(uint16_t addr);

    /**
     * Rotate Left
     */
    void rol(uint16_t addr);

    /**
     * Rotate Right
     */
    void ror(uint16_t addr);

    /**
     * Return from Interrupt
     */
    void rti(uint16_t addr);

    /**
     * Return from Subroutine
     */
    void rts(uint16_t addr);

    /**
     * Subtract with Carry
     */
    void sbc(uint16_t addr);

    /**
     * Subtract with Carry op
     */
    void sbc(uint8_t op);

    /**
     * Set Carry Flag
     */
    void sec(uint16_t addr);

    /**
     * Set Decimal Flag
     */
    void sed(uint16_t addr);

    /**
     * Set Interrupt Disable
     */
    void sei(uint16_t addr);

    /**
     * Store Accumulator
     */
    void sta(uint16_t addr);

    /**
     * Store X Register
     */
    void stx(uint16_t addr);

    /**
     * Store Y Register
     */
    void sty(uint16_t addr);

    /**
     * Transfer Accumulator to X
     */
    void tax(uint16_t addr);

    /**
     * Transfer Accumulator to Y
     */
    void tay(uint16_t addr);

    /**
     * Transfer Stack Pointer to X
     */
    void tsx(uint16_t addr);

    /**
     * Transfer X to Accumulator
     */
    void txa(uint16_t addr);

    /**
     * Transfer X to Stack Pointer
     */
    void txs(uint16_t addr);

    /**
     * Transfer Y to Accumulator
     */
    void tya(uint16_t addr);

    // endregion

    // region Unofficial Instructions

    // TODO: Test

    /**
     * AND then LSR
     */
    void alr(uint16_t addr);

    /**
     * AND with Carry
     */
    void anc(uint16_t addr);

    /**
     * AND then ROR with Overflow
     */
    void arr(uint16_t addr);

    /**
     * AND A to X then subtract Immediate
     */
    void axs(uint16_t addr);

    /**
     * DEC then CMP
     */
    void dcp(uint16_t addr);

    /**
     * INC then SBC
     */
    void isc(uint16_t addr);

    /**
     * AND with Stack to Stack and X and Accumulator
     */
    void las(uint16_t addr);

    /**
     * Load Accumulator and X Register
     */
    void lax(uint16_t addr);

    /**
     * ROL then AND
     */
    void rla(uint16_t addr);

    /**
     * ROR then ADC
     */
    void rra(uint16_t addr);

    /**
     * Store AND X
     */
    void sax(uint16_t addr);

    /**
     * Store A & X & (ADDR_HI + 1)
     */
    void sha(uint16_t addr);

    /**
     * SHA and TXS, where X is replaced by (A & X)
     */
    void shs(uint16_t addr);

    /**
     * Store X & (ADDR_HI + 1)
     */
    void shx(uint16_t addr);

    /**
     * Store Y & (ADDR_HI + 1)
     */
    void shy(uint16_t addr);

    /**
     * NOP with Read
     */
    void skb(uint16_t addr);

    /**
     * ASL then ORA
     */
    void slo(uint16_t addr);

    /**
     * LSR then EOR
     */
    void sre(uint16_t addr);

    /**
     * Stop the CPU
     */
    void stp(uint16_t addr);

    /**
     * Does some weird magic stuff apparently that I'm too dumb to understand
     */
    void xaa(uint16_t addr);

    // endregion
};

#endif //TERMU_CPU_H
