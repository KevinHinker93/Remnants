// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Gameplay/DamageArea.h"
#include "StormDamageArea.generated.h"

class UPrimitiveComponent;

UCLASS()
class REMNANTS_API AStormDamageArea : public ADamageArea
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storm Area")
		bool m_blocksEnemyProjectiles;

	/*UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);*/

	virtual void BeginPlay() override;

private:
	// Which collision channels can be blocked by this area
	// Idx 0 = Enemy objectType
	// Idx 1 = Player objectType
	UPROPERTY(EditAnywhere, Category = "Storm Area", meta = (AllowPrivateAccess = "true"))
		TArray<TEnumAsByte<ECollisionChannel>> m_blockedTypeChannels;
};
