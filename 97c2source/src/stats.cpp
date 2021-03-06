/*
** stats.cpp
** Performance-monitoring statistics
**
**---------------------------------------------------------------------------
** Copyright 1998-2006 Randy Heit
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

#include "doomtype.h"
#include "stats.h"
#include "v_video.h"
#include "v_text.h"
#include "hu_stuff.h"
#include "st_stuff.h"
#include "c_dispatch.h"
#include "m_swap.h"
#include "network.h"

FStat *FStat::FirstStat;

FStat::FStat (const char *name)
{
	m_Name = name;
	m_Active = false;
	m_Next = FirstStat;
	FirstStat = this;
}

FStat::~FStat ()
{
	FStat **prev = &FirstStat;

	while (*prev && *prev != this)
		prev = &((*prev)->m_Next)->m_Next;

	if (*prev == this)
		*prev = m_Next;
}

FStat *FStat::FindStat (const char *name)
{
	FStat *stat = FirstStat;

	while (stat && stricmp (name, stat->m_Name))
		stat = stat->m_Next;

	return stat;
}

void FStat::ToggleStat (const char *name)
{
	FStat *stat = FindStat (name);
	if (stat)
		stat->ToggleStat ();
	else
		Printf ("Unknown stat: %s\n", name);
}

void FStat::ToggleStat ()
{
	m_Active = !m_Active;
	SB_state = screen->GetPageCount ();
}

void FStat::PrintStat ()
{
	// [BC] The server doesn't actually load any fonts.
	int y = ( NETWORK_GetState( ) == NETSTATE_SERVER ) ? 0 : SCREENHEIGHT - SmallFont->GetHeight();
	int count = 0;

	// [BC] The server doesn't actually load any fonts.
	if ( NETWORK_GetState( ) != NETSTATE_SERVER )
		screen->SetFont (ConFont);
	for (FStat *stat = FirstStat; stat != NULL; stat = stat->m_Next)
	{
		if (stat->m_Active)
		{
			FString stattext(stat->GetStats());
			// [BC] In server mode, just display the stats in the console.
			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				Printf( "%s\n", stattext );
			else
			{
				screen->DrawText (CR_GREEN, 5, y, stattext, TAG_DONE);
				y -= SmallFont->GetHeight() + 1;
				count++;
			}
		}
	}
	// [BC] The server doesn't actually load any fonts.
	if ( NETWORK_GetState( ) != NETSTATE_SERVER )
	{
		screen->SetFont (SmallFont);
		if (count)
		{
			SB_state = screen->GetPageCount ();
		}
	}
}

void FStat::DumpRegisteredStats ()
{
	FStat *stat = FirstStat;

	Printf ("Available stats:\n");
	while (stat)
	{
		Printf (" %c%s\n", stat->m_Active ? '*' : ' ', stat->m_Name);
		stat = stat->m_Next;
	}
}

CCMD (stat)
{
	if (argv.argc() != 2)
	{
		Printf ("Usage: stat <statistics>\n");
		FStat::DumpRegisteredStats ();
	}
	else
	{
		FStat::ToggleStat (argv[1]);
	}
}
