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
	UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(DraggableGrowableCanvasPanel->Slot);
	if (CanvasPanelSlot)
	{
		CanvasPanelSlot->SetOffsets({ -CanvasPanelSize.X, -CanvasPanelSize.Y, -CanvasPanelSize.X, -CanvasPanelSize.Y });
	}
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeConstruct ViewportSize.x = %f, ViewportSize.y = %f, ViewportScale = %f"), ViewportSize.X, ViewportSize.Y, ViewportScale);
}

FReply UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) //是不是左键按下
	{
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonDown1"));
		bIsPressing = true;
		PressLastLocation = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		if (IsActiveItem) {
			if (ActiveItemWidget.IsValid()) {
				UCanvasPanelSlot* ActiveWidgetSlot = Cast<UCanvasPanelSlot>(ActiveItemWidget->Slot);
				ActiveWidgetLocation = ActiveWidgetSlot->GetPosition();
			}
		}
		else {
			UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(DraggableGrowableCanvasPanel->Slot);
			if (CanvasPanelSlot)
			{
				CanvasPanelLastOffsets = CanvasPanelSlot->GetOffsets();//储存按下开始的偏移
				UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonDown2 Left = %f, Right = %f, Top = %f, Bottom = %f"), CanvasPanelLastOffsets.Left, CanvasPanelLastOffsets.Right, CanvasPanelLastOffsets.Top, CanvasPanelLastOffsets.Bottom);
			}
		}
		return FReply::Handled().CaptureMouse(TakeWidget());//捕获鼠标输入
	}

	return FReply::Unhandled();
}

