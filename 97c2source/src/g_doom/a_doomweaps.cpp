#include "actor.h"
#include "info.h"
#include "s_sound.h"
#include "m_random.h"
#include "a_pickups.h"
#include "a_doomglobal.h"
#include "d_player.h"
#include "p_pspr.h"
#include "p_local.h"
#include "gstrings.h"
#include "p_effect.h"
#include "gi.h"
#include "templates.h"
#include "thingdef.h"
#include "deathmatch.h"
#include "g_game.h"
#include "sv_commands.h"
#include "team.h"
#include "p_enemy.h"

static FRandom pr_punch ("Punch");
static FRandom pr_saw ("Saw");
static FRandom pr_fireshotgun2 ("FireSG2");
static FRandom pr_fireplasma ("FirePlasma");
static FRandom pr_firerail ("FireRail");
static FRandom pr_bfgspray ("BFGSpray");

// Fist ---------------------------------------------------------------------

void A_Punch (AActor *);

class AFist : public AWeapon
{
	DECLARE_ACTOR (AFist, AWeapon)
};

FState AFist::States[] =
{
#define S_PUNCH 0
	S_NORMAL (PUNG, 'A',	1, A_WeaponReady		, &States[S_PUNCH]),

#define S_PUNCHDOWN (S_PUNCH+1)
	S_NORMAL (PUNG, 'A',	1, A_Lower				, &States[S_PUNCHDOWN]),

#define S_PUNCHUP (S_PUNCHDOWN+1)
	S_NORMAL (PUNG, 'A',	1, A_Raise				, &States[S_PUNCHUP]),

#define S_PUNCH1 (S_PUNCHUP+1)
	S_NORMAL (PUNG, 'B',	4, NULL 				, &States[S_PUNCH1+1]),
	S_NORMAL (PUNG, 'C',	4, A_Punch				, &States[S_PUNCH1+2]),
	S_NORMAL (PUNG, 'D',	5, NULL 				, &States[S_PUNCH1+3]),
	S_NORMAL (PUNG, 'C',	4, NULL 				, &States[S_PUNCH1+4]),
	S_NORMAL (PUNG, 'B',	5, A_ReFire 			, &States[S_PUNCH])
};

IMPLEMENT_ACTOR (AFist, Doom, -1, 0)
	PROP_Weapon_SelectionOrder (3700)
	// [BC] We can use this weapon while respawn invulnerability is active.
	PROP_Weapon_Flags (WIF_WIMPY_WEAPON|WIF_ALLOW_WITH_RESPAWN_INVUL)
	PROP_Weapon_UpState (S_PUNCHUP)
	PROP_Weapon_DownState (S_PUNCHDOWN)
	PROP_Weapon_ReadyState (S_PUNCH)
	PROP_Weapon_AtkState (S_PUNCH1)
	PROP_Weapon_HoldAtkState (S_PUNCH1)
	PROP_Weapon_Kickback (100)
	PROP_Obituary("$OB_MPFIST")

END_DEFAULTS

//
// A_Punch
//
void A_Punch (AActor *actor)
{
	angle_t 	angle;
	int 		damage;
	int 		pitch;

	// [BC] Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	if (actor->player != NULL)
	{
		AWeapon *weapon = actor->player->ReadyWeapon;
		if (weapon != NULL)
		{
			if (!weapon->DepleteAmmo (weapon->bAltFire))
				return;
		}
	}

	damage = (pr_punch()%10+1)<<1;

	if (actor->FindInventory<APowerStrength>())	
		damage *= 10;

	angle = actor->angle;

	angle += pr_punch.Random2() << 18;
	pitch = P_AimLineAttack (actor, angle, MELEERANGE);
	P_LineAttack (actor, angle, MELEERANGE, pitch, damage, MOD_UNKNOWN, RUNTIME_CLASS(ABulletPuff));

	// [BC] Apply spread.
	if (( actor->player ) && ( actor->player->Powers & PW_SPREAD ))
	{
		P_LineAttack( actor, angle + ( ANGLE_45 / 3 ), MELEERANGE, pitch, damage, MOD_UNKNOWN, RUNTIME_CLASS( ABulletPuff ));
		P_LineAttack( actor, angle - ( ANGLE_45 / 3 ), MELEERANGE, pitch, damage, MOD_UNKNOWN, RUNTIME_CLASS( ABulletPuff ));
	}

	// [BC] If the player hit a player with his attack, potentially give him a medal.
	if ( actor->player )
	{
		if ( actor->player->bStruckPlayer )
			PLAYER_StruckPlayer( actor->player );
		else
			actor->player->ulConsecutiveHits = 0;

		// Tell all the bots that a weapon was fired.
		BOTS_PostWeaponFiredEvent( ULONG( actor->player - players ), BOTEVENT_USEDFIST, BOTEVENT_ENEMY_USEDFIST, BOTEVENT_PLAYER_USEDFIST );
	}

	// turn to face target
	if (linetarget)
	{
		S_Sound (actor, CHAN_WEAPON, "*fist", 1, ATTN_NORM);
		actor->angle = R_PointToAngle2 (actor->x,
										actor->y,
										linetarget->x,
										linetarget->y);

		// [BC] Play the hit sound to clients.
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			SERVERCOMMANDS_SoundActor( actor, CHAN_WEAPON, "*fist", 127, ATTN_NORM );
	}
}

// Pistol -------------------------------------------------------------------

void A_FirePistol (AActor *);

class APistol : public AWeapon
{
	DECLARE_ACTOR (APistol, AWeapon)
};

FState APistol::States[] =
{
#define S_PISTOL 0
	S_NORMAL (PISG, 'A',	1, A_WeaponReady		, &States[S_PISTOL]),

#define S_PISTOLDOWN (S_PISTOL+1)
	S_NORMAL (PISG, 'A',	1, A_Lower				, &States[S_PISTOLDOWN]),

#define S_PISTOLUP (S_PISTOLDOWN+1)
	S_NORMAL (PISG, 'A',	1, A_Raise				, &States[S_PISTOLUP]),

#define S_PISTOL1 (S_PISTOLUP+1)
	S_NORMAL (PISG, 'A',	4, NULL 				, &States[S_PISTOL1+1]),
	S_NORMAL (PISG, 'B',	6, A_FirePistol 		, &States[S_PISTOL1+2]),
	S_NORMAL (PISG, 'C',	4, NULL 				, &States[S_PISTOL1+3]),
	S_NORMAL (PISG, 'B',	5, A_ReFire 			, &States[S_PISTOL]),

#define S_PISTOLFLASH (S_PISTOL1+4)
	S_BRIGHT (PISF, 'A',	7, A_Light1 			, &AWeapon::States[S_LIGHTDONE]),
// This next state is here just in case people want to shoot plasma balls or railguns
// with the pistol using Dehacked.
	S_BRIGHT (PISF, 'A',	7, A_Light1 			, &AWeapon::States[S_LIGHTDONE]),

// [BC] State for the pistol as a weapon picked up off the ground.
#define S_PIST (S_PISTOLFLASH+2)
	S_NORMAL (PIST, 'A',	-1, NULL 				, NULL),
};

IMPLEMENT_ACTOR (APistol, Doom, 5010, 162)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (16)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_PIST)

	PROP_Weapon_SelectionOrder (1900)
	// [BC] We can use this weapon while respawn invulnerability is active.
	PROP_Weapon_Flags (WIF_WIMPY_WEAPON|WIF_ALLOW_WITH_RESPAWN_INVUL)

	PROP_Weapon_AmmoUse1 (1)
	PROP_Weapon_AmmoGive1 (20)

	PROP_Weapon_UpState (S_PISTOLUP)
	PROP_Weapon_DownState (S_PISTOLDOWN)
	PROP_Weapon_ReadyState (S_PISTOL)
	PROP_Weapon_AtkState (S_PISTOL1)
	PROP_Weapon_HoldAtkState (S_PISTOL1)
	PROP_Weapon_FlashState (S_PISTOLFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_AmmoType1 ("Clip")
	PROP_Obituary("$OB_MPPISTOL")
	PROP_Inventory_PickupMessage("$PICKUP_PISTOL_DROPPED")
END_DEFAULTS

//
// A_FirePistol
//
void A_FirePistol (AActor *actor)
{
	bool accurate;

	if (actor->player != NULL)
	{
		AWeapon *weapon = actor->player->ReadyWeapon;
		if (weapon != NULL)
		{
			if (!weapon->DepleteAmmo (weapon->bAltFire))
				return;

			P_SetPsprite (actor->player, ps_flash, weapon->FlashState);
		}
		actor->player->mo->PlayAttacking2 ();

		// [BC] If we're the server, tell clients to update this player's state.
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			SERVERCOMMANDS_SetPlayerState( ULONG( actor->player - players ), STATE_PLAYER_ATTACK2, ULONG( actor->player - players ), SVCF_SKIPTHISCLIENT );

		accurate = !actor->player->refire;
	}
	else
	{
		accurate = true;
	}

	// [BC] If we're the server, tell clients that a weapon is being fired.
	if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( actor->player ))
		SERVERCOMMANDS_WeaponSound( ULONG( actor->player - players ), "weapons/pistol", ULONG( actor->player - players ), SVCF_SKIPTHISCLIENT );

	S_Sound (actor, CHAN_WEAPON, "weapons/pistol", 1, ATTN_NORM);

	// [BC] Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	P_BulletSlope (actor);
	P_GunShot (actor, accurate, RUNTIME_CLASS(ABulletPuff));

	// [BC] Apply spread.
	if (( actor->player ) && ( actor->player->Powers & PW_SPREAD ))
	{
		fixed_t		SavedActorAngle;

		SavedActorAngle = actor->angle;
		actor->angle += ( ANGLE_45 / 3 );
		P_GunShot( actor, accurate, RUNTIME_CLASS( ABulletPuff ));
		actor->angle = SavedActorAngle;

		SavedActorAngle = actor->angle;
		actor->angle -= ( ANGLE_45 / 3 );
		P_GunShot( actor, accurate, RUNTIME_CLASS( ABulletPuff ));
		actor->angle = SavedActorAngle;
	}

	// [BC] If the player hit a player with his attack, potentially give him a medal.
	if ( actor->player )
	{
		if ( actor->player->bStruckPlayer )
			PLAYER_StruckPlayer( actor->player );
		else
			actor->player->ulConsecutiveHits = 0;

		// Tell all the bots that a weapon was fired.
		BOTS_PostWeaponFiredEvent( ULONG( actor->player - players ), BOTEVENT_FIREDPISTOL, BOTEVENT_ENEMY_FIREDPISTOL, BOTEVENT_PLAYER_FIREDPISTOL );
	}
}

// Chainsaw -----------------------------------------------------------------

void A_Saw (AActor *);

class AChainsaw : public AWeapon
{
	DECLARE_ACTOR (AChainsaw, AWeapon)
};

