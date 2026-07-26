// Made by Kurchev Aleksandr; e-mail: kurchev-al@yandex.ru

#include "Actors/Tower.h"

#include "Abilities/TowerDefenceTags.h"
#include "AbilitySystemComponent.h"
#include "Attributes/TowerAttributeSet.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Utils/BaseUtils.h"
#include "Widgets/ProgressBarWidget.h"

ATower::ATower()
{
    TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
    SetRootComponent(TowerMesh);

    TowerHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TowerHitBox"));
    TowerHitBox->SetupAttachment(TowerMesh);

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    WidgetComponent->SetupAttachment(TowerMesh);

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
}

UAbilitySystemComponent* ATower::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ATower::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
    if (HealthWidget.IsValid())
    {
        HealthWidget->SetPercent(OnAttributeChangeData.NewValue / AttributeSet->GetMaxHealth());
    }
}

void ATower::BeginPlay()
{
    Super::BeginPlay();

    AttributeSet = AbilitySystemComponent->GetSet<UTowerAttributeSet>();
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UTowerAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

    TowerHitBox->OnComponentBeginOverlap.AddDynamic(this, &ATower::OnEnemyEntered);
    if (HealthWidget = Cast<UProgressBarWidget>(WidgetComponent->GetWidget()); HealthWidget.IsValid())
    {
        HealthWidget->SetPercent(1.f);
    }
}

void ATower::OnEnemyEntered(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    AbilitySystem::SendGameplayEventToInstigator(OtherActor, this, Action_Damage_Take);
}
