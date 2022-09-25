// Fill out your copyright notice in the Description page of Project Settings.


#include "BustersGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "HUD/StartupMenu.h"

const static FName SESSION_NAME = TEXT("Game Session");

void UBustersGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Subsystem %s found!"), *Subsystem->GetSubsystemName().ToString());
			
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UBustersGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UBustersGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UBustersGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UBustersGameInstance::OnJoinSessionComplete);
			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Subsystem not found"));
		}
	}
}

void UBustersGameInstance::LoadMenuWidget(int32 NumOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	if (StartupMenuClass)
	{
		StartupMenu = CreateWidget<UStartupMenu>(this, StartupMenuClass);
		if (StartupMenu)
		{
			NumPublicConnections = NumOfPublicConnections;
			MatchType = TypeOfMatch;
			PathToLobby = LobbyPath;

			StartupMenu->MenuSetup();
			StartupMenu->SetBustersGameInstance(this);
		}
	}
}

void UBustersGameInstance::Host(FString SessionName)
{
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
			SessionSettings.NumPublicConnections = NumPublicConnections;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bUsesPresence = true;
			SessionSettings.bUseLobbiesIfAvailable = true;
			SessionSettings.Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), SESSION_NAME, SessionSettings);

			UE_LOG(LogTemp, Warning, TEXT("Hosting Session..."));
		}
	}
}

void UBustersGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), SESSION_NAME, SessionSearch->SearchResults[Index]);

	UE_LOG(LogTemp, Warning, TEXT("Joining Session..."));
}

void UBustersGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		SessionSearch->bIsLanQuery = false;
		SessionSearch->MaxSearchResults = 10000;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());

		UE_LOG(LogTemp, Warning, TEXT("Finding Sessions..."));
	}
}

void UBustersGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/Maps/LobbyMap");
	}
}

void UBustersGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
}

void UBustersGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && SessionSearch.IsValid() && StartupMenu != nullptr)
	{
		TArray<FSessionData> SessionData;
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			FSessionData Data;
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.HostUserName = SearchResult.Session.OwningUserName;
			FString SessionName;
			if (SearchResult.Session.SessionSettings.Get(SESSION_NAME, SessionName))
			{
				Data.Name = SessionName;
			}
			else
			{
				Data.Name = "Invalid Session Name";
			}
			SessionData.Add(Data);
		}
		StartupMenu->SetSessionScrollBox(SessionData);

		UE_LOG(LogTemp, Warning, TEXT("Find Sessions complete"));
	}
}

void UBustersGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Error OnJoinSessionComplete"));
		return;
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}