FState AChainsaw::States[] =
{
#define S_SAW 0
	S_NORMAL (SAWG, 'C',	4, A_WeaponReady		, &States[S_SAW+1]),
	S_NORMAL (SAWG, 'D',	4, A_WeaponReady		, &States[S_SAW+0]),

#define S_SAWDOWN (S_SAW+2)
	S_NORMAL (SAWG, 'C',	1, A_Lower				, &States[S_SAWDOWN]),

#define S_SAWUP (S_SAWDOWN+1)
	S_NORMAL (SAWG, 'C',	1, A_Raise				, &States[S_SAWUP]),

#define S_SAW1 (S_SAWUP+1)
	S_NORMAL (SAWG, 'A',	4, A_Saw				, &States[S_SAW1+1]),
	S_NORMAL (SAWG, 'B',	4, A_Saw				, &States[S_SAW1+2]),
	S_NORMAL (SAWG, 'B',	0, A_ReFire 			, &States[S_SAW]),

#define S_CSAW (S_SAW1+3)
	S_NORMAL (CSAW, 'A',   -1, NULL 				, NULL)
};

IMPLEMENT_ACTOR (AChainsaw, Doom, 2005, 32)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (16)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_CSAW)

	PROP_Weapon_SelectionOrder (2200)
	PROP_Weapon_Flags (WIF_ALLOW_WITH_RESPAWN_INVUL)
	PROP_Weapon_UpState (S_SAWUP)
	PROP_Weapon_DownState (S_SAWDOWN)
	PROP_Weapon_ReadyState (S_SAW)
	PROP_Weapon_AtkState (S_SAW1)
	PROP_Weapon_HoldAtkState (S_SAW1)
	PROP_Weapon_UpSound ("weapons/sawup")
	PROP_Weapon_ReadySound ("weapons/sawidle")
	PROP_Obituary("$OB_MPCHAINSAW")
	PROP_Inventory_PickupMessage("$GOTCHAINSAW")
END_DEFAULTS

//
// A_Saw
//
void A_Saw (AActor *actor)
{
	angle_t 	angle;
	int 		damage=0;
	player_t *player;

	// [BC] Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;
	
	int fullsound;
	int hitsound;
	const PClass * pufftype = NULL;

	if (NULL == (player = actor->player))
	{
		return;
	}

	AWeapon *weapon = actor->player->ReadyWeapon;
	if (weapon != NULL)
	{
		if (!weapon->DepleteAmmo (weapon->bAltFire))
			return;
	}

	int index = CheckIndex (4, NULL);
	if (index >= 0) 
	{
		fullsound = StateParameters[index];
		hitsound = StateParameters[index+1];
		damage = EvalExpressionI (StateParameters[index+2], actor);
		pufftype = PClass::FindClass ((ENamedName)StateParameters[index+3]);
	}
	else
	{
		fullsound = S_FindSound("weapons/sawfull");
		hitsound = S_FindSound("weapons/sawhit");
	}
	if (pufftype == NULL) pufftype = RUNTIME_CLASS(ABulletPuff);
	if (damage == 0) damage = 2;
	
	damage *= (pr_saw()%10+1);
	angle = actor->angle;
	angle += pr_saw.Random2() << 18;
	
	// use meleerange + 1 so the puff doesn't skip the flash (i.e. plays all states)
	P_LineAttack (actor, angle, MELEERANGE+1,
				  P_AimLineAttack (actor, angle, MELEERANGE+1), damage,
				  MOD_UNKNOWN, pufftype);

	// [BC] Apply spread.
	if ( player->Powers & PW_SPREAD )
	{
		P_LineAttack( actor, angle + ( ANGLE_45 / 3 ), MELEERANGE + 1,
					  P_AimLineAttack( actor, angle + ( ANGLE_45 / 3 ), MELEERANGE + 1 ), damage,
					  MOD_UNKNOWN, pufftype );

		P_LineAttack( actor, angle - ( ANGLE_45 / 3 ), MELEERANGE + 1,
					  P_AimLineAttack( actor, angle - ( ANGLE_45 / 3 ), MELEERANGE + 1 ), damage,
					  MOD_UNKNOWN, pufftype );
	}

	// [BC] If the player hit a player with his attack, potentially give him a medal.
	if ( actor->player->bStruckPlayer )
		PLAYER_StruckPlayer( actor->player );
	else
		actor->player->ulConsecutiveHits = 0;

	// [BC] If we're the server, tell clients that a weapon is being fired.
//	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
//		SERVERCOMMANDS_WeaponSound( ULONG( player - players ), linetarget ? hitsound : fullsound, ULONG( player - players ), SVCF_SKIPTHISCLIENT );

	if (!linetarget)
	{
		S_SoundID (actor, CHAN_WEAPON, fullsound, 1, ATTN_NORM);

		// [BC] If we're the server, tell clients to play the saw sound.
		if ( NETWORK_GetState( ) == NETSTATE_SERVER )
			SERVERCOMMANDS_SoundIDActor( actor, CHAN_WEAPON, fullsound, 127, ATTN_NORM );
		return;
	}
	S_SoundID (actor, CHAN_WEAPON, hitsound, 1, ATTN_NORM);
	// [BC] If we're the server, tell clients to play the saw sound.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_SoundIDActor( actor, CHAN_WEAPON, hitsound, 127, ATTN_NORM );
		
	// turn to face target
	angle = R_PointToAngle2 (actor->x, actor->y,
							 linetarget->x, linetarget->y);
	if (angle - actor->angle > ANG180)
	{
		if (angle - actor->angle < (angle_t)(-ANG90/20))
			actor->angle = angle + ANG90/21;
		else
			actor->angle -= ANG90/20;
	}
	else
	{
		if (angle - actor->angle > ANG90/20)
			actor->angle = angle - ANG90/21;
		else
			actor->angle += ANG90/20;
	}
	actor->flags |= MF_JUSTATTACKED;

	// [BC] If we're the server, tell clients to adjust the player's angle.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_SetThingAngle( actor );

	// [BC] Tell all the bots that a weapon was fired.
	BOTS_PostWeaponFiredEvent( ULONG( actor->player - players ), BOTEVENT_USEDCHAINSAW, BOTEVENT_ENEMY_USEDCHAINSAW, BOTEVENT_PLAYER_USEDCHAINSAW );
}

// Shotgun ------------------------------------------------------------------

void A_FireShotgun (AActor *);

class AShotgun : public AWeapon
{
	DECLARE_ACTOR (AShotgun, AWeapon)
};

FState AShotgun::States[] =
{
#define S_SGUN 0
	S_NORMAL (SHTG, 'A',	1, A_WeaponReady		, &States[S_SGUN]),

#define S_SGUNDOWN (S_SGUN+1)
	S_NORMAL (SHTG, 'A',	1, A_Lower				, &States[S_SGUNDOWN]),

#define S_SGUNUP (S_SGUNDOWN+1)
	S_NORMAL (SHTG, 'A',	1, A_Raise				, &States[S_SGUNUP]),

#define S_SGUN1 (S_SGUNUP+1)
	S_NORMAL (SHTG, 'A',	3, NULL 				, &States[S_SGUN1+1]),
	S_NORMAL (SHTG, 'A',	7, A_FireShotgun		, &States[S_SGUN1+2]),
	S_NORMAL (SHTG, 'B',	5, NULL 				, &States[S_SGUN1+3]),
	S_NORMAL (SHTG, 'C',	5, NULL 				, &States[S_SGUN1+4]),
	S_NORMAL (SHTG, 'D',	4, NULL 				, &States[S_SGUN1+5]),
	S_NORMAL (SHTG, 'C',	5, NULL 				, &States[S_SGUN1+6]),
	S_NORMAL (SHTG, 'B',	5, NULL 				, &States[S_SGUN1+7]),
	S_NORMAL (SHTG, 'A',	3, NULL 				, &States[S_SGUN1+8]),
	S_NORMAL (SHTG, 'A',	7, A_ReFire 			, &States[S_SGUN]),

#define S_SGUNFLASH (S_SGUN1+9)
	S_BRIGHT (SHTF, 'A',	4, A_Light1 			, &States[S_SGUNFLASH+1]),
	S_BRIGHT (SHTF, 'B',	3, A_Light2 			, &AWeapon::States[S_LIGHTDONE]),

#define S_SHOT (S_SGUNFLASH+2)
	S_NORMAL (SHOT, 'A',   -1, NULL 				, NULL)
};

