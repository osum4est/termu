package main.java.com.eightbitforest.termu.emu.nes;

import main.java.com.eightbitforest.termu.emu.core.IEmulator;
import main.java.com.eightbitforest.termu.emu.core.IRom;
import main.java.com.eightbitforest.termu.emu.core.RomPath;
import main.java.com.eightbitforest.termu.emu.core.exceptions.RomLoadException;
import main.java.com.eightbitforest.termu.emu.nes.rom.NesRom;

import java.io.File;

/**
 * HUGE thanks to <a href="wiki.nesdev.com">wiki.nesdev.com</a> for providing detailed information on all the inner workings of the NES.
 */
public class NesEmulator implements IEmulator {
    private NesRom rom;

    @Override
    public String[] getRomFileExtensions() {
        return new String[] {
            "nes"
        };
    }

    @Override
    public IRom loadRom(RomPath romPath) throws RomLoadException {
        // TODO: Make sure emulator isn't already running
        return rom = new NesRom(romPath);
    }

    @Override
    public IRom getLoadedRom() {
        return rom;
    }
}
