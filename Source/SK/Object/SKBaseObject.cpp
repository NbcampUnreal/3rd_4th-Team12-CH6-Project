// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/SKBaseObject.h"

// Sets default values
ASKBaseObject::ASKBaseObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASKBaseObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASKBaseObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

