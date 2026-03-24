#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "MyClass.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBoxDestroyed, uint8, TeamByte, int32, Points);

class UHealthbar; 
UCLASS()
class PRACTCE_API AMyClass : public AActor
{
    GENERATED_BODY()

public:
    AMyClass();

    // ✅ Mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Box")
    UStaticMeshComponent* BoxMesh;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Box Stats")
    FString BoxTypeName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Box Stats")
    int32 CurrentHealth;


    // ✅ Effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
    USoundBase* DestroySound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
    UParticleSystem* DestroyEffect;

    // ✅ Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Box Stats")
    int32 Health = 100;   // ✅ ek hi health rakho

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Box Stats")
    int32 MaxHealth = 100;
    UPROPERTY()
    bool bDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Box Stats")
    int32 Score = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Box Stats")
    FLinearColor BoxColor = FLinearColor::Red;

    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnBoxDestroyed OnBoxDestroyed;
    UFUNCTION(BlueprintNativeEvent, Category="Box")
    void OnDeath();
    virtual void OnDeath_Implementation();  // Default C++ impl
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
    UWidgetComponent* HealthBarComponent;

    UPROPERTY()
    UHealthbar* HealthBarWidget;


    




    // ✅ Movement
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float FlySpeed = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float FlyRange = 500.0f;
    // ✅ Dissolve curve property
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
    UCurveFloat* DissolveCurve;

protected:
    virtual void BeginPlay() override;
    virtual void Destroyed() override;

    FVector StartLocation;
    bool bMovingForward = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Team")
    uint8 TeamByte = 1;
    

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Team")
    FName DeathTag = FName("Dead");
    


    UPROPERTY()
    UMaterialInstanceDynamic* DynMat;

public:
    virtual void Tick(float DeltaTime) override;

    // ✅ BP callable
    UFUNCTION(BlueprintCallable, Category="Box")
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                             AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable, Category="Box")
    void ApplyBoxStats(const FLinearColor& InColor, int32 InHealth, int32 InScore);
    void UpdateHealthBar();

};