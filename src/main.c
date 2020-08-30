#include <gb/gb.h>
#include <gb/drawing.h>
#include "SpaceBgTiles.h"
#include "SpaceBgMap.h"
#include "SpaceShipTiles.h"
#include "SpaceShipLargeTiles.h"
#include "SpaceShipEngineTiles.h"
#include "SpaceShipLargeEngineTiles.h"
#include "gbt_player.h"

extern const unsigned char * spaceSong_Data[];

#define WHITE  0
#define SILVER 1
#define GRAY   2
#define BLACK  3
#define PALETTE(c0, c1, c2, c3) c0 | c1 << 2 | c2 << 4 | c3 << 6

#define BLANK_SPRITE_INDEX 0

//sprite index = index of sprite to load tiles into
//sprite count = number of sprites to display at one time (8x8 = 1, 16x16 = 4)
//tile index = index of tile in memory
//tile count = total number of tiles

#define SMALL_SHIP_SPRITE_INDEX 1
#define SMALL_SHIP_SPRITE_COUNT 1
#define SMALL_SHIP_TILE_INDEX 1
#define SMALL_SHIP_TILE_COUNT 2

#define SMALL_SHIP_ENGINE_SPRITE_INDEX 2
#define SMALL_SHIP_ENGINE_SPRITE_COUNT 1
#define SMALL_SHIP_ENGINE_TILE_INDEX 3
#define SMALL_SHIP_ENGINE_TILE_COUNT 2

#define LARGE_SHIP_SPRITE_INDEX 3
#define LARGE_SHIP_SPRITE_COUNT 4
#define LARGE_SHIP_TILE_INDEX 5
#define LARGE_SHIP_TILE_COUNT 4

#define LARGE_SHIP_ENGINE_SPRITE_INDEX 7
#define LARGE_SHIP_ENGINE_SPRITE_COUNT 1
#define LARGE_SHIP_ENGINE_TILE_INDEX 26
#define LARGE_SHIP_ENGINE_TILE_COUNT 2

#define SONG_BANK 2
#define SONG_SPEED 7

UINT8 ship_x, ship_y, ship_dir;

UBYTE previous_KEYS = 0;
UBYTE keys = 0;

//performance delay to free up CPU
void performantDelay(UINT8 numloops)
{
    UINT8 i;
    for(i=0;i < numloops;i++)
    {   
        wait_vbl_done();
    }   
}

//joypad functions
void updateKeys()
{
    previous_KEYS = keys;
    keys = joypad();
}

INT8 keyPressed(INT8 K)
{
    return keys & (K);
}

INT8 keyTicked(INT8 K)
{
    return (keys & (K) && !(previous_KEYS & (K)));
}

INT8 keyReleased(INT8 K)
{
    return previous_KEYS & (K) && !(keys & (K));
}

void anyKey()
{
    keys;
}

void HideSprite(UINT8 sprite_index){
    set_sprite_tile(sprite_index, BLANK_SPRITE_INDEX);
}

void TilePos_8x8(UINT8 sprite_index, UINT8 x, UINT8 y){
    move_sprite(sprite_index, x, y);
}

void TileDir_8x8(UINT8 sprite_index, UINT8 tile_index, UINT8 dir){
    // 0: up
    // 1: down
    // 2: left
    // 3: right

    // expects vertical 8x8 shape to be in first tile
    // and horizontal 8x8 shape to be in second tile

    if (dir == 0){ //up
        set_sprite_tile(sprite_index, tile_index);
        set_sprite_prop(sprite_index, get_sprite_prop(sprite_index) & ~S_FLIPY);
    } else if (dir == 1){ //down
        set_sprite_tile(sprite_index, tile_index);
        set_sprite_prop(sprite_index, get_sprite_prop(sprite_index) | S_FLIPY);
    } else if (dir == 2){ //left
        set_sprite_tile(sprite_index, tile_index+1);
        set_sprite_prop(sprite_index, get_sprite_prop(sprite_index) | S_FLIPX);
    } else if (dir == 3){ //right
        set_sprite_tile(sprite_index, tile_index+1);
        set_sprite_prop(sprite_index, get_sprite_prop(sprite_index) & ~S_FLIPX);
    }
}

