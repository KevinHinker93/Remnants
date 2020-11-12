// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AEnemyDrone.h"
#include "BossDrone.generated.h"

class UShieldComponent;
class UElementDamageType;

UCLASS()
class REMNANTS_API ABossDrone : public AAEnemyDrone
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AbsorbDamage"))
		void AbsorbDamage();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DamageElementShield"))
		void DamageElementShield();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnAnyBossDamage"))
		void OnAnyBossDamage(UShieldComponent* damagedShield, UShieldComponent* shieldComponentWidgetRef, float damage);
	UFUNCTION(BlueprintCallable, Category="RemoveElementShieldFromMap")
		void RemoveElementShieldFromMap(FName elementName);
	UFUNCTION(BlueprintCallable, Category = "AddBrokenShield")
		void AddBrokenShield(FName elementName);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "DamageHealth"))
		void DamageHealth();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ActivateBoss"))
		void ActivateBoss();

	// Interface function: IKillAble
	virtual void Kill() override;

	// Call this function to damage this actor
	// Use the damage event to gain information about the incoming damage, for ex. type
	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= BossDrone)
		TMap<FName, UShieldComponent*> m_elementShields;
	// Is drone activated and can attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BossDrone)
		bool m_bossActivated;
	// How many element shields boss should have
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossDrone)
		int m_requiredElementShieldCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossDrone)
		int m_droppedItemAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossDrone)
		TArray<int> m_directReceivedItemIdsAfterDeath;

	UFUNCTION()
		void ActivateBossWithBrokenShields(const TSet<FString> brokenShields);

	UFUNCTION()
		void OnKeyHolderKill(int keyItemId);

private:
	TSet<FName> m_brokenShields;
	bool IsAbsorbingDamage(UElementDamageType* damageType);
	bool IsDamagingBossHealth(UElementDamageType* damageType, float damage);
	UShieldComponent* GetElementalShieldToDamage(FName elementName);
	void DamageElementalShield(float damage, UElementDamageType* damageType, AActor* DamageCauser);
	float CalcActualDamage(UElementDamageType* damageType, float initialDamage);
};