IMPLEMENT_ACTOR (AShotgun, Doom, 2001, 27)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (16)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_SHOT)

	PROP_Weapon_SelectionOrder (1300)
	PROP_Weapon_AmmoUse1 (1)
	PROP_Weapon_AmmoGive1 (8)
	PROP_Weapon_UpState (S_SGUNUP)
	PROP_Weapon_DownState (S_SGUNDOWN)
	PROP_Weapon_ReadyState (S_SGUN)
	PROP_Weapon_AtkState (S_SGUN1)
	PROP_Weapon_HoldAtkState (S_SGUN1)
	PROP_Weapon_FlashState (S_SGUNFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_AmmoType1 ("Shell")
	PROP_Obituary("$OB_MPSHOTGUN")
	PROP_Inventory_PickupMessage("$GOTSHOTGUN")
END_DEFAULTS

//
// A_FireShotgun
//
void A_FireShotgun (AActor *actor)
{
	int i;
	player_t *player;

	if (NULL == (player = actor->player))
	{
		return;
	}

	// [BC] If we're the server, tell clients that a weapon is being fired.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_WeaponSound( ULONG( player - players ), "weapons/shotgf", ULONG( player - players ), SVCF_SKIPTHISCLIENT );

	S_Sound (actor, CHAN_WEAPON,  "weapons/shotgf", 1, ATTN_NORM);
	AWeapon *weapon = actor->player->ReadyWeapon;
	if (weapon != NULL)
	{
		if (!weapon->DepleteAmmo (weapon->bAltFire))
			return;
		P_SetPsprite (player, ps_flash, weapon->FlashState);
	}
	player->mo->PlayAttacking2 ();

	// [BC] If we're the server, tell clients to update this player's state.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_SetPlayerState( ULONG( player - players ), STATE_PLAYER_ATTACK2, ULONG( player - players ), SVCF_SKIPTHISCLIENT );

	// [BC] Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	P_BulletSlope (actor);

	for (i=0 ; i<7 ; i++)
		P_GunShot (actor, false, RUNTIME_CLASS(ABulletPuff));

	// [BC] Apply spread.
	if ( player->Powers & PW_SPREAD )
	{
		fixed_t		SavedActorAngle;

		SavedActorAngle = actor->angle;
		actor->angle += ( ANGLE_45 / 3 );
		for (i=0 ; i<7 ; i++)
			P_GunShot( actor, false, RUNTIME_CLASS( ABulletPuff ));
		actor->angle = SavedActorAngle;

		SavedActorAngle = actor->angle;
		actor->angle -= ( ANGLE_45 / 3 );
		for (i=0 ; i<7 ; i++)
			P_GunShot( actor, false, RUNTIME_CLASS( ABulletPuff ));
		actor->angle = SavedActorAngle;
	}

	// [BC] If the player hit a player with his attack, potentially give him a medal.
	if ( player->bStruckPlayer )
		PLAYER_StruckPlayer( player );
	else
		player->ulConsecutiveHits = 0;

	// [BC] Tell all the bots that a weapon was fired.
	BOTS_PostWeaponFiredEvent( ULONG( player - players ), BOTEVENT_FIREDSHOTGUN, BOTEVENT_ENEMY_FIREDSHOTGUN, BOTEVENT_PLAYER_FIREDSHOTGUN );
}

// Super Shotgun ------------------------------------------------------------

void A_FireShotgun2 (AActor *actor);
void A_OpenShotgun2 (AActor *actor);
void A_LoadShotgun2 (AActor *actor);
void A_CloseShotgun2 (AActor *actor);

class ASuperShotgun : public AWeapon
{
	DECLARE_ACTOR (ASuperShotgun, AWeapon)
};

FState ASuperShotgun::States[] =
{
#define S_DSGUN 0
	S_NORMAL (SHT2, 'A',	1, A_WeaponReady		, &States[S_DSGUN]),

#define S_DSGUNDOWN (S_DSGUN+1)
	S_NORMAL (SHT2, 'A',	1, A_Lower				, &States[S_DSGUNDOWN]),

#define S_DSGUNUP (S_DSGUNDOWN+1)
	S_NORMAL (SHT2, 'A',	1, A_Raise				, &States[S_DSGUNUP]),

#define S_DSGUN1 (S_DSGUNUP+1)
	S_NORMAL (SHT2, 'A',	3, NULL 				, &States[S_DSGUN1+1]),
	S_NORMAL (SHT2, 'A',	7, A_FireShotgun2		, &States[S_DSGUN1+2]),
	S_NORMAL (SHT2, 'B',	7, NULL 				, &States[S_DSGUN1+3]),
	S_NORMAL (SHT2, 'C',	7, A_CheckReload		, &States[S_DSGUN1+4]),
	S_NORMAL (SHT2, 'D',	7, A_OpenShotgun2		, &States[S_DSGUN1+5]),
	S_NORMAL (SHT2, 'E',	7, NULL 				, &States[S_DSGUN1+6]),
	S_NORMAL (SHT2, 'F',	7, A_LoadShotgun2		, &States[S_DSGUN1+7]),
	S_NORMAL (SHT2, 'G',	6, NULL 				, &States[S_DSGUN1+8]),
	S_NORMAL (SHT2, 'H',	6, A_CloseShotgun2		, &States[S_DSGUN1+9]),
	S_NORMAL (SHT2, 'A',	5, A_ReFire 			, &States[S_DSGUN]),

#define S_DSNR (S_DSGUN1+10)
	S_NORMAL (SHT2, 'B',	7, NULL 				, &States[S_DSNR+1]),
	S_NORMAL (SHT2, 'A',	3, NULL 				, &States[S_DSGUNDOWN]),

#define S_DSGUNFLASH (S_DSNR+2)
	S_BRIGHT (SHT2, 'I',	4, A_Light1 			, &States[S_DSGUNFLASH+1]),
	S_BRIGHT (SHT2, 'J',	3, A_Light2 			, &AWeapon::States[S_LIGHTDONE]),

#define S_SHOT2 (S_DSGUNFLASH+2)
	S_NORMAL (SGN2, 'A',   -1, NULL 				, NULL)
};

IMPLEMENT_ACTOR (ASuperShotgun, Doom, 82, 33)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (16)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_SHOT2)

	PROP_Weapon_SelectionOrder (400)
	PROP_Weapon_AmmoUse1 (2)
	PROP_Weapon_AmmoGive1 (8)
	PROP_Weapon_UpState (S_DSGUNUP)
	PROP_Weapon_DownState (S_DSGUNDOWN)
	PROP_Weapon_ReadyState (S_DSGUN)
	PROP_Weapon_AtkState (S_DSGUN1)
	PROP_Weapon_HoldAtkState (S_DSGUN1)
	PROP_Weapon_FlashState (S_DSGUNFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_AmmoType1 ("Shell")
	PROP_Obituary("$OB_MPSSHOTGUN")
	PROP_Inventory_PickupMessage("$GOTSHOTGUN2")
END_DEFAULTS

//
// A_FireShotgun2
//
void A_FireShotgun2 (AActor *actor)
{
	int 		i;
	angle_t 	angle;
	int 		damage;
	player_t *player;

	if (NULL == (player = actor->player))
	{
		return;
	}

	// [BC] If we're the server, tell clients that a weapon is being fired.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_WeaponSound( ULONG( player - players ), "weapons/sshotf", ULONG( player - players ), SVCF_SKIPTHISCLIENT );

	S_Sound (actor, CHAN_WEAPON, "weapons/sshotf", 1, ATTN_NORM);
	AWeapon *weapon = actor->player->ReadyWeapon;
	if (weapon != NULL)
	{
		if (!weapon->DepleteAmmo (weapon->bAltFire))
			return;
		P_SetPsprite (player, ps_flash, weapon->FlashState);
	}
	player->mo->PlayAttacking2 ();

	// [BC] If we're the server, tell clients to update this player's state.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_SetPlayerState( ULONG( player - players ), STATE_PLAYER_ATTACK2, ULONG( player - players ), SVCF_SKIPTHISCLIENT );

	// [BC] Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	P_BulletSlope (actor);
		
	for (i=0 ; i<20 ; i++)
	{
		damage = 5*(pr_fireshotgun2()%3+1);
		angle = actor->angle;
		angle += pr_fireshotgun2.Random2() << 19;

		// Doom adjusts the bullet slope by shifting a random number [-255,255]
		// left 5 places. At 2048 units away, this means the vertical position
		// of the shot can deviate as much as 255 units from nominal. So using
		// some simple trigonometry, that means the vertical angle of the shot
		// can deviate by as many as ~7.097 degrees or ~84676099 BAMs.

		P_LineAttack (actor,
					  angle,
					  PLAYERMISSILERANGE,
					  bulletpitch + (pr_fireshotgun2.Random2() * 332063), damage,
					  MOD_UNKNOWN, RUNTIME_CLASS(ABulletPuff));

		// [BC] Apply spread.
		if ( player->Powers & PW_SPREAD )
		{
			P_LineAttack (actor,
						  angle + ( ANGLE_45 / 3 ),
						  PLAYERMISSILERANGE,
						  bulletpitch + (pr_fireshotgun2.Random2() * 332063), damage,
						  MOD_UNKNOWN, RUNTIME_CLASS(ABulletPuff));

			P_LineAttack (actor,
						  angle - ( ANGLE_45 / 3 ),
						  PLAYERMISSILERANGE,
						  bulletpitch + (pr_fireshotgun2.Random2() * 332063), damage,
						  MOD_UNKNOWN, RUNTIME_CLASS(ABulletPuff));
		}
	}

	// [BC] If the player hit a player with his attack, potentially give him a medal.
	if ( player->bStruckPlayer )
		PLAYER_StruckPlayer( player );
	else
		player->ulConsecutiveHits = 0;

	// [BC] Tell all the bots that a weapon was fired.
	BOTS_PostWeaponFiredEvent( ULONG( player - players ), BOTEVENT_FIREDSSG, BOTEVENT_ENEMY_FIREDSSG, BOTEVENT_PLAYER_FIREDSSG );
}

void A_OpenShotgun2 (AActor *actor)
{
	S_Sound (actor, CHAN_WEAPON, "weapons/sshoto", 1, ATTN_NORM);

	// [BC] If we're the server, tell clients that a weapon is being fired.
	if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( actor->player ))
		SERVERCOMMANDS_WeaponSound( ULONG( actor->player - players ), "weapons/sshoto", ULONG( actor->player - players ), SVCF_SKIPTHISCLIENT );
}

void A_LoadShotgun2 (AActor *actor)
{
	S_Sound (actor, CHAN_WEAPON, "weapons/sshotl", 1, ATTN_NORM);

	// [BC] If we're the server, tell clients that a weapon is being fired.
	if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( actor->player ))
		SERVERCOMMANDS_WeaponSound( ULONG( actor->player - players ), "weapons/sshot1", ULONG( actor->player - players ), SVCF_SKIPTHISCLIENT );
}

void A_CloseShotgun2 (AActor *actor)
{
	S_Sound (actor, CHAN_WEAPON, "weapons/sshotc", 1, ATTN_NORM);
	A_ReFire (actor);

	// [BC] If we're the server, tell clients that a weapon is being fired.
	if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( actor->player ))
		SERVERCOMMANDS_WeaponSound( ULONG( actor->player - players ), "weapons/sshotc", ULONG( actor->player - players ), SVCF_SKIPTHISCLIENT );
}

// Chaingun -----------------------------------------------------------------

void A_FireCGun (AActor *);

class AChaingun : public AWeapon
{
	DECLARE_ACTOR (AChaingun, AWeapon)
};

FState AChaingun::States[] =
{
#define S_CHAIN 0
	S_NORMAL (CHGG, 'A',	1, A_WeaponReady		, &States[S_CHAIN]),

#define S_CHAINDOWN (S_CHAIN+1)
	S_NORMAL (CHGG, 'A',	1, A_Lower				, &States[S_CHAINDOWN]),

#define S_CHAINUP (S_CHAINDOWN+1)
	S_NORMAL (CHGG, 'A',	1, A_Raise				, &States[S_CHAINUP]),

#define S_CHAIN1 (S_CHAINUP+1)
	S_NORMAL (CHGG, 'A',	4, A_FireCGun			, &States[S_CHAIN1+1]),
	S_NORMAL (CHGG, 'B',	4, A_FireCGun			, &States[S_CHAIN1+2]),
	S_NORMAL (CHGG, 'B',	0, A_ReFire 			, &States[S_CHAIN]),

#define S_CHAINFLASH (S_CHAIN1+3)
	S_BRIGHT (CHGF, 'A',	5, A_Light1 			, &AWeapon::States[S_LIGHTDONE]),
	S_BRIGHT (CHGF, 'B',	5, A_Light2 			, &AWeapon::States[S_LIGHTDONE]),

#define S_MGUN (S_CHAINFLASH+2)
	S_NORMAL (MGUN, 'A',   -1, NULL 				, NULL)
};