FReply UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseButtonUp1 bIsDragging = %d, bIsPressing = %d"), bIsDragging, bIsPressing);
		if (bIsDragging) {
			bIsDragging = false;
			if (IsActiveItem) {
				if (ActiveItemWidget.IsValid()) {
					UCanvasPanelSlot* ActiveWidgetSlot = Cast<UCanvasPanelSlot>(ActiveItemWidget->Slot);
					if (ActiveWidgetSlot) {
						FVector2D LocalPosition = ActiveWidgetSlot->GetPosition();
						ResizeCanvasPanel(LocalPosition);
					}
				}
			}
		}
		else {//如果没有拖动，就视为点击
			const FGeometry& CanvasPanelGeometry = DraggableGrowableCanvasPanel->GetCachedGeometry();
			FVector2D LocalPosition = CanvasPanelGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
			AddDraggableGrowableItemToCanvas(UDraggableGrowableItemWidget::StaticClass(), LocalPosition);
		}
		ActiveItemWidget = nullptr;
		IsActiveItem = false;
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
		FVector2D Delta = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()) - PressLastLocation;
		PressLastLocation = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove1 Delta x= %f, y= %f"), Delta.X, Delta.Y);
		if (IsActiveItem) {//如果有活动的item，就移动item
			if (ActiveItemWidget.IsValid()) {
				UCanvasPanelSlot* ActiveWidgetSlot = Cast<UCanvasPanelSlot>(ActiveItemWidget->Slot);
				if (ActiveWidgetSlot) {
					FVector2D NewPosition = ActiveWidgetLocation + Delta;
					ActiveWidgetSlot->SetPosition(NewPosition);
					ActiveWidgetLocation = NewPosition;
				}
			}
		}
		else {
			//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove1.2 Left = %f, Right = %f, Top = %f, Bottom = %f"), CanvasPanelStartOffsets.Left, CanvasPanelStartOffsets.Right, CanvasPanelStartOffsets.Top, CanvasPanelStartOffsets.Bottom);
			float Left = CanvasPanelLastOffsets.Left + Delta.X,
				Top = CanvasPanelLastOffsets.Top + Delta.Y,
				Right = CanvasPanelLastOffsets.Right - Delta.X,
				Bottom = CanvasPanelLastOffsets.Bottom - Delta.Y;


			//边界处理，限制缩放下不能拖出框
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
				ESlateVisibility SlateVisibility = DraggableGrowableCanvasPanel->GetVisibility();
				UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseMove3 SlateVisibility= %d"), static_cast<uint8>(SlateVisibility));
			}
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
		if (Scale.X + Offset <= 1.f) {//最大3倍
			FVector2D NewScale = { Scale.X + Offset, Scale.Y + Offset };
			DraggableGrowableCanvasPanel->SetRenderScale(NewScale);
		}
	}
	else if (Offset < 0)
	{
		// 缩小UI
		FVector2D Scale = DraggableGrowableCanvasPanel->RenderTransform.Scale;
		if (Scale.X + Offset >= 0.5f) {//最小只能缩放到1倍
			FVector2D NewScale = { Scale.X + Offset, Scale.Y + Offset };
			UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(DraggableGrowableCanvasPanel->Slot);
			if (CanvasPanelSlot)
			{
				//边界处理
				{
					FMargin Offsets = CanvasPanelSlot->GetOffsets();
					UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseWheel2 NewScale.x= %f, NewScale.Y= %f, Left= %f, Top= %f, Right= %f, Bottom= %f, "), NewScale.X, NewScale.Y, Offsets.Left, Offsets.Top, Offsets.Right, Offsets.Bottom);
					FMargin ActualOffset = GetActualOffset(Offsets.Left, Offsets.Top, Offsets.Right, Offsets.Bottom, NewScale);
					UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseWheel2.2 NewScale.x= %f, NewScale.Y= %f, Left= %f, Top= %f, Right= %f, Bottom= %f, "), NewScale.X, NewScale.Y, ActualOffset.Left, ActualOffset.Top, ActualOffset.Right, ActualOffset.Bottom);
					if (ActualOffset.Left > 0) {
						float Distance = Offsets.Left + Offsets.Right;
						Offsets.Left = (CanvasPanelSize.X - Distance) * (NewScale.X - 1) / 2;
						Offsets.Right = Distance - Offsets.Left;//左边改变了，右边也要想要改变，保持相对距离不变
					}
					if (ActualOffset.Right > 0) {
						float Distance = Offsets.Left + Offsets.Right;
						Offsets.Right = (CanvasPanelSize.X - Distance) * (NewScale.X - 1) / 2;
						Offsets.Left = Distance - Offsets.Right;
					}
					if (ActualOffset.Top > 0) {
						float Distance = Offsets.Top + Offsets.Bottom;
						Offsets.Top = (CanvasPanelSize.Y - Distance) * (NewScale.Y - 1) / 2;;
						Offsets.Bottom = Distance - Offsets.Top;
					}
					if (ActualOffset.Bottom > 0) {
						float Distance = Offsets.Top + Offsets.Bottom;
						Offsets.Bottom = (CanvasPanelSize.Y - Distance) * (NewScale.Y - 1) / 2;
						Offsets.Top = Distance - Offsets.Bottom;
					}
					UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::NativeOnMouseWheel2.3 NewScale.x= %f, NewScale.Y= %f, Left= %f, Top= %f, Right= %f, Bottom= %f, "), NewScale.X, NewScale.Y, Offsets.Left, Offsets.Top, Offsets.Right, Offsets.Bottom);
					CanvasPanelSlot->SetOffsets(Offsets);


				}
				DraggableGrowableCanvasPanel->SetRenderScale(NewScale);
			}
		}
	}
	return FReply::Handled();
}

