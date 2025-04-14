// Fill out your copyright notice in the Description page of Project Settings.


#include "ThisProjectFunctionLibrary.h"

#include "MyProjectDemo1/FilePaths/DefaultPropertyValue.h"

FVector UThisProjectFunctionLibrary::FVectorZToGround(const FVector& Source)
{
	return FVector(Source.X, Source.Y, VectorToGround_FloatValue);
}

void UThisProjectFunctionLibrary::ClampMoveRange2D(const FVector& CurrentLocation, float RangeToClamp,
                                                 FVector& ClampedTargetLocation)
{
	// 计算2D距离（忽略高度差异）
	float Distance2D = FVector::Dist2D(CurrentLocation, ClampedTargetLocation);
	
	// 如果2D距离超过了允许的范围
	if (Distance2D > RangeToClamp)
	{
		// 计算2D方向（保持Z不变）
		FVector Direction = ClampedTargetLocation - CurrentLocation;
		Direction.Z = 0; // 确保我们只在XY平面上计算方向
		Direction.Normalize();
		
		// 使用最大允许距离修正目标位置，保留原始高度
		float OriginalZ = ClampedTargetLocation.Z;
		ClampedTargetLocation = CurrentLocation + (Direction * RangeToClamp);
		ClampedTargetLocation.Z = OriginalZ; // 恢复原始高度
	}
}
