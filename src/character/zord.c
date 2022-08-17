/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "zord.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Zord character structure
enum
{
	Zord_ArcMain_Idle0,
	Zord_ArcMain_Idle1,
	Zord_ArcMain_Idle2,
	Zord_ArcMain_Idle3,
	Zord_ArcMain_Idle4,
	Zord_ArcMain_Idle5,
	Zord_ArcMain_Idle6,

	Zord_ArcMain_Left0,
	Zord_ArcMain_Left1,
	Zord_ArcMain_Left2,
	Zord_ArcMain_Left3,

	Zord_ArcMain_Lefta0,
	Zord_ArcMain_Lefta1,
	Zord_ArcMain_Lefta2,
	Zord_ArcMain_Lefta3,

	Zord_ArcMain_Down0,
	Zord_ArcMain_Down1,
	Zord_ArcMain_Down2,
	Zord_ArcMain_Down3,

	Zord_ArcMain_Downa0,
	Zord_ArcMain_Downa1,
	Zord_ArcMain_Downa2,
	Zord_ArcMain_Downa3,

	Zord_ArcMain_Up0,
	Zord_ArcMain_Up1,
	Zord_ArcMain_Up2,
	Zord_ArcMain_Up3,

	Zord_ArcMain_Upalt0,
	Zord_ArcMain_Upalt1,
	Zord_ArcMain_Upalt2,
	Zord_ArcMain_Upalt3,

	Zord_ArcMain_Right0,
	Zord_ArcMain_Right1,
	Zord_ArcMain_Right2,
	Zord_ArcMain_Right3,

