// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponProjectile.generated.h"

UCLASS()
class REMNANTS_API AWeaponProjectile : public AActor
{
	GENERATED_BODY()
	
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* m_projectileMovement;

public:	
	// Sets default values for this actor's properties
	AWeaponProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float m_damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float m_speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float m_size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float m_density;

	/** Sound to play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* m_sound;

	UPROPERTY(EditAnywhere)
	class USceneComponent* m_rootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* m_mesh;

	/** effect played on respawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	class UParticleSystem* m_particle;

	//particle system missing

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
