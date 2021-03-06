/*
** a_doomarmor.cpp
** Contains the definitions for Skulltag's red armor.
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

#include "a_pickups.h"
#include "d_player.h"

// Red armor ---------------------------------------------------------------
// Skulltag's red armor must be defined internally since it has special
// handling for ::AbsorbDamage.

class ARedArmor : public ABasicArmorPickup
{
	DECLARE_ACTOR( ARedArmor, ABasicArmorPickup )
public:

	bool	Use( bool bPickup );
//	void	AbsorbDamage( int damage, int damageType, int &newdamage );
};

FState ARedArmor::States[ ] =
{
	S_NORMAL( ARM3, 'A',	6, NULL 				, &States[1] ),
	S_BRIGHT( ARM3, 'B',	6, NULL 				, &States[0] )
};

IMPLEMENT_ACTOR( ARedArmor, Doom, 5040, 168 )
	PROP_RadiusFixed( 20 )
	PROP_HeightFixed( 16 )
	PROP_Flags( MF_SPECIAL )
	PROP_FlagsST( STFL_SUPERARMOR )
	PROP_SpawnState( 0 )

	PROP_BasicArmorPickup_SavePercent( 66 )
	PROP_BasicArmorPickup_SaveAmount( 200 )
	PROP_Inventory_Icon( "ARM3A0" )
	PROP_Inventory_PickupMessage( "$PICKUP_REDARMOR" )
END_DEFAULTS

bool ARedArmor::Use( bool bPickup )
{
	if ( Super::Use( bPickup ))
	{
		// Red armor gives the player the power of fire resistance.
		if ( Owner->player )
			Owner->player->Powers |= PW_FIRERESISTANT;

		return ( true );
	}

	return ( false );
}
/*
void ARedArmor::AbsorbDamage( int damage, int damageType, int &newdamage )
{
	if (damageType != MOD_WATER)
	{
		int saved = FixedMul (damage, SavePercent);
		if (Amount < saved)
		{
			saved = Amount;
		}

		newdamage -= saved;
		Amount -= saved;
		if (Amount == 0)
		{
			// The armor has become useless
			SavePercent = 0;
			// Now see if the player has some more armor in their inventory
			// and use it if so. As in Strife, the best armor is used up first.
			ABasicArmorPickup *best = NULL;
			AInventory *probe = Owner->Inventory;
			while (probe != NULL)
			{
				if (probe->IsKindOf (RUNTIME_CLASS(ABasicArmorPickup)))
				{
					ABasicArmorPickup *inInv = static_cast<ABasicArmorPickup*>(probe);
					if (best == NULL || best->SavePercent < inInv->SavePercent)
					{
						best = inInv;
					}
				}
				probe = probe->Inventory;
			}
			if (best != NULL)
			{
				Owner->UseInventory (best);
			}
		}

		// With red armor, the amount of damage taken by fire is cut in 8.
		if ( damageType == MOD_FIRE )
			newdamage /= 8;
	}
	if ( Inventory )
	{
		Inventory->AbsorbDamage( damage, damageType, newdamage );
	}
}
*/
