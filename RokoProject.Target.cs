// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RokoProjectTarget : TargetRules
{
	public RokoProjectTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("RokoProject");
		WindowsPlatform.PCHMemoryAllocationFactor = 10000;
	}
}
