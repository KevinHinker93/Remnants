// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShootergameProjectile.generated.h"

UCLASS(config=Game)
class AShootergameProjectile : public AActor
{
	GENERATED_BODY()

public:
	AShootergameProjectile();
	// called when projectile hits something
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Blueprintcallable, Category = "ProjectileBehaviorEndCollision")
		void ProjectileBehaviorEndCollision(AActor* otherActor);

	// Returns CollisionComp subobject
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	// Returns ProjectileMovement subobject
	UFUNCTION(BlueprintCallable, Category = "Projectile")
		FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UFUNCTION(BlueprintCallable, Category = "Projectile")
		float GetProjectileBaseDamage() const { return m_damage; }

	UFUNCTION(BlueprintCallable, Category = "Projectile")
		TSubclassOf<class UElementDamageType> GetProjectileDamageType() const { return m_damageTypeClass; }

	UFUNCTION(BlueprintCallable, Category = "Projectile")
		void SetProjectileBaseDamage(float newDamage);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
		float GetProjectileMaxLifeTime() const { return m_maxLifeTime; }

	// Shooter type will define where the projectile belongs to and set collision, module type defines from what type of weapon: long range or short range it was shot
	void InitProjectile(float damage, float criticalChance, float criticalDamageMult, float speed, float size, float explosionRadius, float range, FName shooterType, int moduleType,
			AActor* projectileSpawner,
			TSubclassOf<class UElementDamageType> damageTypeClass);

	/*void OnDestroy(FVector explosionLoc);*/
	void OnExplode(FVector explosionLoc, bool destroy = false, float explosionSize = 1.0f);
	// Calculates total damage by checking critical chance
	float GetProjectileSize();

	float CalcTotalDamage(float initialDamage);

	AActor* GetProjectileSpawner();
	const FName GetShooterType() const { return m_shooterType; }

	void GetOverlappingActors(TArray<AActor*>& overlappingActors);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UStaticMeshComponent* m_mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
		class UParticleSystem* m_explosion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* m_sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Properties")
		float m_damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Properties")
		float m_criticalChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Properties")
		float m_criticalDamageMult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Properties")
		float m_BaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Properties")
		float m_speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Properties")
		float m_size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Properties")
		float m_density;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Properties")
		float m_explosionRadius;

	UPROPERTY()
		AActor* m_projectileSpawner;
	UPROPERTY()
		TSubclassOf<class UElementDamageType> m_damageTypeClass;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type endPlayReason) override;

private:
	// Sphere collision component
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		class USphereComponent* CollisionComp;

	// Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particle", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* m_particleComponent;

	UPROPERTY()
		class UProjectileBehaviourComponentBase* m_projectileBehaviour;

	// Called when beam overlaps something
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	// Called when beam stops overlaping an object
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FCriticalSection m_criticalSection;
	FName m_collisionPostFix;
	// Who shot the projectile
	FName m_shooterType; 
	int m_firedModuleType; // Idx determining from which module type (long or short range) the projectile was shot
	bool m_exploded;
	float m_maxLifeTime;
};

