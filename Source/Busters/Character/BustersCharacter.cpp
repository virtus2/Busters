// Fill out your copyright notice in the Description page of Project Settings.

#include "BustersCharacter.h"

#include "Busters/BustersComponents/CombatComponent.h"
#include "AlsCameraComponent.h"
#include "AlsCharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Busters/Weapon/Weapon.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ABustersCharacter::ABustersCharacter()
{
	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);

	OverlaySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("OverlaySkeletalMesh");
	OverlaySkeletalMesh->SetupAttachment(GetMesh());
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Cast<USceneComponent>(GetCapsuleComponent()));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = 0.f;

	ThirdPersonSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonSpringArm"));
	ThirdPersonSpringArm->SetupAttachment(GetMesh());
	ThirdPersonSpringArm->bUsePawnControlRotation = true;
	ThirdPersonSpringArm->SetRelativeLocation({ -30,0,175 });

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(ThirdPersonSpringArm);
	FieldOfView = ThirdPersonCamera->FieldOfView;

	ThirdPersonArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ThirdPersonArrow"));
	ThirdPersonArrow->SetupAttachment(ThirdPersonSpringArm);

	ADSSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ADSSkeletalMesh"));
	ADSSkeletalMesh->SetupAttachment(SpringArm);
	ADSSkeletalMesh->SetOnlyOwnerSee(true);
	ADSSkeletalMesh->SetVisibility(false);

	ADSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ADSCamera"));
	ADSCamera->SetupAttachment(SpringArm);
	
}

void ABustersCharacter::NotifyControllerChanged()
{
	const auto* PreviousPlayer{ Cast<APlayerController>(PreviousController) };
	if (IsValid(PreviousPlayer))
	{
		auto* EnhancedInputSubsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPlayer->GetLocalPlayer()) };
		if (IsValid(EnhancedInputSubsystem))
		{
			EnhancedInputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}

	auto* NewPlayer{ Cast<APlayerController>(GetController()) };
	if (IsValid(NewPlayer))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		auto* EnhancedInputSubsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer()) };
		if (IsValid(EnhancedInputSubsystem))
		{
			EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	Super::NotifyControllerChanged();

}

void ABustersCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SmoothADSCamera(DeltaSeconds);
}

void ABustersCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	auto* EnhancedInput{ Cast<UEnhancedInputComponent>(Input) };
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::InputLookMouse);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		EnhancedInput->BindAction(RunAction, ETriggerEvent::Triggered, this, &ThisClass::InputRun);
		EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::InputCrouch);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::InputJump);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::InputAim);
		EnhancedInput->BindAction(ADSAction, ETriggerEvent::Triggered, this, &ThisClass::InputADS);
		EnhancedInput->BindAction(FireAction, ETriggerEvent::Triggered, this, &ThisClass::InputFire);
	}
}

void ABustersCharacter::InputLookMouse(const FInputActionValue& ActionValue)
{
	const auto Value{ ActionValue.Get<FVector2D>() };

	AddControllerPitchInput(Value.Y * LookUpMouseSensitivity);
	AddControllerYawInput(Value.X * LookRightMouseSensitivity);
	
}

void ABustersCharacter::InputLook(const FInputActionValue& ActionValue)
{
	const auto Value{ ActionValue.Get<FVector2D>() };
}

void ABustersCharacter::InputMove(const FInputActionValue& ActionValue)
{
	const auto Value{ UAlsMath::ClampMagnitude012D(ActionValue.Get<FVector2D>()) };

	const auto ForwardDirection{ UAlsMath::AngleToDirectionXY(UE_REAL_TO_FLOAT(GetViewState().Rotation.Yaw)) };
	const auto RightDirection{ UAlsMath::PerpendicularCounterClockwiseXY(ForwardDirection) };

	AddMovementInput(ForwardDirection * Value.Y + RightDirection * Value.X);
}

void ABustersCharacter::InputRun(const FInputActionValue& ActionValue)
{
	if (bADS) ToADSCamera(false);
	SetDesiredGait(ActionValue.Get<bool>() ? AlsGaitTags::Running : AlsGaitTags::Walking);
}

void ABustersCharacter::InputCrouch()
{
	if (GetDesiredStance() == AlsStanceTags::Standing)
	{
		SetDesiredStance(AlsStanceTags::Crouching);
	}
	else if (GetDesiredStance() == AlsStanceTags::Crouching)
	{
		SetDesiredStance(AlsStanceTags::Standing);
	}
}

void ABustersCharacter::InputJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
	{
		if (GetStance() == AlsStanceTags::Crouching)
		{
			SetDesiredStance(AlsStanceTags::Standing);
			return;
		}

		Jump();
		ToADSCamera(false);
	}
	else
	{
		StopJumping();
	}
}

void ABustersCharacter::InputAim(const FInputActionValue& ActionValue)
{

}

void ABustersCharacter::InputADS(const FInputActionValue& ActionValue)
{
	UE_LOG(LogTemp, Warning, TEXT("InputADS called"));
	// if (IsValid(CombatComponent) && CombatComponent->GetWeapon().IsNull()) return;

	if (GetDesiredGait() == AlsGaitTags::Running) SetDesiredGait(AlsGaitTags::Walking);

	if (bADS) ToADSCamera(false);
	else ToADSCamera(true);
}

void ABustersCharacter::InputFire(const FInputActionValue& ActionValue)
{
	if(ActionValue.Get<bool>())
	{
		UE_LOG(LogTemp, Warning, TEXT("True"));
		if(IsValid(CombatComponent))
		{

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("False"));
	}
}

