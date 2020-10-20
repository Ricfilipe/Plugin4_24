// Fill out your copyright notice in the Description page of Project Settings.


#include "Operations/Actor/Render/RenderCreation.h"

#include "CineCameraComponent.h"
#include "LevelSequenceActor.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Factories/Factory.h"
#include "Tracks/MovieSceneCameraCutTrack.h"
#include "MovieSceneTrack.h"
#include "Sections/MovieSceneCameraCutSection.h"
#include "MovieScene.h"
#include "MovieSceneToolHelpers.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Tracks/MovieSceneFloatTrack.h"
#include "Protocols/AudioCaptureProtocol.h"
#include "Protocols/ImageSequenceProtocol.h"
#include "ImageWriteTypes.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "GameFramework/GameMode.h"

#define LOCTEXT_NAMESPACE "RenderCreation"

FString sequenceName;
ULevelSequence* current = NULL;


UAutomatedLevelSequenceCapture* movieSceneCapture;

FMovieSceneFloatChannel* posX;
FMovieSceneFloatChannel* posY;
FMovieSceneFloatChannel* posZ;

FMovieSceneFloatChannel* rotX;
FMovieSceneFloatChannel* rotY;
FMovieSceneFloatChannel* rotZ;

FMovieSceneFloatChannel* apertureChannel;
FMovieSceneFloatChannel* focalChannel;
FMovieSceneFloatChannel* focusChannel;

RenderCreation::RenderCreation(FVector pos, FRotator rot, float focal, float ape, float fcs, int wdh, int hgt, int frm, FString nm, FString pth)
{
	camera_pos = pos;
	camera_rot = rot;
	focalLenght = focal;
	aperture = ape;
	focus = fcs;
	width = wdh;
	height = hgt;
	frame = frm;
	name = nm;
	path = pth;
}

