// Fill out your copyright notice in the Description page of Project Settings.

#include "EmptyActor.h"


// Sets default values
AEmptyActor::AEmptyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_transform = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	m_transform->SetMobility(EComponentMobility::Static);
	SetRootComponent(m_transform);
}
