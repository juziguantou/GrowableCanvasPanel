// Fill out your copyright notice in the Description page of Project Settings.

#include "DraggableGrowableItemWidget.h"




FReply UDraggableGrowableItemWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsDragging)
	{
		FVector2D NewPosition = InMouseEvent.GetScreenSpacePosition() - DragOffset;
		SetPositionInViewport(NewPosition, true);
	}

	return FReply::Handled();
}

FReply UDraggableGrowableItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsDragging = true;
	DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()) - GetCachedGeometry().GetAbsolutePosition();
	return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply UDraggableGrowableItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsDragging = false;
	return FReply::Handled().ReleaseMouseCapture();
}