IMPLEMENT_ACTOR (AChaingun, Doom, 2002, 28)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (16)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_MGUN)

	PROP_Weapon_SelectionOrder (700)
	PROP_Weapon_AmmoUse1 (1)
	PROP_Weapon_AmmoGive1 (20)
	PROP_Weapon_UpState (S_CHAINUP)
	PROP_Weapon_DownState (S_CHAINDOWN)
	PROP_Weapon_ReadyState (S_CHAIN)
	PROP_Weapon_AtkState (S_CHAIN1)
	PROP_Weapon_HoldAtkState (S_CHAIN1)
	PROP_Weapon_FlashState (S_CHAINFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_AmmoType1 ("Clip")
	PROP_Obituary("$OB_MPCHAINGUN")
	PROP_Inventory_PickupMessage("$GOTCHAINGUN")
END_DEFAULTS

//
// A_FireCGun
//
void A_FireCGun (AActor *actor)
{
	player_t *player;

	if (actor == NULL || NULL == (player = actor->player))
	{
		return;
	}
	S_Sound (actor, CHAN_WEAPON, "weapons/chngun", 1, ATTN_NORM);

	// [BC] If we're the server, tell clients that a weapon is being fired.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_WeaponSound( ULONG( player - players ), "weapons/chngun", ULONG( player - players ), SVCF_SKIPTHISCLIENT );

	AWeapon *weapon = player->ReadyWeapon;
	if (weapon != NULL)
	{
		if (!weapon->DepleteAmmo (weapon->bAltFire))
			return;
		if (weapon->FlashState != NULL)
		{
			// [RH] Fix for Sparky's messed-up Dehacked patch! Blargh!
			int theflash = clamp (int(players->psprites[ps_weapon].state - weapon->AtkState), 0, 1);

			if (weapon->FlashState[theflash].sprite.index != weapon->FlashState->sprite.index)
			{
				theflash = 0;
			}

			P_SetPsprite (player, ps_flash, weapon->FlashState + theflash);
		}

	}
	player->mo->PlayAttacking2 ();

	// [BC] If we're the server, tell clients to update this player's state.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_SetPlayerState( ULONG( player - players ), STATE_PLAYER_ATTACK2, ULONG( player - players ), SVCF_SKIPTHISCLIENT );

	// [BC] Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	P_BulletSlope (actor);
	P_GunShot (actor, !player->refire, RUNTIME_CLASS(ABulletPuff));

	// [BC] Apply apread.
	if ( player->Powers & PW_SPREAD )
	{
		fixed_t		SavedActorAngle;

		SavedActorAngle = actor->angle;
		actor->angle += ( ANGLE_45 / 3 );
		P_GunShot( actor, !player->refire, RUNTIME_CLASS( ABulletPuff ));
		actor->angle = SavedActorAngle;
	
		SavedActorAngle = actor->angle;
		actor->angle -= ( ANGLE_45 / 3 );
		P_GunShot( actor, !player->refire, RUNTIME_CLASS( ABulletPuff ));
		actor->angle = SavedActorAngle;
	}

	// [BC] If the player hit a player with his attack, potentially give him a medal.
	if ( player->bStruckPlayer )
		PLAYER_StruckPlayer( player );
	else
		player->ulConsecutiveHits = 0;

	// [BC] Tell all the bots that a weapon was fired.
	BOTS_PostWeaponFiredEvent( ULONG( player - players ), BOTEVENT_FIREDCHAINGUN, BOTEVENT_ENEMY_FIREDCHAINGUN, BOTEVENT_PLAYER_FIREDCHAINGUN );
}

// Minigun -----------------------------------------------------------------

void A_FireMiniGun (AActor *);

class AMinigun : public AWeapon
{
	DECLARE_ACTOR (AMinigun, AWeapon)
};

FState AMinigun::States[] =
{
#define S_MINI 0
	S_NORMAL (MNGG, 'A',	1, A_WeaponReady		, &States[S_MINI]),

#define S_MINIDOWN (S_MINI+1)
	S_NORMAL (MNGG, 'A',	1, A_Lower				, &States[S_MINIDOWN]),

#define S_MINIUP (S_MINIDOWN+1)
	S_NORMAL (MNGG, 'A',	1, A_Raise				, &States[S_MINIUP]),

#define S_MINI1 (S_MINIUP+1)
	S_NORMAL (MNGG, 'A',	2, A_FireMiniGun		, &States[S_MINI1+1]),
	S_NORMAL (MNGG, 'B',	2, A_FireMiniGun		, &States[S_MINI1+2]),
	S_NORMAL (MNGG, 'A',	2, A_ReFire 			, &States[S_MINI1+3]),
	S_NORMAL (MNGG, 'B',	2, NULL		 			, &States[S_MINI1+4]),
	S_NORMAL (MNGG, 'A',	4, NULL		 			, &States[S_MINI1+5]),
	S_NORMAL (MNGG, 'B',	4, NULL		 			, &States[S_MINI1+6]),
	S_NORMAL (MNGG, 'A',	8, NULL					, &States[S_MINI1+7]),
	S_NORMAL (MNGG, 'B',	8, NULL		 			, &States[S_MINI]),

#define S_MINIFLASH (S_MINI1+8)
	S_BRIGHT (MNGF, 'A',	5, A_Light1 			, &AWeapon::States[S_LIGHTDONE]),
	S_BRIGHT (MNGF, 'B',	5, A_Light2 			, &AWeapon::States[S_LIGHTDONE]),

#define S_MINIGUN (S_MINIFLASH+2)
	S_NORMAL (MNGN, 'A',   -1, NULL 				, NULL)
};

IMPLEMENT_ACTOR (AMinigun, Doom, 5014, 214)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (16)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_MINIGUN)

	PROP_Weapon_SelectionOrder (700)
	PROP_Weapon_AmmoUse1 (1)
	PROP_Weapon_AmmoGive1 (20)
	PROP_Weapon_UpState (S_MINIUP)
	PROP_Weapon_DownState (S_MINIDOWN)
	PROP_Weapon_ReadyState (S_MINI)
	PROP_Weapon_AtkState (S_MINI1)
	PROP_Weapon_HoldAtkState (S_MINI1)
	PROP_Weapon_FlashState (S_MINIFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_AmmoType1 ("Clip")
	PROP_Obituary("$OB_MINIGUN")
	PROP_Inventory_PickupMessage("$PICKUP_MINIGUN")
END_DEFAULTS

//
// A_FireMiniGun
//
void A_FireMiniGun( AActor *actor )
{
	player_t	*pPlayer;
	AWeapon		*pWeapon;

	if (( actor == NULL ) || ( actor->player == NULL ))
		return;

	pPlayer = actor->player;

	// Play the weapon sound effect.
	S_Sound( actor, CHAN_WEAPON, "weapons/minigun", 1, ATTN_NORM );

	// [BC] If we're the server, tell clients that a weapon is being fired.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_WeaponSound( ULONG( pPlayer - players ), "weapons/minigun", ULONG( pPlayer - players ), SVCF_SKIPTHISCLIENT );

	pWeapon = pPlayer->ReadyWeapon;
	if ( pWeapon != NULL )
	{
		// Break out if we don't have enough ammo to fire the weapon. This shouldn't ever happen
		// because the function that puts the weapon into its fire state first checks to see if
		// the weapon has enough ammo to fire.
		if ( pWeapon->DepleteAmmo( pWeapon->bAltFire ) == false )
			return;

		if ( pWeapon->FlashState != NULL )
			P_SetPsprite( pPlayer, ps_flash, pWeapon->FlashState );

	}

	pPlayer->mo->PlayAttacking2( );

	// If we're the server, tell clients to update this player's state.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
		SERVERCOMMANDS_SetPlayerState( ULONG( pPlayer - players ), STATE_PLAYER_ATTACK2, ULONG( pPlayer - players ), SVCF_SKIPTHISCLIENT );

	// Weapon firing is server side.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	P_BulletSlope( actor );

	// Minigun is always inaccurate.
	P_GunShot( actor, false, RUNTIME_CLASS( ABulletPuff ));

	// Apply spread.
	if ( pPlayer->Powers & PW_SPREAD )
	{
		fixed_t		SavedActorAngle;

		SavedActorAngle = actor->angle;

		actor->angle += ( ANGLE_45 / 3 );
		P_GunShot( actor, false, RUNTIME_CLASS( ABulletPuff ));
		actor->angle = SavedActorAngle;

		actor->angle -= ( ANGLE_45 / 3 );
		P_GunShot( actor, false, RUNTIME_CLASS( ABulletPuff ));
		actor->angle = SavedActorAngle;
	}

	// If the player hit a player with his attack, potentially give him a medal.
	if ( pPlayer->bStruckPlayer )
		PLAYER_StruckPlayer( pPlayer );
	else
		pPlayer->ulConsecutiveHits = 0;

	// Tell all the bots that a weapon was fired.
	BOTS_PostWeaponFiredEvent( ULONG( pPlayer - players ), BOTEVENT_FIREDMINIGUN, BOTEVENT_ENEMY_FIREDMINIGUN, BOTEVENT_PLAYER_FIREDMINIGUN );
}

// Rocket launcher ---------------------------------------------------------

void A_FireMissile (AActor *);
void A_Explode (AActor *);

class ARocketLauncher : public AWeapon
{
	DECLARE_ACTOR (ARocketLauncher, AWeapon)
};

FState ARocketLauncher::States[] =
{
#define S_MISSILE 0
	S_NORMAL (MISG, 'A',	1, A_WeaponReady		, &States[S_MISSILE]),

#define S_MISSILEDOWN (S_MISSILE+1)
	S_NORMAL (MISG, 'A',	1, A_Lower				, &States[S_MISSILEDOWN]),

#define S_MISSILEUP (S_MISSILEDOWN+1)
	S_NORMAL (MISG, 'A',	1, A_Raise				, &States[S_MISSILEUP]),

#define S_MISSILE1 (S_MISSILEUP+1)
	S_NORMAL (MISG, 'B',	8, A_GunFlash			, &States[S_MISSILE1+1]),
	S_NORMAL (MISG, 'B',   12, A_FireMissile		, &States[S_MISSILE1+2]),
	S_NORMAL (MISG, 'B',	0, A_ReFire 			, &States[S_MISSILE]),

#define S_MISSILEFLASH (S_MISSILE1+3)
	S_BRIGHT (MISF, 'A',	3, A_Light1 			, &States[S_MISSILEFLASH+1]),
	S_BRIGHT (MISF, 'B',	4, NULL 				, &States[S_MISSILEFLASH+2]),
	S_BRIGHT (MISF, 'C',	4, A_Light2 			, &States[S_MISSILEFLASH+3]),
	S_BRIGHT (MISF, 'D',	4, A_Light2 			, &AWeapon::States[S_LIGHTDONE]),

#define S_LAUN (S_MISSILEFLASH+4)
	S_NORMAL (LAUN, 'A',   -1, NULL 				, NULL)
};

IMPLEMENT_ACTOR (ARocketLauncher, Doom, 2003, 29)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (16)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_LAUN)

	PROP_Weapon_SelectionOrder (2500)
	PROP_Weapon_Flags (WIF_NOAUTOFIRE)
	PROP_Weapon_AmmoUse1 (1)
	PROP_Weapon_AmmoGive1 (2)
	PROP_Weapon_UpState (S_MISSILEUP)
	PROP_Weapon_DownState (S_MISSILEDOWN)
	PROP_Weapon_ReadyState (S_MISSILE)
	PROP_Weapon_AtkState (S_MISSILE1)
	PROP_Weapon_HoldAtkState (S_MISSILE1)
	PROP_Weapon_FlashState (S_MISSILEFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_AmmoType1 ("RocketAmmo")
	PROP_Weapon_ProjectileType ("Rocket")
	PROP_Inventory_PickupMessage("$GOTLAUNCHER")
END_DEFAULTS

FState ARocket::States[] =
{
#define S_ROCKET 0
	S_BRIGHT (MISL, 'A',	1, NULL 						, &States[S_ROCKET]),

#define S_EXPLODE (S_ROCKET+1)
	S_BRIGHT (MISL, 'B',	8, A_Explode					, &States[S_EXPLODE+1]),
	S_BRIGHT (MISL, 'C',	6, NULL 						, &States[S_EXPLODE+2]),
	S_BRIGHT (MISL, 'D',	4, NULL 						, NULL)
};

IMPLEMENT_ACTOR (ARocket, Doom, -1, 127)
	PROP_RadiusFixed (11)
	PROP_HeightFixed (8)
	PROP_SpeedFixed (20)
	PROP_Damage (20)
	PROP_Flags (MF_NOBLOCKMAP|MF_MISSILE|MF_DROPOFF|MF_NOGRAVITY)
	PROP_Flags2 (MF2_PCROSS|MF2_IMPACT|MF2_NOTELEPORT)
	PROP_Flags4 (MF4_RANDOMIZE)
	PROP_Flags5 (MF5_DEHEXPLOSION)
	PROP_FXFlags (FX_ROCKET)
	PROP_FlagsST( STFL_EXPLODEONDEATH )

	PROP_SpawnState (S_ROCKET)
	PROP_DeathState (S_EXPLODE)

	// [BC] Set the damage type so we can check for it when player's have red armor.
	PROP_DamageType( MOD_ROCKET )

	PROP_SeeSound ("weapons/rocklf")
	PROP_DeathSound ("weapons/rocklx")
	PROP_Obituary("$OB_MPROCKET")
END_DEFAULTS

//
// A_FireMissile
//
void A_FireMissile (AActor *actor)
{
	player_t *player;

	if (NULL == (player = actor->player))
	{
		return;
	}
	AWeapon *weapon = actor->player->ReadyWeapon;
	if (weapon != NULL)
	{
		if (!weapon->DepleteAmmo (weapon->bAltFire))
			return;
	}

	// [BC] Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	P_SpawnPlayerMissile (actor, RUNTIME_CLASS(ARocket));

	// [BC] Apply spread.
	if ( player->Powers & PW_SPREAD )
	{
		P_SpawnPlayerMissile( actor, RUNTIME_CLASS( ARocket ), actor->angle + ( ANGLE_45 / 3 ), false );
		P_SpawnPlayerMissile( actor, RUNTIME_CLASS( ARocket ), actor->angle - ( ANGLE_45 / 3 ), false );
	}

	// [BC] Tell all the bots that a weapon was fired.
	BOTS_PostWeaponFiredEvent( ULONG( player - players ), BOTEVENT_FIREDROCKET, BOTEVENT_ENEMY_FIREDROCKET, BOTEVENT_PLAYER_FIREDROCKET );
}

// Grenade launcher ---------------------------------------------------------

void A_FireSTGrenade (AActor *);
void A_Explode (AActor *);

class AGrenadeLauncher : public AWeapon
{
	DECLARE_ACTOR (AGrenadeLauncher, AWeapon)
};

FState AGrenadeLauncher::States[] =
{
#define S_GRENADELAUNCHER 0
	S_NORMAL (GRLG, 'A',	1, A_WeaponReady		, &States[S_GRENADELAUNCHER]),

#define S_GRENADELAUNCHERDOWN (S_GRENADELAUNCHER+1)
	S_NORMAL (GRLG, 'A',	1, A_Lower				, &States[S_GRENADELAUNCHERDOWN]),

#define S_GRENADELAUNCHERUP (S_GRENADELAUNCHERDOWN+1)
	S_NORMAL (GRLG, 'A',	1, A_Raise				, &States[S_GRENADELAUNCHERUP]),

#define S_GRENADELAUNCHER1 (S_GRENADELAUNCHERUP+1)
	S_NORMAL (GRLG, 'B',	8, A_GunFlash			, &States[S_GRENADELAUNCHER1+1]),
	S_NORMAL (GRLG, 'B',   12, A_FireSTGrenade		, &States[S_GRENADELAUNCHER1+2]),
	S_NORMAL (GRLG, 'B',	0, A_ReFire 			, &States[S_GRENADELAUNCHER]),

#define S_GRENADELAUNCHERFLASH (S_GRENADELAUNCHER1+3)
	S_BRIGHT (GRLF, 'A',	3, A_Light1 			, &States[S_GRENADELAUNCHERFLASH+1]),
	S_BRIGHT (GRLF, 'B',	4, NULL 				, &States[S_GRENADELAUNCHERFLASH+2]),
	S_BRIGHT (GRLF, 'C',	4, A_Light2 			, &States[S_GRENADELAUNCHERFLASH+3]),
	S_BRIGHT (GRLF, 'D',	4, A_Light2 			, &AWeapon::States[S_LIGHTDONE]),

#define S_GLAU (S_GRENADELAUNCHERFLASH+4)
	S_NORMAL (GLAU, 'A',   -1, NULL 				, NULL)
};

IMPLEMENT_ACTOR (AGrenadeLauncher, Doom, 5011, 163)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (16)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_GLAU)

	PROP_Weapon_SelectionOrder (2500)
	PROP_Weapon_Flags (WIF_NOAUTOFIRE)
	PROP_Weapon_AmmoUse1 (1)
	PROP_Weapon_AmmoGive1 (2)
	PROP_Weapon_UpState (S_GRENADELAUNCHERUP)
	PROP_Weapon_DownState (S_GRENADELAUNCHERDOWN)
	PROP_Weapon_ReadyState (S_GRENADELAUNCHER)
	PROP_Weapon_AtkState (S_GRENADELAUNCHER1)
	PROP_Weapon_HoldAtkState (S_GRENADELAUNCHER1)
	PROP_Weapon_FlashState (S_GRENADELAUNCHERFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_AmmoType1 ("RocketAmmo")
	PROP_Weapon_ProjectileType ("Rocket")
	PROP_Inventory_PickupMessage("$PICKUP_GRENADELAUNCHER")
END_DEFAULTS

FState AGrenade::States[] =
{
#define S_GRENADE 0
	S_BRIGHT (GREN, 'A',	1, NULL 						, &States[S_GRENADE]),

#define S_GRENADE_EXPLODE (S_GRENADE+1)
	S_BRIGHT (MISL, 'B',	8, A_Explode					, &States[S_GRENADE_EXPLODE+1]),
	S_BRIGHT (MISL, 'C',	6, NULL 						, &States[S_GRENADE_EXPLODE+2]),
	S_BRIGHT (MISL, 'D',	4, NULL 						, NULL)
};

IMPLEMENT_ACTOR (AGrenade, Doom, -1, 216)
	PROP_RadiusFixed (8)
	PROP_HeightFixed (8)
	PROP_SpeedFixed (25)
	PROP_Damage (20)
	PROP_Flags (MF_NOBLOCKMAP|MF_MISSILE|MF_DROPOFF)
	PROP_Flags2 (MF2_PCROSS|MF2_IMPACT|MF2_DOOMBOUNCE|MF2_NOTELEPORT)
	PROP_Flags4 (MF4_RANDOMIZE)
	PROP_Flags5 (MF5_DEHEXPLOSION)
	PROP_FXFlags (FX_GRENADE)
	PROP_FlagsST( STFL_QUARTERGRAVITY|STFL_EXPLODEONDEATH )

	PROP_SpawnState (S_GRENADE)
	PROP_DeathState (S_GRENADE_EXPLODE)

	PROP_DamageType( MOD_GRENADE )
	PROP_SeeSound ("weapons/grenlf")
	PROP_DeathSound ("weapons/grenlx")
	PROP_Obituary("$OB_GRENADE")

END_DEFAULTS

void AGrenade::BeginPlay ()
{
	Super::BeginPlay ();
	this->special1 = (( 5 * TICRATE ) / 2 );
}

void AGrenade::Tick( )
{
	Super::Tick( );

	// Server takes care of exploding missiles.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	if ( this->special1 && GAME_GetFreezeMode( ) == false )
	{
		this->special1--;
		if ( this->special1 == 0 )
			P_ExplodeMissile( this, NULL, NULL );
	}
}

bool AGrenade::FloorBounceMissile( secplane_t &plane )
{
	fixed_t bouncemomx = momx / 4;
	fixed_t bouncemomy = momy / 4;
	fixed_t bouncemomz = FixedMul (momz, (fixed_t)(-0.6*FRACUNIT));
/*
	if (abs (bouncemomz) < (FRACUNIT/2))
	{
		P_ExplodeMissile( this, NULL );
	}
	else
	{
*/
		if (!Super::FloorBounceMissile (plane))
		{
			momx = bouncemomx;
			momy = bouncemomy;
			momz = bouncemomz;
			return false;
		}
//	}
	
	return true;
}

void AGrenade::PreExplode( )
{
	// Prevent the explosing from "falling".
	this->ulSTFlags &= ~STFL_QUARTERGRAVITY;
	this->flags |= MF_NOGRAVITY;
	this->special1 = 0;
}

//
// A_FireSTGrenade
//
void A_FireSTGrenade (AActor *actor)
{
//	AActor		*pGrenade;
	player_t	*player;
	fixed_t		SavedActorPitch;

	if (NULL == (player = actor->player))
	{
		return;
	}

	AWeapon *weapon = actor->player->ReadyWeapon;
	if (weapon != NULL)
	{
		if (!weapon->DepleteAmmo (weapon->bAltFire))
			return;
	}

	// Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	SavedActorPitch = actor->pitch;

	actor->pitch = actor->pitch - ( 1152 << FRACBITS );
	P_SpawnPlayerMissile( actor, RUNTIME_CLASS( AGrenade ));

	// Apply spread.
	if ( player->Powers & PW_SPREAD )
	{
		P_SpawnPlayerMissile( actor, RUNTIME_CLASS( AGrenade ), actor->angle + ( ANGLE_45 / 3 ), false );
		P_SpawnPlayerMissile( actor, RUNTIME_CLASS( AGrenade ), actor->angle - ( ANGLE_45 / 3 ), false );
	}
	
	actor->pitch = SavedActorPitch;

#if 0
	Printf( "%d\n", actor->pitch >> FRACBITS );
	pGrenade = P_SpawnPlayerMissile( actor, RUNTIME_CLASS( AGrenade ));
	if ( pGrenade )
		pGrenade->momz += ( 3 * FRACUNIT );

	// Apply spread.
	if ( player->Powers & PW_SPREAD )
	{
		pGrenade = P_SpawnPlayerMissile( actor, RUNTIME_CLASS( AGrenade ), actor->angle + ( ANGLE_45 / 3 ), false );
		if ( pGrenade )
			pGrenade->momz += ( 3 * FRACUNIT );

		pGrenade = P_SpawnPlayerMissile( actor, RUNTIME_CLASS( AGrenade ), actor->angle - ( ANGLE_45 / 3 ), false );
		if ( pGrenade )
			pGrenade->momz += ( 3 * FRACUNIT );
	}
#endif

	// Tell all the bots that a weapon was fired.
	BOTS_PostWeaponFiredEvent( ULONG( player - players ), BOTEVENT_FIREDGRENADE, BOTEVENT_ENEMY_FIREDGRENADE, BOTEVENT_PLAYER_FIREDGRENADE );
}

// Plasma rifle ------------------------------------------------------------

void A_FirePlasma (AActor *);

class APlasmaRifle : public AWeapon
{
	DECLARE_ACTOR (APlasmaRifle, AWeapon)
};

FState APlasmaRifle::States[] =
{
#define S_PLASMA 0
	S_NORMAL (PLSG, 'A',	1, A_WeaponReady		, &States[S_PLASMA]),

#define S_PLASMADOWN (S_PLASMA+1)
	S_NORMAL (PLSG, 'A',	1, A_Lower				, &States[S_PLASMADOWN]),

#define S_PLASMAUP (S_PLASMADOWN+1)
	S_NORMAL (PLSG, 'A',	1, A_Raise				, &States[S_PLASMAUP]),

#define S_PLASMA1 (S_PLASMAUP+1)
	S_NORMAL (PLSG, 'A',	3, A_FirePlasma 		, &States[S_PLASMA1+1]),
	S_NORMAL (PLSG, 'B',   20, A_ReFire 			, &States[S_PLASMA]),

#define S_PLASMAFLASH (S_PLASMA1+2)
	S_BRIGHT (PLSF, 'A',	4, A_Light1 			, &AWeapon::States[S_LIGHTDONE]),
	S_BRIGHT (PLSF, 'B',	4, A_Light1 			, &AWeapon::States[S_LIGHTDONE]),

#define S_PLAS (S_PLASMAFLASH+2)
	S_NORMAL (PLAS, 'A',   -1, NULL 				, NULL)
};

IMPLEMENT_ACTOR (APlasmaRifle, Doom, 2004, 30)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (16)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_PLAS)

	PROP_Weapon_SelectionOrder (100)
	PROP_Weapon_AmmoUse1 (1)
	PROP_Weapon_AmmoGive1 (40)
	PROP_Weapon_UpState (S_PLASMAUP)
	PROP_Weapon_DownState (S_PLASMADOWN)
	PROP_Weapon_ReadyState (S_PLASMA)
	PROP_Weapon_AtkState (S_PLASMA1)
	PROP_Weapon_HoldAtkState (S_PLASMA1)
	PROP_Weapon_FlashState (S_PLASMAFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_ProjectileType ("PlasmaBall")
	PROP_Weapon_AmmoType1 ("Cell")
	PROP_Inventory_PickupMessage("$GOTPLASMA")
END_DEFAULTS

FState APlasmaBall::States[] =
{
#define S_PLASBALL 0
	S_BRIGHT (PLSS, 'A',	6, NULL 						, &States[S_PLASBALL+1]),
	S_BRIGHT (PLSS, 'B',	6, NULL 						, &States[S_PLASBALL]),

#define S_PLASEXP (S_PLASBALL+2)
	S_BRIGHT (PLSE, 'A',	4, NULL 						, &States[S_PLASEXP+1]),
	S_BRIGHT (PLSE, 'B',	4, NULL 						, &States[S_PLASEXP+2]),
	S_BRIGHT (PLSE, 'C',	4, NULL 						, &States[S_PLASEXP+3]),
	S_BRIGHT (PLSE, 'D',	4, NULL 						, &States[S_PLASEXP+4]),
	S_BRIGHT (PLSE, 'E',	4, NULL 						, NULL)
};

IMPLEMENT_ACTOR (APlasmaBall, Doom, -1, 51)
	PROP_RadiusFixed (13)
	PROP_HeightFixed (8)
	PROP_SpeedFixed (25)
	PROP_Damage (5)
	PROP_Flags (MF_NOBLOCKMAP|MF_MISSILE|MF_DROPOFF|MF_NOGRAVITY)
	PROP_Flags2 (MF2_PCROSS|MF2_IMPACT|MF2_NOTELEPORT)
	PROP_Flags3 (MF3_WARNBOT)
	PROP_Flags4 (MF4_RANDOMIZE)
	PROP_RenderStyle (STYLE_Add)
	PROP_Alpha (TRANSLUC75)

	PROP_SpawnState (S_PLASBALL)
	PROP_DeathState (S_PLASEXP)

	PROP_SeeSound ("weapons/plasmaf")
	PROP_DeathSound ("weapons/plasmax")
	PROP_Obituary("$OB_MPPLASMARIFLE")
END_DEFAULTS

//
// A_FirePlasma
//
void A_FirePlasma (AActor *actor)
{
	player_t *player;

	if (NULL == (player = actor->player))
	{
		return;
	}
	AWeapon *weapon = actor->player->ReadyWeapon;
	if (weapon != NULL)
	{
		if (!weapon->DepleteAmmo (weapon->bAltFire))
			return;
		if (weapon->FlashState != NULL)
		{
			P_SetPsprite (player, ps_flash, weapon->FlashState + (pr_fireplasma()&1));
		}
	}

	// [BC] Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	P_SpawnPlayerMissile (actor, RUNTIME_CLASS(APlasmaBall));

	// [BC] Apply spread.
	if ( player->Powers & PW_SPREAD )
	{
		P_SpawnPlayerMissile( actor, RUNTIME_CLASS( APlasmaBall ), actor->angle + ( ANGLE_45 / 3 ), false );
		P_SpawnPlayerMissile( actor, RUNTIME_CLASS( APlasmaBall ), actor->angle - ( ANGLE_45 / 3 ), false );
	}

	// [BC] Tell all the bots that a weapon was fired.
	BOTS_PostWeaponFiredEvent( ULONG( player - players ), BOTEVENT_FIREDPLASMA, BOTEVENT_ENEMY_FIREDPLASMA, BOTEVENT_PLAYER_FIREDPLASMA );
}

//
// [RH] A_FireRailgun
//
/*static*/ int RailOffset;

void A_FireRailgun (AActor *actor)
{
	int damage;
	player_t *player;
	LONG		lInnerColor;
	LONG		lOuterColor;
	LONG		lColor;

	if (NULL == (player = actor->player))
	{
		return;
	}

	AWeapon *weapon = actor->player->ReadyWeapon;
	if (weapon != NULL)
	{
		if (!weapon->DepleteAmmo (weapon->bAltFire))
			return;
		if (weapon->FlashState != NULL)
		{
			P_SetPsprite (player, ps_flash, weapon->FlashState + (pr_firerail()&1));
		}
	}

	// Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	damage = 200;
	if ( deathmatch || teamgame )
	{
		if ( instagib )
			damage = 1000;
		else
			damage = 75;
	}

	// Determine the railgun trail's color.
	switch ( player->userinfo.lRailgunTrailColor )
	{
	case RAILCOLOR_BLUE:
	default:

		lColor = V_GetColorFromString( NULL, "00 00 ff" );
		break;
	case RAILCOLOR_RED:

		lColor = V_GetColorFromString( NULL, "ff 00 00" );
		break;
	case RAILCOLOR_YELLOW:

		lColor = V_GetColorFromString( NULL, "ff ff 00" );
		break;
	case RAILCOLOR_BLACK:

		lColor = V_GetColorFromString( NULL, "0f 0f 0f" );
		break;
	case RAILCOLOR_SILVER:

		lColor = V_GetColorFromString( NULL, "9f 9f 9f" );
		break;
	case RAILCOLOR_GOLD:

		lColor = V_GetColorFromString( NULL, "bf 8f 2f" );
		break;
	case RAILCOLOR_GREEN:

		lColor = V_GetColorFromString( NULL, "00 ff 00" );
		break;
	case RAILCOLOR_WHITE:

		lColor = V_GetColorFromString( NULL, "ff ff ff" );
		break;
	case RAILCOLOR_PURPLE:

		lColor = V_GetColorFromString( NULL, "ff 00 ff" );
		break;
	case RAILCOLOR_ORANGE:

		lColor = V_GetColorFromString( NULL, "ff 8f 00" );
		break;
	case RAILCOLOR_RAINBOW:

		lColor = -2;
		break;
	}

	if (( teamgame || teamplay || teamlms || teampossession ) && ( player->bOnTeam ))
	{
		lOuterColor = TEAM_GetRailgunColor( player->ulTeam );
		lInnerColor = lColor;
	}
	else
	{
		lOuterColor = lColor;
		lInnerColor = V_GetColorFromString( NULL, "ff ff ff" );
	}

	P_RailAttack (actor, damage, RailOffset, lOuterColor, lInnerColor);

	// [BC] Apply spread.
	if ( player->Powers & PW_SPREAD )
	{
		fixed_t		SavedActorAngle;

		SavedActorAngle = actor->angle;

		actor->angle += ( ANGLE_45 / 3 );
		P_RailAttack( actor, damage, RailOffset, lOuterColor, lInnerColor );
		actor->angle = SavedActorAngle;

		actor->angle -= ( ANGLE_45 / 3 );
		P_RailAttack( actor, damage, RailOffset, lOuterColor, lInnerColor );
		actor->angle = SavedActorAngle;
	}

	RailOffset = 0;

	// [BC] Tell all the bots that a weapon was fired.
	BOTS_PostWeaponFiredEvent( ULONG( player - players ), BOTEVENT_FIREDRAILGUN, BOTEVENT_ENEMY_FIREDRAILGUN, BOTEVENT_PLAYER_FIREDRAILGUN );
}

void A_FireRailgunRight (AActor *actor)
{
	RailOffset = 10;
	A_FireRailgun (actor);
}

void A_FireRailgunLeft (AActor *actor)
{
	RailOffset = -10;
	A_FireRailgun (actor);
}

void A_RailWait (AActor *actor)
{
	// Okay, this was stupid. Just use a NULL function instead of this.
}

// Railgun ------------------------------------------------------------

void A_CheckRailReload( AActor *pActor );

class ARailgun : public AWeapon
{
	DECLARE_ACTOR (ARailgun, AWeapon)
};

FState ARailgun::States[] =
{
#define S_RAILGUN 0
	S_NORMAL (RLGG, 'A',	1, A_WeaponReady		, &States[S_RAILGUN]),

#define S_RAILGUNDOWN (S_RAILGUN+1)
	S_NORMAL (RLGG, 'A',	1, A_Lower				, &States[S_RAILGUNDOWN]),

#define S_RAILGUNUP (S_RAILGUNDOWN+1)
	S_NORMAL (RLGG, 'A',	1, A_Raise				, &States[S_RAILGUNUP]),

#define S_RAILGUN1 (S_RAILGUNUP+1)
	S_NORMAL (RLGG, 'E',	12, A_FireRailgun 		, &States[S_RAILGUN1+1]),
	S_NORMAL (RLGG, 'F',	6, A_CheckRailReload	, &States[S_RAILGUN1+2]),
	S_NORMAL (RLGG, 'G',	6, NULL					, &States[S_RAILGUN1+3]),
	S_NORMAL (RLGG, 'H',	6, NULL					, &States[S_RAILGUN1+4]),
	S_NORMAL (RLGG, 'I',	6, NULL					, &States[S_RAILGUN1+5]),
	S_NORMAL (RLGG, 'J',	6, NULL					, &States[S_RAILGUN1+6]),
	S_NORMAL (RLGG, 'K',	6, NULL					, &States[S_RAILGUN1+7]),
	S_NORMAL (RLGG, 'L',	6, NULL					, &States[S_RAILGUN1+8]),
	S_NORMAL (RLGG, 'A',	6, NULL					, &States[S_RAILGUN1+9]),
	S_NORMAL (RLGG, 'M',	0, A_ReFire				, &States[S_RAILGUN]),

#define S_RAILGUNFLASH (S_RAILGUN1+10)
	S_BRIGHT (TNT1, 'A',	5, A_Light1 			, &States[S_RAILGUNFLASH+1]),
	S_BRIGHT (TNT1, 'A',	5, A_Light2 			, &AWeapon::States[S_LIGHTDONE]),

#define S_RAIL (S_RAILGUNFLASH+2)
	S_NORMAL (RAIL, 'A',	-1, NULL 				, NULL)
};

IMPLEMENT_ACTOR (ARailgun, Doom, 5012, 164)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (16)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_RAIL)

	PROP_Weapon_SelectionOrder (100)
	PROP_Weapon_AmmoUse1 (10)
	PROP_Weapon_AmmoGive1 (40)
	PROP_Weapon_UpState (S_RAILGUNUP)
	PROP_Weapon_DownState (S_RAILGUNDOWN)
	PROP_Weapon_ReadyState (S_RAILGUN)
	PROP_Weapon_AtkState (S_RAILGUN1)
	PROP_Weapon_HoldAtkState (S_RAILGUN1)
	PROP_Weapon_FlashState (S_RAILGUNFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_AmmoType1 ("Cell")
	PROP_Inventory_PickupMessage("$PICKUP_RAILGUN")
END_DEFAULTS

void A_CheckRailReload( AActor *pActor )
{
	if ( pActor->player == NULL )
		return;

	pActor->player->ulRailgunShots++;
	// If we have not made our 4th shot...
	if ((( pActor->player->ulRailgunShots % 4 ) == 0 ) == false )
		// Go back to the refire frames, instead of continuing on to the reload frames.
		P_SetPsprite( pActor->player, ps_weapon, pActor->player->ReadyWeapon->AtkState + 8 );
	else
		// We need to reload. However, don't reload if we're out of ammo.
		pActor->player->ReadyWeapon->CheckAmmo( false, false );
}

// BFG 9000 -----------------------------------------------------------------

void A_FireBFG (AActor *);
void A_BFGSpray (AActor *);
void A_BFGsound (AActor *);

class ABFG9000 : public AWeapon
{
	DECLARE_ACTOR (ABFG9000, AWeapon)
};

class ABFGExtra : public AActor
{
	DECLARE_ACTOR (ABFGExtra, AActor)
};

FState ABFG9000::States[] =
{
#define S_BFG 0
	S_NORMAL (BFGG, 'A',	1, A_WeaponReady		, &States[S_BFG]),

#define S_BFGDOWN (S_BFG+1)
	S_NORMAL (BFGG, 'A',	1, A_Lower				, &States[S_BFGDOWN]),

#define S_BFGUP (S_BFGDOWN+1)
	S_NORMAL (BFGG, 'A',	1, A_Raise				, &States[S_BFGUP]),

#define S_BFG1 (S_BFGUP+1)
	S_NORMAL (BFGG, 'A',   20, A_BFGsound			, &States[S_BFG1+1]),
	S_NORMAL (BFGG, 'B',   10, A_GunFlash			, &States[S_BFG1+2]),
	S_NORMAL (BFGG, 'B',   10, A_FireBFG			, &States[S_BFG1+3]),
	S_NORMAL (BFGG, 'B',   20, A_ReFire 			, &States[S_BFG]),

#define S_BFGFLASH (S_BFG1+4)
	S_BRIGHT (BFGF, 'A',   11, A_Light1 			, &States[S_BFGFLASH+1]),
	S_BRIGHT (BFGF, 'B',	6, A_Light2 			, &AWeapon::States[S_LIGHTDONE]),

#define S_BFUG (S_BFGFLASH+2)
	S_NORMAL (BFUG, 'A',   -1, NULL 				, NULL)
};

IMPLEMENT_ACTOR (ABFG9000, Doom, 2006, 31)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (20)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_BFUG)

	PROP_Weapon_Flags (WIF_NOAUTOFIRE)
	PROP_Weapon_SelectionOrder (2800)
	PROP_Weapon_AmmoUse1 (40)
	PROP_Weapon_AmmoGive1 (40)
	PROP_Weapon_UpState (S_BFGUP)
	PROP_Weapon_DownState (S_BFGDOWN)
	PROP_Weapon_ReadyState (S_BFG)
	PROP_Weapon_AtkState (S_BFG1)
	PROP_Weapon_HoldAtkState (S_BFG1)
	PROP_Weapon_FlashState (S_BFGFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_AmmoType1 ("Cell")
	PROP_Weapon_ProjectileType ("BFGBall")
	PROP_Inventory_PickupMessage("$GOTBFG9000")
END_DEFAULTS

FState ABFGBall::States[] =
{
#define S_BFGSHOT 0
	S_BRIGHT (BFS1, 'A',	4, NULL 						, &States[S_BFGSHOT+1]),
	S_BRIGHT (BFS1, 'B',	4, NULL 						, &States[S_BFGSHOT]),

#define S_BFGLAND (S_BFGSHOT+2)
	S_BRIGHT (BFE1, 'A',	8, NULL 						, &States[S_BFGLAND+1]),
	S_BRIGHT (BFE1, 'B',	8, NULL 						, &States[S_BFGLAND+2]),
	S_BRIGHT (BFE1, 'C',	8, A_BFGSpray					, &States[S_BFGLAND+3]),
	S_BRIGHT (BFE1, 'D',	8, NULL 						, &States[S_BFGLAND+4]),
	S_BRIGHT (BFE1, 'E',	8, NULL 						, &States[S_BFGLAND+5]),
	S_BRIGHT (BFE1, 'F',	8, NULL 						, NULL)
};

IMPLEMENT_ACTOR (ABFGBall, Doom, -1, 128)
	PROP_RadiusFixed (13)
	PROP_HeightFixed (8)
	PROP_SpeedFixed (25)
	PROP_Damage (100)
	PROP_Flags (MF_NOBLOCKMAP|MF_MISSILE|MF_DROPOFF|MF_NOGRAVITY)
	PROP_Flags2 (MF2_PCROSS|MF2_IMPACT|MF2_NOTELEPORT)
	PROP_Flags4 (MF4_RANDOMIZE)
	PROP_RenderStyle (STYLE_Add)
	PROP_Alpha (TRANSLUC75)

	PROP_SpawnState (S_BFGSHOT)
	PROP_DeathState (S_BFGLAND)

	PROP_DeathSound ("weapons/bfgx")
	PROP_Obituary("$OB_MPBFG_BOOM")
END_DEFAULTS

FState ABFGExtra::States[] =
{
	S_BRIGHT (BFE2, 'A',	8, NULL 				, &States[1]),
	S_BRIGHT (BFE2, 'B',	8, NULL 				, &States[2]),
	S_BRIGHT (BFE2, 'C',	8, NULL 				, &States[3]),
	S_BRIGHT (BFE2, 'D',	8, NULL 				, NULL)
};

IMPLEMENT_ACTOR (ABFGExtra, Doom, -1, 0)
	PROP_Flags (MF_NOBLOCKMAP|MF_NOGRAVITY)
	PROP_RenderStyle (STYLE_Add)
	PROP_Alpha (TRANSLUC75)

	PROP_SpawnState (0)
END_DEFAULTS

//
// A_FireBFG
//

void A_FireBFG (AActor *actor)
{
	player_t *player;

	if (NULL == (player = actor->player))
	{
		return;
	}
	// [RH] bfg can be forced to not use freeaim
	angle_t storedpitch = actor->pitch;
	int storedaimdist = player->userinfo.aimdist;

	AWeapon *weapon = actor->player->ReadyWeapon;
	if (weapon != NULL)
	{
		if (!weapon->DepleteAmmo (weapon->bAltFire))
			return;
	}

	// [BC] Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	if (( dmflags2 & DF2_YES_FREEAIMBFG ) == false )
	{
		actor->pitch = 0;
		player->userinfo.aimdist = ANGLE_1*35;
	}
	P_SpawnPlayerMissile (actor, RUNTIME_CLASS(ABFGBall));

	// [BC] Apply spread.
	if ( player->Powers & PW_SPREAD )
	{
		P_SpawnPlayerMissile( actor, RUNTIME_CLASS( ABFGBall ), actor->angle + ( ANGLE_45 / 3 ), false );
		P_SpawnPlayerMissile( actor, RUNTIME_CLASS( ABFGBall ), actor->angle - ( ANGLE_45 / 3 ), false );
	}

	actor->pitch = storedpitch;
	player->userinfo.aimdist = storedaimdist;
}

//
// A_BFGSpray
// Spawn a BFG explosion on every monster in view
//
void A_BFGSpray (AActor *mo) 
{
	int 				i;
	int 				j;
	int 				damage;
	angle_t 			an;
	AActor				*thingToHit;
	const PClass		*spraytype = NULL;
	int					numrays = 40;
	int					damagecnt = 15;
	// [BC] Pointer to the spawned tracer that we send to clients.
	AActor				*pActor;

	// [BC] This is not done on the client end.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	int index = CheckIndex (3, NULL);
	if (index >= 0) 
	{
		spraytype = PClass::FindClass ((ENamedName)StateParameters[index]);
		numrays = EvalExpressionI (StateParameters[index+1], mo);
		if (numrays <= 0)
			numrays = 40;
		damagecnt = EvalExpressionI (StateParameters[index+2], mo);
		if (damagecnt <= 0)
			damagecnt = 15;
	}
	if (spraytype == NULL)
	{
		spraytype = RUNTIME_CLASS(ABFGExtra);
	}

	// [RH] Don't crash if no target
	if (!mo->target)
		return;

	// offset angles from its attack angle
	for (i = 0; i < numrays; i++)
	{
		an = mo->angle - ANG90/2 + ANG90/numrays*i;

		// mo->target is the originator (player) of the missile
		P_AimLineAttack (mo->target, an, 16*64*FRACUNIT, ANGLE_1*32);

		if (!linetarget)
			continue;

		// [BC] Get a pointer to the spawned tracer so we can send it to clients.
		pActor = Spawn (spraytype, linetarget->x, linetarget->y,
			linetarget->z + (linetarget->height>>2), ALLOW_REPLACE);
		
		// [BC] Tell clients to spawn the tracers.
		if (( NETWORK_GetState( ) == NETSTATE_SERVER ) && ( pActor ))
			SERVERCOMMANDS_SpawnThing( pActor );

		damage = 0;
		for (j = 0; j < damagecnt; ++j)
			damage += (pr_bfgspray() & 7) + 1;

		thingToHit = linetarget;
		P_DamageMobj (thingToHit, mo->target, mo->target, damage, MOD_BFG_SPLASH);
		P_TraceBleed (damage, thingToHit, mo->target);
	}
}

//
// A_BFGsound
//
void A_BFGsound (AActor *actor)
{
	S_Sound (actor, CHAN_WEAPON, "weapons/bfgf", 1, ATTN_NORM);

	// [BC] Tell the clients to trigger the BFG firing sound.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		if ( actor->player )
			SERVERCOMMANDS_SoundActor( actor, CHAN_WEAPON, "weapons/bfgf", 127, ATTN_NORM, ULONG( actor->player - players ), SVCF_SKIPTHISCLIENT );
		else
			SERVERCOMMANDS_SoundActor( actor, CHAN_WEAPON, "weapons/bfgf", 127, ATTN_NORM );
	}

	if ( actor->player )
	{
		// Tell all the bots that a weapon was fired.
		BOTS_PostWeaponFiredEvent( ULONG( actor->player - players ), BOTEVENT_FIREDBFG, BOTEVENT_ENEMY_FIREDBFG, BOTEVENT_PLAYER_FIREDBFG );
	}
}

