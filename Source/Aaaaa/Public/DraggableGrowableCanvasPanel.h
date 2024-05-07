// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
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

private:
	bool bIsPressing = false;//是否正在按下
	bool bIsDragging = false;//是否正在拖动
	FVector2D PressStartLocation;//按下开始位置
	FMargin CanvasPanelStartOffsets;//CanvasPanel按下开始偏移
	//FVector2D CanvasPanelStartLocation;//CanvasPanel按下开始位置


	TWeakObjectPtr<UCanvasPanel> DraggableGrowableCanvasPanel;
public:

protected:


};