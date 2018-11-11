// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableComp.h"
#include "LatentInteractableComp.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Interact), meta = (BlueprintSpawnableComponent))
class YANGINTERACT_API ULatentInteractableComp : public UInteractableComp
{
	GENERATED_BODY()

public:
	ULatentInteractableComp();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void TryToInterrupt() override;

protected:
	/** [Server] On interact */
	virtual void OnInteract(UInteractorComp* InteractorComp) override;

	/** [Server] On Interact, this function will run to check condition on every tick*/
	virtual void OnInteractLatent();

	/** [Server] + [Local] A value that determine how long the interaction will take place for*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Interact")
	float InteractDelayDuration;

private:
	/** [Server] World Time during Interaction */
	UPROPERTY(Transient)
	float InteractTime;

	/**
	* If true this Will set so that you don't have to hold for the Timer to continue
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	uint8 bTriggerTimer : 1;

	/**
	* If True, Character have to be facing at the actor while interacting
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	uint8 bShouldFaceInteractable : 1;

};