// BFG 10k -----------------------------------------------------------------

void A_FireBFG10k( AActor *pActor );
void A_BFG10kSound( AActor *pActor );
void A_BFG10kCoolDown( AActor *pActor );

class ABFG10K : public AWeapon
{
	DECLARE_ACTOR (ABFG10K, AWeapon)
};

FState ABFG10K::States[] =
{
#define S_BFG10K 0
	S_NORMAL (BFG2, 'A',   -1, NULL 				, NULL),

#define S_BFG10KIDLE (S_BFG10K + 1)
	S_NORMAL (BG2G, 'A',	1, A_WeaponReady		, &States[S_BFG10KIDLE+1]),
	S_NORMAL (BG2G, 'A',	1, A_WeaponReady		, &States[S_BFG10KIDLE+2]),
	S_NORMAL (BG2G, 'A',	1, A_WeaponReady		, &States[S_BFG10KIDLE+3]),
	S_NORMAL (BG2G, 'B',	1, A_WeaponReady		, &States[S_BFG10KIDLE+4]),
	S_NORMAL (BG2G, 'B',	1, A_WeaponReady		, &States[S_BFG10KIDLE+5]),
	S_NORMAL (BG2G, 'B',	1, A_WeaponReady		, &States[S_BFG10KIDLE+6]),
	S_NORMAL (BG2G, 'C',	1, A_WeaponReady		, &States[S_BFG10KIDLE+7]),
	S_NORMAL (BG2G, 'C',	1, A_WeaponReady		, &States[S_BFG10KIDLE+8]),
	S_NORMAL (BG2G, 'C',	1, A_WeaponReady		, &States[S_BFG10KIDLE+9]),
	S_NORMAL (BG2G, 'D',	1, A_WeaponReady		, &States[S_BFG10KIDLE+10]),
	S_NORMAL (BG2G, 'D',	1, A_WeaponReady		, &States[S_BFG10KIDLE+11]),
	S_NORMAL (BG2G, 'D',	1, A_WeaponReady		, &States[S_BFG10KIDLE]),

#define S_BFG10KDOWN (S_BFG10KIDLE+12)
	S_NORMAL (BG2G, 'E',	1, A_Lower				, &States[S_BFG10KDOWN]),

#define S_BFG10KUP (S_BFG10KDOWN+1)
	S_NORMAL (BG2G, 'E',	1, A_Raise				, &States[S_BFG10KUP]),

#define S_BFG10KATK1 (S_BFG10KUP+1)
	S_NORMAL (BG2G, 'E',   20, A_BFG10kSound			, &States[S_BFG10KATK1+1]),
	S_NORMAL (BG2G, 'F',   4, NULL					, &States[S_BFG10KATK1+2]),
	S_NORMAL (BG2G, 'G',   1, NULL					, &States[S_BFG10KATK1+3]),
	S_NORMAL (BG2G, 'H',   1, NULL					, &States[S_BFG10KATK1+4]),
	S_NORMAL (BG2G, 'I',   1, NULL					, &States[S_BFG10KATK1+5]),
	S_NORMAL (BG2G, 'J',   1, NULL					, &States[S_BFG10KATK1+6]),
	S_NORMAL (BG2G, 'K',   2, A_GunFlash			, &States[S_BFG10KATK1+7]),
	S_NORMAL (BG2G, 'L',   2, A_FireBFG10k			, &States[S_BFG10KATK1+8]),
	S_NORMAL (BG2G, 'M',   2, NULL					, &States[S_BFG10KATK1+9]),
	S_NORMAL (BG2G, 'N',   0, A_ReFire 				, &States[S_BFG10KATK1+10]),
	S_NORMAL (BG2G, 'O',   35, A_BFG10kCoolDown		, &States[S_BFG10KIDLE]),

#define S_BFG10KFLASH (S_BFG10KATK1+11)
	S_BRIGHT (TNT1, 'A',	2, A_Light1 			, &States[S_BFG10KFLASH+1]),
	S_BRIGHT (TNT1, 'A',	3, NULL		 			, &AWeapon::States[S_LIGHTDONE]),
	S_BRIGHT (TNT1, 'A',	3, A_Light2 			, &AWeapon::States[S_LIGHTDONE]),
	S_BRIGHT (TNT1, 'A',	3, A_Light2 			, &AWeapon::States[S_LIGHTDONE]),
};

