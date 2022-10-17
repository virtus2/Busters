// Fill out your copyright notice in the Description page of Project Settings.


#include "BustersPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Busters/HUD/GunShop.h"

void ABustersPlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto* EnhancedInputSubsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()) };
	if(EnhancedInputSubsystem)
	{
		EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void ABustersPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	auto* EnhancedInput{ Cast<UEnhancedInputComponent>(InputComponent) };
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(ShopAction, ETriggerEvent::Triggered, this, &ThisClass::InputShop);
	}
}

void ABustersPlayerController::InputShop()
{
	UE_LOG(LogTemp, Warning, TEXT("Input Shop"));
	if(bShopping)
	{
		bShopping = false;
		// GunShop 메뉴를 닫는건 BP_GunShop에서 처리된다.
		// 이유: InputModeUIOnly에서 키보드 인풋이 EnhancedInput에 전달되지 않음
	}
	else
	{
		bShopping = true;
		if (IsValid(GunShop))
		{
			OpenedGunShop = Cast<UGunShop>(CreateWidget<UUserWidget>(this, GunShop));
			if(IsValid(OpenedGunShop))
			{
				OpenedGunShop->MenuSetup();
			}
		}
	}
}