Response RenderCreation::execute()
{
	ALevelSequenceActor* realActor = NULL;
	if (current == NULL || name != sequenceName) {
		sequenceName = name;
		FTransform objectTrasform(FRotator(0, 0, 0), camera_pos, FVector(1, 1, 1));
		UWorld* currentWorld = GEditor->GetEditorWorldContext().World();
		ULevel* currentLevel = currentWorld->GetCurrentLevel();
		UClass* staticMeshClass = ALevelSequenceActor::StaticClass();
		AActor* newActorCreated = GEditor->AddActor(currentLevel, staticMeshClass, objectTrasform, true, RF_Public | RF_Standalone | RF_Transactional);

		realActor = Cast<ALevelSequenceActor>(newActorCreated);
		realActor->SetActorLabel(name);
		auto original = LoadObject<ULevelSequence>(nullptr, *FString("/ButtonExample/LevelSequence/ExampleSequence.ExampleSequence"));
		current = AuxiliarFunctions::CreateLevelSeq(sequenceName, FString("/Game/MyStaticMeshes/LevelSequence"), original);
		realActor->SetSequence(current);
		TArray<FMovieSceneBinding> binds = current->GetMovieScene()->GetBindings();

		auto transformTrack = Cast<UMovieScene3DTransformTrack>(binds[0].GetTracks()[0]);
		posX = transformTrack->GetAllSections()[0]->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[0];
		posY = transformTrack->GetAllSections()[0]->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[1];
		posZ = transformTrack->GetAllSections()[0]->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[2];

		rotX = transformTrack->GetAllSections()[0]->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[3];
		rotY = transformTrack->GetAllSections()[0]->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[4];
		rotZ = transformTrack->GetAllSections()[0]->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[5];



		auto focusTrack = Cast<UMovieSceneFloatTrack>(binds[1].GetTracks()[0]);
		focusChannel = focusTrack->GetAllSections()[0]->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[0];
		auto focalTrack = Cast<UMovieSceneFloatTrack>(binds[1].GetTracks()[1]);
		focalChannel = focalTrack->GetAllSections()[0]->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[0];
		auto apertureTrack = Cast<UMovieSceneFloatTrack>(binds[1].GetTracks()[2]);
		apertureChannel = apertureTrack->GetAllSections()[0]->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>()[0];

	}
	current->GetMovieScene()->SetSelectionRange(TRange<FFrameNumber>(FFrameNumber(param[3])));
	FFrameNumber currentFrame = FFrameNumber();
	currentFrame.Value = param[2];
	posX->AddConstantKey(currentFrame, camera_pos.X);
	posY->AddConstantKey(currentFrame, camera_pos.Y);
	posZ->AddConstantKey(currentFrame, camera_pos.Z);

	rotX->AddConstantKey(currentFrame, camera_rot.Euler().X);
	rotY->AddConstantKey(currentFrame, camera_rot.Euler().Y);
	rotZ->AddConstantKey(currentFrame, camera_rot.Euler().Z);

	focusChannel->AddConstantKey(currentFrame, focus);
	focalChannel->AddConstantKey(currentFrame, focalLenght);
	apertureChannel->AddConstantKey(currentFrame, aperture);



	movieSceneCapture = NewObject<UAutomatedLevelSequenceCapture>(GetTransientPackage(), UAutomatedLevelSequenceCapture::StaticClass(), FName("MovieCapture"), RF_Transient);
	movieSceneCapture->ImageCaptureProtocolType = UImageSequenceProtocol_PNG::StaticClass();
	UImageSequenceProtocol_PNG* ImageCaptureProtocol = NewObject<UImageSequenceProtocol_PNG>(movieSceneCapture, UImageSequenceProtocol_PNG::StaticClass(), FName("UUserDefinedImageCaptureProtocol"));
	movieSceneCapture->ImageCaptureProtocol = ImageCaptureProtocol;
	movieSceneCapture->LoadFromConfig();

	movieSceneCapture->LevelSequenceAsset = current->GetMovieScene()->GetOuter()->GetPathName();
	ULevelEditorPlaySettings* PlayInEditorSettings = GetMutableDefault<ULevelEditorPlaySettings>();


	FMovieSceneCaptureSettings Settings = movieSceneCapture->GetSettings();

	PlayInEditorSettings->NewWindowWidth = Settings.Resolution.ResX;
	PlayInEditorSettings->NewWindowHeight = Settings.Resolution.ResY;
	PlayInEditorSettings->CenterNewWindow = true;
	PlayInEditorSettings->LastExecutedPlayModeType = EPlayModeType::PlayMode_InEditorFloating;

	TSharedRef<SWindow> CustomWindow = SNew(SWindow)
		.Title(LOCTEXT("MovieRenderPreviewTitle", "Movie Render - Preview"))
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		.UseOSWindowBorder(true)
		.FocusWhenFirstShown(false)
		.ActivationPolicy(EWindowActivationPolicy::Never)
		.HasCloseButton(true)
		.SupportsMaximize(false)
		.SupportsMinimize(true)
		.MaxWidth(Settings.Resolution.ResX)
		.MaxHeight(Settings.Resolution.ResY)
		.SizingRule(ESizingRule::FixedSize);



	PlayInEditorSettings->CustomPIEWindow = CustomWindow;

	// Reset everything else
	PlayInEditorSettings->GameGetsMouseControl = false;
	PlayInEditorSettings->ShowMouseControlLabel = false;
	PlayInEditorSettings->ViewportGetsHMDControl = false;
	PlayInEditorSettings->ShouldMinimizeEditorOnVRPIE = true;
	PlayInEditorSettings->EnableGameSound = movieSceneCapture->AudioCaptureProtocolType != UNullAudioCaptureProtocol::StaticClass();
	PlayInEditorSettings->bOnlyLoadVisibleLevelsInPIE = false;
	PlayInEditorSettings->bPreferToStreamLevelsInPIE = false;
	PlayInEditorSettings->PIEAlwaysOnTop = false;
	PlayInEditorSettings->DisableStandaloneSound = false;
	PlayInEditorSettings->AdditionalLaunchParameters = TEXT("");
	PlayInEditorSettings->BuildGameBeforeLaunch = EPlayOnBuildMode::PlayOnBuild_Never;
	PlayInEditorSettings->LaunchConfiguration = EPlayOnLaunchConfiguration::LaunchConfig_Default;
	PlayInEditorSettings->SetPlayNetMode(EPlayNetMode::PIE_Standalone);
	PlayInEditorSettings->SetRunUnderOneProcess(true);
	PlayInEditorSettings->SetPlayNetDedicated(false);
	PlayInEditorSettings->SetPlayNumberOfClients(1);

	FString* directory = new FString;
	FString* output = new FString;
	path.Split(FString("\\"), directory, output, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	*directory = directory->Replace(*FString("\\"), *FString("/"));
	FFrameRate DisplayRate = FFrameRate();
	movieSceneCapture->Settings.bUseCustomFrameRate = true;
	movieSceneCapture->Settings.OutputFormat = *output;
	movieSceneCapture->Settings.OutputDirectory.Path = *directory;
	movieSceneCapture->Settings.FrameRate = DisplayRate;
	movieSceneCapture->Settings.ZeroPadFrameNumbers = 0;
	movieSceneCapture->Settings.bUseRelativeFrameNumbers = false;

	FFrameNumber StartFrame = FFrameNumber(0);
	FFrameNumber EndFrame = FFrameNumber(1);
	movieSceneCapture->SetFrameOverrides(StartFrame, EndFrame);

	movieSceneCapture->AddToRoot();
	movieSceneCapture->OnCaptureFinished().AddRaw(this, &Operation::OnLevelSequenceFinished);
	movieSceneCapture->Settings.Resolution = FCaptureResolution(param[0], param[1]);


	UGameViewportClient::OnViewportCreated().AddRaw(this, &Operation::OnPIEViewportStarted);




	GEditor->RequestPlaySession(true, nullptr, false);



	return realActor;
}




ULevelSequence* AuxiliarFunctions::CreateLevelSeq(const FString& AssetName, const FString& PackagePath, UObject* AssetToDuplicate) {
	IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
	UObject* NewAsset = nullptr;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		UClass* CurrentClass = *It;
		if (CurrentClass->IsChildOf(UFactory::StaticClass()) && !(CurrentClass->HasAnyClassFlags(CLASS_Abstract)))
		{
			UFactory* Factory = Cast<UFactory>(CurrentClass->GetDefaultObject());
			if (Factory->CanCreateNew() && Factory->ImportPriority >= 0 && Factory->SupportedClass == ULevelSequence::StaticClass())
			{
				if (AssetToDuplicate != nullptr)
				{
					NewAsset = AssetTools.DuplicateAsset(AssetName, PackagePath, AssetToDuplicate);
				}
				else
				{
					NewAsset = AssetTools.CreateAsset(AssetName, PackagePath, ULevelSequence::StaticClass(), Factory);
				}
				break;
			}
		}
	}
	return Cast<ULevelSequence>(NewAsset);
}

