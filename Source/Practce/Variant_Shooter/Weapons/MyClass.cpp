#include "MyClass.h"

#include "ShooterGameMode.h"
#include "ShooterPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

AMyClass::AMyClass()
{
    PrimaryActorTick.bCanEverTick = true;

    BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
    RootComponent = BoxMesh;

    // ✅ Collision enable karo
    BoxMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxMesh->SetCollisionResponseToAllChannels(ECR_Ignore);   // sab ignore
    BoxMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // ✅ sirf trace block
    BoxMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block); // optional


    // Default cube mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (MeshAsset.Succeeded())
    {
        BoxMesh->SetStaticMesh(MeshAsset.Object);
    }

    // ✅ Assign your material instance here
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

    // Apply initial color
    if (BoxMesh)
    {
        UMaterialInstanceDynamic* DynMat = BoxMesh->CreateAndSetMaterialInstanceDynamic(0);
        if (DynMat)
        {
            DynMat->SetVectorParameterValue("Base Color", BoxColor); // ✅ match parameter name
        }
    }
}

void AMyClass::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Simple oscillating movement along X axis
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
}
// MyClass.cpp

float AMyClass::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                           AController* EventInstigator, AActor* DamageCauser)
{
    Health -= (int32)DamageAmount;

    if (Health <= 0)
    {
        if (AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
        {
            GM->IncrementTeamScore(TeamByte, Score);
        }
        Destroy();
    }
    return DamageAmount;
}




void AMyClass::ApplyBoxStats(const FLinearColor& InColor, int32 InHealth, int32 InScore)
{
    BoxColor = InColor;
    Health = InHealth;
    Score = InScore;

    if (BoxMesh)
    {
        UMaterialInstanceDynamic* DynMat = BoxMesh->CreateAndSetMaterialInstanceDynamic(0);
        if (DynMat)
        {
            DynMat->SetVectorParameterValue("Base Color", BoxColor); // ✅ same parameter name
        }
    }
}
void AMyClass::Destroyed()
{
    Super::Destroyed();
    UE_LOG(LogTemp, Warning, TEXT("Box destroyed: playing sound/effect"));
    // ✅ Play sound
    if (DestroySound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
    }

    // ✅ Spawn particle effect
    if (DestroyEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorTransform());
    }

    Tags.Add(DeathTag);
    

    // Score broadcast
    OnBoxDestroyed.Broadcast(TeamByte, Score);


     // ✅ Broadcast with actual score
}