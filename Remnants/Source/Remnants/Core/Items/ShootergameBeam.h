// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillAble.h"
#include "ShootergameBeam.generated.h"

class UBoxComponent;
class UParticleSystemComponent;

UCLASS()
class REMNANTS_API AShootergameBeam : public AActor, public IKillAble
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShootergameBeam();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnEndBeam"))
		void OnEndBeam();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EndBeamSound"))
		void EndBeamSound();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "BeamInit"))
		void OnBeamInit();

	void InitBeam(float damage, FName shooterType, int moduleType,
			AActor* projectileSpawner,
			TSubclassOf<class UElementDamageType> damageTypeClass);

	void OnHitExplode(FVector explosionLoc);

	UFUNCTION(Blueprintcallable, Category = "Beam")
		virtual void Kill() override;

	// Returns CollisionComp subobject
	FORCEINLINE UBoxComponent* GetCollisionComp() const { return m_boxCollider; }
	void ChangeParticleSystemSize(FVector size);
	void SetBeamEndPos(FVector pos);
	void SetBeamBoxLength(float length);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
		UParticleSystemComponent* m_particleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
		class UParticleSystem* m_onHitExplosion;

	UPROPERTY(VisibleDefaultsOnly, Category = "Beam Collider")
		UBoxComponent* m_boxCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loc")
		class USceneComponent* m_sceneComponent;

	// Sound to play
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* m_sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion Properties")
		float m_explosionSizeMult;

	// ------------ Experimental
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp Properties")
		float m_maxBeamLenght;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp Properties")
		int m_maxEnemiesHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp Collider")
		float m_beamCollWidthHeight;

	float m_currentBeamLenght;
	bool m_canDamage;
	// ------------ Experimental

	UFUNCTION(Blueprintcallable, Category = "Beam")
		AActor* GetSpawner();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type endPlayReason) override;

private:
	UPROPERTY()
		AActor* m_projectileSpawner;
	UPROPERTY()
		TSubclassOf<class UElementDamageType> m_damageTypeClass;
	UPROPERTY()
		class UBeamBehaviorComponentBase* m_beamBehaviour;

	float m_damage;

	FName m_collisionPostFix;
	// Who shot the projectile
	FName m_shooterType; 
	int m_firedModuleType; // Idx determining from which module type (long or short range) the projectile was shot

	// Called when beam overlaps something
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	// Called when beam stops overlaping an object
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// ------------ Experimental
	void UpdateBeamCollider();
	void SetBeamLenght(float lenght);
	// ------------ Experimental
};
