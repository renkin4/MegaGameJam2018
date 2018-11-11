// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "YangInteract.h"

#define LOCTEXT_NAMESPACE "FYangInteractModule"

DEFINE_LOG_CATEGORY(LogYangInteract);

void FYangInteractModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogYangInteract, Log, TEXT("Starting Yang Interact Plugin Module"));
}

void FYangInteractModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogYangInteract, Log, TEXT("Shuting down Yang Interact Plugin Module"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FYangInteractModule, YangInteract)
