// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RocketSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETREFINERY2_API URocketSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
    URocketSaveGame();

    UPROPERTY(VisibleAnywhere, Category = "Score")
    TArray<int32> TopScores;

    UPROPERTY(VisibleAnywhere, Category = "Score")
    int32 HighScore;
};
