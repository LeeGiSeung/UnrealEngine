// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

//생성자
//라이브 코딩에서 생성자를 추가하더라도 적용되지 않는 경우가 있으니 에디터를 닫고 Run Build Task를 꼭 진행하라
UTriggerComponent::UTriggerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    UE_LOG(LogTemp, Display,TEXT("Trigger constructing"));
}

void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();

    UE_LOG(LogTemp, Display,TEXT("Trigger Component Alive"));
}

void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // UE_LOG(LogTemp, Display,TEXT("Tick is suss"));
// 1111

    AActor* Actor = GetAcceptableActor();
    if(Actor!=nullptr){
        UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
        if(Component != nullptr){
            Component->SetSimulatePhysics(false);
        }
        Actor->AttachToComponent(this,FAttachmentTransformRules::KeepWorldTransform);
        Mover->SetShouldMove(true);
        // UE_LOG(LogTemp,Display,TEXT("Unlocking"));
    }
    else{
        Mover->SetShouldMove(false);
        // UE_LOG(LogTemp,Display,TEXT("Relocking"));
    }

    // TArray<AActor*> Actors;
    // //// 이 액터가 겹치는 액터(구성요소와 겹치는 모든 구성요소) 목록을 반환합니다.
    // //// .num을 이용해 몇개의 액터가 들어있는지 확인 할 수 있다.
    // //// 겹치는 액터가 하나 이상일때 작동하게 만들 수 있다.
    // GetOverlappingActors(Actors);

    // int32 index = 0;
    // while(index<Actors.Num()){
    //     FString ActorName = Actors[index]->GetActorNameOrLabel();
    //     UE_LOG(LogTemp,Display,TEXT("OverLapping : %s"),*ActorName)
    //     index = index++;
    // }

    // for (int32 i = 0; i<Actors.Num(); i++){
    //     FString ActorName = Actors[i]->GetActorNameOrLabel();
    //     UE_LOG(LogTemp,Display,TEXT("OverLapping : %s"),*ActorName)
    // }

    //모든 액터를 순환하는 코드 액터 말고도 다른 여러 개 Type에 사용가능하다.
    // for(AActor* Actor : Actors)
    // {
    //     // FString ActorName = Actor->GetActorNameOrLabel();
    //     if(Actor->ActorHasTag(AcceptableActorTag)){
    //     // if(Actor->ActorHasTag("Unlock1")){
    //         UE_LOG(LogTemp,Display,TEXT("Unlocking"));
    //     }
       
    // }

}
//22222
void UTriggerComponent::SetMover(UMover* NewMover){
    //파라미터와 변수
    Mover = NewMover;
}

AActor* UTriggerComponent::GetAcceptableActor() const{
    TArray<AActor*> Actors;
    GetOverlappingActors(Actors);
    for(AActor* Actor:Actors){
        bool HasAcceptableTag = Actor->ActorHasTag(AcceptableActorTag);
        bool IsGrabbed = Actor->ActorHasTag("Grabbed");
        if(HasAcceptableTag && !IsGrabbed){
            return Actor;
        }
    } 
    return nullptr;
}