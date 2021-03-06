#include <gb/gb.h>
#include <gb/cgb.h>
#include <gb/drawing.h>
#include "playerTiles.c"
#include "powerupTiles.c"
#include "deathEffectTiles.c"
#include "levelTiles.c"
#include "level1Map.c"
#include "level2Map.c"
#include "level3Map.c"
#include "level4Map.c"
#include "level5Map.c"
#include "level6Map.c"
#include "levelEndMap.c"
#include "music.c"

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

UINT8 powerup1_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE (SMALL POWER UP)
UINT8 powerup2_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE (MEDIUM POWER UP)
UINT8 powerup3_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE (LARGE POWER UP)

UINT8 effect1_state[6] = {0, 0, 0, 0, 0, 0}; //X,Y,W,H,DIR,TILE (DEATH EFFECT)

UINT8 background_type;

UINT8 level_start_xs[7] = {72,8,8,8,136,8,76};
UINT8 level_start_ys[7] = {96,96,96,96,96,96,88};

UINT8 time_in_air;
UINT8 time_boosting;

UINT8 checkpoint_level;

UINT8 player_move_x;
UINT8 player_move_y;

UINT8 player_powerup; //0: no power up, 1: small boost, 2: medium boost, 3: large boost

UINT8 new_level;

UINT8 level_num;

UINT8 death_anim_complete;
UINT8 player_death_frame_index;
UINT8 player_death_frame_count;

UINT8 attack_anim_complete;
UINT8 player_attack_frame_count;
UINT8 player_attack_frame_index;

UINT8 player_is_dead;
UINT8 player_jump_count;
UINT8 player_walk_count;
UINT8 player_is_walking;
UINT8 player_is_attacking;
UINT8 player_is_on_floor;
UINT8 player_is_jumping;
UINT8 player_walk_index;

UWORD spritePalette[] = {RGB(0,0,0), RGB(9,31,27), RGB(2,19,31), RGB(31,31,31)};

UWORD bkgPalette_normal[] = { spritePalette[3], spritePalette[1], spritePalette[2], spritePalette[0]};
UWORD bkgPalette_blank[] = { spritePalette[0], spritePalette[0], spritePalette[0], spritePalette[0]};
UWORD bkgPalette_fade1[] = { spritePalette[1], spritePalette[1], spritePalette[2], spritePalette[0]};
UWORD bkgPalette_fade2[] = { spritePalette[2], spritePalette[1], spritePalette[2], spritePalette[0]};

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
    set_sprite_data(0, 56, playerTiles);
    set_sprite_tile(0, 4);
    set_sprite_tile(1, 8);

    set_sprite_palette(0, 1, spritePalette);
}

void UpdateBackground(){
    if (background_type == 0){ //hide
        set_bkg_palette(0, 1, bkgPalette_blank);
    } else if (background_type == 1){ //normal
        set_bkg_palette(0, 1, bkgPalette_normal);
    } else if (background_type == 2){ //fade 1
        set_bkg_palette(0, 1, bkgPalette_fade2);
    } else if (background_type == 3){ //fade 2
        set_bkg_palette(0, 1, bkgPalette_fade1);
    }

    if (background_type > 0){
        if (player_powerup < 1){
            powerup1_state[OBJECT_TILE] = 1;
        } else {
            powerup1_state[OBJECT_TILE] = 0;
        }
        if (player_powerup < 2){
            powerup2_state[OBJECT_TILE] = 1;
        } else {
            powerup2_state[OBJECT_TILE] = 0;
        }
        if (player_powerup < 3){
            powerup3_state[OBJECT_TILE] = 1;
        } else {
            powerup3_state[OBJECT_TILE] = 0;
        }
    } else {
        powerup1_state[OBJECT_TILE] = 0;
        powerup2_state[OBJECT_TILE] = 0;
        powerup3_state[OBJECT_TILE] = 0;
    }
}

void UpdatePlayer(){
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

    if (player_state[OBJECT_TILE] == 0){ //hide
        set_sprite_tile(0, 0);
        set_sprite_tile(1, 2);
    } else if (player_state[OBJECT_TILE] == 1){ //normal
        set_sprite_tile(0, 4);
        set_sprite_tile(1, 6);
    } else if (player_state[OBJECT_TILE] == 2){ //walk frame 1
        set_sprite_tile(0, 8);
        set_sprite_tile(1, 10);
    } else if (player_state[OBJECT_TILE] == 3){ //walk frame 2
        set_sprite_tile(0, 12);
        set_sprite_tile(1, 14);
    } else if (player_state[OBJECT_TILE] == 4){ //walk frame 3
        set_sprite_tile(0, 16);
        set_sprite_tile(1, 18);
    } else if (player_state[OBJECT_TILE] == 5){ //jump frame 1
        set_sprite_tile(0, 20);
        set_sprite_tile(1, 22);
    } else if (player_state[OBJECT_TILE] == 6){ //jump frame 2
        set_sprite_tile(0, 24);
        set_sprite_tile(1, 26);
    } else if (player_state[OBJECT_TILE] == 7){ //attack frame 1
        set_sprite_tile(0, 28);
        set_sprite_tile(1, 30);
    } else if (player_state[OBJECT_TILE] == 8){ //attack frame 2
        set_sprite_tile(0, 32);
        set_sprite_tile(1, 34);
    } else if (player_state[OBJECT_TILE] == 9){ //attack frame 3
        set_sprite_tile(0, 36);
        set_sprite_tile(1, 38);
    } else if (player_state[OBJECT_TILE] == 10){ //attack frame 4
        set_sprite_tile(0, 40);
        set_sprite_tile(1, 42);
    } else if (player_state[OBJECT_TILE] == 11){ //attack frame 5
        set_sprite_tile(0, 44);
        set_sprite_tile(1, 46);
    } else if (player_state[OBJECT_TILE] == 12){ //attack frame 6
        set_sprite_tile(0, 48);
        set_sprite_tile(1, 50);
    } else if (player_state[OBJECT_TILE] == 13){ //jump frame 4
        set_sprite_tile(0, 52);
        set_sprite_tile(1, 54);
    }

    if (player_state[OBJECT_DIR] == 0){
        move_sprite(0, player_state[OBJECT_X] + 4, player_state[OBJECT_Y] + 16);
        move_sprite(1, player_state[OBJECT_X] + 12, player_state[OBJECT_Y] + 16);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPY);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPY);
    } else if (player_state[OBJECT_DIR] == 1){
        move_sprite(0, player_state[OBJECT_X] + 12, player_state[OBJECT_Y] + 16);
        move_sprite(1, player_state[OBJECT_X] + 4, player_state[OBJECT_Y] + 16);
        set_sprite_prop(0, get_sprite_prop(0) | S_FLIPX);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPY);
        set_sprite_prop(1, get_sprite_prop(1) | S_FLIPX);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPY);
    }
}

