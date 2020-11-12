// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ShootergameProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "ElementDamageType.h"
#include "Components/ProjectileBehaviour/ProjectileBehaviourComponentBase.h"
#include "Engine/World.h"

AShootergameProjectile::AShootergameProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AShootergameProjectile::OnHit);		// set up a notification for when this component hits something blocking
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AShootergameProjectile::OnOverlapBegin);		// set up a notification for when this component hits something blocking
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	// Set as root component
	RootComponent = CollisionComp;

	m_particleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System"));
	m_particleComponent->SetupAttachment(RootComponent);

	m_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	m_mesh->SetupAttachment(RootComponent);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	m_BaseSpeed = ProjectileMovement->InitialSpeed;
	//// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	m_collisionPostFix = TEXT("Projectile");
	m_exploded = false;
	m_maxLifeTime = 0.0f;
}

void AShootergameProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	TSubclassOf<UElementDamageType> damType = m_damageTypeClass;
	AActor* damageCauser = m_projectileSpawner;
	//AController* controller = damageCauser->GetInstigatorController();

	if(m_projectileBehaviour)
			m_projectileBehaviour->OnHit(OtherActor, FVector::ZeroVector, Hit, nullptr);
	else
	{
		FVector spawnLoc = Hit.ImpactPoint;
		OnExplode(spawnLoc, true, m_size);
	}	
}

void AShootergameProjectile::ProjectileBehaviorEndCollision(AActor* otherActor)
{
	if (m_projectileBehaviour && otherActor->ActorHasTag("Enemy"))
		m_projectileBehaviour->EndCollision();
}

void AShootergameProjectile::SetProjectileBaseDamage(float newDamage)
{
	if (m_projectileBehaviour)
		m_projectileBehaviour->SetDamage(newDamage);
	else
		m_damage = newDamage;
}

