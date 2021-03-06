#ifndef __A_ARTIFACTS_H__
#define __A_ARTIFACTS_H__

#include "farchive.h"
#include "a_pickups.h"

#define INVERSECOLOR 0x00345678
#define GOLDCOLOR 0x009abcde

// [BC] More hacks!
#define REDCOLOR 0x00beefee
#define GREENCOLOR 0x00beefad

#define STREAM_ENUM(e) \
	inline FArchive &operator<< (FArchive &arc, e &i) \
	{ \
		BYTE val = (BYTE)i; \
		arc << val; \
		i = (e)val; \
		return arc; \
	}

class player_s;

// A powerup is a pseudo-inventory item that applies an effect to its
// owner while it is present.
class APowerup : public AInventory
{
	DECLARE_STATELESS_ACTOR (APowerup, AInventory)
public:
	virtual void Tick ();
	virtual void Destroy ();
	virtual bool HandlePickup (AInventory *item);
	virtual AInventory *CreateCopy (AActor *other);
	virtual AInventory *CreateTossable ();
	virtual void Serialize (FArchive &arc);
	virtual void OwnerDied ();
	virtual PalEntry GetBlend ();
	virtual bool DrawPowerup (int x, int y);

	int EffectTics;
	PalEntry BlendColor;

protected:
	virtual void InitEffect ();
	virtual void DoEffect ();
	virtual void EndEffect ();
};

// An artifact is an item that gives the player a powerup when activated.
class APowerupGiver : public AInventory
{
	DECLARE_STATELESS_ACTOR (APowerupGiver, AInventory)
public:
	virtual bool Use (bool pickup);
	virtual void Serialize (FArchive &arc);

	const PClass *PowerupType;
	int EffectTics;			// Non-0 to override the powerup's default tics
	PalEntry BlendColor;	// Non-0 to override the powerup's default blend
};

class APowerInvulnerable : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerInvulnerable, APowerup)
protected:
	void InitEffect ();
	void DoEffect ();
	void EndEffect ();
	void AlterWeaponSprite (vissprite_t *vis);
};

class APowerStrength : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerStrength, APowerup)
public:
	PalEntry GetBlend ();
protected:
	void InitEffect ();
	void DoEffect ();
	bool HandlePickup (AInventory *item);
};

class APowerInvisibility : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerInvisibility, APowerup)
protected:
	void InitEffect ();
	void EndEffect ();
	void AlterWeaponSprite (vissprite_t *vis);
};

class APowerGhost : public APowerInvisibility
{
	DECLARE_STATELESS_ACTOR (APowerGhost, APowerInvisibility)
protected:
	void InitEffect ();
};

class APowerShadow : public APowerInvisibility
{
	DECLARE_STATELESS_ACTOR (APowerShadow, APowerInvisibility)
protected:
	void InitEffect ();
};

class APowerIronFeet : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerIronFeet, APowerup)
public:
	void AbsorbDamage (int damage, int damageType, int &newdamage);
};

class APowerMask : public APowerIronFeet
{
	DECLARE_STATELESS_ACTOR (APowerMask, APowerIronFeet)
public:
	void AbsorbDamage (int damage, int damageType, int &newdamage);
	void DoEffect ();
};

class APowerLightAmp : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerLightAmp, APowerup)
protected:
	void DoEffect ();
	void EndEffect ();
};

class APowerTorch : public APowerLightAmp
{
	DECLARE_STATELESS_ACTOR (APowerTorch, APowerLightAmp)
public:
	void Serialize (FArchive &arc);
protected:
	void DoEffect ();
	int NewTorch, NewTorchDelta;
};

class APowerFlight : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerFlight, APowerup)
public:
	bool DrawPowerup (int x, int y);
	void Serialize (FArchive &arc);

protected:
	void InitEffect ();
	void DoEffect ();
	void EndEffect ();

	bool HitCenterFrame;
};

class APowerWeaponLevel2 : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerWeaponLevel2, APowerup)
protected:
	void InitEffect ();
	void EndEffect ();
};

class APowerSpeed : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerSpeed, APowerup)
protected:
	void InitEffect ();
	void DoEffect ();
	void EndEffect ();
};

class APowerMinotaur : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerMinotaur, APowerup)
};

class APowerScanner : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerScanner, APowerup)
};

class APowerTargeter : public APowerup
{
	DECLARE_ACTOR (APowerTargeter, APowerup)
protected:
	void InitEffect ();
	void DoEffect ();
	void EndEffect ();
	void PositionAccuracy ();
	void Travelled ();
};

class APowerFrightener : public APowerup
{
	DECLARE_STATELESS_ACTOR (APowerFrightener, APowerup)
protected:
	void InitEffect ();
	void EndEffect ();
};

// [BC] Start of new Skulltag powerup types.
class APowerTimeFreezer : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerTimeFreezer, APowerup )
protected:
	void InitEffect( );
	void DoEffect( );
	void EndEffect( );
};

class APowerQuadDamage : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerQuadDamage, APowerup )
protected:
	void InitEffect( );
	void EndEffect( );
};

class APowerQuarterDamage : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerQuarterDamage, APowerup )
protected:
	void InitEffect( );
	void EndEffect( );
};

