// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventSystemHandlerSingleton.h"
#include "Components/ProjectileBehaviour/FloatingBubbleComponent.h"
#include "ConnectingProjectileComponent.generated.h"

class UProjectileMovementComponent;
class AController;
class AShootergameBeam;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UConnectingProjectileComponent : public UFloatingBubbleComponent
{
	GENERATED_BODY()
	
public:
	virtual void InitBehaviour(float damage, FName shooterType, int moduleType, AActor* projectileSpawner, 
			TSubclassOf<UElementDamageType> damageTypeClass, class UProjectileMovementComponent* projectileMovement, float range) override;
	virtual void OnHit(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator) override;
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
			AController* EventInstigator) override;
	virtual void EndCollision() override;
	virtual void OnRangeEnd(AActor* shooter, float moduleCriticalChance, float moduleCriticalDamageMult, int elementLvL,
			float explosionRadiusMult) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		TSubclassOf<AShootergameBeam> m_connectingBeamClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
		class UParticleSystem* m_onHitExplosion;
	// How long particle lives after it has been connected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_connectedTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_connectingRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_minConnectionDistance;
	// Tags the projectile can connect to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		TArray<FName> m_connectingTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		int m_maxConnectionCount;

	void EndPlay(EEndPlayReason::Type endPlayReason);
	virtual void OnStartFloating() override;

	// Returns sorted array of actors in Range, nearest is index 0
	TArray<AActor*> GetActorsInRange();

	UFUNCTION()
		void OnConnectRequest(AActor* sender, AActor* receiver, float distance);
	UFUNCTION()
		void OnConnectReply(AActor* receiver, bool canConnect, float connectDuration, int connectionsLeft);
	UFUNCTION()
		void OnNotifyProjectileConnecter(AActor* projectileConnecter, float lifeTime, int connectionsLeft);
	UFUNCTION()
		void OnNotifyConnectedProjectile(AActor* connectedProjectile, float lifeTime, int connectionsLeft);

private:
	UPROPERTY()
		AActor* m_connectedActor;
	UPROPERTY()
		AShootergameBeam* m_beam;
	UPROPERTY()
		int m_connectionLeft;
	UPROPERTY()
		UEventSystemHandlerSingleton* m_eventSystem;
	UFUNCTION()
	void UpdateProjectileBeamParams(float lifeTime, int connectionsleft);
};
