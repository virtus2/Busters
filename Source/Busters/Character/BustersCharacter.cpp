// Fill out your copyright notice in the Description page of Project Settings.

#include "BustersCharacter.h"

#include "Busters/BustersComponents/CombatComponent.h"
#include "AlsCameraComponent.h"
#include "AlsCharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Busters/Weapon/Weapon.h"
#include "Busters/Weapon/WeaponTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

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

	BaseMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	BaseMaxWalkSpeedCrouched = GetCharacterMovement()->MaxWalkSpeedCrouched;
	
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
	WeaponSway(DeltaSeconds);
	MovingSway(DeltaSeconds);
}

void ABustersCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	auto* EnhancedInput{ Cast<UEnhancedInputComponent>(Input) };
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::InputLookMouse);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ThisClass::InputSprint);
		EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::InputCrouch);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::InputJump);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::InputAim);
		EnhancedInput->BindAction(ADSAction, ETriggerEvent::Triggered, this, &ThisClass::InputADS);
		EnhancedInput->BindAction(FireAction, ETriggerEvent::Triggered, this, &ThisClass::InputFire);
	}
}

void ABustersCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

void ABustersCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(IsValid(CombatComponent))
	{
		CombatComponent->Character = this;
	}
}

void ABustersCharacter::InputLookMouse(const FInputActionValue& ActionValue)
{
	const auto Value{ ActionValue.Get<FVector2D>() };

	AddControllerPitchInput(Value.Y * LookUpMouseSensitivity);
	AddControllerYawInput(Value.X * LookRightMouseSensitivity);

	SwayPitch = Value.Y; // Up
	SwayYaw = Value.X; // Left
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

	SwayForward = ForwardDirection * Value.Y;
	SwayRight = RightDirection * Value.X;
}

void ABustersCharacter::InputWalk(const FInputActionValue& ActionValue)
{
	if (GetDesiredGait() == AlsGaitTags::Walking)
	{
		SetDesiredGait(AlsGaitTags::Running);
	}
	else if (GetDesiredGait() == AlsGaitTags::Running)
	{
		SetDesiredGait(AlsGaitTags::Walking);
	}
}

void ABustersCharacter::InputSprint(const FInputActionValue& ActionValue)
{
	if(ActionValue.Get<bool>())
	{
		if(bADS)
		{
			ToADSCamera(false);
		}
		SetDesiredGait(AlsGaitTags::Sprinting);
	}
	else
	{
		if(bADS)
		{
			SetDesiredGait(AlsGaitTags::Walking);
		}
		else
		{
			SetDesiredGait(AlsGaitTags::Running);
		}
	}
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
		if (bADS)
		{
			ToADSCamera(false);
		}
		Jump();
	}
	else
	{
		StopJumping();
	}
}

void ABustersCharacter::InputAim(const FInputActionValue& ActionValue)
{
	UE_LOG(LogTemp, Warning, TEXT("InputAim Called %d"), ActionValue.Get<bool>());
	if(ActionValue.Get<bool>())
	{
		SetDesiredAiming(true);
		SetDesiredGait(AlsGaitTags::Walking);
	}
	else
	{
		SetDesiredAiming(false);
		SetDesiredGait(AlsGaitTags::Running);
	}
}

void ABustersCharacter::InputADS(const FInputActionValue& ActionValue)
{
	UE_LOG(LogTemp, Warning, TEXT("InputADS called"));
	/*
	if (GetDesiredGait() == AlsGaitTags::Running || GetDesiredGait() == AlsGaitTags::Sprinting)
	{
		SetDesiredGait(AlsGaitTags::Walking);
	}
	else if (GetDesiredGait() == AlsGaitTags::Walking)
	{
		SetDesiredGait(AlsGaitTags::Running);
	}
	*/
	if (bADS)
	{
		SetDesiredGait(AlsGaitTags::Running);
		ToADSCamera(false);
	}
	else 
	{
		SetDesiredGait(AlsGaitTags::Walking);
		ToADSCamera(true);
	}
}

