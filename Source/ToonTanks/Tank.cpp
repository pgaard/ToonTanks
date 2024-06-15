// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h" // not needed
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ATank::ATank()
{
  SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Sprint Arm"));
  SpringArm->SetupAttachment(RootComponent);

  Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
  Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
  Super::BeginPlay();

  TankPlayerController = Cast<APlayerController>(GetController());
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (TankPlayerController)
  {
    FHitResult HitResult;
    if (TankPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult))
    {
      RotateTurret(HitResult.ImpactPoint);
    }

    // DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 12, FColor::Red, false, -1.f); // persist 1 frame only
  }
}

void ATank::HandleDestruction()
{
  Super::HandleDestruction();
  SetActorHiddenInGame(true);
  SetActorTickEnabled(false);
  bAlive = false;
}

void ATank::Move(float value)
{
  float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
  FVector move(value * DeltaTime * Speed, 0.f, 0.f);
  // UE_LOG(LogTemp, Display, TEXT("Move value %f DeltaTime %f Speed %f movex %f"), value, DeltaTime, Speed, move.X);
  AddActorLocalOffset(move, true);
}

void ATank::Rotate(float value)
{
  float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
  FRotator rotate(0.f, value * DeltaTime * RotationSpeed, 0.f);
  AddActorLocalRotation(rotate, true);
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
  PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATank::Rotate);

  PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATank::Fire);
}
