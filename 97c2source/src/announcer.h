/*
** announcer.h
** Contains announcer structures and prototypes
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

#ifndef __ANNOUNCER_H__
#define __ANNOUNCER_H__

#include "c_cvars.h"
#include "doomtype.h"

//*****************************************************************************
//  DEFINES

// Maximum amount of announcer profiles that can be loaded.
#define	MAX_ANNOUNCERPROFILES			32

// Maximum number of entries that can be defined within each announcer profile.
#define	MAX_ANNOUNCERPROFILE_ENTRIES	128

//*****************************************************************************
//  STRUCTURES

typedef struct
{
	// The text name of this entry.
	char	szName[32];

	// The sound name for this entry.
	char	szSound[64];

} ANNOUNCERENTRY_t;

//*****************************************************************************
typedef struct
{
	// The name of this announcer profile.
	char				szName[64];

	// The list of announcer entries present in this profile.
	ANNOUNCERENTRY_t	**paAnnouncerEntries;

} ANNOUNCERPROFILE_t;

//*****************************************************************************
//  PROTOTYPES

void	ANNOUNCER_Construct( void );
void	ANNOUNCER_Destruct( void );

void	ANNOUNCER_ParseAnnouncerInfo( void );
ULONG	ANNOUNCER_GetNumProfiles( void );

bool	ANNOUNCER_DoesEntryExist( ULONG ulProfileIdx, const char *pszEntry );
void	ANNOUNCER_PlayEntry( ULONG ulProfileIdx, const char *pszEntry );
void	ANNOUNCER_PlayFragSounds( ULONG ulPlayer, LONG lOldFragCount, LONG lNewFragCount );
//void	ANNOUNCER_PlayScoreSounds( ULONG ulPlayer, LONG lOldPointCount, LONG lNewPointCount );
void	ANNOUNCER_PlayTeamFragSounds( ULONG ulTeam, LONG lOldFragCount, LONG lNewFragCount );
void	ANNOUNCER_AllowNumFragsAndPointsLeftSounds( void );

// These all return true if they generated a text message

bool AnnounceGameStart ();
bool AnnounceKill (AActor *killer, AActor *killee);
bool AnnounceTelefrag (AActor *killer, AActor *killee);
bool AnnounceSpree (AActor *who);
bool AnnounceSpreeLoss (AActor *who);
bool AnnounceMultikill (AActor *who);
bool AnnounceKillSD1 (ULONG ulPlayer, ULONG ulState); //GHK
bool AnnounceTelefragSD1 (ULONG ulPlayer); //GHK
bool AnnounceSpree (AActor *who);
bool AnnounceSpreeLoss (AActor *who);
bool AnnounceMultikill (AActor *who);
bool AnnounceBigKill (ULONG ulPlayer, bool bOnlyGood=false); //GHK
bool AnnounceMultikillSD (ULONG ulPlayer); //GHK
bool AnnounceSDGameStart (const char *levelname); //GHK
void DoVoiceAnnounceIntermission (bool win=false);//GHK
void DoSDSound (ULONG ulPlayer, ULONG ulState); //GHK

static int LastAnnounceTime; //ghk

// Access functions.
char	*ANNOUNCER_GetName( ULONG ulIdx );

//*****************************************************************************
//  EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Int, cl_announcer )

#endif	// __ANNOUNCER_H__
