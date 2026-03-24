#include "MyClass.h"

#include "Healthbar.h"
#include "ShooterGameMode.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"


AMyClass::AMyClass()
{
    PrimaryActorTick.bCanEverTick = true;

    BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
    RootComponent = BoxMesh;
    HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthBarComponent->SetupAttachment(RootComponent);

    HealthBarWidget = Cast<UHealthbar>(HealthBarComponent->GetUserWidgetObject());
    HealthBarComponent->SetAbsolute(false, true, false); 
    HealthBarComponent->SetWidgetSpace(EWidgetSpace::World);
    HealthBarComponent->SetDrawSize(FVector2D(200.f, 20.f));


    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (MeshAsset.Succeeded())
    {
        BoxMesh->SetStaticMesh(MeshAsset.Object);
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInstance> MatAsset(TEXT("/Game/LevelPrototyping/Materials/MI_DefaultColorway.MI_DefaultColorway"));
    if (MatAsset.Succeeded())
    {
        BoxMesh->SetMaterial(0, MatAsset.Object);
    }
    

}


void AMyClass::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();

    DynMat = BoxMesh->CreateAndSetMaterialInstanceDynamic(0);
    if (HealthBarComponent)
    {
        // ✅ Widget ko apne UHealthbar class me cast karo
        HealthBarWidget = Cast<UHealthbar>(HealthBarComponent->GetUserWidgetObject());
        HealthBarComponent->SetWorldRotation(FRotator::ZeroRotator);
        
        UpdateHealthBar();
    }

    if (DynMat)
    {
        DynMat->SetVectorParameterValue("Base Color", BoxColor);
        DynMat->SetScalarParameterValue("Dissolve", 0.0f);
    }
}

void AMyClass::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector CurrentLocation = GetActorLocation();

    if (bMovingForward)
    {
        CurrentLocation.X += FlySpeed * DeltaTime;
        if (FMath::Abs(CurrentLocation.X - StartLocation.X) >= FlyRange)
        {
            bMovingForward = false;
        }
    }
    else
    {
        CurrentLocation.X -= FlySpeed * DeltaTime;
        if (FMath::Abs(CurrentLocation.X - StartLocation.X) <= 10.0f)
        {
            bMovingForward = true;
        }
    }

    SetActorLocation(CurrentLocation);
    if (HealthBarComponent)
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC && PC->PlayerCameraManager)
        {
            FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
            FRotator LookAtRotation = (CameraLocation - HealthBarComponent->GetComponentLocation()).Rotation();
            HealthBarComponent->SetWorldRotation(LookAtRotation);
        }
    }
}


void AMyClass::UpdateHealthBar()
{
    if (HealthBarWidget)
    {
        float Percent = (float)Health / (float)MaxHealth;   // ✅ direct Health use
        HealthBarWidget->SetHealthPercent(Percent);
    }
}


float AMyClass::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                           AController* EventInstigator, AActor* DamageCauser)
{
    if (bDead) 
    {
        // ✅ Already dead → ignore further damage
        return 0.0f;
    }

    CurrentHealth -= (int32)DamageAmount;
    if (CurrentHealth < 0) CurrentHealth = 0;

    if (HealthBarWidget)
    {
        HealthBarWidget->SetHealthPercent((float)CurrentHealth / (float)MaxHealth);
    }

    if (CurrentHealth <= 0)
    {
        if (AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
        {
            GM->IncrementTeamScore(1,Score);
            GM->IncrementTeamScore(2,1);
            
        }
        bDead = true;// ✅ Mark dead once
        OnDeath();
        // ✅ Trigger dissolve only once
    }

    return DamageAmount;
}
void AMyClass::OnDeath_Implementation()
{
    // ✅ Default C++ behavior
    Destroy();
}

void AMyClass::ApplyBoxStats(const FLinearColor& InColor, int32 InHealth, int32 InScore)
{
    BoxColor = InColor;
    Health = InHealth;
    Score = InScore;
    MaxHealth = Health;
    CurrentHealth = Health;

    // ✅ Healthbar ko initial percent set karo
    if (HealthBarWidget)
    {
        HealthBarWidget->SetHealthPercent((float)CurrentHealth / (float)MaxHealth);
    }


    if (BoxMesh)
    {
        DynMat = BoxMesh->CreateAndSetMaterialInstanceDynamic(0);
        if (DynMat)
        {
            DynMat->SetVectorParameterValue("Base Color", BoxColor);
        }
    }
}

void AMyClass::Destroyed()
{
    Super::Destroyed();
    SetActorEnableCollision(false);
    if (DestroySound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
    }

    if (DestroyEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorTransform());
    }


    // ✅ Actor is already marked for destruction, no need to call Destroy() again
}