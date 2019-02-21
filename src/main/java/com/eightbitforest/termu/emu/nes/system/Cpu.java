package main.java.com.eightbitforest.termu.emu.nes.system;

import main.java.com.eightbitforest.termu.emu.core.exceptions.EmuException;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.StandardOpenOption;

class Cpu {
    // Registers
    private int PC;
    private byte S;
    private byte A;
    private byte X;
    private byte Y;

    // Flags
    private byte CF;
    private byte ZF;
    private byte IF;
    private byte DF;
    private byte BF;
    private byte VF;
    private byte NF;

    private short nmiVector;
    private short resetVector;
    private short irqVector;

    private Mem mem;
    private Instruction[] instructions;

    private int currentCycle;
    private int cyclesLeft;

    Cpu(Mem mem) {
        this.mem = mem;
        instructions = new Instruction[0xff];
        setupInstructions();
    }

    void start() {
//        nmiVector = mem.getShort(0xfffa);
//        resetVector = mem.getShort(0xfffc);
//        irqVector = mem.getShort(0xfffe);

        // TODO: Set PC to reset vector. Using 0xC000 since PPU is not implemented yet
        PC = 0xc000;
        S = (byte) 0xfd;

        CF = 0;
        ZF = 0;
        IF = 1;
        DF = 0;
        BF = 0;
        VF = 0;
        NF = 0;

        currentCycle = 0;

        try {
            Files.write(new File("termu.log").toPath(), new byte[]{});
        } catch (IOException e) {
            e.printStackTrace();
        }

        run();
    }

    private void run() {
        while (true) {
            // TODO: Remove/proper debugging
            try {
                Files.write(new File("termu.log").toPath(), String.format("%04x                                            A:%02x X:%02x Y:%02x P:%02x SP:%02x CPUC:%d\n",
                        PC,
                        A, X, Y, getStatus(), S, currentCycle).toUpperCase().getBytes(), StandardOpenOption.APPEND);
            } catch (IOException e) {
                e.printStackTrace();
            }

            byte opCode = getByte(PC);
            Instruction instruction = instructions[btoi(opCode)];
            if (instruction == null)
                throw new EmuException(String.format("Invalid opcode: %02x.", opCode));

            PC++;

            int addr = instruction.addressingMode.call(instruction.writes);
            instruction.instruction.call(addr);
        }
    }

    /**
     * Cycles the CPU. Will block until the cycle lasted long enough.
     */
    private void cycle(int addr, boolean write) {
        // TODO: Clock rate.
        // TODO: Other cycle things
        try {
            Files.write(new File("termu.log").toPath(), String.format("      %s     $%04x\n",
                    write ? "WRITE" : "READ ", addr).toUpperCase().getBytes(), StandardOpenOption.APPEND);
        } catch (IOException e) {
            e.printStackTrace();
        }

        currentCycle++;
    }

    private Instruction rInstr(IInstruction instruction) {
        return instr(instruction, this::implied, false, true);
    }

    private Instruction rInstr(IInstruction instruction, IAddressingMode mode) {
        return instr(instruction, mode, false, true);
    }

    private Instruction wInstr(IInstruction instruction) {
        return instr(instruction, this::implied, true, true);
    }

    private Instruction wInstr(IInstruction instruction, IAddressingMode mode) {
        return instr(instruction, mode, true, true);
    }

    private Instruction instr(IInstruction instruction, IAddressingMode mode, boolean writes, boolean official) {
        return new Instruction(instruction, mode, writes, official);
    }

