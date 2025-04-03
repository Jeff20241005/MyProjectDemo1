// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCheckBoxUI.h"
#include "Components/CheckBox.h"
#include "Styling/SlateTypes.h"

void UActionCheckBoxUI::NativePreConstruct()
{
	Super::NativePreConstruct();

	// 应用复选框大小缩放
	if (CheckBox)
	{
		// 获取当前样式
		FCheckBoxStyle Style = CheckBox->GetWidgetStyle();

		// 计算缩放后的尺寸
		FVector2D NewSize(16.0f * CheckBoxScale, 16.0f * CheckBoxScale);

		// 设置基础尺寸
		//Style.SetCheckBoxSize(NewSize);

		// 缩放所有图像
		ScaleCheckBoxImage(Style.UncheckedImage, NewSize);
		ScaleCheckBoxImage(Style.UncheckedHoveredImage, NewSize);
		ScaleCheckBoxImage(Style.UncheckedPressedImage, NewSize);

		ScaleCheckBoxImage(Style.CheckedImage, NewSize);
		ScaleCheckBoxImage(Style.CheckedHoveredImage, NewSize);
		ScaleCheckBoxImage(Style.CheckedPressedImage, NewSize);

		ScaleCheckBoxImage(Style.UndeterminedImage, NewSize);
		ScaleCheckBoxImage(Style.UndeterminedHoveredImage, NewSize);
		ScaleCheckBoxImage(Style.UndeterminedPressedImage, NewSize);

		// 应用样式
		CheckBox->SetWidgetStyle(Style);

		// 设置整体渲染缩放
		CheckBox->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
		CheckBox->SetRenderScale(FVector2D(1.0f, 1.0f)); // 由于已调整图像大小，不需要额外缩放
	}
}

void UActionCheckBoxUI::ScaleCheckBoxImage(FSlateBrush& Brush, const FVector2D& NewSize)
{
	// 保持原有图像的纵横比和其他属性，只调整大小
	Brush.ImageSize = NewSize;
}

void UActionCheckBoxUI::ToggleCheckBox()
{
	if (CheckBox)
	{
		// 设置新状态
		CheckBox->SetIsChecked(!IsChecked());

		// 手动广播状态变化事件
		if (CheckBox->OnCheckStateChanged.IsBound())
		{
			CheckBox->OnCheckStateChanged.Broadcast(IsChecked());
		}
	}
}

void UActionCheckBoxUI::SetIsChecked(bool bIsChecked)
{
	if (CheckBox)
	{
		CheckBox->SetCheckedState(bIsChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}
}

bool UActionCheckBoxUI::IsChecked() const
{
	if (CheckBox)
	{
		return CheckBox->IsChecked();
	}
	return false;
}
