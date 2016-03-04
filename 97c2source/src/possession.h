/*
** possession.h
** Contains possession structures and prototypes
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

#ifndef __POSSESSION_H__
#define __POSSESSION_H__

#include "d_player.h"
#include "doomtype.h"

//*****************************************************************************
//	DEFINES

typedef enum
{
	PSNS_WAITINGFORPLAYERS,
	PSNS_COUNTDOWN,
	PSNS_INPROGRESS,
	PSNS_ARTIFACTHELD,
	PSNS_PRENEXTROUNDCOUNTDOWN,
	PSNS_NEXTROUNDCOUNTDOWN,
	PSNS_HOLDERSCORED,

} PSNSTATE_e;

//*****************************************************************************
//	PROTOTYPES

void	POSSESSION_Construct( void );
void	POSSESSION_Tick( void );
void	POSSESSION_Render( void );

void	POSSESSION_StartCountdown( ULONG ulTicks );
void	POSSESSION_StartNextRoundCountdown( ULONG ulTicks );
void	POSSESSION_DoFight( void );
void	POSSESSION_ScorePossessionPoint( player_s *pPlayer );
void	POSSESSION_ArtifactPickedUp( player_s *pPlayer, ULONG ulTicks );
void	POSSESSION_ArtifactDropped( void );
bool	POSSESSION_ShouldRespawnArtifact( void );

// Access functions.
ULONG	POSSESSION_GetCountdownTicks( void );
void	POSSESSION_SetCountdownTicks( ULONG ulTicks );

PSNSTATE_e	POSSESSION_GetState( void );
void		POSSESSION_SetState( PSNSTATE_e State );

//*****************************************************************************
//	EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Int, sv_possessioncountdowntime )
EXTERN_CVAR( Int, sv_possessionholdtime )
EXTERN_CVAR( Bool, sv_usemapsettingspossessionholdtime )

#endif	// __POSSESSION_H__
