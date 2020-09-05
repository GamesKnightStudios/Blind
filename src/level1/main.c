#include <gb/gb.h>
#include <gb/cgb.h>
#include <gb/drawing.h>
#include "playerTiles.h"
#include "levelTiles.c"
#include "level1Map.c"
#include "level2Map.c"

#define BLANK_SPRITE_INDEX 0

#define OBJECT_X 0
#define OBJECT_Y 1
#define OBJECT_W 2
#define OBJECT_H 3
#define OBJECT_DIR 4
#define OBJECT_TILE 5

UINT8 player_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE
UINT8 test_player_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE
UINT8 prev_player_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE
UINT8 platform1_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE
UINT8 platform2_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE
UINT8 platform3_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE
UINT8 platform4_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE
UINT8 platform5_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE 

UINT8 player_move_x;
UINT8 player_move_y;

UINT8 new_level;

UINT8 level_num;

UINT8 player_jump_count;
UINT8 player_walk_count;
UINT8 player_attack_count;
UINT8 player_attack_index;
UINT8 player_is_walking;
UINT8 player_is_attacking;
UINT8 player_is_on_floor;
UINT8 player_is_jumping;
UINT8 player_walk_index;
UINT8 bkg_x, bkg_y;

UWORD bkgPalette_normal[] = { RGB(28,29,5), RGB(7,23,8), RGB(4,14,6), RGB(0,7,5)};
UWORD bkgPalette_blank[] = { RGB(0,7,5), RGB(0,7,5), RGB(0,7,5), RGB(0,7,5)};
UWORD bkgPalette_fade1[] = { RGB(7,23,8), RGB(7,23,8), RGB(4,14,6), RGB(0,7,5)};
UWORD bkgPalette_fade2[] = { RGB(4,14,6), RGB(7,23,8), RGB(4,14,6), RGB(0,7,5)};

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

// Check if two rectangles from x1,y1, and extending out w1, h1, 
//  overlap with another, x2,y2, and extending out w2, h2
UINT8 collisionCheck(UINT8 x1, UINT8 y1, UINT8 w1, UINT8 h1, UINT8 x2, UINT8 y2, UINT8 w2, UINT8 h2) {
	if ((x1 < (x2+w2)) && ((x1+w1) > x2) && (y1 < (h2+y2)) && ((y1+h1) > y2)) {
		
		return 1;
	
	} else {
		
		return 0;
	
	}
}

UINT8 collisionCheckDir(UINT8 x1, UINT8 y1, UINT8 w1, UINT8 h1, UINT8 x2, UINT8 y2, UINT8 w2, UINT8 h2) {
	if ((x1 < (x2+w2)) && ((x1+w1) > x2) && (y1 < (h2+y2)) && ((y1+h1) > y2)) {
		
		return 1;
	
	} else {
		
		return 0;
	
	}
}

UINT8 collisionStateCheck(UINT8 stateA[6], UINT8 stateB[6]){
    return collisionCheck(
        stateA[OBJECT_X],stateA[OBJECT_Y],stateA[OBJECT_W],stateA[OBJECT_H],
        stateB[OBJECT_X],stateB[OBJECT_Y],stateB[OBJECT_W],stateB[OBJECT_H]
    );
}

UINT8 resetFromCollision(UINT8 prevstateA[6], UINT8 stateA[6], UINT8 stateB[6]){
    //ONLY WORKS IF DONE ON X AND THEN Y (NO DIAGONALS!!)
    if(collisionStateCheck(stateA,stateB)){
        stateA[OBJECT_Y] = prevstateA[OBJECT_Y];
        stateA[OBJECT_X] = prevstateA[OBJECT_X];
        return 1;
    } else {
        return 0;
    }
}

void PlayerInit(){
    set_sprite_data(0, 52, playerTiles);
    set_sprite_tile(0, 4);
    set_sprite_tile(1, 8);

    UWORD spritePalette[] = {0, RGB(7,23,8), RGB(4,14,6), RGB(28,29,5)};

    set_sprite_palette(0, 1, spritePalette);
}

