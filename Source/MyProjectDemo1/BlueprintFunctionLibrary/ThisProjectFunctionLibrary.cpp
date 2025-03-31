// Fill out your copyright notice in the Description page of Project Settings.


#include "ThisProjectFunctionLibrary.h"

FVector UThisProjectFunctionLibrary::FVectorZToGround(const FVector& Source)
{
	return FVector(Source.X, Source.Y, 5.f);
}

void UThisProjectFunctionLibrary::ClampMoveRange2D(const FVector& CurrentLocation, float RangeToMove,
                                                 FVector& TargetLocation)
{
	// 计算2D距离（忽略高度差异）
	float Distance2D = FVector::Dist2D(CurrentLocation, TargetLocation);
	
	// 如果2D距离超过了允许的范围
	if (Distance2D > RangeToMove)
	{
		// 计算2D方向（保持Z不变）
		FVector Direction = TargetLocation - CurrentLocation;
		Direction.Z = 0; // 确保我们只在XY平面上计算方向
		Direction.Normalize();
		
		// 使用最大允许距离修正目标位置，保留原始高度
		float OriginalZ = TargetLocation.Z;
		TargetLocation = CurrentLocation + (Direction * RangeToMove);
		TargetLocation.Z = OriginalZ; // 恢复原始高度
	}
}
