// Fill out your copyright notice...

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "MyClass.h"
#include "Boxspawner.generated.h"

UCLASS()
class PRACTCE_API ABoxspawner : public AActor
{
	GENERATED_BODY()

public:
	ABoxspawner();
	UPROPERTY(EditAnywhere, Category="Effects")
	USoundBase* DefaultDestroySound;

	UPROPERTY(EditAnywhere, Category="Effects")
	UParticleSystem* DefaultDestroyEffect;
	UPROPERTY(EditAnywhere, Category="Spawning")
	TSubclassOf<AMyClass> BoxClass;
	void OnBoxDataResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


protected:
	virtual void BeginPlay() override;
	

public:
	virtual void Tick(float DeltaTime) override;
};