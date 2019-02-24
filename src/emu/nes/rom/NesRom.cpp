package main.java.com.eightbitforest.termu.emu.nes.rom;

import main.java.com.eightbitforest.termu.emu.core.IRom;
import main.java.com.eightbitforest.termu.emu.core.RomPath;
import main.java.com.eightbitforest.termu.emu.core.exceptions.RomLoadException;
import main.java.com.eightbitforest.termu.emu.nes.exceptions.MissingMapperException;
import main.java.com.eightbitforest.termu.emu.nes.rom.mappers.Mapper;
import main.java.com.eightbitforest.termu.emu.nes.rom.mappers.MapperRegistry;

import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * Only supports NES 2.0 (and the backwards compatible iNES) format.
 */
@SuppressWarnings("Duplicates")
public class NesRom implements IRom {
    private String name;

    private byte[] header;
    private byte[] trainer;
    private byte[] prgRom;
    private byte[] prgRam;
    private byte[] chrRom;
    private byte[] miscRom;

    private boolean isValid;
    private int prgRomSize;
    private int chrRomSize;
    private MirroringType mirroringType;
    private boolean hasBattery;
    private boolean hasTrainer;
    private boolean hasHardWiredFourScreenmode;
    private ConsoleType consoleType;
    private boolean isNes20;
    private Mapper mapper;
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

    public byte[] getHeader() {
        return header;
    }

    public byte[] getTrainer() {
        return trainer;
    }

    public byte[] getPrgRom() {
        return prgRom;
    }

    public byte[] getPrgRam() {
        return prgRam;
    }

    public byte[] getChrRom() {
        return chrRom;
    }

    public byte[] getMiscRom() {
        return miscRom;
    }

    public boolean isValid() {
        return isValid;
    }

    public int getPrgRomSize() {
        return prgRomSize;
    }

    public int getChrRomSize() {
        return chrRomSize;
    }

    public MirroringType getMirroringType() {
        return mirroringType;
    }

    public boolean hasBattery() {
        return hasBattery;
    }

    public boolean hasTrainer() {
        return hasTrainer;
    }

    public boolean hasHardWiredFourScreenmode() {
        return hasHardWiredFourScreenmode;
    }

    public ConsoleType getConsoleType() {
        return consoleType;
    }

    public boolean isNes20() {
        return isNes20;
    }

    public Mapper getMapper() {
        return mapper;
    }

    public byte getSubMapper() {
        return subMapper;
    }

    public int getPrgRamSize() {
        return prgRamSize;
    }

    public int getEepromSize() {
        return eepromSize;
    }

    public int getChrRamSize() {
        return chrRamSize;
    }

    public int getChrNvRamSize() {
        return chrNvRamSize;
    }

    public TimingMode getCpuPpuTimingMode() {
        return cpuPpuTimingMode;
    }

    public byte getVsPpuType() {
        return vsPpuType;
    }

    public byte getVsHardwareType() {
        return vsHardwareType;
    }

    public byte getExtendedConsoleType() {
        return extendedConsoleType;
    }

    public byte getMiscRomCount() {
        return miscRomCount;
    }

    public byte getDefaultExpansionDevice() {
        return defaultExpansionDevice;
    }

    public int getMiscRomSize() {
        return miscRomSize;
    }

