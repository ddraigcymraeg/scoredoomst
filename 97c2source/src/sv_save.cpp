/*
** sv_save.cpp
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

#include "network.h"
#include "sv_save.h"
#include "v_text.h"

//*****************************************************************************
//	VARIABLES

// Global list of saved information.
static	PLAYERSAVEDINFO_t	g_SavedPlayerInfo[MAXPLAYERS];

//*****************************************************************************
//	PROTOTYPES

void	server_save_UpdateSlotWithInfo( ULONG ulSlot, PLAYERSAVEDINFO_t *pInfo );

//*****************************************************************************
//	FUNCTIONS

void SERVER_SAVE_Construct( void )
{
	// Initialzed the saved player info list.
	SERVER_SAVE_ClearList( );
}

//*****************************************************************************
//
PLAYERSAVEDINFO_t *SERVER_SAVE_GetSavedInfo( char *pszPlayerName, netadr_t Address )
{
	ULONG	ulIdx;
	char	szPlayerName[128];

	sprintf( szPlayerName, "%s", pszPlayerName );
	V_RemoveColorCodes( szPlayerName );

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if ( g_SavedPlayerInfo[ulIdx].bInitialized == false )
			continue;

		if (( stricmp( szPlayerName, g_SavedPlayerInfo[ulIdx].szName ) == 0 ) &&
			( NETWORK_CompareAddress( Address, g_SavedPlayerInfo[ulIdx].Address, false )))
		{
			return ( &g_SavedPlayerInfo[ulIdx] );
		}
	}

	return ( NULL );
}

//*****************************************************************************
//
void SERVER_SAVE_ClearList( void )
{
	ULONG	ulIdx;

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		g_SavedPlayerInfo[ulIdx].Address.ip[0] = 0;
		g_SavedPlayerInfo[ulIdx].Address.ip[1] = 0;
		g_SavedPlayerInfo[ulIdx].Address.ip[2] = 0;
		g_SavedPlayerInfo[ulIdx].Address.ip[3] = 0;
		g_SavedPlayerInfo[ulIdx].Address.port = 0;
		g_SavedPlayerInfo[ulIdx].bInitialized = false;
		g_SavedPlayerInfo[ulIdx].lFragCount = 0;
		g_SavedPlayerInfo[ulIdx].lPointCount = 0;
		g_SavedPlayerInfo[ulIdx].lWinCount = 0;
		g_SavedPlayerInfo[ulIdx].szName[0] = 0;
	}
}

//*****************************************************************************
//
void SERVER_SAVE_SaveInfo( PLAYERSAVEDINFO_t *pInfo )
{
	ULONG	ulIdx;
	char	szPlayerName[128];

	sprintf( szPlayerName, "%s", pInfo->szName );
	V_RemoveColorCodes( szPlayerName );

	for ( ulIdx = 0; ulIdx < MAXPLAYERS; ulIdx++ )
	{
		if ( g_SavedPlayerInfo[ulIdx].bInitialized )
		{
			// If this slot matches the player we're trying to save, just update it.
			if (( stricmp( szPlayerName, g_SavedPlayerInfo[ulIdx].szName ) == 0 ) &&
				( NETWORK_CompareAddress( pInfo->Address, g_SavedPlayerInfo[ulIdx].Address, false )))
			{
				server_save_UpdateSlotWithInfo( ulIdx, pInfo );
				return;
			}

			continue;
		}

		server_save_UpdateSlotWithInfo( ulIdx, pInfo );
		return;
	}
}

//*****************************************************************************
//*****************************************************************************
//
void server_save_UpdateSlotWithInfo( ULONG ulSlot, PLAYERSAVEDINFO_t *pInfo )
{
	if (( ulSlot < 0 ) || ( ulSlot >= MAXPLAYERS ))
		return;

	g_SavedPlayerInfo[ulSlot].bInitialized		= true;
	g_SavedPlayerInfo[ulSlot].Address			= pInfo->Address;
	g_SavedPlayerInfo[ulSlot].lFragCount		= pInfo->lFragCount;
	g_SavedPlayerInfo[ulSlot].lPointCount		= pInfo->lPointCount;
	g_SavedPlayerInfo[ulSlot].lWinCount			= pInfo->lWinCount;
	sprintf( g_SavedPlayerInfo[ulSlot].szName, pInfo->szName );

	V_RemoveColorCodes( g_SavedPlayerInfo[ulSlot].szName );
}
