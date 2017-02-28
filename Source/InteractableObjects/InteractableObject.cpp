// Fill out your copyright notice in the Description page of Project Settings.

#include "HomeInvasion.h"
#include "InteractableObject.h"

#include "../SaveSystem.h"

// Sets default values
AInteractableObject::AInteractableObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Root scene component creation
	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	RootComponent = SceneRoot;

	// Create the mesh component
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	/*Mesh->AttachParent = RootComponent;*/
	//Mesh->AttachTo(RootComponent);
	Mesh->SetupAttachment(RootComponent);

	// Set up the collision box that will be used for line tracing
	TraceCollisionBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Trace Collision Box"));
	TraceCollisionBox->SetBoxExtent(FVector(25.0f, 25.0f, 25.0f));
	TraceCollisionBox->ComponentTags.Add("InteractableObject");
	TraceCollisionBox->bGenerateOverlapEvents = false;
	TraceCollisionBox->SetCollisionProfileName("IgnoreOnlyPawn");
	/*TraceCollisionBox->AttachParent = Mesh;*/
	//TraceCollisionBox->AttachTo(Mesh);
	TraceCollisionBox->SetupAttachment(Mesh);

	Active = true;

	UsefulItemStencil = 252;
	
	NeedsGuidSet = true;

	Tags.Add("Interactable");
}

// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();

	if(UsefulItem)
		Tags.Add("UsefulItem");

	// Original position and rotation in the world
	OriginalPosition = GetActorLocation();
	OriginalRotation = GetActorRotation();
}

// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableObject::Interact(AActor* InteractingActor)
{
	// This function is overriden by the inherited classes
	// The function should always still be called however. Super::Interact()

	// Call potential blueprint function
	InteractEvent(InteractedWith, InteractingActor);
	InteractedWith = true;
}

void AInteractableObject::AddToBeDestroyed()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASaveSystem::StaticClass(), Actors);

	if (Actors.Num() == 0)
		return;

	if (Actors[0] != nullptr)
	{
		Cast<ASaveSystem>(Actors[0])->AddObjectToBeDestroyed(GUID);
	}
}

void AInteractableObject::SetOutlineActive(bool OutlineActive, int32 StencilValue)
{
	Mesh->SetRenderCustomDepth(OutlineActive);
	Mesh->CustomDepthStencilValue = StencilValue;
}