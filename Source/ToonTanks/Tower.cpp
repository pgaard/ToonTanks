// Fill out your copyright notice in the Description page of Project Settings.

#include "Tower.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void ATower::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (TankIsInRange())
  {
    RotateTurret(Tank->GetActorLocation());
  }
}

void ATower::HandleDestruction()
{
  Super::HandleDestruction();
  Destroy();
}

bool ATower::TankIsInRange()
{
  if (Tank)
  {
    // find the distance to the tank
    float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());

    // check to see if the tank is in range
    return Distance <= FireRange;
  }

  return false;
}

void ATower::BeginPlay()
{
  Super::BeginPlay();

  Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));

  GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

void ATower::CheckFireCondition()
{
  if (Tank == nullptr)
  {
    return;
  }
  if (TankIsInRange() && Tank->bAlive)
  {
    Fire();
  }
}