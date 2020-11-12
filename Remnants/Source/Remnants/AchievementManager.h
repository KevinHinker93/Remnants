#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OnlineStats.h"
#include "ElementStatusEffects/ElementEffectTypes.h"
#include "Containers/Queue.h"
#include "AchievementManager.generated.h"

UCLASS()
class REMNANTS_API AAchievementManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AAchievementManager();

	/** Get all the available achievements and cache them */
	void QueryAchievements();

	/** Called when the cache finishes */
	void OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful);

	/** Updates the achievement progress */
	void UpdateAchievementProgress(const FString& Id, float Percent);

	/** The object we're going to use in order to progress any achievement */
	FOnlineAchievementsWritePtr AchievementsWriteObjectPtr;

	float GetAccumulatedPlayingTime();
	float GetAccumulatedBossTime();
	bool GetStatChanged() { return m_statAdjusted; }
	bool GetPlayerDied() { return m_playerDied; }

	void SetAccumulatedPlayingTime(float time);
	void SetAccumulatedBossTime(float time);
	void SetStatChanged(bool changed);
	void SetPlayerDied(bool died);

protected:
	// Which lvl the player should reach to trigger an achievement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Settings")
		int m_playerLvlToReach;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Settings")
		int m_buffOrbAmountHalf;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Settings")
		int m_buffOrbAmountFull;

	// Maximum time the player have to kill the boss for receiving an achievement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Settings")
		float m_bossTimeToBeat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Settings")
		float m_damageForAchievement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Settings")
		float m_achievementPlayingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Settings")
		TMap<ElementEffectType, FString> m_elementKillStatsForAchievements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Debug")
		bool m_displayPlayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement Debug")
		bool m_displayBossTime;

	// save
	UPROPERTY(BlueprintReadWrite, Category = "Achievement Vars")
		float m_accumulatedPlayingTime;
	UPROPERTY(BlueprintReadWrite, Category = "Achievement Vars")
		float m_startingTime;

	UFUNCTION(BlueprintCallable, DisplayName = "Dequeue last achievement", Category = "Achievement")
		void RemoveRequestedAchievement(const FString finishedAchievement);

	UFUNCTION(BlueprintCallable, DisplayName = "Add achievement stat", Category = "Achievement")
		void AddRequestedAchievementStat(const FString finishedAchievementStat);

	UFUNCTION(BlueprintCallable, DisplayName = "Process next achievement stat", Category = "Achievement")
		void NextRequestedAchievementStat();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, DisplayName = "Check Playing Time Achievement", Category = "Achievement")
		bool CheckPlayingTimeAchievement();

	// Completely unlocks an achievement
	UFUNCTION(BlueprintCallable, DisplayName = "Complete Achievement By Name", Category = "Achievement")
		void CompleteAchievementByNameBinding(const FString& achievementName);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, DisplayName = "Complete Achievement", Category = "Achievement")
		void CompleteAchievementByName(const FString& achievementName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, DisplayName = "Update Achievement Stat", Category = "Achievement")
		void SetAchievementStatAmountByName(const FString& achievementStat, int statAmount);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, DisplayName = "Update Achievement Stat Cached", Category = "Achievement")
		void SetAchievementStatAmountByNameCached(const FString& achievementStat, int statAmount);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:	
	TQueue<FString, EQueueMode::Spsc> m_multipleAchievementUnlocks;
	TQueue<FString, EQueueMode::Spsc> m_multipleAchievementStatsUnlocks;

	//float m_bossStartingTimeInSeconds;
	// save both
	bool m_playerDied;
	bool m_statAdjusted;
	float m_totalGameTime;
	float m_totalBossTime;
	bool m_bossActivated;

	UFUNCTION()
		void OnUpdateElementalKeyAchievement(const FString elementalKeyName);

	UFUNCTION()
		void OnUpdateAllKeyInsertionAchievement(FName keyType);

	UFUNCTION()
		void OnUpdateKillBossAchievement();

	UFUNCTION()
		void ActivateKillBossAchievement();
	UFUNCTION()
		void TryActivateTimedBossAchievement();
	UFUNCTION()
		void ActivateTimedBossAchievement();

	UFUNCTION()
		void OnUpdateDestroyLootingCrystalAchievement();

	UFUNCTION()
		void OnUpdateReachLvlAchievement(const int newLvl);
	
	UFUNCTION()
		void OnUpdateKillDronesAchievement(float exp, FName killer);

	UFUNCTION()
		void OnUpdateCraftItemAchievement();

	UFUNCTION()
		void OnUpdateBuffOrbAchievement(int buffOrbAmount);

	UFUNCTION()
		void OnBossActivated(bool fullFight);

	UFUNCTION()
		void OnPlayerDied();

	UFUNCTION()
		void OnGameCompleted();

	UFUNCTION()
		void TryActivatePlayerDontDieAchievement();
	UFUNCTION()
		void TryActivateStatChangedAchievement();
	UFUNCTION()
		void TryActivateTimedRundAchievement();

	UFUNCTION()
		void OnPlayerDealtDamage(float damage);

	UFUNCTION()
		void OnStatAdjusted();

	UFUNCTION()
		void OnKilledDroneWithElement(ElementEffectType type);

	UFUNCTION()
		void CompleteTutorialAchievement();
};
