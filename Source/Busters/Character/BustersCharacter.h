// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "BustersCharacter.generated.h"

class UCombatComponent;
class UAlsCameraComponent;
class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
class AWeapon;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class BUSTERS_API ABustersCharacter : public AAlsCharacter
{
	GENERATED_BODY()
		
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UCombatComponent> CombatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Busters Character")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Busters Character")
	TObjectPtr<UCameraComponent> ADSCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Busters Character")
	TObjectPtr<USpringArmComponent> ThirdPersonSpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Busters Character")
	TObjectPtr<UCameraComponent> ThirdPersonCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Busters Character")
	TObjectPtr<UArrowComponent> ThirdPersonArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Busters Character")
	TObjectPtr<USkeletalMeshComponent> OverlaySkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Busters Character")
	TObjectPtr<USkeletalMeshComponent> ADSSkeletalMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character")
	TObjectPtr<UInputAction> LookMouseAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character")
	TObjectPtr<UInputAction> RunAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character")
	TObjectPtr<UInputAction> ADSAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character", Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookUpMouseSensitivity{ 3.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings|Busters Character", Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookRightMouseSensitivity{ 3.0f };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Busters Character")
	float SmoothCameraSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Busters Character")
	float SmoothCameraTimeThreshold = 0.5f;
	float SmoothCameraCurrentTime = 0.f;

	UPROPERTY(BlueprintReadWrite)
	bool bADS = false;
	bool bDesiredADS = false;

public:
	ABustersCharacter();

	virtual void NotifyControllerChanged() override;

protected:
	virtual void Tick(float DeltaSeconds) override;

	// Input

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

private:
	void InputLookMouse(const FInputActionValue& ActionValue);

	void InputLook(const FInputActionValue& ActionValue);

	void InputMove(const FInputActionValue& ActionValue);

	void InputRun(const FInputActionValue& ActionValue);

	void InputCrouch();

	void InputJump(const FInputActionValue& ActionValue);

	void InputAim(const FInputActionValue& ActionValue);

	void InputADS(const FInputActionValue& ActionValue);

	void InputFire(const FInputActionValue& ActionValue);

	void InputSwitchShoulder();

private:
	void ToADSCamera(bool bToADS);
	void SmoothADSCamera(float DeltaTime);

	void EquipWeapon(AWeapon* WeaponToEquip);


	

};