bool UDraggableGrowableCanvasPanelWidget::AddDraggableGrowableItemToCanvas(TSubclassOf<UDraggableGrowableItemWidget> DraggableGrowableItem, FVector2D LocalPosition)
{
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::AddDraggableGrowableItemToCanvas X = %f, Y = %f"), LocalPosition.X, LocalPosition.Y);
	// 在这里创建你的UserWidget
	FString DraggableGrowableItemClassPath = "/Game/UI/BPDraggableGrowableItem.BPDraggableGrowableItem_C";
	UClass* DraggableGrowableItemClass = LoadClass<UDraggableGrowableItemWidget>(nullptr, *DraggableGrowableItemClassPath);
	UDraggableGrowableItemWidget* ItemWidget = CreateWidget<UDraggableGrowableItemWidget>(GetWorld(), DraggableGrowableItemClass);
	if (ItemWidget)
	{
		// 添加到CanvasPanel中
		UCanvasPanelSlot* Slot = DraggableGrowableCanvasPanel->AddChildToCanvas(ItemWidget);
		if (Slot)
		{
			ItemWidgets.Add(ItemWidget);
			UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(DraggableGrowableCanvasPanel->Slot);
			if (CanvasPanelSlot)
			{
				//FMargin Offsets = CanvasPanelSlot->GetOffsets();
				//LocalPosition = FVector2D(LocalPosition.X - Offsets.Left, LocalPosition.Y - Offsets.Top);//根据CanvasPane偏移转化坐标
				//FVector2D Scale = DraggableGrowableCanvasPanel->RenderTransform.Scale;
				//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::AddDraggableGrowableItemToCanvas2.1 X = %f, Y = %f"), LocalPosition.X, LocalPosition.Y);
				//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::AddDraggableGrowableItemToCanvas2.2 X = %f, Y = %f"), Scale.X, Scale.Y);
				//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::AddDraggableGrowableItemToCanvas2.3 X = %f, Y = %f"), Offsets.Left, Offsets.Top);
				//LocalPosition = LocalPosition * Scale;
				// 设置Widget的位置
				Slot->SetPosition(LocalPosition);
				ResizeCanvasPanel(LocalPosition);
			}
		}
		ItemWidget->DraggableGrowableCanvasPanel = this;
	}
	return true;
}

void UDraggableGrowableCanvasPanelWidget::SetActiveItemWidget(TWeakObjectPtr<UDraggableGrowableItemWidget> ActiveItemWidget)
{
	this->ActiveItemWidget = ActiveItemWidget;
	IsActiveItem = true;
}

FMargin UDraggableGrowableCanvasPanelWidget::GetActualOffset(float Left, float Top, float Right, float Bottom, FVector2D RenderTransformScale)
{
	float DGCanvasPanelSizeX = CanvasPanelSize.X - Left - Right;
	float LeftOffset = Left - DGCanvasPanelSizeX * (RenderTransformScale.X - 1) / 2;//左边的实际偏移距离
	float RightOffset = Right - DGCanvasPanelSizeX * (RenderTransformScale.X - 1) / 2;
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::GetActualOffset CanvasPanelSize.X = %f, Left = %f, Right = %f, RenderTransformScale.X = %f, LeftOffset = %f"), CanvasPanelSize.X, Left, Right, RenderTransformScale.X, LeftOffset);
	float DGCanvasPanelSizeY = CanvasPanelSize.Y - Top - Bottom;
	float TopOffset = Top - DGCanvasPanelSizeY * (RenderTransformScale.Y - 1) / 2;
	float BottomOffset = Bottom - DGCanvasPanelSizeY * (RenderTransformScale.Y - 1) / 2;
	return FMargin(LeftOffset, TopOffset, RightOffset, BottomOffset);
}

