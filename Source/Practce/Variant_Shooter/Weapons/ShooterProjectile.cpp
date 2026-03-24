// Copyright Epic Games, Inc. All Rights Reserved.


#include "ShooterProjectile.h"

#include "MyClass.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "TimerManager.h"

AShooterProjectile::AShooterProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(16.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
    RootComponent = CollisionComponent;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 3000.0f;
    ProjectileMovement->MaxSpeed = 3000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;

    Damage = 25.0f;
    BoxDamage = 1.0f;
    HitDamageType = UDamageType::StaticClass();
}

void AShooterProjectile::BeginPlay()
{
    Super::BeginPlay();
    if (GetInstigator())
    {
        CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
    }
}

void AShooterProjectile::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorld()->GetTimerManager().ClearTimer(DestructionTimer);
}

void AShooterProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other,
                                   UPrimitiveComponent* OtherComp, bool bSelfMoved,
                                   FVector HitLocation, FVector HitNormal,
                                   FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHit) return;
    bHit = true;

    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MakeNoise(NoiseLoudness, GetInstigator(), GetActorLocation(), NoiseRange, NoiseTag);

    if (bExplodeOnHit)
    {
        ExplosionCheck(GetActorLocation());
    }
    else
    {
        ProcessHit(Other, OtherComp, Hit.ImpactPoint, -Hit.ImpactNormal);
    }

    BP_OnProjectileHit(Hit);

    if (DeferredDestructionTime > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(DestructionTimer, this, &AShooterProjectile::OnDeferredDestruction, DeferredDestructionTime, false);
    }
    else
    {
        Destroy();
    }
}

void AShooterProjectile::ProcessHit(AActor* Other, UPrimitiveComponent* HitComp,
									const FVector& HitLocation, const FVector& HitDirection)
{
	if (Other && Other != GetOwner())
	{
		if (AMyClass* HitBox = Cast<AMyClass>(Other))
		{
			UGameplayStatics::ApplyDamage(HitBox, BoxDamage, GetInstigatorController(), this, UDamageType::StaticClass());
		}
		else if (ACharacter* HitCharacter = Cast<ACharacter>(Other))
		{
			UGameplayStatics::ApplyDamage(HitCharacter, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}

	if (HitComp && HitComp->IsSimulatingPhysics())
	{
		HitComp->AddImpulseAtLocation(HitDirection * 100.0f, HitLocation);
	}

	Destroy(); // ✅ ek hi hit ke baad destroy
}

void AShooterProjectile::ExplosionCheck(const FVector& ExplosionCenter)
{
	// do a sphere overlap check look for nearby actors to damage
	TArray<FOverlapResult> Overlaps;

	FCollisionShape OverlapShape;
	OverlapShape.SetSphere(ExplosionRadius);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (!bDamageOwner)
	{
		QueryParams.AddIgnoredActor(GetInstigator());
	}

	GetWorld()->OverlapMultiByObjectType(Overlaps, ExplosionCenter, FQuat::Identity, ObjectParams, OverlapShape, QueryParams);

	TArray<AActor*> DamagedActors;

	// process the overlap results
	for (const FOverlapResult& CurrentOverlap : Overlaps)
	{
		// overlaps may return the same actor multiple times per each component overlapped
		// ensure we only damage each actor once by adding it to a damaged list
		if (DamagedActors.Find(CurrentOverlap.GetActor()) == INDEX_NONE)
		{
			DamagedActors.Add(CurrentOverlap.GetActor());

			// apply physics force away from the explosion
			const FVector& ExplosionDir = CurrentOverlap.GetActor()->GetActorLocation() - GetActorLocation();

			// push and/or damage the overlapped actor
			ProcessHit(CurrentOverlap.GetActor(), CurrentOverlap.GetComponent(), GetActorLocation(), ExplosionDir.GetSafeNormal());
		}
			
	}
}

void AShooterProjectile::OnDeferredDestruction()
{
    Destroy();
}

void AShooterProjectile::FireInDirection(const FVector& ShootDirection)
{
    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
    }
}