    // TODO: Make all write instrs wInstr
    private void setupInstructions() {
        instructions[0x00] = rInstr(this::brk);
        instructions[0x01] = rInstr(this::ora, this::indirectX);
        instructions[0x05] = rInstr(this::ora, this::zeroPage);
        instructions[0x06] = rInstr(this::asl, this::zeroPage);
        instructions[0x08] = rInstr(this::php);
        instructions[0x09] = rInstr(this::ora, this::immediate);
        instructions[0x0a] = rInstr(this::asl, this::accumulator);
        instructions[0x0d] = rInstr(this::ora, this::absolute);
        instructions[0x0e] = rInstr(this::asl, this::absolute);

        instructions[0x10] = rInstr(this::bpl, this::relative);
        instructions[0x11] = rInstr(this::ora, this::indirectY);
        instructions[0x15] = rInstr(this::ora, this::zeroPageX);
        instructions[0x16] = rInstr(this::asl, this::zeroPageX);
        instructions[0x18] = rInstr(this::clc);
        instructions[0x19] = rInstr(this::ora, this::absoluteY);
        instructions[0x1d] = rInstr(this::ora, this::absoluteX);
        instructions[0x1e] = wInstr(this::asl, this::absoluteX);

        instructions[0x20] = rInstr(this::jsr, this::absoluteJsr);
        instructions[0x21] = rInstr(this::and, this::indirectX);
        instructions[0x24] = rInstr(this::bit, this::zeroPage);
        instructions[0x25] = rInstr(this::and, this::zeroPage);
        instructions[0x26] = rInstr(this::rol, this::zeroPage);
        instructions[0x28] = rInstr(this::plp);
        instructions[0x29] = rInstr(this::and, this::immediate);
        instructions[0x2a] = rInstr(this::rol, this::accumulator);
        instructions[0x2c] = rInstr(this::bit, this::absolute);
        instructions[0x2d] = rInstr(this::and, this::absolute);
        instructions[0x2e] = rInstr(this::rol, this::absolute);

        instructions[0x30] = rInstr(this::bmi, this::relative);
        instructions[0x31] = rInstr(this::and, this::indirectY);
        instructions[0x35] = rInstr(this::and, this::zeroPageX);
        instructions[0x36] = rInstr(this::rol, this::zeroPageX);
        instructions[0x38] = rInstr(this::sec);
        instructions[0x39] = rInstr(this::and, this::absoluteY);
        instructions[0x3d] = rInstr(this::and, this::absoluteX);
        instructions[0x3e] = wInstr(this::rol, this::absoluteX);

        instructions[0x40] = rInstr(this::rti);
        instructions[0x41] = rInstr(this::eor, this::indirectX);
        instructions[0x45] = rInstr(this::eor, this::zeroPage);
        instructions[0x46] = rInstr(this::lsr, this::zeroPage);
        instructions[0x48] = rInstr(this::pha);
        instructions[0x49] = rInstr(this::eor, this::immediate);
        instructions[0x4a] = rInstr(this::lsr, this::accumulator);
        instructions[0x4c] = rInstr(this::jmp, this::absolute);
        instructions[0x4d] = rInstr(this::eor, this::absolute);
        instructions[0x4e] = rInstr(this::lsr, this::absolute);

        instructions[0x50] = rInstr(this::bvc, this::relative);
        instructions[0x51] = rInstr(this::eor, this::indirectY);
        instructions[0x55] = rInstr(this::eor, this::zeroPageX);
        instructions[0x56] = rInstr(this::lsr, this::zeroPageX);
        instructions[0x58] = rInstr(this::cli);
        instructions[0x59] = rInstr(this::eor, this::absoluteY);
        instructions[0x5d] = rInstr(this::eor, this::absoluteX);
        instructions[0x5e] = wInstr(this::lsr, this::absoluteX);

        instructions[0x60] = rInstr(this::rts);
        instructions[0x61] = rInstr(this::adc, this::indirectX);
        instructions[0x65] = rInstr(this::adc, this::zeroPage);
        instructions[0x66] = rInstr(this::ror, this::zeroPage);
        instructions[0x68] = rInstr(this::pla);
        instructions[0x69] = rInstr(this::adc, this::immediate);
        instructions[0x6a] = rInstr(this::ror, this::accumulator);
        instructions[0x6c] = rInstr(this::jmp, this::indirect);
        instructions[0x6d] = rInstr(this::adc, this::absolute);
        instructions[0x6e] = rInstr(this::ror, this::absolute);

        instructions[0x70] = rInstr(this::bvs, this::relative);
        instructions[0x71] = rInstr(this::adc, this::indirectY);
        instructions[0x75] = rInstr(this::adc, this::zeroPageX);
        instructions[0x76] = rInstr(this::ror, this::zeroPageX);
        instructions[0x78] = rInstr(this::sei);
        instructions[0x79] = rInstr(this::adc, this::absoluteY);
        instructions[0x7d] = rInstr(this::adc, this::absoluteX);
        instructions[0x7e] = wInstr(this::ror, this::absoluteX);

        instructions[0x81] = rInstr(this::sta, this::indirectX);
        instructions[0x84] = rInstr(this::sty, this::zeroPage);
        instructions[0x85] = rInstr(this::sta, this::zeroPage);
        instructions[0x86] = rInstr(this::stx, this::zeroPage);
        instructions[0x88] = rInstr(this::dey);
        instructions[0x8a] = rInstr(this::txa);
        instructions[0x8c] = rInstr(this::sty, this::absolute);
        instructions[0x8d] = rInstr(this::sta, this::absolute);
        instructions[0x8e] = rInstr(this::stx, this::absolute);

        instructions[0x90] = rInstr(this::bcc, this::relative);
        instructions[0x91] = wInstr(this::sta, this::indirectY);
        instructions[0x94] = rInstr(this::sty, this::zeroPageX);
        instructions[0x95] = rInstr(this::sta, this::zeroPageX);
        instructions[0x96] = rInstr(this::stx, this::zeroPageY);
        instructions[0x98] = rInstr(this::tya);
        instructions[0x99] = wInstr(this::sta, this::absoluteY);
        instructions[0x9a] = rInstr(this::txs);
        instructions[0x9d] = wInstr(this::sta, this::absoluteX);

        instructions[0xa0] = rInstr(this::ldy, this::immediate);
        instructions[0xa1] = rInstr(this::lda, this::indirectX);
        instructions[0xa2] = rInstr(this::ldx, this::immediate);
        instructions[0xa4] = rInstr(this::ldy, this::zeroPage);
        instructions[0xa5] = rInstr(this::lda, this::zeroPage);
        instructions[0xa6] = rInstr(this::ldx, this::zeroPage);
        instructions[0xa8] = rInstr(this::tay);
        instructions[0xa9] = rInstr(this::lda, this::immediate);
        instructions[0xaa] = rInstr(this::tax);
        instructions[0xac] = rInstr(this::ldy, this::absolute);
        instructions[0xad] = rInstr(this::lda, this::absolute);
        instructions[0xae] = rInstr(this::ldx, this::absolute);

        instructions[0xb0] = rInstr(this::bcs, this::relative);
        instructions[0xb1] = rInstr(this::lda, this::indirectY);
        instructions[0xb4] = rInstr(this::ldy, this::zeroPageX);
        instructions[0xb5] = rInstr(this::lda, this::zeroPageX);
        instructions[0xb6] = rInstr(this::ldx, this::zeroPageY);
        instructions[0xb8] = rInstr(this::clv);
        instructions[0xb9] = rInstr(this::lda, this::absoluteY);
        instructions[0xba] = rInstr(this::tsx);
        instructions[0xbc] = rInstr(this::ldy, this::absoluteX);
        instructions[0xbd] = rInstr(this::lda, this::absoluteX);
        instructions[0xbe] = rInstr(this::ldx, this::absoluteY);

        instructions[0xc0] = rInstr(this::cpy, this::immediate);
        instructions[0xc1] = rInstr(this::cmp, this::indirectX);
        instructions[0xc4] = rInstr(this::cpy, this::zeroPage);
        instructions[0xc5] = rInstr(this::cmp, this::zeroPage);
        instructions[0xc6] = rInstr(this::dec, this::zeroPage);
        instructions[0xc8] = rInstr(this::iny);
        instructions[0xc9] = rInstr(this::cmp, this::immediate);
        instructions[0xca] = rInstr(this::dex);
        instructions[0xcc] = rInstr(this::cpy, this::absolute);
        instructions[0xcd] = rInstr(this::cmp, this::absolute);
        instructions[0xce] = rInstr(this::dec, this::absolute);

        instructions[0xd0] = rInstr(this::bne, this::relative);
        instructions[0xd1] = rInstr(this::cmp, this::indirectY);
        instructions[0xd5] = rInstr(this::cmp, this::zeroPageX);
        instructions[0xd6] = rInstr(this::dec, this::zeroPageX);
        instructions[0xd8] = rInstr(this::cld);
        instructions[0xd9] = rInstr(this::cmp, this::absoluteY);
        instructions[0xdd] = rInstr(this::cmp, this::absoluteX);
        instructions[0xde] = wInstr(this::dec, this::absoluteX);

        instructions[0xe0] = rInstr(this::cpx, this::immediate);
        instructions[0xe1] = rInstr(this::sbc, this::indirectX);
        instructions[0xe4] = rInstr(this::cpx, this::zeroPage);
        instructions[0xe5] = rInstr(this::sbc, this::zeroPage);
        instructions[0xe6] = rInstr(this::inc, this::zeroPage);
        instructions[0xe8] = rInstr(this::inx);
        instructions[0xe9] = rInstr(this::sbc, this::immediate);
        instructions[0xea] = rInstr(this::nop);
        instructions[0xec] = rInstr(this::cpx, this::absolute);
        instructions[0xed] = rInstr(this::sbc, this::absolute);
        instructions[0xee] = rInstr(this::inc, this::absolute);

        instructions[0xf0] = rInstr(this::beq, this::relative);
        instructions[0xf1] = rInstr(this::sbc, this::indirectY);
        instructions[0xf5] = rInstr(this::sbc, this::zeroPageX);
        instructions[0xf6] = rInstr(this::inc, this::zeroPageX);
        instructions[0xf8] = rInstr(this::sed);
        instructions[0xf9] = rInstr(this::sbc, this::absoluteY);
        instructions[0xfd] = rInstr(this::sbc, this::absoluteX);
        instructions[0xfe] = wInstr(this::inc, this::absoluteX);
    }

