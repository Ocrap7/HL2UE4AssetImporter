// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HL2EntityData.h"

#include "BaseEntity.generated.h"

class UBaseEntityComponent;

class ABaseEntity;
class AVBSPInfo;

DECLARE_LOG_CATEGORY_EXTERN(LogHL2IOSystem, Log, All);

USTRUCT()
struct FPendingFireOutput
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FEntityLogicOutput Output;

	UPROPERTY()
	ABaseEntity* Caller;

	UPROPERTY()
	ABaseEntity* Activator;

	UPROPERTY()
	TArray<FString> Args;

	UPROPERTY()
	FTimerHandle TimerHandle;
};

UCLASS()
class HL2RUNTIME_API ABaseEntity : public AActor
{
	GENERATED_BODY()
	
public:

	/** The raw entity data straight from the vbsp entities lump. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HL2")
	FHL2EntityData EntityData;

	/** The world model, if any, for this entity. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HL2")
	UStaticMesh* WorldModel;

	/** The targetname, if any, for this entity. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HL2")
	FName TargetName;

	/** The vbsp info for the map. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HL2")
	AVBSPInfo* VBSPInfo;

protected:

	/** All current logic outputs, valid or not, on this entity. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HL2")
	TArray<FEntityLogicOutput> LogicOutputs;

	UPROPERTY()
	TArray<FTimerHandle> PendingOutputs;

public:

	ABaseEntity();

	virtual void BeginPlay() override;

	/**
	 * Fires a logic input on this entity.
	 * Returns true if the logic input was successfully handled.
	 * The caller is the entity which is directly responsible for firing the input. For example, if the player walked into a trigger that has fired this input, the trigger is the caller.
	 * The activator is the entity which is indirectly responsible for firing the input. For example, if the player walked into a trigger that has fired this input, the player is the activator.
	 * Either caller or activator, or both, may be null.
	 */
	UFUNCTION(BlueprintCallable, Category = "HL2")
	bool FireInput(const FName inputName, const TArray<FString>& args, ABaseEntity* caller = nullptr, ABaseEntity* activator = nullptr);

	/**
	 * Fires a logic output on this entity.
	 * Returns the number of outputs that were actually fired.
	 */
	UFUNCTION(BlueprintCallable, Category = "HL2")
	int FireOutput(const FName outputName, const TArray<FString>& args, ABaseEntity* caller = nullptr, ABaseEntity* activator = nullptr);

	/**
	 * Resets all logic outputs to their initial state.
	 */
	UFUNCTION(BlueprintCallable, Category = "HL2")
	void ResetLogicOutputs();

	/**
	 * Resolves a target name into an array of targets.
	 * Supports wildcards and special target names.
	 */
	UFUNCTION(BlueprintCallable, Category = "HL2")
	void ResolveTargetName(const FName targetNameToResolve, TArray<ABaseEntity*>& out, ABaseEntity* caller = nullptr, ABaseEntity* activator = nullptr) const;

	/**
	 * Sets the parent of this entity to a given target name.
	 * The target name may also refer to an attachment, e.g. "parent,attachment".
	 * The target name can be blank to clear the parent.
	 */
	UFUNCTION(BlueprintCallable, Category = "HL2")
	void SetParent(const FString& parent, ABaseEntity* caller = nullptr, ABaseEntity* activator = nullptr);

	/**
	 * Gets if this entity has the specified spawnflag.
	 * Shortcut for fetching the spawnflag int from entity data and performing bitwise and.
	 */
	UFUNCTION(BlueprintPure, Category = "HL2")
	bool HasSpawnFlag(int flag) const;

protected:

	UFUNCTION()
	void FireOutputInternal(const FPendingFireOutput& data);

	/**
	 * Published when an input has been fired on this entity.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HL2")
	void OnInputFired(const FName inputName, const TArray<FString>& args, ABaseEntity* caller, ABaseEntity* activator);

	void OnInputFired_Implementation(const FName inputName, const TArray<FString>& args, ABaseEntity* caller, ABaseEntity* activator);

};