    private void load(RomPath romPath) throws RomLoadException {
        byte[] romBytes = romPath.readAllBytes();
        if (romBytes == null)
            throw new RomLoadException("Could not read header.");

        header = Arrays.copyOfRange(romBytes, 0, 16);

        isValid = header[0] == 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == 0x1a;
        if (!isValid)
            throw new RomLoadException("Rom is not an nes rom.");

        // Rom is assumed to be correct at this point
        try {
            // Header info
            byte prgRomSizeLsb = header[4];
            byte chrRomSizeLsb = header[5];

            byte flags6 = header[6];
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

            byte flags7 = header[7];
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

            byte mapperMsb = header[8];
            short mapperId = (short) (mapperMsb << 8 & 0x0f00);
            mapperId |= flags7 & 0xf0;
            mapperId |= flags6 >>> 4;
            subMapper = (byte) (mapperMsb >>> 4);

            byte romSizeMsb = header[9];
            byte prgRomSizeMsb = (byte) (romSizeMsb & 0x0f);
            byte chrRomSizeMsb = (byte) (romSizeMsb >>> 4);
            if (prgRomSizeMsb != 0x0f) {
                prgRomSize = ((prgRomSizeMsb << 8) & 0xff00 | (prgRomSizeLsb & 0x00ff)) * 16 * 1024; // 16 KiB blocks
            } else {
                // If you have a rom > 4GB then you should re-evaluate your life instead of trying to murder my
                // emulator.
                byte e = (byte) (prgRomSizeLsb >>> 2);
                byte m = (byte) (prgRomSizeLsb & 0x03);
                prgRomSize = (2 << e - 1) * (m * 2 + 1);
            }
            if (chrRomSizeMsb != 0x0f) {
                chrRomSize = ((chrRomSizeMsb << 8) & 0xff00 | (chrRomSizeLsb & 0x00ff)) * 8 * 1024; // 8 KiB blocks
            } else {
                byte e = (byte) (chrRomSizeLsb >>> 2);
                byte m = (byte) (chrRomSizeLsb & 0x03);
                chrRomSize = (2 << e - 1) * (m * 2 + 1);
            }

            byte prgRamEepromSize = header[10];
            byte prgRamShift = (byte) (prgRamEepromSize & 0x0f);
            byte eepromShift = (byte) (prgRamEepromSize >>> 4);
            prgRamSize = prgRamShift != 0 ? 64 << prgRamShift : 0;
            eepromSize = eepromShift != 0 ? 64 << eepromShift : 0;

            byte chrRamNvRamSize = header[11];
            byte chrRamShift = (byte) (chrRamNvRamSize & 0x0f);
            byte chrNvRamShift = (byte) (chrRamNvRamSize >>> 4);
            chrRamSize = chrRamShift != 0 ? 64 << chrRamShift : 0;
            chrNvRamSize = chrNvRamShift != 0 ? 64 << chrNvRamShift : 0;

            switch ((byte) (header[12] & 0x03)) {
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

            byte consoleTypeInfo = header[13];
            if (consoleType == ConsoleType.VsSystem) {
                vsPpuType = (byte) (consoleTypeInfo & 0x0f);
                vsHardwareType = (byte) (consoleTypeInfo >>> 4);
            } else if (consoleType == ConsoleType.Extended) {
                extendedConsoleType = (byte) (consoleTypeInfo & 0x0f);
            }

            miscRomCount = (byte) (header[14] & 0x03);

            defaultExpansionDevice = (byte) (header[15] & 0x3f);

            int byteSectionStart = 16;

            // Trainer
            if (hasTrainer) {
                trainer = Arrays.copyOfRange(romBytes, byteSectionStart, byteSectionStart + 512);
                byteSectionStart += 512;
            }
            else {
                trainer = Arrays.copyOfRange(romBytes, byteSectionStart, byteSectionStart);
            }

            // PRG-ROM
            prgRom = Arrays.copyOfRange(romBytes, byteSectionStart, byteSectionStart + prgRomSize);
            byteSectionStart += prgRomSize;

            // CHR-ROM
            chrRom = Arrays.copyOfRange(romBytes, byteSectionStart, byteSectionStart + chrRomSize);
            byteSectionStart += chrRomSize;

            // Misc ROM data
            miscRomSize = romBytes.length - header.length - trainer.length - prgRom.length - chrRom.length;
            miscRom = Arrays.copyOfRange(romBytes, byteSectionStart, byteSectionStart + miscRomSize);

            // Ram
            prgRam = new byte[prgRamSize];

            // Load Mapper
            try {
                mapper = MapperRegistry.getMapper(mapperId, this);
            } catch (MissingMapperException e) {
                throw new RomLoadException("Mapper " + mapperId + " is currently not supported.");
            }
        } catch (IndexOutOfBoundsException e) {
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
