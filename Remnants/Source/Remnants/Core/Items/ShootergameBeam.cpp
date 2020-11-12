// Fill out your copyright notice in the Description page of Project Settings.

#include "ShootergameBeam.h"
#include "Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Components/SceneComponent.h"
#include "ElementDamageType.h"
#include "Kismet/GameplayStatics.h"
#include "../../Components/BeamBehavior/BeamBehaviorComponentBase.h"
#include "Core/Characters/AEnemyDrone.h"

// Sets default values
AShootergameBeam::AShootergameBeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Location"));
	RootComponent = m_sceneComponent;
	m_particleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System"));
	m_particleComponent->SetupAttachment(RootComponent);

	// Use a sphere as a simple collision representation
	m_boxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	m_boxCollider->BodyInstance.SetCollisionProfileName("Projectile");
	m_boxCollider->OnComponentBeginOverlap.AddDynamic(this, &AShootergameBeam::OnOverlapBegin);
	m_boxCollider->OnComponentEndOverlap.AddDynamic(this, &AShootergameBeam::OnOverlapEnd);
	m_boxCollider->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	m_boxCollider->CanCharacterStepUpOn = ECB_No;
	m_boxCollider->SetupAttachment(m_particleComponent);

	m_canDamage = true;

	m_collisionPostFix = TEXT("Beam");
}

void AShootergameBeam::InitBeam(float damage, FName shooterType, int moduleType,
			AActor* projectileSpawner,
			TSubclassOf<class UElementDamageType> damageTypeClass)
{
	m_damage = damage;
	m_damageTypeClass = damageTypeClass;

	m_shooterType = shooterType;

	if (projectileSpawner)
	{
		m_projectileSpawner = projectileSpawner;
		UE_LOG(LogTemp, Verbose, TEXT("Projectile Spawner: %s"), *m_projectileSpawner->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No projectile Spawner"));
	}

	// Element Behaviour Proto -----------------------------------------------------
	m_firedModuleType = moduleType;
	TArray<UActorComponent*> comps;
	comps = GetComponentsByClass(UBeamBehaviorComponentBase::StaticClass());
	checkf(comps.Num() == 0 || comps.Num() > 1, TEXT("Too less projectile behaviour components (%d) assigned to projectile, must be more than one or zero, have you forgott to add some?"), comps.Num());
	if(comps.Num() > 0 && m_firedModuleType >= 0)
		m_beamBehaviour = Cast<UBeamBehaviorComponentBase>(comps[m_firedModuleType]);

	// Change collsion preset
	FString profileName = m_shooterType.ToString() + m_collisionPostFix.ToString(); // Shootertype is typically tag of shooter
	FName collisionProfileName = FName(*profileName);
	m_boxCollider->BodyInstance.SetCollisionProfileName(collisionProfileName);
	// Element Behaviour Proto End -----------------------------------------------------

	if (m_beamBehaviour)
	{
		m_beamBehaviour->InitBehaviour(m_damage, m_projectileSpawner, m_damageTypeClass, moduleType);
		OnBeamInit();
		UE_LOG(LogTemp, VeryVerbose, TEXT("Init proj behaviour"));
	}
}


void AShootergameBeam::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (m_beamBehaviour)
	{
		m_beamBehaviour->DamageEnemy(OtherActor, Hit.ImpactNormal, Hit, NULL, m_projectileSpawner, m_damageTypeClass);
		UE_LOG(LogTemp, Display, TEXT("Beam overlap"));
	}
}

void AShootergameBeam::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (m_beamBehaviour)
	{
		m_beamBehaviour->OnOverlapEnd();
		UE_LOG(LogTemp, Display, TEXT("Beam overlapEnd"));
	}
}

AActor* AShootergameBeam::GetSpawner()
{
	return m_projectileSpawner;
}

void AShootergameBeam::BeginPlay()
{
	Super::BeginPlay();
}

void AShootergameBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShootergameBeam::EndPlay(EEndPlayReason::Type endPlayReason)
{
	if (m_beamBehaviour)
		m_beamBehaviour->OnDeath();
}

void AShootergameBeam::OnHitExplode(FVector explosionLoc)
{
	auto particle = UGameplayStatics::SpawnEmitterAtLocation(this, m_onHitExplosion, explosionLoc);
	particle->SetWorldScale3D(m_explosionSizeMult * FVector::OneVector);
}

void AShootergameBeam::Kill()
{
	if (m_beamBehaviour)
		m_beamBehaviour->OnDeath();
	EndBeamSound();
	Destroy();
}

void AShootergameBeam::ChangeParticleSystemSize(FVector size)
{
	m_particleComponent->SetVectorParameter("size", size);
}

void AShootergameBeam::SetBeamEndPos(FVector pos)
{
	m_particleComponent->SetVectorParameter("beamEnd", pos);
}

void AShootergameBeam::SetBeamBoxLength(float length)
{
	FVector boxExtent = m_boxCollider->GetScaledBoxExtent();
	m_boxCollider->SetBoxExtent(FVector(length / 2, boxExtent.Y, boxExtent.Z));
	m_boxCollider->SetRelativeLocation(FVector(length / 2, 0, 0));
}

void AShootergameBeam::UpdateBeamCollider()
{
	m_boxCollider->SetBoxExtent(FVector(m_currentBeamLenght / 2, m_beamCollWidthHeight, m_beamCollWidthHeight));
	m_boxCollider->SetRelativeLocation(FVector(m_currentBeamLenght / 2, 0, 0));
}

void AShootergameBeam::SetBeamLenght(float lenght)
{
	m_currentBeamLenght = lenght;
	UE_LOG(LogTemp, Verbose, TEXT("beamLength:: %f"), m_currentBeamLenght);
	m_particleComponent->SetFloatParameter("BeamLength", m_currentBeamLenght);
	UpdateBeamCollider();
}