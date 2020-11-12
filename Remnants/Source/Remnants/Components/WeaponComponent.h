// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "WeaponComponent.generated.h"

class UAModule;
class UAElement;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, UWeaponComponent*, Sender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartOverheat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndOverheat);

UENUM(BlueprintType)
enum class WeaponStates : uint8
{
	Default 	UMETA(DisplayName = "Default"),
	Frozen 	UMETA(DisplayName = "Frozen"),
};

USTRUCT(BlueprintType)
struct FElementIdSet
{
	GENERATED_USTRUCT_BODY()

public:
	FElementIdSet()
	{ }

	UPROPERTY(EditAnywhere)
		TArray<int> m_elementIds;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REMNANTS_API UWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();
	
	// Starts shooting
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void BeginShooting(int weaponType);

	// Tells module to stop firing
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFiring(int weaponType);

	UPROPERTY(BlueprintAssignable, Category= "Overheat")
		FOnStartOverheat OnStartOverheat;
	UPROPERTY(BlueprintAssignable, Category= "Overheat")
		FOnEndOverheat OnEndOverheat;

	// Set module at primary or secondary slot
	void SetModuleAtSlotIdx(int idx, UAModule* module);

	//void SetElement(int moduleIdx, int elementIdx,)
	//void SetElementSetAtSlotIdx(int idx, FElementSet elementSet, bool autoEquip = false);

	// Changes ElementSet of Weapon, Positive Value means next Set, Negative is previous Set
	UFUNCTION(BlueprintCallable)
		void ChangeElementSet(float val);
	UFUNCTION(BlueprintCallable)
		void SetElementSet(int val);
	UFUNCTION(BlueprintCallable)
		void SwapElements();

	// requests equipment
	void RequestEquipment();
	bool HasReceivedEquipment();

	// setup equipment ids (before first tick)
	void SetModuleIds(TArray<int> moduleIds);
	void SetElementIdSets(TArray<FElementIdSet> elementIdSets);
	void SetDefaultElementId(int defaultElementId);

	UFUNCTION(BlueprintCallable, Category = "WeaponEquip")
		void SetActorStatsComponent(class UActorStatsComponent* actorStatsComponent);
	UFUNCTION(BlueprintCallable, Category = "WeaponEquip")
		void UpdateStats();

	float GetFiringSpeedMult() const;
	void SetFiringSpeedMult(float firingSpeedMult);

	float GetOverheatingMult() const;
	void SetOverheatingMult(float overheatingMult);

	float GetDamageMult() const;
	void SetDamageMult(float damageMult);

	void SetWeaponState(WeaponStates weaponState);

	// equipment getters/setters
	UFUNCTION(BlueprintPure, Category = "WeaponEquip")
		UAModule* GetModule(int moduleIdx);
	UFUNCTION(BlueprintPure, Category = "WeaponEquip")
		UAElement* GetElement(int elementSetIdx, int moduleIdx);
	UFUNCTION(BlueprintCallable, Category = "WeaponEquip")
		UAModule* SetModule(int moduleIdx, UAModule* module);
	UFUNCTION(BlueprintCallable, Category = "WeaponEquip")
		UAElement* SetElement(int elementSetIdx, int moduleIdx, UAElement* element);
	UFUNCTION(BlueprintPure, Category = "WeaponEquip")
		int GetModuleCount();
	UFUNCTION(BlueprintPure, Category = "WeaponEquip")
		int GetElementCount();
	UFUNCTION(BlueprintPure, Category = "WeaponEquip")
		int GetElementSetCount();
	UFUNCTION(BlueprintPure)
		int GetActiveElementSetIdx() { return m_activeElementSetIdx; }

	UFUNCTION()
		void OnExternalOverheat(AActor* actorToOverheat, float overheatingMult, int sourceModule);
	UFUNCTION()
		void OnExternalEndFiring(AActor* actorToEndFire, int sourceModule);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Weapon Event")
		FOnWeaponChanged OnWeaponChanged;

	// For negative status effect (frozen)
	void ResetWeaponState(); 

	void ResetOverheat();

	UFUNCTION(BlueprintCallable)
		void SetCanFire(bool canFire);
	UFUNCTION()
		void OnSceneChangeStarted(const FString& currentScene, const FString& sceneToLoad);
	UFUNCTION()
		void OnSceneChangeFinished(const FString& previousScene, const FString& loadedScene);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FTimerHandle GetOverheatTimer() { return m_overheatingTimerHandle; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsOverheated() { return m_overheated; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetOverheatedPercentage() { return m_overheatedPercentage; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetCurrentOverheatedPercentage() { return m_currentOverheatVal / m_maxOverheatingVal; }
	
protected:
	// these variables are set in the BP
	UPROPERTY(EditAnywhere, Category = "Equipment") // module IDs
		TArray<int> m_moduleIds;
	UPROPERTY(EditAnywhere, Category = "Equipment") // element sets
		TArray<FElementIdSet> m_elementIdSets;
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		int m_defaultElementId;

	// these are the actual objects
	UPROPERTY()
		TArray<UAModule*> m_modules; // module objects
	UPROPERTY()
		TArray<UAElement*> m_elements; // element objects (1d array)
	UPROPERTY()
		UAElement* m_defaultElement; // default element object

	UFUNCTION()
		void ReceiveItemChunk(UObject* sender, TArray<UAItem*>& items);

	// stats:
	int m_baseDamageVal;
	float m_damageMult;

	float m_firingSpeed;
	float m_firingSpeedMult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float m_maxOverheatingVal;

	float m_overheatedPercentage; // Threshold: weapon is overheated

	float m_overheatingImpact; // Base overheating value that will be added per shot
	float m_overheatingMult;

	float m_coolAmount;

	float m_coolSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float m_currentOverheatVal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int m_activeElementSetIdx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int m_currentModuleIdx;

	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type endPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	UPROPERTY()
		class UActorStatsComponent* m_actorStatsComponent;

	// Overheating regeneration interval handle
	FTimerHandle m_overheatingTimerHandle;

	WeaponStates m_currentWeaponState;
	int m_activeModuleIdx;

	bool m_canFire;
	bool m_firing;
	bool m_canCooldown;
	bool m_overheated;

	bool m_requestedEquipment = false;
	bool m_receivedEquipment = false;

	// Add value to current overheating value, can be negative
	void UpdateOverheat(float value);
	// Cools the weapon and disable cooldown
	void CoolDownWeapon();
	// Enables cooldown
	void EnableCooldown();
};