void EffectsInit(){
    // load death effect sprite data
    set_sprite_data(68, 14, deathEffectTiles);
    set_sprite_tile(8, 68);
    set_sprite_tile(9, 70);

    move_sprite(8, effect1_state[OBJECT_X] + 8, effect1_state[OBJECT_Y] + 16);
    move_sprite(9, effect1_state[OBJECT_X] + 16, effect1_state[OBJECT_Y] + 16);
}

void UpdateEffects(){
    if (effect1_state[OBJECT_TILE] == 0){ //hide
        set_sprite_tile(8, 0);
        set_sprite_tile(9, 2);
    } else if (effect1_state[OBJECT_TILE] == 1){ // animation frame 1
        set_sprite_tile(8, 68);
        set_sprite_tile(9, 70);
    } else if (effect1_state[OBJECT_TILE] == 2){ // animation frame 2
        set_sprite_tile(8, 72);
        set_sprite_tile(9, 74);
    } else if (effect1_state[OBJECT_TILE] == 3){ // animation frame 3
        set_sprite_tile(8, 76);
        set_sprite_tile(9, 78);
    } else if (effect1_state[OBJECT_TILE] == 4){ // animation frame 4
        set_sprite_tile(8, 80);
        set_sprite_tile(9, 82);
    } else if (effect1_state[OBJECT_TILE] == 5){ // animation frame 5
        set_sprite_tile(8, 84);
        set_sprite_tile(9, 86);
    } else if (effect1_state[OBJECT_TILE] == 6){ // animation frame 6
        set_sprite_tile(8, 88);
        set_sprite_tile(9, 90);
    } else if (effect1_state[OBJECT_TILE] == 7){ // animation frame 7
        set_sprite_tile(8, 92);
        set_sprite_tile(9, 94);
    }

    move_sprite(8, effect1_state[OBJECT_X] + 8, effect1_state[OBJECT_Y] + 16);
    move_sprite(9, effect1_state[OBJECT_X] + 16, effect1_state[OBJECT_Y] + 16);
}

void PowerupsInit(){
    // load powerup sprite data
    set_sprite_data(56, 12, powerupTiles);
    set_sprite_tile(2, 58);
    set_sprite_tile(3, 62);
    set_sprite_tile(4, 66);

    move_sprite(2, powerup1_state[OBJECT_X] + 8, powerup1_state[OBJECT_Y] + 8);
    move_sprite(3, powerup2_state[OBJECT_X] + 8, powerup2_state[OBJECT_Y] + 8);
    move_sprite(4, powerup3_state[OBJECT_X] + 8, powerup3_state[OBJECT_Y] + 8);
}

void UpdatePowerups(){
    // update powerup sprites and tiles
    // tile 0: hidden
    // tile 1: animation frame 1

    if (powerup1_state[OBJECT_TILE] == 0){ //hide
        set_sprite_tile(2, 2);
    } else if (powerup1_state[OBJECT_TILE] == 1){ //normal
        set_sprite_tile(2, 58);
    }
    if (powerup2_state[OBJECT_TILE] == 0){ //hide
        set_sprite_tile(3, 2);
    } else if (powerup2_state[OBJECT_TILE] == 1){ //normal
        set_sprite_tile(3, 62);
    }
    if (powerup3_state[OBJECT_TILE] == 0){ //hide
        set_sprite_tile(4, 2);
    } else if (powerup3_state[OBJECT_TILE] == 1){ //normal
        set_sprite_tile(4, 66);
    }

    move_sprite(2, powerup1_state[OBJECT_X] + 8, powerup1_state[OBJECT_Y] + 8);
    move_sprite(3, powerup2_state[OBJECT_X] + 8, powerup2_state[OBJECT_Y] + 8);
    move_sprite(4, powerup3_state[OBJECT_X] + 8, powerup3_state[OBJECT_Y] + 8);
}

void UpdateAll(){
    UpdatePlayer();
    UpdateBackground();
    UpdatePowerups();
    UpdateEffects();
    UpdateSound();
}

