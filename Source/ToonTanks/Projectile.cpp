// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));

	SmokeTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke Trail"));
	SmokeTrail->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	StaticMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComponent, FVector NormalImpluse, const FHitResult &Hit)
{
	auto MyOwner = GetOwner();

	UE_LOG(LogTemp, Warning, TEXT("OnHit HitComp %s OtherActor %s OtherComponent %s MyOwner %s"),
				 *HitComp->GetName(), *OtherActor->GetName(), *OtherComponent->GetName(), *MyOwner->GetName());
	if (MyOwner == nullptr)
	{
		Destroy();
		return;
	}

	AController *MyOwnerInstigator = MyOwner->GetInstigatorController();

	auto DamageTypeClass = UDamageType::StaticClass();

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		UGameplayStatics::ApplyDamage(
				OtherActor,				 // AActor* DamagedActor
				Damage,						 // float BaseDamage
				MyOwnerInstigator, // AController EventInstigator
				this,							 // AActor* DamageCauser
				DamageTypeClass);	 // TSubclassOf<class UDamageType> DamageTypeClass

		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}

		if (HitCameraShakeClass)
		{
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);
		}

		if (HitParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
					this,
					HitParticles,
					GetActorLocation(),
					GetActorRotation());
		}
	}

	Destroy(); // remove projectile
}