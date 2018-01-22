// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"

#define OUT


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
    FindPhysicsHandleComponent();
    SetupInputComponent();
    
}

void UGrabber::FindPhysicsHandleComponent()
{
    // Find for attached Physic Handle
    PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
    if(PhysicsHandle == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *GetOwner()->GetName());
    }
    
}

void UGrabber::SetupInputComponent()
{
    // Find for attached Input Component
    InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
    if(InputComponent)
    {
        // Bind input
        InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
        InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
        
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *GetOwner()->GetName());
    }
    
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // If physics handle is attached
    if(PhysicsHandle->GrabbedComponent)
    {
        // Move the object that we are holding
        PhysicsHandle->SetTargetLocation(GetReachLineEnd());
    }

}

void UGrabber::Grab()
{
    auto HitResult = GetFirstPhysicsBodyInReach();
    auto ComponentToGrab = HitResult.GetComponent();
    auto ActorHit = HitResult.GetActor();
    
    // If we hit something then attach a physics handle
    if(ActorHit)
    {
        PhysicsHandle->GrabComponent(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), true);// Allow Rotation
    }
    
}

void UGrabber::Release()
{
    PhysicsHandle->ReleaseComponent();
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
    // Setup query parameters
    FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
    
    // Ray-cast out to reach distance
    FHitResult HitResult;
    GetWorld()->LineTraceSingleByObjectType(
                OUT HitResult,
                GetReachLineStart(),
                GetReachLineEnd(),
                FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
                TraceParameters
    );
    
    return HitResult;
}

FVector UGrabber::GetReachLineStart()
{
    FVector PlayerViewPointLocation;
    FRotator PlayerViewPointRotation;
    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
                                                               OUT PlayerViewPointLocation,
                                                               OUT PlayerViewPointRotation
                                                               );
    
    return PlayerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd()
{
    FVector PlayerViewPointLocation;
    FRotator PlayerViewPointRotation;
    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
                                                               OUT PlayerViewPointLocation,
                                                               OUT PlayerViewPointRotation
                                                               );
    
   return PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);
}

