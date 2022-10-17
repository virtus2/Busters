// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Busters/Character/BustersCharacter.h"
#include "Busters/Weapon/Weapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

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
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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

FTransform UCombatComponent::GetWeaponADSTransform()
{
	if (Weapon) return Weapon->GetADSTransform();
	else return {};
}

