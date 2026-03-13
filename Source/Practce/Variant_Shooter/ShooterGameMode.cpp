// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Shooter/ShooterGameMode.h"
#include "ShooterUI.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	// create the UI
	ShooterUI = CreateWidget<UShooterUI>(UGameplayStatics::GetPlayerController(GetWorld(), 0), ShooterUIClass);
	ShooterUI->AddToViewport(0);
}

void AShooterGameMode::IncrementTeamScore(uint8 TeamByte, int32 Points)
{
	UE_LOG(LogTemp, Warning, TEXT("IncrementTeamScore called: Team=%d Points=%d"), TeamByte, Points);


	int32 Score = 0;
	if (int32* FoundScore = TeamScores.Find(TeamByte))
	{
		Score = *FoundScore;
	}

	Score += Points;   // ✅ use the actual points value
	TeamScores.Add(TeamByte, Score);

	ShooterUI->BP_UpdateScore(TeamByte, Score);
}