void PlayerState(UINT8 x, UINT8 y, UINT8 dir, UINT8 type){
    //Dir
    //0: left
    //1: right

    //Type
    //1: normal
    //2: walk frame 1
    //3: walk frame 2
    //4: walk frame 3
    //5: jump frame 1
    //6: jump frame 2 (falling)
    //7: attack frame 1
    //8: attack frame 2
    //9: attack frame 3
    //10: attack frame 4
    //11: attack frame 5
    //12: attack frame 6

    if (type == 0){ //hide
        set_bkg_palette(0, 1, bkgPalette_blank);
        set_sprite_tile(0, 0);
        set_sprite_tile(1, 2);
    } else if (type == 1){ //normal
        set_bkg_palette(0, 1, bkgPalette_blank);
        set_sprite_tile(0, 4);
        set_sprite_tile(1, 6);
    } else if (type == 2){ //walk frame 1
        set_bkg_palette(0, 1, bkgPalette_blank);
        set_sprite_tile(0, 8);
        set_sprite_tile(1, 10);
    } else if (type == 3){ //walk frame 2
        set_bkg_palette(0, 1, bkgPalette_blank);
        set_sprite_tile(0, 12);
        set_sprite_tile(1, 14);
    } else if (type == 4){ //walk frame 3
        set_bkg_palette(0, 1, bkgPalette_blank);
        set_sprite_tile(0, 16);
        set_sprite_tile(1, 18);
    } else if (type == 5){ //jump frame 1
        set_bkg_palette(0, 1, bkgPalette_blank);
        set_sprite_tile(0, 20);
        set_sprite_tile(1, 22);
    } else if (type == 6){ //jump frame 2
        set_bkg_palette(0, 1, bkgPalette_blank);
        set_sprite_tile(0, 24);
        set_sprite_tile(1, 26);
    } else if (type == 7){ //attack frame 1
        set_bkg_palette(0, 1, bkgPalette_blank);
        set_sprite_tile(0, 28);
        set_sprite_tile(1, 30);
    } else if (type == 8){ //attack frame 2
        set_bkg_palette(0, 1, bkgPalette_blank);
        set_sprite_tile(0, 32);
        set_sprite_tile(1, 34);
    } else if (type == 9){ //attack frame 3
        set_bkg_palette(0, 1, bkgPalette_blank);
        set_sprite_tile(0, 36);
        set_sprite_tile(1, 38);
    } else if (type == 10){ //attack frame 4
        set_bkg_palette(0, 1, bkgPalette_fade1);
        set_sprite_tile(0, 40);
        set_sprite_tile(1, 42);
    } else if (type == 11){ //attack frame 5
        set_bkg_palette(0, 1, bkgPalette_normal);
        set_sprite_tile(0, 44);
        set_sprite_tile(1, 46);
    } else if (type == 12){ //attack frame 6
        set_bkg_palette(0, 1, bkgPalette_fade2);
        set_sprite_tile(0, 48);
        set_sprite_tile(1, 50);
    }

    if (dir == 0){
        move_sprite(0, x + 4, y + 16);
        move_sprite(1, x + 12, y + 16);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPY);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPY);
    } else if (dir == 1){
        move_sprite(0, x + 12, y + 16);
        move_sprite(1, x + 4, y + 16);
        set_sprite_prop(0, get_sprite_prop(0) | S_FLIPX);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPY);
        set_sprite_prop(1, get_sprite_prop(1) | S_FLIPX);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPY);
    }
}

void checkSceneCollisons(){
    if (resetFromCollision(prev_player_state, player_state, platform1_state)){
        player_is_on_floor = 1;
    }
    if (resetFromCollision(prev_player_state, player_state, platform2_state)){
        player_is_on_floor = 1;
    }
    if (resetFromCollision(prev_player_state, player_state, platform3_state)){
        player_is_on_floor = 1;
    }
    if (resetFromCollision(prev_player_state, player_state, platform4_state)){
        player_is_on_floor = 1;
    }
    if (resetFromCollision(prev_player_state, player_state, platform5_state)){
        player_is_on_floor = 1;
    }

    // make sure player doesn't go outside screen
    if (player_state[OBJECT_Y] <= 0 || player_state[OBJECT_Y] > 200){
        player_state[OBJECT_Y] = 0;
    }

    if (player_state[OBJECT_Y] > 140){
        player_state[OBJECT_Y] = 140;
    }

    if (player_state[OBJECT_X] <= 2){
        player_state[OBJECT_X] = 2;
    }

    if (player_state[OBJECT_X] > 150){
        player_state[OBJECT_X] = 150;
    }
}

