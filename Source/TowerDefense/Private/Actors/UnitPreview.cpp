

#include "Actors/UnitPreview.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AUnitPreview::AUnitPreview()
{
    CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    SetRootComponent(CapsuleCollision);

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(CapsuleCollision);
    Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

bool AUnitPreview::HasCollisionIntersections() const
{
    return !OverlappedActors.IsEmpty();
}

USkeletalMeshComponent* AUnitPreview::GetMesh() const
{
    return Mesh;
}

// Called when the game starts or when spawned
void AUnitPreview::BeginPlay()
{
    Super::BeginPlay();
    CapsuleCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBeginOverlap);
    CapsuleCollision->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnEndOverlap);
}

void AUnitPreview::OnBeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int, bool, const FHitResult&)
{
    OverlappedActors.Emplace(OtherActor);
}

void AUnitPreview::OnEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int)
{
    OverlappedActors.Remove(OtherActor);
}
