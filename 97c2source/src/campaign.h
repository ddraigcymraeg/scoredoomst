/*
** campaign.h
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
//-----------------------------------------------------------------------------
//
// Skulltag Source
// Copyright (C) 2003-2005 Brad Carney
// Date created:  1/15/05
//
//
// Filename: 
//
// Description: 
//
//-----------------------------------------------------------------------------

#ifndef	__CAMPAIGN_H__
#define	__CAMPAIGN_H__

#include "doomdef.h"
#include "doomtype.h"

//*****************************************************************************
//	STRUCTURES

typedef struct
{
	// The name of this bot that's going to spawn in.
	char	szBotName[32];

	// The team this bot's on when he spawns in.
	char	szBotTeam[8];

} BOTSPAWNINFO_t;

//*****************************************************************************
typedef struct CAMPAIGNINFO_s
{
	// The fraglimit of this map in the campaign.
	LONG			lFragLimit;

	// The timelimit of this map in the campaign.
	float			fTimeLimit;

	// The pointlimit of this map in the campaign.
	LONG			lPointLimit;

	// The duellimit of this map in the campaign.
	LONG			lDuelLimit;

	// The winlimit of this map in the campaign.
	LONG			lWinLimit;

	// The wavelimit of this map in the campaign.
	LONG			lWaveLimit;

	// The game mode that's being played in this map.
	LONG			lGameMode;

	// The dmflags that are to be used on this map.
	LONG			lDMFlags;

	// The dmflags2 that are to be used on this map.
	LONG			lDMFlags2;

	// The compatflags that are to be used on this map.
	LONG			lCompatFlags;

	// The map for which this campaign data applies.
	char			szMapName[8];

	// If this is a teamgame, which team should the player be on?
	char			szPlayerTeam[8];

	// If this is a duel, does the player have to win every single duel to beat the level?
	bool			bMustWinAllDuels;

	// How lost must the possession artifact be held?
	LONG			lPossessionHoldTime;

	// Table of bots and their teams that spawn in this round.
	BOTSPAWNINFO_t	BotSpawn[MAXPLAYERS];

	struct CAMPAIGNINFO_s	*pNextInfo;

} CAMPAIGNINFO_t;

//*****************************************************************************
//	FUNCTIONS

void				CAMPAIGN_Construct( void );
void				CAMPAIGN_Destruct( void );
void				CAMPAIGN_ParseCampaignInfo( void );
CAMPAIGNINFO_t		*CAMPAIGN_GetCampaignInfo( char *pszMapName );
void				CAMPAIGN_EnableCampaign( void );
void				CAMPAIGN_DisableCampaign( void );
bool				CAMPAIGN_AllowCampaign( void );
bool				CAMPAIGN_InCampaign( void );
void				CAMPAIGN_SetInCampaign( bool bInCampaign );
bool				CAMPAIGN_DidPlayerBeatMap( void );

#endif	// __CAMPAIGN_H__
