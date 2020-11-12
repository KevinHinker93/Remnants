// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/FloatingBubbleComponent.h"
#include "../../WeaponFiringStrategies/HomingShotgunStrategy.h"
#include "../../Core/Items/ShootergameProjectile.h"
#include "TimerManager.h"
#include "HomingGravitonComponent.generated.h"

USTRUCT()
struct FGravitonTimerData
{
	GENERATED_USTRUCT_BODY()
		
public:
	FGravitonTimerData() {}

	FTimerHandle m_handle;
	FTimerDelegate m_timerDelegate;
};

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UHomingGravitonComponent : public UFloatingBubbleComponent
{
	GENERATED_BODY()

public:
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo, AController* EventInstigator);
	virtual void EndCollision();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing Settings")
		float m_homingSpeedDiv;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing Settings")
		float m_homingMagnitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings")
		TSubclassOf<UHomingShotgunStrategy> m_gravitonShotFiringStrat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings")
		TSubclassOf<AShootergameProjectile> m_gravitonAlternativeProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings")
		USoundBase* m_gravitonProjSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings")
		float m_gravitonRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings")
		int m_gravitonNumProjectiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings")
		float m_gravitonShotSizeMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings")
		float m_gravitonInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings")
		float m_gravitonShotDamageMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings", meta = (UIMin = "0.2", UIMax = "2.5"))
		float m_maxRandomShotTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings", meta = (UIMin = "0.05", UIMax = "0.1"))
		float m_minRandomShotTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graviton Settings")
		float m_gravitonRotOffset;

	virtual void OnStartFloating() override;

	TArray<AActor*> GetActorsInRange();
	bool CheckTargetIsBlocked(AActor* target, AActor* lastHitActor);

private:
	FTimerHandle m_gravitonIntervalHandle;

	UPROPERTY()
		TArray<FGravitonTimerData> m_timerData;

	UFUNCTION()
		void OnGraviton();
	UFUNCTION()
		void OnShootGravitonOnEnemy(AActor* actorToShootTo, int insertedIdx);
};
