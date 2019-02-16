package main.java.com.eightbitforest.termu.emu.nes.rom.mappers;

import main.java.com.eightbitforest.termu.emu.nes.rom.NesRom;

public abstract class Mapper {
    private NesRom rom;

    protected byte[] trainer;
    protected byte[] prgRom;
    protected byte[] prgRam;
    protected byte[] chrRom;
    protected byte[] miscRom;

    public Mapper(NesRom rom) {
        this.rom = rom;
        trainer = rom.getTrainer();
        prgRom = rom.getPrgRom();
        prgRam = rom.getPrgRam();
        chrRom = rom.getChrRom();
        miscRom = rom.getMiscRom();
    }

    protected NesRom getRom() {
        return rom;
    }

    public abstract byte memget(int addr);
}