    // region Addressing Modes

    private int accumulator(boolean writes) {
        getByte(PC);
        return -1;
    }

    private int implied(boolean writes) {
        getByte(PC);
        return -1;
    }

    private int immediate(boolean writes) {
        PC++;
        return PC - 1;
    }

    private int absolute(boolean writes) {
        PC += 2;
        return stoi(getShort(PC - 2));
    }

    private int absoluteJsr(boolean writes) {
        PC++;
        return btoi(getByte(PC - 1));
    }

    private int absoluteX(boolean writes) {
        PC += 2;
        int addr = stoi(getShort(PC - 2));
        if (btoi(addr) + btoi(X) > 0xff || writes)
            getByte((addr & 0xff00) | btoi(btoi(addr) + btoi(X)));
        return stoi(addr + btoi(X));
    }

    private int absoluteY(boolean writes) {
        PC += 2;
        int addr = stoi(getShort(PC - 2));
        if (btoi(addr) + btoi(Y) > 0xff || writes)
            getByte((addr & 0xff00) | btoi(btoi(addr) + btoi(Y)));
        return stoi(addr + btoi(Y));
    }

    private int zeroPage(boolean writes) {
        PC++;
        return btoi(getByte(PC - 1));
    }

    private int zeroPageX(boolean writes) {
        PC++;
        int addr = btoi(getByte(PC - 1));
        getByte(addr);
        return btoi(addr + btoi(X));
    }

