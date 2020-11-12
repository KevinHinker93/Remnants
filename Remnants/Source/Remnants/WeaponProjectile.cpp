// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AWeaponProjectile::AWeaponProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_rootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = m_rootComponent;
	m_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	//m_mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	m_mesh->SetupAttachment(RootComponent);

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	//CollisionComp->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
	CollisionComp->SetupAttachment(RootComponent);

	m_projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	m_projectileMovement->UpdatedComponent = CollisionComp;
	m_projectileMovement->InitialSpeed = 3000.f;
	m_projectileMovement->MaxSpeed = 3000.f;
	m_projectileMovement->bRotationFollowsVelocity = true;
	m_projectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::SpawnEmitterAttached(m_particle, Cast<USceneComponent>(FindComponentByClass(UStaticMeshComponent::StaticClass())));
}

// Called every frame
void AWeaponProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

