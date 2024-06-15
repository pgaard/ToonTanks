// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"

// Sets default values
ABasePawn::ABasePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// unreal engine way of constructing components
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
}

void ABasePawn::HandleDestruction()
{
	// TODO: visual/sound effects
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (DeathParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
				this,
				DeathParticles,
				GetActorLocation(),
				GetActorRotation());
	}

	if (DeathCameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DeathCameraShakeClass);
	}
}

void ABasePawn::RotateTurret(FVector LookAtTarget)
{
	// world space vector to target is target - turret location
	FVector ToTarget = LookAtTarget - TurretMesh->GetComponentLocation();

	// get the rotation but just the z-axis so we don't look up or down
	FRotator LookAtRotation = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);

	// TurretMesh->SetWorldRotation(LookAtRotation);
	TurretMesh->SetWorldRotation(
			FMath::RInterpTo(
					TurretMesh->GetComponentRotation(),
					LookAtRotation,
					UGameplayStatics::GetWorldDeltaSeconds(this),
					10.f));
}

void ABasePawn::Fire()
{
	// DrawDebugSphere(GetWorld(), ProjectileSpawnPoint->GetComponentLocation(), 10.f, 12, FColor::Red, false, 3.f); // persist 1 frame only

	auto Projectile = GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass,
			ProjectileSpawnPoint->GetComponentLocation(),
			ProjectileSpawnPoint->GetComponentRotation());

	// set owner so we can detect if we shot ourself
	Projectile->SetOwner(this);
}