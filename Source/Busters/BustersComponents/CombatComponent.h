// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

class AWeapon;
class ABustersCharacter;
class USkeletalMeshComponent;
enum class EWeaponType : uint8;
enum class EBulletType : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUSTERS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
		
public:
	friend class ABustersCharacter;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Weapon, Category = "Weapon")
	TObjectPtr<AWeapon> Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Starting5mmAmmo = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Starting7mmAmmo = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Starting9mmAmmo = 0;

private:
	UPROPERTY()
	TObjectPtr<ABustersCharacter> Character;

protected:
	FVector HitTarget;

	bool bFireButtonPressed;
	bool bCanFire;

	TMap<EBulletType, int32> AmmoMap;


public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Weapon();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	//
	// Fire
	//
	void FireButtonPressed(bool bPressed);

	void FireWeapon();
	bool CanFireWeapon();

	UFUNCTION(Server, Reliable)
	void ServerFireWeapon(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireWeapon(const FVector_NetQuantize& TraceHitTarget);

	void LocalFireWeapon(const FVector_NetQuantize& TraceHitTarget);

	//
	// Ammo
	// 
	void InitializeAmmo();
	
public:
	FTransform GetWeaponADSTransform();
	inline TObjectPtr<AWeapon> GetWeapon() { return Weapon; };
	inline void SetWeapon(TObjectPtr<AWeapon> WeaponToSet) { Weapon = WeaponToSet; };
};
