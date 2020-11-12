// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Structural/WeightedRandom.h"
#include "KillAble.h"
#include "ItemCollection.h"
#include "LootCrate.generated.h"

UCLASS()
class REMNANTS_API ALootCrate : public AActor, public IKillAble
{
	GENERATED_BODY()
	
public:	
	ALootCrate();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnLootableObjectDestroy"))
		void OnLootableObjectDestroy();
	// Call this function to damage this actor
	// Use the damage event to gain information about the incoming damage, for ex. type
	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		class UBoxComponent* m_collider;
	UPROPERTY(EditAnywhere)
		class UParticleSystem* m_deathParticleSystem;

	UPROPERTY(EditAnywhere, Category="Stats")
		int m_itemCountMin;
	UPROPERTY(EditAnywhere, Category="Stats")
		int m_itemCountMax;
	UPROPERTY(EditAnywhere, Category="Stats")
		TArray<int> m_possibleItemIds;
	UPROPERTY(EditAnywhere, Category="Stats")
		TArray<float> m_itemProbabilities;
	UPROPERTY(EditAnywhere, Category="Stats")
		float m_itemSpread;
	UPROPERTY(EditAnywhere, Category = "Save Options")
		bool m_save;

	WeightedRandom m_weightedRandom;

	UPROPERTY()
		UItemCollection* m_items;

	void GenerateInventory();
	void ReleaseInventory();
	void GenerateItem();
	//void OnPickupSpawned(AActor* sender, APickUp* pickup);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION()
		void OnCrateDestructionRequest(FString crateName);


	void Kill();

private:
	FCriticalSection m_criticalSection;

};
