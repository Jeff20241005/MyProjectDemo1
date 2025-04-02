// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionBase.h"
#include "ActionCheckBoxUI.generated.h"

class UCheckBox;
struct FSlateBrush;

/**
 * 
 */
UCLASS()
class MYPROJECTDEMO1_API UActionCheckBoxUI : public UActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UCheckBox* CheckBox;
	
	/** 复选框大小缩放因子 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Checkbox", meta=(
		ExposeOnSpawn=true, 
		ClampMin="0.1", 
		ClampMax="5.0", 
		UIMin="0.1", 
		UIMax="5.0"
	))
	float CheckBoxScale = 1.5f;
	
	/** 切换复选框状态（选中/未选中） */
	UFUNCTION(BlueprintCallable, Category="UI|Checkbox")
	void ToggleCheckBox();
	
	/** 设置复选框状态 */
	UFUNCTION(BlueprintCallable, Category="UI|Checkbox")
	void SetIsChecked(bool bIsChecked);
	
	/** 获取复选框是否选中 */
	UFUNCTION(BlueprintPure, Category="UI|Checkbox")
	bool IsChecked() const;
	
	virtual void NativePreConstruct() override;

private:
	/** 缩放复选框图像 */
	void ScaleCheckBoxImage(FSlateBrush& Brush, const FVector2D& NewSize);
};
