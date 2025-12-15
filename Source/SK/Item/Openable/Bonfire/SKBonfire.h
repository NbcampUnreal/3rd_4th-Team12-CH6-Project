#pragma once

#include "CoreMinimal.h"
#include "Item/Openable/SKOpenableBase.h"
#include "SKBonfire.generated.h"

class ASKPlayerCharacter;
class UNiagaraComponent;

UCLASS()
class SK_API ASKBonfire : public ASKOpenableBase
{
	GENERATED_BODY()

public:
	ASKBonfire();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|StaticMesh")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components|Niagara")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

#pragma region Interaction
	
public:
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bonfire")
	TObjectPtr<UAnimMontage> EndMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bonfire")
	TSubclassOf<UGameplayEffect> ResetGameplayEffect;
	
#pragma endregion

public:
	void ResetBonfire(ASKPlayerCharacter* PlayerCharacter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bonfire")
	bool bIsDefaultBonfire;
};
