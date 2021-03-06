/*
** medal.h
** Contains medal structures and prototypes
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

#ifndef __MEDAL_H__
#define __MEDAL_H__

#include "doomdef.h"
#include "info.h"

//*****************************************************************************
//	DEFINES

#define	MEDAL_ICON_DURATION			( 3 * TICRATE )

// Same values as defined in AFloatyIcon::States[].
#define	S_TERMINATORARTIFACT			0
#define S_CHAT					( S_TERMINATORARTIFACT + 4 )
#define S_ENEMY					( S_CHAT + 1 )
#define	S_BLUEFLAG				( S_ENEMY + 1 )
#define	S_REDFLAG				( S_BLUEFLAG + 6 )
#define	S_WHITEFLAG				( S_REDFLAG + 6 )
#define	S_BLUESKULL				( S_WHITEFLAG + 6 )
#define	S_REDSKULL				( S_BLUESKULL + 2 )
#define	S_EXCELLENT				( S_REDSKULL + 2 )
#define	S_INCREDIBLE			( S_EXCELLENT + 1 )
#define	S_IMPRESSIVE			( S_INCREDIBLE + 1 )
#define	S_MOST_IMPRESSIVE		( S_IMPRESSIVE + 1 )
#define	S_DOMINATION			( S_MOST_IMPRESSIVE + 1 )
#define	S_TOTAL_DOMINATION		( S_DOMINATION + 1 )
#define	S_ACCURACY				( S_TOTAL_DOMINATION + 1 )
#define	S_PRECISION				( S_ACCURACY + 1 )
#define	S_VICTORY				( S_PRECISION + 1 )
#define	S_PERFECT				( S_VICTORY + 1 )
#define	S_FIRSTFRAG				( S_PERFECT + 1 )
#define	S_TERMINATION			( S_FIRSTFRAG + 1 )
#define	S_CAPTURE				( S_TERMINATION + 1 )
#define	S_TAG					( S_CAPTURE + 1 )
#define	S_ASSIST				( S_TAG + 1 )
#define	S_DEFENSE				( S_ASSIST + 1 )
#define	S_LLAMA					( S_DEFENSE + 1 )
#define	S_YOUFAILIT				( S_LLAMA + 1 )
#define	S_YOURSKILLISNOTENOUGH	( S_YOUFAILIT + 1 )
#define	S_LAG					( S_YOURSKILLISNOTENOUGH + 1 )
#define	S_FISTING				( S_LAG + 1 )
#define	S_SPAM					( S_FISTING + 1 )
#define	S_POSSESSIONARTIFACT		( S_SPAM + 1 )

//*****************************************************************************
enum
{
	MEDAL_EXCELLENT,
	MEDAL_INCREDIBLE,
	MEDAL_IMPRESSIVE,
	MEDAL_MOSTIMPRESSIVE,
	MEDAL_DOMINATION,
	MEDAL_TOTALDOMINATION,
	MEDAL_ACCURACY,
	MEDAL_PRECISION,
	MEDAL_VICTORY,
	MEDAL_PERFECT,
	MEDAL_TERMINATION,
	MEDAL_FIRSTFRAG,
	MEDAL_CAPTURE,
	MEDAL_TAG,
	MEDAL_ASSIST,
	MEDAL_DEFENSE,
	MEDAL_LLAMA,
	MEDAL_SPAM,
	MEDAL_YOUFAILIT,
	MEDAL_YOURSKILLISNOTENOUGH,
	MEDAL_FISTING,

	NUM_MEDALS
};

//*****************************************************************************
#define	MEDALQUEUE_DEPTH			NUM_MEDALS

//*****************************************************************************
//	STRUCTURES

typedef struct
{
	// Icon that displays on the screen when this medal is received.
	char		szLumpName[8];

	// Frame the floaty icon above the player's head is set to.
	USHORT		usFrame;

	// Text that appears below the medal icon when received.
	char		*szStr;

	// Color that text is displayed in.
	ULONG		ulTextColor;

	// Announcer entry that's played when this medal is triggered.
	char		szAnnouncerEntry[32];

	// Name of sound to play when this medal type is triggered.
	char		szSoundName[64];

} MEDAL_t;

//*****************************************************************************
typedef struct
{
	// Type of medal in this queue entry.
	ULONG		ulMedal;

	// Amount of time before the medal display in this queue expires.
	ULONG		ulTick;

} MEDALQUEUE_t;

//*****************************************************************************
//	PROTOTYPES

// Standard API.
void	MEDAL_Construct( void );
void	MEDAL_Input( void );
void	MEDAL_Tick( void );
void	MEDAL_Render( void );

void	MEDAL_GiveMedal( ULONG ulPlayer, ULONG ulMedal );
void	MEDAL_RenderAllMedals( LONG lYOffset );
void	MEDAL_RenderAllMedalsFullscreen( player_s *pPlayer );
ULONG	MEDAL_GetDisplayedMedal( ULONG ulPlayer );
void	MEDAL_ClearMedalQueue( ULONG ulPlayer );

//*****************************************************************************
//	EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Bool, cl_medals )
EXTERN_CVAR( Bool, cl_icons )

#endif