    private int zeroPageY(boolean writes) {
        PC++;
        int addr = btoi(getByte(PC - 1));
        getByte(addr);
        return btoi(addr + btoi(Y));
    }

    private int relative(boolean writes) {
        PC++;
        return PC - 1;
    }

    private int indirect(boolean writes) {
        PC += 2;
        return stoi(getPagedShort(stoi(getShort(PC - 2))));
    }

    private int indirectX(boolean writes) {
        PC++;
        int addr = btoi(getByte(PC - 1));
        getByte(addr);
        addr = btoi(addr + btoi(X));
        return stoi(getPagedShort(addr));
    }

    private int indirectY(boolean writes) {
        PC++;
        int addr = btoi(getByte(PC - 1));
        addr = stoi(getPagedShort(addr));
        if (btoi(addr) + btoi(Y) > 0xff || writes)
            getByte((addr & 0xff00) | btoi(btoi(addr) + btoi(Y)));
        return stoi(addr + btoi(Y));
    }

    // endregion

    // region Instruction Helpers

    private int btoi(byte b) {
        return b & 0xff;
    }

    private int btoi(int b) {
        return b & 0xff;
    }

    private int stoi(short s) {
        return s & 0xffff;
    }

    private int stoi(int s) {
        return s & 0xffff;
    }

    private void setByte(int addr, byte b) {
        mem.set(addr, b);
        cycle(addr, true);
    }

