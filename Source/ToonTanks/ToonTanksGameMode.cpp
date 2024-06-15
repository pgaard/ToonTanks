// Fill out your copyright notice in the Description page of Project Settings.

#include "ToonTanksGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Tower.h"
#include "ToonTanksPlayerController.h"
#include "TimerManager.h"

void AToonTanksGameMode::ActorDied(AActor *DeadActor)
{
  if (DeadActor == Tank)
  {
    Tank->HandleDestruction();

    if (ToonTanksPlayerController)
    {
      ToonTanksPlayerController->SetPlayerEnabledState(false);
    }

    // player is dead
    GameOver(false);
  }
  else if (ATower *DestroyedTower = Cast<ATower>(DeadActor))
  {
    DestroyedTower->HandleDestruction();
    TargetTowers--;
    if (TargetTowers == 0)
    {
      GameOver(true);
    }
  }
}

void AToonTanksGameMode::BeginPlay()
{
  Super::BeginPlay();

  HandleGameStart();
}

void AToonTanksGameMode::HandleGameStart()
{
  TargetTowers = GetTargetTowerCount();  
  Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
  ToonTanksPlayerController = Cast<AToonTanksPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

  StartGame();

  if (ToonTanksPlayerController)
  {
    ToonTanksPlayerController->SetPlayerEnabledState(false);

    // use a timer delegate so we can pass a parameter to SetPlayerEnabledState
    FTimerHandle PlayerEnableTimerHandle;

    FTimerDelegate PlayerEnabletimerDelegate = FTimerDelegate::CreateUObject(
        ToonTanksPlayerController,
        &AToonTanksPlayerController::SetPlayerEnabledState,
        true);

    GetWorldTimerManager().SetTimer(
        PlayerEnableTimerHandle,
        PlayerEnabletimerDelegate,
        StartDelay,
        false);
  }
}

int32 AToonTanksGameMode::GetTargetTowerCount()
{
  TArray<AActor *> TowerArray;
  UGameplayStatics::GetAllActorsOfClass(this, ATower::StaticClass(), TowerArray);
  return TowerArray.Num();
}