@echo off

lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\main.o src\main.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceBgMap.o src\SpaceBgMap.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceBgTiles.o src\SpaceBgTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceShipTiles.o src\SpaceShipTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceShipLargeTiles.o src\SpaceShipLargeTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceShipEngineTiles.o src\SpaceShipEngineTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\SpaceShipLargeEngineTiles.o src\SpaceShipLargeEngineTiles.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\output.o src\output.c
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\gbt_player.o src\gbt_player.s
lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o build\gbt_player_bank1.o src\gbt_player_bank1.s

lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -Wl-yt1 -Wl-yo4 -Wl-ya0 -o build\space.gb build\main.o build\SpaceBgMap.o build\SpaceBgTiles.o build\SpaceShipTiles.o build\SpaceShipLargeTiles.o build\SpaceShipEngineTiles.o build\SpaceShipLargeEngineTiles.o build\output.o build\gbt_player.o build\gbt_player_bank1.o

del build\*.o build\*.lst build\*.sym build\*.map