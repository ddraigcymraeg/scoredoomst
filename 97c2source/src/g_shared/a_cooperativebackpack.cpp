/*
** a_cooperativebackpack.cpp
** Contains definitions for the cooperative backpack.
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

#include "a_sharedglobal.h"
#include "gstrings.h"
#include "sv_commands.h"

// Cooperative backpack -----------------------------------------------------

// This special backpack is spawned when a player dies. It contains his weapons,
// ammo, and a normal backpack if the player carried one.

FState ACooperativeBackpack::States[] =
{
	S_NORMAL( BPK2, 'A',   -1, NULL 						, NULL )
};

IMPLEMENT_ACTOR( ACooperativeBackpack, Any, -1, -1 )
	PROP_HeightFixed (26)
	PROP_Flags( MF_SPECIAL )
	PROP_SpawnState( 0 )
END_DEFAULTS

//===========================================================================
//
// ACooperativeBackpack :: TryPickup
//
// Transfers all the inventory from the backpack to the player picking it up.
//
//===========================================================================

bool ACooperativeBackpack::TryPickup( AActor *pToucher )
{
	AInventory	*pInventory;
	AInventory	*pGivenInventory;

	// First, do non-ammo.
	pInventory = Inventory;
	while ( pInventory )
	{
		if ( pInventory->IsKindOf( RUNTIME_CLASS( AAmmo )))
		{
			pInventory = pInventory->Inventory;
			continue;
		}

		pGivenInventory = pToucher->GiveInventoryType( pInventory->GetClass( ));
		if ( pGivenInventory )
		{
			pGivenInventory->Amount = pInventory->Amount;

			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				SERVERCOMMANDS_GiveInventory( ULONG( pToucher->player - players ), pGivenInventory );
		}

		pInventory = pInventory->Inventory;
	}

	// Then, do ammo.
	pInventory = Inventory;
	while ( pInventory )
	{
		if ( pInventory->IsKindOf( RUNTIME_CLASS( AAmmo )) == false )
		{
			pInventory = pInventory->Inventory;
			continue;
		}

		pGivenInventory = pToucher->GiveInventoryType( pInventory->GetClass( ));
		if ( pGivenInventory )
		{
			pGivenInventory->Amount = pInventory->Amount;

			if ( NETWORK_GetState( ) == NETSTATE_SERVER )
				SERVERCOMMANDS_GiveInventory( ULONG( pToucher->player - players ), pGivenInventory );
		}

		pInventory = pInventory->Inventory;
	}

	GoAwayAndDie( );
	return ( true );
}

//===========================================================================
//
// ACooperativeBackpack :: FillBackpack
//
// Transfer key inventory items from the player to the backpack.
//
//===========================================================================

void ACooperativeBackpack::FillBackpack( player_s *pPlayer )
{
	AInventory	*pInventory;
	AInventory	*pGivenInventory;

	// If the player is NULL or his body doesn't exist, then we can't fill the
	// backpack.
	if (( pPlayer == NULL ) || ( pPlayer->mo == NULL ))
		return;

	// First, do non-ammo.
	pInventory = pPlayer->mo->Inventory;
	while ( pInventory )
	{
		if (( pInventory->GetClass( )->IsDescendantOf( RUNTIME_CLASS( AWeapon ))) ||
			( pInventory->GetClass( )->IsDescendantOf( RUNTIME_CLASS( ABackpack ))) ||
			( pInventory->GetClass( )->IsDescendantOf( RUNTIME_CLASS( ARune ))))
		{
			pGivenInventory = this->GiveInventoryType( pInventory->GetClass( ));
			if ( pGivenInventory )
				pGivenInventory->Amount = pInventory->Amount;
		}

		pInventory = pInventory->Inventory;
	}

	// Then, do ammo.
	pInventory = pPlayer->mo->Inventory;
	while ( pInventory )
	{
		if ( pInventory->GetClass( )->IsDescendantOf( RUNTIME_CLASS( AAmmo )))
		{
			pGivenInventory = this->GiveInventoryType( pInventory->GetClass( ));
			if ( pGivenInventory )
				pGivenInventory->Amount = pInventory->Amount;
		}

		pInventory = pInventory->Inventory;
	}
}

//===========================================================================
//
// ACooperativeBackpack :: PickupMessage
//
//===========================================================================

const char *ACooperativeBackpack::PickupMessage( )
{
	return GStrings( "PICKUP_COOPERATIVEBACKPACK" );
}

//===========================================================================
//
// ACooperativeBackpack :: ShouldRespawn
//
//===========================================================================

bool ACooperativeBackpack::ShouldRespawn( )
{
	return ( false );
}
