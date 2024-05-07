// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MyShootingTarget : TargetRules
{
	public MyShootingTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		ExtraModuleNames.Add("MyShooting");
        ExtraModuleNames.AddRange(new string[] { "Aaaaa" });
    }
}
