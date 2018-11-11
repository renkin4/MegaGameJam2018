// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractType.h"
#include "InteractableComp.generated.h"

/**
 *
 */

DECLARE_LOG_CATEGORY_EXTERN(LogInteractableComponent, Log, All);

class UInteractorComp;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInteractionInterrupted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractCompleted, bool, bSucceeded, EFailedReason, FailedReason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRepInteractState);

UCLASS( ClassGroup=(Interact), meta=(BlueprintSpawnableComponent) )
class YANGINTERACT_API UInteractableComp : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	friend class UInteractorComp;

public:
	/** Try Interrupt Current Process */
	virtual void TryToInterrupt();

	/** This Bool is not working yet Still in planning stage */
	UPROPERTY(EditAnywhere, Category = "Interact")
	uint8 bShouldBeInfront : 1;

	/** A Degree that will be calculated for Character's Current position around the Interactable Comp's Owner. Taking the forward vector as the Middle point */
	UPROPERTY(EditAnywhere, Category = "Interact", meta = (EditCondition = "bShouldBeInfront"))
	float AcceptableForwardDegree;

	/** Only can be interact once */
	UPROPERTY(EditAnywhere, Category = "Interact")
	uint8 bOneTimeInteraction : 1;

	/** Allow interactable Comp to Interact */
	UPROPERTY(Replicated, EditAnywhere, Category = "Interact")
	uint8 bAllowInteraction : 1;
	
	/** [Local] For Widget Purposes */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Interactable")
	FText InteractacbleCompDescription;

	/** [Local] For Widget Purposes */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Interactable")
	FText InstructionTxt;

protected:
	/** [Server] On Completed */
	virtual void OnSucceeded();

	/** [Server] On interact */
	virtual void OnInteract(UInteractorComp* InteractorComp);

	/** [Server] OnInterrupted */
	virtual void OnInterrupted();

	/** [Server] On Interaction Failed */
	virtual void OnFailedInteract(const EFailedReason& _FailedReason);

	/** [Server] Change Interact State */
	virtual void ChangeInteractState(EInteractState NewState);

	/** [Server] + [Local] Check if Interactable comp Allow Interaction 
	 * @ Failed reason Should be passed in from Interactor as a Ref and Be altered from here
	 */
	virtual bool CheckAvailability(EFailedReason& OutReason, UInteractorComp * CheckInteractor) const;

	/** [Local] Client Interact State*/
	UFUNCTION()
	void OnRep_InteractState();

	/** [Server] + [Local] Current Interact State */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_InteractState, BlueprintReadOnly, Category = "Interactable")
	EInteractState InteractState;

	/** [Server] A Reference towards the Current Interactor that's Interacting with this */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Interactable")
	UInteractorComp* CurrentInteractor;

	/** Will by pass all Check Condition And Pass true to it */
	UPROPERTY(Transient)
	uint8 bForceInteract : 1;

private:
	/** Check if Interactor Is In front of Owner */
	bool InteractorIsAtAcceptableFrontDegree(UInteractorComp * const CheckInteractor) const;

public:
	/** Get a Pointer of Current Interactor */
	FORCEINLINE UInteractorComp * GetCurrentInteractor() const 
	{
		return CurrentInteractor; 
	}

	/** Set should force interaction. Best be used on Server */
	UFUNCTION()
	void SetForceInteract(bool bShouldForce = true) { bForceInteract = bShouldForce; }

	/** [Server] On Event Completed Delegate To be bind by Owner normally */
	UPROPERTY(BlueprintAssignable)
	FInteractCompleted InteractCompleted;

	/** [Server] On Event Interrupted Delegate To be bind by Owner normally */
	UPROPERTY(BlueprintAssignable)
	FInteractionInterrupted InteractionInterrupted;

	/** [Local] On Event Interact State Changes For Client. For Aesthetics purposes*/
	UPROPERTY(BlueprintAssignable)
	FOnRepInteractState OnRepInteractState;


};
