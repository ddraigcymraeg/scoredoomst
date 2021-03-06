/*
** announcer.cpp
** Contains announcer functions
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

#include "announcer.h"
#include "c_cvars.h"
#include "c_dispatch.h"
#include "cmdlib.h"
#include "d_player.h"
#include "deathmatch.h"
#include "doomtype.h"
#include "i_system.h"
#include "s_sound.h"
#include "sc_man.h"
#include "team.h"
#include "w_wad.h"
#include "c_console.h" //ghk
#include "announcer.h"  //ghk bbannouncer this is for, for LastAnnounceTime

//*****************************************************************************
//	VARIABLES

static	ANNOUNCERPROFILE_t	*g_AnnouncerProfile[MAX_ANNOUNCERPROFILES];

// Have the "Three frags left!", etc. sounds been played yet?
static	bool			g_bThreeFragsLeftSoundPlayed;
static	bool			g_bTwoFragsLeftSoundPlayed;
static	bool			g_bOneFragLeftSoundPlayed;

// Have the "Three points left!", etc. sounds been played yet?
static	bool			g_bThreePointsLeftSoundPlayed;
static	bool			g_bTwoPointsLeftSoundPlayed;
static	bool			g_bOnePointLeftSoundPlayed;


//*****************************************************************************
//	PROTOTYPES

bool				announcer_AddAnnouncerProfile( ANNOUNCERPROFILE_t *pInfo );
void				announcer_AddProfileEntry( ANNOUNCERPROFILE_t *pInfo, const char *pszEntry, const char *pszSound );
ANNOUNCERENTRY_t	*announcer_FindEntry( ANNOUNCERPROFILE_t *pInfo, const char *pszEntry );
void				announcer_FinishAddingEntries( ANNOUNCERPROFILE_t *pInfo );

//*****************************************************************************
//	FUNCTIONS

void ANNOUNCER_Construct( void )
{
	ULONG	ulIdx;

	// Initialize all announcer info pointers.
	for ( ulIdx = 0; ulIdx < MAX_ANNOUNCERPROFILES; ulIdx++ )
		g_AnnouncerProfile[ulIdx] = NULL;

	// Call ANNOUNCER_Destruct() when Skulltag closes.
	atterm( ANNOUNCER_Destruct );
}

//*****************************************************************************
//
void ANNOUNCER_Destruct( void )
{
	ULONG	ulIdx;
	ULONG	ulIdx2;

	// First, go through and free all additional announcer info's.
	for ( ulIdx = 0; ulIdx < MAX_ANNOUNCERPROFILES; ulIdx++ )
	{
		if ( g_AnnouncerProfile[ulIdx] != NULL )
		{
			if ( g_AnnouncerProfile[ulIdx]->paAnnouncerEntries != NULL )
			{
				for ( ulIdx2 = 0; ulIdx2 < MAX_ANNOUNCERPROFILE_ENTRIES; ulIdx2++ )
				{
					if ( g_AnnouncerProfile[ulIdx]->paAnnouncerEntries[ulIdx2] )
					{
						free( g_AnnouncerProfile[ulIdx]->paAnnouncerEntries[ulIdx2] );
						g_AnnouncerProfile[ulIdx]->paAnnouncerEntries[ulIdx2] = NULL;
					}
				}

				free( g_AnnouncerProfile[ulIdx]->paAnnouncerEntries );
				g_AnnouncerProfile[ulIdx]->paAnnouncerEntries = NULL;
			}

			free( g_AnnouncerProfile[ulIdx] );
			g_AnnouncerProfile[ulIdx] = NULL;
		}
	}
}

//*****************************************************************************
//
void ANNOUNCER_ParseAnnouncerInfo( void )
{
	LONG		lCurLump;
	LONG		lLastLump = 0;
	char		szKey[128];
	char		szValue[128];
	ULONG		ulIdx;

	// Search through all loaded wads for a lump called "ANCRINFO".
	while (( lCurLump = Wads.FindLump( "ANCRINFO", (int *)&lLastLump )) != -1 )
	{
		// Make pszBotInfo point to the raw data (which should be a text file) in the ANCRINFO lump.
		SC_OpenLumpNum( lCurLump, "ANCRINFO" );

		// Begin parsing that text. COM_Parse will create a token (com_token), and
		// pszBotInfo will skip past the token.
		while ( SC_GetString( ))
		{
			ANNOUNCERPROFILE_t	AnnouncerProfile;

			// Initialize our announcer info variable.
			sprintf( AnnouncerProfile.szName,					"UNNAMED ANNOUNCER" );
			AnnouncerProfile.paAnnouncerEntries = (ANNOUNCERENTRY_t **)malloc( sizeof( ANNOUNCERENTRY_t ** ) * MAX_ANNOUNCERPROFILE_ENTRIES );
			for ( ulIdx = 0; ulIdx < MAX_ANNOUNCERPROFILE_ENTRIES; ulIdx++ )
			{
				AnnouncerProfile.paAnnouncerEntries[ulIdx] = (ANNOUNCERENTRY_t *)malloc( sizeof( ANNOUNCERENTRY_t ));

				AnnouncerProfile.paAnnouncerEntries[ulIdx]->szName[0] = '\0';
				AnnouncerProfile.paAnnouncerEntries[ulIdx]->szSound[0] = '\0';
			}

			while ( sc_String[0] != '{' )
				SC_GetString( );

			// We've encountered a starting bracket. Now continue to parse until we hit an end bracket.
			while ( sc_String[0] != '}' )
			{
				// The current token should be our key. (key = value) If it's an end bracket, break.
				SC_GetString( );
				sprintf( szKey, sc_String );
				if ( sc_String[0] == '}' )
					break;

				// The following key must be an = sign. If not, the user made an error!
				SC_GetString( );
				if ( stricmp( sc_String, "=" ) != 0 )
						I_Error( "ANNOUNCER_ParseAnnouncerInfo: Missing \"=\" in ANCRINFO lump for field \"%s\".\n", szKey );

				// The last token should be our value.
				SC_GetString( );
				sprintf( szValue, sc_String );

				// If we're specifying the name of the profile, set it here.
				if ( stricmp( szKey, "name" ) == 0 )
					sprintf( AnnouncerProfile.szName, szValue );
				// Add the new key, along with its value to the profile.
				else
					announcer_AddProfileEntry( &AnnouncerProfile, (const char *)szKey, (const char *)szValue );
			}

			// Now that we're done adding entries, sort them alphabetically.
			announcer_FinishAddingEntries( &AnnouncerProfile );

			// Finally, add our completed announcer profile.
			announcer_AddAnnouncerProfile( &AnnouncerProfile );
		}
	}
}

//*****************************************************************************
//
ULONG ANNOUNCER_GetNumProfiles( void )
{
	ULONG	ulIdx;
	ULONG	ulNumProfiles = 0;

	for ( ulIdx = 0; ulIdx < MAX_ANNOUNCERPROFILES; ulIdx++ )
	{
		if ( g_AnnouncerProfile[ulIdx] != NULL )
			ulNumProfiles++;
		else
			return ( ulNumProfiles );
	}

	return ( ulNumProfiles );
}

//*****************************************************************************
//
bool ANNOUNCER_DoesEntryExist( ULONG ulProfileIdx, const char *pszEntry )
{
	// Return false if the profile index is invalid, or a profile doesn't exist on this index.
	if (( ulProfileIdx >= MAX_ANNOUNCERPROFILES ) || ( g_AnnouncerProfile[ulProfileIdx] == NULL ))
		return ( false );

	// If the profile doesn't have any entries defined, return false.
	if ( g_AnnouncerProfile[ulProfileIdx]->paAnnouncerEntries == NULL )
		return ( false );

	// If the entry exists in the profile, return true.
	return ( announcer_FindEntry( g_AnnouncerProfile[ulProfileIdx], pszEntry ) != NULL );
}

//*****************************************************************************
//
void ANNOUNCER_PlayEntry( ULONG ulProfileIdx, const char *pszEntry )
{
	ANNOUNCERENTRY_t	*pEntry;

	// Return false if the profile index is invalid, or a profile doesn't exist on this index.
	if (( ulProfileIdx >= MAX_ANNOUNCERPROFILES ) || ( g_AnnouncerProfile[ulProfileIdx] == NULL ))
		return;

	// If the profile doesn't have any entries defined, return false.
	if ( g_AnnouncerProfile[ulProfileIdx]->paAnnouncerEntries == NULL )
		return;

	// If the entry exists and has a sound, play it.
	pEntry = announcer_FindEntry( g_AnnouncerProfile[ulProfileIdx], pszEntry );
	if (( pEntry ) && ( pEntry->szSound ))
		S_Sound( CHAN_VOICE, pEntry->szSound, 1, ATTN_NONE );
}

//*****************************************************************************
//
//GHK: Hijack this for playing coop & team coop(?) lead info. 
void ANNOUNCER_PlayFragSounds( ULONG ulPlayer, LONG lOldFragCount, LONG lNewFragCount )
{
	ULONG			ulIdx;
	LONG			lOldLeaderFrags;
	LONG			lNewLeaderFrags;
	LEADSTATE_e		OldLeadState;
	LEADSTATE_e		LeadState;

	// Announce a lead change. Only do if the consoleplayer's camera is valid.
	if (( players[consoleplayer].mo != NULL ) &&
		( players[consoleplayer].camera != NULL ) &&
		( players[consoleplayer].camera->player != NULL ) &&
		( players[consoleplayer].camera->player->bSpectating == false ))
	{
		// Find the highest fragcount of a player other than the player whose eye's we're
		// looking through. This is compared to the player's old fragcount, and the player's
		// new fragcount. If the player's lead state has changed, play a sound.
		lOldLeaderFrags = INT_MIN;
		lNewLeaderFrags = INT_MIN;
		for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		{
			// Don't factor our fragcount into the highest fragcount.
			if ( players[ulIdx].mo->CheckLocalView( consoleplayer ))
				continue;

			// Don't factor in players who are not in the game, or who are spectating.
			if (( playeringame[ulIdx] == false ) || ( players[ulIdx].bSpectating ))
				continue;

			if ( ulIdx == ulPlayer )
			{
				if ( lOldLeaderFrags < lOldFragCount )
					lOldLeaderFrags = lOldFragCount;
				if ( lNewLeaderFrags < lNewFragCount )
					lNewLeaderFrags = lNewFragCount;
			}
			else
			{
				if ( lOldLeaderFrags < players[ulIdx].lPointCount )
					lOldLeaderFrags = players[ulIdx].lPointCount;
				if ( lNewLeaderFrags < players[ulIdx].lPointCount )
					lNewLeaderFrags = players[ulIdx].lPointCount;
			}
		}

		// If players were found in game, just break out, since our lead couldn't have possibly changed.
		if (( lOldLeaderFrags == INT_MIN ) && ( lNewLeaderFrags == INT_MIN ))
			return;

		if ( players[ulPlayer].mo->CheckLocalView( consoleplayer ))
		{
			OldLeadState = ( lOldFragCount > lOldLeaderFrags ) ? LEADSTATE_INTHELEAD : ( lOldFragCount == lOldLeaderFrags ) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_NOTINTHELEAD;
			LeadState = ( lNewFragCount > lNewLeaderFrags ) ? LEADSTATE_INTHELEAD : ( lNewFragCount == lNewLeaderFrags ) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_NOTINTHELEAD;
		}
		else
		{
			OldLeadState = ( players[consoleplayer].camera->player->lPointCount > lOldLeaderFrags ) ? LEADSTATE_INTHELEAD : ( players[consoleplayer].camera->player->lPointCount == lOldLeaderFrags ) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_NOTINTHELEAD;
			LeadState = ( players[consoleplayer].camera->player->lPointCount > lNewLeaderFrags ) ? LEADSTATE_INTHELEAD : ( players[consoleplayer].camera->player->lPointCount == lNewLeaderFrags ) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_NOTINTHELEAD;
		}

		// If our lead state has changed, play a sound.
		if ( OldLeadState != LeadState )
		{
			switch ( LeadState )
			{
			// Display player has taken the lead!
			case LEADSTATE_INTHELEAD:
			
				C_MidPrintSD("You Have Taken The Lead!");
				if (LastAnnounceTime == 0 || LastAnnounceTime <= level.time-5)
					ANNOUNCER_PlayEntry( cl_announcer, "YouveTakenTheLead" );
				break;
			// Display player is tied for the lead!
			//case LEADSTATE_TIEDFORTHELEAD:
				//C_MidPrintSD("You Are Tied For The Lead");
				//if (LastAnnounceTime == 0 || LastAnnounceTime <= level.time-5)
					//ANNOUNCER_PlayEntry( cl_announcer, "YouAreTiedForTheLead" );
				//break;
			// Display player has lost the lead.
			case LEADSTATE_NOTINTHELEAD:

				C_MidPrintSD("You Have Lost The Lead!");
				if (LastAnnounceTime == 0 || LastAnnounceTime <= level.time-5)
					ANNOUNCER_PlayEntry( cl_announcer, "YouveLostTheLead" );
				break;
			}
		}
	}

	// Potentially play the "3 frags left", etc. announcer sounds.
	/*if (( lastmanstanding == false ) && ( teamlms == false ) && ( deathmatch ) && ( fraglimit ))
	{
		switch ( fraglimit - lNewFragCount )
		{
		case 3:

			if ( g_bThreeFragsLeftSoundPlayed == false )
			{
				ANNOUNCER_PlayEntry( cl_announcer, "ThreeFragsLeft" );
				g_bThreeFragsLeftSoundPlayed = true;
			}
			break;
		case 2:
			
			if ( g_bTwoFragsLeftSoundPlayed == false )
			{
				ANNOUNCER_PlayEntry( cl_announcer, "TwoFragsLeft" );
				g_bTwoFragsLeftSoundPlayed = true;
			}
			break;
		case 1:

			if ( g_bOneFragLeftSoundPlayed == false )
			{
				ANNOUNCER_PlayEntry( cl_announcer, "OneFragLeft" );
				g_bOneFragLeftSoundPlayed = true;
			}
			break;
		}
	} */
}