    private byte getByte(int addr) {
        byte b = mem.get(addr);
        cycle(addr, false);
        return b;
    }

    private short getShort(int addr) {
        return (short) ((getByte(addr) & 0x00ff) | ((getByte(addr + 1) << 8) & 0xff00));
    }

    private short getPagedShort(int addr) {
        int lo = addr;
        int hi = (addr & 0xff00) | (addr + 1) & 0xff;
        return (short) ((getByte(lo) & 0x00ff) | (getByte(hi) << 8 & 0xff00));
    }

    private void setZN(byte b) {
        ZF = (byte) (b == 0 ? 1 : 0);
        NF = (byte) (b < 0 ? 1 : 0);
    }

    private void branch(boolean branch, int addr) {
        byte offset = getByte(addr);
        if (branch) {
            getByte(PC);
            if (btoi(btoi(PC) + btoi(offset)) > 0xff)
                getByte(PC + 1);
            PC += btoi(offset);
        }
    }

    private int compare(byte a, byte b) {
        int cmp = btoi(a) - btoi(b);
        CF = (byte) (btoi(a) >= btoi(b) ? 1 : 0);
        ZF = (byte) (btoi(a) == btoi(b) ? 1 : 0);
        NF = (byte) ((cmp >>> 7 & 0x01) == 1 ? 1 : 0);
        return 0;
    }

    private int accumulatorInstr(int cf, int bOld, int bNew, int addr) {
        CF = (byte) (cf);

        if (addr == -1) {
            A = (byte) bNew;
        } else {
            setByte(addr, (byte) bOld);
            setByte(addr, (byte) bNew);
        }

        setZN((byte) bNew);
        return 2;
    }

    private void pushByte(byte b) {
        setByte(0x0100 | btoi(S), b);
        S = (byte) (btoi(S) - 1);
    }

    private void pushShort(short s) {
        pushByte((byte) (s >>> 8));
        pushByte((byte) (s & 0xff));
    }

    private byte popByte() {
        return popByte(true);
    }

    private byte popByte(boolean cycle) {
        if (cycle)
            getByte(0x0100 | btoi(S));

        S = (byte) (btoi(S) + 1);
        return getByte(0x0100 | btoi(S));
    }

    private short popShort() {
        return popShort(true);
    }

    private short popShort(boolean cycle) {
        if (cycle)
            getByte(0x0100 | btoi(S));

        byte lo = popByte(false);
        byte hi = popByte(false);
        return (short) ((btoi(hi) << 8) | btoi(lo));
    }

    private byte getStatus() {
        byte status = 0x20;

        if (NF == 1)
            status |= 0x80;
        if (VF == 1)
            status |= 0x40;
        if (BF == 1)
            status |= 0x10;
        if (DF == 1)
            status |= 0x08;
        if (IF == 1)
            status |= 0x04;
        if (ZF == 1)
            status |= 0x02;
        if (CF == 1)
            status |= 0x01;

        return status;
    }

    private void setStatus(byte b) {
        NF = (byte) (b >>> 7 & 0x01);
        VF = (byte) (b >>> 6 & 0x01);
        DF = (byte) (b >>> 3 & 0x01);
        IF = (byte) (b >>> 2 & 0x01);
        ZF = (byte) (b >>> 1 & 0x01);
        CF = (byte) (b & 0x01);
    }

    // endregion

    // region Instructions

    /**
     * Add with Carry
     */
    private void adc(int addr) {
        byte op = getByte(addr);
        int tempA = btoi(A) + btoi(op) + CF;
        int tempASigned = A + op + CF;
        CF = (byte) (tempA > 0xff ? 1 : 0);
        VF = (byte) (tempASigned < -128 || tempASigned > 127 ? 1 : 0);
        A = (byte) tempA;
        setZN(A);
    }

    /**
     * Logical AND
     */
    private void and(int addr) {
        A &= getByte(addr);
        setZN(A);
    }

    /**
     * Arithmetic Shift Left
     */
    private void asl(int addr) {
        byte op = addr == -1 ? A : getByte(addr);
        accumulatorInstr(btoi(op) >>> 7, op, btoi(op) << 1, addr);
    }

