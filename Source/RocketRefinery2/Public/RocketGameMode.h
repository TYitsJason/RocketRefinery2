// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "RocketGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ROCKETREFINERY2_API ARocketGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    ARocketGameMode();

    virtual void StartPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Game")
    void StartGame();

    UFUNCTION(BlueprintCallable, Category = "Game")
    void EndGame();

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Points);

    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 GetCurrentScore() const { return CurrentScore; }

    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 GetHighScore() const { return HighScore; }

    UFUNCTION(BlueprintCallable, Category = "Time")
    float GetRemainingTime() const { return RemainingTime; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game")
    float GameDuration;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Score")
    TArray<int32> TopScores;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
    bool bIsGameActive;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
    int32 CurrentScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
    int32 HighScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time")
    float RemainingTime;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

private:
    void UpdateHighScore();
    void SaveHighScores();
    void LoadHighScores();
    UPROPERTY()
    UUserWidget* GameOverWidget;
};