//*****************************************************************************
//
void ANNOUNCER_PlayTeamFragSounds( ULONG ulTeam, LONG lOldFragCount, LONG lNewFragCount )
{
	LEADSTATE_e		OldBlueLeadState;
	LEADSTATE_e		BlueLeadState;

	// The given team is not valid.
	if ( ulTeam >= NUM_TEAMS )
		return;

	// Determine the lead state by comparing our score to whoever has the highest score that isn't us.
	if ( ulTeam == TEAM_BLUE )
	{
		OldBlueLeadState = ( lOldFragCount > TEAM_GetScore( TEAM_RED )) ? LEADSTATE_INTHELEAD : ( lOldFragCount == TEAM_GetScore( TEAM_RED )) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_NOTINTHELEAD;
		BlueLeadState = ( lNewFragCount > TEAM_GetScore( TEAM_RED )) ? LEADSTATE_INTHELEAD : ( lNewFragCount == TEAM_GetScore( TEAM_RED )) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_NOTINTHELEAD;
	}
	else
	{
		OldBlueLeadState = ( lOldFragCount > TEAM_GetScore( TEAM_BLUE )) ? LEADSTATE_NOTINTHELEAD : ( lOldFragCount == TEAM_GetScore( TEAM_BLUE )) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_INTHELEAD;
		BlueLeadState = ( lNewFragCount > TEAM_GetScore( TEAM_BLUE )) ? LEADSTATE_NOTINTHELEAD : ( lNewFragCount == TEAM_GetScore( TEAM_BLUE )) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_INTHELEAD;
	}

	// If blue's lead state has changed, play a sound.
	if ( OldBlueLeadState != BlueLeadState )
	{
		switch ( BlueLeadState )
		{
		// Blue is in the lead!
		case LEADSTATE_INTHELEAD:

			ANNOUNCER_PlayEntry( cl_announcer, "BlueLeads" );
			break;
		// Teams are tied!
		case LEADSTATE_TIEDFORTHELEAD:

			ANNOUNCER_PlayEntry( cl_announcer, "TeamsAreTied" );
			break;
		// If blue is not in the lead, then red leads!
		case LEADSTATE_NOTINTHELEAD:

			ANNOUNCER_PlayEntry( cl_announcer, "RedLeads" );
			break;
		}
	}

	// Potentially play the "3 frags left", etc. announcer sounds.
	/*if ( fraglimit )
	{
		switch ( fraglimit - lNewFragCount )
		{
		case 3:

			if ( g_bThreeFragsLeftSoundPlayed == false )
			{
				ANNOUNCER_PlayEntry( cl_announcer, "ThreeFragsLeft" );
				g_bThreeFragsLeftSoundPlayed = true;
			}
			break;
		case 2:
			
			if ( g_bTwoFragsLeftSoundPlayed == false )
			{
				ANNOUNCER_PlayEntry( cl_announcer, "TwoFragsLeft" );
				g_bTwoFragsLeftSoundPlayed = true;
			}
			break;
		case 1:

			if ( g_bOneFragLeftSoundPlayed == false )
			{
				ANNOUNCER_PlayEntry( cl_announcer, "OneFragLeft" );
				g_bOneFragLeftSoundPlayed = true;
			}
			break;
		}
	}*/
}

