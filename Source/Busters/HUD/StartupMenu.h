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

	UFUNCTION()
	void MenuSetup();
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
	
private:
	/*
	 * 세션 스크롤 박스
	 */
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
	UFUNCTION()
	void SetSessionScrollBox(TArray<FSessionData>& SessionData);
	
private:
	/*
	 * 버튼 온클릭 함수
	 */
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

	
private:
	/*
	 * 온라인 서브시스템
	 */
	UPROPERTY()
	class UBustersGameInstance* BustersGameInstance;

public:
	FORCEINLINE void SetBustersGameInstance(UBustersGameInstance* Instance) { BustersGameInstance = Instance; }
	
};
