#include "AchievementManager.h"
#include "OnlineAchievementsInterface.h"
#include "OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Engine.h"
#include "EventSystemHandlerSingleton.h"

#define ACH_COMPLETE_TUTORIAL FString("ACH_COMPLETE_TUTORIAL")
#define ACH_FIND_LAVA_KEY FString("ACH_FIND_LAVA_KEY")
#define ACH_FIND_ICE_KEY FString("ACH_FIND_ICE_KEY")
#define ACH_FIND_THUNDER_KEY FString("ACH_FIND_THUNDER_KEY")
#define ACH_FIND_ACID_KEY FString("ACH_FIND_ACID_KEY")
#define ACH_FIND_STEAM_KEY FString("ACH_FIND_STEAM_KEY")
#define ACH_FIND_CRYSTAL_KEY FString("ACH_FIND_CRYSTAL_KEY")
#define ACH_GET_ALL_KEYS FString("ACH_GET_ALL_KEYS")
#define ACH_KILL_BOSS FString("ACH_KILL_BOSS")
#define ACH_HALF_BUFF_ORBS FString("ACH_HALF_BUFF_ORBS")
#define ACH_ALL_BUFF_ORBS FString("ACH_ALL_BUFF_ORBS")
#define ACH_DESTROY_CRYSTALS FString("LootedCrystals")
#define ACH_LVL_50 FString("ACH_LVL_50")
#define ACH_KILL_DRONES FString("KilledEnemies")
#define ACH_FORGE FString("Forge")
#define ACH_TIMED_BOSS FString("ACH_TIMED_BOSS")
#define ACH_DONT_DIE FString("ACH_DONT_DIE")
#define ACH_DEAL_CERTAIN_DAMAGE FString("ACH_DEAL_CERTAIN_DAMAGE")
#define ACH_NO_STAT_ADJUSTEMENT FString("ACH_NO_STAT_ADJUSTEMENT")
#define ACH_TIMED_RUN FString("ACH_TIMED_RUN")
#define ACH_LAVA_KILLER FString("LavaKill")
#define ACH_ICE_KILLER FString("IceKill")
#define ACH_THUNDER_KILLER FString("ThunderKill")
#define ACH_STEAM_KILLER FString("SteamKill")
#define ACH_CRYSTAL_KILLER FString("CrystalKill")
#define ACH_ACID_KILLER FString("AcidKill")


AAchievementManager::AAchievementManager()
{
	PrimaryActorTick.bCanEverTick = true;
	m_playerLvlToReach = 50;
	m_bossTimeToBeat = 600.0f;
	m_playerDied = false;
	m_damageForAchievement = 300.0f;
	m_statAdjusted = false;
	m_accumulatedPlayingTime = 0.0f;
	m_achievementPlayingTime = 2400.0f;
	m_startingTime = 0.0f;
	m_buffOrbAmountHalf = 8;
	m_buffOrbAmountFull = 15;
	m_totalGameTime = 0.0f;
	m_displayPlayTime = false;
	m_displayBossTime = false;
}

void AAchievementManager::RemoveRequestedAchievement(const FString finishedAchievement)
{
	FString requestedAchievement = "";
	if (m_multipleAchievementUnlocks.IsEmpty())
		return;
	m_multipleAchievementUnlocks.Peek(requestedAchievement);
	if (requestedAchievement == finishedAchievement)
		m_multipleAchievementUnlocks.Pop();
}

void AAchievementManager::AddRequestedAchievementStat(const FString achievementStat)
{
	m_multipleAchievementStatsUnlocks.Enqueue(achievementStat);
}

void AAchievementManager::NextRequestedAchievementStat()
{
	if (!m_multipleAchievementStatsUnlocks.IsEmpty())
	{
		FString nextAchievement = "";
		m_multipleAchievementStatsUnlocks.Peek(nextAchievement);
		m_multipleAchievementStatsUnlocks.Pop();
		SetAchievementStatAmountByName(nextAchievement, 1);
	}
}

void AAchievementManager::CompleteAchievementByNameBinding(const FString& achievementName)
{
	if (achievementName == ACH_DONT_DIE)
	{
		if (!m_playerDied)
		{
			UE_LOG(LogTemp, Log, TEXT("Ach: Player died: %s"), (m_playerDied ? TEXT("True") : TEXT("False")));
			CompleteAchievementByName(ACH_DONT_DIE);
		}
	}
	else if (achievementName == ACH_NO_STAT_ADJUSTEMENT)
	{
		if (!m_statAdjusted)
		{
			CompleteAchievementByName(ACH_NO_STAT_ADJUSTEMENT);
			UE_LOG(LogTemp, Log, TEXT("Ach: Player changed stats: %s"), (m_statAdjusted ? TEXT("True") : TEXT("False")));
		}
	}
	else
		CompleteAchievementByName(achievementName);
}

