// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MyShootingEditorTarget : TargetRules
{
	public MyShootingEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("MyShooting");
        ExtraModuleNames.AddRange(new string[] { "Aaaaa" });
    }
}
