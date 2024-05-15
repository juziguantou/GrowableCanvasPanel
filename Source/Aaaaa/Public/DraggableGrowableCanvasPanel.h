// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "DraggableGrowableItemWidget.h"
#include "DraggableGrowableCanvasPanel.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class AAAAA_API UDraggableGrowableCanvasPanelWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UDraggableGrowableCanvasPanelWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	bool AddDraggableGrowableItemToCanvas(TSubclassOf<UDraggableGrowableItemWidget>  DraggableGrowableItem, FVector2D LocalPosition);
	void SetActiveItemWidget(TWeakObjectPtr<UDraggableGrowableItemWidget> ActiveItemWidget);//设置活动item，需要在鼠标按下时设置,鼠标抬起时会自动清除
private:
	FMargin GetActualOffset(float Left, float Top, float Right, float Bottom, FVector2D RenderTransformScale);
	void ResizeCanvasPanel(FVector2D Location);//扩容
private:
	bool bIsPressing = false;//是否正在按下
	bool bIsDragging = false;//是否正在拖动
	FVector2D PressLastLocation;//按下上一次位置
	FMargin CanvasPanelLastOffsets;//CanvasPanel按下开始偏移
	FVector2D ActiveWidgetLocation;//ActiveItem按下开始位置
	FVector2D CanvasPanelSize;//实际ui界面的大小

	TWeakObjectPtr<UDraggableGrowableItemWidget> ActiveItemWidget;//正在活动的item
	bool IsActiveItem;//是否有活动的item

	TArray<TWeakObjectPtr<UDraggableGrowableItemWidget>> ItemWidgets;

public:
	TWeakObjectPtr<UCanvasPanel> DraggableGrowableCanvasPanel;

protected:


};
