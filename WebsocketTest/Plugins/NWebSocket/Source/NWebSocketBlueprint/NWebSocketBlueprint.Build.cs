
using UnrealBuildTool;
using System.IO;

public class NWebSocketBlueprint : ModuleRules
{
	public NWebSocketBlueprint(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "WebSockets"
            }
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"WebSockets"
			}
		);
		
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
	}
}