void checkPowerUpCollisions(){
    if(collisionStateCheck(player_state,powerup1_state) && player_powerup < 1 && player_is_on_floor){
        player_powerup = 1;
        // hide power up
        powerup1_state[OBJECT_TILE] = 0;
        // stop power up from being collideable
        powerup1_state[OBJECT_W] = 0;
        powerup1_state[OBJECT_H] = 0;
        PlayNote(C4_LR,C4_HR);
    } else if(collisionStateCheck(player_state,powerup2_state) && player_powerup < 2 && player_is_on_floor){
        player_powerup = 2;
        // hide power up
        powerup2_state[OBJECT_TILE] = 0;
        // stop power up from being collideable
        powerup2_state[OBJECT_W] = 0;
        powerup2_state[OBJECT_H] = 0;
        PlayNote(E4_LR,E4_HR);
    } else if(collisionStateCheck(player_state,powerup3_state) && player_powerup < 3 && player_is_on_floor){
        player_powerup = 3;
        // hide power up
        powerup3_state[OBJECT_TILE] = 0;
        // stop power up from being collideable
        powerup3_state[OBJECT_W] = 0;
        powerup3_state[OBJECT_H] = 0;
        PlayNote(G4_LR,G4_HR);
    }
}

void setupLevel(UINT8 level){
    
    if (level == 1){
        set_bkg_data(0, level1Map_tile_count, level1Map_tile_data ); //load background tile set
        set_bkg_tiles( 0, 0, 20, 18, level1Map_map_data); //load background map
        platform1_state[OBJECT_X] = 0;
        platform1_state[OBJECT_Y] = 112;
        platform1_state[OBJECT_W] = 160;
        platform1_state[OBJECT_H] = 24;

        platform2_state[OBJECT_X] = 0;
        platform2_state[OBJECT_Y] = 0;
        platform2_state[OBJECT_W] = 0;
        platform2_state[OBJECT_H] = 0;

        platform3_state[OBJECT_X] = 0;
        platform3_state[OBJECT_Y] = 0;
        platform3_state[OBJECT_W] = 0;
        platform3_state[OBJECT_H] = 0;

        platform4_state[OBJECT_X] = 0;
        platform4_state[OBJECT_Y] = 0;
        platform4_state[OBJECT_W] = 0;
        platform4_state[OBJECT_H] = 0;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;

        powerup1_state[OBJECT_X] = 48;
        powerup1_state[OBJECT_Y] = 104;
        powerup1_state[OBJECT_W] = 8;
        powerup1_state[OBJECT_H] = 8;
        powerup1_state[OBJECT_TILE] = 0;

        powerup2_state[OBJECT_X] = 18;
        powerup2_state[OBJECT_Y] = 104;
        powerup2_state[OBJECT_W] = 8;
        powerup2_state[OBJECT_H] = 8;
        powerup2_state[OBJECT_TILE] = 0;

        powerup3_state[OBJECT_X] = 128;
        powerup3_state[OBJECT_Y] = 104;
        powerup3_state[OBJECT_W] = 8;
        powerup3_state[OBJECT_H] = 8;
        powerup3_state[OBJECT_TILE] = 0;

    } else if (level == 2){
        set_bkg_data(0, 19, levelTiles ); //load background tile set
        set_bkg_tiles( 0, 0, 20, 18, level2Map); //load background map
        platform1_state[OBJECT_X] = 8;
        platform1_state[OBJECT_Y] = 112;
        platform1_state[OBJECT_W] = 16;
        platform1_state[OBJECT_H] = 16;

        platform2_state[OBJECT_X] = 40;
        platform2_state[OBJECT_Y] = 112;
        platform2_state[OBJECT_W] = 72;
        platform2_state[OBJECT_H] = 16;

        platform3_state[OBJECT_X] = 56;
        platform3_state[OBJECT_Y] = 104;
        platform3_state[OBJECT_W] = 48;
        platform3_state[OBJECT_H] = 8;

        platform4_state[OBJECT_X] = 136;
        platform4_state[OBJECT_Y] = 112;
        platform4_state[OBJECT_W] = 16;
        platform4_state[OBJECT_H] = 16;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;

        powerup1_state[OBJECT_X] = 160;
        powerup1_state[OBJECT_Y] = 144;
        powerup1_state[OBJECT_W] = 0;
        powerup1_state[OBJECT_H] = 0;
        powerup1_state[OBJECT_TILE] = 0;

        powerup2_state[OBJECT_X] = 160;
        powerup2_state[OBJECT_Y] = 144;
        powerup2_state[OBJECT_W] = 0;
        powerup2_state[OBJECT_H] = 0;
        powerup2_state[OBJECT_TILE] = 0;

        powerup3_state[OBJECT_X] = 160;
        powerup3_state[OBJECT_Y] = 144;
        powerup3_state[OBJECT_W] = 0;
        powerup3_state[OBJECT_H] = 0;
        powerup3_state[OBJECT_TILE] = 0;

    } else if (level == 3){
        set_bkg_data(0, 19, levelTiles ); //load background tile set
        set_bkg_tiles( 0, 0, 20, 18, level3Map); //load background map
        platform1_state[OBJECT_X] = 8;
        platform1_state[OBJECT_Y] = 112;
        platform1_state[OBJECT_W] = 16;
        platform1_state[OBJECT_H] = 16;

        platform2_state[OBJECT_X] = 40;
        platform2_state[OBJECT_Y] = 96;
        platform2_state[OBJECT_W] = 16;
        platform2_state[OBJECT_H] = 16;

        platform3_state[OBJECT_X] = 88;
        platform3_state[OBJECT_Y] = 96;
        platform3_state[OBJECT_W] = 16;
        platform3_state[OBJECT_H] = 16;

        platform4_state[OBJECT_X] = 136;
        platform4_state[OBJECT_Y] = 112;
        platform4_state[OBJECT_W] = 16;
        platform4_state[OBJECT_H] = 16;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;

        powerup1_state[OBJECT_X] = 160;
        powerup1_state[OBJECT_Y] = 144;
        powerup1_state[OBJECT_W] = 0;
        powerup1_state[OBJECT_H] = 0;
        powerup1_state[OBJECT_TILE] = 0;

        powerup2_state[OBJECT_X] = 160;
        powerup2_state[OBJECT_Y] = 144;
        powerup2_state[OBJECT_W] = 0;
        powerup2_state[OBJECT_H] = 0;
        powerup2_state[OBJECT_TILE] = 0;

        powerup3_state[OBJECT_X] = 160;
        powerup3_state[OBJECT_Y] = 144;
        powerup3_state[OBJECT_W] = 0;
        powerup3_state[OBJECT_H] = 0;
        powerup3_state[OBJECT_TILE] = 0;

    } else if (level == 4){
        set_bkg_data(0, 19, levelTiles ); //load background tile set
        set_bkg_tiles( 0, 0, 20, 18, level4Map); //load background map
        platform1_state[OBJECT_X] = 8;
        platform1_state[OBJECT_Y] = 112;
        platform1_state[OBJECT_W] = 16;
        platform1_state[OBJECT_H] = 16;

        platform2_state[OBJECT_X] = 40;
        platform2_state[OBJECT_Y] = 96;
        platform2_state[OBJECT_W] = 16;
        platform2_state[OBJECT_H] = 16;

        platform3_state[OBJECT_X] = 8;
        platform3_state[OBJECT_Y] = 64;
        platform3_state[OBJECT_W] = 16;
        platform3_state[OBJECT_H] = 16;

        platform4_state[OBJECT_X] = 136;
        platform4_state[OBJECT_Y] = 120;
        platform4_state[OBJECT_W] = 16;
        platform4_state[OBJECT_H] = 16;

        platform5_state[OBJECT_X] = 136;
        platform5_state[OBJECT_Y] = 32;
        platform5_state[OBJECT_W] = 16;
        platform5_state[OBJECT_H] = 16;

        powerup1_state[OBJECT_X] = 48;
        powerup1_state[OBJECT_Y] = 88;
        powerup1_state[OBJECT_W] = 8;
        powerup1_state[OBJECT_H] = 8;
        powerup1_state[OBJECT_TILE] = 0;

        powerup2_state[OBJECT_X] = 16;
        powerup2_state[OBJECT_Y] = 56;
        powerup2_state[OBJECT_W] = 8;
        powerup2_state[OBJECT_H] = 8;
        powerup2_state[OBJECT_TILE] = 0;

        powerup3_state[OBJECT_X] = 144;
        powerup3_state[OBJECT_Y] = 112;
        powerup3_state[OBJECT_W] = 8;
        powerup3_state[OBJECT_H] = 8;
        powerup3_state[OBJECT_TILE] = 0;

    } else if (level == 5){
        set_bkg_data(0, 19, levelTiles ); //load background tile set
        set_bkg_tiles( 0, 0, 20, 18, level5Map); //load background map
        platform1_state[OBJECT_X] = 136;
        platform1_state[OBJECT_Y] = 112;
        platform1_state[OBJECT_W] = 16;
        platform1_state[OBJECT_H] = 16;

        platform2_state[OBJECT_X] = 104;
        platform2_state[OBJECT_Y] = 112;
        platform2_state[OBJECT_W] = 16;
        platform2_state[OBJECT_H] = 32;

        platform3_state[OBJECT_X] = 88;
        platform3_state[OBJECT_Y] = 64;
        platform3_state[OBJECT_W] = 16;
        platform3_state[OBJECT_H] = 80;

        platform4_state[OBJECT_X] = 40;
        platform4_state[OBJECT_Y] = 0;
        platform4_state[OBJECT_W] = 16;
        platform4_state[OBJECT_H] = 88;

        platform5_state[OBJECT_X] = 8;
        platform5_state[OBJECT_Y] = 48;
        platform5_state[OBJECT_W] = 16;
        platform5_state[OBJECT_H] = 16;

        powerup1_state[OBJECT_X] = 160;
        powerup1_state[OBJECT_Y] = 144;
        powerup1_state[OBJECT_W] = 0;
        powerup1_state[OBJECT_H] = 0;
        powerup1_state[OBJECT_TILE] = 0;

        powerup2_state[OBJECT_X] = 112;
        powerup2_state[OBJECT_Y] = 104;
        powerup2_state[OBJECT_W] = 8;
        powerup2_state[OBJECT_H] = 8;
        powerup2_state[OBJECT_TILE] = 0;

        powerup3_state[OBJECT_X] = 96;
        powerup3_state[OBJECT_Y] = 56;
        powerup3_state[OBJECT_W] = 8;
        powerup3_state[OBJECT_H] = 8;
        powerup3_state[OBJECT_TILE] = 0;

    } else if (level == 6){
        set_bkg_data(0, 19, levelTiles ); //load background tile set
        set_bkg_tiles( 0, 0, 20, 18, level6Map); //load background map
        platform1_state[OBJECT_X] = 8;
        platform1_state[OBJECT_Y] = 120;
        platform1_state[OBJECT_W] = 24;
        platform1_state[OBJECT_H] = 16;

        platform2_state[OBJECT_X] = 32;
        platform2_state[OBJECT_Y] = 32;
        platform2_state[OBJECT_W] = 24;
        platform2_state[OBJECT_H] = 112;

        platform3_state[OBJECT_X] = 80;
        platform3_state[OBJECT_Y] = 0;
        platform3_state[OBJECT_W] = 24;
        platform3_state[OBJECT_H] = 112;

        platform4_state[OBJECT_X] = 128;
        platform4_state[OBJECT_Y] = 48;
        platform4_state[OBJECT_W] = 24;
        platform4_state[OBJECT_H] = 16;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;

        powerup1_state[OBJECT_X] = 160;
        powerup1_state[OBJECT_Y] = 144;
        powerup1_state[OBJECT_W] = 0;
        powerup1_state[OBJECT_H] = 0;
        powerup1_state[OBJECT_TILE] = 0;

        powerup2_state[OBJECT_X] = 160;
        powerup2_state[OBJECT_Y] = 144;
        powerup2_state[OBJECT_W] = 0;
        powerup2_state[OBJECT_H] = 0;
        powerup2_state[OBJECT_TILE] = 0;

        powerup3_state[OBJECT_X] = 24;
        powerup3_state[OBJECT_Y] = 112;
        powerup3_state[OBJECT_W] = 8;
        powerup3_state[OBJECT_H] = 8;
        powerup3_state[OBJECT_TILE] = 0;
    } else if (level == 7){
        set_bkg_data(0, 19, levelTiles ); //load background tile set
        set_bkg_tiles( 0, 0, 20, 18, levelEndMap); //load background map
        platform1_state[OBJECT_X] = 72;
        platform1_state[OBJECT_Y] = 96;
        platform1_state[OBJECT_W] = 16;
        platform1_state[OBJECT_H] = 16;

        platform2_state[OBJECT_X] = 0;
        platform2_state[OBJECT_Y] = 0;
        platform2_state[OBJECT_W] = 0;
        platform2_state[OBJECT_H] = 0;

        platform3_state[OBJECT_X] = 0;
        platform3_state[OBJECT_Y] = 0;
        platform3_state[OBJECT_W] = 0;
        platform3_state[OBJECT_H] = 0;

        platform4_state[OBJECT_X] = 0;
        platform4_state[OBJECT_Y] = 0;
        platform4_state[OBJECT_W] = 0;
        platform4_state[OBJECT_H] = 0;

        platform5_state[OBJECT_X] = 0;
        platform5_state[OBJECT_Y] = 0;
        platform5_state[OBJECT_W] = 0;
        platform5_state[OBJECT_H] = 0;

        powerup1_state[OBJECT_X] = 160;
        powerup1_state[OBJECT_Y] = 144;
        powerup1_state[OBJECT_W] = 0;
        powerup1_state[OBJECT_H] = 0;
        powerup1_state[OBJECT_TILE] = 0;

        powerup2_state[OBJECT_X] = 160;
        powerup2_state[OBJECT_Y] = 144;
        powerup2_state[OBJECT_W] = 0;
        powerup2_state[OBJECT_H] = 0;
        powerup2_state[OBJECT_TILE] = 0;

        powerup3_state[OBJECT_X] = 160;
        powerup3_state[OBJECT_Y] = 144;
        powerup3_state[OBJECT_W] = 0;
        powerup3_state[OBJECT_H] = 0;
        powerup3_state[OBJECT_TILE] = 0;
    }
}

