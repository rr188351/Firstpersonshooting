// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Playerui.generated.h"

UCLASS()
class PRACTCE_API UPlayerui : public UUserWidget
{
	GENERATED_BODY()

public:
	// Team scores and destroyed counts exposed to Blueprint
	UPROPERTY(BlueprintReadOnly, Category="Score")
	int32 Team1Score = 0;

	UPROPERTY(BlueprintReadOnly, Category="Score")
	int32 Team2Score = 0;

	UPROPERTY(BlueprintReadOnly, Category="Score")
	int32 Team1Destroyed = 0;

	UPROPERTY(BlueprintReadOnly, Category="Score")
	int32 Team2Destroyed = 0;

	// Function to update values from C++
	UFUNCTION(BlueprintCallable, Category="Score")
	void UpdateScore(int32 InTeam1Score, int32 InTeam2Score, int32 InTeam1Destroyed, int32 InTeam2Destroyed);
};