void setupLevel(UINT8 level){
    if (level == 1){
        set_bkg_data(0, 18, levelTiles ); //load background tile set
        set_bkg_tiles( 0, 0, 20, 18, level2Map); //load background map
        //set_bkg_data(0, level1_tile_count, level1_tile_data ); //load background tile set
        //set_bkg_tiles( 0, 0, level1_tile_map_width, level1_tile_map_height, level1_map_data); //load background map
        platform1_state[OBJECT_X] = 0;
        platform1_state[OBJECT_Y] = 120;
        platform1_state[OBJECT_W] = 160;
        platform1_state[OBJECT_H] = 24;
        platform1_state[OBJECT_DIR] = 0;
        platform1_state[OBJECT_TILE] = 0;

        platform2_state[OBJECT_X] = 0;
        platform2_state[OBJECT_Y] = 0;
        platform2_state[OBJECT_W] = 0;
        platform2_state[OBJECT_H] = 0;
        platform2_state[OBJECT_DIR] = 0;
        platform2_state[OBJECT_TILE] = 0;

        platform3_state[OBJECT_X] = 0;
        platform3_state[OBJECT_Y] = 0;
        platform3_state[OBJECT_W] = 0;
        platform3_state[OBJECT_H] = 0;
        platform3_state[OBJECT_DIR] = 0;
        platform3_state[OBJECT_TILE] = 0;

        platform4_state[OBJECT_X] = 0;
        platform4_state[OBJECT_Y] = 0;
        platform4_state[OBJECT_W] = 0;
        platform4_state[OBJECT_H] = 0;
        platform4_state[OBJECT_DIR] = 0;
        platform4_state[OBJECT_TILE] = 0;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;
        platform5_state[OBJECT_DIR] = 0;
        platform5_state[OBJECT_TILE] = 0;
    } else if (level == 2){
        //set_bkg_data(0, 18, levelTiles ); //load background tile set
        //set_bkg_tiles( 0, 0, 20, 18, level2Map); //load background map
        //set_bkg_data(0, level2_tile_count, level2_tile_data ); //load background tile set
        //set_bkg_tiles( 0, 0, level2_tile_map_width, level2_tile_map_height, level2_map_data); //load background map
        platform1_state[OBJECT_X] = 0;
        platform1_state[OBJECT_Y] = 120;
        platform1_state[OBJECT_W] = 23;
        platform1_state[OBJECT_H] = 24;
        platform1_state[OBJECT_DIR] = 0;
        platform1_state[OBJECT_TILE] = 0;

        platform2_state[OBJECT_X] = 40;
        platform2_state[OBJECT_Y] = 120;
        platform2_state[OBJECT_W] = 79;
        platform2_state[OBJECT_H] = 15;
        platform2_state[OBJECT_DIR] = 0;
        platform2_state[OBJECT_TILE] = 0;

        platform3_state[OBJECT_X] = 136;
        platform3_state[OBJECT_Y] = 120;
        platform3_state[OBJECT_W] = 15;
        platform3_state[OBJECT_H] = 15;
        platform3_state[OBJECT_DIR] = 0;
        platform3_state[OBJECT_TILE] = 0;

        platform4_state[OBJECT_X] = 0;
        platform4_state[OBJECT_Y] = 0;
        platform4_state[OBJECT_W] = 0;
        platform4_state[OBJECT_H] = 0;
        platform4_state[OBJECT_DIR] = 0;
        platform4_state[OBJECT_TILE] = 0;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;
        platform5_state[OBJECT_DIR] = 0;
        platform5_state[OBJECT_TILE] = 0;
    } else if (level == 3){
        //set_bkg_data(0, level3_tile_count, level3_tile_data ); //load background tile set
        //set_bkg_tiles( 0, 0, level3_tile_map_width, level3_tile_map_height, level3_map_data); //load background map
        platform1_state[OBJECT_X] = 0;
        platform1_state[OBJECT_Y] = 120;
        platform1_state[OBJECT_W] = 160;
        platform1_state[OBJECT_H] = 24;
        platform1_state[OBJECT_DIR] = 0;
        platform1_state[OBJECT_TILE] = 0;

        platform2_state[OBJECT_X] = 0;
        platform2_state[OBJECT_Y] = 0;
        platform2_state[OBJECT_W] = 0;
        platform2_state[OBJECT_H] = 0;
        platform2_state[OBJECT_DIR] = 0;
        platform2_state[OBJECT_TILE] = 0;

        platform3_state[OBJECT_X] = 0;
        platform3_state[OBJECT_Y] = 0;
        platform3_state[OBJECT_W] = 0;
        platform3_state[OBJECT_H] = 0;
        platform3_state[OBJECT_DIR] = 0;
        platform3_state[OBJECT_TILE] = 0;

        platform4_state[OBJECT_X] = 0;
        platform4_state[OBJECT_Y] = 0;
        platform4_state[OBJECT_W] = 0;
        platform4_state[OBJECT_H] = 0;
        platform4_state[OBJECT_DIR] = 0;
        platform4_state[OBJECT_TILE] = 0;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;
        platform5_state[OBJECT_DIR] = 0;
        platform5_state[OBJECT_TILE] = 0;
    } else if (level == 4){
        //set_bkg_data(0, level4_tile_count, level4_tile_data ); //load background tile set
        //set_bkg_tiles( 0, 0, level4_tile_map_width, level4_tile_map_height, level4_map_data); //load background map
        platform1_state[OBJECT_X] = 0;
        platform1_state[OBJECT_Y] = 120;
        platform1_state[OBJECT_W] = 160;
        platform1_state[OBJECT_H] = 24;
        platform1_state[OBJECT_DIR] = 0;
        platform1_state[OBJECT_TILE] = 0;

        platform2_state[OBJECT_X] = 0;
        platform2_state[OBJECT_Y] = 0;
        platform2_state[OBJECT_W] = 0;
        platform2_state[OBJECT_H] = 0;
        platform2_state[OBJECT_DIR] = 0;
        platform2_state[OBJECT_TILE] = 0;

        platform3_state[OBJECT_X] = 0;
        platform3_state[OBJECT_Y] = 0;
        platform3_state[OBJECT_W] = 0;
        platform3_state[OBJECT_H] = 0;
        platform3_state[OBJECT_DIR] = 0;
        platform3_state[OBJECT_TILE] = 0;

        platform4_state[OBJECT_X] = 0;
        platform4_state[OBJECT_Y] = 0;
        platform4_state[OBJECT_W] = 0;
        platform4_state[OBJECT_H] = 0;
        platform4_state[OBJECT_DIR] = 0;
        platform4_state[OBJECT_TILE] = 0;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;
        platform5_state[OBJECT_DIR] = 0;
        platform5_state[OBJECT_TILE] = 0;
    } else if (level == 5){
        //set_bkg_data(0, level5_tile_count, level5_tile_data ); //load background tile set
        //set_bkg_tiles( 0, 0, level5_tile_map_width, level5_tile_map_height, level5_map_data); //load background map
        platform1_state[OBJECT_X] = 0;
        platform1_state[OBJECT_Y] = 120;
        platform1_state[OBJECT_W] = 160;
        platform1_state[OBJECT_H] = 24;
        platform1_state[OBJECT_DIR] = 0;
        platform1_state[OBJECT_TILE] = 0;

        platform2_state[OBJECT_X] = 0;
        platform2_state[OBJECT_Y] = 0;
        platform2_state[OBJECT_W] = 0;
        platform2_state[OBJECT_H] = 0;
        platform2_state[OBJECT_DIR] = 0;
        platform2_state[OBJECT_TILE] = 0;

        platform3_state[OBJECT_X] = 0;
        platform3_state[OBJECT_Y] = 0;
        platform3_state[OBJECT_W] = 0;
        platform3_state[OBJECT_H] = 0;
        platform3_state[OBJECT_DIR] = 0;
        platform3_state[OBJECT_TILE] = 0;

        platform4_state[OBJECT_X] = 0;
        platform4_state[OBJECT_Y] = 0;
        platform4_state[OBJECT_W] = 0;
        platform4_state[OBJECT_H] = 0;
        platform4_state[OBJECT_DIR] = 0;
        platform4_state[OBJECT_TILE] = 0;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;
        platform5_state[OBJECT_DIR] = 0;
        platform5_state[OBJECT_TILE] = 0;
    } else if (level == 6){
        //set_bkg_data(0, level6_tile_count, level6_tile_data ); //load background tile set
        //set_bkg_tiles( 0, 0, level6_tile_map_width, level6_tile_map_height, level6_map_data); //load background map
        platform1_state[OBJECT_X] = 0;
        platform1_state[OBJECT_Y] = 120;
        platform1_state[OBJECT_W] = 160;
        platform1_state[OBJECT_H] = 24;
        platform1_state[OBJECT_DIR] = 0;
        platform1_state[OBJECT_TILE] = 0;

        platform2_state[OBJECT_X] = 0;
        platform2_state[OBJECT_Y] = 0;
        platform2_state[OBJECT_W] = 0;
        platform2_state[OBJECT_H] = 0;
        platform2_state[OBJECT_DIR] = 0;
        platform2_state[OBJECT_TILE] = 0;

        platform3_state[OBJECT_X] = 0;
        platform3_state[OBJECT_Y] = 0;
        platform3_state[OBJECT_W] = 0;
        platform3_state[OBJECT_H] = 0;
        platform3_state[OBJECT_DIR] = 0;
        platform3_state[OBJECT_TILE] = 0;

        platform4_state[OBJECT_X] = 0;
        platform4_state[OBJECT_Y] = 0;
        platform4_state[OBJECT_W] = 0;
        platform4_state[OBJECT_H] = 0;
        platform4_state[OBJECT_DIR] = 0;
        platform4_state[OBJECT_TILE] = 0;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;
        platform5_state[OBJECT_DIR] = 0;
        platform5_state[OBJECT_TILE] = 0;
    }
}

