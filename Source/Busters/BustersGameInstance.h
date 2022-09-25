// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BustersGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BUSTERS_API UBustersGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

private:
	/*
	 * 메뉴
	 */
	UPROPERTY()
	class UStartupMenu* StartupMenu;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> StartupMenuClass;

	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget(int32 NumOfPublicConnections=4, FString TypeOfMatch=FString(TEXT("Demolition")), FString LobbyPath=FString(TEXT("/Game/Maps/LobbyMap")));
	
	
private:
	/*
	 * 온라인 서브시스템
	 */
	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll") };
	FString PathToLobby{ TEXT("") };

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

public:
	UFUNCTION(Exec)
	void Host(FString SessionName);
	UFUNCTION(Exec)
	void Join(uint32 Index);

	void RefreshServerList();
private:
	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
};
