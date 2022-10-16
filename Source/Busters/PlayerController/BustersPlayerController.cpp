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
