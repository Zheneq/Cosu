// Fill out your copyright notice in the Description page of Project Settings.

#include "Cosu.h"
#include "Private/BaseClasses/CosuFL.h"
#include "Private/BaseClasses/CosuPlayerControllerBase.h"
#include "Runtime/Landscape/Classes/Landscape.h"
#include "ToolSpawner.h"

AToolSpawner::AToolSpawner()
{
	SnapDistance = 50.f;
	MaxPlaceDistance = 300.f;
	bSnapIsOn = true;
}

void AToolSpawner::Destroyed()
{
	Super::Destroyed();

	if (IsValid(Building))
	{
		Building->Destroy();
	}
}

void AToolSpawner::Use_Implementation()
{
	if (IsValid(Building) && Building->Place())
	{
		Deactivate();
	}
}

void AToolSpawner::Tick(float DeltaTime)
{
	if (Holder && Building)
	{
		bool bCanBePlaced = false;
		ACosuPlayerControllerBase* PC = Cast<ACosuPlayerControllerBase>(Holder->GetController());
		FHitResult HitResult;
		FCollisionObjectQueryParams::InitType QueryType = UCosuFL::HasOnlyFlag(Building->CanBeFoundedOn, EBuildingFoundationType::F_Landscape) ? FCollisionObjectQueryParams::InitType::AllStaticObjects : FCollisionObjectQueryParams::InitType::AllObjects;

		if (PC && PC->TraceView(HitResult, 5.f * MaxPlaceDistance, QueryType))
		{
			Building->SetActorTransform(FTransform(Holder->GetActorRotation(), HitResult.Location, FVector(1.f, 1.f, 1.f)));

			bool bSnapRequired = UCosuFL::HasOnlyFlag(Building->CanBeFoundedOn, EBuildingFoundationType::F_Socket);

			bool bSnapped = false;
			bool bGoodFoundation = true;
			if (bSnapRequired || bSnapIsOn)
			{
				FTransform SnappedTransform;
				bSnapped = Snap(SnappedTransform);
				if (bSnapped)
				{
					Building->SetActorTransform(SnappedTransform);
				}
				else
				{
					bGoodFoundation = CheckFoundation(HitResult.GetActor());
				}
			}
			else
			{
				bGoodFoundation = CheckFoundation(HitResult.GetActor());
			}
			
			bCanBePlaced = (bSnapped || (!bSnapRequired && bGoodFoundation)) && Building->IsValidLocation();


			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Black, FString::Printf(TEXT("ToolSpawner: Snap is %srequired, %strying to snap, %ssnapped"), bSnapRequired ? TEXT("") : TEXT("not "), bSnapIsOn ? TEXT("") : TEXT("not "), bSnapped ? TEXT("") : TEXT("not ")));
		}

		Building->SetCanBePlaced(bCanBePlaced);
		if (bCanBePlaced)
		{
			Building->SetActorHiddenInGame(false);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Black, TEXT("ToolSpawner: Nowhere to place."));
			Building->SetActorHiddenInGame(true);
		}
	}
	else
	{
		Deactivate();
	}
}

void AToolSpawner::SetBuildable(ABuildableBase* NewBuildable)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, TEXT("ToolSpawner activated."));

	Building = NewBuildable;
	UCosuFL::EnableActor(this);
}

void AToolSpawner::Deactivate()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Black, TEXT("ToolSpawner deactivated."));

	if (IsValid(Building))
	{
		Building->Destroy();
	}
	Building = nullptr;
	UCosuFL::EnableActor(this, false);
}

bool AToolSpawner::CheckFoundation(AActor* Foundation)
{
	if (!Building)
	{
		return false;
	}

	bool Result = false;

	if (UCosuFL::HasFlag(Building->CanBeFoundedOn, EBuildingFoundationType::F_Landscape))
	{
		Result = Result || !!Cast<ALandscape>(Foundation);
	}

	if (UCosuFL::HasFlag(Building->CanBeFoundedOn, EBuildingFoundationType::F_Floor))
	{
		ABuildableBase* F = Cast<ABuildableBase>(Foundation);
		Result = Result || F && F->bCanBeFoundation && (F->FoundationType == EBuildingFoundationType::F_Floor);
	}

	// Socket is not to be checked here (if the buildable is snapped, it is deemed to have proper foundation by default, and this function is not called)

	return Result;
}

