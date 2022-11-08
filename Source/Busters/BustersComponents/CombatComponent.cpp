	// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Busters/Character/BustersCharacter.h"
#include "Busters/Weapon/BulletTypes.h"
#include "Busters/Weapon/Weapon.h"
#include "Busters/Weapon/WeaponTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	
	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if(IsValid(Character))
	{
		if(Character->HasAuthority())
		{
			InitializeAmmo();
		}
	}
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(Character) && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
		
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, Weapon);
}

void UCombatComponent::OnRep_Weapon()
{
	if(IsValid(Character))
	{
		Character->UpdateWeaponMesh();
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (IsValid(GEngine) && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		TObjectPtr<UWorld> World = GetWorld();
		if(IsValid(World))
		{
			World->LineTraceSingleByChannel(
				TraceHitResult,
				Start,
				End,
				ECollisionChannel::ECC_Visibility
			);
		}
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if(bFireButtonPressed)
	{
		FireWeapon();
	}
}

void UCombatComponent::FireWeapon()
{
	if(CanFireWeapon())
	{
		// 총을 쏜 유저에게는 바로 발사 애니메이션을 재생한다.
		if (IsValid(Character))
		{
			if (Character->GetADS())
			{
				Character->GetADSSkeletalMesh()->PlayAnimation(Weapon->FireAnimation, false);
				Character->OnFireWeapon();
			}
			else
			{
				Character->GetOverlaySkeletalMesh()->PlayAnimation(Weapon->FireAnimation, false);
			}
			// LocalFireWeapon(HitTarget);
		}

		// 이후 Server, Multicast RPC를 호출해 모든 유저에게 발사 애니메이션을 재생한다.
		ServerFireWeapon(HitTarget);
	}
}

bool UCombatComponent::CanFireWeapon()
{
	if (!IsValid(Weapon)) return false;
	return true;
}

void UCombatComponent::ServerFireWeapon_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFireWeapon(TraceHitTarget);
}

void UCombatComponent::MulticastFireWeapon_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	// 소리 재생(클라, 서버)
	if (IsValid(Character) && IsValid(Weapon) && IsValid(Weapon->FireSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, Weapon->FireSound, Character->GetActorLocation());
		// 오버레이 메쉬 발사 애니메이션 재생(다른 유저)
		if(!Character->IsLocallyControlled())
		{
			Character->GetOverlaySkeletalMesh()->PlayAnimation(Weapon->FireAnimation, false);
		}
	}
	// 
	if(IsValid(Character) && IsValid(Weapon) && Character->HasAuthority())
	{
		Weapon->Fire(TraceHitTarget, Character->GetADS());
	}
	
	//LocalFireWeapon(TraceHitTarget);
}

void UCombatComponent::LocalFireWeapon(const FVector_NetQuantize& TraceHitTarget)
{
	if (IsValid(Character) && IsValid(Weapon))
	{
		UKismetSystemLibrary::PrintText(this, FText::FromString("Local Fire"));
		Character->GetOverlaySkeletalMesh()->PlayAnimation(Weapon->FireAnimation, false);
	}
}

void UCombatComponent::InitializeAmmo()
{
	AmmoMap.Emplace(EBulletType::EBT_5mm, Starting5mmAmmo);
	AmmoMap.Emplace(EBulletType::EBT_7mm, Starting7mmAmmo);
	AmmoMap.Emplace(EBulletType::EBT_9mm, Starting9mmAmmo);
}

FTransform UCombatComponent::GetWeaponADSTransform()
{
	if (Weapon) return Weapon->GetADSTransform();
	else return {};
}

