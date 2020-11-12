// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageArea.generated.h"

class AController;
class UElementDamageType;

UCLASS()
class REMNANTS_API ADamageArea : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageArea();

	void InitDamageArea(float damage, float interval, AController* eventInstigator,
			AActor* damageCauser, TSubclassOf<UElementDamageType> damageTypeClass, FName shooterType);

	UFUNCTION(BlueprintCallable)
		void OnAreaAttached();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area")
		bool m_areaDamages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area")
		bool m_destroyOnAttachedDestroyed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area")
		class USphereComponent* m_damageAreaCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area")
		class UParticleSystemComponent* m_particleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area")
		float m_damageMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area")
		FName m_collisionPostfix;

	UPROPERTY()
		AActor* m_damageCauser;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnKillAnimation"))
		void OnKillAnimation();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle m_damageTimerHandle;
	float m_damageInterval;
	float m_damage;
	UPROPERTY(BlueprintReadOnly, Category = "Damage Area", meta = (AllowPrivateAccess = true, HideInEditor = true))
		float m_lifeTime;

	UPROPERTY()
		AController* m_eventInstigator;
	
	TSubclassOf<UElementDamageType> m_damageTypeClass;
	
	void OnDamage();

	UFUNCTION()
		void OnAttachedActorDestroyed(AActor* destroyedActor);
};
