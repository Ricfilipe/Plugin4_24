// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "ButtonExample.h"
#include "ButtonExampleStyle.h"
#include "ButtonExampleCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SSlider.h"
#include "Misc/Attribute.h"
#include "ButtonExample/Public/Primitive.h"
#include "UObject/ConstructorHelpers.h"
#include "Primitive.h"
#include "FuctionObject.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "FRunnableConsumer.h"
#include "FRunnableProducer.h"
#include <string>
#include <iostream>	
#include <fstream>
#include "Builders/CubeBuilder.h"
#include "Widgets/Input/SCheckBox.h"
#include "Styling/SlateTypes.h"
#include "CustomBrushes/VertixBuilder.h"
#include "Operations/Operation.h"
#include "rpi_service.h"




using namespace std;

 rpi_service::RpiService* rpi;

static const FName ButtonExampleTabName("ButtonExample");

TAttribute<int> SpF;
bool AddictiveAtribute=true;

float Vy, Vz;
static int cubecount =0;


FRunnableProducer* producer;

#define LOCTEXT_NAMESPACE "FButtonExampleModule"

void FButtonExampleModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

		
		ofstream myfile;
		myfile.open("C:\\Users\\Utilizador\\Documents\\teste.txt");

		int x=0, z=0;


		for (int i=0; i < 20; i++) {
			for (int j=0; j < 20; j++) {
				myfile << "cube " << x << " 0 " << z << " 0 0 0 2 2 1 /Game/StarterContent/Materials/M_Wood_Oak.M_Wood_Oak\n";
					x += 140;
			}
			x = 0;
			z += 140;
		}
		myfile.close();

	FButtonExampleStyle::Initialize();
	FButtonExampleStyle::ReloadTextures();

	FButtonExampleCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FButtonExampleCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FButtonExampleModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FButtonExampleModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FButtonExampleModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ButtonExampleTabName, FOnSpawnTab::CreateRaw(this, &FButtonExampleModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FButtonExampleTabTitle", "ButtonExample"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	 FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FButtonExampleModule::checkQueue),0.0f);
	 producer = new FRunnableProducer();
}

void FButtonExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	delete producer;


	FButtonExampleStyle::Shutdown();

	FButtonExampleCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ButtonExampleTabName);
}

TSharedRef<SDockTab> FButtonExampleModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::FromString(
		"Action done per Tick");
	SpF.Set(3);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SScrollBox) + SScrollBox::Slot().Padding(10, 5)[
			SNew(SUniformGridPanel)
			.SlotPadding(FMargin(5.0f))
			+ SUniformGridPanel::Slot(0, 0)
			.HAlign(HAlign_Left)
				[
					
					SNew(STextBlock)
					.Text(WidgetText)
				]
			+ SUniformGridPanel::Slot(1, 0)
				.HAlign(HAlign_Left)
				[

					SNew(SSpinBox<int>).MaxValue(1000.0f)
					.MinDesiredWidth(100.0f)
					.OnValueChanged_Raw(this, &FButtonExampleModule::OnSpFChanged)
					.Value(SpF)	
				]
			+ SUniformGridPanel::Slot(0, 1)
			.HAlign(HAlign_Left)
				[
					SNew(SButton).OnClicked_Raw(this,&FButtonExampleModule::ButtonReply)[
						SNew(STextBlock)
							.Text (FText::FromString("Criar"))
					]
				]
			+ SUniformGridPanel::Slot(1, 1)
				.HAlign(HAlign_Left)
				[
					SNew(SButton).OnClicked_Raw(this, &FButtonExampleModule::ButtonReply2)[
						SNew(STextBlock)
							.Text(FText::FromString("Apagar"))
					]
				]
			+ SUniformGridPanel::Slot(0, 2)
				.HAlign(HAlign_Left)
				[
					SNew(SButton).OnClicked_Raw(this, &FButtonExampleModule::ButtonReply3)[
						SNew(STextBlock)
							.Text(FText::FromString("Brush"))
					]
				]
			+ SUniformGridPanel::Slot(1, 2)
				.HAlign(HAlign_Left)
				[
					SNew(SCheckBox).OnCheckStateChanged_Raw(this,&FButtonExampleModule::Addictive)
					
				]
			+ SUniformGridPanel::Slot(0, 3)
				.HAlign(HAlign_Left)
				[
					SNew(SButton).OnClicked_Raw(this, &FButtonExampleModule::ButtonReply4)[
						SNew(STextBlock)
							.Text(FText::FromString("Make StaticMesh"))
					]
				]
			]

		
		];

}

void FButtonExampleModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(ButtonExampleTabName);
}

void FButtonExampleModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FButtonExampleCommands::Get().OpenPluginWindow);
}

void FButtonExampleModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FButtonExampleCommands::Get().OpenPluginWindow);
}

void FButtonExampleModule::OnSpFChanged(int x)
{
	SpF.Set(x);
}





void  FButtonExampleModule::OnYChanged(float y) {
	Vy = y;
}

void  FButtonExampleModule::OnZChanged(float z) {
	Vz = z;
}

 FReply FButtonExampleModule::ButtonReply() {

	return FReply::Handled();
}

 FReply FButtonExampleModule::ButtonReply2() {

	 FButtonExampleModule::DeleteAll();
	 return FReply::Handled();
 }

 FReply FButtonExampleModule::ButtonReply3() {

	 TArray<FVector> vert;
	 vert.Add(FVector(50, 50, 0));
	 vert.Add(FVector(50, 30, 0));
	 vert.Add(FVector(20, 30, 0));
	 vert.Add(FVector(20, -30, 0));
	 vert.Add(FVector(50, -30, 0));
	 vert.Add(FVector(50, -50, 0));
	 vert.Add(FVector(-50, -50, 0));
	 vert.Add(FVector(-50, -30, 0));
	 vert.Add(FVector(-20, -30, 0));
	 vert.Add(FVector(-20, 30, 0));
	 vert.Add(FVector(-50, 30, 0));
	 vert.Add(FVector(-50, 50, 0));
	 Primitive::createBeamCustom(FVector(0,0,0), FRotator(0,0,0), 200.0f,vert,12);
	 return FReply::Handled();
 }

 FReply FButtonExampleModule::ButtonReply4() {
	 Primitive::ConvertToStaticMesh();
	 return FReply::Handled();
 }

 void FButtonExampleModule::PlacingStaticMesh() {
	 // Name & Transform

 }


 


   void FButtonExampleModule::DeleteAll() {
	 UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
	 TArray<AActor *> toRemove;

	 int size = currentWorld->PersistentLevel->Actors.Num();

	 for (int i = 0; i < size; i++) {
		 if (currentWorld->PersistentLevel->Actors[i]->ActorHasTag(FName("StaticMesh"))) {
			 toRemove.Add(currentWorld->PersistentLevel->Actors[i]);
		 };
	 }

	  size = toRemove.Num();

	 for (int i = 0; i < size; i++) {

		 currentWorld->RemoveActor(toRemove[i], true);
		 toRemove[i]->Destroy();
	 }
	 currentWorld->CleanupActors();
	 GEditor->EditorUpdateComponents();
	 currentWorld->UpdateWorldComponents(true, false);
	 GLevelEditorModeTools().MapChangeNotify();
 }




   bool  FButtonExampleModule::checkQueue(float delta) {
	   return Primitive::checkQueue(delta, SpF.Get());
   }

   int FButtonExampleModule::GetSpF() const
   {
	   return SpF.Get();
   }

   void FButtonExampleModule::Addictive(ECheckBoxState state) {


		   AddictiveAtribute = state == ECheckBoxState::Unchecked;
	   


   }
		
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FButtonExampleModule, ButtonExample)
