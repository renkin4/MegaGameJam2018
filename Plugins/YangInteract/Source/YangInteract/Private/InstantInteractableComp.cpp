// Fill out your copyright notice in the Description page of Project Settings.

#include "InstantInteractableComp.h"

void UInstantInteractableComp::OnInteract(UInteractorComp* InteractorComp)
{
	Super::OnInteract(InteractorComp);
	OnSucceeded();
}
