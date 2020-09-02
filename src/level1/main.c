#include <gb/gb.h>
#include <gb/cgb.h>
#include <gb/drawing.h>
#include "playerTiles.h"
#include "levelMap.c"

#define BLANK_SPRITE_INDEX 0

UINT8 player_x, player_y, player_dir, player_type;
UINT8 player_jump_count;
UINT8 player_walk_count;
UINT8 player_attack_count;
UINT8 player_is_walking;
UINT8 player_is_attacking;
UINT8 player_is_on_floor;
UINT8 player_is_jumping;
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
    set_sprite_data(0, 30, playerTiles);
    set_sprite_tile(0, 0);
    set_sprite_tile(1, 2);

    UWORD spritePalette[] = {0, RGB(7,23,8), RGB(4,14,6), RGB(28,29,5)};

    set_sprite_palette(0, 1, spritePalette);
}

void PlayerState(UINT8 x, UINT8 y, UINT8 dir, UINT8 type){
    //Dir
    //0: left
    //1: right

    //Type
    //0: normal
    //1: walk frame 1
    //2: walk frame 2
    //3: walk frame 3
    //4: jump frame 1
    //5: jump frame 2 (falling)
    //6: attack frame 1

    if (type == 0){ //normal
        set_sprite_tile(0, 0);
        set_sprite_tile(1, 2);
    } else if (type == 1){ //walk frame 1
        set_sprite_tile(0, 4);
        set_sprite_tile(1, 6);
    } else if (type == 2){ //walk frame 2
        set_sprite_tile(0, 8);
        set_sprite_tile(1, 10);
    } else if (type == 3){ //walk frame 3
        set_sprite_tile(0, 12);
        set_sprite_tile(1, 14);
    } else if (type == 4){ //jump frame 1
        set_sprite_tile(0, 16);
        set_sprite_tile(1, 18);
    } else if (type == 5){ //jump frame 2
        set_sprite_tile(0, 20);
        set_sprite_tile(1, 22);
    } else if (type == 6){ //attack frame 1
        set_sprite_tile(0, 24);
        set_sprite_tile(1, 26);
    } else if (type == 7){ //attack frame 2
        set_sprite_tile(0, 28);
        set_sprite_tile(1, 30);
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
    player_attack_count = 0;
    player_is_jumping = 0;
    player_is_walking = 0;
    player_is_on_floor = 0;
    player_is_attacking = 0;
    player_walk_index = 0;
    floor_y = 104;

    // load backgrounds
    //set_bkg_data(0, 20, level1Tiles ); //load background tile set
    //set_bkg_tiles( 0, 0, level1MapWidth, level1MapHeight, level1Map); //load background map
    UWORD bkgPalette[] = { RGB(28,29,5), RGB(7,23,8), RGB(4,14,6), RGB(0,7,5)};
    set_bkg_palette(0, 1, bkgPalette);
    set_bkg_data(0, background1_tile_count, background1_tile_data ); //load background tile set
    set_bkg_tiles( 0, 0, background1_tile_map_width, background1_tile_map_height, background1_map_data); //load background map
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

        updateKeys(); // check key presses

        if((joypad() != J_RIGHT) && (joypad() != J_LEFT)){
            player_is_walking = 0;
        }

        // move sprite if joykey pressed
        if (keyPressed(J_RIGHT) || keyPressed(J_LEFT)){
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
            
        }

        /*
        if(keyPressed(J_A)) {
            //attack
            player_attack_count++;
            if (player_attack_count < 10){
                player_is_attacking = 1;
            } else {
                player_is_attacking = 0;
                player_attack_count = 10;
            }
        } else {
            player_attack_count = 0;
        }
        */

        if(keyPressed(J_UP)) {
            //jump
            player_jump_count++;
            if (player_jump_count < 50){ // restrict jump time
                player_y-=3;
                player_is_jumping = 1;
            } else {
                player_jump_count = 50;
                player_is_jumping = 0;
            }
        } else {
            player_is_jumping = 0;
            player_jump_count = 0;
        }

        if(keyPressed(J_DOWN)) {
            player_y+=1;
        }

        //Add gravity
        player_y = player_y + 1;

        //Check floor collision
        if (player_y >= floor_y){
            player_y = floor_y;
            player_is_on_floor = 1;
        } else {
            player_is_on_floor = 0;
        }

        if (player_is_attacking == 1){
            player_type = 6; //set player type to attack frame 1
        } else if (player_is_on_floor == 0){
            if (player_is_jumping == 1){
                player_type = 4; //set player type to jump frame 1
            } else {
                player_type = 5; //set player type to jump frame 2 (fall)
            }
        } else {
            if (player_is_walking == 1){
                if (player_walk_count > 4){
                    if (player_walk_index == 0){
                        player_type = 0; //set player type to normal frame
                    } else if (player_walk_index == 1) {
                        player_type = 1; //set player type to walk frame 1
                    } else if (player_walk_index == 2){
                        player_type = 2; //set player type to walk frame 2
                    } else if (player_walk_index == 3){
                        player_type = 3; //set player type to walk frame 3
                    }
                    player_walk_index++;
                    if (player_walk_index > 3){
                        player_walk_index = 0;
                    }
                    player_walk_count = 0;
                }
                player_walk_count++;
            } else {
                player_walk_count = 0;
                player_type = 0;
            }
        }

        if (player_y <= 10){
            player_y = 10;
        }

        PlayerState(player_x,player_y,player_dir,player_type);
    }
}