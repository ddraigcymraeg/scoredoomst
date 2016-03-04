/*
** survival.h
**
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

#ifndef __SURVIVAL_H__
#define __SURVIVAL_H__

#include "c_cvars.h"

//*****************************************************************************
//	DEFINES

typedef enum
{
	SURVS_WAITINGFORPLAYERS,
	SURVS_COUNTDOWN,
	SURVS_INPROGRESS,
	SURVS_MISSIONFAILED,
	SURVS_NEWMAP,

} SURVIVALSTATE_e;

//*****************************************************************************
//	PROTOTYPES

void	SURVIVAL_Construct( void );
void	SURVIVAL_Tick( void );

ULONG	SURVIVAL_CountActivePlayers( bool bLiving );
void	SURVIVAL_StartCountdown( ULONG ulTicks );
void	SURVIVAL_DoFight( void );
//void	SURVIVAL_DoMissionFailed( void );

// Access functions
ULONG	SURVIVAL_GetCountdownTicks( void );
void	SURVIVAL_SetCountdownTicks( ULONG ulTicks );

SURVIVALSTATE_e	SURVIVAL_GetState( void );
void			SURVIVAL_SetState( SURVIVALSTATE_e State );

//*****************************************************************************
//  EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Int, sv_survivalcountdowntime )

#endif	// __SURVIVAL_H__