void EffectDeathAnim(UINT8 frame_time){
    if (player_death_frame_count > frame_time){
        if (player_death_frame_index == 0){
            effect1_state[OBJECT_TILE] = 0; //set player type to attack frame 1
            background_type = 1;
            PlaySlide(C4_LR,C4_HR,1);
        } else if (player_death_frame_index == 1){
            effect1_state[OBJECT_TILE] = 1; //set player type to attack frame 1
            background_type = 3;
        } else if (player_death_frame_index == 2) {
            effect1_state[OBJECT_TILE] = 2; //set player type to attack frame 1
            background_type = 2;
        } else if (player_death_frame_index == 3) {
            effect1_state[OBJECT_TILE] = 3; //set player type to attack frame 2
            background_type = 3;
        } else if (player_death_frame_index == 4) {
            effect1_state[OBJECT_TILE] = 4; //set player type to attack frame 2
            background_type = 1;
        } else if (player_death_frame_index == 5) {
            effect1_state[OBJECT_TILE] = 5; //set player type to attack frame 3
            background_type = 3;
        } else if (player_death_frame_index == 6) {
            effect1_state[OBJECT_TILE] = 6; //set player type to attack frame 4
            background_type = 2;
        } else if (player_death_frame_index == 7) {
            effect1_state[OBJECT_TILE] = 7; //set player type to attack frame 5
            background_type = 0;
        }
        player_death_frame_index++;
        if (player_death_frame_index > 7){
            player_death_frame_index = 7;
            effect1_state[OBJECT_TILE] = 0;
            death_anim_complete = 1;
        }
        player_death_frame_count = 0;
    }
    player_death_frame_count++;
}