void Operation::OnLevelSequenceFinished() {


	GEditor->RequestEndPlayMap();

	FEditorDelegates::EndPIE.RemoveAll(this);
	UGameViewportClient::OnViewportCreated().RemoveAll(this);
	movieSceneCapture->OnCaptureFinished().RemoveAll(this);

	movieSceneCapture->Close();
	movieSceneCapture->RemoveFromRoot();
}


void Operation::OnPIEViewportStarted() {
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.WorldType == EWorldType::PIE)
		{

			FSlatePlayInEditorInfo* SlatePlayInEditorSession = GEditor->SlatePlayInEditorMap.Find(Context.ContextHandle);
			if (SlatePlayInEditorSession)
			{
				UWorld* CapturingFromWorld = Context.World();

				TSharedPtr<SWindow> Window = SlatePlayInEditorSession->SlatePlayInEditorWindow.Pin();


				FMovieSceneCaptureSettings Settings = movieSceneCapture->GetSettings();

				SlatePlayInEditorSession->SlatePlayInEditorWindowViewport.Get()->SetViewportSize(Settings.Resolution.ResX, Settings.Resolution.ResY);

				FVector2D PreviewWindowSize(Settings.Resolution.ResX, Settings.Resolution.ResY);

				// Keep scaling down the window size while we're bigger than half the desktop width/height
				{
					FDisplayMetrics DisplayMetrics;
					FSlateApplication::Get().GetCachedDisplayMetrics(DisplayMetrics);

					while (PreviewWindowSize.X >= DisplayMetrics.PrimaryDisplayWidth * .5f || PreviewWindowSize.Y >= DisplayMetrics.PrimaryDisplayHeight * .5f)
					{
						PreviewWindowSize *= .5f;
					}
				}

				// Resize and move the window into the desktop a bit
				FVector2D PreviewWindowPosition(50, 50);
				Window->ReshapeWindow(PreviewWindowPosition, PreviewWindowSize);

				if (movieSceneCapture->Settings.GameModeOverride != nullptr)
				{

					CapturingFromWorld->GetWorldSettings()->DefaultGameMode = movieSceneCapture->Settings.GameModeOverride;
				}

				auto CachedEngineShowFlags = SlatePlayInEditorSession->SlatePlayInEditorWindowViewport->GetClient()->GetEngineShowFlags();
				if (CachedEngineShowFlags && Settings.bUsePathTracer)
				{
					auto CachedPathTracingMode = CachedEngineShowFlags->PathTracing;
					CachedEngineShowFlags->SetPathTracing(true);
				}
				movieSceneCapture->Initialize(SlatePlayInEditorSession->SlatePlayInEditorWindowViewport, Context.PIEInstance);

			}

			FString CapturePath = movieSceneCapture->ResolveFileFormat(movieSceneCapture->Settings.OutputDirectory.Path, FFrameMetrics());
			UE_LOG(LogTemp, Warning, TEXT("%s"), *(CapturePath));
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Recieved PIE Creation callback but failed to find PIE World or missing FSlatePlayInEditorInfo for world."));
}

#undef LOCTEXT_NAMESPACE