//扩容面板
void UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel(FVector2D Location)
{
	UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(DraggableGrowableCanvasPanel->Slot);
	if (CanvasPanelSlot)
	{
		FMargin Offsets = CanvasPanelSlot->GetOffsets();
		FVector2D OffsetPosition{0,0};
		FVector2D Scale = DraggableGrowableCanvasPanel->RenderTransform.Scale;
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel1.1  X = %f, Y = %f, X = %f, Y = %f"), Location.X, Location.Y, Scale.X, Scale.Y);
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel1.2  Left = %f, Right = %f, Top = %f, Bottom = %f"), Offsets.Left, Offsets.Right, Offsets.Top, Offsets.Bottom);
		FMargin ActualOffset = GetActualOffset(Offsets.Left, Offsets.Top, Offsets.Right, Offsets.Bottom, Scale);
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel1.3  Left = %f, Right = %f, Top = %f, Bottom = %f"), ActualOffset.Left, ActualOffset.Right, ActualOffset.Top, ActualOffset.Bottom);
		if (Location.X < CanvasPanelSize.X) {
			UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel3.1"));
			float OffsetX = CanvasPanelSize.X - Location.X;
			float OldLeft = Offsets.Left;
			float DGCanvasPanelSizeX = CanvasPanelSize.X - Offsets.Left - Offsets.Right;//先算出无限画布的宽
			Offsets.Right = (DGCanvasPanelSizeX + OffsetX) * (Scale.X - 1) / 2 + ActualOffset.Right;//宽度增加，保持实际右偏移不变，算出缩放前右偏移
			Offsets.Left = CanvasPanelSize.X - Offsets.Right - (DGCanvasPanelSizeX + OffsetX);
			OffsetPosition.X = OldLeft - Offsets.Left;
		}
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel1.4  Left = %f, Right = %f, Top = %f, Bottom = %f"), Offsets.Left, Offsets.Right, Offsets.Top, Offsets.Bottom);
		if (Location.Y < CanvasPanelSize.Y) {
			UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel3.2"));
			float OffsetY = CanvasPanelSize.Y - Location.Y;
			float OldTop = Offsets.Top;
			float DGCanvasPanelSizeY = CanvasPanelSize.Y - Offsets.Top - Offsets.Bottom + OffsetY;
			Offsets.Bottom = DGCanvasPanelSizeY * (Scale.Y - 1) / 2 + ActualOffset.Bottom;
			Offsets.Top = CanvasPanelSize.Y - Offsets.Bottom - DGCanvasPanelSizeY;
			OffsetPosition.Y = OldTop - Offsets.Top;
		}
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel1.5  Left = %f, Right = %f, Top = %f, Bottom = %f"), Offsets.Left, Offsets.Right, Offsets.Top, Offsets.Bottom);
		if (Location.X > -Offsets.Right - Offsets.Left - CanvasPanelSize.X) {//向外扩展都是负的
			UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel3.3"));
			float OffsetX = Location.X + CanvasPanelSize.X + Offsets.Right + Offsets.Left;
			float DGCanvasPanelSizeX = CanvasPanelSize.X - Offsets.Left - Offsets.Right + OffsetX;//先算出增加后的宽
			Offsets.Left = DGCanvasPanelSizeX * (Scale.X - 1) / 2 + ActualOffset.Left;//保持实际左偏移不变，算出缩放前左偏移
			Offsets.Right = CanvasPanelSize.X - Offsets.Left - DGCanvasPanelSizeX;
		}
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel1.6  Left = %f, Right = %f, Top = %f, Bottom = %f"), Offsets.Left, Offsets.Right, Offsets.Top, Offsets.Bottom);
		if (Location.Y > - Offsets.Bottom - Offsets.Top - CanvasPanelSize.Y) {
			UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel3.4"));
			float OffsetY = Location.Y + CanvasPanelSize.Y + Offsets.Bottom + Offsets.Top;
			float DGCanvasPanelSizeY = CanvasPanelSize.Y - Offsets.Top - Offsets.Bottom + OffsetY;
			Offsets.Top = DGCanvasPanelSizeY * (Scale.Y - 1) / 2 + ActualOffset.Top;
			Offsets.Bottom = CanvasPanelSize.Y - Offsets.Top - DGCanvasPanelSizeY;
		}
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel1.7  Left = %f, Right = %f, Top = %f, Bottom = %f"), Offsets.Left, Offsets.Right, Offsets.Top, Offsets.Bottom);
		CanvasPanelSlot->SetOffsets(Offsets);
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel3.2  Left = %f, Right = %f, RenderTransformScale.X = %f, Y = %f"), Scale.X, Scale.Y, OffsetPosition.X, OffsetPosition.Y);
		OffsetPosition *= Scale;
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableCanvasPanelWidget::ResizeCanvasPanel3.3  Left = %f, Right = %f, "), OffsetPosition.X, OffsetPosition.Y);

		if (OffsetPosition.X != 0.f || OffsetPosition.Y != 0.f) {
			//所有item集体偏移
			for(auto ItemWidget : ItemWidgets)
			{
				if (ItemWidget.IsValid()) {
					UCanvasPanelSlot* ItemWidgetSlot = Cast<UCanvasPanelSlot>(ItemWidget->Slot);
					if (ItemWidgetSlot) {
						ItemWidgetSlot->SetPosition(ItemWidgetSlot->GetPosition() + OffsetPosition);
					}
				}
			}
		}
	}
}
