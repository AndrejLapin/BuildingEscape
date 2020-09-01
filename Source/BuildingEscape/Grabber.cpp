// Copyright Andrej Lapin 2020

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
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
	
	FindPhysicsHandle();

	SetupInputComponent();
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s was unable to find Input component!"), *GetOwner()->GetName());
	}
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("%s was unable to get PhysicsHandle from itself!(it might be missing on this object)"), *GetOwner()->GetName());
	}
}

void UGrabber::Grab()
{
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	//If we hit something then attach the physics handle
	//TODO attach physics handle.
	if (HitResult.GetActor())
	{
		PhysicsHandle->GrabComponentAtLocation
			(
			ComponentToGrab,
			NAME_None,
			GetLineTraceEnd()
			);
	}
	
}

void UGrabber::Release()
{
	PhysicsHandle->ReleaseComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If the physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}

	// Get players viewpoint
	
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;
	// Ray-cast out to a certain distance (Reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayerWorldPosition(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
		);

	AActor* AcotrHit = Hit.GetActor();

	if (AcotrHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor %s is hit"), *AcotrHit->GetName());
	}

	return Hit;
}

FVector UGrabber::GetLineTraceEnd() const
{
	FVector PlayerViewpointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint
	(
		OUT PlayerViewpointLocation,
		OUT PlayerViewPointRotation
		);

	// Draw a line from a player showing the reach.

	return PlayerViewpointLocation + PlayerViewPointRotation.Vector() * Reach;
}

FVector UGrabber::GetPlayerWorldPosition() const
{
	return GetOwner()->GetActorLocation();
}

