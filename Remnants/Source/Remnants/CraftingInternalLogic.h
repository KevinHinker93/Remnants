// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum CrystalType
{
	None,
	Fire,
	Water,
	Air,
	Earth,
	Steam,
	Ice,
	Lava,
	Acid,
	Lightning,
	Crystal,
};

class REMNANTS_API AbstractCrystalElement
{
public:
	AbstractCrystalElement(CrystalType type, int itemManagerIndex)
	{
		CrystalElementType = type;
		ItemManagerIndex = itemManagerIndex;
		TypeLookupTable.Add(ItemManagerIndex, CrystalElementType);
	}
	CrystalType CrystalElementType = CrystalType::None;
	int ItemManagerIndex = -1;
	static TMap<int, CrystalType> TypeLookupTable;
	virtual AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) = 0;
};

class REMNANTS_API FireElement : public AbstractCrystalElement
{
public:
	FireElement() : AbstractCrystalElement(CrystalType::Fire, 4) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};

class REMNANTS_API WaterElement : public AbstractCrystalElement
{
public:
	WaterElement() : AbstractCrystalElement(CrystalType::Water, 5) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};

class REMNANTS_API AirElement : public AbstractCrystalElement
{
public:
	AirElement() : AbstractCrystalElement(CrystalType::Air, 7) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};

class REMNANTS_API EarthElement : public AbstractCrystalElement
{
public:
	EarthElement() : AbstractCrystalElement(CrystalType::Earth, 6) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};

class REMNANTS_API SteamElement : public AbstractCrystalElement
{
public:
	SteamElement() : AbstractCrystalElement(CrystalType::Steam, 11) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};

class REMNANTS_API LavaElement : public AbstractCrystalElement
{
public:
	LavaElement() : AbstractCrystalElement(CrystalType::Lava, 10) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};

class REMNANTS_API AcidElement : public AbstractCrystalElement
{
public:
	AcidElement() : AbstractCrystalElement(CrystalType::Acid, 9) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};

class REMNANTS_API IceElement : public AbstractCrystalElement
{
public:
	IceElement() : AbstractCrystalElement(CrystalType::Ice, 13) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};

class REMNANTS_API LightningElement : public AbstractCrystalElement
{
public:
	LightningElement() : AbstractCrystalElement(CrystalType::Lightning, 8) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};

class REMNANTS_API CrystalElement : public AbstractCrystalElement
{
public:
	CrystalElement() : AbstractCrystalElement(CrystalType::Crystal, 12) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};

class REMNANTS_API NullElement : public AbstractCrystalElement
{
public:
	NullElement() : AbstractCrystalElement(CrystalType::None, 3) {}
	AbstractCrystalElement* CombineWith(AbstractCrystalElement* otherElement) override;
};