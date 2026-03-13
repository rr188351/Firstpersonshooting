#include "Boxspawner.h"
#include "MyClass.h" // BoxActor
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "ShooterGameMode.h"
#include "Particles/ParticleSystem.h"

ABoxspawner::ABoxspawner()
{
    PrimaryActorTick.bCanEverTick = false;// ✅ disable tick
    static ConstructorHelpers::FClassFinder<AMyClass> BoxBPClass(TEXT("/Game/Characters/boxactor.boxactor_C"));
    if (BoxBPClass.Succeeded())
    {
        BoxClass = BoxBPClass.Class; // ✅ BP assign ho gaya
    }

}

void ABoxspawner::BeginPlay()
{
    Super::BeginPlay();

    FString URL = TEXT("https://raw.githubusercontent.com/CyrusCHAU/Varadise-Technical-Test/refs/heads/main/data.json");

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &ABoxspawner::OnBoxDataResponse);
    Request->SetURL(URL);
    Request->SetVerb("GET");
    Request->ProcessRequest();
}

void ABoxspawner::OnBoxDataResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP Request failed!"));
        return;
    }

    FString JsonString = Response->GetContentAsString();
    TSharedPtr<FJsonObject> RootObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("JSON Parse failed!"));
        return;
    }

    // 🔹 Build type lookup
    TMap<FString, FLinearColor> TypeColors;
    TMap<FString, int32> TypeHealth;
    TMap<FString, int32> TypeScore;

    const TArray<TSharedPtr<FJsonValue>>* TypesArray = nullptr;
    if (RootObject->TryGetArrayField(TEXT("types"), TypesArray) && TypesArray)
    {
        for (auto& TypeVal : *TypesArray)
        {
            TSharedPtr<FJsonObject> TypeObj = TypeVal->AsObject();
            if (!TypeObj.IsValid()) continue;

            FString Name = TypeObj->GetStringField("name");

            const TArray<TSharedPtr<FJsonValue>>* ColorArray = nullptr;
            if (TypeObj->TryGetArrayField("color", ColorArray) && ColorArray && ColorArray->Num() >= 3)
            {
                FLinearColor ParsedColor(
                    (*ColorArray)[0]->AsNumber() / 255.0f,
                    (*ColorArray)[1]->AsNumber() / 255.0f,
                    (*ColorArray)[2]->AsNumber() / 255.0f
                );
                TypeColors.Add(Name, ParsedColor);
            }

            TypeHealth.Add(Name, TypeObj->GetIntegerField("health"));
            TypeScore.Add(Name, TypeObj->GetIntegerField("score"));

            UE_LOG(LogTemp, Warning, TEXT("Parsed type: %s"), *Name);
        }
    }

    // 🔹 Spawn objects
    const TArray<TSharedPtr<FJsonValue>>* ObjectsArray = nullptr;
    if (RootObject->TryGetArrayField(TEXT("objects"), ObjectsArray) && ObjectsArray)
    {
        for (auto& ObjVal : *ObjectsArray)
        {
            TSharedPtr<FJsonObject> Obj = ObjVal->AsObject();
            if (!Obj.IsValid()) continue;

            FString TypeName = Obj->GetStringField("type");
            UE_LOG(LogTemp, Warning, TEXT("Spawning object of type: %s"), *TypeName);

            TSharedPtr<FJsonObject> TransformObj = Obj->GetObjectField("transform");

            // Location
            FVector Location = FVector::ZeroVector;
            const TArray<TSharedPtr<FJsonValue>>* LocArray = nullptr;
            if (TransformObj->TryGetArrayField("location", LocArray) && LocArray && LocArray->Num() >= 3)
            {
                Location = FVector(
                    (*LocArray)[0]->AsNumber(),
                    (*LocArray)[1]->AsNumber(),
                    (*LocArray)[2]->AsNumber()+400.0f // ✅ thoda height add kar do
                );
            }

            // Rotation
            FRotator Rotation = FRotator::ZeroRotator;
            const TArray<TSharedPtr<FJsonValue>>* RotArray = nullptr;
            if (TransformObj->TryGetArrayField("rotation", RotArray) && RotArray && RotArray->Num() >= 3)
            {
                Rotation = FRotator(
                    (*RotArray)[0]->AsNumber(),
                    (*RotArray)[1]->AsNumber(),
                    (*RotArray)[2]->AsNumber()
                );
            }

            // Scale
            FVector Scale = FVector(1.0f);
            const TArray<TSharedPtr<FJsonValue>>* ScaleArray = nullptr;
            if (TransformObj->TryGetArrayField("scale", ScaleArray) && ScaleArray && ScaleArray->Num() >= 3)
            {
                Scale = FVector(
                    (*ScaleArray)[0]->AsNumber(),
                    (*ScaleArray)[1]->AsNumber(),
                    (*ScaleArray)[2]->AsNumber()
                );
            }

            // Final transform
            FTransform SpawnTransform(Rotation, Location, Scale);

            // ✅ Spawn Box using Blueprint class
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            AMyClass* SpawnedBox = GetWorld()->SpawnActor<AMyClass>(
                BoxClass,        // ✅ Blueprint class assign hoga
                SpawnTransform,
                Params
            );

            if (SpawnedBox)
            {
                SpawnedBox->BoxTypeName = TypeName;
                UE_LOG(LogTemp, Warning, TEXT("Spawned Box: %s Sound=%s Effect=%s"),*SpawnedBox->GetName(),SpawnedBox->DestroySound ? *SpawnedBox->DestroySound->GetName() : TEXT("NULL"),SpawnedBox->DestroyEffect ? *SpawnedBox->DestroyEffect->GetName() : TEXT("NULL"));


                if (TypeColors.Contains(TypeName) && TypeHealth.Contains(TypeName) && TypeScore.Contains(TypeName))
                {
                    SpawnedBox->ApplyBoxStats(TypeColors[TypeName], TypeHealth[TypeName], TypeScore[TypeName]);
                }
                SpawnedBox->DestroySound = DefaultDestroySound;
                SpawnedBox->DestroyEffect = DefaultDestroyEffect;

                if (AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode()))
                {
                    SpawnedBox->OnBoxDestroyed.AddDynamic(GM, &AShooterGameMode::IncrementTeamScore);
                }
            }
        
        }
    }
}

void ABoxspawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}