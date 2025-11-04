// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/SKUIManagerSubSystem.h"
#include "UI/SKLayoutWidgetBase.h"
#include "AbilitySystemComponent.h"
#include "GameInstance/SKGameInstance.h"
#include "GameData/UILayoutDataAsset.h"
#include "CommonActivatableWidget.h"
#include "UI/LayoutWidget/SKConfirmLayouWidget.h"

void USKUIManagerSubSystem::CreateLayoutWidget()
{
	if(!SwtichAbleLayoutData) return;

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("LocalPlayer is nullptr"));
		return;
	}
	
	APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController is nullptr"));
		return;
	}

	CreateLayoutFromData(SwtichAbleLayoutData, PC);
	
	//기본으로 띄울 UI 태그
	CachedASC->AddLooseGameplayTag(TAG_UI_Layout_InGame);

	if (!ConfirmLayoutData)
		return;

	CreateLayoutFromData(ConfirmLayoutData, PC);
}

void USKUIManagerSubSystem::SetLayoutVisibeByTag(FGameplayTag LayoutTag)
{
	if (!LayoutWidgets.Contains(LayoutTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Target layout widget not found: %s"), *LayoutTag.ToString());
		return;
	}

	UCommonActivatableWidget* VisibleWidget = LayoutWidgets[LayoutTag];
	if (VisibleWidget)
	{
		VisibleWidget->SetVisibility(ESlateVisibility::Visible);
		VisibleWidget->ActivateWidget();
		CurrentLayoutTag = LayoutTag;
	}
}

void USKUIManagerSubSystem::SetLayoutHiddenByTag(FGameplayTag LayoutTag)
{
	if (!LayoutWidgets.Contains(LayoutTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetLayoutHiddenByTag widget not found: %s"), *LayoutTag.ToString());
	}
	else
	{
		UCommonActivatableWidget* HiddenWidget = LayoutWidgets[LayoutTag];
		if (HiddenWidget)
		{
			HiddenWidget->SetVisibility(ESlateVisibility::Hidden);
			HiddenWidget->DeactivateWidget();
		}
	}
}

void USKUIManagerSubSystem::SettingLayout()
{
	//DataAsset 설정(임시)
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
		return;

	UGameInstance* GI = LocalPlayer->GetGameInstance();
	if (!GI)
		return;
	
	USKGameInstance* MyGI = Cast<USKGameInstance>(GI);
	if (!MyGI)
		return;

	if (MyGI->TestLayoutData)
	{
		SwtichAbleLayoutData = Cast<UUILayoutDataAsset>(MyGI->TestLayoutData.Get());
	}

	if (MyGI->TestConfirmData)
	{
		ConfirmLayoutData = Cast<UUILayoutDataAsset>(MyGI->TestConfirmData.Get());
	}
	
	APlayerController* PC = LocalPlayer->GetPlayerController(GI->GetWorld());
	if (!PC)
		return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
		return;

	// Pawn에서 AbilitySystemComponent 가져오기
	CachedASC = Pawn->FindComponentByClass<UAbilitySystemComponent>();
	if (!CachedASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("USKUIManagerSubSystem::Initialize - AbilitySystemComponent not found on Pawn"));
	}

	CreateLayoutWidget();
}

void USKUIManagerSubSystem::RequestConfirmUI(FDataTableRowHandle ConfirmUIDataRow)
{
	if (!ConfirmUIDataRow.DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UIManager] ConfirmUIDataRow.DataTable is null!"));
		return;
	}

	// 데이터테이블 이름
	FString TableName = ConfirmUIDataRow.DataTable->GetName();

	// Row 이름
	FString RowName = ConfirmUIDataRow.RowName.ToString();

	UE_LOG(LogTemp, Log, TEXT("[UIManager] ConfirmUIDataRow Info - Table: %s | Row: %s"),
		*TableName, *RowName);

	ConfirmLayoutWidgets[TAG_UI_Layout_Confirm]->SetVisibility(ESlateVisibility::Visible);
	ConfirmLayoutWidgets[TAG_UI_Layout_Confirm]->ActivateWidget();
	
	USKConfirmLayouWidget* ConfirmWidget = Cast<USKConfirmLayouWidget>(ConfirmLayoutWidgets[TAG_UI_Layout_Confirm]);
	if (!ConfirmWidget)
		return;

	ConfirmWidget->SettingConfirmMesseage(ConfirmUIDataRow.RowName);
}

void USKUIManagerSubSystem::RequestResult(bool bResult)
{
	OnConfirmResult.Broadcast(bResult);
	ConfirmLayoutWidgets[TAG_UI_Layout_Confirm]->SetVisibility(ESlateVisibility::Hidden);
	ConfirmLayoutWidgets[TAG_UI_Layout_Confirm]->DeactivateWidget();
	LayoutWidgets[CurrentLayoutTag]->ActivateWidget();
}

