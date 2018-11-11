// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractorComp.h"
#include "InteractableComp.h"
#include "UnrealNetwork.h"
#include "GameFramework/Pawn.h"

DEFINE_LOG_CATEGORY(LogInteractorComponent);

// Sets default values for this component's properties
UInteractorComp::UInteractorComp(const class FObjectInitializer & ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsReplicated(true);
	bIsInteracting = false;
	CurrentInteractableComp = nullptr;
	PreviousInteractableComp = nullptr;

	// 2 meter In front Of Pawn
	TraceDistance = 200.f;
	bIsPressingKey = false;

	FailedReason = EFailedReason::FR_None;

	CollisionChannel = ECollisionChannel::ECC_Visibility;

	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

void UInteractorComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION(UInteractorComp, bIsInteracting, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UInteractorComp, FailedReason, COND_OwnerOnly);

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called when the game starts
void UInteractorComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UInteractorComp::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInteractorComp::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction(KeyToInteract, IE_Pressed, this, &UInteractorComp::OnPressedKey);
	PlayerInputComponent->BindAction(KeyToInteract, IE_Released, this, &UInteractorComp::OnReleasedKey);
}

void UInteractorComp::InitInteractor(APawn* const _OwningPawn, APlayerController * const _OwningPlayerController)
{
	OwningPawn = _OwningPawn;
	OwningPlayerController = _OwningPlayerController;
}

bool UInteractorComp::TraceFromPawnPOV(FHitResult & HitData) const
{
	if (!OwningPawn)
	{
		UE_LOG(LogInteractorComponent, Error, TEXT("Please make sure you Has A Pawn when trying to Interator (%s)"), *GetNameSafe(GetClass()));
		return false;
	}

	FVector OutLocation;
	FRotator OutRot;
	OwningPawn->GetActorEyesViewPoint(OutLocation, OutRot);

	const FVector TraceStartLoc = OutLocation;
	const FVector TraceEndLoc = (OutRot.Vector() * TraceDistance) + OutLocation;

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(GetOwner());
	TraceParams.bTraceComplex = true;

	const bool bLineTraceHits = GetWorld()->LineTraceSingleByChannel(HitData, TraceStartLoc, TraceEndLoc, CollisionChannel, TraceParams);

	return bLineTraceHits;
}

void UInteractorComp::TryInteract()
{
	FHitResult HitData(ForceInit);
	TraceFromPawnPOV(HitData);

	if (HitData.bBlockingHit)
	{
		Interact(HitData.GetActor());
		UE_LOG(LogInteractorComponent, Log, TEXT("%s Detected something When Trying to interact. Interacting With (%s)"), *GetNameSafe(GetOwner()), *GetNameSafe(HitData.GetActor()));
	}
}

void UInteractorComp::InterruptInteract()
{
	if (!bIsInteracting || !CurrentInteractableComp)
		return;

	if (IsNetMode(NM_Client))
	{
		SERVER_InterruptInteract();
		return;
	}

	CurrentInteractableComp->TryToInterrupt();
}

void UInteractorComp::OnPressedKey()
{
	TryInteract();
}

void UInteractorComp::OnReleasedKey()
{
	InterruptInteract();
}

void UInteractorComp::Interact(AActor * ActorToInteract, bool bForceInteract)
{
	ensureMsgf(GetOwner() && ActorToInteract, TEXT("Please make sure Owner and Actor To Interact when using this Function"));

	SetCurrentInteractableComp(TryGetInterableCompoent(ActorToInteract));

	if (!CurrentInteractableComp) 
	{
		UE_LOG(LogInteractorComponent, Warning, TEXT("(%s) Tried to Interact But there's nothing for you to Interact At the distance of (%f)cm"), *GetNameSafe(GetOwner()), TraceDistance);
		return;
	}
	
	const bool bAvailable = CurrentInteractableComp->CheckAvailability(FailedReason, this);

	/** Force Early Exit For Both Interactor and Interactable Comp*/
	if (!bAvailable && IsValid(CurrentInteractableComp))
	{
		CurrentInteractableComp->OnFailedInteract(FailedReason);
		return;
	}
	
	if (IsNetMode(NM_Client))
	{
		SERVER_Interact(ActorToInteract, bForceInteract);
		return;
	}

	// Server side 
	CurrentInteractableComp->SetForceInteract(bForceInteract);
	CurrentInteractableComp->OnInteract(this);
}

void UInteractorComp::TraceForInteractableComp()
{
	if (GetNetMode() == NM_DedicatedServer || !OwningPlayerController)
		return;

	FHitResult HitData(ForceInit);
	TraceFromPawnPOV(HitData);

	SetCurrentInteractableComp(TryGetInterableCompoent(HitData.GetActor()));

	if (CurrentInteractableComp)
	{
		const bool bAvailable = CurrentInteractableComp->CheckAvailability(FailedReason, this);

		if (bAvailable && PreviousInteractableComp != CurrentInteractableComp)
		{
			PreviousInteractableComp = CurrentInteractableComp;
			OnFoundInteractableComp.Broadcast();
		}
	}
	// Reset Can Interact
	else
	{
		PreviousInteractableComp = nullptr;
	}
}

void UInteractorComp::SERVER_InterruptInteract_Implementation()
{
	InterruptInteract();
}

bool UInteractorComp::SERVER_InterruptInteract_Validate()
{
	return true;
}

void UInteractorComp::SERVER_Interact_Implementation(AActor* ActorToInteract, bool bForceInteract /*= false*/)
{
	Interact(ActorToInteract, bForceInteract);
}

bool UInteractorComp::SERVER_Interact_Validate(AActor* ActorToInteract, bool bForceInteract /*= false*/)
{
	return true;
}

void UInteractorComp::OnRep_IsInteracting()
{

}

void UInteractorComp::OnRep_FailedReason()
{
	//TOOD Log Failed Reason
	// TODO Send Delegate for Aethetics
}

void UInteractorComp::SetCurrentInteractableComp(UInteractableComp * NewInteractableComp)
{
	if (CurrentInteractableComp == NewInteractableComp)
		return;

	CurrentInteractableComp = NewInteractableComp;
	OnCurrentInteractableCompChanges.Broadcast(CurrentInteractableComp);
}

UInteractableComp* UInteractorComp::TryGetInterableCompoent(AActor* ActorToGetFrom) const
{
	if (!ActorToGetFrom || ActorToGetFrom->IsPendingKill())
		return nullptr;

	UInteractableComp * InteractableComponent = Cast<UInteractableComp>(ActorToGetFrom->FindComponentByClass(UInteractableComp::StaticClass()));

	return InteractableComponent;
}

void UInteractorComp::SetOwningPawn(APawn* NewOwner)
{
	if (!NewOwner)
	{
		UE_LOG(LogInteractorComponent, Error, TEXT("Please make sure New owner (%s) is A Pawn for %s."), *GetNameSafe(OwningPawn), *GetClass()->GetName());
		return;
	}

	OwningPawn = NewOwner;
}

void UInteractorComp::SetIsInteracting(bool NewState)
{
	bIsInteracting = NewState;

	// TODO if it's interacting Set some interacting Pop some Interacting UI Up
	// TODO setup some Broadcast Delegate here for Interactable Component to Tie to.

	// Run On Listen Server and standalone
	if (IsNetMode(NM_ListenServer) || IsNetMode(NM_Standalone))
	{
		OnRep_IsInteracting();
	}
}

void UInteractorComp::OnInteractCompleted(UInteractableComp* _InteractableComp, const bool bSucceeded)
{

}

void UInteractorComp::SetupOwningPlayerController(APlayerController * NewPlayerController)
{

}