    /**
     * Branch if Carry Clear
     */
    private void bcc(int addr) {
        branch(CF == 0, addr);
    }

    /**
     * Branch if Carry Set
     */
    private void bcs(int addr) {
        branch(CF == 1, addr);
    }

    /**
     * Branch if Equal
     */
    private void beq(int addr) {
        branch(ZF == 1, addr);
    }

    /**
     * Bit Test
     */
    private void bit(int addr) {
        byte op = getByte(addr);
        ZF = (byte) ((A & op) == 0 ? 1 : 0);
        VF = (byte) (op >>> 6 & 0x01);
        NF = (byte) (op >>> 7 & 0x01);
    }

    /**
     * Branch if Minus
     */
    private void bmi(int addr) {
        branch(NF == 1, addr);
    }

    /**
     * Branch if Not Equal
     */
    private void bne(int addr) {
        branch(ZF == 0, addr);
    }

    /**
     * Branch if Positive
     */
    private void bpl(int addr) {
        branch(NF == 0, addr);
    }

    /**
     * Force Interrupt
     */
    private void brk(int addr) {
        PC++;
        BF = 1;
        pushShort((short) PC);
        pushByte(getStatus());
        PC = stoi(getShort(0xfffe));
        BF = 0;
        // TODO: actually break in exec
    }

    /**
     * Branch if Overflow Clear
     */
    private void bvc(int addr) {
        branch(VF == 0, addr);
    }

    /**
     * Branch if Overflow Set
     */
    private void bvs(int addr) {
        branch(VF == 1, addr);
    }

    /**
     * Clear Carry Flag
     */
    private void clc(int addr) {
        CF = 0;
    }

    /**
     * Clear Decimal Mode
     */
    private void cld(int addr) {
        DF = 0;
    }

    /**
     * Clear Interrupt Disable
     */
    private void cli(int addr) {
        IF = 0;
    }

    /**
     * Clear Overflow Flag
     */
    private void clv(int addr) {
        VF = 0;
    }

    /**
     * Compare
     */
    private void cmp(int addr) {
        compare(A, getByte(addr));
    }

    /**
     * Compare X Register
     */
    private void cpx(int addr) {
        compare(X, getByte(addr));
    }

    /**
     * Compare Y Register
     */
    private void cpy(int addr) {
        compare(Y, getByte(addr));
    }

    /**
     * Decrement Memory
     */
    private void dec(int addr) {
        byte op = getByte(addr);
        setByte(addr, op);
        op = (byte) (btoi(op) - 1);
        setZN(op);
        setByte(addr, op);
    }

    /**
     * Decrement X Register
     */
    private void dex(int addr) {
        X = (byte) (btoi(X) - 1);
        setZN(X);
    }

    /**
     * Decrement Y Register
     */
    private void dey(int addr) {
        Y = (byte) (btoi(Y) - 1);
        setZN(Y);
    }

    /**
     * Exclusive OR
     */
    private void eor(int addr) {
        A ^= btoi(getByte(addr));
        setZN(A);
    }

    /**
     * Increment Memory
     */
    private void inc(int addr) {
        byte op = getByte(addr);
        setByte(addr, op);
        op = (byte) (btoi(op) + 1);
        setZN(op);
        setByte(addr, op);
    }

    /**
     * Increment X Register
     */
    private void inx(int addr) {
        X = (byte) (btoi(X) + 1);
        setZN(X);
    }

    /**
     * Increment Y Register
     */
    private void iny(int addr) {
        Y = (byte) (btoi(Y) + 1);
        setZN(Y);
    }

    /**
     * Jump
     */
    private void jmp(int addr) {
        PC = addr;
    }

    /**
     * Jump to Subroutine
     */
    private void jsr(int addr) {
        getByte(0x0100 | btoi(S));
        pushShort((short) (PC));
        byte hi = getByte(PC);
        PC = (btoi(hi) << 8) | btoi(addr);
    }

    /**
     * Load Accumulator
     */
    private void lda(int addr) {
        A = getByte(addr);
        setZN(A);
    }

    /**
     * Load X Register
     */
    private void ldx(int addr) {
        X = getByte(addr);
        setZN(X);
    }

    /**
     * Load Y Register
     */
    private void ldy(int addr) {
        Y = getByte(addr);
        setZN(Y);
    }

