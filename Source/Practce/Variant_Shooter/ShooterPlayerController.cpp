#include "Variant_Shooter/ShooterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "ShooterCharacter.h"
#include "ShooterBulletCounterUI.h"
#include "Practce.h"
#include "ShooterUI.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AShooterPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalPlayerController())
    {
        // Existing ShooterUI
        if (ShooterUIClass)
        {
            ShooterUIWidget = CreateWidget<UShooterUI>(this, ShooterUIClass);
            if (ShooterUIWidget)
            {
                UE_LOG(LogTemp, Warning, TEXT("ShooterUIWidget created: %s"), *ShooterUIWidget->GetName());
                ShooterUIWidget->AddToViewport();
            }
        }

        // ✅ New PlayerUI (UI_Shooter1)
        if (PlayerUIClass)
        {
            PlayerUIWidget = CreateWidget<UPlayerui>(this, PlayerUIClass);
            if (PlayerUIWidget)
            {
                UE_LOG(LogTemp, Warning, TEXT("PlayerUIWidget created: %s"), *PlayerUIWidget->GetName());
                PlayerUIWidget->AddToViewport();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Could not spawn PlayerUI widget."));
            }
        }

        // Mobile controls + BulletCounterUI (tumhare code me already hai)
        if (ShouldUseTouchControls())
        {
            MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
            if (MobileControlsWidget)
            {
                MobileControlsWidget->AddToPlayerScreen(0);
            }
            else
            {
                UE_LOG(LogPractce, Error, TEXT("Could not spawn mobile controls widget."));
            }
        }

        BulletCounterUI = CreateWidget<UShooterBulletCounterUI>(this, BulletCounterUIClass);
        if (BulletCounterUI)
        {
            BulletCounterUI->AddToPlayerScreen(0);
        }
        else
        {
            UE_LOG(LogPractce, Error, TEXT("Could not spawn bullet counter widget."));
        }
    }
}
void AShooterPlayerController::AddScore(uint8 TeamByte, int32 PointsEarned)
{
    if (TeamByte == 1)
    {
        Team1Score += PointsEarned;
        Team1Destroyed++;
    }
    else if (TeamByte == 2)
    {
        Team2Score += PointsEarned;
        Team2Destroyed++;
    }

    UE_LOG(LogTemp, Warning, TEXT("Score Update: Team1=%d | Team2=%d | Destroyed1=%d | Destroyed2=%d"),
        Team1Score, Team2Score, Team1Destroyed, Team2Destroyed);

    // ✅ Update Points in UI_Shooter1
    if (ShooterUIWidget)
    {
        ShooterUIWidget->SetPoints(Team1Score + Team2Score); // Example: total points
    }

    // Blueprint event bhi call ho raha hai
    BP_UpdateScore(Team1Score, Team2Score, Team1Destroyed, Team2Destroyed);
}
void AShooterPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (IsLocalPlayerController())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
            {
                Subsystem->AddMappingContext(CurrentContext, 0);
            }

            if (!ShouldUseTouchControls())
            {
                for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
                {
                    Subsystem->AddMappingContext(CurrentContext, 0);
                }
            }
        }
    }
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    InPawn->OnDestroyed.AddDynamic(this, &AShooterPlayerController::OnPawnDestroyed);

    if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn))
    {
        ShooterCharacter->Tags.Add(PlayerPawnTag);
        ShooterCharacter->OnBulletCountUpdated.AddDynamic(this, &AShooterPlayerController::OnBulletCountUpdated);
        ShooterCharacter->OnDamaged.AddDynamic(this, &AShooterPlayerController::OnPawnDamaged);
        ShooterCharacter->OnDamaged.Broadcast(1.0f);
    }
}

void AShooterPlayerController::OnBoxDestroyedHandler(uint8 TeamByte, int32 Points)
{
    AddScore(TeamByte, Points);
}

void AShooterPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
    if (IsValid(BulletCounterUI))
    {
        BulletCounterUI->BP_UpdateBulletCounter(0, 0);
    }

    TArray<AActor*> ActorList;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);

    if (ActorList.Num() > 0)
    {
        AActor* RandomPlayerStart = ActorList[FMath::RandRange(0, ActorList.Num() - 1)];
        const FTransform SpawnTransform = RandomPlayerStart->GetActorTransform();

        if (AShooterCharacter* RespawnedCharacter = GetWorld()->SpawnActor<AShooterCharacter>(CharacterClass, SpawnTransform))
        {
            Possess(RespawnedCharacter);
        }
    }
}

void AShooterPlayerController::OnBulletCountUpdated(int32 MagazineSize, int32 Bullets)
{
    if (BulletCounterUI)
    {
        BulletCounterUI->BP_UpdateBulletCounter(MagazineSize, Bullets);
    }
}

void AShooterPlayerController::OnPawnDamaged(float LifePercent)
{
    if (IsValid(BulletCounterUI))
    {
        BulletCounterUI->BP_Damaged(LifePercent);
    }
}

bool AShooterPlayerController::ShouldUseTouchControls() const
{
    return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}