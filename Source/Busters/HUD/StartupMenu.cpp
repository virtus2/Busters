// Fill out your copyright notice in the Description page of Project Settings.


#include "StartupMenu.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "SessionScrollBoxItem.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

UStartupMenu::UStartupMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> SessionScrollBoxItemBPClass(TEXT("WidgetBlueprint'/Game/Blueprints/HUD/WBP_SessionScrollBoxItem'"));
	if (SessionScrollBoxItemBPClass.Succeeded() && SessionScrollBoxItemBPClass.Class != nullptr) 
		SessionScrollBoxItemClass = SessionScrollBoxItemBPClass.Class;
}

void UStartupMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			bIsFocusable = true;
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	/*
	// 온라인 서브시스템
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("SessionInterface found!"));
		}
	}
	*/
}

bool UStartupMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UStartupMenu::OpenSessionScrollBoxMenu);
	}
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UStartupMenu::HostButtonClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UStartupMenu::QuitButtonClicked);
	}
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UStartupMenu::OpenStartupMenu);
	}

	return true;
}

void UStartupMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	MenuTearDown();
}

void UStartupMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}

void UStartupMenu::SelectSessionItem(uint32 Index)
{

}

void UStartupMenu::JoinButtonClicked()
{

}

void UStartupMenu::HostButtonClicked()
{
	/*
	if (BustersGameInstance)
	{
		BustersGameInstance->Host("Game Session");
	}
	*/
}

void UStartupMenu::QuitButtonClicked()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			PlayerController->ConsoleCommand("quit");
		}
	}
}

void UStartupMenu::OpenStartupMenu()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}
}

void UStartupMenu::OpenSessionScrollBoxMenu()
{
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(1);

		if (LoadingText)
		{
			LoadingText->SetVisibility(ESlateVisibility::Visible);
		}

		/*
		if (BustersGameInstance)
		{
			BustersGameInstance->RefreshServerList();
		}
		*/
	}
}

void UStartupMenu::SetSessionScrollBox(TArray<FSessionData>& SessionData)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	SessionScrollBox->ClearChildren();

	for (const auto& Data : SessionData)
	{
		USessionScrollBoxItem* Item = CreateWidget<USessionScrollBoxItem>(World, SessionScrollBoxItemClass);
		if (Item == nullptr) return;

		/*
		Item->SessionNameText->SetText(FText::FromString(Data.Name));
		Item->HostNameText->SetText(FText::FromString(Data.HostUserName));
		Item->SessionNameText->SetText(FText::FromString(Data.Name));
		*/
		SessionScrollBox->AddChild(Item);
	}

	if (LoadingText)
	{
		LoadingText->SetVisibility(ESlateVisibility::Hidden);
	}
}
