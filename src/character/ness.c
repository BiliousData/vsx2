/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ness.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Ness character structure
enum
{
	Ness_ArcMain_Idle,
	Ness_ArcMain_Hit,
	Ness_ArcMain_Right,
	Ness_ArcMain_Swing0,
	Ness_ArcMain_Swing1,
	Ness_ArcMain_Ouch0,
	Ness_ArcMain_Ouch1,
	
	Ness_Arc_Max,
};

boolean noidle;

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Ness_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Ness;

//Ness character definitions
static const CharFrame char_ness_frame[] = {
	{Ness_ArcMain_Idle,  {  0,   0,  83, 110}, {  0,   0}}, //0
	{Ness_ArcMain_Idle,  { 83,   0,  82, 111}, {  0,   1}}, //1
	{Ness_ArcMain_Idle,  {165,   0,  81, 112}, {  0,   2}}, //2
	{Ness_ArcMain_Idle,  {  0, 112,  81, 112}, {  0,   2}}, //3
	{Ness_ArcMain_Idle,  { 81, 112,  81, 112}, {  0,   2}}, //4

	{Ness_ArcMain_Hit, 	 {  0,   0,  81,  99}, { 12, -10}}, //5
	{Ness_ArcMain_Hit, 	 { 81,   0,  78,  99}, { 10, -10}}, //6

	{Ness_ArcMain_Hit, 	 {159,   0,  88,  84}, { 13, -25}}, //7
	{Ness_ArcMain_Hit, 	 {  0,  99,  86,  85}, { 13, -24}}, //8

	{Ness_ArcMain_Hit, 	 { 86,  99,  78, 114}, { -3,   4}}, //9
	{Ness_ArcMain_Hit, 	 {164,  99,  79, 113}, { -2,   3}}, //10

	{Ness_ArcMain_Right, {  0,   0,  98, 105}, {  5,  -5}}, //11
	{Ness_ArcMain_Right, { 98,   0,  96, 105}, {  5,  -5}}, //12

	{Ness_ArcMain_Swing0,  {  0,   0,  66, 111}, { -4,   1}}, //13
	{Ness_ArcMain_Swing0,  { 66,   0,  65, 109}, { -5,  -1}}, //14
	{Ness_ArcMain_Swing0,  {131,   0,  67, 109}, { -5,  -1}}, //15
	{Ness_ArcMain_Swing0,  {  0, 111, 144, 109}, { 16,  -1}}, //16
	{Ness_ArcMain_Swing1,  {  0,   0, 137, 111}, { 17,   1}}, //17
	{Ness_ArcMain_Swing1,  {137,   0, 118,  98}, { 50, -12}}, //18
	{Ness_ArcMain_Swing1,  {  0, 111, 117,  98}, { 49, -12}}, //19
	{Ness_ArcMain_Swing1,  {117, 111, 116,  98}, { 48, -12}}, //20

	{Ness_ArcMain_Ouch0,   {  0,   0, 113, 101}, { 45,  -9}}, //21
	{Ness_ArcMain_Ouch0,   {113,   0, 116,  99}, { 48, -11}}, //22
	{Ness_ArcMain_Ouch0,   {  0, 101, 115, 100}, { 47, -10}}, //23
	{Ness_ArcMain_Ouch0,   {115, 101, 117, 100}, { 49, -10}}, //24
	{Ness_ArcMain_Ouch1,   {  0,   0, 110, 102}, { 42,  -8}}, //25
	{Ness_ArcMain_Ouch1,   {110,   0, 110, 102}, { 43,  -8}}, //26
};

static const Animation char_ness_anim[CharAnim_Max] = {
	{2, (const u8[]){0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){5, 6, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){7, 8, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){9, 10, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){11, 12, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
	{2, (const u8[]){13, 14, 15, 16, 17, 18, 19, 20, ASCR_BACK, 1}}, //Swing Bat
	{2, (const u8[]){21, 22, 23, 23, 23, 23, 24, 25, 26, ASCR_BACK, 1}}, //get stoned
};

//Ness character functions
void Char_Ness_SetFrame(void *user, u8 frame)
{
	Char_Ness *this = (Char_Ness*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_ness_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Ness_Tick(Character *character)
{
	Char_Ness *this = (Char_Ness*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Ness_SetFrame);
	Character_Draw(character, &this->tex, &char_ness_frame[this->frame]);

	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		switch (stage.song_step)
		{
			case 653:
			{
				character->set_anim(character, CharAnim_Extra1); //SMAAAASH!!
				noidle = true;
				break;
			}
			case 662:
			{
				character->set_anim(character, CharAnim_Extra2); //you little shit
				break;
			}
		}
	}
}

void Char_Ness_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Ness_Free(Character *character)
{
	Char_Ness *this = (Char_Ness*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Ness_New(fixed_t x, fixed_t y)
{
	//Allocate ness object
	Char_Ness *this = Mem_Alloc(sizeof(Char_Ness));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Ness_New] Failed to allocate ness object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Ness_Tick;
	this->character.set_anim = Char_Ness_SetAnim;
	this->character.free = Char_Ness_Free;
	
	Animatable_Init(&this->character.animatable, char_ness_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(37,1);
	this->character.focus_y = FIXED_DEC(36,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\NESS.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle.tim",
		"hit.tim",
		"right.tim",
		"swing0.tim",
		"swing1.tim",
		"ouch0.tim",
		"ouch1.tim",
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

	noidle = false;
	
	return (Character*)this;
}
