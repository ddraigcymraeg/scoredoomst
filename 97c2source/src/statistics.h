/*
** statistics.h
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

#ifndef __STATISTICS_H__
#define __STATISTICS_H__

//*****************************************************************************
//	DEFINES

//*****************************************************************************
//	STRUCTURES

//*****************************************************************************
//	PROTOTYPES

void	STATISTICS_Construct( void );

void	STATISTICS_GetNode( char *pszName, ULONG ulPlayer );
void	INVASION_StartFirstCountdown( ULONG ulTicks );
void	INVASION_StartCountdown( ULONG ulTicks );
void	INVASION_BeginWave( ULONG ulWave );
void	INVASION_DoWaveComplete( void );
void	INVASION_WriteSaveInfo( FILE *pFile );
void	INVASION_ReadSaveInfo( PNGHandle *pPng );

ULONG	INVASION_GetCountdownTicks( void );
void	INVASION_SetCountdownTicks( ULONG ulTicks );	

INVASIONSTATE_e		INVASION_GetState( void );
void				INVASION_SetState( INVASIONSTATE_e State );

ULONG	INVASION_GetNumMonstersLeft( void );
void	INVASION_SetNumMonstersLeft( ULONG ulLeft );

ULONG	INVASION_GetNumArchVilesLeft( void );
void	INVASION_SetNumArchVilesLeft( ULONG ulLeft );

ULONG	INVASION_GetCurrentWave( void );

//*****************************************************************************
//  EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Int, sv_invasioncountdowntime )
EXTERN_CVAR( Int, wavelimit )

#endif	// __STATISTICS_H__
