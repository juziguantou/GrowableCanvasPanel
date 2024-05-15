// Fill out your copyright notice in the Description page of Project Settings.

#include "DraggableGrowableItemWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "DraggableGrowableCanvasPanel.h"
#include "CoreMinimal.h"




//FReply UDraggableGrowableItemWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	if (bIsPressing)
//	{
//		bIsDragging = true;
//		FVector2D Delta = InMouseEvent.GetScreenSpacePosition() - PressLastLocation;
//		//PressLastLocation = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
//		//if (DraggableGrowableCanvasPanel.IsValid()) {
//		//	FVector2D Scale = DraggableGrowableCanvasPanel.Get()->DraggableGrowableCanvasPanel->RenderTransform.Scale;
//		//	Delta = Delta / Scale;
//		//	FVector2D NewPosition = ThisLastLocation + Delta;
//		//	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseMove1 X = %f, Y = %f"), Scale.X, Scale.Y);
//		//	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseMove1.2 X = %f, Y = %f"), NewPosition.X, NewPosition.Y);
//		//	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseMove1.3 X = %f, Y = %f"), NewPosition.X, NewPosition.Y);
//		//	UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
//		//	if (CanvasPanelSlot) {
//		//		CanvasPanelSlot->SetPosition(NewPosition);
//		//		ThisLastLocation = NewPosition;
//		//	}
//		//}
//		FVector2D NewPosition = ThisLastLocation + Delta;
//		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseMove1.2 X = %f, Y = %f"), Delta.X, Delta.Y);
//		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseMove1.3 X = %f, Y = %f"), InMouseEvent.GetScreenSpacePosition().X, InMouseEvent.GetScreenSpacePosition().Y);
//		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseMove1.4 X = %f, Y = %f"), InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()).X, InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()).Y);
//		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
//		if (CanvasPanelSlot) {
//			CanvasPanelSlot->SetPosition(NewPosition);
//			//ThisLastLocation = NewPosition;
//		}
//
//	}
//
//	return FReply::Handled();
//}

FReply UDraggableGrowableItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//bIsPressing = true;
	//PressLastLocation = InMouseEvent.GetScreenSpacePosition();
	//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseButtonDown1 X = %f, Y = %f"), PressLastLocation.X, PressLastLocation.Y);
	//UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	//if (CanvasPanelSlot) {
	//	ThisLastLocation = CanvasPanelSlot->GetPosition();
	//	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseButtonDown1.2 X = %f, Y = %f"), ThisLastLocation.X, ThisLastLocation.Y);
	//}
	DraggableGrowableCanvasPanel->SetActiveItemWidget(this);
	return FReply::Unhandled();
}

//FReply UDraggableGrowableItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseButtonUp bIsDragging = %d, bIsPressing = %d"), bIsDragging, bIsPressing);
//	if (bIsDragging) {
//		bIsDragging = false;
//	}
//	else {
//	}
//	bIsPressing = false;
//	return FReply::Handled().ReleaseMouseCapture();
//}