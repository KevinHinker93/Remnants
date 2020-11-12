// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemStruct.h"
#include "Core/Items/PickUp.h"
#include "ItemCollection.h"
#include "Core/Structural/EventFacade.h"
#include "ElementDamageType.h"
#include "Components/DamageEffectiveness.h"
#include "Components/ItemEffectivenessData.h"
#include "EventSystemHandlerSingleton.generated.h"

class UAItem;
class UEventFacade;
class AInteractible;

#pragma region delegate declarations

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSceneChangeRequestDelegate, const FString&, sceneToLoad, bool, save);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSceneChangeStartedDelegate, const FString&, currentScene, const FString&, sceneToLoad);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSceneChangeFinishedDelegate, const FString&, previousScene, const FString&, loadedScene);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDoorStateChangedDelegate, const FString&, doorName, bool, locked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDoorStateChangeRequestedDelegate, const FString&, doorName, bool, locked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnterInteractibleDelegate, AAInteractible*, interactible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExitInteractibleDelegate, AAInteractible*, interactible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDroneDiedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyItemIdRequestDelegate, UObject*, sender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKeyItemIdResponseDelegate, int, keyItemId, UObject*, sender, UObject*, receiver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyHolderKillDelegate, int, keyItemId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPedestalUpdatedDelegate, TArray<int>, equippedItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPedestalUpdateRequestDelegate, TArray<int>, equippedItems);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGemRecipeDeserialized, int, gemId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerCanCraftRequestDelegate, UObject*, sender, int, itemId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerCanCraftResponseDelegate, UObject*, sender, int, itemId, bool, canCraft);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnPickupSpawnRequestDelegate, UObject*, Sender, UAItem*, Item, const FTransform&, transform, ULevel*, level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnPickupSpawnByIdRequestDelegate, UObject*, Sender, int, ItemId, const FTransform&, transform, ULevel*, level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickupSpawnedDelegate, AActor*, Sender, APickUp*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickupDelegate, UObject*, pickUpReceiver, UAItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrbPickedUpDelegate, const FString&, orbName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRequestDelegate, UObject*, Sender, int, ItemId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemResponseDelegate, UObject*, Sender, UAItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemChunkRequestDelegate, UObject*, Sender, TArray<int>&, ItemIds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemChunkResponseDelegate, UObject*, Sender, TArray<UAItem*>&, Items);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemInfoRequestDelegate, UObject*, Sender, int, ItemId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemInfoResponseDelegate, UObject*, Sender, const FItemStruct&, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCraftingRequestDelegate, UObject*, Sender, int, itemId, UItemCollection*, Items);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCraftingResponseDelegate, UObject*, Sender, UAItem*, item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCraftingOptionRequestDelegate, UObject*, Sender, UItemCollection*, Items);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCraftingOptionResponseDelegate, UObject*, Sender, int, CraftingOptionId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSprintingDelegate, APawn*, sprintingCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterStopSprintingDelegate, APawn*, sprintingCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCharacterCrouchingDelegate, APawn*, crouchingCharacter, FVector, crouchOffset, float, crouchSpeed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeleporterRequestDelegate, UObject*, sender, int, teleporterId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeleporterReplyDelegate, UObject*, sender, AActor*, teleporter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemySpawnedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyDiedDelegate, float, exp, FName, deathCauserTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedDelegate, UHealthComponent*, Sender, float, Percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChangedDelegate, UShieldComponent*, Sender, float, Percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOverheatChangedDelegate, UWeaponComponent*, Sender, float, Percentage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnExternalOverheatDelegate, AActor*, actorToOverheat, float, overheatMult, int, sourceModule);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExternalEndFiringDelegate,AActor*, actorToEndFire, int, firedModuleType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExternalChangeStateDelegate,AActor*, actorToChangeState, FName, desiredState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExternalResetStateDelegate,AActor*, actorToChangeState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxShieldChangedDelegate, UShieldComponent*, Sender, float, MaxShield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxHealthChangedDelegate, UHealthComponent*, Sender, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorLevelChangedDelegate, UActorStatsComponent*, Sender, int, ActorLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnReceiveDamageDelegate, UShieldComponent*, Sender, int, Damage, EDamageEffectiveness, damageEffectiveness);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProjectileConnectionRequest, AActor*, Sender, AActor*, Receiver, float, Distance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnProjectileConnectionReply, AActor*, Receiver, bool, CanConnect, float, connectDuration, int, connectionsLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNotifyProjectileConnecter, AActor*, ProjectileConnecter, float, LifeTime, int, connectionsLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNotifyConnectedProjectile, AActor*, ConnectedProjectile, float, LifeTime, int, connectionsLeft);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterHasItemResponse, AActor*, receiver, bool, hasItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCharacterHasItemRequest, AActor*, sender, AActor*, receivingCharacter, int, itemId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterHasItemsResponse, AActor*, receiver, TArray<int>, availableItemIds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCharacterHasItemsRequest, AActor*, sender, AActor*, receivingCharacter, TArray<int>, itemIds);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPedestalInsertionDelegate, FName, eventName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenDoorDelegate, FName, eventName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyHolderSpawnedDelegate, AAEnemyDrone*, keyHolder);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyHolderDiedDelegate, int, keyItemId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrateDestroyedDelegate, FString, chestName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrateDestructionRequestDelegate, FString, chestName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggleElementClouds, int, cloudIdentifier);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnModifyInteractionWidget, bool, modifyVisibility, bool, visibility, FText, interactionText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDisplayUIMessage, FString, message, float, duration);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDisplayQuest, UTexture2D*, questImage, FText, questText, bool, questDone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnOffQuest);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemReceivedNotification, UTexture2D*, displayImage, FString, itemName, int, amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReceiveMiscNotification, FString, receiveType, int, amount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossShieldChanged, FName, elementName, float, percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossShieldBreak, FName, elementName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossHealthChanged, float, percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivateBoss);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPowerUpBuffActivated, FName, buffName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPowerUpCountChanged, int, powerUpCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxPowerUpCountChanged, int, idx, int, maxPowerUpCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNeededPowerUpCountChanged, int, idx, int, maxPowerUpCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnpointObjectSpawnedDelegate, const FString&, spawnGroupId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnpointObjectDestroyedDelegate, const FString&, spawnGroupId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpawnPointActivatedDelegate, const FString&, spawnGroupId, bool, activated);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadRequestedDelegate, bool, save);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelSaveRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelLoadRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSaveRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerLoadRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveFinishedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadFinishedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDiedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnpointLevelChangedDelegate, int, newLevel);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFirstLootCrateOpenTutorial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnKillFirstDronesTutorial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFirstEnterMainRoomTutorial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenDoorAfterTutorialComplete, int, idx);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraAnimEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialStateLoadedFromSaveGameDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombineTutorialCompletedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombineTutorialEnabledDelegate, int, craftableIdx);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaySong, FName, songName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeSongVolume, float, newVolume);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResetSong);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndCurrentSong);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShowHUD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideHUD);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResetSavegameManagerState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpreadExplosion, TSet<AActor*>, actorsReceivedExplosion);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetElementAffinity, AActor*, actorReceivedAffiniy, const TArray<FItemEffectivenessData>&, elementItemIdsWithEffectiveness);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReceivedElement, AActor*, actorReceivedElement, UAElement*, receivedElement);

