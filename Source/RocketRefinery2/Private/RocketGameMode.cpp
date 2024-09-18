// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "RocketSaveGame.h"

ARocketGameMode::ARocketGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    GameDuration = 60.0f;
    bIsGameActive = false;
    CurrentScore = 0;
    HighScore = 0;
    RemainingTime = GameDuration;
}


void ARocketGameMode::StartPlay()
{
    Super::StartPlay();

    check(GEngine != nullptr);

    LoadHighScores();
}

void ARocketGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsGameActive)
    {
        RemainingTime -= DeltaTime;
        if (RemainingTime <= 0)
        {
            EndGame();
        }
    }
}

void ARocketGameMode::StartGame()
{
    bIsGameActive = true;
    CurrentScore = 0;
    RemainingTime = GameDuration;
}

void ARocketGameMode::EndGame()
{
    bIsGameActive = false;
    UpdateHighScore();
    SaveHighScores();
    if (GameOverWidgetClass)
    {
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
        if (PlayerController)
        {
            GameOverWidget = CreateWidget<UUserWidget>(PlayerController, GameOverWidgetClass);
            if (GameOverWidget)
            {
                GameOverWidget->AddToViewport();
                PlayerController->SetShowMouseCursor(true);
                PlayerController->SetInputMode(FInputModeUIOnly());
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameOverWidgetClass is not set in the GameMode Blueprint!"));
    }
}

void ARocketGameMode::AddScore(int32 Points)
{
    if (bIsGameActive)
    {
        CurrentScore += Points;
    }
}


void ARocketGameMode::UpdateHighScore()
{
    if (CurrentScore > HighScore)
    {
        HighScore = CurrentScore;
    }

    // Update TopScores array
    TopScores.Add(CurrentScore);
    TopScores.Sort([](const int32& A, const int32& B) { return A > B; });

    // Keep only top 10 scores
    if (TopScores.Num() > 10)
    {
        TopScores.SetNum(10);
    }
}

void ARocketGameMode::SaveHighScores()
{
    URocketSaveGame* SaveGameInstance = Cast<URocketSaveGame>(UGameplayStatics::CreateSaveGameObject(URocketSaveGame::StaticClass()));
    if (SaveGameInstance)
    {
        SaveGameInstance->TopScores = TopScores;
        SaveGameInstance->HighScore = HighScore;

        UGameplayStatics::SaveGameToSlot(SaveGameInstance, "HighScores", 0);
    }
}

void ARocketGameMode::LoadHighScores()
{
    if (UGameplayStatics::DoesSaveGameExist("HighScores", 0))
    {
        URocketSaveGame* LoadedGame = Cast<URocketSaveGame>(UGameplayStatics::LoadGameFromSlot("HighScores", 0));
        if (LoadedGame)
        {
            TopScores = LoadedGame->TopScores;
            HighScore = LoadedGame->HighScore;
        }
    }
}