// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ButtonExampleStyle.h"

class FButtonExampleCommands : public TCommands<FButtonExampleCommands>
{
public:

	FButtonExampleCommands()
		: TCommands<FButtonExampleCommands>(TEXT("ButtonExample"), NSLOCTEXT("Contexts", "ButtonExample", "ButtonExample Plugin"), NAME_None, FButtonExampleStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};