FGameplayTagContainer USKUIManagerSubSystem::GetLayoutTags()
{
	return SwitchableLayoutTags;
}

void USKUIManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USKUIManagerSubSystem::Deinitialize()
{
	Super::Deinitialize();

	RemoveLayout();
}

//Layout 제거와 Layout 전환 이벤트 해제
void USKUIManagerSubSystem::RemoveLayout()
{
	for (auto& LayoutPair : LayoutWidgets)
	{
		if (LayoutPair.Value)
		{
			CachedASC->UnregisterGameplayTagEvent(LayoutTagDelegateHandles[LayoutPair.Key],LayoutPair.Key, EGameplayTagEventType::AnyCountChange);
			LayoutPair.Value->RemoveFromParent();
		}
	}
	LayoutWidgets.Empty();
	LayoutTagDelegateHandles.Empty();
	SwitchableLayoutTags.Reset();
}

void USKUIManagerSubSystem::CreateLayoutFromData(UUILayoutDataAsset* CreateData, APlayerController* OwningPC)
{
	for (const FLayoutWithSlots& LayoutWithSlots : CreateData->Layouts)
	{
		const FLayoutData& LayoutInfo = LayoutWithSlots.Layout;
 
		if (!LayoutInfo.LayoutWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("CreateLayoutFromData: LayoutWidgetClass is null for LayoutTag %s"), *LayoutInfo.LayoutTag.ToString());
			continue;
		}
 
		// 레이아웃 위젯 생성
		USKLayoutWidgetBase* LayoutWidget = CreateWidget<USKLayoutWidgetBase>(OwningPC, LayoutInfo.LayoutWidgetClass);
		if (!LayoutWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("CreateLayoutFromData: Failed to create LayoutWidget %s"), *LayoutInfo.LayoutTag.ToString());
			continue;
		}
 
		// 뷰포트에 추가
		LayoutWidget->AddToViewport();
		//해당 Layout의 Slot 데이터 전달
		LayoutWidget->SetSlotData(LayoutWithSlots.Slots);

		if (SwtichAbleLayoutData == CreateData)
		{
			//Layout 변경 이벤트 등록
			AddSwitchLayoutRegisterEvent(LayoutInfo.LayoutTag);

			//Set에 Layout 태그 추가
			SwitchableLayoutTags.AddTag(LayoutInfo.LayoutTag);

			//Map에 현재 레이아웃 태그와 위젯 추가
			LayoutWidgets.Add(LayoutInfo.LayoutTag, LayoutWidget);
		}
		else if (ConfirmLayoutData == CreateData)
		{
			ConfirmLayoutWidgets.Add(LayoutInfo.LayoutTag, LayoutWidget);
		}
	}
}

/*
 * NewCount 1 이상 :이미 있을 경우 태그 추가 X
 * NewCount 0: Layout 비표시로 변경
 * NewCount 1: Layout 표시로 변경 및 다른 Layout 비표시로 변경
 */
void USKUIManagerSubSystem::HandleSwitchLayout(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 1)
	{
		CachedASC->RemoveLooseGameplayTag(Tag);
	}
	else if (NewCount == 0)
	{
		SetLayoutHiddenByTag(Tag);
	}
	else if (NewCount == 1)
	{
		SetLayoutVisibeByTag(Tag);

		// SwitchableLayoutTags 컨테이너 내 태그 중, 현재 들어온 Tag 제외하고 ASC에서 제거
		for (const FGameplayTag& OtherTag : SwitchableLayoutTags)
		{
			if (OtherTag != Tag)
			{
				CachedASC->RemoveLooseGameplayTag(OtherTag);
				UE_LOG(LogTemp, Log, TEXT("Removed Layout Tag: %s"), *OtherTag.ToString());
			}
		}
	}
}

//Layout Tag의 숫자가 변경 되었을 경우(추가 혹은 삭제) 실행되는 이벤트 등록
void USKUIManagerSubSystem::AddSwitchLayoutRegisterEvent(FGameplayTag RegisterTag)
{
	if (!CachedASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC Null"));
		return;
	}

	if (LayoutTagDelegateHandles.Contains(RegisterTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("이미 등록된 태그."));
		return;
	}
	
	FDelegateHandle Handle = CachedASC->RegisterGameplayTagEvent(RegisterTag, EGameplayTagEventType::AnyCountChange)
		.AddUObject(this, &USKUIManagerSubSystem::HandleSwitchLayout);

	if (Handle.IsValid())
	{
		LayoutTagDelegateHandles.Add(RegisterTag, Handle);
	}
}
