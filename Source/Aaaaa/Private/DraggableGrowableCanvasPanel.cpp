// Fill out your copyright notice in the Description page of Project Settings.

#include "DraggableGrowableCanvasPanel.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetLayoutLibrary.h"

UDraggableGrowableCanvasPanelWidget::UDraggableGrowableCanvasPanelWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UDraggableGrowableCanvasPanelWidget::Initialize()
{
	Super::Initialize();
	DraggableGrowableCanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("DraggableGrowableCanvasPanel"))));
	return true;
}

void UDraggableGrowableCanvasPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

FReply UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) //是不是左键按下
	{
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonDown1"));
		bIsPressing = true;
		PressStartLocation = InMouseEvent.GetScreenSpacePosition();
		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(DraggableGrowableCanvasPanel->Slot);
		if (CanvasPanelSlot)
		{
			CanvasPanelStartOffsets = CanvasPanelSlot->GetOffsets();//储存按下开始的偏移
			UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonDown2 Left = %f, Right = %f, Top = %f, Bottom = %f"), CanvasPanelStartOffsets.Left, CanvasPanelStartOffsets.Right, CanvasPanelStartOffsets.Top, CanvasPanelStartOffsets.Bottom);
			//CanvasPanelStartLocation = CanvasPanelSlot->GetPosition();
		}
		return FReply::Handled().CaptureMouse(TakeWidget());//捕获鼠标输入
	}

	return FReply::Unhandled();
}

FReply UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonUp1 bIsDragging = %b, bIsPressing = %b"), bIsDragging, bIsPressing);
		if (bIsDragging) {
			bIsDragging = false;
		}
		bIsPressing = false;
		return FReply::Handled().ReleaseMouseCapture();//释放鼠标输入
	}

	return FReply::Unhandled();
}

FReply UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsPressing)
	{
		bIsDragging = true;
		FVector2D Delta = InMouseEvent.GetScreenSpacePosition() - PressStartLocation;
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove1 Delta x= %f, y= %f"), Delta.X, Delta.Y);
		//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove1.2 Left = %f, Right = %f, Top = %f, Bottom = %f"), CanvasPanelStartOffsets.Left, CanvasPanelStartOffsets.Right, CanvasPanelStartOffsets.Top, CanvasPanelStartOffsets.Bottom);
		float Left = CanvasPanelStartOffsets.Left + Delta.X, 
			Top = CanvasPanelStartOffsets.Top + Delta.Y, 
			Right = CanvasPanelStartOffsets.Right - Delta.X, 
			Bottom =CanvasPanelStartOffsets.Bottom - Delta.Y;

		//限制不能拖出框
		//if (Left > 0.f) Left = 0;
		//if (Top > 0.f) Top = 0;
		//if (Right > 0.f) Right = 0;
		//if (Bottom > 0.f) Bottom = 0;

		//限制缩放下不能拖出框
		FVector2D RenderTransformScale = DraggableGrowableCanvasPanel->RenderTransform.Scale;//获取CanvasPanel的缩放
		float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);//获取视口缩放
		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize (this);//获取视口大小
		FVector2D CanvasPanelSize = ViewportSize / ViewportScale;//视口大小/视口缩放=实际ui界面的大小
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove2 ViewportSize.x = %f, ViewportSize.y = %f, ViewportScale = %f"), ViewportSize.X, ViewportSize.Y, ViewportScale);
		float CanvasPanelSizeX = CanvasPanelSize.X - Left - Right;
		float LeftOffset = Left - CanvasPanelSizeX * (RenderTransformScale.X - 1) / 2;//左边的实际偏移距离
		float RightOffset = Right - CanvasPanelSizeX * (RenderTransformScale.X - 1) / 2;
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove2.2 CanvasPanelSize.X = %f, Left = %f, Right = %f, RenderTransformScale.X = %f, LeftOffset = %f"), CanvasPanelSize.X, Left, Right, RenderTransformScale.X, LeftOffset);
		float CanvasPanelSizeY = CanvasPanelSize.Y - Top - Bottom;
		float TopOffset = Top - CanvasPanelSizeY * (RenderTransformScale.Y - 1) / 2;
		float BottomOffset = Bottom - CanvasPanelSizeY * (RenderTransformScale.Y - 1) / 2;
		if (LeftOffset > 0) {
			if (RenderTransformScale.X < 1.0001f) {//如果是一倍，直接赋值成0
				Left = 0;
			}
			else{
				Left = (CanvasPanelSize.X - Right) / (2 / (RenderTransformScale.X - 1) + 1);
			}
		}
		if (RightOffset > 0) {
			if (RenderTransformScale.X < 1.0001f) {
				Right = 0;
			}
			else {
				Right = (CanvasPanelSize.X - Left) / (2 / (RenderTransformScale.X - 1) + 1);
			}
		}
		if (TopOffset > 0) {
			if (RenderTransformScale.Y < 1.0001f) {
				Top = 0;
			}
			else {
				Top = (CanvasPanelSize.Y - Bottom) / (2 / (RenderTransformScale.Y - 1) + 1);
			}
		}
		if (BottomOffset > 0) {
			if (RenderTransformScale.Y < 1.0001f) {
				Bottom = 0;
			}
			else {
				Bottom = (CanvasPanelSize.Y - Top) / (2 / (RenderTransformScale.Y - 1) + 1);
			}
		}

		//设置新的偏移位置
		FMargin NewCanvasPanelOffsets(Left, Top, Right, Bottom);
		

		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(DraggableGrowableCanvasPanel->Slot);
		if (CanvasPanelSlot)
		{
			CanvasPanelSlot->SetOffsets(NewCanvasPanelOffsets);
			//CanvasPanelSlot->SetPosition();
		}
	}

	return FReply::Handled();
}