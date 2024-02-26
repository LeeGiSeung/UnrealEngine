#include "Mover.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UMover::UMover()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMover::BeginPlay()
{
	Super::BeginPlay();

	OriginalLocation = GetOwner()->GetActorLocation();

	// ...
	
}

//컴포넌트를 통해 액터에게 사운드를 부여하거나 액터의 위치를 파악하거나 설정하는 등의 작업을 수행할려면 포인터를 액터에게 먼저 전달해야한다.
// Called every frame
void UMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FVector TargetLocation = OriginalLocation;
	if (ShouldMove){
		TargetLocation = OriginalLocation + MoveOffset;
	}
	FVector CurrentLocation = GetOwner()->GetActorLocation();
	// FVector TargetLocation = OriginalLocation + MoveOffset;
	// float Speed = FVector::Distance(OriginalLocation, TargetLocation) / MoveTime;
	float Speed = MoveOffset.Length()/MoveTime;

	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);
	GetOwner() -> SetActorLocation(NewLocation);
}

void UMover::SetShouldMove(bool NewShouldMove){
	ShouldMove = NewShouldMove;
}

