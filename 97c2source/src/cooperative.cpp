/*
** cooperative.cpp
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

#include "cooperative.h"
#include "deathmatch.h"
#include "team.h"

//*****************************************************************************
//	CONSOLE COMMANDS/VARIABLES


CUSTOM_CVAR( Bool, cooperative, true, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = false;

		// Disable deathmatch and teamgame if we're playing cooperative.
		if ( deathmatch != false )
			deathmatch.ForceSet( Val, CVAR_Bool );
		if ( teamgame != false )
			teamgame.ForceSet( Val, CVAR_Bool );
	}
	else
	{
		Val.Bool = false;

		// Cooperative, so disable all related modes.
		survival.ForceSet( Val, CVAR_Bool );
		invasion.ForceSet( Val, CVAR_Bool );
		//teamcoop.ForceSet( Val, CVAR_Bool );

	}
}

//*****************************************************************************
//
CUSTOM_CVAR( Bool, survival, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = true;

		// Enable cooperative.
		cooperative.ForceSet( Val, CVAR_Bool );

		Val.Bool = false;

		// Disable other modes.
		invasion.ForceSet( Val, CVAR_Bool );
	}
}

//*****************************************************************************
//
CUSTOM_CVAR( Bool, invasion, false, CVAR_SERVERINFO | CVAR_LATCH | CVAR_CAMPAIGNLOCK )
{
	UCVarValue	Val;

	if ( self == true )
	{
		Val.Bool = true;

		// Enable cooperative.
		cooperative.ForceSet( Val, CVAR_Bool );

		Val.Bool = false;

		// Disable other modes.
		survival.ForceSet( Val, CVAR_Bool );
	}
}

//*****************************************************************************
//*****************************************************************************
//	CONSOLE VARIABLES

