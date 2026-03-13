// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Playerui.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

class UInputMappingContext;
class AShooterCharacter;
class UShooterBulletCounterUI;
class UShooterUI;
class UPlayerui;
UCLASS(abstract, config="Game")
class PRACTCE_API AShooterPlayerController : public APlayerController
{
    GENERATED_BODY()
    
protected:

    UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
    TArray<UInputMappingContext*> DefaultMappingContexts;

    UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
    TArray<UInputMappingContext*> MobileExcludedMappingContexts;

    UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
    TSubclassOf<UUserWidget> MobileControlsWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> MobileControlsWidget;

    UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
    bool bForceTouchControls = false;

    UPROPERTY(EditAnywhere, Category="Shooter|Respawn")
    TSubclassOf<AShooterCharacter> CharacterClass;

    UPROPERTY(EditAnywhere, Category="Shooter|UI")
    TSubclassOf<UShooterBulletCounterUI> BulletCounterUIClass;

    UPROPERTY(EditAnywhere, Category="Shooter|Player")
    FName PlayerPawnTag = FName("Player");

    UPROPERTY()
    TObjectPtr<UShooterBulletCounterUI> BulletCounterUI;

protected:

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void OnPossess(APawn* InPawn) override;

    UFUNCTION()
    void OnBoxDestroyedHandler(uint8 TeamByte, int32 Points);

    UFUNCTION()
    void OnPawnDestroyed(AActor* DestroyedActor);

    UFUNCTION()
    void OnBulletCountUpdated(int32 MagazineSize, int32 Bullets);

    UFUNCTION()
    void OnPawnDamaged(float LifePercent);

    bool ShouldUseTouchControls() const;

    UPROPERTY(EditAnywhere, Category="Shooter|UI")
    TSubclassOf<UShooterUI> ShooterUIClass;

    UPROPERTY()
    UShooterUI* ShooterUIWidget;

    // Team scores and destroyed counts
    int32 Team1Score = 0;
    int32 Team2Score = 0;
    int32 Team1Destroyed = 0;
    int32 Team2Destroyed = 0;

public:
    // Blueprint event for updating UI
    UFUNCTION(BlueprintImplementableEvent, Category="UI")
    void BP_UpdateScore(int32 InTeam1Score, int32 InTeam2Score, int32 InTeam1Destroyed, int32 InTeam2Destroyed);

    void AddScore(uint8 TeamByte, int32 Points);
    UPROPERTY(EditAnywhere, Category="Shooter|UI")
    TSubclassOf<UPlayerui> PlayerUIClass;

    UPROPERTY()
    UPlayerui* PlayerUIWidget;

};