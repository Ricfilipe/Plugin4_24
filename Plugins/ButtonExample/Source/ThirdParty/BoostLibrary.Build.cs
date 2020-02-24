// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System.IO;
using System;

namespace UnrealBuildTool.Rules
{
    public class BoostLibrary : ModuleRules
    {
        private string ModulePath
        {
            get { return ModuleDirectory; }
        }

        private string BinariesPath
        {
            get { return Path.GetFullPath(Path.Combine(ModulePath, "../Binaries/")); }
        }

        public BoostLibrary(ReadOnlyTargetRules Target) : base(Target)
        {
            // Tell Unreal that this Module only imports Third-Party-Assets
            Type = ModuleType.External;

            LoadPCL(Target);
        }

        public bool LoadPCL(ReadOnlyTargetRules Target)
        {
            bool isLibrarySupported = false;
            //bool bDebug = (Target.Configuration == UnrealTargetConfiguration.Debug && BuildConfiguration.bDebugBuildsActuallyUseDebugCRT);

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                isLibrarySupported = true;

                //string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";

                // Explicitly name the used libraries
                
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_atomic-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_chrono-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_container-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_contract-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_coroutine-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_date_time-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_exception-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_fiber-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_filesystem-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_graph-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_iostreams-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_locale-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_log_setup-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_log-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_math_c99f-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_math_c99l-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_math_c99-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_math_tr1f-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_math_tr1l-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_math_tr1-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_system-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_thread-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_timer-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_prg_exec_monitor-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_random-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_program_options-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_regex-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_serialization-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_stacktrace_noop-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_stacktrace_windbg-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_stacktrace_windbg_cached-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_system-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_test_exec_monitor-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_type_erasure-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_wave-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_wserialization-vc142-mt-x64-1_71.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "Boost/lib/libboost_unit_test_framework-vc142-mt-x64-1_71.lib"));


                //PublicAdditionalLibraries.Add(Path.Combine(ModulePath, "PCL_181/lib/pcl_visualization_release.lib"));
            }

            if (isLibrarySupported)
            {
   
                PublicIncludePaths.Add(Path.Combine(ModulePath, "Boost/include/boost-1_71"));
              

                //PublicIncludePaths.Add(Path.Combine(ModulePath, "FLANN/include"));
                //PublicIncludePaths.Add(Path.Combine(ModulePath, "VTK/include/vtk-8.0"));

                // Not sure if needed
                Definitions.Add("_CRT_SECURE_NO_WARNINGS=1");
                Definitions.Add("BOOST_DISABLE_ABI_HEADERS=1");

                // Needed configurations in order to run Boost
                bUseRTTI = true;
                bEnableExceptions = true;
                //bEnableUndefinedIdentifierWarnings = false;
            }

    
            Definitions.Add(string.Format("WITH_BOOST_BINDING={0}", isLibrarySupported ? 1 : 0));

            return isLibrarySupported;
        }
    }
}