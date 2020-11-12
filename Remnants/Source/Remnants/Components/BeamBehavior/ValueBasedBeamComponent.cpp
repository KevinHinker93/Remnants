// Fill out your copyright notice in the Description page of Project Settings.

#include "ValueBasedBeamComponent.h"
#include "../../Core/Characters/AEnemyDrone.h"
#include "../ShieldComponent.h"
#include "Kismet/GameplayStatics.h"

void UValueBasedBeamComponent::DamageEnemy(AActor* DamagedActor, FVector const& HitFromDirection, FHitResult const& HitInfo,
	AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UElementDamageType> DamageTypeClass)
{
	Super::DamageEnemy(DamagedActor, HitFromDirection, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);
}

void UValueBasedBeamComponent::OnApplyDamage(AActor* damagedActor, float baseDamage, FVector const& hitFromDirection, FHitResult const& hitInfo,
	AController* eventInstigator, AActor* damageCauser, TSubclassOf<UDamageType> damageTypeClass)
{
	float damageMult = 1.0f;
	AAEnemyDrone* enemy = Cast<AAEnemyDrone>(damagedActor);
	if (enemy)
	{
		UShieldComponent* shield = Cast<UShieldComponent>(enemy->GetComponentByClass(UShieldComponent::StaticClass()));
		if (!shield)
			return;
		float shieldPerc = shield->GetCurrentHealthPercentage();
		damageMult = shieldPerc;
		damageMult = FMath::Max(m_minDamageMultVal, damageMult);
	}

	UGameplayStatics::ApplyPointDamage(damagedActor, baseDamage * damageMult, hitFromDirection, hitInfo, NULL, damageCauser, damageTypeClass);
}

