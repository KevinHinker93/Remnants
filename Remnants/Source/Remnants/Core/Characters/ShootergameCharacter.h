// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbleToShoot.h"
#include "ElementStatusEffectVictim.h"
#include "KillAble.h"
#include "ShootergameCharacter.generated.h"

class UAItem;
class UInputComponent;

UCLASS(config=Game)
class AShootergameCharacter : public ACharacter, public IAbleToShoot, public IElementStatusEffectVictim, public IKillAble
{
	GENERATED_BODY()

public:
	AShootergameCharacter();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "LvlUp"))
		void LvlUp();

	/*UFUNCTION(BlueprintCallable)
		bool CanCraftItem(int id);*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		class UWeaponComponent* m_weaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
		class UInventoryComponent* m_inventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		class UActorStatsComponent* m_actorStatsComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "States")
		class UCharacterStateComponent* m_stateComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Stamina")
		class UStaminaComponent* m_staminaComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Camera Controll")
		class UCameraControllerComponent* m_cameraControllerComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Capsule Controll")
		class UCapsuleCollControllerComponent* m_capsuleControllerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		class UCharacterInteractionComponent* m_interactionComponent;

	// Animation to play each time we fire a bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* m_fireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector m_gunOffset;

	// Base turn rate, in deg/sec. Other scaling may affect final turn rate.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float m_baseTurnRate;

	// Base look up/down rate, in deg/sec. Other scaling may affect final rate.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float m_baseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		float m_currentExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		float m_neededExpForLvlUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Exp")
		float m_neededExpPerLevelMult;

	// Interface functions: IAbleToShoot
	virtual class USceneComponent* GetMuzzelLoc() override;
	virtual void WeaponFireAnimation() override;
	virtual float GetCurrentOverheatPercentage() const;
	virtual float GetDamageMultiplier() const;
	
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

	// Call this function to damage this actor
	// Use the damage event to gain information about the incoming damage, for ex. type
	UFUNCTION(BlueprintCallable, Category = "Damage")
	float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Returns Mesh1P subobject
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return m_mesh1P; }
	// Returns FirstPersonCameraComponent subobject
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return m_firstPersonCameraComponent; }

	virtual void Kill() override;

	UFUNCTION(BlueprintCallable, Category = "Stats")
		void UpdateStats();

	UFUNCTION(Blueprintcallable, Category = "Stats")
		void ChangeLevel(int level);

	void Reset();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		class UHealthComponent* m_healthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		class UShieldComponent* m_shieldComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "ElementEffect")
		class UElementEffectComponent* m_elementEffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
		class UHitComboComponent* m_hitComboComponent;

	// Interaction collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Collision")
		class UBoxComponent* m_interactionCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DamageResistance")
		float m_damageResistance;

	float m_moveSpeedMult;

	virtual void BeginPlay();
	virtual void EndPlay(EEndPlayReason::Type endPlayReason);
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// Gets triggered when firePrimary button is pressed once
	void OnFirePrimaryButtonPressed();
	// Gets triggered when firePrimary button is released, to tell weapon component to stop firing
	void OnFirePrimaryButtonReleased();

	// Gets triggered when fireSecondary button is pressed once
	void OnFireSecondaryButtonPressed();
	// Gets triggered when fireSecondary button is released, to tell weapon component to stop firing
	void OnFireSecondaryButtonReleased();

	void ToggleCrouch();

	// On MouseWheel Up -> next module slot
	void ChangeElementPos();
	// On MouseWheel Down -> previous element slot
	void ChangeElementNeg();

	// Handles moving forward/backward
	void MoveForward(float Val);

	// Handles stafing movement, left and right
	void Strafe(float Val);

	// Called via input to turn at a given rate.
	// This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	void TurnAtRate(float Rate);

	// Called via input to turn look up/down at a given rate.
	// This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	void LookUpAtRate(float Rate);

	void Interact();

	bool m_isDead;

private:
	FCriticalSection m_criticalSection;

	// Pawn mesh: 1st person view (arms; seen only by self)
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
		class USkeletalMeshComponent* m_mesh1P;

	// Gun mesh: 1st person view (seen only by self)
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* m_fpGun;

	// Location on gun mesh where projectiles should spawn.
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* m_muzzleLocation;

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* m_firstPersonCameraComponent;

	//Event listener
	UFUNCTION()
		void OnStopSprinting(APawn* sprintingPawn);
	UFUNCTION()
		void OnEnemyDied(float exp, FName tagOfDeathCauser);

	UFUNCTION()
		void OnExternalChangeState(AActor* actorToChangeState, FName desiredState);

	UFUNCTION()
		void OnExternalResetState(AActor* actorToChangeState);

	UFUNCTION(Blueprintcallable, Category = "Stats")
		void AddExp(float exp);
	void CheckForLvlUp();

	void StartSprinting();
	void StopSprinting();
};