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

	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);//获取视口缩放
	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);//获取视口大小
	CanvasPanelSize = ViewportSize / ViewportScale;//视口大小/视口缩放=实际ui界面的大小
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove2 ViewportSize.x = %f, ViewportSize.y = %f, ViewportScale = %f"), ViewportSize.X, ViewportSize.Y, ViewportScale);
}

FReply UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) //是不是左键按下
	{
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonDown1"));
		bIsPressing = true;
		PressLastLocation = InMouseEvent.GetScreenSpacePosition();
		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(DraggableGrowableCanvasPanel->Slot);
		if (CanvasPanelSlot)
		{
			CanvasPanelLastOffsets = CanvasPanelSlot->GetOffsets();//储存按下开始的偏移
			UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonDown2 Left = %f, Right = %f, Top = %f, Bottom = %f"), CanvasPanelLastOffsets.Left, CanvasPanelLastOffsets.Right, CanvasPanelLastOffsets.Top, CanvasPanelLastOffsets.Bottom);
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
		FVector2D Delta = InMouseEvent.GetScreenSpacePosition() - PressLastLocation;
		PressLastLocation = InMouseEvent.GetScreenSpacePosition();
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove1 Delta x= %f, y= %f"), Delta.X, Delta.Y);
		//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove1.2 Left = %f, Right = %f, Top = %f, Bottom = %f"), CanvasPanelStartOffsets.Left, CanvasPanelStartOffsets.Right, CanvasPanelStartOffsets.Top, CanvasPanelStartOffsets.Bottom);
		float Left = CanvasPanelLastOffsets.Left + Delta.X, 
			Top = CanvasPanelLastOffsets.Top + Delta.Y, 
			Right = CanvasPanelLastOffsets.Right - Delta.X, 
			Bottom =CanvasPanelLastOffsets.Bottom - Delta.Y;


		//限制缩放下不能拖出框
		FVector2D RenderTransformScale = DraggableGrowableCanvasPanel->RenderTransform.Scale;//获取CanvasPanel的缩放
		FMargin ActualOffset = GetActualOffset(Left, Top, Right, Bottom, RenderTransformScale);
		if (ActualOffset.Left > 0 || ActualOffset.Right > 0) {
			Left = CanvasPanelLastOffsets.Left;
			Right = CanvasPanelLastOffsets.Right;
		}
		if (ActualOffset.Top > 0 || ActualOffset.Bottom > 0) {
			Top = CanvasPanelLastOffsets.Top;
			Bottom = CanvasPanelLastOffsets.Bottom;
		}

		//设置新的偏移位置
		UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(DraggableGrowableCanvasPanel->Slot);
		if (CanvasPanelSlot)
		{
			CanvasPanelLastOffsets = FMargin(Left, Top, Right, Bottom);
			CanvasPanelSlot->SetOffsets(CanvasPanelLastOffsets);
			//CanvasPanelSlot->SetPosition();
		}
	}

	return FReply::Handled();
}