void transition(UINT8 next_level){
    while (1){
        if (player_attack_count > 5){
            if (player_attack_index == 0){
                player_state[OBJECT_TILE] = 7; //set player type to attack frame 1
            } else if (player_attack_index == 1) {
                player_state[OBJECT_TILE] = 7; //set player type to attack frame 1
            } else if (player_attack_index == 2) {
                player_state[OBJECT_TILE] = 8; //set player type to attack frame 2
            } else if (player_attack_index == 3) {
                player_state[OBJECT_TILE] = 8; //set player type to attack frame 2
            } else if (player_attack_index == 4) {
                player_state[OBJECT_TILE] = 9; //set player type to attack frame 3
            } else if (player_attack_index == 5) {
                player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            } else if (player_attack_index == 6) {
                player_state[OBJECT_TILE] = 11; //set player type to attack frame 5
            } else if (player_attack_index == 7) {
                player_state[OBJECT_TILE] = 11; //set player type to attack frame 5
            } else if (player_attack_index == 8) {
                player_state[OBJECT_TILE] = 12; //set player type to attack frame 6
            } else if (player_attack_index == 9) {
                player_state[OBJECT_TILE] = 12; //set player type to attack frame 6
            } else if (player_attack_index == 10) {
                player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            } else if (player_attack_index == 11) {
                player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            } else if (player_attack_index == 12) {
                player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            } else if (player_attack_index == 13) {
                player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            } else if (player_attack_index == 14) {
                player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            }
            player_attack_index++;
            if (player_attack_index > 14){
                player_attack_index = 14;
                break;
            }
            player_attack_count = 0;
        }
        player_attack_count++;
        PlayerState(player_state[OBJECT_X],player_state[OBJECT_Y],player_state[OBJECT_DIR],player_state[OBJECT_TILE]);
        wait_vbl_done();
    }
    player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
    PlayerState(player_state[OBJECT_X],player_state[OBJECT_Y],player_state[OBJECT_DIR],player_state[OBJECT_TILE]);
    wait_vbl_done();
    setupLevel(next_level);
    for(UINT8 i = player_state[OBJECT_X]; i > 2; i-=1){
        player_state[OBJECT_X] = i;
        PlayerState(player_state[OBJECT_X],player_state[OBJECT_Y],player_state[OBJECT_DIR],player_state[OBJECT_TILE]);
        set_bkg_palette(0, 1, bkgPalette_blank); //keep background hidden
        wait_vbl_done();
    }
}

