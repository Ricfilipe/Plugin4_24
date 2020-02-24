// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ButtonExampleCommands.h"

#define LOCTEXT_NAMESPACE "FButtonExampleModule"

void FButtonExampleCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ButtonExample", "Bring up ButtonExample window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
