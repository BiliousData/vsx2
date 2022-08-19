/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "x.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//X character structure
enum
{
	//X_ArcMain_PlaceHolder,
	X_ArcMain_Idle0,
	X_ArcMain_Idle1,
	X_ArcMain_Idle2,
	X_ArcMain_Idle3,
	X_ArcMain_Idle4,

	X_ArcMain_Left0,
	X_ArcMain_Left1,
	X_ArcMain_Left2,

	X_ArcMain_Down0,
	X_ArcMain_Down1,
	X_ArcMain_Down2,

	X_ArcMain_Up0,
	X_ArcMain_Up1,
	X_ArcMain_Up2,

	X_ArcMain_Right0,
	X_ArcMain_Right1,
	X_ArcMain_Right2,
	
	X_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[X_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_X;
u8 bikebump;

//X character definitions
static const CharFrame char_x_frame[] = {
	//{X_ArcMain_PlaceHolder, {  0,   0,  51,  41}, {  0,   0}}, //0 idle 1
	{X_ArcMain_Idle0, {0, 0, 224, 200}, {0,0}}, //0
	{X_ArcMain_Idle1, {0, 0, 224, 200}, {0,0}}, //1
	{X_ArcMain_Idle2, {0, 0, 224, 200}, {0,0}}, //2
	{X_ArcMain_Idle3, {0, 0, 224, 200}, {0,0}}, //3
	{X_ArcMain_Idle4, {0, 0, 224, 200}, {0,0}}, //4

	{X_ArcMain_Left0, {0, 0, 224, 200}, {0,0}}, //5
	{X_ArcMain_Left1, {0, 0, 224, 200}, {0,0}}, //6
	{X_ArcMain_Left2, {0, 0, 224, 200}, {0,0}}, //7

	{X_ArcMain_Down0, {0, 0, 224, 200}, {0,0}}, //8
	{X_ArcMain_Down1, {0, 0, 224, 200}, {0,0}}, //9
	{X_ArcMain_Down2, {0, 0, 224, 200}, {0,0}}, //10

	{X_ArcMain_Up0, {0, 0, 224, 200}, {0,0}}, //11
	{X_ArcMain_Up1, {0, 0, 224, 200}, {0,0}}, //12
	{X_ArcMain_Up2, {0, 0, 224, 200}, {0,0}}, //13

	{X_ArcMain_Right0, {0, 0, 224, 200}, {0,0}}, //14
	{X_ArcMain_Right1, {0, 0, 224, 200}, {0,0}}, //15
	{X_ArcMain_Right2, {0, 0, 224, 200}, {0,0}}, //16
};

static const Animation char_x_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 7, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 8, 9, 10, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){11, 12, 13, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){14, 15, 16, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//X character functions
void Char_X_SetFrame(void *user, u8 frame)
{
	Char_X *this = (Char_X*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_x_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_X_Tick(Character *character)
{
	Char_X *this = (Char_X*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_X_SetFrame);
	Character_Draw(character, &this->tex, &char_x_frame[this->frame]);
}

void Char_X_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_X_Free(Character *character)
{
	Char_X *this = (Char_X*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_X_New(fixed_t x, fixed_t y)
{
	//Allocate x object
	Char_X *this = Mem_Alloc(sizeof(Char_X));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_X_New] Failed to allocate x object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_X_Tick;
	this->character.set_anim = Char_X_SetAnim;
	this->character.free = Char_X_Free;
	
	Animatable_Init(&this->character.animatable, char_x_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(149,1);
	this->character.focus_y = FIXED_DEC(85,1);
	this->character.focus_zoom = 1534;
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\X.ARC;1");
	
	const char **pathp = (const char *[]){
		//"pholder.tim", //X_ArcMain_Idle0
		"idle0.tim",
		"idle1.tim",
		"idle2.tim",
		"idle3.tim",
		"idle4.tim",
		"left0.tim",
		"left1.tim",
		"left2.tim",
		"down0.tim",
		"down1.tim",
		"down2.tim",
		"up0.tim",
		"up1.tim",
		"up2.tim",
		"right0.tim",
		"right1.tim",
		"right2.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;

	this->character.hr = 0;
	this->character.hg = 153;
	this->character.hb = 204;
	
	return (Character*)this;
}
