package main.java.com.eightbitforest.termu.emu.nes.system;

import main.java.com.eightbitforest.termu.emu.core.exceptions.EmuException;
import main.java.com.eightbitforest.termu.emu.nes.rom.NesRom;
import main.java.com.eightbitforest.termu.emu.nes.rom.mappers.Mapper;

public class Mem {
    // Memory
    private byte[] internalRam;
    private byte[] apuIoRegs;
    private byte[] apuIoTest;

    private NesRom cartridge;
    private Mapper cartridgeMapper;

    private byte[] ppuRegs;
    private byte[] ppuPatternTables;
    private byte[] ppuNameTables;
    private byte[] ppuPaletteIndexes;
    private byte[] ppuOam;

    public Mem(NesRom cartridge) {
        internalRam = new byte[0x0800];
        apuIoRegs = new byte[0x0018];
        apuIoTest = new byte[0x0008];

        this.cartridge = cartridge;
        cartridgeMapper = cartridge.getMapper();

        ppuRegs = new byte[0x0008];
        ppuPatternTables = new byte[0x2000];
        ppuNameTables = new byte[0x1000];
        ppuPaletteIndexes = new byte[0x0020];
    }

    public byte getCpu(int addr) {
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

    public void setCpu(int addr, byte b) {
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

    public void setPpuRegs(byte[] ppuRegs) {
        this.ppuRegs = ppuRegs;
    }

    public byte getPpu(int addr) {
        if (addr < 0x3000)
            return ppuPatternTables[addr];

        if (addr < 0x3f00)
            return ppuNameTables[(addr - 0x3000) % ppuNameTables.length];

        if (addr < 0x3fff)
            return ppuPaletteIndexes[(addr - 0x3f00) % ppuPaletteIndexes.length];

        throw new EmuException(String.format("Cannot access ppu memory at %x.", addr));
    }

    public byte setPpu(int addr, byte b) {
        if (addr < 0x3000)
            return ppuPatternTables[addr] = b;

        if (addr < 0x3f00)
            return ppuNameTables[(addr - 0x3000) % ppuNameTables.length] = b;

        if (addr < 0x3fff)
            return ppuPaletteIndexes[(addr - 0x3f00) % ppuPaletteIndexes.length] = b;

        throw new EmuException(String.format("Cannot access ppu memory at %x.", addr));
    }

    public byte getPpuOam(int addr) {
        return ppuOam[addr];
    }

    public byte setPpuOam(int addr, byte b) {
        return ppuOam[addr];
    }
}
