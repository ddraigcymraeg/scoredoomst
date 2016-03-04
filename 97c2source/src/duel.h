/*
** duel.h
** Contains duel structures and prototypes
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

#ifndef __DUEL_H__
#define __DUEL_H__

#include "doomtype.h"

//*****************************************************************************
//	DEFINES

typedef enum
{
	DS_WAITINGFORPLAYERS,
	DS_COUNTDOWN,
	DS_INDUEL,
	DS_WINSEQUENCE,

} DUELSTATE_e;

//*****************************************************************************
//	PROTOTYPES

void	DUEL_Construct( void );
void	DUEL_Tick( void );

ULONG	DUEL_CountActiveDuelers( void );
void	DUEL_StartCountdown( ULONG ulTicks );
void	DUEL_DoFight( void );
void	DUEL_DoWinSequence( ULONG ulPlayer );
void	DUEL_SendLoserToSpectators( void );
bool	DUEL_IsDueler( ULONG ulPlayer );

// Access functions
ULONG	DUEL_GetCountdownTicks( void );
void	DUEL_SetCountdownTicks( ULONG ulTicks );

void	DUEL_SetLoser( ULONG ulPlayer );
ULONG	DUEL_GetLoser( void );

DUELSTATE_e	DUEL_GetState( void );
void		DUEL_SetState( DUELSTATE_e State );

ULONG	DUEL_GetNumDuels( void );
void	DUEL_SetNumDuels( ULONG ulNumDuels );

bool	DUEL_GetStartNextDuelOnLevelLoad( void );
void	DUEL_SetStartNextDuelOnLevelLoad( bool bStart );

//*****************************************************************************
//  EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Int, sv_duelcountdowntime )
EXTERN_CVAR( Int, duellimit )

#endif	// __DUEL_H__