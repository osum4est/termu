package main.java.com.eightbitforest.termu.emu.nes.rom;

import main.java.com.eightbitforest.termu.emu.core.IRom;
import main.java.com.eightbitforest.termu.emu.core.RomPath;
import main.java.com.eightbitforest.termu.emu.core.exceptions.RomLoadException;

import java.nio.ByteBuffer;

/**
 * Only supports NES 2.0 (and the backwards compatible iNES) format.
 */
@SuppressWarnings("Duplicates")
public class NesRom implements IRom {
    private String name;

    private ByteBuffer header;
    private ByteBuffer trainer;
    private ByteBuffer prgRom;
    private ByteBuffer chrRom;
    private ByteBuffer miscRom;

    private boolean isValid;
    private int prgRomSize;
    private int chrRomSize;
    private MirroringType mirroringType;
    private boolean hasBattery;
    private boolean hasTrainer;
    private boolean hasHardWiredFourScreenmode;
    private ConsoleType consoleType;
    private boolean isNes20;
    private short mapper;
    private byte subMapper;
    private int prgRamSize;
    private int eepromSize;
    private int chrRamSize;
    private int chrNvRamSize;
    private TimingMode cpuPpuTimingMode;
    private byte vsPpuType;
    private byte vsHardwareType;
    private byte extendedConsoleType;
    private byte miscRomCount;
    private byte defaultExpansionDevice;
    private int miscRomSize;

    public NesRom(RomPath romPath) throws RomLoadException {
        name = romPath.getFilenameWithoutExtension();
        load(romPath);
    }

    @Override
    public String getName() {
        return name;
    }

    ByteBuffer getHeader() {
        return header.asReadOnlyBuffer();
    }

    ByteBuffer getTrainer() {
        return trainer.asReadOnlyBuffer();
    }

    ByteBuffer getPrgRom() {
        return prgRom.asReadOnlyBuffer();
    }

    ByteBuffer getChrRom() {
        return chrRom.asReadOnlyBuffer();
    }

    ByteBuffer getMiscRom() {
        return miscRom.asReadOnlyBuffer();
    }

    boolean isValid() {
        return isValid;
    }

    int getPrgRomSize() {
        return prgRomSize;
    }

    int getChrRomSize() {
        return chrRomSize;
    }

    MirroringType getMirroringType() {
        return mirroringType;
    }

    boolean hasBattery() {
        return hasBattery;
    }

    boolean hasTrainer() {
        return hasTrainer;
    }

    boolean hasHardWiredFourScreenmode() {
        return hasHardWiredFourScreenmode;
    }

    ConsoleType getConsoleType() {
        return consoleType;
    }

    boolean isNes20() {
        return isNes20;
    }

    short getMapper() {
        return mapper;
    }

    byte getSubMapper() {
        return subMapper;
    }

    int getPrgRamSize() {
        return prgRamSize;
    }

    int getEepromSize() {
        return eepromSize;
    }

    int getChrRamSize() {
        return chrRamSize;
    }

    int getChrNvRamSize() {
        return chrNvRamSize;
    }

    TimingMode getCpuPpuTimingMode() {
        return cpuPpuTimingMode;
    }

    byte getVsPpuType() {
        return vsPpuType;
    }

    byte getVsHardwareType() {
        return vsHardwareType;
    }

    byte getExtendedConsoleType() {
        return extendedConsoleType;
    }

    byte getMiscRomCount() {
        return miscRomCount;
    }

    byte getDefaultExpansionDevice() {
        return defaultExpansionDevice;
    }

    int getMiscRomSize() {
        return miscRomSize;
    }