	Zord_ArcMain_Righta0,
	Zord_ArcMain_Righta1,
	Zord_ArcMain_Righta2,
	Zord_ArcMain_Righta3,
	
	
	Zord_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Zord_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Zord;

//Zord character definitions
static const CharFrame char_zord_frame[] = {
	{ Zord_ArcMain_Idle0, {0, 0, 160, 180}, {0, 0}}, //0
	{ Zord_ArcMain_Idle1, {0, 0, 160, 180}, {0, 0}}, //1
	{ Zord_ArcMain_Idle2, {0, 0, 160, 180}, {0, 0}}, //2
	{ Zord_ArcMain_Idle3, {0, 0, 160, 180}, {0, 0}}, //3
	{ Zord_ArcMain_Idle4, {0, 0, 160, 180}, {0, 0}}, //4
	{ Zord_ArcMain_Idle5, {0, 0, 160, 180}, {0, 0}}, //5
	{ Zord_ArcMain_Idle6, {0, 0, 160, 180}, {0, 0}}, //6

	{ Zord_ArcMain_Left0, {0, 0, 160, 180}, {0, 0}}, //7
	{ Zord_ArcMain_Left1, {0, 0, 160, 180}, {0, 0}}, //8
	{ Zord_ArcMain_Left2, {0, 0, 160, 180}, {0, 0}}, //9
	{ Zord_ArcMain_Left3, {0, 0, 160, 180}, {0, 0}}, //10

	{Zord_ArcMain_Lefta0, {0, 0, 160, 180}, {0, 0}}, //11
	{Zord_ArcMain_Lefta1, {0, 0, 160, 180}, {0, 0}}, //12
	{Zord_ArcMain_Lefta2, {0, 0, 160, 180}, {0, 0}}, //13
	{Zord_ArcMain_Lefta3, {0, 0, 160, 180}, {0, 0}}, //14

	{ Zord_ArcMain_Down0, {0, 0, 160, 180}, {0, 0}}, //15
	{ Zord_ArcMain_Down1, {0, 0, 160, 180}, {0, 0}}, //16
	{ Zord_ArcMain_Down2, {0, 0, 160, 180}, {0, 0}}, //17
	{ Zord_ArcMain_Down3, {0, 0, 160, 180}, {0, 0}}, //18

	{Zord_ArcMain_Downa0, {0, 0, 160, 180}, {0, 0}}, //19
	{Zord_ArcMain_Downa1, {0, 0, 160, 180}, {0, 0}}, //20
	{Zord_ArcMain_Downa2, {0, 0, 160, 180}, {0, 0}}, //21
	{Zord_ArcMain_Downa3, {0, 0, 160, 180}, {0, 0}}, //22

	{   Zord_ArcMain_Up0, {0, 0, 160, 180}, {0, 0}}, //23
	{   Zord_ArcMain_Up1, {0, 0, 160, 180}, {0, 0}}, //24
	{   Zord_ArcMain_Up2, {0, 0, 160, 180}, {0, 0}}, //25
	{   Zord_ArcMain_Up3, {0, 0, 160, 180}, {0, 0}}, //26

	{Zord_ArcMain_Upalt0, {0, 0, 160, 180}, {0, 0}}, //27
	{Zord_ArcMain_Upalt1, {0, 0, 160, 180}, {0, 0}}, //28
	{Zord_ArcMain_Upalt2, {0, 0, 160, 180}, {0, 0}}, //29
	{Zord_ArcMain_Upalt3, {0, 0, 160, 180}, {0, 0}}, //30

	{Zord_ArcMain_Right0, {0, 0, 160, 180}, {0, 0}}, //31
	{Zord_ArcMain_Right1, {0, 0, 160, 180}, {0, 0}}, //32
	{Zord_ArcMain_Right2, {0, 0, 160, 180}, {0, 0}}, //33
	{Zord_ArcMain_Right3, {0, 0, 160, 180}, {0, 0}}, //34

	{Zord_ArcMain_Righta0, {0, 0, 160, 180}, {0, 0}}, //35
	{Zord_ArcMain_Righta1, {0, 0, 160, 180}, {0, 0}}, //36
	{Zord_ArcMain_Righta2, {0, 0, 160, 180}, {0, 0}}, //37
	{Zord_ArcMain_Righta3, {0, 0, 160, 180}, {0, 0}}, //38
};

static const Animation char_zord_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 7, 8, 9, 10, ASCR_BACK, 1}},         //CharAnim_Left
	{2, (const u8[]){ 11, 12, 13, 14, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{2, (const u8[]){15, 16, 17, 18, ASCR_BACK, 1}},         //CharAnim_Down
	{2, (const u8[]){19, 20, 21, 22, ASCR_BACK, 1}},   //CharAnim_DownAlt
	{2, (const u8[]){23, 24, 25, 26, ASCR_BACK, 1}},         //CharAnim_Up
	{2, (const u8[]){27, 28, 29, 30, ASCR_BACK, 1}},   //CharAnim_UpAlt
	{2, (const u8[]){31, 32, 33, 34, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){35, 36, 37, 38, ASCR_BACK, 1}},   //CharAnim_RightAlt
};

//Zord character functions
void Char_Zord_SetFrame(void *user, u8 frame)
{
	Char_Zord *this = (Char_Zord*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_zord_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Zord_Tick(Character *character)
{
	Char_Zord *this = (Char_Zord*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Zord_SetFrame);
	Character_DrawScale(character, &this->tex, &char_zord_frame[this->frame], 4, 2);
}

void Char_Zord_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Zord_Free(Character *character)
{
	Char_Zord *this = (Char_Zord*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_ConsoleZord_New(fixed_t x, fixed_t y)
{
	//Allocate zord object
	Char_Zord *this = Mem_Alloc(sizeof(Char_Zord));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_ConsoleZord_New]\nCONSOLES CAN'T COMBINE YOU IDIOT");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Zord_Tick;
	this->character.set_anim = Char_Zord_SetAnim;
	this->character.free = Char_Zord_Free;
	
	Animatable_Init(&this->character.animatable, char_zord_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(311,1);
	this->character.focus_y = FIXED_DEC(174,1);
	this->character.focus_zoom = 674;
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\ZORD.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"idle2.tim",
		"idle3.tim",
		"idle4.tim",
		"idle5.tim",
		"idle6.tim",
		"left0.tim",
		"left1.tim",
		"left2.tim",
		"left3.tim",
		"lefta0.tim",
		"lefta1.tim",
		"lefta2.tim",
		"lefta3.tim",
		"down0.tim",
		"down1.tim",
		"down2.tim",
		"down3.tim",
		"downa0.tim",
		"downa1.tim",
		"downa2.tim",
		"downa3.tim",
		"up0.tim",
		"up1.tim",
		"up2.tim",
		"up3.tim",
		"upalt0.tim",
		"upalt1.tim",
		"upalt2.tim",
		"upalt3.tim",
		"right0.tim",
		"right1.tim",
		"right2.tim",
		"right3.tim",
		"righta0.tim",
		"righta1.tim",
		"righta2.tim",
		"righta3.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	printf("holy SHIT that is a lot of sprites\n");
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;

	this->character.hr = 175;
	this->character.hg = 102;
	this->character.hb = 206;
	
	return (Character*)this;
}