IMPLEMENT_ACTOR (ABFG10K, Doom, 5013, 165)
	PROP_RadiusFixed (20)
	PROP_HeightFixed (20)
	PROP_Flags (MF_SPECIAL)
	PROP_SpawnState (S_BFG10K)

	PROP_Weapon_Flags (WIF_NOAUTOFIRE|WIF_RADIUSDAMAGE_BOSSES)
	PROP_Weapon_SelectionOrder (2800)
	PROP_Weapon_AmmoUse1 (5)
//	PROP_Weapon_AmmoUseDM1 (10)
	PROP_Weapon_AmmoGive1 (40)
	PROP_Weapon_UpState (S_BFG10KUP)
	PROP_Weapon_DownState (S_BFG10KDOWN)
	PROP_Weapon_ReadyState (S_BFG10KIDLE)
	PROP_Weapon_AtkState (S_BFG10KATK1)
	PROP_Weapon_HoldAtkState (S_BFG10KATK1+6)
	PROP_Weapon_FlashState (S_BFG10KFLASH)
	PROP_Weapon_Kickback (100)
	PROP_Weapon_AmmoType1 ("Cell")
	PROP_Weapon_ProjectileType ("BFG10kShot")
	PROP_Weapon_ReadySound ("weapons/bfg10kidle")
	PROP_Inventory_PickupMessage("$PICKUP_BFG10K")
