// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbleToShoot.h"
#include "KillAble.h"
#include "ElementStatusEffectVictim.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "../../Components/ElementAffinityComponent.h"
#include "ItemStruct.h"
#include "AEnemyDrone.generated.h"

class UAModule;
//class UElementAffinityComponent;

UCLASS()
class REMNANTS_API AAEnemyDrone : public ACharacter, public IAbleToShoot, public IElementStatusEffectVictim, public IKillAble
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAEnemyDrone();

	void RequestRandomEquipment();

	// Interface functions: IAbleToShoot
	virtual void WeaponFireAnimation() override;
	virtual class USceneComponent* GetMuzzelLoc() override;
	virtual float GetCurrentOverheatPercentage() const;

	// Interface functions: IElementStatusEffectVictim
	virtual void RemoveStatusEffect(ElementEffectType elementStatusEffectType) override;
	virtual int GetVictimMaxHealth() override;
	virtual int GetVictimMaxElementImpactVal() override;
	virtual float GetVictimElementStatusEffectDepleteAmount() override;
	virtual float GetVictimElementStatusEffectDepleteTime() override;
	virtual class UShieldComponent* GetVictimShieldComponent() override;
	virtual void SetFloatVal(EntityStats stat, float val) override;
	virtual void ResetFloatVal(EntityStats stat, float val) override;
	virtual void ChangeState(EntityState state) override;
	virtual void ResetState(EntityState state) override;

	// Interface function: IKillAble
	UFUNCTION(BlueprintCallable)
		virtual void Kill() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDroneKilled"))
		void OnDroneKilled();

	// Call this function to damage this actor
	// Use the damage event to gain information about the incoming damage, for ex. type
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void ChangeLevel(int level);
	void SetKeyHolder(int keyItemId);
	void SetPossibleElementIDs(TArray<int> possibleElementIDs);

	float PushPullResistance();

protected:
	// --- equipment
	UPROPERTY(EditAnywhere, Category = "Equipment")
		TArray<int> m_moduleIds;
	UPROPERTY(EditAnywhere, Category="Equipment")
		TArray<int> m_possibleElementIds; // TODO: Probability?
	UPROPERTY(EditAnywhere, Category = "Equipment")
		int m_elementSetCount; // TODO: Probability?
	UPROPERTY(EditAnywhere, Category = "Equipment")
		float m_dropItemChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
		float m_disassembleDroppedItemChance;
	// ---

	// Sphere collision component
	UPROPERTY(VisibleDefaultsOnly, Category = "Enemy Collision")
		class USphereComponent* m_collisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skeletal Mesh")
		class USkeletalMeshComponent* m_skeletalMesh;

	// Stats
	UPROPERTY(EditAnywhere, Category = "Stats")
		class UActorStatsComponent* m_actorStatsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		class UHealthComponent* m_healthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		class UShieldComponent* m_shieldComponent;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		class UWeaponComponent* m_weaponComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "ElementEffect")
		class UElementEffectComponent* m_elementEffectComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "States")
		class UCharacterStateComponent* m_stateComponent;

	UPROPERTY(EditAnywhere, Category = "Widget")
		class UWidgetComponent* m_widgetComponent;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
		FName m_enemyName;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
		float m_moveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
		float m_turnSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
		float m_droppedExpBase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
		float m_droppedExp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
		float m_droppedExpLevelMult;

	// Used as mass for faking physics, GetMass cannot be called when physics are not enabled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
		float m_pushPullResistance;

	float m_moveSpeedMult;
	FName m_tagOfLastDamageCauser;
	ElementEffectType m_lastHittedElement;

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type endPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	// Starts firing
	UFUNCTION(BlueprintCallable, Category = "Attack LongRange")
		void OnFirePrimaryPressed();
	// Stops firing
	UFUNCTION(BlueprintCallable, Category = "Stop attack long range")
		void OnFirePrimaryReleased();

	// Starts firing
	UFUNCTION(BlueprintCallable, Category = "Attack ShortRange")
	void OnFireSecondaryPressed();
	// Stops firing
	UFUNCTION(BlueprintCallable, Category = "Stop Attack Short Range")
	void OnFireSecondaryReleased();

	UFUNCTION()
		void OnKeyItemRequest(UObject* sender);
	UFUNCTION()
		void OnKeyHolderKill(int keyItemId);

	UFUNCTION()
		void OnBossKilled();

	void DamageShield(float damage, EDamageEffectiveness effectiveness);
	void DamageHealth(float damage, EDamageEffectiveness effectiveness);

	bool m_destroyOnKill = true;

	UPROPERTY(BlueprintReadWrite, Category = BossDrone)
		bool m_isDead;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "ProjectileSpawnLoc")
		class USceneComponent* m_muzzleLoc;

	UPROPERTY()
		UElementAffinityComponent* m_elementAffinityComp;

	FCriticalSection m_criticalSection;
	FTimerHandle m_shootTimerHandle;
	FTimerHandle m_timeToNextShootHandle;

	UPROPERTY()
		TArray<UWeaponComponent*> m_Weapons;

	bool ChangeModuleType(int newType);

	/*bool m_equipped;
	bool m_requestedEquipment;*/

	bool m_canMove;
	bool m_canShoot;

	bool m_isKeyHolder;
	int m_keyItemId = -1;

	bool m_wantChangeModuleType = false;
	int m_moduleChangeTryCountMax = 300;
	int m_currentModuleTryCount = 0;
	int m_newModuleType;
};