void checkLevelComplete(){
    if (level_num == 1){
        if(keyPressed(J_A)){
            new_level = 1;
        }
    } else if (level_num == 2){
        test_player_state[OBJECT_Y] = player_state[OBJECT_Y] + 1;
        if (collisionStateCheck(test_player_state,platform3_state)){
            new_level = 1;
            /*
            if (player_state[OBJECT_X] > platform3_state[OBJECT_X]){
                if (player_state[OBJECT_Y] < platform3_state[OBJECT_Y]){
                    new_level = 1;
                }
            }
            */
        }
        
    }
}

void main() {
    player_state[OBJECT_X] = 2;
    player_state[OBJECT_Y] = 104;
    player_state[OBJECT_W] = 8;
    player_state[OBJECT_H] = 16;
    player_state[OBJECT_DIR] = 0;
    player_state[OBJECT_TILE] = 1;

    prev_player_state[OBJECT_X] = player_state[OBJECT_X];
    prev_player_state[OBJECT_Y] = player_state[OBJECT_Y];
    prev_player_state[OBJECT_W] = player_state[OBJECT_W];
    prev_player_state[OBJECT_H] = player_state[OBJECT_H];
    prev_player_state[OBJECT_DIR] = player_state[OBJECT_DIR];
    prev_player_state[OBJECT_TILE] = player_state[OBJECT_TILE];

    level_num = 1;
    bkg_x = 0, bkg_y = 0;
    player_jump_count = 0;
    player_walk_count = 0;
    player_attack_count = 0;
    player_attack_index = 0;
    player_is_jumping = 0;
    player_is_walking = 0;
    player_is_on_floor = 0;
    player_is_attacking = 0;
    player_walk_index = 0;

    // load backgrounds
    set_bkg_palette(0, 1, bkgPalette_blank);
    setupLevel(1);
	scroll_bkg(0, 0);
    SHOW_BKG;

    new_level = 0;

    // initalise character sprite
    SPRITES_8x16;
    PlayerInit();
    PlayerState(player_state[OBJECT_X],player_state[OBJECT_Y],player_state[OBJECT_DIR],player_state[OBJECT_TILE]);
    SHOW_SPRITES; // display sprites

    // game loop
    while(1) {
        if (new_level == 1){
            transition(level_num+1);
            level_num++;
            new_level = 0;
        }

        player_move_x = 0;
        player_move_y = 0;

        wait_vbl_done();

        updateKeys(); // check key presses

        if((joypad() != J_RIGHT) && (joypad() != J_LEFT)){
            player_is_walking = 0;
        }

        if(keyPressed(J_A)) {
            //attack
            player_attack_count++;
            if (player_attack_count < 50){
                player_is_attacking = 1;
            } else {
                player_is_attacking = 0;
                player_attack_count = 50;
            }
        } else {
            player_is_attacking = 0;
            player_attack_count = 0;
            player_attack_index = 0;
        }

        if (player_is_attacking == 0){
            // move sprite if joykey pressed
            if (keyPressed(J_RIGHT) || keyPressed(J_LEFT)){
                if (keyPressed(J_RIGHT)){
                    player_move_x++;
                    player_state[OBJECT_DIR] = 0;
                    player_is_walking = 1;
                }
                if(keyPressed(J_LEFT)) {
                    player_move_x--;
                    player_state[OBJECT_DIR] = 1;
                    player_is_walking = 1;
                }
            }

            if(keyPressed(J_UP)) {
                //jump
                player_jump_count++;
                if (player_jump_count < 50){ // restrict jump time
                    player_move_y-=3;
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
                player_move_y+=1;
            }
        }

        //Add gravity
        player_move_y++;

        //move x
        player_state[OBJECT_X] = player_state[OBJECT_X] + player_move_x;

        player_is_on_floor = 0;

        //check collision
        checkSceneCollisons();

        prev_player_state[OBJECT_X] = player_state[OBJECT_X];
        prev_player_state[OBJECT_Y] = player_state[OBJECT_Y];

        // move y
        player_state[OBJECT_Y] = player_state[OBJECT_Y] + player_move_y;

        // check collision
        checkSceneCollisons();

        if (player_is_on_floor == 0){
            if (player_is_jumping == 1){
                player_state[OBJECT_TILE] = 5; //set player type to jump frame 1
            } else {
                player_state[OBJECT_TILE] = 6; //set player type to jump frame 2 (fall)
            }
        } else {
            if (player_is_attacking == 1){
                if (player_attack_count > 10){
                    if (player_attack_index == 0){
                        player_state[OBJECT_TILE] = 7; //set player type to attack frame 1
                    } else if (player_attack_index == 1) {
                        player_state[OBJECT_TILE] = 7; //set player type to attack frame 1
                    } else if (player_attack_index == 2) {
                        player_state[OBJECT_TILE] = 8; //set player type to attack frame 2
                    } else if (player_attack_index == 3) {
                        player_state[OBJECT_TILE] = 8; //set player type to attack frame 2
                    } else if (player_attack_index == 4) {
                        player_state[OBJECT_TILE] = 9; //set player type to attack frame 3
                    } else if (player_attack_index == 5) {
                        player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
                    } else if (player_attack_index == 6) {
                        player_state[OBJECT_TILE] = 11; //set player type to attack frame 5
                    } else if (player_attack_index == 7) {
                        player_state[OBJECT_TILE] = 11; //set player type to attack frame 5
                    } else if (player_attack_index == 8) {
                        player_state[OBJECT_TILE] = 12; //set player type to attack frame 6
                    } else if (player_attack_index == 9) {
                        player_state[OBJECT_TILE] = 12; //set player type to attack frame 6
                    } else if (player_attack_index == 10) {
                        player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
                    }
                    player_attack_index++;
                    if (player_attack_index > 10){
                        player_attack_index = 10;
                    }
                    player_attack_count = 0;
                }
                player_attack_count++;
            }
            else if (player_is_walking == 1){
                if (player_walk_count > 10){
                    if (player_walk_index == 0){
                        player_state[OBJECT_TILE] = 2; //set player type to walk frame 1
                    } else if (player_walk_index == 1) {
                        player_state[OBJECT_TILE] = 3; //set player type to walk frame 2
                    } else if (player_walk_index == 2){
                        player_state[OBJECT_TILE] = 4; //set player type to walk frame 3
                    } else if (player_walk_index == 3){
                        player_state[OBJECT_TILE] = 1; //set player type to normal frame
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
                player_state[OBJECT_TILE] = 1;
            }
        }

        PlayerState(player_state[OBJECT_X],player_state[OBJECT_Y],player_state[OBJECT_DIR],player_state[OBJECT_TILE]);

        prev_player_state[OBJECT_X] = player_state[OBJECT_X];
        prev_player_state[OBJECT_Y] = player_state[OBJECT_Y];
        prev_player_state[OBJECT_W] = player_state[OBJECT_W];
        prev_player_state[OBJECT_H] = player_state[OBJECT_H];
        prev_player_state[OBJECT_DIR] = player_state[OBJECT_DIR];
        prev_player_state[OBJECT_TILE] = player_state[OBJECT_TILE];

        test_player_state[OBJECT_X] = player_state[OBJECT_X];
        test_player_state[OBJECT_Y] = player_state[OBJECT_Y];
        test_player_state[OBJECT_W] = player_state[OBJECT_W];
        test_player_state[OBJECT_H] = player_state[OBJECT_H];
        test_player_state[OBJECT_DIR] = player_state[OBJECT_DIR];
        test_player_state[OBJECT_TILE] = player_state[OBJECT_TILE];

        checkLevelComplete();
    }
}