    private void load(RomPath romPath) throws RomLoadException {
        byte[] romBytes = romPath.readAllBytes();
        if (romBytes == null)
            throw new RomLoadException("Could not read header.");

        header = ByteBuffer.wrap(romBytes, 0, 16);

        isValid = header.get() == 'N' && header.get() == 'E' && header.get() == 'S' && header.get() == 0x1a;
        if (!isValid)
            throw new RomLoadException("Rom is not an nes rom.");

        // Rom is assumed to be correct at this point
        try {
            // Header info
            byte prgRomSizeLsb = header.get();
            byte chrRomSizeLsb = header.get();

            byte flags6 = header.get();
            switch((byte) (flags6 & 1)) {
                case 0:
                    mirroringType = MirroringType.Horizontal;
                    break;
                case 1:
                    mirroringType = MirroringType.Vertical;
                    break;
            }

            hasBattery = (flags6 >>> 1 & 1) == 1;
            hasTrainer = (flags6 >>> 2 & 1) == 1;
            hasHardWiredFourScreenmode = (flags6 >>> 3 & 1) == 1;

            byte flags7 = header.get();
            switch ((byte) (flags7 & 0x03)) {
                case 0:
                    consoleType = ConsoleType.NES;
                    break;
                case 1:
                    consoleType = ConsoleType.VsSystem;
                    break;
                case 2:
                    consoleType = ConsoleType.Playchoice;
                    break;
                case 3:
                    consoleType = ConsoleType.Extended;
                    break;
            }

            isNes20 = (flags7 & 0x0c) == 0x08;

            byte mapperMsb = header.get();
            mapper = (short) (mapperMsb << 8 & 0x0f00);
            mapper |= flags7 & 0xf0;
            mapper |= flags6 >>> 4;
            subMapper = (byte) (mapperMsb >>> 4);

            byte romSizeMsb = header.get();
            byte prgRomSizeMsb = (byte) (romSizeMsb & 0x0f);
            byte chrRomSizeMsb = (byte) (romSizeMsb >>> 4);
            if (prgRomSizeMsb != 0x0f) {
                prgRomSize = ((prgRomSizeMsb << 8) | prgRomSizeLsb) * 16 * 1024; // 16 KiB blocks
            } else {
                // If you have a rom > 4GB then you should re-evaluate your life instead of trying to murder my
                // emulator.
                byte e = (byte) (prgRomSizeLsb >>> 2);
                byte m = (byte) (prgRomSizeLsb & 0x03);
                prgRomSize = (2 << e - 1) * (m * 2 + 1);
            }
            if (chrRomSizeMsb != 0x0f) {
                chrRomSize = ((chrRomSizeMsb << 8) | chrRomSizeLsb) * 8 * 1024; // 8 KiB blocks
            } else {
                byte e = (byte) (chrRomSizeLsb >>> 2);
                byte m = (byte) (chrRomSizeLsb & 0x03);
                chrRomSize = (2 << e - 1) * (m * 2 + 1);
            }

            byte prgRamEepromSize = header.get();
            byte prgRamShift = (byte) (prgRamEepromSize & 0x0f);
            byte eepromShift = (byte) (prgRamEepromSize >>> 4);
            prgRamSize = prgRamShift != 0 ? 64 << prgRamShift : 0;
            eepromSize = eepromShift != 0 ? 64 << eepromShift : 0;

            byte chrRamNvRamSize = header.get();
            byte chrRamShift = (byte) (chrRamNvRamSize & 0x0f);
            byte chrNvRamShift = (byte) (chrRamNvRamSize >>> 4);
            chrRamSize = chrRamShift != 0 ? 64 << chrRamShift : 0;
            chrNvRamSize = chrNvRamShift != 0 ? 64 << chrNvRamShift : 0;

            switch ((byte) (header.get() & 0x03)) {
                case 0:
                    cpuPpuTimingMode = TimingMode.NTSC;
                    break;
                case 1:
                    cpuPpuTimingMode = TimingMode.PAL;
                    break;
                case 2:
                    cpuPpuTimingMode = TimingMode.MultiRegion;
                    break;
                case 3:
                    cpuPpuTimingMode = TimingMode.Dendy;
                    break;
            }

            byte consoleTypeInfo = header.get();
            if (consoleType == ConsoleType.VsSystem) {
                vsPpuType = (byte) (consoleTypeInfo & 0x0f);
                vsHardwareType = (byte) (consoleTypeInfo >>> 4);
            } else if (consoleType == ConsoleType.Extended) {
                extendedConsoleType = (byte) (consoleTypeInfo & 0x0f);
            }

            miscRomCount = (byte) (header.get() & 0x03);

            defaultExpansionDevice = (byte) (header.get() & 0x3f);

            int byteSectionStart = 16;

            // Trainer
            if (hasTrainer) {
                trainer = ByteBuffer.wrap(romBytes, byteSectionStart, 512);
                byteSectionStart += 512;
            }
            else {
                trainer = ByteBuffer.wrap(romBytes, byteSectionStart, 0);
            }

            // PRG-ROM
            prgRom = ByteBuffer.wrap(romBytes, byteSectionStart, prgRomSize);
            byteSectionStart += prgRomSize;

            // CHR-ROM
            chrRom = ByteBuffer.wrap(romBytes, byteSectionStart, chrRomSize);
            byteSectionStart += chrRomSize;

            // Misc ROM data
            miscRomSize = romBytes.length - 16 - (hasTrainer ? 512 : 0) - prgRomSize - chrRomSize;
            miscRom = ByteBuffer.wrap(romBytes, byteSectionStart, miscRomSize);
        } catch (Exception e) {
            throw new RomLoadException("Could not read header. Please make sure that it is valid.");
        }
    }

    public enum MirroringType {
        Horizontal,
        Vertical
    }

    public enum ConsoleType {
        NES,
        VsSystem,
        Playchoice,
        Extended
    }

    public enum TimingMode {
        NTSC,
        PAL,
        MultiRegion,
        Dendy
    }
}
