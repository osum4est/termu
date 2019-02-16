package main.java.com.eightbitforest.termu.emu.core;

import main.java.com.eightbitforest.termu.emu.core.exceptions.RomLoadException;
import main.java.com.eightbitforest.termu.emu.nes.NesEmulator;

import java.io.File;
import java.util.Arrays;

public class EmulatorLoader {
    private static final IEmulator[] emulators = new IEmulator[]{
            new NesEmulator()
    };

    public static IEmulator getEmulatorForRom(RomPath romPath) throws RomLoadException {
        IEmulator emulator = null;
        for (IEmulator possibleEmulator : emulators) {
            if (Arrays.asList(possibleEmulator.getRomFileExtensions()).contains(romPath.getExtension()))
                emulator = possibleEmulator;
        }

        if (emulator == null)
            throw new RomLoadException("Could not find emulator for extension: " + romPath.getExtension() + ".");

        return emulator;
    }
}
