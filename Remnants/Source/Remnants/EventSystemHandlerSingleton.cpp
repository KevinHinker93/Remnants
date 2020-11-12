// Fill out your copyright notice in the Description page of Project Settings.

#include "EventSystemHandlerSingleton.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

UEventSystemHandlerSingleton::UEventSystemHandlerSingleton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//m_text = "SingletonBitches";
}

UEventSystemHandlerSingleton* UEventSystemHandlerSingleton::GetEventSystemHandler(bool& IsValid)
{
	IsValid = false;

	UEventSystemHandlerSingleton* instance = Cast<UEventSystemHandlerSingleton>(GEngine->GameSingleton);

	if (!instance) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get singleton instance"));
		return NULL;
	}
	if (!instance->IsValidLowLevel()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("instance is not valid lowLevel"));
		return NULL;
	}

	IsValid = true;
	return instance;
}

UEventFacade* UEventSystemHandlerSingleton::GetEventFacade()
{
	if (!m_eventFacade)
	{
		m_eventFacade = NewObject<UEventFacade>();
		m_eventFacade->SetEventHandler(this);
	}
	return m_eventFacade;
}
