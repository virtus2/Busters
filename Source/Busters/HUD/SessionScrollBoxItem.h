// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionScrollBoxItem.generated.h"

/**
 * 
 */
UCLASS()
class BUSTERS_API USessionScrollBoxItem : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void Setup(class UStartupMenu* Menu, uint32 Index);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SessionNameText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HostNameText;
	UPROPERTY(meta = (BindWidget))
	class UButton* ItemButton;

private:
	UFUNCTION()
	void ItemButtonClicked();

	UPROPERTY()
	UStartupMenu* StartupMenu;

	uint32 ItemIndex;
};
