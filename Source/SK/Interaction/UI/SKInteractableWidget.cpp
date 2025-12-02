#include "SKInteractableWidget.h"

void USKInteractableWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (DisplayText)
	{
		const FText FormattedText = FText::Format(FText::FromString(TEXT("E {0}")), InitialText);
		DisplayText->SetText(FormattedText);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Display Text is Not Binding"));
	}
}