void PlayerAttackAnim(UINT8 frame_time){
    if (player_attack_frame_count > frame_time){
        if (player_attack_frame_index == 0){
            player_state[OBJECT_TILE] = 7; //set player type to attack frame 1
            background_type = 0;
        } else if (player_attack_frame_index == 1) {
            player_state[OBJECT_TILE] = 7; //set player type to attack frame 1
            background_type = 0;
        } else if (player_attack_frame_index == 2) {
            player_state[OBJECT_TILE] = 8; //set player type to attack frame 2
            background_type = 0;
        } else if (player_attack_frame_index == 3) {
            player_state[OBJECT_TILE] = 8; //set player type to attack frame 2
            background_type = 0;
        } else if (player_attack_frame_index == 4) {
            player_state[OBJECT_TILE] = 9; //set player type to attack frame 3
            background_type = 0;
        } else if (player_attack_frame_index == 5) {
            player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            background_type = 0;
        } else if (player_attack_frame_index == 6) {
            player_state[OBJECT_TILE] = 11; //set player type to attack frame 5
            background_type = 2;
            PlayNote(C3_LR,C3_HR);
        } else if (player_attack_frame_index == 7) {
            player_state[OBJECT_TILE] = 11; //set player type to attack frame 5
            background_type = 2;
        } else if (player_attack_frame_index == 8) {
            player_state[OBJECT_TILE] = 12; //set player type to attack frame 6
            background_type = 3;
            if (new_level){
                if (level_num == 1){
                    PlayNote(G4_LR,G4_HR);
                } else {
                    PlayNote(C4_LR,C4_HR);
                }
            }
            //play_shake2_sound = 1;
        } else if (player_attack_frame_index == 9) {
            player_state[OBJECT_TILE] = 12; //set player type to attack frame 6
            background_type = 3;
        } else if (player_attack_frame_index == 10) {
            player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            background_type = 1;
            if (new_level){
                if (level_num == 1){
                    PlayNote(G4_LR,G4_HR);
                } else {
                    PlayNote(G4_LR,G4_HR);
                }
            }
            //play_shake1_sound = 1;
        } else if (player_attack_frame_index == 11) {
            player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            background_type = 1;
        } else if (player_attack_frame_index == 12) {
            player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            background_type = 1;
            if (new_level){
                if (level_num == 1){
                    PlayNote(C5_LR,C5_HR);
                }
            }
        } else if (player_attack_frame_index == 13) {
            player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            background_type = 1;
        } else if (player_attack_frame_index == 14) {
            player_state[OBJECT_TILE] = 10; //set player type to attack frame 4
            background_type = 1;
        }
        player_attack_frame_index++;
        if (player_attack_frame_index > 14){
            player_attack_frame_index = 14;
            attack_anim_complete = 1;
        }
        player_attack_frame_count = 0;
    }
    player_attack_frame_count++;
}

