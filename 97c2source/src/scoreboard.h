/*
** scoreboard.h
** Contains scoreboard structures and prototypes
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

#ifndef __SCOREBOARD_H__
#define __SCOREBOARD_H__

#include "d_player.h"

//*****************************************************************************
//	DEFINES

#define	COLUMN1_XPOS	24
#define	COLUMN2_XPOS	72
#define	COLUMN3_XPOS	192
#define	COLUMN4_XPOS	256

//*****************************************************************************
enum
{
	ST_FRAGCOUNT,
	ST_POINTCOUNT,
	ST_KILLCOUNT,
	ST_WINCOUNT,
	ST_KILLCOUNTSD, //GHK
	ST_ITEMCOUNT,
	ST_SECRETCOUNT,
	ST_BARRELCOUNT,
	ST_TOTALCOUNT,

	NUM_SORT_TYPES
};

//*****************************************************************************
//	PROTOTYPES

void	SCOREBOARD_Render( player_s *pPlayer );
void	SCOREBOARD_RenderBoard( player_s *pPlayer );
void	SCOREBOARD_RenderDMStats( void );
void	SCOREBOARD_RenderTeamStats( player_s *pPlayer );
void	SCOREBOARD_RenderInvasionStats( void );
void	SCOREBOARD_RenderInVote( void );
void	SCOREBOARD_RenderDuelCountdown( ULONG ulTimeLeft );
void	SCOREBOARD_RenderLMSCountdown( ULONG ulTimeLeft );
void	SCOREBOARD_RenderPossessionCountdown( char *pszTitleString, ULONG ulTimeLeft );
void	SCOREBOARD_RenderSurvivalCountdown( ULONG ulTimeLeft );
void	SCOREBOARD_RenderInvasionFirstCountdown( ULONG ulTimeLeft );
void	SCOREBOARD_RenderInvasionCountdown( ULONG ulTimeLeft );
LONG	SCOREBOARD_CalcSpread( ULONG ulPlayerNum );
ULONG	SCOREBOARD_CalcRank( ULONG ulPlayerNum );
bool	SCOREBOARD_IsTied( ULONG ulPlayerNum );
void	SCOREBOARD_DisplayFragMessage( player_s *pFraggedPlayer );
void	SCOREBOARD_DisplayFraggedMessage( player_s *pFraggingPlayer );
void	SCOREBOARD_RefreshHUD( void );
ULONG	SCOREBOARD_GetNumPlayers( void );
ULONG	SCOREBOARD_GetRank( void );
LONG	SCOREBOARD_GetSpread( void );
bool	SCOREBOARD_IsTied( void );
ULONG	SCOREBOARD_CalcRankSD ( ULONG ulPlayerNum , ULONG type ); //ghk
bool	SCOREBOARD_IsTiedSD( ULONG ulPlayerNum , ULONG type);

//*****************************************************************************
//	EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Bool, cl_alwaysdrawdmstats )
EXTERN_CVAR( Bool, cl_alwaysdrawteamstats )

#endif // __SCOREBOARD_H__