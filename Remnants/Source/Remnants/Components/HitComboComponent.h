#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitComboComponent.generated.h"

class UHealthComponent;
class UShieldComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UHitComboComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHitComboComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Settings")
		float m_maxComboDamageMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Settings")
		int m_maxComboHitCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Settings")
		float m_maxComboDuration;
	// How much health can be depleted before combo ends (in %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Settings")
		float m_maxComboHealthLoss;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Settings")
		float m_comboHealthLossDifficultyChange;
	// Step size i.e. 0.25 when the combo difficulty lvl increases
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo Settings")
		float m_comboDifficultyStep;

	UFUNCTION()
		void InflictedDamage(float damage);

	void WasDamaged(float damage);
	void EndCombo();

	float GetCurrentDamageMultiplier() const { return m_currentDamageMultiplier; }
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UHealthComponent* m_healthCompOfOwner;
	UShieldComponent* m_shieldCompOfOwner;

	float m_currentAccumulatedDamage;
	int m_currentHitCount;
	float m_currentDamageMultiplier;
	float m_currentComboDuration;
	float m_currentComboHealthLoss;
	int m_currentDifficultyStep;
	float m_currentReceivedAccumulatedDamage;
	int m_currentDifficultyLvl;
	
	FTimerHandle m_comboTimerHandle;

	void ClearComboTimer();
};