void AAchievementManager::BeginPlay()
{
	Super::BeginPlay();
	m_totalGameTime = 0.0f;
	m_totalBossTime = 0.0f;
	m_bossActivated = false;
	QueryAchievements();

	bool checkSingleton = false;
	auto eventHandler = UEventSystemHandlerSingleton::GetEventSystemHandler(checkSingleton);
	if (eventHandler)
	{
		eventHandler->OnReceiveElementalKey.AddDynamic(this, &AAchievementManager::OnUpdateElementalKeyAchievement);
		eventHandler->OnPedestalInsertionDelegate.AddDynamic(this, &AAchievementManager::OnUpdateAllKeyInsertionAchievement);
		eventHandler->OnBossDied.AddDynamic(this, &AAchievementManager::OnUpdateKillBossAchievement);
		eventHandler->OnLootingCrystalDestroyed.AddDynamic(this, &AAchievementManager::OnUpdateDestroyLootingCrystalAchievement);
		eventHandler->OnPlayerLvlUp.AddDynamic(this, &AAchievementManager::OnUpdateReachLvlAchievement);
		eventHandler->OnEnemyDied.AddDynamic(this, &AAchievementManager::OnUpdateKillDronesAchievement);
		eventHandler->OnCraftedItem.AddDynamic(this, &AAchievementManager::OnUpdateCraftItemAchievement);
		eventHandler->OnReceivedBuffOrb.AddDynamic(this, &AAchievementManager::OnUpdateBuffOrbAchievement);
		eventHandler->OnBossActivated.AddDynamic(this, &AAchievementManager::OnBossActivated);
		eventHandler->OnPlayerDiedAchievementNotif.AddDynamic(this, &AAchievementManager::OnPlayerDied);
		eventHandler->OnGameCompleted.AddDynamic(this, &AAchievementManager::OnGameCompleted);
		/*eventHandler->OnCheckPlayerDied.AddDynamic(this, &AAchievementManager::TryActivatePlayerDontDieAchievement);
		eventHandler->OnCheckPlayerChangedStats.AddDynamic(this, &AAchievementManager::TryActivateStatChangedAchievement);*/
		eventHandler->OnPlayerDealtDamage.AddDynamic(this, &AAchievementManager::OnPlayerDealtDamage);
		eventHandler->OnStatAdjusted.AddDynamic(this, &AAchievementManager::OnStatAdjusted);
		eventHandler->OnDroneElementDeathCause.AddDynamic(this, &AAchievementManager::OnKilledDroneWithElement);
		eventHandler->OnCompleteTutorialLevelAchievement.AddDynamic(this, &AAchievementManager::CompleteTutorialAchievement);
		eventHandler->OnCompleteAchievementByName.AddDynamic(this, &AAchievementManager::CompleteAchievementByNameBinding);
		eventHandler->OnCheckBossTimedRunDelegate.AddDynamic(this, &AAchievementManager::ActivateTimedBossAchievement);
		
	}
}

void AAchievementManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_multipleAchievementUnlocks.IsEmpty())
	{
		FString nextAchievement = "";
		m_multipleAchievementUnlocks.Peek(nextAchievement);
		CompleteAchievementByName(nextAchievement);
	}

	m_totalGameTime += DeltaTime;
	if (m_displayPlayTime)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Play time %f"), m_totalGameTime));
	}

	if (m_bossActivated)
	{
		m_totalBossTime += DeltaTime;

		if (GEngine && m_displayBossTime)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Boss time %f"), m_totalBossTime));
	}
}

void AAchievementManager::QueryAchievements()
{
	//Get the online sub system
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		//Get the Identity from the sub system 
		//Meaning our player's profile and various online services
		IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();

		if (Identity.IsValid())
		{
			//Get a thread-safe pointer (for more info check out this link: https://docs.unrealengine.com/latest/INT/API/Runtime/Core/Templates/TSharedPtr/index.html )
			TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);

			//Get the achievements interface for this platform
			IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

			if (Achievements.IsValid())
			{
				//Cache all the game's achievements for future use and bind the OnQueryAchievementsComplete function to fire when we're finished caching
				Achievements->QueryAchievements(*UserId.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &AAchievementManager::OnQueryAchievementsComplete));
			}
		}
	}
}

