// Fill out your copyright notice in the Description page of Project Settings.

#include "LatentInteractableComp.h"
#include "InteractorComp.h"

ULatentInteractableComp::ULatentInteractableComp() 
{
	InteractDelayDuration = 2.f;
	InteractTime = 0.f;
	bTriggerTimer = false;
	bShouldFaceInteractable = false;

	PrimaryComponentTick.bCanEverTick = true;
}

void ULatentInteractableComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner()->Role >= ROLE_Authority
		&& InteractState == EInteractState::IS_Interacting) 
	{
		OnInteractLatent();
	}
}

void ULatentInteractableComp::TryToInterrupt()
{
	if (CurrentInteractor) 
	{
		// Latent Interact will change the state of the Interactable Component later.
		CurrentInteractor->SetIsInteracting(false);
	}
}

void ULatentInteractableComp::OnInteract(UInteractorComp * InteractorComp)
{
	InteractTime = GetWorld()->GetTimeSeconds();

	Super::OnInteract(InteractorComp);
}

void ULatentInteractableComp::OnInteractLatent()
{
	const float CurrentGameTime = GetWorld()->GetTimeSeconds();

	// On Succeeded
	if (CurrentGameTime > InteractTime + InteractDelayDuration) 
	{
		OnSucceeded();
		return;
	}
		
	if (bForceInteract)
		return;

	// Check for failed condition 
	if (CurrentInteractor) 
	{
		//TODO do a check if it's still facing the component's owner
		//TODO check if it's still pressing the key

		//TODO Ignore this if it's trigger system
		if (!CurrentInteractor->GetIsInteracting()) 
		{
			OnFailedInteract(EFailedReason::FR_NotPressingButton);
		}
	}
	else 
	{
		OnFailedInteract(EFailedReason::FR_IsPendingKill);
	}
}
