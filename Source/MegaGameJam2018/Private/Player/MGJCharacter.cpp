// Fill out your copyright notice in the Description page of Project Settings.

#include "MGJCharacter.h"
#include "InteractorComp.h"

// Sets default values
AMGJCharacter::AMGJCharacter(const class FObjectInitializer & ObjectInitializer)
	:Super(ObjectInitializer)
{
	InteractorComp = ObjectInitializer.CreateDefaultSubobject<UInteractorComp>(this, TEXT("Interactor Component"));

	PrimaryActorTick.bCanEverTick = false;
}

// Called to bind functionality to input
void AMGJCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InteractorComp->SetupPlayerInputComponent(PlayerInputComponent);
}

void AMGJCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(InteractorComp);
	InteractorComp->InitInteractor(this, Cast<APlayerController>(GetController()));
}