// Achievement Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiveElementalKeyDelegate, const FString, elementalKeyName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLootingCrystalDestroyedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLvlUpDelegate, const int, newLvl);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCraftedItemDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceivedBuffOrbDelegate, const int, currentBuffOrbAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossActivatedDelegate, bool, fullFight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDiedAchievementNotifDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameCompletedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDealtDamageDelegate, const float, damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatAdjustedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDroneElementDeathCauseDelegate, const ElementEffectType, elementEffectType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompleteTutorialLevelAchievementDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckPlayerDiedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckPlayerChangedStatsDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompleteAchievementByName, const FString&, achievementName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckBossTimedRunDelegate);

// Hint Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestHintDisplayDelegate, const FString, hintType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHintDisplayDelegate, const FString, hintText, const float, displayTime);

// Combo Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnComboDataChanged, float, accumulatedDamage, int, comboHitCount, float, comboDamageMult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnComboEnded);

// Boss Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossShieldBroken, const FString, shieldName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestBossActivation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivateBossWBrokenShields, const TSet<FString>, brokenShields);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossDropItems, const TArray<int>&, droppedItemIds, const int&, amount);

#pragma endregion


UCLASS(Blueprintable, BlueprintType)
class REMNANTS_API UEventSystemHandlerSingleton : public UObject
{
	GENERATED_BODY()
	