void AShootergameProjectile::InitProjectile(float damage, float criticalChance, float criticalDamageMult, float speed, float size, float explosionRadius, float range, FName shooterType, int moduleType,
	AActor* projectileSpawner,
	TSubclassOf<class UElementDamageType> damageTypeClass)
{
	m_damage = damage;
	m_criticalChance = criticalChance;
	m_criticalDamageMult = criticalDamageMult;
	m_damageTypeClass = damageTypeClass;
	m_size = size;
	RootComponent->SetWorldScale3D(FVector(m_size, m_size, m_size));
	ProjectileMovement->InitialSpeed *= speed;
	ProjectileMovement->MaxSpeed *= speed;
	m_explosionRadius = explosionRadius;
	m_shooterType = shooterType;

	float rangeInCentimeter = 100.0f * range;
	m_maxLifeTime = range / (ProjectileMovement->InitialSpeed / 100);
	SetLifeSpan(m_maxLifeTime);

	if (projectileSpawner)
	{
		m_projectileSpawner = projectileSpawner;
		UE_LOG(LogTemp, Verbose, TEXT("Projectile Spawner: %s"), *m_projectileSpawner->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No projectile Spawner"));
	}

	// Change collsion preset
	FString profileName = m_shooterType.ToString() + m_collisionPostFix.ToString(); // Shootertype is typically tag of shooter
	FName collisionProfileName = FName(*profileName);
	CollisionComp->BodyInstance.SetCollisionProfileName(collisionProfileName);

	// Element Behaviour Proto -----------------------------------------------------
	m_firedModuleType = moduleType;
	TArray<UActorComponent*> comps;
	comps = GetComponentsByClass(UProjectileBehaviourComponentBase::StaticClass());
	checkf(comps.Num() == 0 || comps.Num() > 1, TEXT("Too less projectile behaviour components (%d) assigned to projectile, must be more than one or zero, have you forgott to add some?"), comps.Num());
	if(comps.Num() > 0 && m_firedModuleType >= 0)
		m_projectileBehaviour = Cast<UProjectileBehaviourComponentBase>(comps[m_firedModuleType]);
	if (m_projectileBehaviour)
	{
		AActor* damageCauser = m_projectileSpawner;
		TSubclassOf<UElementDamageType> damType = m_damageTypeClass;

		m_projectileBehaviour->InitBehaviour(m_damage, m_shooterType, m_firedModuleType, damageCauser, damType, ProjectileMovement, range);
		UE_LOG(LogTemp, VeryVerbose, TEXT("Init proj behaviour"));
	}

	// Element Behaviour Proto End -----------------------------------------------------
}

// Called when the game starts or when spawned
void AShootergameProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AShootergameProjectile::EndPlay(EEndPlayReason::Type endPlayReason)
{
	if (m_projectileBehaviour)
		m_projectileBehaviour->OnRangeEnd(m_projectileSpawner, m_criticalChance, m_criticalDamageMult, 1, m_explosionRadius);
}

//void AShootergameProjectile::OnDestroy(FVector explosionLoc)
//{
//	auto particle = UGameplayStatics::SpawnEmitterAtLocation(this, m_explosion, explosionLoc);
//	UE_LOG(LogTemp, Warning, TEXT("Explode"));
//	Destroy();
//}

void AShootergameProjectile::OnExplode(FVector explosionLoc, bool destroy, float explosionSize)
{
	m_criticalSection.Lock();
	if (m_exploded)
		return;
	if(destroy)
		m_exploded = true;
	m_criticalSection.Unlock();

	auto particle = UGameplayStatics::SpawnEmitterAtLocation(this, m_explosion, explosionLoc);
	FVector particleSize;
	if (explosionSize != 1.0f)
	{
		FVector explSize = FVector(explosionSize, explosionSize, explosionSize);
		particle->SetVectorParameter("Size", explSize);
	}

	UE_LOG(LogTemp, VeryVerbose, TEXT("Projectile %s exploded"), *GetName());

	if(destroy)
		Destroy();
}

float AShootergameProjectile::GetProjectileSize()
{
	return m_size;
}

float AShootergameProjectile::CalcTotalDamage(float initialDamage)
{
	float totalDamage = initialDamage;
	//Critical Damage
	int randSeed = FMath::Rand();
	FMath::RandInit(randSeed * GetWorld()->RealTimeSeconds);
	float criticalPercentage = FMath::RandRange(0.0f, 1.0f);
	if (criticalPercentage <= m_criticalChance)
	{
		UE_LOG(LogTemp, Error, TEXT("Critical Strike:: value: %f // chance: %f"), criticalPercentage, m_criticalChance);
		totalDamage *= m_criticalDamageMult;
	}

	return totalDamage;
}

AActor* AShootergameProjectile::GetProjectileSpawner()
{
	return m_projectileSpawner;
}

void AShootergameProjectile::GetOverlappingActors(TArray<AActor*>& overlappingActors)
{
	CollisionComp->GetOverlappingActors(overlappingActors); 
}

void AShootergameProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	if ((OtherActor != NULL) && (OtherActor != this) && !OtherActor->IsPendingKill())
	{
		AActor* damageCauser = m_projectileSpawner;

		FVector spawnLoc = Hit.ImpactPoint;
		//OnExplode(spawnLoc, true);

		TSubclassOf<UElementDamageType> damType = m_damageTypeClass;
		
		//AController* controller = damageCauser->GetInstigatorController();

		//Critical Damage
		float totalDamage = CalcTotalDamage(m_damage);

		if (m_projectileBehaviour)
		{
			if(!OtherActor->ActorHasTag("Buffing"))
				m_projectileBehaviour->DamageEnemy(OtherActor, Hit.ImpactNormal, Hit, NULL);
		}
		else if(!OtherActor->ActorHasTag("Pulling"))
		{
			UGameplayStatics::ApplyPointDamage(OtherActor, totalDamage, Hit.ImpactNormal, Hit, NULL, damageCauser, damType); 
			OnExplode(spawnLoc, true, m_size);
		}
	}
	
}

void AShootergameProjectile::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (m_projectileBehaviour)
		m_projectileBehaviour->EndCollision();
}