void TilePos_16x16(UINT8 sprite_index, UINT8 x, UINT8 y){
    move_sprite(sprite_index, x, y);
    move_sprite(sprite_index+1, x, y+8);
    move_sprite(sprite_index+2, x+8, y);
    move_sprite(sprite_index+3, x+8, y+8);
}

void TileDir_16x16sym(UINT8 sprite_index, UINT8 tile_index, UINT8 dir){
    // 0: up
    // 1: down
    // 2: left
    // 3: right

    // expects vertical 16x16 shape to be made from 2 symetrical tiles
    // and horizontal 16x16 shape to be made from 2 symertrical tiles
    // tile 1: vertical top left 
    // tile 2: vertical bottom left
    // tile 3: horizontal top right
    // tile 4: horizontal top left

    UINT8 sprite_1 = sprite_index;
    UINT8 sprite_2 = sprite_index+1;
    UINT8 sprite_3 = sprite_index+2;
    UINT8 sprite_4 = sprite_index+3;

    UINT8 tile_1 = tile_index;
    UINT8 tile_2 = tile_index+1;
    UINT8 tile_3 = tile_index+2;
    UINT8 tile_4 = tile_index+3;

    if (dir == 0){ //up
        set_sprite_tile(sprite_1, tile_1);
        set_sprite_prop(sprite_1, get_sprite_prop(sprite_1) & ~S_FLIPX);
        set_sprite_prop(sprite_1, get_sprite_prop(sprite_1) & ~S_FLIPY);
        set_sprite_tile(sprite_2, tile_2);
        set_sprite_prop(sprite_2, get_sprite_prop(sprite_2) & ~S_FLIPX);
        set_sprite_prop(sprite_2, get_sprite_prop(sprite_2) & ~S_FLIPY);
        set_sprite_tile(sprite_3, tile_1);
        set_sprite_prop(sprite_3, get_sprite_prop(sprite_3) | S_FLIPX);
        set_sprite_prop(sprite_3, get_sprite_prop(sprite_3) & ~S_FLIPY);
        set_sprite_tile(sprite_4, tile_2);
        set_sprite_prop(sprite_4, get_sprite_prop(sprite_4) | S_FLIPX);
        set_sprite_prop(sprite_4, get_sprite_prop(sprite_4) & ~S_FLIPY);
    } else if (dir == 1){ //down
        set_sprite_tile(sprite_1, tile_2);
        set_sprite_prop(sprite_1, get_sprite_prop(sprite_1) & ~S_FLIPX);
        set_sprite_prop(sprite_1, get_sprite_prop(sprite_1) | S_FLIPY);
        set_sprite_tile(sprite_2, tile_1);
        set_sprite_prop(sprite_2, get_sprite_prop(sprite_2) & ~S_FLIPX);
        set_sprite_prop(sprite_2, get_sprite_prop(sprite_2) | S_FLIPY);
        set_sprite_tile(sprite_3, tile_2);
        set_sprite_prop(sprite_3, get_sprite_prop(sprite_3) | S_FLIPX);
        set_sprite_prop(sprite_3, get_sprite_prop(sprite_3) | S_FLIPY);
        set_sprite_tile(sprite_4, tile_1);
        set_sprite_prop(sprite_4, get_sprite_prop(sprite_4) | S_FLIPX);
        set_sprite_prop(sprite_4, get_sprite_prop(sprite_4) | S_FLIPY);
    } else if (dir == 2){ //left
        set_sprite_tile(sprite_1, tile_3);
        set_sprite_prop(sprite_1, get_sprite_prop(sprite_1) | S_FLIPX);
        set_sprite_prop(sprite_1, get_sprite_prop(sprite_1) & ~S_FLIPY);
        set_sprite_tile(sprite_2, tile_3);
        set_sprite_prop(sprite_2, get_sprite_prop(sprite_2) | S_FLIPX);
        set_sprite_prop(sprite_2, get_sprite_prop(sprite_2) | S_FLIPY);
        set_sprite_tile(sprite_3, tile_4);
        set_sprite_prop(sprite_3, get_sprite_prop(sprite_3) | S_FLIPX);
        set_sprite_prop(sprite_3, get_sprite_prop(sprite_3) & ~S_FLIPY);
        set_sprite_tile(sprite_4, tile_4);
        set_sprite_prop(sprite_4, get_sprite_prop(sprite_4) | S_FLIPX);
        set_sprite_prop(sprite_4, get_sprite_prop(sprite_4) | S_FLIPY);
    } else if (dir == 3){ //right
        set_sprite_tile(sprite_1, tile_4);
        set_sprite_prop(sprite_1, get_sprite_prop(sprite_1) & ~S_FLIPX);
        set_sprite_prop(sprite_1, get_sprite_prop(sprite_1) & ~S_FLIPY);
        set_sprite_tile(sprite_2, tile_4);
        set_sprite_prop(sprite_2, get_sprite_prop(sprite_2) & ~S_FLIPX);
        set_sprite_prop(sprite_2, get_sprite_prop(sprite_2) | S_FLIPY);
        set_sprite_tile(sprite_3, tile_3);
        set_sprite_prop(sprite_3, get_sprite_prop(sprite_3) & ~S_FLIPX);
        set_sprite_prop(sprite_3, get_sprite_prop(sprite_3) & ~S_FLIPY);
        set_sprite_tile(sprite_4, tile_3);
        set_sprite_prop(sprite_4, get_sprite_prop(sprite_4) & ~S_FLIPX);
        set_sprite_prop(sprite_4, get_sprite_prop(sprite_4) | S_FLIPY);
    }
}

