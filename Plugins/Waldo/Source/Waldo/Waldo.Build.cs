using UnrealBuildTool;

public class Waldo : ModuleRules
{
	public Waldo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
				
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"Serial",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
			);
	}
}
