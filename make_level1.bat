@echo off

lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\main.o src\level1\main.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\playerTiles.o src\level1\playerTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\playerWalkTiles.o src\level1\playerWalkTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\level1Map.o src\level1\level1Map.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\level1Tiles.o src\level1\level1Tiles.c

lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -Wl-yt1 -Wl-yo4 -Wl-ya0 -o build\space.gb build\main.o build\playerTiles.o build\playerWalkTiles.o build\level1Map.o build\level1Tiles.o

del build\*.o build\*.lst build\*.sym build\*.map