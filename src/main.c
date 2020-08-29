#include <gb/gb.h>
#include <gb/drawing.h>
#include "SpaceBgTiles.h"
#include "SpaceBgMap.h"
#include "SpaceShipTiles.h"
#include "gbt_player.h"

extern const unsigned char * spaceSong_Data[];

#define WHITE  0
#define SILVER 1
#define GRAY   2
#define BLACK  3
#define PALETTE(c0, c1, c2, c3) c0 | c1 << 2 | c2 << 4 | c3 << 6

#define SHIP_TYPE_UD 0
#define SHIP_TYPE_LR 1
#define SPACESHIP_TILE_LEN 8

#define SONG_BANK 2
#define SONG_SPEED 7

UINT8 ship_x, ship_y;

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

    // load spaceship sprite
    SPRITES_8x8;
    set_sprite_data(0, SPACESHIP_TILE_LEN, SpaceShipTiles);
    set_sprite_tile(0, SHIP_TYPE_LR);
    move_sprite(0, ship_x, ship_y); // set ship sprite inital position
    SHOW_SPRITES; // display sprites
    
    // game loop
    while(1) {
        wait_vbl_done();

        updateKeys(); // check key presses
        // move sprite if joykey pressed
        if (keyPressed(J_RIGHT) || keyPressed(J_LEFT) || keyPressed(J_UP) || keyPressed(J_DOWN)){
            if (keyPressed(J_RIGHT)){
                ship_x++;
                set_sprite_tile(0, SHIP_TYPE_LR);
                set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
            }
            if(keyPressed(J_LEFT)) {
                ship_x--;
                set_sprite_tile(0, SHIP_TYPE_LR);
                set_sprite_prop(0, get_sprite_prop(0) | S_FLIPX);
            }
            if(keyPressed(J_UP)) {
                ship_y--;
                set_sprite_tile(0, SHIP_TYPE_UD);
                set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPY);
            }
            if(keyPressed(J_DOWN)) {
                ship_y++;
                set_sprite_tile(0, SHIP_TYPE_UD);
                set_sprite_prop(0, get_sprite_prop(0) | S_FLIPY);
            }
            move_sprite(0, ship_x, ship_y); // move sprite
        }
        gbt_update();
        // delay to control frame rate
        //performantDelay(2);
    }
}