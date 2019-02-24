package main.java.com.eightbitforest.termu.emu.nes.rom.mappers;

import main.java.com.eightbitforest.termu.emu.nes.exceptions.MissingMapperException;
import main.java.com.eightbitforest.termu.emu.nes.rom.NesRom;

import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.Map;

public class MapperRegistry {
    private static Map<Short, Class<? extends Mapper>> mappers = new HashMap<Short, Class<? extends Mapper>>() {
        {
            put((short) 0x0000, Mapper000.class);
        }
    };

    public static Mapper getMapper(short mapperId, NesRom rom) throws MissingMapperException {
        Mapper mapper = null;

        try {
            Class<? extends Mapper> mapperClass = mappers.get(mapperId);
            if(mapperClass!= null) {
                mapper = (Mapper) mapperClass.getDeclaredConstructors()[0].newInstance(rom);
            }
        } catch (InstantiationException | IllegalAccessException | InvocationTargetException e) {
            e.printStackTrace();
        }

        if (mapper == null)
            throw new MissingMapperException();

        return mapper;
    }
}
