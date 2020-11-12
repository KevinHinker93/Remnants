// Fill out your copyright notice in the Description page of Project Settings.

#include "CraftingInternalLogic.h"

TMap<int, CrystalType> AbstractCrystalElement::TypeLookupTable;

#pragma region Fire
AbstractCrystalElement* FireElement::CombineWith(AbstractCrystalElement * otherElement)
{
	switch (otherElement->CrystalElementType)
	{
	case CrystalType::Fire:
		return new FireElement();
		break;
	case CrystalType::Water:
		return new SteamElement();
		break;
	case CrystalType::Earth:
		return new LavaElement();
		break;
	case CrystalType::Air:
		return new LightningElement();
		break;
	}
	return new NullElement();
}

#pragma endregion
#pragma region Water
AbstractCrystalElement* WaterElement::CombineWith(AbstractCrystalElement * otherElement)
{
	switch (otherElement->CrystalElementType)
	{
	case CrystalType::Fire:
		return new SteamElement();
		break;
	case CrystalType::Water:
		return new WaterElement();
		break;
	case CrystalType::Earth:
		return new AcidElement();
		break;
	case CrystalType::Air:
		return new IceElement();
		break;
	}
	return new NullElement();
}
#pragma endregion
#pragma region Earth
AbstractCrystalElement* EarthElement::CombineWith(AbstractCrystalElement * otherElement)
{
	switch (otherElement->CrystalElementType)
	{
	case CrystalType::Fire:
		return new LavaElement();
		break;
	case CrystalType::Water:
		return new AcidElement();
		break;
	case CrystalType::Earth:
		return new EarthElement();
		break;
	case CrystalType::Air:
		return new CrystalElement();
		break;
	}
	return new NullElement();
}
#pragma endregion
#pragma region Air
AbstractCrystalElement* AirElement::CombineWith(AbstractCrystalElement * otherElement)
{
	switch (otherElement->CrystalElementType)
	{
	case CrystalType::Fire:
		return new LightningElement();
		break;
	case CrystalType::Water:
		return new IceElement();
		break;
	case CrystalType::Earth:
		return new CrystalElement();
		break;
	case CrystalType::Air:
		return new AirElement();
		break;
	}
	return new NullElement();
}
#pragma endregion
#pragma region Lava
AbstractCrystalElement* LavaElement::CombineWith(AbstractCrystalElement * otherElement)
{
	switch (otherElement->CrystalElementType)
	{
	case CrystalType::Lava:
		return new LavaElement();
		break;
	}
	return new NullElement();
}
#pragma endregion
#pragma region Steam
AbstractCrystalElement* SteamElement::CombineWith(AbstractCrystalElement * otherElement)
{
	switch (otherElement->CrystalElementType)
	{
	case CrystalType::Steam:
		return new SteamElement();
		break;
	}
	return new NullElement();
}
#pragma endregion
#pragma region Acid
AbstractCrystalElement* AcidElement::CombineWith(AbstractCrystalElement * otherElement)
{
	switch (otherElement->CrystalElementType)
	{
	case CrystalType::Acid:
		return new AcidElement();
		break;
	}
	return new NullElement();
}
#pragma endregion
#pragma region Ice
AbstractCrystalElement* IceElement::CombineWith(AbstractCrystalElement * otherElement)
{
	switch (otherElement->CrystalElementType)
	{
	case CrystalType::Ice:
		return new IceElement();
		break;
	}
	return new NullElement();
}
#pragma endregion
#pragma region Lightning
AbstractCrystalElement* LightningElement::CombineWith(AbstractCrystalElement * otherElement)
{
	switch (otherElement->CrystalElementType)
	{
	case CrystalType::Lightning:
		return new LightningElement();
		break;
	}
	return new NullElement();
}
#pragma endregion
#pragma region Crystal
AbstractCrystalElement* CrystalElement::CombineWith(AbstractCrystalElement * otherElement)
{
	switch (otherElement->CrystalElementType)
	{
	case CrystalType::Crystal:
		return new CrystalElement();
		break;
	}
	return new NullElement();
}
#pragma endregion

AbstractCrystalElement* NullElement::CombineWith(AbstractCrystalElement * otherElement)
{
	return new NullElement();
}