END_DEFAULTS

class ABFG10kShot : public AActor
{
	DECLARE_ACTOR (ABFG10kShot, AActor)
public:
	void BeginPlay ();
};

FState ABFG10kShot::States[] =
{
#define S_BFG10KSHOT (0)
	S_BRIGHT (BFE1, 'A',	1, NULL							, &States[S_BFG10KSHOT+1]),
	S_BRIGHT (BFE1, 'A',	2, A_Detonate					, &States[S_BFG10KSHOT+2]),
	S_BRIGHT (BFE1, 'B',	3, NULL 						, &States[S_BFG10KSHOT+3]),
	S_BRIGHT (BFE1, 'C',	3, NULL							, &States[S_BFG10KSHOT+4]),
	S_BRIGHT (BFE1, 'D',	3, NULL 						, &States[S_BFG10KSHOT+5]),
	S_BRIGHT (BFE1, 'E',	3, NULL 						, &States[S_BFG10KSHOT+6]),
	S_BRIGHT (BFE1, 'F',	3, NULL 						, NULL)
};

IMPLEMENT_ACTOR (ABFG10kShot, Doom, -1, 217)
	PROP_RadiusFixed (11)
	PROP_HeightFixed (8)
	PROP_SpeedFixed (20)
	PROP_Damage (160)
	PROP_Flags (MF_NOBLOCKMAP|MF_DROPOFF|MF_NOGRAVITY)
	PROP_Flags2 (MF2_PCROSS|MF2_IMPACT|MF2_NOTELEPORT)
	PROP_Flags3 (MF3_PUFFONACTORS)
	PROP_RenderStyle (STYLE_Add)
	PROP_Alpha (TRANSLUC75)
	PROP_DamageType( MOD_BFG10K )

	PROP_SpawnState (S_BFG10KSHOT)

	PROP_SeeSound ("weapons/bfg10kx")
	PROP_AttackSound ("weapons/bfg10kx")
	PROP_Obituary( "$OB_BFG10K" )
