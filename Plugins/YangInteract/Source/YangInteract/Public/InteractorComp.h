// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractType.h"
#include "InteractorComp.generated.h"

/**
 * ***** Please only Add this guy to A Pawn ******
 * Please Initialize this Interactor Component on constructed
 * Handles server RPCs and receive info from Server
 * The line trace will go through Choosen Channel
 * Setup Key by Project Settings or Input Config and Input the name to KeyToInteract
 */
DECLARE_LOG_CATEGORY_EXTERN(LogInteractorComponent, Log, All);

class UInteractableComp;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFoundInteractableComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentInteractableCompChanges, UInteractableComp*, InteractableComp);

UCLASS(ClassGroup=(Interact), meta=(BlueprintSpawnableComponent))
class YANGINTERACT_API UInteractorComp : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/** Setting the Owning player controller */
	void SetupOwningPlayerController(APlayerController * NewPlayerController);

	/** [Local] Getting player's Pawn Inputcomponent and setting it up. Please Initialize this on Constructing this Interactor Component */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	virtual void InitInteractor(APawn* const _OwningPawn, APlayerController * const _OwningPlayerController);
	
	/** Get key from the Name of the Config Setting you put for your Inputs */
	UPROPERTY(EditDefaultsOnly, Category = "Interactor|Trace")
	FName KeyToInteract;

protected:
	virtual void BeginPlay() override;

	/** Tracing from Owning pawn's Point of view*/
	virtual bool TraceFromPawnPOV(FHitResult & HitData) const;

	/** [Local] Will try to Interact if it traces something*/
	virtual void TryInteract();

	/** [Server] Force Interrupt Interact */
	virtual void InterruptInteract();

	/** On Pressed Binded keys from KeyToInteract*/
	virtual void OnPressedKey();

	/** On Release Binded keys from KeyToInteract*/
	virtual void OnReleasedKey();

	/** [Server] + [Local] Server will interact while Local will just Retrieve Aesthetics and store some info */
	virtual void Interact(AActor* ActorToInteract, bool bForceInteract = false);

	/** [Local] Check if it's Interactable with Component. AI will be disabled*/
	virtual void TraceForInteractableComp();

	UFUNCTION(Server, WithValidation, Reliable)
	void SERVER_Interact(AActor* ActorToInteract, bool bForceInteract = false);

	UFUNCTION(Server, WithValidation, Reliable)
	void SERVER_InterruptInteract();

	/** Is Currently Interacting */
	UPROPERTY(BlueprintReadOnly, BlueprintGetter = GetIsInteracting, Transient, ReplicatedUsing = OnRep_IsInteracting)
	uint8 bIsInteracting : 1;

	/** [Local] To Inform Interactor that's it is currently interacting*/
	UFUNCTION()
	void OnRep_IsInteracting();

	/** [Server] + [Local] This var is set by Interactable comp by passing as ref to set it for Aethetics Purposes*/
	UPROPERTY(ReplicatedUsing = OnRep_FailedReason)
	EFailedReason FailedReason;

	/** [Local] Just incase if Client Approve but Server Denies This Function Will Runs */
	UFUNCTION()
	void OnRep_FailedReason();

	/** Might not even need this. Can just use bIsInteracting*/
	UPROPERTY(Transient)
	uint8 bIsPressingKey : 1;

private:
	/** Pointer of Current Interactable Comp*/
	UPROPERTY(Transient)
	UInteractableComp* CurrentInteractableComp;

	/** Pointer of previous Interactable Comp*/
	UPROPERTY(Transient)
	UInteractableComp* PreviousInteractableComp;

	/** Pointer to Owning Pawn*/
	UPROPERTY(Transient)
	APawn* OwningPawn;

	/** Pointer to Owning player Controller */
	UPROPERTY(Transient)
	APlayerController * OwningPlayerController;

	/** Get Interactable Component by Searching throught all it's Components */
	UFUNCTION()
	UInteractableComp* TryGetInterableCompoent(AActor* ActorToGetFrom) const;

	// Select the Channel The Interactor would react to
	UPROPERTY(EditDefaultsOnly, Category = "Interactor|Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	/** Distance to trace to From Owning Pawn's Loc */
	UPROPERTY(EditDefaultsOnly, Category = "Interactor|Trace")
	float TraceDistance;

	/** [Server] + [Local] Set the Current Interactable Component */
	UFUNCTION()
	void SetCurrentInteractableComp(UInteractableComp * NewInteractableComp);

public:
	/** [Server] + [Local] Get the Player controller of my Owning Pawn. Might be null if attached to AI */
	UFUNCTION()
	APlayerController * GetOwningPlayerController() const { return OwningPlayerController; }

	/** [Server] + [Local] Storing Owning Pawn */
	void SetOwningPawn(APawn* NewOwner);

	/** [Server] + [Local] Get the current Pawn that's owning me */
	UFUNCTION()
	APawn * GetOwningPawn() const { return OwningPawn; }

	/** [Server] + [Local] Get Is Interacting*/
	UFUNCTION(BlueprintGetter)
	bool GetIsInteracting() const { return bIsInteracting; }

	/** [Server] Set is Interacting From Interactable Component */
	void SetIsInteracting(bool NewState);

	/** Collision Channel for tracing*/
	TEnumAsByte<ECollisionChannel> GetCollisionChannel() { return CollisionChannel; }

	/** [Server] Will fire when the Interaction Is Completed. Signal Is Send by Interactable Comp*/
	void OnInteractCompleted(UInteractableComp* _InteractableComp, const bool bSucceeded);

	/** [Local] Will fire When a Interactable Component with all Condition Met is found */
	UPROPERTY(BlueprintAssignable)
	FOnFoundInteractableComp OnFoundInteractableComp;

	/** [Server] + [Local] Will fires when the Previous Interactable Comp is Not the same as Current*/
	UPROPERTY(BlueprintAssignable)
	FOnCurrentInteractableCompChanges OnCurrentInteractableCompChanges;

};
