// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableComp.h"
#include "InstantInteractableComp.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Interact), meta = (BlueprintSpawnableComponent))
class YANGINTERACT_API UInstantInteractableComp : public UInteractableComp
{
	GENERATED_BODY()
	
protected:
	/** [Server] On interact */
	virtual void OnInteract(UInteractorComp* InteractorComp) override;
	
	
	
};
