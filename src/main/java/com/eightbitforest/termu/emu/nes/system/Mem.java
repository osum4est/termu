package main.java.com.eightbitforest.termu.emu.nes.system;

import main.java.com.eightbitforest.termu.emu.nes.rom.NesRom;
import main.java.com.eightbitforest.termu.emu.nes.rom.mappers.Mapper;

public class Mem {
    // Memory
    private byte[] internalRam;
    private byte[] ppuRegs; // TODO: Move to ppu class
    private byte[] apuIoRegs;
    private byte[] apuIoTest;

    private NesRom cartridge;
    private Mapper cartridgeMapper;

    public Mem(NesRom cartridge) {
        internalRam = new byte[0x0800];
        ppuRegs = new byte[0x0008];
        apuIoRegs = new byte[0x0018];
        apuIoTest = new byte[0x0008];

        this.cartridge = cartridge;
        cartridgeMapper = cartridge.getMapper();
    }

    /**
     * Little endian.
     */
    public short getShort(int addr) {
        return (short) ((get(addr + 1) << 8) & 0xff00 | (get(addr) & 0x00ff));
    }

    public byte get(int addr) {
        if (addr < 0x2000)
            return internalRam[addr % internalRam.length];

        if (addr < 0x4000)
            return ppuRegs[(addr - 0x2000) % ppuRegs.length];

        if (addr < 0x4018)
            return apuIoRegs[addr - 0x4000];

        if (addr < 0x4020)
            return apuIoTest[addr - 0x4018];

        return cartridgeMapper.memget(addr);
    }

    public void set(int addr, byte b) {
        if (addr < 0x2000)
            internalRam[addr % internalRam.length] = b;

        else if (addr < 0x4000)
            ppuRegs[(addr - 0x2000) % ppuRegs.length] = b;

        else if (addr < 0x4018)
            apuIoRegs[addr - 0x4000] = b;

        else if (addr < 0x4020)
            apuIoTest[addr - 0x4018] = b;

        else
            cartridgeMapper.memset(addr, b);
    }
}
