// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MyShootingCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Aaaaa/Bbbbb.h"

AMyShootingCharacter::AMyShootingCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f,0.f,75.f);
	CameraBoom->RelativeRotation = FRotator(0.f,180.f,0.f);

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyShootingCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyShootingCharacter::PressedU);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyShootingCharacter::MoveRight);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyShootingCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyShootingCharacter::TouchStopped);
	//PlayerInputComponent->BindKey(EKeys::U, IE_Pressed, this, &AMyShootingCharacter::PressedU);
	//UClass* u = UBbbbb::StaticClass();
	//UE_LOG(LogTemp, Log, TEXT("aaaaaa %s AAAAAA"), *u->GetName());
	//UBbbbb* bbb = NewObject<UBbbbb>();

	//FName InFName("NativeFunc");
	//for (size_t i = 0; i < u->NativeFunctionLookupTable.Num(); i++)
	//{
	//	FNativeFunctionLookup NativeFunction = u->NativeFunctionLookupTable[i];
	//	if (NativeFunction.Name == InFName) {
	//		u->NativeFunctionLookupTable[i] = FNativeFunctionLookup(InFName, (Native)&(UBbbbb::execNativeFunc2));
	//		break;
	//	}
	//}
	//UFunction*  Func = u->FindFunctionByName(InFName);
	//Func->Bind();

	//bbb->NativeFunc(0.323);
	//int i = bbb->Callable(4);
	//UClass* MyBlueprintClass = LoadObject<UClass>(nullptr, TEXT("Blueprint'/Game/BbbBluepring/BbbbbBlueprint.BbbbbBlueprint_C'"));
	//if (MyBlueprintClass)
	//{
	//	UBbbbb* MyBlueprintObject = NewObject<UBbbbb>(this, MyBlueprintClass);
	//	MyBlueprintObject->NativeFunc(1);
	//	MyBlueprintObject->ImplementableFunc(true, 1);
	//}
}

void AMyShootingCharacter::MoveRight(float Value)
{
	// add movement in that direction
	AddMovementInput(FVector(0.f,-1.f,0.f), Value);
}

void AMyShootingCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// jump on any touch
	Jump();
}

void AMyShootingCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	StopJumping();
}


void AMyShootingCharacter::PressedU()
{


	//UClass* MyBlueprintClass = LoadObject<UClass>(nullptr, TEXT("Blueprint'/Game/BbbBluepring/BbbbbBlueprint.BbbbbBlueprint_C'"));
	//if (MyBlueprintClass)
	//{
	//	UBbbbb* MyBlueprintObject = NewObject<UBbbbb>(this, MyBlueprintClass);
	//	MyBlueprintObject->NativeFunc(1);
	//	MyBlueprintObject->ImplementableFunc(true, 1);
	//}
	//UObject* MyBlueprintClass2 = StaticLoadObject(UClass::StaticClass(), nullptr, TEXT("Blueprint'/Game/BbbBluepring/BbbbbBlueprint.BbbbbBlueprint'"));
	//if (MyBlueprintClass2)
	//{
	//	UBbbbb* MyBlueprintObject = NewObject<UBbbbb>(MyBlueprintClass2);
	//	MyBlueprintObject->NativeFunc(1);
	//}
}