void transition(UINT8 next_level){ 
    // reset powerups and hide 
    powerup1_state[OBJECT_TILE] = 0;
    powerup1_state[OBJECT_W] = 0;
    powerup1_state[OBJECT_H] = 0;
    powerup2_state[OBJECT_TILE] = 0;
    powerup2_state[OBJECT_W] = 0;
    powerup2_state[OBJECT_H] = 0;
    powerup3_state[OBJECT_TILE] = 0;
    powerup3_state[OBJECT_W] = 0;
    powerup3_state[OBJECT_H] = 0;

    background_type = 0;
    UpdateAll();
    set_bkg_palette(0, 1, bkgPalette_blank); //keep background hidden
    wait_vbl_done();
    setupLevel(next_level);
    UINT8 level_start_x = level_start_xs[next_level-1];
    UINT8 level_start_y = level_start_ys[next_level-1];
    UINT8 reached_x = 0;
    UINT8 reached_y = 0;
    while(reached_x == 0 || reached_y == 0){
        if (player_state[OBJECT_X] > level_start_x){
            player_state[OBJECT_X] = player_state[OBJECT_X] - 1;
        } else if (player_state[OBJECT_X] < level_start_x){
            player_state[OBJECT_X] = player_state[OBJECT_X] + 1;
        } else {
            reached_x = 1;
        }
        if (player_state[OBJECT_Y] > level_start_y){
            player_state[OBJECT_Y] = player_state[OBJECT_Y] - 1;
        } else if (player_state[OBJECT_Y] < level_start_y){
            player_state[OBJECT_Y] = player_state[OBJECT_Y] + 1;
        } else {
            reached_y = 1;
        }
        UpdateAll();
        set_bkg_palette(0, 1, bkgPalette_blank); //keep background hidden
        wait_vbl_done();
    }
}

void checkSceneCollisons(){
    resetFromCollision(prev_player_state, player_state, platform1_state);
    resetFromCollision(prev_player_state, player_state, platform2_state);
    resetFromCollision(prev_player_state, player_state, platform3_state);
    resetFromCollision(prev_player_state, player_state, platform4_state);
    resetFromCollision(prev_player_state, player_state, platform5_state);

    // make sure player doesn't go outside screen
    if (player_state[OBJECT_Y] <= 0 || player_state[OBJECT_Y] > 200){
        player_state[OBJECT_Y] = 0;
    }

    if (player_state[OBJECT_Y] > 140){
        player_state[OBJECT_Y] = 140;
        player_is_dead = 1;
    } else {
        player_is_dead = 0;
    }

    if (player_state[OBJECT_X] <= 2){
        player_state[OBJECT_X] = 2;
    }

    if (player_state[OBJECT_X] > 150){
        player_state[OBJECT_X] = 150;
    }
}

void checkLevelComplete(){
    if (level_num == 1){
        if(keyPressed(J_B)){ //power pressed
            new_level = 1;
        }
    } else if (level_num == 2){
        test_player_state[OBJECT_Y] = player_state[OBJECT_Y] + 1;
        if (collisionStateCheck(test_player_state,platform4_state)){ //on last platform
            new_level = 1;
        }
    } else if (level_num == 3){
        test_player_state[OBJECT_Y] = player_state[OBJECT_Y] + 1;
        if (collisionStateCheck(test_player_state,platform4_state)){ //on last platform
            new_level = 1;
        }
    } else if (level_num == 4){
        test_player_state[OBJECT_Y] = player_state[OBJECT_Y] + 1;
        if (collisionStateCheck(test_player_state,platform5_state)){ //on last platform
            new_level = 1;
        }
    }else if (level_num == 5){
        test_player_state[OBJECT_Y] = player_state[OBJECT_Y] + 1;
        if (collisionStateCheck(test_player_state,platform5_state)){ //on last platform
            new_level = 1;
        }
    }else if (level_num == 6){
        test_player_state[OBJECT_Y] = player_state[OBJECT_Y] + 1;
        if (collisionStateCheck(test_player_state,platform4_state)){ //on last platform
            new_level = 1;
        }
    }else if (level_num == 7){
        test_player_state[OBJECT_Y] = player_state[OBJECT_Y] + 1;
        if (collisionStateCheck(test_player_state,platform1_state)){ //on last platform
            new_level = 1;
        }
    }
}

