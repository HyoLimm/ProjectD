// Copyright(c) Depsee. All Rights Reserved.


#include "Game/Item/PDEffectActor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Game/AbilitySystem/PDAbilitySystemComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
APDEffectActor::APDEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	OverlapCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	OverlapCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapCollisionBox->OnComponentBeginOverlap.AddDynamic(this,&ThisClass::OnBoxOverlap);
}


// Called when the game starts or when spawned
void APDEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APDEffectActor::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ApplyEffectToTarget(OtherActor,InstantGamePlayEffectClass);

	this->Destroy();
}

void APDEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GamePlayEffectClass)
{
	check(GamePlayEffectClass);
	if (auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		const float Level = 1.0f;
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GamePlayEffectClass, Level, EffectContext);
		if (EffectSpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		}
	}	
}

