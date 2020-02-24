// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "FuctionObject.h"


class FToolBarBuilder;
class FMenuBuilder;

class FButtonExampleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	FReply ButtonReply();
	FReply ButtonReply2();
	FReply ButtonReply3();
	FReply ButtonReply4();
	void PlacingStaticMesh();
	void DeleteAll();
	void read(FuctionObject fo);
	bool  checkQueue(float delta);
	int GetSpF() const;
	void Addictive(ECheckBoxState state);

private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void OnSpFChanged(int x);
	
	void OnYChanged(float y);
	void OnZChanged(float x);
	

	void AddMenuExtension(FMenuBuilder& Builder);

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	
private:
	TSharedPtr<class FUICommandList> PluginCommands;
};