//*****************************************************************************
//
/*
void ANNOUNCER_PlayScoreSounds( ULONG ulPlayer, LONG lOldPointCount, LONG lNewPointCount )
{
	ULONG			ulIdx;
	LONG			lHighestPointCount;
	LEADSTATE_e		OldLeadState;
	LEADSTATE_e		LeadState;

	// Announce a lead change. Only do if the consoleplayer's camera is valid.
	if (( players[consoleplayer].mo != NULL ) &&
		( players[consoleplayer].camera != NULL ) &&
		( players[consoleplayer].camera->player != NULL ) &&
		( players[consoleplayer].camera->player->bSpectating == false ))
	{
		// Find the highest fragcount of a player other than the player whose eye's we're
		// looking through. This is compared to the player's old fragcount, and the player's
		// new fragcount. If the player's lead state has changed, play a sound.
		lHighestPointCount = INT_MIN;
		for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
		{
			// Don't factor our fragcount into the highest fragcount.
			if ( players[ulIdx].mo->CheckLocalView( consoleplayer ))
				continue;

			// Don't factor in players who are not in the game, or who are spectating.
			if (( playeringame[ulIdx] == false ) || ( players[ulIdx].bSpectating ))
				continue;

			if ( lHighestPointCount < players[ulIdx].lPointCount )
				lHighestPointCount = players[ulIdx].lPointCount;
		}

		// If players were found in game, just break out, since our lead couldn't have possibly changed.
		if ( lHighestPointCount == INT_MIN )
			return;

		// Determine the lead state by comparing our score to whoever has the highest score that isn't us.
		if ( players[ulPlayer].mo->CheckLocalView( consoleplayer ))
		{
			OldLeadState = ( lOldPointCount > lHighestPointCount ) ? LEADSTATE_INTHELEAD : ( lOldPointCount == lHighestPointCount ) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_NOTINTHELEAD;
			LeadState = ( lNewPointCount > lHighestPointCount ) ? LEADSTATE_INTHELEAD : ( lNewPointCount == lHighestPointCount ) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_NOTINTHELEAD;
		}
		else
		{
			OldLeadState = ( players[consoleplayer].lPointCount > lHighestPointCount ) ? LEADSTATE_INTHELEAD : ( players[consoleplayer].lPointCount == lHighestPointCount ) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_NOTINTHELEAD;

			if ( lHighestPointCount < lNewPointCount )
				lHighestPointCount = lNewPointCount;

			LeadState = ( players[consoleplayer].lPointCount > lHighestPointCount ) ? LEADSTATE_INTHELEAD : ( players[consoleplayer].lPointCount == lHighestPointCount ) ? LEADSTATE_TIEDFORTHELEAD : LEADSTATE_NOTINTHELEAD;
		}

		// If our lead state has changed, play a sound.
		if ( OldLeadState != LeadState )
		{
			switch ( LeadState )
			{
			// Display player has taken the lead!
			case LEADSTATE_INTHELEAD:

				ANNOUNCER_PlayEntry( cl_announcer, "YouveTakenTheLead" );
				break;
			// Display player is tied for the lead!
			case LEADSTATE_TIEDFORTHELEAD:

				ANNOUNCER_PlayEntry( cl_announcer, "YouAreTiedForTheLead" );
				break;
			// Display player has lost the lead.
			case LEADSTATE_NOTINTHELEAD:

				ANNOUNCER_PlayEntry( cl_announcer, "YouveLostTheLead" );
				break;
			}
		}
	}

	// Potentially play the "3 points left", etc. announcer sounds.
	if (( lastmanstanding == false ) && ( teamlms == false ) && ( deathmatch ) && ( pointlimit ))
	{
		switch ( pointlimit - lNewPointCount )
		{
		case 3:

			if ( g_bThreePointsLeftSoundPlayed == false )
			{
				ANNOUNCER_PlayEntry( cl_announcer, "ThreePointsLeft" );
				g_bThreePointsLeftSoundPlayed = true;
			}
			break;
		case 2:
			
			if ( g_bTwoPointsLeftSoundPlayed == false )
			{
				ANNOUNCER_PlayEntry( cl_announcer, "TwoPointsLeft" );
				g_bTwoPointsLeftSoundPlayed = true;
			}
			break;
		case 1:

			if ( g_bOnePointLeftSoundPlayed == false )
			{
				ANNOUNCER_PlayEntry( cl_announcer, "OnePointLeft" );
				g_bOnePointLeftSoundPlayed = true;
			}
			break;
		}
	}
}
*/
//*****************************************************************************
//
void ANNOUNCER_AllowNumFragsAndPointsLeftSounds( void )
{
	g_bThreeFragsLeftSoundPlayed = false;
	g_bTwoFragsLeftSoundPlayed = false;
	g_bOneFragLeftSoundPlayed = false;

	g_bThreePointsLeftSoundPlayed = false;
	g_bTwoPointsLeftSoundPlayed = false;
	g_bOnePointLeftSoundPlayed = false;
}

