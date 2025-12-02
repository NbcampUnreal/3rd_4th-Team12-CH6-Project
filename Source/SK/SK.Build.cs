// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SK : ModuleRules
{
	public SK(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput"
			
			// UI
			,"UMG", "Slate", "SlateCore", "CommonUI", "CommonInput",
			"Paper2D",
			
			// GAS
			"GameplayAbilities",       
			"GameplayTags",            
			"GameplayTasks",   
			
			// AI
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			
			// Animation
			"MotionWarping",
			
			// Niagara 관련 추가
			"Niagara","NiagaraCore", "NiagaraShader", "RenderCore",

			// IP 관련 내용
			"HTTP",				// 대문자로 수정! ("Http") // Module 'Http' (referenced via Target -> TFD.Build.cs) has incorrect text case. Did you mean 'HTTP'?
			"Json",				// (선택적 - JSON 파싱 시 필요)
			"JsonUtilities",	// (선택적 - JSON 구조체 바인딩 시 필요)
			
			//폴더 내 에셋 리스트 검색
			"AssetRegistry",
			
			// 지오메트리 컬랙션
			"GeometryCollectionEngine",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayDebugger" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		
		PublicIncludePaths.AddRange(new string[] { 
			ModuleDirectory + "/../SK",
			// Engine 의 경로는 팀원마다 상이할 수 있습니다
			// 에러를 일이키는 것은 아니지만 해당 폴더를 찾을 수 없다는 에러가 발생합니다.
			// 제거 후 빌드와 테스트를 통과 함으로 주석 처리 했습니다.
			//Path.Combine(ModuleDirectory, "../../Engine/Plugins/2D/Paper2D/Source/Paper2D/Public")
		});
		
	}
}
