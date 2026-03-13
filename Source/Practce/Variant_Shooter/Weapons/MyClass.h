#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyClass.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBoxDestroyed, uint8, TeamByte, int32, Points);

UCLASS()
class PRACTCE_API AMyClass : public AActor
{
	GENERATED_BODY()

public:
	AMyClass();
	
	UPROPERTY(EditAnywhere, Category="Effects")
	USoundBase* DestroySound;

	UPROPERTY(EditAnywhere, Category="Effects")
	UParticleSystem* DestroyEffect;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BoxMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Box Stats")
	FString BoxTypeName;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnBoxDestroyed OnBoxDestroyed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Box Stats")
	int32 Health = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Box Stats")
	int32 Score = 10;   // ✅ yehi points use karo

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Box Stats")
	FLinearColor BoxColor = FLinearColor::Red;

	// Movement vars...
	UPROPERTY(EditAnywhere, Category="Movement")
	float FlySpeed = 200.0f;

	UPROPERTY(EditAnywhere, Category="Movement")
	float FlyRange = 500.0f;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Team")
	uint8 TeamByte = 1;
	virtual void BeginPlay() override;
	FVector StartLocation;
	bool bMovingForward = true;
	virtual void Destroyed() override;
	UPROPERTY(EditAnywhere, Category="Team")
	FName DeathTag = FName("Dead");


public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Box")
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void ApplyBoxStats(const FLinearColor& InColor, int32 InHealth, int32 InScore);
};