#include <gb/gb.h>
#include <gb/drawing.h>
#include "playerTiles.h"
#include "playerWalkTiles.h"
#include "level1Map.h"
#include "level1Tiles.h"

#define BLANK_SPRITE_INDEX 0

UINT8 player_x, player_y, player_dir, player_type;
UINT8 player_jump_count;
UINT8 player_walk_count;
UINT8 player_count_since_walk;
UINT8 player_is_walking;
UINT8 player_walk_index;
UINT8 floor_y;
UINT8 bkg_x, bkg_y;

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

void PlayerInit(){
    set_sprite_data(0, 8, playerTiles);
    set_sprite_tile(0, 0);
    set_sprite_tile(1, 2);
    set_sprite_data(8, 8, playerWalkTiles);
    set_sprite_tile(2, 8);
    set_sprite_tile(3, 10);
}

void PlayerState(UINT8 x, UINT8 y, UINT8 dir, UINT8 type){
    //Dir
    //0: left
    //1: right

    //Type
    //0: normal
    //1: jump
    //2: walk frame 1
    //3: walk frame 2

    if (type == 0){ //normal
        set_sprite_tile(0, 0);
        set_sprite_tile(1, 2);
    } else if (type == 1){ //jump
        set_sprite_tile(0, 4);
        set_sprite_tile(1, 6);
    } else if (type == 2){ //walk frame 1
        set_sprite_tile(0, 8);
        set_sprite_tile(1, 10);
    } else if (type == 3){ //walk frame 2
        set_sprite_tile(0, 12);
        set_sprite_tile(1, 14);
    }

    if (dir == 0){
        move_sprite(0, x, y);
        move_sprite(1, x + 8, y);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPY);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPY);
    } else if (dir == 1){
        move_sprite(0, x + 8, y);
        move_sprite(1, x, y);
        set_sprite_prop(0, get_sprite_prop(0) | S_FLIPX);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPY);
        set_sprite_prop(1, get_sprite_prop(1) | S_FLIPX);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPY);
    }
}

void main() {
    player_x = 83, player_y = 83, player_dir = 0, player_type = 0;
    bkg_x = 0, bkg_y = 0;
    player_jump_count = 0;
    player_walk_count = 0;
    player_count_since_walk = 0;
    player_is_walking = 0;
    player_walk_index = 0;
    floor_y = 104;

    // load backgrounds
    set_bkg_data(0, 20, level1Tiles ); //load background tile set
    set_bkg_tiles( 0, 0, level1MapWidth, level1MapHeight, level1Map); //load background map
    scroll_bkg(bkg_x, bkg_y);
    SHOW_BKG;

    // initalise character sprite
    SPRITES_8x16;
    PlayerInit();
    PlayerState(player_x,player_y,player_dir,player_type);
    
    SHOW_SPRITES; // display sprites
    
    // game loop
    while(1) {
        wait_vbl_done();
        //player_is_walking = 0;

        updateKeys(); // check key presses

        if((joypad() != J_RIGHT) && (joypad() != J_LEFT)){
            player_is_walking = 0;
        }

        // move sprite if joykey pressed
        if (keyPressed(J_RIGHT) || keyPressed(J_LEFT) || keyPressed(J_UP) || keyPressed(J_DOWN)){
            if (keyPressed(J_RIGHT)){
                //player_x++;
                player_dir = 0;
                //player_type = 0;
                scroll_bkg(1, 0);
                player_is_walking = 1;
            }
            if(keyPressed(J_LEFT)) {
                //player_x--;
                player_dir = 1;
                //player_type = 0;
                scroll_bkg(-1, 0);
                player_is_walking = 1;
            }
            if(keyPressed(J_UP)) {
                //jump
                player_jump_count++;
                if (player_jump_count < 10){ // restrict jump time
                    player_y-=3;
                } else {
                    player_jump_count = 10;
                }
            } else {
                player_jump_count = 0;
            }
        }
        //Add gravity
        player_y = player_y + 1;

        //Check floor collision
        if (player_y > floor_y){
            player_y = floor_y;
            if (player_is_walking == 1){
                if (player_walk_count > 4){
                    if (player_walk_index == 0){
                        player_type = 0; //set player type to normal frame
                    } else if (player_walk_index == 1) {
                        player_type = 2; //set player type to walk frame 1
                    } else if (player_walk_index == 2){
                        player_type = 3; //set player type to walk frame 2
                    }
                    player_walk_index++;
                    if (player_walk_index > 2){
                        player_walk_index = 0;
                    }
                    player_walk_count = 0;
                }
                player_walk_count++;
            } else {
                player_walk_count = 0;
                player_type = 0;
            }
        } else {
            player_type = 1; //set player type to jump frame
        }
        PlayerState(player_x,player_y,player_dir,player_type);
    }
}