void AAchievementManager::OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GLog->Log("achievements were cached");
	}
	else
	{
		GLog->Log("failed to cache achievements");
	}
}

void AAchievementManager::UpdateAchievementProgress(const FString& Id, float Percent)
{
	//Get the online sub system
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		//Get the Identity from the sub system 
		//Meaning our player's profile and various online services
		IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();

		if (Identity.IsValid())
		{
			//Get a thread-safe pointer (for more info check out this link: https://docs.unrealengine.com/latest/INT/API/Runtime/Core/Templates/TSharedPtr/index.html )
			TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);

			//Get the achievements interface for this platform
			IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

			if (Achievements.IsValid() && (!AchievementsWriteObjectPtr.IsValid() || !AchievementsWriteObjectPtr->WriteState != EOnlineAsyncTaskState::InProgress))
			{
				//Make a shared pointer for achievement writing
				AchievementsWriteObjectPtr = MakeShareable(new FOnlineAchievementsWrite());

				//Sets the progress of the desired achievement - does nothing if the id is not valid
				AchievementsWriteObjectPtr->SetFloatStat(*Id, Percent);

				//Write the achievements progress
				FOnlineAchievementsWriteRef AchievementsWriteObjectRef = AchievementsWriteObjectPtr.ToSharedRef();
				Achievements->WriteAchievements(*UserId, AchievementsWriteObjectRef);
			}
		}
	}
}

float AAchievementManager::GetAccumulatedPlayingTime()
{
	return m_totalGameTime; 
}

float AAchievementManager::GetAccumulatedBossTime()
{
	return m_totalBossTime;
}

void AAchievementManager::SetAccumulatedPlayingTime(float time)
{ 
	m_totalGameTime = time;
	UE_LOG(LogTemp, Log, TEXT("Playing time in total: %f"), m_totalGameTime);
}

void AAchievementManager::SetAccumulatedBossTime(float time)
{
	m_totalBossTime = time;
}

void AAchievementManager::SetStatChanged(bool changed)
{ 
	m_statAdjusted = changed; 
	UE_LOG(LogTemp, Log, TEXT("Player changed stats: %s"), (m_statAdjusted ? TEXT("True") : TEXT("False")));
}

void AAchievementManager::SetPlayerDied(bool died)
{ 
	if(m_playerDied == false)
		m_playerDied = died; 
	UE_LOG(LogTemp, Log, TEXT("Player died: %s"), (m_playerDied ? TEXT("True") : TEXT("False")));
}

void AAchievementManager::OnUpdateElementalKeyAchievement(const FString elementalKeyName)
{
	if (elementalKeyName == "Lava Key")
		CompleteAchievementByName(ACH_FIND_LAVA_KEY);
	else if (elementalKeyName == "Ice Key")
		CompleteAchievementByName(ACH_FIND_ICE_KEY);
	else if (elementalKeyName == "Thunder Key")
		CompleteAchievementByName(ACH_FIND_THUNDER_KEY);
	else if (elementalKeyName == "Acid Key")
		CompleteAchievementByName(ACH_FIND_ACID_KEY);
	else if (elementalKeyName == "Steam Key")
		CompleteAchievementByName(ACH_FIND_STEAM_KEY);
	else if (elementalKeyName == "Crystal Key")
		CompleteAchievementByName(ACH_FIND_CRYSTAL_KEY);
}

void AAchievementManager::OnUpdateAllKeyInsertionAchievement(FName keyType)
{
	if (keyType == "AllKeys")
		CompleteAchievementByName(ACH_GET_ALL_KEYS);
}

void AAchievementManager::OnUpdateKillBossAchievement()
{
	m_bossActivated = false;
	ActivateKillBossAchievement();
	//TryActivateTimedBossAchievement();
	m_displayBossTime = false;
}

void AAchievementManager::ActivateKillBossAchievement()
{
	CompleteAchievementByName(ACH_KILL_BOSS);
	//m_multipleAchievementUnlocks.Enqueue(ACH_KILL_BOSS);
}

void AAchievementManager::TryActivateTimedBossAchievement()
{
 	if (m_totalBossTime < m_bossTimeToBeat)
	{
		UE_LOG(LogTemp, Log, TEXT("Ach: Boss killed in: %f"), m_totalBossTime);
		m_multipleAchievementUnlocks.Enqueue(ACH_TIMED_BOSS);
	}
		//CompleteAchievementByName(ACH_TIMED_BOSS);
	
}

