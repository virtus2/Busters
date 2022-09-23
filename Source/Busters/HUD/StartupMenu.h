// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "StartupMenu.generated.h"

USTRUCT()
struct FSessionData
{
	GENERATED_BODY()

	FString Name;
	int32 CurrentPlayers;
	int32 MaxPlayers;
	FString HostUserName;
};

/**
 *
 */
UCLASS()
class BUSTERS_API UStartupMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UStartupMenu(const FObjectInitializer& ObjectInitializer);
	// 블루프린트에서 호출하는 메뉴 시작 함수
	UFUNCTION(BlueprintCallable, Category = Widget)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/Maps/StartingMap")));
protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
private:
	void MenuTearDown();
private:
	// 메뉴 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;
	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;


	/*
	 * 세션 스크롤 박스
	 */
private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LoadingText;
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* SessionScrollBox;

	// 세션 스크롤박스와 메뉴 전환
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	// 세션 스크롤박스에 추가할 클래스
	TSubclassOf<class UUserWidget> SessionScrollBoxItemClass;
public:
	UFUNCTION()
		void SelectSessionItem(uint32 Index);

	/*
	 * 버튼 온클릭 함수
	 */
private:
	UFUNCTION()
	void JoinButtonClicked();
	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void QuitButtonClicked();

	UFUNCTION()
	void OpenSessionScrollBoxMenu();
	UFUNCTION()
	void OpenStartupMenu();

	/*
	 * 온라인 서브시스템
	 */
private:

public:
	UFUNCTION()
		void SetSessionScrollBox(TArray<FSessionData>& SessionData);
};
