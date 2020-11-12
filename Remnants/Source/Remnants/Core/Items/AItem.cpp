// Fill out your copyright notice in the Description page of Project Settings.

#include "AItem.h"
#include "Kismet/KismetMathLibrary.h"
//#include "ItemManager.h"

UAItem::UAItem()
{ 
	
}

int UAItem::GetId()
{
	if (!m_idSet)
		UE_LOG(LogTemp, Error, TEXT("Id not set. Always create items via ItemManager!"));
	return m_itemId;
}

void UAItem::SetId(int id)
{
	if (!m_idSet)
	{
		m_itemId = id;
		m_idSet = true;
	}
}

AActor* UAItem::GetOwner()
{
	return m_owner;
}

void UAItem::SetOwner(AActor* owner)
{
	m_owner = owner;
}

int UAItem::GetAmount()
{
	return m_currentAmount;
}

int UAItem::GetMaxAmount()
{
	return m_maxAmount;
}

int UAItem::AddAmount(int amount)
{
	int freeSpace = m_maxAmount - m_currentAmount;
	int addedInstances = UKismetMathLibrary::Clamp(amount, 0, freeSpace);
	int remainder = amount - addedInstances;

	m_currentAmount += addedInstances;

	return remainder;
}

int UAItem::RemoveAmount(int amount)
{
	int removedInstances = UKismetMathLibrary::Clamp(amount, 0, m_currentAmount);
	int remainder = amount - removedInstances;

	m_currentAmount -= removedInstances;

	return remainder;
}

void UAItem::SetMaxAmount(int size)
{
	m_maxAmount = size;
}

//FArchive& operator<<(FArchive& a, UAItem i)
//{
//	a << i.m_itemId;
//	a << i.m_currentAmount;
//	a << i.m_maxAmount;
//
//	UE_LOG(LogTemp, Display, TEXT("Serialized Item"));
//}