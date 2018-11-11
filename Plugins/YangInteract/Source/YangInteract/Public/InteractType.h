// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EFailedReason : uint8
{
	FR_None						UMETA(DisplayName = "None"),
	FR_OthersInteracting		UMETA(DisplayName = "Other's Interacting"),
	FR_IsPendingKill			UMETA(DisplayName = "Is Pending Kill"),
	FR_NotAllowingInteraction	UMETA(DisplayName = "Not Allowing Interaction"),
	FR_NotPressingButton		UMETA(DisplayName = "Not Pressing Button"),
	FR_LookedAway				UMETA(DisplayName = "Looked Away"),
	FR_NotInfrontInteractable	UMETA(DisplayName = "Not Infront Interactable"),
};

UENUM(BlueprintType)
enum class EInteractState : uint8
{
	IS_Idle			UMETA(DisplayName = "Idle"),
	IS_Interacting	UMETA(DisplayName = "Interacting"),
	IS_Completed	UMETA(DisplayName = "Completed"),
	IS_Failed		UMETA(DisplayName = "Failed"),
};
