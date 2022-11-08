// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Busters/Character/BustersCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetSystemLibrary.h"

void AProjectileWeapon::Fire(const FVector& HitTarget, const bool bADS)
{
	Super::Fire(HitTarget, bADS);

	TObjectPtr<ABustersCharacter> OwnerCharacter = Cast<ABustersCharacter>(GetOwner());
	if(IsValid(OwnerCharacter))
	{
		const auto WeaponMesh = OwnerCharacter->GetOverlaySkeletalMesh();
		if(IsValid(WeaponMesh))
		{
			const auto MuzzleSocket = WeaponMesh->GetSocketByName(FName("MuzzleSocket"));
			if(IsValid(MuzzleSocket))
			{
				FTransform SocketTransform = MuzzleSocket->GetSocketTransform(WeaponMesh);
				FVector ToTarget = HitTarget - SocketTransform.GetLocation();
				FRotator TargetRotation = ToTarget.Rotation();
				
				FVector Start = SocketTransform.GetLocation();

				if (bADS)
				{
					Start = OwnerCharacter->GetADSSkeletalMesh()->GetComponentLocation();
				}
				UKismetSystemLibrary::PrintText(this, FText::FromString("Fire Projectile"));
				UKismetSystemLibrary::DrawDebugLine(this, Start, HitTarget, FLinearColor::Red, 1.f, 1.f);

				TObjectPtr<UWorld> World = GetWorld();
				if(IsValid(World))
				{
					FHitResult HitResult;
					World->LineTraceSingleByChannel(HitResult, Start, ToTarget, ECollisionChannel::ECC_Visibility);

					TObjectPtr<ACharacter> HitActor = Cast<ACharacter>(HitResult.GetActor());
					if(IsValid(HitActor))
					{
						HitActor->GetMesh()->SetVisibility(false);
						UKismetSystemLibrary::PrintText(HitActor, FText::FromString("Hit Projectile"));
					}
				}
			}
		}
		
	}
	
}
