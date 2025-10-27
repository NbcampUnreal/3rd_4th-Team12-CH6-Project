// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SKCharacterBase.h"

// Sets default values
ASKCharacterBase::ASKCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASKCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASKCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASKCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

