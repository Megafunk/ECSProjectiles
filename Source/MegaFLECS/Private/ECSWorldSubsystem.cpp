// Fill out your copyright notice in the Description page of Project Settings.


#include "ECSWorldSubsystem.h"
#include "flecs.h"
#include "MegaFLECSDeveloperSettings.h"
#include "ECSModule.h"

UECSWorldSubsystem::UECSWorldSubsystem()
	: Super()
{

}

void UECSWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnTickDelegate = FTickerDelegate::CreateUObject(this, &ThisClass::Tick);
	OnTickHandle = FTicker::GetCoreTicker().AddTicker(OnTickDelegate);

	ECSWorld = TSharedPtr<flecs::world>(new flecs::world());
	ECSWorld->set_context(GetWorld());

	const UMegaFLECSDeveloperSettings* DevSettings = GetDefault<UMegaFLECSDeveloperSettings>();

	for (auto& Modules : DevSettings->ECSModules)
	{
		Modules.GetDefaultObject()->InitializeComponents(ECSWorld);
	}

	for (auto& Modules : DevSettings->ECSModules)
	{
		Modules.GetDefaultObject()->InitializeSystems(ECSWorld);
	}

	for (auto& Modules : DevSettings->ECSModules)
	{
		Modules.GetDefaultObject()->FinishInitialize(ECSWorld);
	}

	
}

void UECSWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FTicker::GetCoreTicker().RemoveTicker(OnTickHandle);
	ECSWorld = nullptr;
}

bool UECSWorldSubsystem::Tick(float DeltaTime)
{
	if (ECSWorld.IsValid())
	{
		ECSWorld->progress(DeltaTime);
	}
	return true;
}
