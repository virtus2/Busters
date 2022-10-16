// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BustersPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UGunShop;
/**
 * 
 */
UCLASS()
class BUSTERS_API ABustersPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> GunShop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInputAction> ShopAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	bool bShopping = false;
private:
	UPROPERTY()
		TObjectPtr<UGunShop> OpenedGunShop;

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:
	void InputShop();
	
};
