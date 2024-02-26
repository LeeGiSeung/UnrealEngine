// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Tower.h"
#include "Tank.h"

void ATower::Tick(float DeltaTime){
    Super::Tick(DeltaTime);

    // if(Tank){
    //     // 탱크까지의 거리를 구하고
    //    float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
    //    // 탱크가 사정거리에 들어왔는지
    //    if (Distance <= FireRange){
    //     // 만약 들어왔다면 Turret의 시선을 탱크에게로
    //         RotateTurret(Tank->GetActorLocation());
    //    }
    // }
    
    if (InFireRange()){
        RotateTurret(Tank->GetActorLocation());
    }
}

void ATower::HandleDestruction(){
    Super::HandleDestruction();
    Destroy();
}

void ATower::BeginPlay(){
    Super::BeginPlay();

    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this,0));
    
    GetWorldTimerManager().SetTimer(FireRateTimerHandle,this,&ATower::CheckFireCondition, FireRate, true);


}

void ATower::CheckFireCondition(){
    // if(Tank){
    //     // 탱크까지의 거리를 구하고
    //    float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
    //    // 탱크가 사정거리에 들어왔는지
    //    if (Distance <= FireRange){
    //     // Fire 함수 실행
    //         Fire();
    //    }
    // }
    if (Tank == nullptr){
        return;
    }

    if(InFireRange()&&Tank->bAlive){
        Fire();
    }
}

bool ATower::InFireRange(){
    if (Tank){
        float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
        if (Distance<=FireRange){
            return true;
        }
    }
    return false;

}