END_DEFAULTS

// [BC] EWEWEWEWEEEWEEW
extern	AActor	*shootthing;
void ABFG10kShot::BeginPlay ()
{
	Super::BeginPlay ( );

	// BFG10k shots create an explosion, in which case we need to know the target.
	if ( shootthing && shootthing->player )
		target = shootthing;

	SetState( this->SpawnState );
}

//
// A_FireBFG10k
//
void A_FireBFG10k( AActor *pActor )
{
	player_s	*pPlayer;
	AWeapon		*pWeapon;

	pPlayer = pActor->player;
	if ( pPlayer == NULL )
		return;

	pWeapon = pPlayer->ReadyWeapon;
	if ( pWeapon != NULL )
	{
		// Break out if we don't have enough ammo to fire the weapon. This shouldn't ever happen
		// because the function that puts the weapon into its fire state first checks to see if
		// the weapon has enough ammo to fire.
		if ( pWeapon->DepleteAmmo( pWeapon->bAltFire ) == false )
			return;
	}

	// Weapons are handled by the server.
	if ( NETWORK_GetState( ) == NETSTATE_CLIENT )
		return;

	P_BulletSlope( pActor );
	P_GunShot( pActor, true, RUNTIME_CLASS( ABFG10kShot ));

	// Apply spread.
	if ( pPlayer->Powers & PW_SPREAD )
	{
		fixed_t		SavedActorAngle;

		SavedActorAngle = pActor->angle;

		pActor->angle += ( ANGLE_45 / 3 );
		P_GunShot( pActor, true, RUNTIME_CLASS( ABFG10kShot ));
		pActor->angle = SavedActorAngle;

		pActor->angle -= ( ANGLE_45 / 3 );
		P_GunShot( pActor, true, RUNTIME_CLASS( ABFG10kShot ));
		pActor->angle = SavedActorAngle;
	}

	// Tell all the bots that a weapon was fired.
	BOTS_PostWeaponFiredEvent( ULONG( pPlayer - players ), BOTEVENT_FIREDBFG10K, BOTEVENT_ENEMY_FIREDBFG10K, BOTEVENT_PLAYER_FIREDBFG10K );
}

//
// A_BFG10kSound
//
void A_BFG10kSound( AActor *actor )
{
	S_Sound( actor, CHAN_WEAPON, "weapons/bfg10kf", 1, ATTN_NORM );

	// Tell the clients to trigger the BFG firing sound.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		if ( actor->player )
			SERVERCOMMANDS_SoundActor( actor, CHAN_WEAPON, "weapons/bfg10kf", 127, ATTN_NORM, ULONG( actor->player - players ), SVCF_SKIPTHISCLIENT );
		else
			SERVERCOMMANDS_SoundActor( actor, CHAN_WEAPON, "weapons/bfg10kf", 127, ATTN_NORM );
	}
}

//
// A_BFG10kCoolDown
//
void A_BFG10kCoolDown( AActor *actor )
{
	S_Sound( actor, CHAN_WEAPON, "weapons/bfg10kcool", 1, ATTN_NORM );

	// Tell the clients to trigger the BFG firing sound.
	if ( NETWORK_GetState( ) == NETSTATE_SERVER )
	{
		if ( actor->player )
			SERVERCOMMANDS_SoundActor( actor, CHAN_WEAPON, "weapons/bfg10kcool", 127, ATTN_NORM, ULONG( actor->player - players ), SVCF_SKIPTHISCLIENT );
		else
			SERVERCOMMANDS_SoundActor( actor, CHAN_WEAPON, "weapons/bfg10kcool", 127, ATTN_NORM );
	}
}