//*****************************************************************************
//*****************************************************************************
//
char *ANNOUNCER_GetName( ULONG ulIdx )
{
	if ( ulIdx >= MAX_ANNOUNCERPROFILES || ( g_AnnouncerProfile[ulIdx] == NULL ))
		return ( NULL );

	return ( g_AnnouncerProfile[ulIdx]->szName );
}

//*****************************************************************************
//*****************************************************************************
//
bool announcer_AddAnnouncerProfile( ANNOUNCERPROFILE_t *pInfo )
{
	ULONG	ulIdx;
	ULONG	ulIdx2;

	// First, find a free slot to add the announcer profile.
	for ( ulIdx = 0; ulIdx < MAX_ANNOUNCERPROFILES; ulIdx++ )
	{
		if ( g_AnnouncerProfile[ulIdx] != NULL )
			continue;

		// Allocate some memory for this new block.
		g_AnnouncerProfile[ulIdx] = (ANNOUNCERPROFILE_t *)malloc( sizeof( ANNOUNCERPROFILE_t ));

		// Now copy all the data we passed in into this block.
		sprintf( g_AnnouncerProfile[ulIdx]->szName, "%s", pInfo->szName );
		g_AnnouncerProfile[ulIdx]->paAnnouncerEntries = (ANNOUNCERENTRY_t **)malloc( sizeof( ANNOUNCERENTRY_t ) * MAX_ANNOUNCERPROFILE_ENTRIES );
		for ( ulIdx2 = 0; ulIdx2 < MAX_ANNOUNCERPROFILE_ENTRIES; ulIdx2++ )
		{
			g_AnnouncerProfile[ulIdx]->paAnnouncerEntries[ulIdx2] = (ANNOUNCERENTRY_t *)malloc( sizeof( ANNOUNCERENTRY_t ));

			sprintf( g_AnnouncerProfile[ulIdx]->paAnnouncerEntries[ulIdx2]->szName, "%s", pInfo->paAnnouncerEntries[ulIdx2]->szName );
			sprintf( g_AnnouncerProfile[ulIdx]->paAnnouncerEntries[ulIdx2]->szSound, "%s", pInfo->paAnnouncerEntries[ulIdx2]->szSound );
		}

		return ( true );
	}

	return ( false );
}

