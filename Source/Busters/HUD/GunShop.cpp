// Fill out your copyright notice in the Description page of Project Settings.


#include "GunShop.h"

#include "Busters/Character/BustersCharacter.h"
#include "Busters/Weapon/Weapon.h"
#include "Components/Button.h"

UGunShop::UGunShop(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool UGunShop::Initialize()
{
	if (!Super::Initialize()) return false;

	if(IsValid(BuyARButton))
	{
		BuyARButton->OnClicked.AddDynamic(this, &UGunShop::BuyAR);
	}
	if (IsValid(BuyAKButton))
	{
		BuyAKButton->OnClicked.AddDynamic(this, &UGunShop::BuyAK);
	}

	return true;
}

void UGunShop::MenuSetup()
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
}

void UGunShop::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	MenuTearDown();
}

void UGunShop::MenuTearDown()
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

void UGunShop::BuyAR()
{
	TObjectPtr<APlayerController> PlayerController = GetOwningPlayer();
	if(IsValid(PlayerController))
	{
		TObjectPtr<ABustersCharacter> Character = Cast<ABustersCharacter>(PlayerController->GetCharacter());
		TObjectPtr<UWorld> World = GetWorld();
		if(IsValid(World) && IsValid(Character))
		{
			TObjectPtr<AWeapon> NewWeapon = World->SpawnActor<AWeapon>(WeaponAR);
			Character->EquipWeapon(NewWeapon);
		}
	}
}

void UGunShop::BuyAK()
{
	TObjectPtr<APlayerController> PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		TObjectPtr<ABustersCharacter> Character = Cast<ABustersCharacter>(PlayerController->GetCharacter());
		TObjectPtr<UWorld> World = GetWorld();
		if (IsValid(World) && IsValid(Character))
		{
			TObjectPtr<AWeapon> NewWeapon = World->SpawnActor<AWeapon>(WeaponAK);
			Character->EquipWeapon(NewWeapon);
		}
	}
}