void AAchievementManager::ActivateTimedBossAchievement()
{
	if (m_totalBossTime < m_bossTimeToBeat)
	{
		UE_LOG(LogTemp, Log, TEXT("Ach: Boss killed in: %f"), m_totalBossTime);
		CompleteAchievementByName(ACH_TIMED_BOSS);
		m_totalBossTime = 0.0f;
	}
}

void AAchievementManager::OnUpdateDestroyLootingCrystalAchievement()
{
	SetAchievementStatAmountByName(ACH_DESTROY_CRYSTALS, 1);
}

void AAchievementManager::OnUpdateReachLvlAchievement(const int newLvl)
{
	if (newLvl >= m_playerLvlToReach)
		CompleteAchievementByName(ACH_LVL_50);
}

void AAchievementManager::OnUpdateKillDronesAchievement(float exp, FName killer)
{
	if (killer == "Player")
	{
		SetAchievementStatAmountByNameCached(ACH_KILL_DRONES, 1);
		//m_multipleAchievementStatsUnlocks.Enqueue(ACH_KILL_DRONES);
		//SetAchievementStatAmountByName(ACH_KILL_DRONES, 1);
	}
}

void AAchievementManager::OnUpdateCraftItemAchievement()
{
	SetAchievementStatAmountByName(ACH_FORGE, 1);
}

void AAchievementManager::OnUpdateBuffOrbAchievement(int buffOrbAmount)
{
	if(buffOrbAmount >= m_buffOrbAmountHalf)
		CompleteAchievementByName(ACH_HALF_BUFF_ORBS);
	if(buffOrbAmount >= m_buffOrbAmountFull)
		CompleteAchievementByName(ACH_ALL_BUFF_ORBS);
}

void AAchievementManager::OnBossActivated(bool fullFight)
{
	/*if(fullFight)
		m_bossStartingTimeInSeconds = GetWorld()->GetTimeSeconds();*/
	m_bossActivated = true;
}

void AAchievementManager::OnPlayerDied()
{
	SetPlayerDied(true);
}

void AAchievementManager::OnGameCompleted()
{
	/*TryActivatePlayerDontDieAchievement();
	TryActivateStatChangedAchievement();*/
	TryActivateTimedRundAchievement();
}

void AAchievementManager::TryActivatePlayerDontDieAchievement()
{
	
	if (!m_playerDied)
	{
		UE_LOG(LogTemp, Log, TEXT("Ach: Player died: %s"), (m_playerDied ? TEXT("True") : TEXT("False")));
		m_multipleAchievementUnlocks.Enqueue(ACH_DONT_DIE);
	}
		//CompleteAchievementByName(ACH_DONT_DIE);
}

void AAchievementManager::TryActivateStatChangedAchievement()
{
	if (!m_statAdjusted)
	{
		m_multipleAchievementUnlocks.Enqueue(ACH_NO_STAT_ADJUSTEMENT);
		UE_LOG(LogTemp, Log, TEXT("Ach: Player changed stats: %s"), (m_statAdjusted ? TEXT("True") : TEXT("False")));
	}
		//CompleteAchievementByName(ACH_NO_STAT_ADJUSTEMENT);
}

void AAchievementManager::TryActivateTimedRundAchievement()
{
	/*if (m_totalGameTime <= m_achievementPlayingTime)
		m_multipleAchievementUnlocks.Enqueue(ACH_TIMED_RUN);*/
	if (m_totalGameTime <= m_achievementPlayingTime)
		CompleteAchievementByName(ACH_TIMED_RUN);
}

void AAchievementManager::OnPlayerDealtDamage(float damage)
{
	if (damage >= m_damageForAchievement)
		CompleteAchievementByName(ACH_DEAL_CERTAIN_DAMAGE);
}

void AAchievementManager::OnStatAdjusted()
{
	SetStatChanged(true);
}

void AAchievementManager::OnKilledDroneWithElement(ElementEffectType type)
{
	if (m_elementKillStatsForAchievements.Contains(type))
	{
		SetAchievementStatAmountByNameCached(m_elementKillStatsForAchievements[type], 1);
		//m_multipleAchievementStatsUnlocks.Enqueue(m_elementKillStatsForAchievements[type]);
		//SetAchievementStatAmountByName(m_elementKillStatsForAchievements[type], 1);
	}
}

void AAchievementManager::CompleteTutorialAchievement()
{
	CompleteAchievementByName(ACH_COMPLETE_TUTORIAL);
}