// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ButtonExample : ModuleRules
{
	public ButtonExample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				"/Engine/Source/Developer/RawMesh/Public/RawMesh.h",
				"/Engine/Source/Runtime/CinematicCamera/Public/CineCameraActor.h",
				"/Engine/Source/Runtime/LevelSequence/Public/LevelSequenceActor.h",
				"/Engine/Source/Runtime/LevelSequence/Public/LevelSequence.h",
				"/Engine/Source/Editor/MovieSceneTools/Public/MovieSceneToolHelpers.h",
				"/Engine/Source/Editor/MovieSceneTools/Public/AutomatedLevelSequenceCapture.h",
				"/Engine/Source/Runtime/MovieSceneCapture/Public/Protocols/AudioCaptureProtocol.h",
				"/Engine/Source/Runtime/ImageWriteQueue/Public/ImageWriteTypes.h",
				"/Engine/Source/Runtime/MovieSceneCapture/Public/Protocols/ImageSequenceProtocol.h"

			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...

			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "BoostLibrary",
				"RawMesh",
				"CinematicCamera",
				"LevelSequence",
				"MovieSceneTracks",
				"MovieScene",
				"MovieSceneTools",
				"MovieSceneCapture",
				"ImageWriteQueue"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        bUseRTTI = true;
        bEnableExceptions = true;
    }
}
