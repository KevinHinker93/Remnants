// Fill out your copyright notice in the Description page of Project Settings.

#include "CraftingController.h"
#include "Core/Items/AElement.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

TMap<CrystalType, AbstractCrystalElement*> UCraftingController::Elements;

int UCraftingController::CraftCrystalFromCrystals(int primaryElementId, int secondaryElementId)
{
	GenerateElements();

	AbstractCrystalElement* primaryCraftingInput = Elements[AbstractCrystalElement::TypeLookupTable[primaryElementId]];
	AbstractCrystalElement* secondaryCraftingInput = Elements[AbstractCrystalElement::TypeLookupTable[secondaryElementId]];
	AbstractCrystalElement* OutputElement = primaryCraftingInput->CombineWith(secondaryCraftingInput);

	Elements.Empty();
	return OutputElement->ItemManagerIndex;
}

void UCraftingController::GenerateElements()
{
	Elements.Add(CrystalType::Acid, new AcidElement());
	Elements.Add(CrystalType::Air, new AirElement());
	Elements.Add(CrystalType::Crystal, new CrystalElement());
	Elements.Add(CrystalType::Earth, new EarthElement());
	Elements.Add(CrystalType::Fire, new FireElement());
	Elements.Add(CrystalType::Ice, new IceElement());
	Elements.Add(CrystalType::Lava, new LavaElement());
	Elements.Add(CrystalType::Lightning, new LightningElement());
	Elements.Add(CrystalType::Steam, new SteamElement());
	Elements.Add(CrystalType::Water, new WaterElement());
	Elements.Add(CrystalType::None, new NullElement());
}
