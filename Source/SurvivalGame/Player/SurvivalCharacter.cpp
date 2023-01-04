// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InteractionComponent.h"

// Sets default values
ASurvivalCharacter::ASurvivalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(GetMesh(),FName("CameraSocket"));
	CameraComponent->bUsePawnControlRotation = true; 

	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>("HelmetMesh");
	HelmetMesh->SetupAttachment(GetMesh());
	HelmetMesh->SetMasterPoseComponent(GetMesh());

	ChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ChestMesh");
	ChestMesh->SetupAttachment(GetMesh());
	ChestMesh->SetMasterPoseComponent(GetMesh());

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LegsMesh");
	LegsMesh->SetupAttachment(GetMesh());
	LegsMesh->SetMasterPoseComponent(GetMesh());

	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>("FeetMesh");
	FeetMesh->SetupAttachment(GetMesh());
	FeetMesh->SetMasterPoseComponent(GetMesh());

	VestMesh = CreateDefaultSubobject<USkeletalMeshComponent>("VestMesh");
	VestMesh->SetupAttachment(GetMesh());
	VestMesh->SetMasterPoseComponent(GetMesh());

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>("HandsMesh");
	HandsMesh->SetupAttachment(GetMesh());
	HandsMesh->SetMasterPoseComponent(GetMesh());

	BackpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>("BackpackMesh");
	BackpackMesh->SetupAttachment(GetMesh());
	BackpackMesh->SetMasterPoseComponent(GetMesh());

	InteractionCheckFrequency = 0.f;
	InteractionCheckDistance = 1000.f;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetMesh()->SetOwnerNoSee(true);

}

// Called when the game starts or when spawned
void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASurvivalCharacter::StartCrouching()
{
	Crouch();
}

void ASurvivalCharacter::StopCrouching()
{
	UnCrouch();
}

void ASurvivalCharacter::MoveForward(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void ASurvivalCharacter::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void ASurvivalCharacter::LookUp(float Val)
{	
	if (Val != 0.f)
	{
		AddControllerPitchInput(Val);
	}
}

void ASurvivalCharacter::Turn(float Val)
{
	if (Val != 0.f)
	{
		AddControllerYawInput(Val);
	}
}

// Called every frame
void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PerformInteractionCheck();

}

void ASurvivalCharacter::PerformInteractionCheck()
{
	if (GetController() == nullptr)
	{
		return;
	}

	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector EyesLoc;
	FRotator EyesRot;

	GetController()->GetPlayerViewPoint(EyesLoc, EyesRot);

	FVector TraceStart = EyesLoc;
	FVector TraceEnd = (EyesRot.Vector() * InteractionCheckDistance) + TraceStart;
	FHitResult TraceHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		//Check if we hit an interactable object
		if (TraceHit.GetActor())
		{
			if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();
				if (InteractionComponent != GetInteractable() && Distance <= InteractionComponent->InteractionDistance)
				{
					FoundNewInteractable(InteractionComponent);
				}
				else if (Distance > InteractionComponent->InteractionDistance && GetInteractable())
				{
					CouldntFindInteractable();
				}

				return;
			}
		}
	}
	CouldntFindInteractable();
}

void ASurvivalCharacter::CouldntFindInteractable()
{
	if (InteractionData.ViewedInteractionComponent)
	{
		InteractionData.ViewedInteractionComponent->SetHiddenInGame(true);
	}
}

void ASurvivalCharacter::FoundNewInteractable(UInteractionComponent* Interactable)
{
	if (Interactable)
	{
		Interactable->SetHiddenInGame(false);
		InteractionData.ViewedInteractionComponent = Interactable; 
	}
}

void ASurvivalCharacter::BeginInteract()
{
	if (!HasAuthority())
	{
		ServerBeginInteract();
	}
}

void ASurvivalCharacter::EndInteract()
{
	if (!HasAuthority())
	{
		ServerEndInteract();
	}
}

void ASurvivalCharacter::ServerEndInteract_Implementation()
{
	EndInteract();
}

bool ASurvivalCharacter::ServerEndInteract_Validate()
{
	return true;
}

void ASurvivalCharacter::ServerBeginInteract_Implementation()
{
	BeginInteract();
}

bool ASurvivalCharacter::ServerBeginInteract_Validate()
{
	return true; 
}

// Called to bind functionality to input
void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASurvivalCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ASurvivalCharacter::EndInteract);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASurvivalCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASurvivalCharacter::StopCrouching);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASurvivalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurvivalCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASurvivalCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ASurvivalCharacter::Turn);
}

