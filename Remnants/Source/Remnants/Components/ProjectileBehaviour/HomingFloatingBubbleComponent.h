// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProjectileBehaviour/FloatingBubbleComponent.h"
#include "HomingFloatingBubbleComponent.generated.h"

class USphereComponent;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REMNANTS_API UHomingFloatingBubbleComponent : public UFloatingBubbleComponent
{
	GENERATED_BODY()
	
public:
	virtual void DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
		AController* EventInstigator) override;
	virtual void EndCollision() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_homingSpeedDiv;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_homingCheckRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour Settings")
		float m_homingMagnitude; // Homing adjust parameter, higher values for faster turn of projectile

	virtual void OnStartFloating() override;

private:
	UPROPERTY()
		USphereComponent* m_homingCheckColl;

	UFUNCTION()
		void OnHomingOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
		void OnStartHoming(AActor* actorToHomeTo);
	
};