		UEventSystemHandlerSingleton(const FObjectInitializer& ObjectInitializer);
public:	
		UFUNCTION(BlueprintPure)
		UEventFacade* GetEventFacade();
		
		UFUNCTION(BlueprintPure, Category = "Solus Data Singleton")
		static UEventSystemHandlerSingleton* GetEventSystemHandler(bool& IsValid);

#pragma region delegates

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "World Events")
			FOnTutorialStateLoadedFromSaveGameDelegate OnTutorialStateLoadedFromSaveGame;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnSpawnpointObjectSpawnedDelegate OnSpawnpointObjectSpawned;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnSpawnpointObjectDestroyedDelegate OnSpawnpointObjectDestroyed;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "World Events")
			FOnSceneChangeRequestDelegate OnSceneChangeRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "World Events")
			FOnSceneChangeStartedDelegate OnSceneChangeStarted;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "World Events")
			FOnSceneChangeFinishedDelegate OnSceneChangeFinished;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "World Events")
			FOnDoorStateChangedDelegate OnDoorStateChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "World Events")
			FOnDoorStateChangeRequestedDelegate OnDoorStateChangeRequested;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "World Events")
			FOnGemRecipeDeserialized OnGemRecipeDeserialized;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Pickup Events")
			FOnPickupSpawnRequestDelegate OnPickupSpawnRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Pickup Events")
			FOnPickupSpawnByIdRequestDelegate OnPickupSpawnByIdRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Pickup Events")
			FOnPickupSpawnedDelegate OnPickupSpawned;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Pickup Events")
			FOnPickupDelegate OnPickup;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Item Events")
			FOnItemRequestDelegate OnItemRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Item Events")
			FOnItemResponseDelegate OnItemResponse;

		UPROPERTY(VisibleAnywhere, Category = "Item Events")
			FOnItemChunkRequestDelegate OnItemChunkRequest;

		UPROPERTY(VisibleAnywhere, Category = "Item Events")
			FOnItemChunkResponseDelegate OnItemChunkResponse;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Item Events")
			FOnItemInfoRequestDelegate OnItemInfoRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Item Events")
			FOnItemInfoResponseDelegate OnItemInfoResponse;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Item Events")
			FOnCraftingRequestDelegate OnCraftingRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Item Events")
			FOnCraftingResponseDelegate OnCraftingResponse;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Item Events")
			FOnCraftingOptionRequestDelegate OnCraftingOptionRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Item Events")
			FOnCraftingOptionResponseDelegate OnCraftingOptionResponse;

		UPROPERTY(VisibleAnywhere, Category = "Character Events")
			FOnCharacterSprintingDelegate OnCharacterSprinting;

		UPROPERTY(VisibleAnywhere, Category = "Character Events")
			FOnCharacterStopSprintingDelegate OnCharacterStopSprinting;

		UPROPERTY(VisibleAnywhere, Category = "Character Events")
			FOnCharacterCrouchingDelegate OnCharacterCrouching;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Character Events")
			FOnPlayerCanCraftRequestDelegate OnPlayerCanCraftRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Character Events")
			FOnPlayerCanCraftResponseDelegate OnPlayerCanCraftResponse;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Character Events")
			FOnEnemyDiedDelegate OnEnemyDied;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Health Event")
			FOnHealthChangedDelegate OnHealthChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Health Event")
			FOnMaxHealthChangedDelegate OnMaxHealthChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Shield Event")
			FOnShieldChangedDelegate OnShieldChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Shield Event")
			FOnMaxShieldChangedDelegate OnMaxShieldChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Boss Event")
			FOnBossShieldChanged OnBossShieldChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Boss Event")
			FOnBossShieldBreak OnBossShieldBreak;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Boss Event")
			FOnBossHealthChanged OnBossHealthChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Boss Event")
			FOnActivateBoss OnActivateBoss;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Boss Event")
			FOnBossDroneDiedDelegate OnBossDied;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PowerUp Event")
			FOnPowerUpBuffActivated OnPowerUpBuffActivated;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PowerUp Event")
			FOnPowerUpCountChanged OnPowerUpCountChanged;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PowerUp Event")
			FOnMaxPowerUpCountChanged OnMaxPowerUpCountChanged;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PowerUp Event")
			FOnNeededPowerUpCountChanged OnNeededPowerUpCountChanged;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "PowerUp Event")
			FOnOrbPickedUpDelegate OnOrbPickedUp;
		
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Actor Level Event")
			FOnActorLevelChangedDelegate OnActorLevelChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Damage Event")
			FOnReceiveDamageDelegate OnReceiveDamage;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Overheat Event")
			FOnOverheatChangedDelegate OnOverheatChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Overheat Event")
			FOnExternalOverheatDelegate OnExternalOverheat;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Overheat Event")
			FOnExternalEndFiringDelegate OnExternalEndFiring;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "State Event")
			FOnExternalChangeStateDelegate OnExternalChangeState;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "State Event")
			FOnExternalResetStateDelegate OnExternalResetState;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "World Events")
			FOnTeleporterRequestDelegate OnTeleporterRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "World Events")
			FOnTeleporterReplyDelegate OnTeleporterReply;
		
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Projectile Events")
			FOnProjectileConnectionRequest OnProjectileConnectionRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Projectile Events")
			FOnProjectileConnectionReply OnProjectileConnectionReply;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Projectile Events")
			FOnNotifyProjectileConnecter OnNotifyProjectileConnecter;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Projectile Events")
			FOnNotifyConnectedProjectile OnNotifyConnectedProjectile;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Interactible Events")
			FOnEnterInteractibleDelegate OnEnterInteractible;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Interactible Events")
			FOnExitInteractibleDelegate OnExitInteractible;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Interactible Events")
			FOnCharacterHasItemRequest OnCharacterHasItemRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Interactible Events")
			FOnCharacterHasItemResponse OnCharacterHasItemResponse;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Interactible Events")
			FOnCharacterHasItemsRequest OnCharacterHasItemsRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Interactible Events")
			FOnCharacterHasItemsResponse OnCharacterHasItemsResponse;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Interactible Events")
			FOnModifyInteractionWidget OnModifyInteractionWidget;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI Message Events")
			FOnDisplayUIMessage OnDisplayUIMessage;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI Message Events")
			FOnDisplayQuest OnDisplayQuest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI Message Events")
			FOnTurnOffQuest OnTurnOffQuest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI Message Events")
			FOnItemReceivedNotification OnItemReceivedNotification;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI Message Events")
			FOnReceiveMiscNotification OnReceiveMiscNotification;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Pedestal Events")
			FOnPedestalInsertionDelegate OnPedestalInsertionDelegate;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Door Events")
			FOnOpenDoorDelegate OnOpenDoorDelegate;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnToggleElementClouds OnToggleElementClouds;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnKeyHolderSpawnedDelegate OnKeyHolderSpawned;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnKeyHolderDiedDelegate OnKeyHolderDied;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnEnemySpawnedDelegate OnEnemySpawned;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnCrateDestroyedDelegate OnCrateDestroyed;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnCrateDestructionRequestDelegate OnCrateDestructionRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnSaveRequestedDelegate OnSaveRequested;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnLevelSaveRequestedDelegate OnLevelSaveRequested;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnLoadRequestedDelegate OnLoadRequested;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnLevelLoadRequestedDelegate OnLevelLoadRequested;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnSaveFinishedDelegate OnSaveFinished;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnLoadFinishedDelegate OnLoadFinished;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnSpawnpointLevelChangedDelegate OnSpawnpointLevelChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnSpawnPointActivatedDelegate OnSpawnPointActivated;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnKeyItemIdRequestDelegate OnKeyItemIdRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnKeyItemIdResponseDelegate OnKeyItemIdResponse;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnKeyHolderKillDelegate OnKeyHolderKill;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnPedestalUpdatedDelegate OnPedestalUpdated;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnPedestalUpdateRequestDelegate OnPedestalUpdateRequest;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnPlayerLoadRequestedDelegate OnPlayerLoadRequested;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnPlayerSaveRequestedDelegate OnPlayerSaveRequested;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnPlayerDiedDelegate OnPlayerDied;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Tutorial Events")
			FOnFirstLootCrateOpenTutorial OnFirstLootCrateOpenTutorial;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Tutorial Events")
			FOnKillFirstDronesTutorial OnKillFirstDronesTutorial;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Tutorial Events")
			FOnFirstEnterMainRoomTutorial OnFirstEnterMainRoomTutorial;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Tutorial Events")
			FOnCombineTutorialCompletedDelegate OnCombineTutorialCompleted;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Tutorial Events")
			FOnCameraAnimEnd OnCameraAnimEnd;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Tutorial Events")
			FOnOpenDoorAfterTutorialComplete OnOpenDoorAfterTutorialComplete;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Tutorial Events")
			FOnCombineTutorialEnabledDelegate OnCombineTutorialEnabled;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Audio Events")
			FOnPlaySong OnPlaySong;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Audio Events")
			FOnChangeSongVolume OnChangeSongVolume;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Audio Events")
			FOnResetSong OnResetSong;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Audio Events")
			FOnEndCurrentSong OnEndCurrentSong;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI Events")
			FOnShowHUD OnShowHUD;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI Events")
			FOnHideHUD OnHideHUD;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnResetSavegameManagerState OnResetSavegameManagerState;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnSpreadExplosion OnSpreadExplosion;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnSetElementAffinity OnSetElementAffinity;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Gameplay Events")
			FOnReceivedElement OnReceivedElement;

		// Achievement Events
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnReceiveElementalKeyDelegate OnReceiveElementalKey;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnLootingCrystalDestroyedDelegate OnLootingCrystalDestroyed;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnPlayerLvlUpDelegate OnPlayerLvlUp;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnCraftedItemDelegate OnCraftedItem;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnReceivedBuffOrbDelegate OnReceivedBuffOrb;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnBossActivatedDelegate OnBossActivated;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnPlayerDiedAchievementNotifDelegate OnPlayerDiedAchievementNotif;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnGameCompletedDelegate OnGameCompleted;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnCheckPlayerDiedDelegate OnCheckPlayerDied;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnCheckPlayerChangedStatsDelegate OnCheckPlayerChangedStats;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnPlayerDealtDamageDelegate OnPlayerDealtDamage;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnStatAdjustedDelegate OnStatAdjusted;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnDroneElementDeathCauseDelegate OnDroneElementDeathCause;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnCompleteTutorialLevelAchievementDelegate OnCompleteTutorialLevelAchievement;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnCompleteAchievementByName OnCompleteAchievementByName;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Achievement Events")
			FOnCheckBossTimedRunDelegate OnCheckBossTimedRunDelegate;

		// Hint Events
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Hint Events")
			FOnRequestHintDisplayDelegate OnRequestHintDisplay;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Hint Events")
			FOnHintDisplayDelegate OnHintDisplay;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Combo Events")
			FOnComboDataChanged OnComboDataChanged;

		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Combo Events")
			FOnComboEnded OnComboEnded;

		// Boss Events
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Boss Events")
			FOnBossShieldBroken OnBossShieldBroken;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Boss Events")
			FOnRequestBossActivation OnRequestBossActivation;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Boss Events")
			FOnActivateBossWBrokenShields OnActivateBossWBrokenShields;
		UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Boss Events")
			FOnBossDropItems OnBossDropItems;

#pragma endregion

protected:
	UPROPERTY()
	UEventFacade* m_eventFacade;
};