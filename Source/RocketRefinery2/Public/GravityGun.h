// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GravityGun.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROCKETREFINERY2_API UGravityGun : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGravityGun();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Functions to handle gravity gun actions
	void TryGrab();
	void Release();
	void Launch();
	void SetPhysicsHandle(UPhysicsHandleComponent* NewPhysicsHandle);
	void UpdateGunTransform();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	FVector GunOffset = FVector(50.0f, 50.0f, -20.0f);  // Offset to the right and slightly down

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float HeldObjectDistance = 200.0f;  // Distance of held object from camera

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	FVector HeldObjectOffset = FVector(0.0f, 0.0f, 0.0f);  // Offset for fine-tuning held object position

	// Configurable properties
	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float GrabDistance = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float HoldDistance = 200.f;

	UPROPERTY(EditAnywhere, Category = "Gravity Gun")
	float LaunchForce = 5000.f;

	float OriginalLinearDamping;
	float OriginalAngularDamping;
	bool OriginalGravityEnabled;


	void UpdateHeldObjectPosition();

	// Helper functions
	bool IsHolding() const;
	FHitResult TraceForObjects() const;
};