//*****************************************************************************
//
void announcer_AddProfileEntry( ANNOUNCERPROFILE_t *pInfo, const char *pszEntry, const char *pszSound )
{
	ULONG	ulIdx;

	// Invalid profile.
	if ( pInfo == NULL )
		return;

	// The profile has no entries.
	if ( pInfo->paAnnouncerEntries == NULL )
		return;

	// Add the entry to the first available slot.
	for ( ulIdx = 0; ulIdx < MAX_ANNOUNCERPROFILE_ENTRIES; ulIdx++ )
	{
		// Entry already exists.
		if ( stricmp( pInfo->paAnnouncerEntries[ulIdx]->szName, pszEntry ) == 0 )
			return;

		if ( pInfo->paAnnouncerEntries[ulIdx]->szName[0] == '\0' )
		{
			sprintf( pInfo->paAnnouncerEntries[ulIdx]->szName, "%s", pszEntry );
			sprintf( pInfo->paAnnouncerEntries[ulIdx]->szSound, "%s", pszSound );

			// All done.
			return;
		}
	}
}

//*****************************************************************************
//
ANNOUNCERENTRY_t *announcer_FindEntry( ANNOUNCERPROFILE_t *pInfo, const char *pszEntry )
{
	ULONG	ulIdx;

	// Invalid profile.
	if ( pInfo == NULL )
		return ( NULL );

	// The profile has no entries.
	if ( pInfo->paAnnouncerEntries == NULL )
		return ( NULL );

	// Search the announcer profile for the entry. If it exists, return it.
	// FIXME: Use the binary search here.
	for ( ulIdx = 0; ulIdx < MAX_ANNOUNCERPROFILE_ENTRIES; ulIdx++ )
	{
		if ( stricmp( pInfo->paAnnouncerEntries[ulIdx]->szName, pszEntry ) == 0 )
			return ( pInfo->paAnnouncerEntries[ulIdx] );
	}

	return ( NULL );
}

//*****************************************************************************
//
void announcer_FinishAddingEntries( ANNOUNCERPROFILE_t *pInfo )
{
	// FIXME: Sort this list alphabetically.
}

//*****************************************************************************
//	CONSOLE VARIABLES/COMMANDS

CVAR( Int, cl_announcer, 0, CVAR_ARCHIVE )

// Display all the announce profiles that are loaded.
CCMD( announcers )
{
	ULONG	ulIdx;
	ULONG	ulNumProfiles = 0;

	for ( ulIdx = 0; ulIdx < MAX_ANNOUNCERPROFILES; ulIdx++ )
	{
		if ( g_AnnouncerProfile[ulIdx] != NULL )
		{
			Printf( "%d. %s\n", ulIdx + 1, g_AnnouncerProfile[ulIdx]->szName );
			ulNumProfiles++;
		}
	}

	Printf( "\n%d announcer profile(s) loaded.\n", ulNumProfiles );
}