void ABustersCharacter::InputSwitchShoulder()
{
}

void ABustersCharacter::ToADSCamera(bool bToADS)
{
	SmoothCameraCurrentTime = 0.f;
	if (bToADS)
	{
		// 카메라를 ThirdPerson에서 ADS로 변경
		UE_LOG(LogTemp, Warning, TEXT("ADS on"));

		bDesiredADS = true;
		SetViewMode(AlsViewModeTags::FirstPerson);

		if (IsValid(OverlaySkeletalMesh)) OverlaySkeletalMesh->SetVisibility(false);
		if (GetMesh()) GetMesh()->SetVisibility(false);
		if (IsValid(ADSSkeletalMesh)) ADSSkeletalMesh->SetVisibility(true);

	}
	else
	{
		// 카메라를 ADS에서 ThirdPerson으로 변경
		UE_LOG(LogTemp, Warning, TEXT("ADS off"));

		bDesiredADS = false;
		ADSCamera->SetActive(false);
		ThirdPersonCamera->SetActive(true);
		SetViewMode(AlsViewModeTags::ThirdPerson);

		if (IsValid(OverlaySkeletalMesh)) OverlaySkeletalMesh->SetVisibility(true);
		if (GetMesh()) GetMesh()->SetVisibility(true);
		if (IsValid(ADSSkeletalMesh)) ADSSkeletalMesh->SetVisibility(false);
	}
}

void ABustersCharacter::SmoothADSCamera(float DeltaTime)
{
	if(bDesiredADS && !bADS)
	{
		SmoothCameraCurrentTime += DeltaTime;
		// 카메라를 ThirdPerson에서 ADS로 변경
		if (IsValid(ThirdPersonCamera) && IsValid(ADSCamera))
		{
			FVector NewLocation = UKismetMathLibrary::VInterpTo(ThirdPersonCamera->GetComponentLocation(), ADSCamera->GetComponentLocation(), DeltaTime, SmoothCameraSpeed);
			ThirdPersonCamera->SetWorldLocation(NewLocation);

			float NewFOV = UKismetMathLibrary::FInterpTo(ThirdPersonCamera->FieldOfView, ADSCamera->FieldOfView, DeltaTime, SmoothCameraSpeed);
			ThirdPersonCamera->SetFieldOfView(NewFOV);

			if (UKismetMathLibrary::VSize(ThirdPersonCamera->GetComponentLocation() - ADSCamera->GetComponentLocation()) < 0.5f || 
				SmoothCameraCurrentTime >= SmoothCameraTimeThreshold)
			{
				// 변경 완료
				UE_LOG(LogTemp, Warning, TEXT("ADS on finished"));

				SmoothCameraCurrentTime = 0.f;
				bADS = true;

				ThirdPersonCamera->SetActive(false);
				ADSCamera->SetActive(true);

				if (IsValid(ADSSkeletalMesh)) ADSSkeletalMesh->SetVisibility(true);

			}
		}
	}
	else if(!bDesiredADS && bADS)
	{
		SmoothCameraCurrentTime += DeltaTime;
		if (IsValid(ThirdPersonCamera) && IsValid(ADSCamera))
		{
			// 카메라를 ADS에서 ThirdPerson으로 변경
			FVector NewLocation = UKismetMathLibrary::VInterpTo(ThirdPersonCamera->GetComponentLocation(), ThirdPersonArrow->GetComponentLocation(), DeltaTime, SmoothCameraSpeed);
			ThirdPersonCamera->SetWorldLocation(NewLocation);

			float NewFOV = UKismetMathLibrary::FInterpTo(ThirdPersonCamera->FieldOfView, FieldOfView, DeltaTime, SmoothCameraSpeed);
			ThirdPersonCamera->SetFieldOfView(NewFOV);

			if (UKismetMathLibrary::VSize(ThirdPersonCamera->GetComponentLocation() - ThirdPersonArrow->GetComponentLocation()) < 0.5f ||
				SmoothCameraCurrentTime >= SmoothCameraTimeThreshold)
			{
				// 변경 완료
				UE_LOG(LogTemp, Warning, TEXT("ADS off finished"));

				ThirdPersonCamera->SetFieldOfView(FieldOfView);
				SmoothCameraCurrentTime = 0.f;
				bADS = false;
			}
		}
	}
}

void ABustersCharacter::EquipWeapon(TObjectPtr<AWeapon> WeaponToEquip)
{
	if(IsValid(CombatComponent))
	{
		WeaponToEquip->SetOwner(this);
		CombatComponent->SetWeapon(WeaponToEquip);
		if(WeaponToEquip)
		{
			EWeaponType WeaponType = WeaponToEquip->GetWeaponType();
			switch(WeaponType)
			{
			case EWeaponType::EWT_Pistol:
				SetOverlayMode(AlsOverlayModeTags::PistolTwoHanded);
				break;
			case EWeaponType::EWT_Rifle:
				SetOverlayMode(AlsOverlayModeTags::M4);
				OverlaySkeletalMesh->SetSkeletalMesh(WeaponToEquip->GetSkeletalMesh()->SkeletalMesh);

				ADSSkeletalMesh->SetSkeletalMesh(WeaponToEquip->GetSkeletalMesh()->SkeletalMesh);
				ADSSkeletalMesh->SetRelativeTransform(WeaponToEquip->GetADSTransform());

				break;
			default:
				SetOverlayMode(AlsOverlayModeTags::Default);
				break;
			}
		}
	}
}
