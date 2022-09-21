/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "intro.h"

#include "../mem.h"

//Intro background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
} Back_Intro;

//Intro background functions
void Back_Intro_Free(StageBack *back)
{
	Back_Intro *this = (Back_Intro*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Intro_New(void)
{
	//Allocate background structure
	Back_Intro *this = (Back_Intro*)Mem_Alloc(sizeof(Back_Intro));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_overlay = NULL;
	this->back.draw_fg = NULL;
	this->back.draw_md = NULL;
	this->back.draw_bg = NULL;
	this->back.free = Back_Intro_Free;
	
	//Use non-pitch black background
	Gfx_SetClear(0, 17, 37);
	
	return (StageBack*)this;
}
