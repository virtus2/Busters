// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Busters/BustersGameInstance.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();
	UBustersGameInstance* BustersGameInstance = Cast<UBustersGameInstance>(GetGameInstance());
	if(BustersGameInstance)
	{
		if(NumOfPlayers >= 2)//BustersGameInstance->NumPublicConnections)
		{
			UWorld* World = GetWorld();
			if(World)
			{
				bUseSeamlessTravel = true;
				FString MatchType = BustersGameInstance->MatchType;
				if(MatchType == "Demolition")
				{
					World->ServerTravel(FString("/Game/Maps/DemolitionMap?listen"));
				}
			}
		}
	}
}