bool AToolSpawner::Snap(FTransform& OutNewTransform) const
{
	if (!IsValid(Building))
	{
		return false;
	}

	if (UCosuFL::HasFlag(Building->CanBeFoundedOn, EBuildingFoundationType::F_Socket))
	{
		return GetBestSocketTransformInRadiusByTag(OutNewTransform, Building->GetActorLocation(), SnapDistance, Building->FoundationSocketTag, true);
	}
	else if (Building->GetMesh()->HasAnySockets())
	{
		// TODO: Optimize?
		float MinDistance = INFINITY;
		FName BestSocketName;
		FTransform BestSnap;
		auto SocketNames = Building->GetMesh()->GetAllSocketNames();
		for (const auto& SocketName : SocketNames)
		{
			const auto Socket = Building->GetMesh()->GetSocketByName(SocketName);
			if (Socket->Tag.Find(TEXT("MutualSnap")) == 0)
			{
				FTransform SocketTransform = Building->GetMesh()->GetSocketTransform(SocketName);
				FTransform T;
				bool bFound = GetBestSocketTransformInRadiusByTag(T, SocketTransform.GetLocation(), SnapDistance, Socket->Tag);
				if (bFound)
				{
					float Distance = FVector::Dist(T.GetLocation(), SocketTransform.GetLocation());
					if (Distance < MinDistance)
					{
						MinDistance = Distance;
						BestSocketName = SocketName;
						BestSnap = T;
					}
				}
			}
			
		}
		if (FMath::IsFinite(MinDistance))
		{
			FTransform SocketLocalTransform = Building->GetMesh()->GetSocketTransform(BestSocketName, RTS_Actor);
			const FQuat RotModifier(FVector::UpVector, PI);
			FQuat TargetRot = BestSnap.GetRotation() * RotModifier;
			FQuat NewRot = TargetRot * SocketLocalTransform.GetRotation().Inverse();
			FVector NewLocation = BestSnap.GetLocation() - NewRot.RotateVector(SocketLocalTransform.GetLocation());

			OutNewTransform.SetComponents(NewRot, NewLocation, FVector(1.f, 1.f, 1.f));

			DrawDebugSphere(GetWorld(), BestSnap.GetLocation(), 32, 2, FColor::Black);

			return true;
		}
	}

	return false;
}

bool AToolSpawner::GetBestSocketTransformInRadiusByTag(FTransform& OutTransform, FVector Center, float Radius, const FString& Tag, bool bRequireFoundation) const
{
	UWorld* World = GetWorld();
	if (!World) return false;



	bool Result = false;
	// Trace twice as far because socket may be a bit outside of a mesh
	const float TraceRadius = Radius + Radius;

	DrawDebugSphere(World, Center, TraceRadius, 8, FColor::Black);

	TArray<FOverlapResult> Overlaps;
	if (World->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects), FCollisionShape::MakeSphere(TraceRadius)))
	{
		// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Black, FString::Printf(TEXT("GetBestSocketTransformInRadiusByTag: %d objects."), Overlaps.Num()));

		for (const auto Overlap : Overlaps)
		{
			// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Black, FString::Printf(TEXT("GetBestSocketTransformInRadiusByTag: Object %s."), *AActor::GetDebugName(Overlap.GetActor())));

			ABuildableBase* Object = Cast<ABuildableBase>(Overlap.GetActor());
			if (Object && (!bRequireFoundation || Object->bCanBeFoundation))
			{
				// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Black, FString::Printf(TEXT("GetBestSocketTransformInRadiusByTag: Buildable %s."), *AActor::GetDebugName(Object)));

				const UStaticMeshComponent* Mesh = Object->GetMesh();
				TArray<FName> SocketNames = Mesh->GetAllSocketNames();

				// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Black, FString::Printf(TEXT("GetBestSocketTransformInRadiusByTag: %d sockets."), SocketNames.Num()));

				for (const auto SocketName : SocketNames)
				{

					// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Black, FString::Printf(TEXT("%s == %s is %d"), *Mesh->GetSocketByName(SocketName)->Tag, *Building->FoundationSocketTag, (int32)(Mesh->GetSocketByName(SocketName)->Tag == Building->FoundationSocketTag)));

					if (Mesh->GetSocketByName(SocketName)->Tag == Tag)
					{
						FTransform SocketTransform = Mesh->GetSocketTransform(SocketName);

						// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Black, FString::Printf(TEXT("GetBestSocketTransformInRadiusByTag: Found a socket @ (%f %f %f) ."), SocketTransform.GetLocation().X, SocketTransform.GetLocation().Y, SocketTransform.GetLocation().Z));

						GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.f, FColor::Black, FString::Printf(TEXT("GetBestSocketTransformInRadiusByTag: Distance to socket: %f (need <%f)."), (SocketTransform.GetLocation() - Center).Size(), Radius));

						if ((SocketTransform.GetLocation() - Center).Size() <= Radius
							&& (!Result || (SocketTransform.GetLocation() - Center).SizeSquared() < (OutTransform.GetLocation() - Center).SizeSquared()))
						{
							Result = true;
							OutTransform = SocketTransform;
						}

						DrawDebugPoint(World, SocketTransform.GetLocation(), 50.f, FColor::Red, false, 10.1f);
					}
				}
			}
		}
	}
	return Result;
}

/*
TArray<FTransform> AToolSpawner::GetAllSocketTransformsInRadiusByTag(FVector Center, float Radius, const FString& Tag) const
{
	TArray<FTransform> Result;
	UWorld* World = GetWorld();
	if (!World) return Result;

	// Trace twice as far because socket may be a bit outside of a mesh
	const float TraceRadius = Radius + Radius;
	TArray<FOverlapResult> Overlaps;
	if (World->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity, FCollisionObjectQueryParams(), FCollisionShape::MakeSphere(TraceRadius)))
	{
		for (const auto Overlap : Overlaps)
		{
			ABuildableBase* Object = Cast<ABuildableBase>(Overlap.GetActor());
			if (Object)
			{
				const UStaticMeshComponent* Mesh = Object->GetMesh();
				TArray<FName> SocketNames = Mesh->GetAllSocketNames();
				for (const auto SocketName : SocketNames)
				{
					if (Mesh->GetSocketByName(SocketName)->Tag == Object->FoundationSocketTag)
					{
						FTransform SocketTransform = Mesh->GetSocketTransform(SocketName);
						if ((SocketTransform.GetLocation() - Center).Size() <= Radius)
						{
							Result.Add(SocketTransform);
						}
					}
				}
			}
		}
	}
	return Result;
}
*/
