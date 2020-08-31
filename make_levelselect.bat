@echo off

lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\main.o src\levelselect\main.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceBgMap.o src\levelselect\SpaceBgMap.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceBgTiles.o src\levelselect\SpaceBgTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceShipTiles.o src\levelselect\SpaceShipTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceShipLargeTiles.o src\levelselect\SpaceShipLargeTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceShipEngineTiles.o src\levelselect\SpaceShipEngineTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceShipLargeEngineTiles.o src\levelselect\SpaceShipLargeEngineTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\output.o src\levelselect\output.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\gbt_player.o src\levelselect\gbt_player.s
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\gbt_player_bank1.o src\levelselect\gbt_player_bank1.s

lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -Wl-yt1 -Wl-yo4 -Wl-ya0 -o build\space.gb build\main.o build\SpaceBgMap.o build\SpaceBgTiles.o build\SpaceShipTiles.o build\SpaceShipLargeTiles.o build\SpaceShipEngineTiles.o build\SpaceShipLargeEngineTiles.o build\output.o build\gbt_player.o build\gbt_player_bank1.o

del build\*.o build\*.lst build\*.sym build\*.map