void SmallShipBodyInit(){
    // load small spaceship tiles into sprite index 0:1 (two tiles)
    set_sprite_data(SMALL_SHIP_TILE_INDEX, SMALL_SHIP_TILE_COUNT, SpaceShipTiles);
}

void SmallShipBodyPos(UINT8 x, UINT8 y){
    TilePos_8x8(SMALL_SHIP_SPRITE_INDEX, x, y);
}

void SmallShipBodyDir(UINT8 dir){
    // 0: up
    // 1: down
    // 2: left
    // 3: right
    TileDir_8x8(SMALL_SHIP_SPRITE_INDEX, SMALL_SHIP_TILE_INDEX, dir);
}

void SmallShipEngineInit(){
    // load small spaceship tiles into sprite index 0:1 (two tiles)
    set_sprite_data(SMALL_SHIP_ENGINE_TILE_INDEX, SMALL_SHIP_ENGINE_TILE_COUNT, SpaceShipEngineTiles);
}

void SmallShipEnginePos(UINT8 x, UINT8 y){
    TilePos_8x8(SMALL_SHIP_ENGINE_SPRITE_INDEX, x, y);
}

void SmallShipEngineDir(UINT8 dir){
    // 0: up
    // 1: down
    // 2: left
    // 3: right
    TileDir_8x8(SMALL_SHIP_ENGINE_SPRITE_INDEX, SMALL_SHIP_ENGINE_TILE_INDEX, dir);
}

void HideSmallShipEngine(){
    HideSprite(SMALL_SHIP_ENGINE_SPRITE_INDEX);
}

void LargeShipBodyInit(){
    // load large sprite set into 2:6 (2 = start index, 4 = number of tiles to load)
    set_sprite_data(LARGE_SHIP_TILE_INDEX, LARGE_SHIP_TILE_COUNT, SpaceShipLargeTiles);
}

void LargeShipBodyPos(UINT8 x, UINT8 y){
    TilePos_16x16(LARGE_SHIP_SPRITE_INDEX, x, y);
}

void LargeShipBodyDir(UINT8 dir){
    // 0: up
    // 1: down
    // 2: left
    // 3: right
    TileDir_16x16sym(LARGE_SHIP_SPRITE_INDEX, LARGE_SHIP_TILE_INDEX, dir);
}

void LargeShipEngineInit(){
    // load large sprite set into 2:6 (2 = start index, 4 = number of tiles to load)
    set_sprite_data(LARGE_SHIP_ENGINE_TILE_INDEX, LARGE_SHIP_ENGINE_TILE_COUNT, ShipLargeEngineTiles);
}

void LargeShipEnginePos(UINT8 x, UINT8 y){
    TilePos_8x8(LARGE_SHIP_ENGINE_SPRITE_INDEX, x, y);
}

void LargeShipEngineDir(UINT8 dir){
    // 0: up
    // 1: down
    // 2: left
    // 3: right
    TileDir_8x8(LARGE_SHIP_ENGINE_SPRITE_INDEX, LARGE_SHIP_ENGINE_TILE_INDEX, dir);
}

