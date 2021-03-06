/*
** lastmanstanding.h
** Contains LMS structures and prototypes
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

#ifndef __LASTMANSTANDING_H__
#define __LASTMANSTANDING_H__

#include "doomtype.h"

//*****************************************************************************
//	DEFINES

typedef enum
{
	LMSS_WAITINGFORPLAYERS,
	LMSS_COUNTDOWN,
	LMSS_INPROGRESS,
	LMSS_WINSEQUENCE,

} LMSSTATE_e;

#define	LMS_AWF_PISTOL				0x00000001
#define	LMS_AWF_SHOTGUN				0x00000002
#define	LMS_AWF_SSG					0x00000004
#define	LMS_AWF_CHAINGUN			0x00000008
#define	LMS_AWF_MINIGUN				0x00000010
#define	LMS_AWF_ROCKETLAUNCHER		0x00000020
#define	LMS_AWF_GRENADELAUNCHER		0x00000040
#define	LMS_AWF_PLASMA				0x00000080
#define	LMS_AWF_RAILGUN				0x00000100
#define	LMS_AWF_CHAINSAW			0x00000200

#define	LMS_AWF_ALLALLOWED			( LMS_AWF_PISTOL|LMS_AWF_SHOTGUN|LMS_AWF_SSG|LMS_AWF_CHAINGUN| \
									  LMS_AWF_MINIGUN|LMS_AWF_ROCKETLAUNCHER|LMS_AWF_GRENADELAUNCHER|LMS_AWF_PLASMA| \
									  LMS_AWF_RAILGUN|LMS_AWF_CHAINSAW )

#define	LMS_SPF_CHAT				0x00000001
#define	LMS_SPF_VIEW				0x00000002

//*****************************************************************************
//	PROTOTYPES

void	LASTMANSTANDING_Construct( void );
void	LASTMANSTANDING_Tick( void );

ULONG	LASTMANSTANDING_CountActivePlayers( void );
ULONG	LASTMANSTANDING_TeamCountActivePlayers( ULONG ulTeam );
LONG	LASTMANSTANDING_CountMenStanding( void );
LONG	LASTMANSTANDING_TeamCountMenStanding( ULONG ulTeam );
LONG	LASTMANSTANDING_GetLastManStanding( void );
LONG	LASTMANSTANDING_TeamGetLastManStanding( void );
void	LASTMANSTANDING_StartCountdown( ULONG ulTicks );
void	LASTMANSTANDING_DoFight( void );
void	LASTMANSTANDING_DoWinSequence( ULONG ulWinner );

// Access functions.
ULONG	LASTMANSTANDING_GetCountdownTicks( void );
void	LASTMANSTANDING_SetCountdownTicks( ULONG ulTicks );

LMSSTATE_e	LASTMANSTANDING_GetState( void );
void		LASTMANSTANDING_SetState( LMSSTATE_e State );

ULONG	LASTMANSTANDING_GetNumMatches( void );
void	LASTMANSTANDING_SetNumMatches( ULONG ulNumMatches );

bool	LASTMANSTANDING_GetStartNextMatchOnLevelLoad( void );
void	LASTMANSTANDING_SetStartNextMatchOnLevelLoad( bool bStart );

//*****************************************************************************
//	EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Int, sv_lmscountdowntime )
EXTERN_CVAR( Int, lmsallowedweapons );
EXTERN_CVAR( Int, lmsspectatorsettings );
EXTERN_CVAR( Int, winlimit )

#endif	// __LASTMANSTANDING_H__