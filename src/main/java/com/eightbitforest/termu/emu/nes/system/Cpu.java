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
    private Runnable[] instructions;

    private int currentCycle;
    private int cyclesLeft;

    Cpu(Mem mem) {
        this.mem = mem;
        instructions = new Runnable[0xff];
        setupInstructions();
    }

    void start() {
        nmiVector = mem.getShort(0xfffa);
        resetVector = mem.getShort(0xfffc);
        irqVector = mem.getShort(0xfffe);

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
        cyclesLeft = 0;

        try {
            Files.write(new File("termu.log").toPath(), new byte[]{});
        } catch (IOException e) {
            e.printStackTrace();
        }

        run();
    }

    private void run() {
        // TODO: Clock rate.
        while (true) {
            if (cyclesLeft == 0) {

                // TODO: Remove/proper debugging
                try {
                    Files.write(new File("termu.log").toPath(), String.format("%04x  %02x %02x %02x  \t\tA:%02x X:%02x Y:%02x P:%02x SP:%02x CYC:%3d\n",
                            PC,
                            mem.get(PC),
                            mem.get(PC + 1),
                            mem.get(PC + 2),
                            A, X, Y, getStatus(), S, currentCycle).toUpperCase().getBytes(), StandardOpenOption.APPEND);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                byte opCode = mem.get(PC);
                Runnable instruction = instructions[btoi(opCode)];

                if (instruction == null)
                    throw new EmuException(String.format("Invalid opcode: %02x.", opCode));

                instruction.run();
                currentCycle += cyclesLeft;
            } else {
                cyclesLeft--;
            }

//            if (currentCycle > 15000)
//                break;
        }
    }

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

    private void exec(IInstruction instruction, AddressingMode mode, boolean official) {
        int addr = -1;

        if (!official) {
            // TODO: Something else...
            System.out.println("Running unofficial instruction!");
        }

        switch (mode) {
            case Accumulator:
                cyclesLeft = 0;
                PC++;
                break;
            case Immediate:
                addr = PC + 1;
                cyclesLeft = 2;
                PC += 2;
                break;
            case ZeroPage:
                addr = btoi(mem.get(PC + 1));
                cyclesLeft = 3;
                PC += 2;
                break;
            case ZeroPageX:
                addr = btoi(btoi(X) + btoi(mem.get(PC + 1)));
                cyclesLeft = 4;
                PC += 2;
                break;
            case ZeroPageY:
                addr = btoi(btoi(Y) + btoi(mem.get(PC + 1)));
                cyclesLeft = 4;
                PC += 2;
                break;
            case Absolute:
                addr = stoi(mem.getShort(PC + 1));
                cyclesLeft = 4;
                PC += 3;
                break;
            case AbsoluteX:
                addr = stoi(btoi(X) + stoi(mem.getShort(PC + 1)));
                cyclesLeft = 4 + (btoi(X) + btoi(mem.get(PC + 1)) > 0xff ? 1 : 0);
                PC += 3;
                break;
            case AbsoluteXStore:
                addr = stoi(btoi(X) + stoi(mem.getShort(PC + 1)));
                cyclesLeft = 5;
                PC += 3;
                break;
            case AbsoluteY:
                addr = stoi(btoi(Y) + stoi(mem.getShort(PC + 1)));
                cyclesLeft = 4 + (btoi(Y) + btoi(mem.get(PC + 1)) > 0xff ? 1 : 0);
                PC += 3;
                break;
            case AbsoluteYStore:
                addr = stoi(btoi(Y) + stoi(mem.getShort(PC + 1)));
                cyclesLeft = 5;
                PC += 3;
                break;
            case Indirect:
                addr = stoi(mem.getPagedShort(stoi(mem.getShort(PC + 1))));
                cyclesLeft = 6;
                PC += 3;
                break;
            case IndirectX:
                addr = stoi(mem.getPagedShort(btoi(btoi(X) + btoi(mem.get(PC + 1)))));
                cyclesLeft = 6;
                PC += 2;
                break;
            case IndirectY:
                addr = stoi(btoi(Y) + stoi(mem.getPagedShort(btoi(mem.get(PC + 1)))));
                cyclesLeft = 5 + (btoi(Y) + btoi(mem.getPagedShort(btoi(mem.get(PC + 1)))) > 0xff ? 1 : 0);
                PC += 2;
                break;
            case IndirectYStore:
                addr = stoi(btoi(Y) + stoi(mem.getPagedShort(btoi(mem.get(PC + 1)))));
                cyclesLeft = 6;
                PC += 2;
                break;
            case Relative:
                addr = PC + 1;
                int i = btoi(PC) + mem.get(PC + 1);
                cyclesLeft = 2 + (i > 127 || i < -128 ? 2 : 0);
                PC += 2;
                break;
            case Implied:
                cyclesLeft = 2;
                PC++;
                break;
        }

        cyclesLeft += instruction.call(addr == -1 ? A : mem.get(addr), addr);
    }

    private void setZN(byte b) {
        ZF = (byte) (b == 0 ? 1 : 0);
        NF = (byte) (b < 0 ? 1 : 0);
    }

    private int branch(boolean branch, byte offset) {
        if (branch) {
            PC += offset;
            return 1;
        }

        return 0;
    }

    private int compare(byte a, byte b) {
        int cmp = btoi(a) - btoi(b);
        CF = (byte) (btoi(a) >= btoi(b) ? 1 : 0);
        ZF = (byte) (btoi(a) == btoi(b) ? 1 : 0);
        NF = (byte) ((cmp >>> 7 & 0x01) == 1 ? 1 : 0);
        return 0;
    }

    private int accumulator(int cf, int b, int addr) {
        CF = (byte) (cf);

        if (addr == -1) {
            A = (byte) b;
        } else {
            mem.set(addr, (byte) b);
        }

        setZN((byte) b);
        return 2;
    }

    private void pushByte(byte b) {
        mem.set(0x0100 | btoi(S), b);
        S = (byte) (btoi(S) - 1);
    }

    private void pushShort(short s) {
        pushByte((byte) (s >>> 8));
        pushByte((byte) (s & 0xff));
    }

    private byte popByte() {
        S = (byte) (btoi(S) + 1);
        return mem.get(0x0100 | btoi(S));
    }

    private short popShort() {
        byte lo = popByte();
        byte hi = popByte();
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

    private Runnable instr(IInstruction instruction) {
        return instr(instruction, AddressingMode.Implied, true);
    }

    private Runnable instr(IInstruction instruction, boolean official) {
        return instr(instruction, AddressingMode.Implied, official);
    }

    private Runnable instr(IInstruction instruction, AddressingMode mode) {
        return instr(instruction, mode, true);
    }

    private Runnable instr(IInstruction instruction, AddressingMode mode, boolean official) {
        return () -> exec(instruction, mode, official);
    }

    private void setupInstructions() {
        instructions[0x00] = instr(this::brk);
        instructions[0x01] = instr(this::ora, AddressingMode.IndirectX);
        instructions[0x05] = instr(this::ora, AddressingMode.ZeroPage);
        instructions[0x06] = instr(this::asl, AddressingMode.ZeroPage);
        instructions[0x08] = instr(this::php);
        instructions[0x09] = instr(this::ora, AddressingMode.Immediate);
        instructions[0x0a] = instr(this::asl, AddressingMode.Accumulator);
        instructions[0x0d] = instr(this::ora, AddressingMode.Absolute);
        instructions[0x0e] = instr(this::asl, AddressingMode.Absolute);

        instructions[0x10] = instr(this::bpl, AddressingMode.Relative);
        instructions[0x11] = instr(this::ora, AddressingMode.IndirectY);
        instructions[0x15] = instr(this::ora, AddressingMode.ZeroPageX);
        instructions[0x16] = instr(this::asl, AddressingMode.ZeroPageX);
        instructions[0x18] = instr(this::clc);
        instructions[0x19] = instr(this::ora, AddressingMode.AbsoluteY);
        instructions[0x1d] = instr(this::ora, AddressingMode.AbsoluteX);
        instructions[0x1e] = instr(this::asl, AddressingMode.AbsoluteXStore);

        instructions[0x20] = instr(this::jsr, AddressingMode.Absolute);
        instructions[0x21] = instr(this::and, AddressingMode.IndirectX);
        instructions[0x24] = instr(this::bit, AddressingMode.ZeroPage);
        instructions[0x25] = instr(this::and, AddressingMode.ZeroPage);
        instructions[0x26] = instr(this::rol, AddressingMode.ZeroPage);
        instructions[0x28] = instr(this::plp);
        instructions[0x29] = instr(this::and, AddressingMode.Immediate);
        instructions[0x2a] = instr(this::rol, AddressingMode.Accumulator);
        instructions[0x2c] = instr(this::bit, AddressingMode.Absolute);
        instructions[0x2d] = instr(this::and, AddressingMode.Absolute);
        instructions[0x2e] = instr(this::rol, AddressingMode.Absolute);

        instructions[0x30] = instr(this::bmi, AddressingMode.Relative);
        instructions[0x31] = instr(this::and, AddressingMode.IndirectY);
        instructions[0x35] = instr(this::and, AddressingMode.ZeroPageX);
        instructions[0x36] = instr(this::rol, AddressingMode.ZeroPageX);
        instructions[0x38] = instr(this::sec);
        instructions[0x39] = instr(this::and, AddressingMode.AbsoluteY);
        instructions[0x3d] = instr(this::and, AddressingMode.AbsoluteX);
        instructions[0x3e] = instr(this::rol, AddressingMode.AbsoluteXStore);

        instructions[0x40] = instr(this::rti);
        instructions[0x41] = instr(this::eor, AddressingMode.IndirectX);
        instructions[0x45] = instr(this::eor, AddressingMode.ZeroPage);
        instructions[0x46] = instr(this::lsr, AddressingMode.ZeroPage);
        instructions[0x48] = instr(this::pha);
        instructions[0x49] = instr(this::eor, AddressingMode.Immediate);
        instructions[0x4a] = instr(this::lsr, AddressingMode.Accumulator);
        instructions[0x4c] = instr(this::jmp, AddressingMode.Absolute);
        instructions[0x4d] = instr(this::eor, AddressingMode.Absolute);
        instructions[0x4e] = instr(this::lsr, AddressingMode.Absolute);

        instructions[0x50] = instr(this::bvc, AddressingMode.Relative);
        instructions[0x51] = instr(this::eor, AddressingMode.IndirectY);
        instructions[0x55] = instr(this::eor, AddressingMode.ZeroPageX);
        instructions[0x56] = instr(this::lsr, AddressingMode.ZeroPageX);
        instructions[0x58] = instr(this::cli);
        instructions[0x59] = instr(this::eor, AddressingMode.AbsoluteY);
        instructions[0x5d] = instr(this::eor, AddressingMode.AbsoluteX);
        instructions[0x5e] = instr(this::lsr, AddressingMode.AbsoluteXStore);

        instructions[0x60] = instr(this::rts);
        instructions[0x61] = instr(this::adc, AddressingMode.IndirectX);
        instructions[0x65] = instr(this::adc, AddressingMode.ZeroPage);
        instructions[0x66] = instr(this::ror, AddressingMode.ZeroPage);
        instructions[0x68] = instr(this::pla);
        instructions[0x69] = instr(this::adc, AddressingMode.Immediate);
        instructions[0x6a] = instr(this::ror, AddressingMode.Accumulator);
        instructions[0x6c] = instr(this::jmp, AddressingMode.Indirect);
        instructions[0x6d] = instr(this::adc, AddressingMode.Absolute);
        instructions[0x6e] = instr(this::ror, AddressingMode.Absolute);

        instructions[0x70] = instr(this::bvs, AddressingMode.Relative);
        instructions[0x71] = instr(this::adc, AddressingMode.IndirectY);
        instructions[0x75] = instr(this::adc, AddressingMode.ZeroPageX);
        instructions[0x76] = instr(this::ror, AddressingMode.ZeroPageX);
        instructions[0x78] = instr(this::sei);
        instructions[0x79] = instr(this::adc, AddressingMode.AbsoluteY);
        instructions[0x7d] = instr(this::adc, AddressingMode.AbsoluteX);
        instructions[0x7e] = instr(this::ror, AddressingMode.AbsoluteXStore);

        instructions[0x81] = instr(this::sta, AddressingMode.IndirectX);
        instructions[0x84] = instr(this::sty, AddressingMode.ZeroPage);
        instructions[0x85] = instr(this::sta, AddressingMode.ZeroPage);
        instructions[0x86] = instr(this::stx, AddressingMode.ZeroPage);
        instructions[0x88] = instr(this::dey);
        instructions[0x8a] = instr(this::txa);
        instructions[0x8c] = instr(this::sty, AddressingMode.Absolute);
        instructions[0x8d] = instr(this::sta, AddressingMode.Absolute);
        instructions[0x8e] = instr(this::stx, AddressingMode.Absolute);

        instructions[0x90] = instr(this::bcc, AddressingMode.Relative);
        instructions[0x91] = instr(this::sta, AddressingMode.IndirectYStore);
        instructions[0x94] = instr(this::sty, AddressingMode.ZeroPageX);
        instructions[0x95] = instr(this::sta, AddressingMode.ZeroPageX);
        instructions[0x96] = instr(this::stx, AddressingMode.ZeroPageY);
        instructions[0x98] = instr(this::tya);
        instructions[0x99] = instr(this::sta, AddressingMode.AbsoluteYStore);
        instructions[0x9a] = instr(this::txs);
        instructions[0x9d] = instr(this::sta, AddressingMode.AbsoluteXStore);

        instructions[0xa0] = instr(this::ldy, AddressingMode.Immediate);
        instructions[0xa1] = instr(this::lda, AddressingMode.IndirectX);
        instructions[0xa2] = instr(this::ldx, AddressingMode.Immediate);
        instructions[0xa4] = instr(this::ldy, AddressingMode.ZeroPage);
        instructions[0xa5] = instr(this::lda, AddressingMode.ZeroPage);
        instructions[0xa6] = instr(this::ldx, AddressingMode.ZeroPage);
        instructions[0xa8] = instr(this::tay);
        instructions[0xa9] = instr(this::lda, AddressingMode.Immediate);
        instructions[0xaa] = instr(this::tax);
        instructions[0xac] = instr(this::ldy, AddressingMode.Absolute);
        instructions[0xad] = instr(this::lda, AddressingMode.Absolute);
        instructions[0xae] = instr(this::ldx, AddressingMode.Absolute);

        instructions[0xb0] = instr(this::bcs, AddressingMode.Relative);
        instructions[0xb1] = instr(this::lda, AddressingMode.IndirectY);
        instructions[0xb4] = instr(this::ldy, AddressingMode.ZeroPageX);
        instructions[0xb5] = instr(this::lda, AddressingMode.ZeroPageX);
        instructions[0xb6] = instr(this::ldx, AddressingMode.ZeroPageY);
        instructions[0xb8] = instr(this::clv);
        instructions[0xb9] = instr(this::lda, AddressingMode.AbsoluteY);
        instructions[0xba] = instr(this::tsx);
        instructions[0xbc] = instr(this::ldy, AddressingMode.AbsoluteX);
        instructions[0xbd] = instr(this::lda, AddressingMode.AbsoluteX);
        instructions[0xbe] = instr(this::ldx, AddressingMode.AbsoluteY);

        instructions[0xc0] = instr(this::cpy, AddressingMode.Immediate);
        instructions[0xc1] = instr(this::cmp, AddressingMode.IndirectX);
        instructions[0xc4] = instr(this::cpy, AddressingMode.ZeroPage);
        instructions[0xc5] = instr(this::cmp, AddressingMode.ZeroPage);
        instructions[0xc6] = instr(this::dec, AddressingMode.ZeroPage);
        instructions[0xc8] = instr(this::iny);
        instructions[0xc9] = instr(this::cmp, AddressingMode.Immediate);
        instructions[0xca] = instr(this::dex);
        instructions[0xcc] = instr(this::cpy, AddressingMode.Absolute);
        instructions[0xcd] = instr(this::cmp, AddressingMode.Absolute);
        instructions[0xce] = instr(this::dec, AddressingMode.Absolute);

        instructions[0xd0] = instr(this::bne, AddressingMode.Relative);
        instructions[0xd1] = instr(this::cmp, AddressingMode.IndirectY);
        instructions[0xd5] = instr(this::cmp, AddressingMode.ZeroPageX);
        instructions[0xd6] = instr(this::dec, AddressingMode.ZeroPageX);
        instructions[0xd8] = instr(this::cld);
        instructions[0xd9] = instr(this::cmp, AddressingMode.AbsoluteY);
        instructions[0xdd] = instr(this::cmp, AddressingMode.AbsoluteX);
        instructions[0xde] = instr(this::dec, AddressingMode.AbsoluteXStore);

        instructions[0xe0] = instr(this::cpx, AddressingMode.Immediate);
        instructions[0xe1] = instr(this::sbc, AddressingMode.IndirectX);
        instructions[0xe4] = instr(this::cpx, AddressingMode.ZeroPage);
        instructions[0xe5] = instr(this::sbc, AddressingMode.ZeroPage);
        instructions[0xe6] = instr(this::inc, AddressingMode.ZeroPage);
        instructions[0xe8] = instr(this::inx);
        instructions[0xe9] = instr(this::sbc, AddressingMode.Immediate);
        instructions[0xea] = instr(this::nop);
        instructions[0xec] = instr(this::cpx, AddressingMode.Absolute);
        instructions[0xed] = instr(this::sbc, AddressingMode.Absolute);
        instructions[0xee] = instr(this::inc, AddressingMode.Absolute);

        instructions[0xf0] = instr(this::beq, AddressingMode.Relative);
        instructions[0xf1] = instr(this::sbc, AddressingMode.IndirectY);
        instructions[0xf5] = instr(this::sbc, AddressingMode.ZeroPageX);
        instructions[0xf6] = instr(this::inc, AddressingMode.ZeroPageX);
        instructions[0xf8] = instr(this::sed);
        instructions[0xf9] = instr(this::sbc, AddressingMode.AbsoluteY);
        instructions[0xfd] = instr(this::sbc, AddressingMode.AbsoluteX);
        instructions[0xfe] = instr(this::inc, AddressingMode.AbsoluteXStore);
    }

    /**
     * Add with Carry
     */
    private int adc(byte op, int addr) {
        int tempA = btoi(A) + btoi(op) + CF;
        int tempASigned = A + op + CF;
        CF = (byte) (tempA > 0xff ? 1 : 0);
        VF = (byte) (tempASigned < -128 || tempASigned > 127 ? 1 : 0);
        A = (byte) tempA;
        setZN(A);
        return 0;
    }

    /**
     * Logical AND
     */
    private int and(byte op, int addr) {
        A &= op;
        setZN(A);
        return 0;
    }

    /**
     * Arithmetic Shift Left
     */
    private int asl(byte op, int addr) {
        return accumulator(btoi(op) >>> 7, btoi(op) << 1, addr);
    }

    /**
     * Branch if Carry Clear
     */
    private int bcc(byte op, int addr) {
        return branch(CF == 0, op);
    }

    /**
     * Branch if Carry Set
     */
    private int bcs(byte op, int addr) {
        return branch(CF == 1, op);
    }

    /**
     * Branch if Equal
     */
    private int beq(byte op, int addr) {
        return branch(ZF == 1, op);
    }

    /**
     * Bit Test
     */
    private int bit(byte op, int addr) {
        ZF = (byte) ((A & op) == 0 ? 1 : 0);
        VF = (byte) (op >>> 6 & 0x01);
        NF = (byte) (op >>> 7 & 0x01);
        return 0;
    }

    /**
     * Branch if Minus
     */
    private int bmi(byte op, int addr) {
        return branch(NF == 1, op);
    }

    /**
     * Branch if Not Equal
     */
    private int bne(byte op, int addr) {
        return branch(ZF == 0, op);
    }

    /**
     * Branch if Positive
     */
    private int bpl(byte op, int addr) {
        return branch(NF == 0, op);
    }

    /**
     * Force Interrupt
     */
    private int brk(byte op, int addr) {
        BF = 1;
        pushShort((short) PC);
        pushByte(getStatus());
        PC = stoi(irqVector);
        BF = 0;
        // TODO: actually break in exec
        return 5;
    }

    /**
     * Branch if Overflow Clear
     */
    private int bvc(byte op, int addr) {
        return branch(VF == 0, op);
    }

    /**
     * Branch if Overflow Set
     */
    private int bvs(byte op, int addr) {
        return branch(VF == 1, op);
    }

    /**
     * Clear Carry Flag
     */
    private int clc(byte op, int addr) {
        CF = 0;
        return 0;
    }

    /**
     * Clear Decimal Mode
     */
    private int cld(byte op, int addr) {
        DF = 0;
        return 0;
    }

    /**
     * Clear Interrupt Disable
     */
    private int cli(byte op, int addr) {
        IF = 0;
        return 0;
    }

    /**
     * Clear Overflow Flag
     */
    private int clv(byte op, int addr) {
        VF = 0;
        return 0;
    }

    /**
     * Compare
     */
    private int cmp(byte op, int addr) {
        return compare(A, op);
    }

    /**
     * Compare X Register
     */
    private int cpx(byte op, int addr) {
        return compare(X, op);
    }

    /**
     * Compare Y Register
     */
    private int cpy(byte op, int addr) {
        return compare(Y, op);
    }

    /**
     * Decrement Memory
     */
    private int dec(byte op, int addr) {
        op = (byte) (btoi(op) - 1);
        setZN(op);
        mem.set(addr, op);
        return 2;
    }

    /**
     * Decrement X Register
     */
    private int dex(byte op, int addr) {
        X = (byte) (btoi(X) - 1);
        setZN(X);
        return 0;
    }

    /**
     * Decrement Y Register
     */
    private int dey(byte op, int addr) {
        Y = (byte) (btoi(Y) - 1);
        setZN(Y);
        return 0;
    }

    /**
     * Exclusive OR
     */
    private int eor(byte op, int addr) {
        A ^= op;
        setZN(A);
        return 0;
    }

    /**
     * Increment Memory
     */
    private int inc(byte op, int addr) {
        op = (byte) (btoi(op) + 1);
        setZN(op);
        mem.set(addr, op);
        return 2;
    }

    /**
     * Increment X Register
     */
    private int inx(byte op, int addr) {
        X = (byte) (btoi(X) + 1);
        setZN(X);
        return 0;
    }

    /**
     * Increment Y Register
     */
    private int iny(byte op, int addr) {
        Y = (byte) (btoi(Y) + 1);
        setZN(Y);
        return 0;
    }

    /**
     * Jump
     */
    private int jmp(byte op, int addr) {
        PC = addr;
        return -1;
    }

    /**
     * Jump to Subroutine
     */
    private int jsr(byte op, int addr) {
        pushShort((short) (PC - 1));
        PC = addr;
        return 2;
    }

    /**
     * Load Accumulator
     */
    private int lda(byte op, int addr) {
        A = op;
        setZN(A);
        return 0;
    }

    /**
     * Load X Register
     */
    private int ldx(byte op, int addr) {
        X = op;
        setZN(X);
        return 0;
    }

    /**
     * Load Y Register
     */
    private int ldy(byte op, int addr) {
        Y = op;
        setZN(Y);
        return 0;
    }

    /**
     * Logical Shift Right
     */
    private int lsr(byte op, int addr) {
        return accumulator(op & 0x01, btoi(op) >>> 1, addr);
    }

    /**
     * No Operation
     */
    private int nop(byte op, int addr) {
        return 0;
    }

    /**
     * Logical Inclusive OR
     */
    private int ora(byte op, int addr) {
        A |= op;
        setZN(A);
        return 0;
    }

    /**
     * Push Accumulator
     */
    private int pha(byte op, int addr) {
        pushByte(A);
        return 1;
    }

    /**
     * Push Processor Status
     */
    private int php(byte op, int addr) {
        BF = 1;
        pushByte(getStatus());
        BF = 0;
        return 1;
    }

    /**
     * Pull Accumulator
     */
    private int pla(byte op, int addr) {
        A = popByte();
        setZN(A);
        return 2;
    }

    /**
     * Pull Processor Status
     */
    private int plp(byte op, int addr) {
        setStatus(popByte());
        return 2;
    }

    /**
     * Rotate Left
     */
    private int rol(byte op, int addr) {
        return accumulator(btoi(op) >>> 7, btoi(op) << 1 | CF, addr);
    }

    /**
     * Rotate Right
     */
    private int ror(byte op, int addr) {
        return accumulator(op & 0x01, btoi(op) >>> 1 | CF << 7, addr);
    }

    /**
     * Return from Interrupt
     */
    private int rti(byte op, int addr) {
        setStatus(popByte());
        PC = stoi(popShort());
        return 4;
    }

    /**
     * Return from Subroutine
     */
    private int rts(byte op, int addr) {
        PC = stoi(popShort()) + 1;
        return 4;
    }

    /**
     * Subtract with Carry
     */
    private int sbc(byte op, int addr) {
        int tempA = btoi(A) - btoi(op) - (1 - CF);
        int tempASigned = A - op - (1 - CF);
        CF = (byte) (tempA < 0 ? 0 : 1);
        VF = (byte) (tempASigned < -128 || tempASigned > 127 ? 1 : 0);
        A = (byte) tempA;
        setZN(A);
        return 0;
    }

    /**
     * Set Carry Flag
     */
    private int sec(byte op, int addr) {
        CF = 1;
        return 0;
    }

    /**
     * Set Decimal Flag
     */
    private int sed(byte op, int addr) {
        DF = 1;
        return 0;
    }

    /**
     * Set Interrupt Disable
     */
    private int sei(byte op, int addr) {
        IF = 1;
        return 0;
    }

    /**
     * Store Accumulator
     */
    private int sta(byte op, int addr) {
        mem.set(addr, A);
        return 0;
    }

    /**
     * Store X Register
     */
    private int stx(byte op, int addr) {
        mem.set(addr, X);
        return 0;
    }

    /**
     * Store Y Register
     */
    private int sty(byte op, int addr) {
        mem.set(addr, Y);
        return 0;
    }

    /**
     * Transfer Accumulator to X
     */
    private int tax(byte op, int addr) {
        X = A;
        setZN(X);
        return 0;
    }

    /**
     * Transfer Accumulator to Y
     */
    private int tay(byte op, int addr) {
        Y = A;
        setZN(Y);
        return 0;
    }

    /**
     * Transfer Stack Pointer to X
     */
    private int tsx(byte op, int addr) {
        X = S;
        setZN(X);
        return 0;
    }

    /**
     * Transfer X to Accumulator
     */
    private int txa(byte op, int addr) {
        A = X;
        setZN(A);
        return 0;
    }

    /**
     * Transfer X to Stack Pointer
     */
    private int txs(byte op, int addr) {
        S = X;
        return 0;
    }

    /**
     * Transfer Y to Accumulator
     */
    private int tya(byte op, int addr) {
        A = Y;
        setZN(Y);
        return 0;
    }

    private enum AddressingMode {
        Accumulator,
        Immediate,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        Absolute,
        AbsoluteX,
        AbsoluteXStore,
        AbsoluteY,
        AbsoluteYStore,
        Indirect,
        IndirectX,
        IndirectY,
        IndirectYStore,
        Relative,
        Implied,
    }

    private interface IInstruction {
        /**
         * Calls the instruction.
         *
         * @return Returns the number of extra cycles this call took.
         */
        int call(byte op, int addr);
    }
}
