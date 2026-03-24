// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Healthbar.generated.h"

/**
 * 
 */
UCLASS()
class PRACTCE_API UHealthbar : public UUserWidget
{
	GENERATED_BODY()

public:
	// Progress bar reference (bind karna BP me)
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgress;

	// Function to update health
	UFUNCTION(BlueprintCallable, Category="Health")
	void SetHealthPercent(float Percent);
};