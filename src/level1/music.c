#define D3_LR 0x00
#define D3_HR 0xC3

#define C3_LR 0x00
#define C3_HR 0xC0

#define C3_SHARP_LR 0x50
#define C3_SHARP_HR 0xC0

#define C4_SHARP_LR 0x10
#define C4_SHARP_HR 0xCF

#define C4_LR 0x05
#define C4_HR 0xCF

#define D4_LR 0x20
#define D4_HR 0xCF

#define E4_LR 0x38
#define E4_HR 0xCF

#define G4_LR 0x55
#define G4_HR 0xCF

#define C5_LR 0x80
#define C5_HR 0xCF

UINT8 is_sound_playing = 0;

UINT8 slide_sound_frame_count = 0;
UINT8 play_slide_sound = 0;
UINT8 run_slide_timer = 0;

UINT8 note_to_play_low_reg = 0x10;
UINT8 note_to_play_high_reg = 0xCF;
UINT8 note_sound_frame_count = 0;
UINT8 play_note_sound = 0;
UINT8 run_note_timer = 0;
UINT8 note_slide_dir = 0;

void initSound(){
    // these registers must be in this specific order!
    // is 1000 0000 in binary and turns on sound
    NR52_REG = 0x80; 
    // sets the volume for both left and right channel just set to max 0x77
    NR50_REG = 0x77;
    // is 1111 1111 in binary, select which chanels we want to use in this case all of them. 
    // One bit for the L one bit for the R of all four channels
    NR51_REG = 0xFF; 
}

void PlaySlide(UINT8 low_register, UINT8 high_register, UINT8 dir){
    note_to_play_low_reg = low_register;
    note_to_play_high_reg = high_register;
    play_slide_sound = 1;
    note_slide_dir = dir; //0: down, 1: up
}

void PlayNote(UINT8 low_register, UINT8 high_register){
    note_to_play_low_reg = low_register;
    note_to_play_high_reg = high_register;
    play_note_sound = 1;
}

void UpdateSlideSound(){
    if (play_slide_sound == 1){
        if (is_sound_playing == 0){
            play_slide_sound = 0;
            is_sound_playing = 1;
            run_slide_timer = 1;
            if (note_slide_dir == 0){ //slide up
                NR10_REG = 0x16;
            } else { //slide down
                NR10_REG = 0x1E;
            }
            NR11_REG = 0x40;
            NR12_REG = 0x73;
            NR13_REG = note_to_play_low_reg;
            NR14_REG = note_to_play_high_reg;
        }
    }

    if (run_slide_timer == 1){
        slide_sound_frame_count++;
        if (slide_sound_frame_count >= 25){
            play_slide_sound = 0;
            run_slide_timer = 0;
            is_sound_playing = 0;
            slide_sound_frame_count = 0;
        }
    }
}

void UpdateNoteSound(){
    if (play_note_sound == 1){
        if (is_sound_playing == 0){
            play_note_sound = 0;
            is_sound_playing = 1;
            run_note_timer = 1;
            NR10_REG = 0x00;
            NR11_REG = 0x18;
            NR12_REG = 0x73;
            NR13_REG = note_to_play_low_reg;
            NR14_REG = note_to_play_high_reg;
        }
    }

    if (run_note_timer == 1){
        note_sound_frame_count++;
        if (note_sound_frame_count > 2){
            play_note_sound = 0;
            run_note_timer = 0;
            is_sound_playing = 0;
            note_sound_frame_count = 0;
        }
    }
}

void UpdateSound(){
    UpdateSlideSound();
    UpdateNoteSound();
}