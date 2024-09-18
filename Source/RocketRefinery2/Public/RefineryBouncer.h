// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RefineryBouncer.generated.h"

UCLASS()
class ROCKETREFINERY2_API ARefineryBouncer : public AActor
{
	GENERATED_BODY()
	
public:	
	ARefineryBouncer();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bouncer")
	float BounceForceMagnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bouncer")
	int32 ScoreMultiplierIncrease;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
		class UStaticMeshComponent* BouncerMesh;
};