FReply UDraggableGrowableCanvasPanelWidget::NativeOnMouseWheel(const FGeometry & InGeometry, const FPointerEvent & InMouseEvent)
{
	float ScrollDelta = InMouseEvent.GetWheelDelta();
	float Offset = ScrollDelta * 0.05f;
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseWheel Offset= %f"), Offset);
	if (Offset > 0)
	{
		// 放大UI
		FVector2D Scale = DraggableGrowableCanvasPanel->RenderTransform.Scale;
		if (Scale.X + Offset <= 3) {//最大3倍
			FVector2D NewScale = { Scale.X + Offset, Scale.Y + Offset };
			DraggableGrowableCanvasPanel->SetRenderScale(NewScale);
		}
	}
	else if (Offset < 0)
	{
		// 缩小UI
		FVector2D Scale = DraggableGrowableCanvasPanel->RenderTransform.Scale;
		if (Scale.X + Offset >= 1) {//最小只能缩放到1倍
			FVector2D NewScale = { Scale.X + Offset, Scale.Y + Offset };
			UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(DraggableGrowableCanvasPanel->Slot);
			if (CanvasPanelSlot)
			{
				FMargin Offsets = CanvasPanelSlot->GetOffsets();
				UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseWheel2 NewScale.x= %f, NewScale.Y= %f, Left= %f, Top= %f, Right= %f, Bottom= %f, "), NewScale.X, NewScale.Y, Offsets.Left, Offsets.Top, Offsets.Right, Offsets.Bottom);
				FMargin ActualOffset = GetActualOffset(Offsets.Left, Offsets.Top, Offsets.Right, Offsets.Bottom, NewScale);
				UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseWheel2.2 NewScale.x= %f, NewScale.Y= %f, Left= %f, Top= %f, Right= %f, Bottom= %f, "), NewScale.X, NewScale.Y, ActualOffset.Left, ActualOffset.Top, ActualOffset.Right, ActualOffset.Bottom);
				if (ActualOffset.Left > 0) {
					float Distance = Offsets.Left + Offsets.Right;
					if (NewScale.X < 1.0001f && NewScale.X > 0.9999f) {//如果是一倍，直接赋值成0
						Offsets.Left = 0;
					}
					else {
						Offsets.Left = (CanvasPanelSize.X - Distance) * (NewScale.X - 1) / 2;;
					}
					Offsets.Right = Distance - Offsets.Left;//左边改变了，右边也要想要改变，保持相对距离不变
				}
				if (ActualOffset.Right > 0) {
					float Distance = Offsets.Left + Offsets.Right;
					if (NewScale.X < 1.0001f && NewScale.X > 0.9999f) {
						Offsets.Right = 0;
					}
					else {
						Offsets.Right = (CanvasPanelSize.X - Distance) * (NewScale.X - 1) / 2;
					}
					Offsets.Left = Distance - Offsets.Right;
				}
				if (ActualOffset.Top > 0 && NewScale.X > 0.9999f) {
					float Distance = Offsets.Top + Offsets.Bottom;
					if (NewScale.Y < 1.0001f) {
						Offsets.Top = 0;
					}
					else {
						Offsets.Top = (CanvasPanelSize.Y - Distance) * (NewScale.Y - 1) / 2;;
					}
					Offsets.Bottom = Distance - Offsets.Top;
				}
				if (ActualOffset.Bottom > 0 && NewScale.X > 0.9999f) {
					float Distance = Offsets.Top + Offsets.Bottom;
					if (NewScale.Y < 1.0001f) {
						Offsets.Bottom = 0;
					}
					else {
						Offsets.Bottom = (CanvasPanelSize.Y - Distance) * (NewScale.Y - 1) / 2;;
					}
					Offsets.Top = Distance - Offsets.Bottom;
				}
				UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseWheel2.3 NewScale.x= %f, NewScale.Y= %f, Left= %f, Top= %f, Right= %f, Bottom= %f, "), NewScale.X, NewScale.Y, Offsets.Left, Offsets.Top, Offsets.Right, Offsets.Bottom);
				DraggableGrowableCanvasPanel->SetRenderScale(NewScale);
				CanvasPanelSlot->SetOffsets(Offsets);
			}
		}
	}
	return FReply::Handled();
}

FMargin UDraggableGrowableCanvasPanelWidget::GetActualOffset(float Left, float Top, float Right, float Bottom, FVector2D RenderTransformScale)
{
	float CanvasPanelSizeX = CanvasPanelSize.X - Left - Right;
	float LeftOffset = Left - CanvasPanelSizeX * (RenderTransformScale.X - 1) / 2;//左边的实际偏移距离
	float RightOffset = Right - CanvasPanelSizeX * (RenderTransformScale.X - 1) / 2;
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove2.2 CanvasPanelSize.X = %f, Left = %f, Right = %f, RenderTransformScale.X = %f, LeftOffset = %f"), CanvasPanelSize.X, Left, Right, RenderTransformScale.X, LeftOffset);
	float CanvasPanelSizeY = CanvasPanelSize.Y - Top - Bottom;
	float TopOffset = Top - CanvasPanelSizeY * (RenderTransformScale.Y - 1) / 2;
	float BottomOffset = Bottom - CanvasPanelSizeY * (RenderTransformScale.Y - 1) / 2;
	return FMargin(LeftOffset, TopOffset, RightOffset, BottomOffset);
}
