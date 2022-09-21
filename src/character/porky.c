/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "porky.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Porky character structure
enum
{
	Porky_ArcMain_Idle0,
	Porky_ArcMain_Idle1,
	Porky_ArcMain_Left,
	Porky_ArcMain_Down,
	Porky_ArcMain_Up,
	Porky_ArcMain_Right,
	Porky_ArcMain_Hm,
	Porky_ArcMain_Laf,
	Porky_ArcMain_LeftA,
	Porky_ArcMain_DownA,
	Porky_ArcMain_UpA,
	Porky_ArcMain_RightA,
	
	Porky_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Porky_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Porky;

//Porky character definitions
static const CharFrame char_porky_frame[] = {
	{Porky_ArcMain_Idle0, {  0,   0,  79, 120}, {  0,   0}}, //0
	{Porky_ArcMain_Idle0, { 80,   0,  80, 121}, {  1,   1}}, //1
	{Porky_ArcMain_Idle0, {161,   0,  80, 121}, {  1,   1}}, //2
	{Porky_ArcMain_Idle1, {  0,   0,  80, 121}, {  1,   1}}, //3
	{Porky_ArcMain_Idle1, { 81,   0,  80, 121}, {  1,   1}}, //4

	{Porky_ArcMain_Left,  {  0,   0,  91, 117}, {  5,  -2}}, //5
	{Porky_ArcMain_Left,  { 92,   0,  90, 117}, {  4,  -2}}, //6

	{Porky_ArcMain_Down,  {  0,   0,  86, 114}, {  4,  -5}}, //7
	{Porky_ArcMain_Down,  { 87,   0,  85, 115}, {  4,  -4}}, //8

	{Porky_ArcMain_Up,    {  0,   0,  78, 127}, {  2,   8}}, //9
	{Porky_ArcMain_Up,    { 79,   0,  79, 126}, {  2,   7}}, //10

	{Porky_ArcMain_Right, {  0,   0,  83, 122}, { -3,   2}}, //11
	{Porky_ArcMain_Right, { 84,   0,  84, 122}, { -2,   2}}, //12

	{Porky_ArcMain_Hm,    {  0,   0,  86, 122}, {  0,   2}}, //13
	{Porky_ArcMain_Hm,    { 86,   0,  85, 122}, {  0,   2}}, //14
	{Porky_ArcMain_Hm,    {171,   0,  85, 122}, {  0,   2}}, //15

	{Porky_ArcMain_Laf,   {  0,   0,  83, 116}, {  4,  -3}}, //16
	{Porky_ArcMain_Laf,   { 83,   0,  83, 116}, {  3,  -3}}, //17
	{Porky_ArcMain_Laf,   {166,   0,  82, 116}, {  2,  -3}}, //18

	{Porky_ArcMain_LeftA,  {  0,   0,  91, 117}, {  5,  -2}}, //19
	{Porky_ArcMain_LeftA,  { 91,   0,  91, 117}, {  4,  -2}}, //20

	{Porky_ArcMain_DownA,  {  0,   0,  82, 114}, {  1,  -5}}, //21
	{Porky_ArcMain_DownA,  { 82,   0,  83, 115}, {  2,  -4}}, //22

	{Porky_ArcMain_UpA,    {  0,   0,  73, 127}, {  1,   8}}, //23
	{Porky_ArcMain_UpA,    { 73,   0,  73, 126}, {  1,   7}}, //24

	{Porky_ArcMain_RightA, {  0,   0,  81, 122}, { -1,   2}}, //25
	{Porky_ArcMain_RightA, { 81,   0,  81, 122}, { -1,   2}}, //26
};

static const Animation char_porky_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{2, (const u8[]){19, 20, ASCR_BACK, 1}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 7, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{2, (const u8[]){21, 22, ASCR_BACK, 1}},   //CharAnim_DownAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{2, (const u8[]){23, 24, ASCR_BACK, 1}},   //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{2, (const u8[]){25, 26, ASCR_BACK, 1}},   //CharAnim_RightAlt
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},
	{2, (const u8[]){16, 17, 18, ASCR_BACK, 1}},
};

//Porky character functions
void Char_Porky_SetFrame(void *user, u8 frame)
{
	Char_Porky *this = (Char_Porky*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_porky_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Porky_Tick(Character *character)
{
	Char_Porky *this = (Char_Porky*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Porky_SetFrame);
	Character_Draw(character, &this->tex, &char_porky_frame[this->frame]);

	//animation stuff
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step)
		{
			case 383:
				character->set_anim(character, CharAnim_Extra1); //Ok desu ka?
				break;
			case 612:
				character->set_anim(character, CharAnim_Extra2);
				break;
			case 614:
				character->set_anim(character, CharAnim_Extra2);
				break;
			case 677:
				character->forcealt = true;
				break;
			case 740:
				character->set_anim(character, CharAnim_Extra2);
				break;
			case 742:
				character->set_anim(character, CharAnim_Extra2);
				break;
		}
	}
}

void Char_Porky_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Porky_Free(Character *character)
{
	Char_Porky *this = (Char_Porky*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Porky_New(fixed_t x, fixed_t y)
{
	//Allocate porky object
	Char_Porky *this = Mem_Alloc(sizeof(Char_Porky));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Porky_New] Failed to allocate porky object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Porky_Tick;
	this->character.set_anim = Char_Porky_SetAnim;
	this->character.free = Char_Porky_Free;
	
	Animatable_Init(&this->character.animatable, char_porky_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(61,1);
	this->character.focus_y = FIXED_DEC(42,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\PORKY.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Porky_ArcMain_Idle0
		"idle1.tim", //Porky_ArcMain_Idle1
		"left.tim",  //Porky_ArcMain_Left
		"down.tim",  //Porky_ArcMain_Down
		"up.tim",    //Porky_ArcMain_Up
		"right.tim", //Porky_ArcMain_Right
		"hm.tim",
		"laf.tim",
		"lefta.tim",
		"downa.tim",
		"upa.tim",
		"righta.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;

	this->character.hr = 175;
	this->character.hg = 102;
	this->character.hb = 206;

	this->character.forcealt = false;
	
	return (Character*)this;
}
