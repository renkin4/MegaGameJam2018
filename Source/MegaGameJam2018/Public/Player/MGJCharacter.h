// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MGJCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MEGAGAMEJAM2018_API AMGJCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

public:
	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly ,Category = "Interact", meta = (AllowPrivateAccess = "true"))
	class UInteractorComp * InteractorComp;
	
	
};
