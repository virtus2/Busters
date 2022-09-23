// Fill out your copyright notice in the Description page of Project Settings.

#include "SessionScrollBoxItem.h"
#include "StartupMenu.h"
#include "Components/Button.h"


void USessionScrollBoxItem::Setup(UStartupMenu* Menu, uint32 Index)
{
	StartupMenu = Menu;
	ItemIndex = Index;

	ItemButton->OnClicked.AddDynamic(this, &USessionScrollBoxItem::ItemButtonClicked);
}

void USessionScrollBoxItem::ItemButtonClicked()
{
	StartupMenu->SelectSessionItem(ItemIndex);
}