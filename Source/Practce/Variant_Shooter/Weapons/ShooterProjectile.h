// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ShooterProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class ACharacter;
class UPrimitiveComponent;

/**
 *  Simple projectile class for a first person shooter game
 */
UCLASS()
class PRACTCE_API AShooterProjectile : public AActor
{
	GENERATED_BODY()

public:
	AShooterProjectile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float Damage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Damage")
	float BoxDamage = 0.5f;

	UPROPERTY(EditAnywhere, Category="Projectile|Hit")
	TSubclassOf<UDamageType> HitDamageType;

	void FireInDirection(const FVector& ShootDirection);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	// ✅ Sirf NotifyHit rakho
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other,
						   UPrimitiveComponent* OtherComp, bool bSelfMoved,
						   FVector HitLocation, FVector HitNormal,
						   FVector NormalImpulse, const FHitResult& Hit) override;

	void ProcessHit(AActor* HitActor, UPrimitiveComponent* HitComp,
					const FVector& HitLocation, const FVector& HitDirection);

	void ExplosionCheck(const FVector& ExplosionCenter);
	void OnDeferredDestruction();

	UFUNCTION(BlueprintImplementableEvent, Category="Projectile")
	void BP_OnProjectileHit(const FHitResult& Hit);

private:
	USphereComponent* CollisionComponent;
	UProjectileMovementComponent* ProjectileMovement;

	bool bHit = false;
	FTimerHandle DestructionTimer;
	bool bExplodeOnHit = false;
	bool bDamageOwner = false;
	float ExplosionRadius = 500.0f;
	float DeferredDestructionTime = 5.0f;
	float NoiseLoudness = 3.0f;
	float NoiseRange = 3000.0f;
	FName NoiseTag = FName("Projectile");
};