void Death(){
    // reset to last checkpoint level
    level_num = checkpoint_level;
    // hide player
    player_state[OBJECT_TILE] = 0;
    effect1_state[OBJECT_X] = player_state[OBJECT_X];
    effect1_state[OBJECT_Y] = 127;

    while (1){
        wait_vbl_done();
        updateKeys();
        EffectDeathAnim(10);
        UpdateAll();
        if (death_anim_complete == 1){
            death_anim_complete = 0;
            player_death_frame_index = 0;
            break;
        }
    }

    // reset powerups and hide 
    powerup1_state[OBJECT_TILE] = 0;
    powerup1_state[OBJECT_W] = 0;
    powerup1_state[OBJECT_H] = 0;
    powerup2_state[OBJECT_TILE] = 0;
    powerup2_state[OBJECT_W] = 0;
    powerup2_state[OBJECT_H] = 0;
    powerup3_state[OBJECT_TILE] = 0;
    powerup3_state[OBJECT_W] = 0;
    powerup3_state[OBJECT_H] = 0;

    background_type = 0;
    UpdateAll();
    set_bkg_palette(0, 1, bkgPalette_blank); //keep background hidden
    wait_vbl_done();
    setupLevel(level_num);
    
    player_state[OBJECT_X] = level_start_xs[level_num-1];
    player_state[OBJECT_Y] = level_start_ys[level_num-1];

    player_powerup = 0;
}