void ABustersCharacter::InputFire(const FInputActionValue& ActionValue)
{
	if(IsValid(CombatComponent))
	{
		CombatComponent->FireButtonPressed(ActionValue.Get<bool>());
	}
}

void ABustersCharacter::InputSwitchShoulder()
{

}

void ABustersCharacter::OnFireWeapon()
{
	TObjectPtr<APlayerCameraManager> PlayerCameraManager = Cast<APlayerController>(GetController())->PlayerCameraManager;
	if(IsValid(PlayerCameraManager) && IsValid(CameraShakeOnFireWeapon))
	{
		PlayerCameraManager->StartCameraShake(CameraShakeOnFireWeapon);
	}
}


void ABustersCharacter::ToADSCamera(bool bToADS)
{
	SmoothCameraCurrentTime = 0.f;
	if (bToADS)
	{
		// 카메라를 ThirdPerson에서 ADS로 변경
		UE_LOG(LogTemp, Warning, TEXT("ADS on"));

		bDesiredADS = true;
		SetDesiredAiming(true);
		//SetViewMode(AlsViewModeTags::FirstPerson);

		if (IsValid(OverlaySkeletalMesh))
		{
			OverlaySkeletalMesh->SetVisibility(false);
		}
		if (GetMesh())
		{
			GetMesh()->SetVisibility(false);
		}
		if (IsValid(ADSSkeletalMesh))
		{
			ADSSkeletalMesh->SetVisibility(true);
		}

	}
	else
	{
		// 카메라를 ADS에서 ThirdPerson으로 변경
		UE_LOG(LogTemp, Warning, TEXT("ADS off"));

		bDesiredADS = false;
		SetDesiredAiming(false);
		ADSCamera->SetActive(false);
		ThirdPersonCamera->SetActive(true);
		//SetViewMode(AlsViewModeTags::ThirdPerson);

		if (IsValid(OverlaySkeletalMesh))
		{
			OverlaySkeletalMesh->SetVisibility(true);
		}
		if (GetMesh())
		{
			GetMesh()->SetVisibility(true);
		}
		if (IsValid(ADSSkeletalMesh))
		{
			ADSSkeletalMesh->SetVisibility(false);
		}
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

				if (IsValid(ADSSkeletalMesh))
				{
					ADSSkeletalMesh->SetVisibility(true);
				}
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

void ABustersCharacter::WeaponSway(float DeltaTime)
{
	bool check = IsValid(CombatComponent) &&
		IsValid(CombatComponent->GetWeapon()) &&
		CombatComponent->GetWeapon()->GetWeaponType() != EWeaponType::EWT_Default &&
		bADS;

	if (check)
	{
		float MaxSwayDegree = CombatComponent->GetWeapon()->GetMaxSwayDegree();
		float SwaySpeed = CombatComponent->GetWeapon()->GetSwaySpeed();

		float Turn = SwayYaw * MaxSwayDegree;
		float LookUp = SwayPitch * MaxSwayDegree;

		SwayFinalRotation.Roll = LookUp;
		SwayFinalRotation.Pitch = Turn;
		SwayFinalRotation.Yaw = Turn;
		
		FRotator TargetRotation(
			SwayInitialRotation.Pitch + SwayFinalRotation.Pitch,
			SwayInitialRotation.Yaw + SwayFinalRotation.Yaw,
			SwayInitialRotation.Roll + SwayFinalRotation.Roll
		);

		if (IsValid(ADSSkeletalMesh))
		{
			FRotator ResultRotation = UKismetMathLibrary::RInterpTo(ADSSkeletalMesh->GetRelativeRotation(), TargetRotation, DeltaTime, SwaySpeed);
			ResultRotation.Roll = UKismetMathLibrary::FClamp(ResultRotation.Roll, -MaxSwayDegree, MaxSwayDegree);
			ResultRotation.Pitch = UKismetMathLibrary::FClamp(ResultRotation.Pitch, -MaxSwayDegree, MaxSwayDegree);
			ResultRotation.Yaw = UKismetMathLibrary::FClamp(ResultRotation.Yaw, -MaxSwayDegree + SwayInitialRotation.Yaw, MaxSwayDegree + SwayInitialRotation.Yaw);

			ADSSkeletalMesh->SetRelativeRotation(ResultRotation);
		}
	}
}

void ABustersCharacter::MovingSway(float DeltaTime)
{
	if (!bADS)
	{
		return;
	}

	double Speed = GetVelocity().Length();
	SwayMovingTime += DeltaTime;
	if (Speed <= 0.1f)
	{
		SwayMovingTime = 0.f;
		return;
	}
	if(IsValid(SwayMovingVectorCurve))
	{
		FVector NewVector = SwayMovingVectorCurve->GetVectorValue(SwayMovingTime);
		float Roll = UKismetMathLibrary::NormalizeToRange(NewVector.X * Speed, -SwayMovingRange, SwayMovingRange);
		float Pitch = UKismetMathLibrary::NormalizeToRange(NewVector.Y * Speed, -SwayMovingRange, SwayMovingRange);
		float Yaw = UKismetMathLibrary::NormalizeToRange(NewVector.Z * Speed, -SwayMovingRange, SwayMovingRange);

		SwayFinalRotation.Roll = SwayInitialRotation.Roll + Roll;
		SwayFinalRotation.Pitch = SwayInitialRotation.Pitch + Pitch;
		SwayFinalRotation.Yaw = SwayInitialRotation.Yaw + Yaw;

		if(IsValid(ADSSkeletalMesh))
		{
			FRotator ResultRotation = UKismetMathLibrary::RInterpTo(ADSSkeletalMesh->GetRelativeRotation(), SwayFinalRotation, DeltaTime, SwayMovingSpeed);
			ADSSkeletalMesh->SetRelativeRotation(ResultRotation);
		}
	}
}

void ABustersCharacter::BuyWeapon(TSubclassOf<AWeapon> BoughtWeapon)
{
	if(HasAuthority())
	{
		// 서버에서 무기를 구매했을경우
		// 서버에서 무기를 생성해 구매한 캐릭터의 CombatComponent에 할당해준다.
		// 이는 Replicate되어 클라이언트에게도 갱신된다.
		TObjectPtr<UWorld> World = GetWorld();
		if (IsValid(World))
		{
			TObjectPtr<AWeapon> NewWeapon = World->SpawnActor<AWeapon>(BoughtWeapon);
			if (IsValid(NewWeapon) && IsValid(CombatComponent))
			{
				NewWeapon->SetOwner(this);
				CombatComponent->SetWeapon(NewWeapon);
				UpdateWeaponMesh();
			}
		}
	}
	else
	{
		// 클라이언트에서 무기를 구매했을경우
		// 서버 RPC인 ServerBuyWeapon을 호출한다.
		ServerBuyWeapon(BoughtWeapon);
	}
}

void ABustersCharacter::ServerBuyWeapon_Implementation(TSubclassOf<AWeapon> BoughtWeapon)
{
	// 서버에서 무기를 생성해 구매한 캐릭터의 CombatComponent에 할당해준다.
	// 이는 Replicate되어 클라이언트에게도 갱신된다.
	TObjectPtr<UWorld> World = GetWorld();
	if (IsValid(World))
	{
		TObjectPtr<AWeapon> NewWeapon = World->SpawnActor<AWeapon>(BoughtWeapon);
		if (IsValid(NewWeapon) && IsValid(CombatComponent))
		{
			NewWeapon->SetOwner(this);
			CombatComponent->SetWeapon(NewWeapon);
			UpdateWeaponMesh();
		}
	}
}

void ABustersCharacter::UpdateWeaponMesh()
{
	if (IsValid(CombatComponent) && IsValid(CombatComponent->GetWeapon()))
	{
		if (IsValid(OverlaySkeletalMesh) && IsValid(ADSSkeletalMesh))
		{
			EWeaponType WeaponType = CombatComponent->GetWeapon()->GetWeaponType();
			switch (WeaponType)
			{
			case EWeaponType::EWT_Pistol:
				SetOverlayMode(AlsOverlayModeTags::PistolTwoHanded);
				break;
			case EWeaponType::EWT_Rifle:
				SetOverlayMode(AlsOverlayModeTags::M4);
				OverlaySkeletalMesh->SetSkeletalMesh(CombatComponent->GetWeapon()->GetSkeletalMesh()->SkeletalMesh);
				ADSSkeletalMesh->SetSkeletalMesh(CombatComponent->GetWeapon()->GetSkeletalMesh()->SkeletalMesh);
				ADSSkeletalMesh->SetRelativeTransform(CombatComponent->GetWeapon()->GetADSTransform());
				SwayInitialRotation = ADSSkeletalMesh->GetRelativeRotation();

				break;
			default:
				SetOverlayMode(AlsOverlayModeTags::Default);
				break;

			}
		}
	}
}

void ABustersCharacter::EquipWeapon(TSubclassOf<AWeapon> WeaponToEquip)
{
	if(IsValid(CombatComponent))
	{
		// 서버에서만 실행된다.
		if(HasAuthority())
		{
			TObjectPtr<UWorld> World = GetWorld();
			if (IsValid(World))
			{
				UKismetSystemLibrary::PrintString(World, "Server Equip");
				TObjectPtr<AWeapon> NewWeapon = World->SpawnActor<AWeapon>(WeaponToEquip);
				if (IsValid(NewWeapon) && IsValid(CombatComponent) && IsValid(OverlaySkeletalMesh) && IsValid(ADSSkeletalMesh))
				{
					CombatComponent->SetWeapon(NewWeapon);
					NewWeapon->SetOwner(this);
					EWeaponType WeaponType = NewWeapon->GetWeaponType();
					switch (WeaponType)
					{
					case EWeaponType::EWT_Pistol:
						SetOverlayMode(AlsOverlayModeTags::PistolTwoHanded);
						break;
					case EWeaponType::EWT_Rifle:
						SetOverlayMode(AlsOverlayModeTags::M4);
						OverlaySkeletalMesh->SetSkeletalMesh(NewWeapon->GetSkeletalMesh()->SkeletalMesh);

						ADSSkeletalMesh->SetSkeletalMesh(NewWeapon->GetSkeletalMesh()->SkeletalMesh);
						ADSSkeletalMesh->SetRelativeTransform(NewWeapon->GetADSTransform());
						SwayInitialRotation = ADSSkeletalMesh->GetRelativeRotation();

						break;
					default:
						SetOverlayMode(AlsOverlayModeTags::Default);
						break;
					}
				}
			}
		}
		else
		{
			ServerEquipWeapon(WeaponToEquip);
		}
	}
}

void ABustersCharacter::ServerEquipWeapon_Implementation(TSubclassOf<AWeapon> WeaponToEquip)
{
	TObjectPtr<UWorld> World = GetWorld();
	if (IsValid(World))
	{
		UKismetSystemLibrary::PrintString(World, "Client Equip");
		TObjectPtr<AWeapon> NewWeapon = World->SpawnActor<AWeapon>(WeaponToEquip);
		if (IsValid(NewWeapon) && IsValid(CombatComponent) && IsValid(OverlaySkeletalMesh) && IsValid(ADSSkeletalMesh))
		{
			CombatComponent->SetWeapon(NewWeapon);
			NewWeapon->SetOwner(this);
			EWeaponType WeaponType = NewWeapon->GetWeaponType();
			switch (WeaponType)
			{
			case EWeaponType::EWT_Pistol:
				SetOverlayMode(AlsOverlayModeTags::PistolTwoHanded);
				break;
			case EWeaponType::EWT_Rifle:
				SetOverlayMode(AlsOverlayModeTags::M4);
				OverlaySkeletalMesh->SetSkeletalMesh(NewWeapon->GetSkeletalMesh()->SkeletalMesh);

				ADSSkeletalMesh->SetSkeletalMesh(NewWeapon->GetSkeletalMesh()->SkeletalMesh);
				ADSSkeletalMesh->SetRelativeTransform(NewWeapon->GetADSTransform());
				SwayInitialRotation = ADSSkeletalMesh->GetRelativeRotation();

				break;
			default:
				SetOverlayMode(AlsOverlayModeTags::Default);
				break;
			}
		}
	}

}
