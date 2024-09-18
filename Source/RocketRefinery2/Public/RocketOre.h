// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RocketOre.generated.h"

UCLASS()
class ROCKETREFINERY2_API ARocketOre : public AActor
{
	GENERATED_BODY()
	
public:
    ARocketOre();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ore")
    int32 BaseScoreValue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ore")
    int32 CurrentMultiplier;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    class UStaticMeshComponent* OreMesh;

    UFUNCTION(BlueprintCallable, Category = "Ore")
    void ResetMultiplier();

    UFUNCTION(BlueprintCallable, Category = "Ore")
    void IncreaseMultiplier(int32 Increase);

    UFUNCTION(BlueprintCallable, Category = "Ore")
    int32 GetCurrentScore() const;

    UFUNCTION(BlueprintCallable, Category = "Ore")
    void CollectOre();
};
