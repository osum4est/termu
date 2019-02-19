package main.java.com.eightbitforest.termu.emu.nes.rom.mappers;

import main.java.com.eightbitforest.termu.emu.core.exceptions.MemoryException;
import main.java.com.eightbitforest.termu.emu.nes.rom.NesRom;

public class Mapper000 extends Mapper{
    public Mapper000(NesRom rom) {
        super(rom);
    }

    @Override
    public byte memget(int addr) {
        if (addr < 0x6000)
            throw new MemoryException(String.format("Cannot access memory at %x.", addr));

        if (addr < 0x8000)
            return prgRam[(addr - 0x6000) % prgRam.length];

        return prgRom[(addr - 0x8000) % prgRom.length];
    }

    @Override
    public void memset(int addr, byte b) {
        if (addr < 0x6000)
            throw new MemoryException(String.format("Cannot access memory at %x.", addr));

        else if (addr < 0x8000)
            prgRam[(addr - 0x6000) % prgRam.length] = b;

        else
            prgRom[(addr - 0x8000) % prgRom.length] = b;
    }
}
