/*
** maprotation.h
** Contains bot structures and prototypes
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

#ifndef	__MAPROTATION_H__
#define	__MAPROTATION_H__

//*****************************************************************************
//	DEFINES

#define	MAX_MAPROTATIONLIST_ENTRIES		64

//*****************************************************************************
//	STRUCTURES

typedef struct
{
	// The name of the map in this entry.
	char	szMapName[10];

	// Has this map already been used in the rotation?
	bool	bUsed;

} MAPROTATIONENTRY_t;

//*****************************************************************************
//	PROTOTYPES

void	MAPROTATION_Construct( void );

ULONG	MAPROTATION_GetNumUnusedEntries( void ); //ghk
ULONG	MAPROTATION_GetNumEntries( void );
void	MAPROTATION_AdvanceMap( void );
char	*MAPROTATION_GetCurrentMapName( void );
char	*MAPROTATION_GetMapName( ULONG ulIdx );
void	MAPROTATION_SetPositionToMap( char *pszMapName );

static	ULONG	g_SDNumUnusedEntries = 0; //ghk
//*****************************************************************************
//  EXTERNAL CONSOLE VARIABLES

EXTERN_CVAR( Bool, sv_maprotation )
EXTERN_CVAR( Bool, sv_randommaprotation )

#endif	// __MAPROTATION_H__
