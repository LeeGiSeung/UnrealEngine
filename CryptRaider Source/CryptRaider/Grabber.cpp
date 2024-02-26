// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();


	// UPhysicsHandleComponent* PhysicsHandle =  GetPhysicsHandle();
	// if(PhysicsHandle !=nullptr){
	// 	UE_LOG(LogTemp,Display,TEXT("Got Physics Handle: %s"),*PhysicsHandle->GetName());
	// 	PhysicsHandle->GetName();
	// }
	// else
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("No Physics Handle Found %s"));
	// }
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent* PhysicsHandle =  GetPhysicsHandle();
	if(PhysicsHandle == nullptr){
		return;
	}

	if(PhysicsHandle->GetGrabbedComponent() != nullptr){
	FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}
}

void UGrabber::Grab(){

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if(PhysicsHandle == nullptr){
		return;
	}
	//스윕하고 시작과 끝을 계산해 히트했는지 Bool 값을 반환함
	//#########################################################

	FHitResult HitResult;
	bool HasHit = GetGrabableInReach(HitResult);

	if(HasHit)
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		HitComponent->SetSimulatePhysics(true);
		AActor* HitActor = HitResult.GetActor();
		HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HitComponent->WakeAllRigidBodies();
		HitResult.GetActor()->Tags.Add("Grabbed");
		// DrawDebugSphere(GetWorld(), HitResult.Location,10,10, FColor::Green, false, 5);
		// DrawDebugSphere(GetWorld(), HitResult.ImpactPoint,10,10, FColor::Red, false, 5);
		// AActor* HitActor = HitResult.GetActor();
		// UE_LOG(LogTemp,Display,TEXT("Hit Actor: %s"), *HitActor->GetActorNameOrLabel());
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			HitResult.GetComponent()->GetComponentRotation()
			// GetComponentRotation()
		);
	}
	// else
	// {
	// 	UE_LOG(LogTemp,Display,TEXT("No Actor Hit"));
	// }
	//#########################################################
}

void UGrabber::Release(){
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	
	if(PhysicsHandle == nullptr){
		return;
	}
	if(PhysicsHandle->GetGrabbedComponent() != nullptr){
		AActor* GrabbedActor = PhysicsHandle->GetGrabbedComponent()->GetOwner();
		GrabbedActor->Tags.Remove("Grabbed");
		PhysicsHandle->ReleaseComponent();
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent* Result = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(Result == nullptr){
		// UE_LOG(LogTemp, Error, TEXT("Grabber requires a UPhysicsHandleComponent"));
	}
	return Result;
}

bool UGrabber::GetGrabableInReach(FHitResult& OutHitResult) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;
	// DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	// DrawDebugSphere(GetWorld(), End,10,10, FColor::Blue, false, 5);
	//내가 설정한 원으로 콜리션 원 만들기
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	bool HasHit;
	return HasHit = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel2,Sphere);

}




	//바라보는 시선 표현
	// FRotator MyRotation = GetComponentRotation();
	// FString RotationString = MyRotation.ToCompactString();
	// UE_LOG(LogTemp, Display, TEXT("Grabber: %s"),*RotationString);

	//UE_LOG(LogTemp, Display,TEXT("Original Damage is : %f"),Damage);
	//float time = GetWorld()->TimeSeconds;
	//UE_LOG(LogTemp, Display,TEXT("Current Time is : %f"),time);void PrintDamage(float Damage);

