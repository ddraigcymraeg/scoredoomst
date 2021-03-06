/*
** a_springpad.cpp
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

#include "info.h"
#include "r_defs.h"

// Spring pad zone ----------------------------------------------------------

class ASpringPadZone : public AActor
{
	DECLARE_STATELESS_ACTOR( ASpringPadZone, AActor )
public:
	void	PostBeginPlay( );
};

IMPLEMENT_STATELESS_ACTOR( ASpringPadZone, Any, 5068, 0 )
	PROP_Flags( MF_NOBLOCKMAP|MF_NOSECTOR|MF_NOGRAVITY )
	PROP_Flags3( MF3_DONTSPLASH )
	PROP_FlagsNetwork( NETFL_ALLOWCLIENTSPAWN )
END_DEFAULTS

void ASpringPadZone::PostBeginPlay( )
{
	Super::PostBeginPlay( );

	// Make this sector bounce actors up at the same velocity they landed on it with.
	Sector->FloorFlags |= SECF_SPRINGPAD;

	// This object is no longer needed, so just kill it.
	Destroy( );
}