void HideLargeShipEngine(){
    HideSprite(LARGE_SHIP_ENGINE_SPRITE_INDEX);
}

void SmallShipInit(){
    SmallShipBodyInit();
    SmallShipEngineInit();
}

void SmallShipState(UINT8 x, UINT8 y, UINT8 dir, UINT8 engine){
    // Dir
    // 0: up
    // 1: down
    // 2: left
    // 3: right

    // Engine
    // 0: off
    // 1: on

    SmallShipBodyPos(x,y);
    SmallShipBodyDir(dir);
    SmallShipEngineDir(dir);
    if (engine == 1){
        if (dir == 0){ //up
            SmallShipEnginePos(x,y+8);
        } else if (dir == 1){ //down
            SmallShipEnginePos(x,y-8);
        } else if (dir == 2){ //left
            SmallShipEnginePos(x+8,y);
        } else if (dir == 3){ //right
            SmallShipEnginePos(x-8,y);
        }
    } else {
        HideSmallShipEngine();
    }
}

void LargeShipInit(){
    LargeShipBodyInit();
    LargeShipEngineInit();
}

void LargeShipState(UINT8 x, UINT8 y, UINT8 dir, UINT8 engine){
    // Dir
    // 0: up
    // 1: down
    // 2: left
    // 3: right

    // Engine
    // 0: off
    // 1: on

    LargeShipBodyPos(x,y);
    LargeShipBodyDir(dir);
    LargeShipEngineDir(dir);
    LargeShipEnginePos(x,y+16);
    HideLargeShipEngine();
    if (engine == 1){
        //
    }
    /*
    if (engine == 1){
        if (dir == 0){ //up
            //LargeShipEnginePos(x,y+16);
        } else if (dir == 1){ //down
            //LargeShipEnginePos(x,y-16);
        } else if (dir == 2){ //left
            //LargeShipEnginePos(x+16,y);
        } else if (dir == 3){ //right
            //LargeShipEnginePos(x-16,y);
        }
    } else {
        //HideLargeShipEngine();
    }
    */
}

void main() {
    ship_x = 83, ship_y = 83;

    disable_interrupts();

    gbt_play(spaceSong_Data, SONG_BANK, SONG_SPEED);
    //gbt_loop(1);
    set_interrupts(VBL_IFLAG);

    enable_interrupts();
    
    // TODO start screen (press start to continue)
    //waitpad(J_START); // wait for start

    // load background
    set_bkg_data(0, 8, SpaceBgTiles ); //load background tile set
    set_bkg_tiles( 0, 0, SpaceBgMapWidth, SpaceBgMapHeight, SpaceBgMap); //load background map
    SHOW_BKG;
	
	// set color palette
	BGP_REG = PALETTE(BLACK, GRAY, SILVER, WHITE);

    // load sprites
    SPRITES_8x8;

    
    // initalise small ship sprite
    SmallShipInit();
    SmallShipState(ship_x,ship_y,0,0);

    // initalise large ship sprite
    LargeShipInit();
    LargeShipState(ship_x-4,ship_y+16,0,0);
    
    SHOW_SPRITES; // display sprites
    
    // game loop
    while(1) {
        wait_vbl_done();

        updateKeys(); // check key presses
        // move sprite if joykey pressed
        if (keyPressed(J_RIGHT) || keyPressed(J_LEFT) || keyPressed(J_UP) || keyPressed(J_DOWN)){
            if (keyPressed(J_RIGHT)){
                ship_x++;
                ship_dir = 3;
            }
            if(keyPressed(J_LEFT)) {
                ship_x--;
                ship_dir = 2;
            }
            if(keyPressed(J_UP)) {
                ship_y--;
                ship_dir = 0;
            }
            if(keyPressed(J_DOWN)) {
                ship_y++;
                ship_dir = 1;
            }
            SmallShipState(ship_x, ship_y, ship_dir,1);
            LargeShipState(ship_x-4,ship_y+16,ship_dir,1);
        } else {
            SmallShipState(ship_x, ship_y, ship_dir,0);
            LargeShipState(ship_x-4,ship_y+16,ship_dir,0);
        }
        gbt_update();
        // delay to control frame rate
        //performantDelay(2);
    }
}