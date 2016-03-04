/*
** deathmatch.h
** Contains deathmatch structures and prototypes
**
**---------------------------------------------------------------------------
** Copyright 2000-2010 Brad Carney
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/

#ifndef __DEATHMATCH_H__
#define __DEATHMATCH_H__

#include "c_cvars.h"

//*****************************************************************************
//	DEFINES

enum
{
	RAILCOLOR_BLUE,
	RAILCOLOR_RED,
	RAILCOLOR_YELLOW,
	RAILCOLOR_BLACK,
	RAILCOLOR_SILVER,
	RAILCOLOR_GOLD,
	RAILCOLOR_GREEN,
	RAILCOLOR_WHITE,
	RAILCOLOR_PURPLE,
	RAILCOLOR_ORANGE,
	RAILCOLOR_RAINBOW
};

//*****************************************************************************
enum
{
	GAMETYPE_COOPERATIVE,
	GAMETYPE_SURVIVAL,
	GAMETYPE_INVASION,
	GAMETYPE_DEATHMATCH,
	GAMETYPE_TEAMPLAY,
	GAMETYPE_DUEL,
	GAMETYPE_TERMINATOR,
	GAMETYPE_LASTMANSTANDING,
	GAMETYPE_TEAMLMS,
	GAMETYPE_POSSESSION,
	GAMETYPE_TEAMPOSSESSION,
	GAMETYPE_TEAMGAME,
	GAMETYPE_CTF,
	GAMETYPE_ONEFLAGCTF,
	GAMETYPE_SKULLTAG,
	GAMETYPE_TEAMCOOP,

	NUM_GAMETYPES
};

//*****************************************************************************
//  EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Bool, deathmatch )
EXTERN_CVAR( Bool, teamplay )
EXTERN_CVAR( Bool, duel )
EXTERN_CVAR( Bool, terminator )
EXTERN_CVAR( Bool, lastmanstanding )
EXTERN_CVAR( Bool, teamlms )
EXTERN_CVAR( Bool, possession )
EXTERN_CVAR( Bool, teampossession )

EXTERN_CVAR( Int, fraglimit )
EXTERN_CVAR( Float, timelimit )
EXTERN_CVAR( Bool, instagib )
EXTERN_CVAR( Bool, buckshot )

EXTERN_CVAR( Bool, cl_noammoswitch )
EXTERN_CVAR( Bool, cl_useoriginalweaponorder )
EXTERN_CVAR( Bool, cl_showlargefragmessages )

EXTERN_CVAR( Bool, sv_cheats )

#endif	// __DEATHMATCH_H__
