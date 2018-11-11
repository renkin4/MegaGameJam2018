// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableComp.h"
#include "UnrealNetwork.h"
#include "InteractorComp.h"

DEFINE_LOG_CATEGORY(LogInteractableComponent);

// Sets default values for this component's properties
UInteractableComp::UInteractableComp(const class FObjectInitializer & ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsReplicated(true);
	InteractState = EInteractState::IS_Idle;
	CurrentInteractor = nullptr;

	InteractacbleCompDescription = FText::FromString(TEXT("Please enter Description"));
	InstructionTxt = FText::FromString(TEXT("Please Enter Instruction"));

	bForceInteract = false;
	bOneTimeInteraction = false;
	bAllowInteraction = true;

	AcceptableForwardDegree = 45.f;

	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

void UInteractableComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION(UInteractableComp, InteractState, COND_SkipOwner);

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UInteractableComp::TryToInterrupt()
{
	ChangeInteractState(EInteractState::IS_Idle);
}

void UInteractableComp::OnSucceeded()
{
	if (CurrentInteractor)// Interactor might be missing due to Character dying
	{
		CurrentInteractor->OnInteractCompleted(this, true);
		UE_LOG(LogInteractableComponent, Log, TEXT("(%s) has completed Interacting with (%s)"), *GetNameSafe(CurrentInteractor->GetOwner()), *GetNameSafe(GetOwner()));
	}

	bAllowInteraction = !bOneTimeInteraction;
	InteractCompleted.Broadcast(true, EFailedReason::FR_None);

	ChangeInteractState(EInteractState::IS_Completed);
}

void UInteractableComp::OnInteract(UInteractorComp * InteractorComp)
{
	CurrentInteractor = InteractorComp;
	CurrentInteractor->SetIsInteracting(true);

	ChangeInteractState(EInteractState::IS_Interacting);
}

void UInteractableComp::OnInterrupted()
{
	//TODO Log Interrupted Reason
	InteractionInterrupted.Broadcast();

	if (CurrentInteractor 
		&& InteractState != EInteractState::IS_Interacting)// Interactor might be missing due to Character dying
	{
		CurrentInteractor->SetIsInteracting(false);
		CurrentInteractor = nullptr;

		//TODO reset To Idle On A Timer base
	}	
}

void UInteractableComp::OnFailedInteract(const EFailedReason& _FailedReason)
{
	if (CurrentInteractor)// Interactor might be missing due to Character dying
	{
		CurrentInteractor->OnInteractCompleted(this, false);
		UE_LOG(LogInteractableComponent, Log, TEXT("(%s) has completed Interacting with (%s)"), *GetNameSafe(CurrentInteractor->GetOwner()), *GetNameSafe(GetOwner()));
	}

	InteractCompleted.Broadcast(false, _FailedReason);

	ChangeInteractState(EInteractState::IS_Failed);
}

void UInteractableComp::ChangeInteractState(EInteractState NewState)
{
	if (InteractState == NewState)
	{
		return;
	}

	InteractState = NewState;
	OnInterrupted();

	/** Aesthetics on Interact State. Make sure it's Only on StandAlone or Listen Server */
	if (IsNetMode(NM_ListenServer) || IsNetMode(NM_Standalone))
	{
		OnRep_InteractState();
	}
}

bool UInteractableComp::CheckAvailability(EFailedReason & OutReason, UInteractorComp * CheckInteractor) const
{
	OutReason = EFailedReason::FR_None;

	if (!IsValid(this))
	{
		OutReason = EFailedReason::FR_IsPendingKill;
	}
	else if (!bAllowInteraction)
	{
		OutReason = EFailedReason::FR_NotAllowingInteraction;
	}
	else if (InteractState == EInteractState::IS_Interacting)
	{
		OutReason = EFailedReason::FR_OthersInteracting;
	}
	else if (bShouldBeInfront 
		&& !InteractorIsAtAcceptableFrontDegree(CheckInteractor))
	{
		OutReason = EFailedReason::FR_NotInfrontInteractable;
	}

	// Only is available if there's no Failed Reason
	return OutReason == EFailedReason::FR_None;
}

void UInteractableComp::OnRep_InteractState()
{
	OnRepInteractState.Broadcast();
}

bool UInteractableComp::InteractorIsAtAcceptableFrontDegree(UInteractorComp * const CheckInteractor) const
{
	// Safety Check Regardless
	if (!IsValid(CheckInteractor)
		|| !IsValid(CheckInteractor->GetOwner()))
		return false;

	const FRotator & OwnerRot = GetOwner()->GetActorRotation();
	FVector InteractorAndOwnerMagnitude = CheckInteractor->GetOwner()->GetActorLocation() - GetOwner()->GetActorLocation();
	const FRotator & LookAtInteractorRot = FRotationMatrix::MakeFromX(InteractorAndOwnerMagnitude).Rotator();

	FRotator DeltaRot = OwnerRot - LookAtInteractorRot;
	DeltaRot.Normalize();

	return FMath::Abs(DeltaRot.Yaw) <= AcceptableForwardDegree;
}
