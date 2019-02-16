package main.java.com.eightbitforest.termu.emu.core;

import main.java.com.eightbitforest.termu.emu.core.exceptions.RomLoadException;

public interface IEmulator {
    String[] getRomFileExtensions();

    IRom loadRom(RomPath romPath) throws RomLoadException;

    IRom getLoadedRom();
}