    /**
     * Logical Shift Right
     */
    private void lsr(int addr) {
        byte op = addr == -1 ? A : getByte(addr);
        accumulatorInstr(op & 0x01, op, btoi(op) >>> 1, addr);
    }

    /**
     * No Operation
     */
    private void nop(int addr) {
    }

    /**
     * Logical Inclusive OR
     */
    private void ora(int addr) {
        A |= btoi(getByte(addr));
        setZN(A);
    }

    /**
     * Push Accumulator
     */
    private void pha(int addr) {
        pushByte(A);
    }

    /**
     * Push Processor Status
     */
    private void php(int addr) {
        BF = 1;
        pushByte(getStatus());
        BF = 0;
    }

    /**
     * Pull Accumulator
     */
    private void pla(int addr) {
        A = popByte();
        setZN(A);
    }

    /**
     * Pull Processor Status
     */
    private void plp(int addr) {
        setStatus(popByte());
    }

    /**
     * Rotate Left
     */
    private void rol(int addr) {
        byte op = addr == -1 ? A : getByte(addr);
        accumulatorInstr(btoi(op) >>> 7, op, btoi(op) << 1 | CF, addr);
    }

    /**
     * Rotate Right
     */
    private void ror(int addr) {
        byte op = addr == -1 ? A : getByte(addr);
        accumulatorInstr(op & 0x01, op, btoi(op) >>> 1 | CF << 7, addr);
    }

    /**
     * Return from Interrupt
     */
    private void rti(int addr) {
        setStatus(popByte());
        PC = stoi(popShort(false));
    }

    /**
     * Return from Subroutine
     */
    private void rts(int addr) {
        PC = stoi(stoi(popShort()));
        getByte(PC);
        PC++;
    }

    /**
     * Subtract with Carry
     */
    private void sbc(int addr) {
        byte op = getByte(addr);
        int tempA = btoi(A) - btoi(op) - (1 - CF);
        int tempASigned = A - op - (1 - CF);
        CF = (byte) (tempA < 0 ? 0 : 1);
        VF = (byte) (tempASigned < -128 || tempASigned > 127 ? 1 : 0);
        A = (byte) tempA;
        setZN(A);
    }

    /**
     * Set Carry Flag
     */
    private void sec(int addr) {
        CF = 1;
    }

    /**
     * Set Decimal Flag
     */
    private void sed(int addr) {
        DF = 1;
    }

    /**
     * Set Interrupt Disable
     */
    private void sei(int addr) {
        IF = 1;
    }

    /**
     * Store Accumulator
     */
    private void sta(int addr) {
        setByte(addr, A);
    }

    /**
     * Store X Register
     */
    private void stx(int addr) {
        setByte(addr, X);
    }

    /**
     * Store Y Register
     */
    private void sty(int addr) {
        setByte(addr, Y);
    }

    /**
     * Transfer Accumulator to X
     */
    private void tax(int addr) {
        X = A;
        setZN(X);
    }

    /**
     * Transfer Accumulator to Y
     */
    private void tay(int addr) {
        Y = A;
        setZN(Y);
    }

    /**
     * Transfer Stack Pointer to X
     */
    private void tsx(int addr) {
        X = S;
        setZN(X);
    }

    /**
     * Transfer X to Accumulator
     */
    private void txa(int addr) {
        A = X;
        setZN(A);
    }

    /**
     * Transfer X to Stack Pointer
     */
    private void txs(int addr) {
        S = X;
    }

    /**
     * Transfer Y to Accumulator
     */
    private void tya(int addr) {
        A = Y;
        setZN(Y);
    }

    // endregion

    private static final class Instruction {
        private IInstruction instruction;
        private IAddressingMode addressingMode;
        private boolean writes;
        private boolean official;

        Instruction(IInstruction instruction, IAddressingMode addressingMode, boolean writes, boolean official) {
            this.instruction = instruction;
            this.addressingMode = addressingMode;
            this.writes = writes;
            this.official = official;
        }
    }

    private interface IAddressingMode {
        /**
         * Calls the addressing mode. Returns the address.
         */
        int call(boolean writes);
    }

    private interface IInstruction {
        /**
         * Calls the instruction.
         */
        void call(int addr);
    }
}