void main() {
    player_state[OBJECT_X] = level_start_xs[0];
    player_state[OBJECT_Y] = level_start_ys[0];
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

    player_powerup = 0;

    background_type = 0;

    attack_anim_complete = 0;
    player_attack_frame_count = 0;
    player_attack_frame_index = 0;

    death_anim_complete = 0;
    player_death_frame_count = 0;
    player_death_frame_index = 0;

    level_num = 1;
    new_level = 0;

    player_jump_count = 0;
    player_walk_count = 0;
    player_is_jumping = 0;
    player_is_walking = 0;
    player_is_on_floor = 0;
    player_is_attacking = 0;
    player_walk_index = 0;
    player_is_dead = 0;

    checkpoint_level = 1;

    time_in_air = 0;
    time_boosting = 0;

    DISPLAY_ON;

    HIDE_WIN;

    // load backgrounds
    set_bkg_palette(0, 1, bkgPalette_blank);
    setupLevel(1);
	scroll_bkg(0, 0);
    SHOW_BKG;

    // initalise character sprite
    SPRITES_8x16;
    PlayerInit();
    PowerupsInit();
    EffectsInit();
    UpdateAll();
    SHOW_SPRITES; // display sprites

    initSound();

    // game loop
    while(1) {
        player_move_x = 0;
        player_move_y = 0;

        wait_vbl_done();

        updateKeys(); // check key presses

        if((joypad() != J_RIGHT) && (joypad() != J_LEFT)){
            player_is_walking = 0;
        }

        if(keyPressed(J_B)) {
            //attack
            player_attack_frame_count++;
            if (player_attack_frame_count < 50){
                player_is_attacking = 1;
            } else {
                player_is_attacking = 0;
                player_attack_frame_count = 50;
            }
        } else {
            player_is_attacking = 0;
            player_attack_frame_count = 0;
            player_attack_frame_index = 0;
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

            if (player_powerup == 0){ //normal jump
                if (time_boosting < 25){
                    if(keyPressed(J_A)) {
                        time_boosting++;
                        //jump
                        player_jump_count++;
                        if (player_jump_count < 25){ // restrict jump time
                            PlaySlide(D3_LR,D3_HR,0);
                            // accelleration arch to jump
                            if (player_jump_count < 3){
                                player_move_y-=1;
                            } else if (player_jump_count < 5){
                                player_move_y-=2;
                            } else if (player_jump_count < 10){
                                player_move_y-=3;
                            } else if (player_jump_count < 15){
                                player_move_y-=2;
                            } else if (player_jump_count < 20){
                                player_move_y-=1; //gravity is -1 so total movement is net 0
                            }
                            player_is_jumping = 1;
                        } else {
                            player_jump_count = 25;
                            player_is_jumping = 0;
                        }
                    } else {
                        player_is_jumping = 0;
                        player_jump_count = 0;
                    }
                }
            } else if (player_powerup == 1){ //small boost jump
                if (time_boosting < 20){
                    if(keyPressed(J_A)) {
                        time_boosting++;
                        //jump
                        player_jump_count++;
                        if (player_jump_count < 20){ // restrict jump time
                            player_move_y-=3;
                            player_is_jumping = 1;
                            PlayNote(C3_LR,C3_HR);
                        } else {
                            player_jump_count = 20;
                            player_is_jumping = 0;
                        }
                    } else {
                        player_is_jumping = 0;
                        player_jump_count = 0;
                    }
                }
            } else if (player_powerup == 2){ // medium boost jump
                if (time_boosting < 50){
                    if(keyPressed(J_A)) {
                        time_boosting++;
                        //jump
                        player_jump_count++;
                        if (player_jump_count < 30){ // restrict jump time
                            player_move_y-=3;
                            player_is_jumping = 1;
                            PlayNote(C3_LR,C3_HR);
                            //PlaySlide(C3_LR,C3_HR,1);
                        } else {
                            player_jump_count = 30;
                            player_is_jumping = 0;
                        }
                    } else {
                        player_is_jumping = 0;
                        player_jump_count = 0;
                    }
                }
            } else if (player_powerup == 3){ // large boost jump
                if (time_boosting < 90){
                    if(keyPressed(J_A)) {
                        time_boosting++;
                        //jump
                        player_jump_count++;
                        if (player_jump_count < 50){ // restrict jump time
                            player_move_y-=3;
                            player_is_jumping = 1;
                            PlayNote(C3_LR,C3_HR);
                        } else {
                            player_jump_count = 50;
                            player_is_jumping = 0;
                        }
                    } else {
                        player_is_jumping = 0;
                        player_jump_count = 0;
                    }
                }
            }
            

            if(keyPressed(J_DOWN)) {
                player_move_y+=1;
            }
        }

        //Add gravity
        player_move_y++;

        //move x
        player_state[OBJECT_X] = player_state[OBJECT_X] + player_move_x;

        //check collision
        checkSceneCollisons();

        prev_player_state[OBJECT_X] = player_state[OBJECT_X];
        prev_player_state[OBJECT_Y] = player_state[OBJECT_Y];

        // move y
        player_state[OBJECT_Y] = player_state[OBJECT_Y] + player_move_y;

        // check collision
        checkSceneCollisons();

        test_player_state[OBJECT_X] = player_state[OBJECT_X];
        test_player_state[OBJECT_Y] = player_state[OBJECT_Y] + 1;
        test_player_state[OBJECT_W] = player_state[OBJECT_W];
        test_player_state[OBJECT_H] = player_state[OBJECT_H];
        test_player_state[OBJECT_DIR] = player_state[OBJECT_DIR];
        test_player_state[OBJECT_TILE] = player_state[OBJECT_TILE];

        if (collisionStateCheck(test_player_state,platform1_state)){
            player_is_on_floor = 1;
        } else if (collisionStateCheck(test_player_state,platform2_state)){
            player_is_on_floor = 1;
        } else if (collisionStateCheck(test_player_state,platform3_state)){
            player_is_on_floor = 1;
        } else if (collisionStateCheck(test_player_state,platform4_state)){
            player_is_on_floor = 1;
        } else if (collisionStateCheck(test_player_state,platform5_state)){
            player_is_on_floor = 1;
        } else {
            player_is_on_floor = 0;
        }

        if (player_is_on_floor == 0){
            time_in_air++;
            background_type = 0;
            if (player_powerup == 0){
                if (player_is_jumping == 1){
                    player_state[OBJECT_TILE] = 13; //set player type to walk frame 3
                } else {
                    player_state[OBJECT_TILE] = 3; //set player type to walk frame 2
                }
            } else {
                if (player_is_jumping == 1){
                    player_state[OBJECT_TILE] = 5; //set player type to jump frame 1
                } else {
                    player_state[OBJECT_TILE] = 6; //set player type to jump frame 2 (fall)
                }
            }
        } else {
            time_in_air = 0;
            time_boosting = 0;
            if (player_is_attacking == 1){
                PlayerAttackAnim(10);
            }
            else if (player_is_walking == 1){
                if (player_walk_count > 10){
                    if (player_walk_index == 0){
                        player_state[OBJECT_TILE] = 4; //set player type to walk frame 2
                    } else if (player_walk_index == 1) {
                        player_state[OBJECT_TILE] = 1; //set player type to walk frame 3
                    } else if (player_walk_index == 2){
                        player_state[OBJECT_TILE] = 2; //set player type to normal frame
                    } else if (player_walk_index == 3){
                        player_state[OBJECT_TILE] = 3; //set player type to walk frame 1
                    }
                    player_walk_index++;
                    if (player_walk_index > 3){
                        player_walk_index = 0;
                    }
                    player_walk_count = 0;
                }
                player_walk_count++;
            } else {
                player_walk_count = 11;
                player_state[OBJECT_TILE] = 1;
                background_type = 0;
            }
        }

        UpdateAll();

        checkPowerUpCollisions();

        test_player_state[OBJECT_X] = player_state[OBJECT_X];
        test_player_state[OBJECT_Y] = player_state[OBJECT_Y];
        test_player_state[OBJECT_W] = player_state[OBJECT_W];
        test_player_state[OBJECT_H] = player_state[OBJECT_H];
        test_player_state[OBJECT_DIR] = player_state[OBJECT_DIR];
        test_player_state[OBJECT_TILE] = player_state[OBJECT_TILE];

        checkLevelComplete();

        if (new_level == 1){
            player_powerup = 0;
            level_num++;
            if (level_num > 7){
                level_num = 1;
            }
            player_attack_frame_index = 0;
            attack_anim_complete = 0;
            while (1){
                wait_vbl_done();
                updateKeys();
                PlayerAttackAnim(5);
                UpdateAll();
                if (attack_anim_complete == 1){
                    attack_anim_complete = 0;
                    break;
                }
            }
            if (level_num == 2){ // transition from title to level 2
                for (UINT8 i = 0; i < 120; i++){
                    //Hold title animation for longer
                    wait_vbl_done();
                    updateKeys();
                    PlayerAttackAnim(5);
                    UpdateAll();
                }
            } else if (level_num == 5){
                checkpoint_level = 5;
            } else if (level_num == 1){ // transition from end screen to title
                checkpoint_level = 1;
                for (UINT8 i = 0; i < 150; i++){
                    //Hold title animation for longer
                    //TODO add end title effects
                    wait_vbl_done();
                    updateKeys();
                    PlayerAttackAnim(5);
                    UpdateAll();
                }
            }
            player_attack_frame_index = 0;
            //set player type to attack frame 4
            player_state[OBJECT_TILE] = 10; 
            transition(level_num);
            player_attack_frame_index = 0;
            new_level = 0;
        }

        if (player_is_dead){
            Death();
        }

        prev_player_state[OBJECT_X] = player_state[OBJECT_X];
        prev_player_state[OBJECT_Y] = player_state[OBJECT_Y];
        prev_player_state[OBJECT_W] = player_state[OBJECT_W];
        prev_player_state[OBJECT_H] = player_state[OBJECT_H];
        prev_player_state[OBJECT_DIR] = player_state[OBJECT_DIR];
        prev_player_state[OBJECT_TILE] = player_state[OBJECT_TILE];
    }
}