// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GunShop.generated.h"

class UButton;
class AWeapon;
/**
 * 
 */
UCLASS()
class BUSTERS_API UGunShop : public UUserWidget
{
	GENERATED_BODY()

protected:
	UGunShop(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AWeapon> WeaponAR;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AWeapon> WeaponAK;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BuyARButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BuyAKButton;

	UFUNCTION()
	void BuyAR();

	UFUNCTION()
	void BuyAK();

public:
	UFUNCTION()
	void MenuSetup();

	UFUNCTION(BlueprintCallable)
	void MenuTearDown();
};