class APowerPossessionArtifact : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerPossessionArtifact, APowerup )
protected:
	void InitEffect( );
	void DoEffect( );
	void EndEffect( );
};

class APowerTerminatorArtifact : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerTerminatorArtifact, APowerup )
protected:
	void InitEffect( );
	void DoEffect( );
	void EndEffect( );
};

class APowerTranslucency : public APowerInvisibility
{
	DECLARE_STATELESS_ACTOR (APowerTranslucency, APowerInvisibility)
protected:
	void InitEffect ();
};

// [BC] A rune is like a powerup, except its effect lasts until a new rune is picked up,
// or the owner dies. Only one rune may be carried at once.
class ARune : public AInventory
{
	DECLARE_STATELESS_ACTOR (ARune, AInventory)
public:
	virtual void Tick ();
	virtual void Destroy ();
	virtual bool HandlePickup (AInventory *item);
	virtual AInventory *CreateCopy (AActor *other);
	virtual AInventory *CreateTossable ();
	virtual void Serialize (FArchive &arc);
	virtual void OwnerDied ();
	virtual PalEntry GetBlend ();
	virtual bool DrawPowerup (int x, int y);
	virtual void DetachFromOwner( );

	int EffectTics;
	PalEntry BlendColor;

protected:
	virtual void InitEffect ();
	virtual void DoEffect ();
	virtual void EndEffect ();
};

class ARuneGiver : public AInventory
{
	DECLARE_STATELESS_ACTOR (ARuneGiver, AInventory)
public:
	virtual bool Use (bool pickup);
	virtual void Serialize (FArchive &arc);

	const PClass *RuneType;
	int EffectTics;			// Non-0 to override the powerup's default tics
	PalEntry BlendColor;	// Non-0 to override the powerup's default blend
};
//GHK
class APowerDoubleDamage : public APowerup //ARune
{
	DECLARE_STATELESS_ACTOR( APowerDoubleDamage, APowerup )
protected:
	void InitEffect( );
	void EndEffect( );
};

class APowerDoubleFiringSpeed : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerDoubleFiringSpeed, APowerup )
protected:
	void InitEffect( );
	void EndEffect( );
};

class APowerDrain : public APowerup //ARune
{
	DECLARE_STATELESS_ACTOR( APowerDrain, APowerup )
protected:
	void InitEffect( );
	void EndEffect( );
};

class APowerSpread : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerSpread, APowerup )
protected:
	void InitEffect( );
	void EndEffect( );
};

class APowerHalfDamage : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerHalfDamage, APowerup )
protected:
	void InitEffect( );
	void EndEffect( );
};

//ghk
class APowerInfiniteAmmo : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerInfiniteAmmo, APowerup )
protected:
	void InitEffect( );
	void EndEffect( );
};

class APowerRegeneration : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerRegeneration, APowerup )
protected:
	void InitEffect( );
	void EndEffect( );
};


class APowerReflection : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerReflection, APowerup )
protected:
	void InitEffect( );
	void EndEffect( );
};

class APowerHighJump : public APowerup
{
	DECLARE_STATELESS_ACTOR( APowerHighJump, APowerup )
protected:
	void InitEffect( );
	void DoEffect( );
	void EndEffect( );
};

class ARuneSpeed25 : public ARune
{
	DECLARE_STATELESS_ACTOR( ARuneSpeed25, ARune )
protected:
	void InitEffect( );
	void DoEffect( );
	void EndEffect( );
};

class ARuneProsperity : public ARune
{
	DECLARE_STATELESS_ACTOR( ARuneProsperity, ARune )
protected:
	void InitEffect( );
	void EndEffect( );
};

class ARuneDoubleDamage : public ARune
{
	DECLARE_STATELESS_ACTOR( ARuneDoubleDamage, ARune )
protected:
	void InitEffect( );
	void EndEffect( );
};

class ARuneDoubleFiringSpeed : public ARune
{
	DECLARE_STATELESS_ACTOR( ARuneDoubleFiringSpeed, ARune )
protected:
	void InitEffect( );
	void EndEffect( );
};

class ARuneDrain : public ARune
{
	DECLARE_STATELESS_ACTOR( ARuneDrain, ARune )
protected:
	void InitEffect( );
	void EndEffect( );
};

class ARuneSpread : public ARune
{
	DECLARE_STATELESS_ACTOR( ARuneSpread, ARune )
protected:
	void InitEffect( );
	void EndEffect( );
};

class ARuneHalfDamage : public ARune
{
	DECLARE_STATELESS_ACTOR( ARuneHalfDamage, ARune )
protected:
	void InitEffect( );
	void EndEffect( );
};

class ARuneRegeneration : public ARune
{
	DECLARE_STATELESS_ACTOR( ARuneRegeneration, ARune )
protected:
	void InitEffect( );
	void EndEffect( );
};

class ARuneReflection : public ARune
{
	DECLARE_STATELESS_ACTOR( ARuneReflection, ARune )
protected:
	void InitEffect( );
	void EndEffect( );
};

class ARuneHighJump : public ARune
{
	DECLARE_STATELESS_ACTOR( ARuneHighJump, ARune )
protected:
	void InitEffect( );
	void EndEffect( );
};


class player_s;

#endif //__A_ARTIFACTS_H__
