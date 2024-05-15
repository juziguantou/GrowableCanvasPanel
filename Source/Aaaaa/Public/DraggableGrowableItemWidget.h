// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DraggableGrowableItemWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AAAAA_API UDraggableGrowableItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	bool bIsPressing = false;//是否正在按下
	bool bIsDragging = false;//是否正在拖动
	FVector2D PressLastLocation;//按下上一次位置
	FVector2D ThisLastLocation;

	friend class UDraggableGrowableCanvasPanelWidget;

	TWeakObjectPtr<UDraggableGrowableCanvasPanelWidget> DraggableGrowableCanvasPanel;
};
