// Fill out your copyright notice in the Description page of Project Settings.

#include "DraggableGrowableItemWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "DraggableGrowableCanvasPanel.h"
#include "CoreMinimal.h"



bool UDraggableGrowableItemWidget::Initialize()
{
	Super::Initialize();
	DGChidItemsCanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(FName(TEXT("DGChidItemsCanvasPanel"))));
	return true;
}

FReply UDraggableGrowableItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseButtonDown3 %p"), this);
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseButtonDown4 %p"), Slot->Parent);
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseButtonDown X = %s, Y = %s"), *DGCanvasPanelWidgetPtr->DraggableGrowableCanvasPanel->GetPathName(), *GetParent()->GetParent()->GetPathName());
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseButtonDown2 X = %p, Y = %p"), DGCanvasPanelWidgetPtr->DraggableGrowableCanvasPanel.Get(), GetParent()->GetParent());
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::NativeOnMouseButtonDown3 %p"), this);
	DGCanvasPanelWidgetPtr->SetActiveItemWidget(this);
	return FReply::Unhandled();
}

void UDraggableGrowableItemWidget::AddOverlapBoxByLocation(EDGItemOverlapType Type, FVector2D Start, FVector2D End, UCanvasPanel * Panel)
{
	auto Arrar = OverlapTypeToBox.Find(Type);
	if (Arrar == nullptr) {
		OverlapTypeToBox.Add(Type, TArray<FOverlapBox>({ {Start, End, Panel, true} }));
	}
	else {
		Arrar->Add({ Start, End, Panel, true});
	}
}

void UDraggableGrowableItemWidget::AddOverlapBox(EDGItemOverlapType Type, UCanvasPanel * Panel)
{
	UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Panel->Slot);
	FVector2D Start = CanvasPanelSlot->GetPosition();
	FVector2D End = Start + CanvasPanelSlot->GetSize();
	AddOverlapBoxByLocation(Type, Start, End, Panel);
}

void UDraggableGrowableItemWidget::UpdateOverlapBox(EDGItemOverlapType Type, int Idx)
{
	TArray<FOverlapBox>* OverlapBoxs = OverlapTypeToBox.Find(Type);
	if (OverlapBoxs != nullptr) {
		if (Idx >= 0 && Idx < OverlapBoxs->Num()) {
			FOverlapBox& OverlapBox = (*OverlapBoxs)[Idx];
			UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(OverlapBox.Panel->Slot);
			OverlapBox.Start = CanvasPanelSlot->GetPosition();
			OverlapBox.End = OverlapBox.Start + CanvasPanelSlot->GetSize();
		}
	}
}

void UDraggableGrowableItemWidget::EnableOverlapBox(EDGItemOverlapType Type, int Idx, bool IsEnable)
{
	TArray<FOverlapBox>* OverlapBoxs = OverlapTypeToBox.Find(Type);
	if (OverlapBoxs != nullptr) {
		if (Idx >= 0 && Idx < OverlapBoxs->Num()) {
			(*OverlapBoxs)[Idx].IsEnable = IsEnable;
		}
	}
}

UDraggableGrowableItemWidget* UDraggableGrowableItemWidget::GetOverlapBoxDGItem(EDGItemOverlapType Type, int Idx)
{
	for (size_t i = 0; i < DGChidItemsCanvasPanel->GetChildrenCount(); i++)
	{
		UDraggableGrowableItemWidget* ChidItem = Cast<UDraggableGrowableItemWidget>(DGChidItemsCanvasPanel->GetChildAt(i));
		if (ChidItem && ChidItem->OverlapType == Type && ChidItem->OverlapIndex == Idx) {
			return ChidItem;
		}
	}
	return nullptr;
}

void UDraggableGrowableItemWidget::OverlapInit()
{
	//UUserWidget* UserWidget = nullptr;
	UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	//DGRelativePosition = CanvasPanelSlot->GetPosition();
	//while (true)
	//{
	//	Slot->Parent
	//	UPanelWidget* PanelWidget = GetParent();
	//	CanvasPanelSlot = Cast<UCanvasPanelSlot>(PanelWidget->Slot);
	//}
	FVector2D ThisAbsolutePosition = GetCachedGeometry().GetAbsolutePosition();
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapInit2 X = %f, Y = %f"), ThisAbsolutePosition.X, ThisAbsolutePosition.Y);
	DGRelativePosition = DGCanvasPanelWidgetPtr->DraggableGrowableCanvasPanel->GetCachedGeometry().AbsoluteToLocal(GetCachedGeometry().GetAbsolutePosition());//GetCachedGeometry().AbsoluteToLocal(DGCanvasPanelWidgetPtr->DraggableGrowableCanvasPanel->GetCachedGeometry().GetAbsolutePosition());
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapInit3 X = %f, Y = %f"), DGRelativePosition.X, DGRelativePosition.Y);
	FVector2D DGCanvasPanelSize = DGCanvasPanelWidgetPtr->GetDGCanvasPanelSize();
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapInit3.2 X = %f, Y = %f"), DGCanvasPanelSize.X, DGCanvasPanelSize.Y);
	DGRelativePosition -= DGCanvasPanelSize / 2;
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapInit4 X = %f, Y = %f"), DGRelativePosition.X, DGRelativePosition.Y);
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapInit5 X = %f, Y = %f"), CanvasPanelSlot->GetPosition().X, CanvasPanelSlot->GetPosition().Y);

}

bool UDraggableGrowableItemWidget::OverlapCheck(EDGItemOverlapType Type, FVector2D & Location, UDraggableGrowableItemWidget * DGItemWidget)
{
	//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapCheck1 X = %f, Y = %f"), Location.X, Location.Y);
	//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapCheck2 X = %f, Y = %f"), (Location - DGRelativePosition).X, (Location - DGRelativePosition).Y);
	TArray<FOverlapBox>* OverlapBoxs = OverlapTypeToBox.Find(Type);
	if (OverlapBoxs != nullptr) {
		for (int i = 0; i < OverlapBoxs->Num(); i++)
		{
			FOverlapBox& OverlapBox = (*OverlapBoxs)[i];
			//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapCheck5 X = %f, Y = %f"), OverlapBox.Start.X, OverlapBox.Start.Y);
			//UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapCheck6 X = %f, Y = %f"), OverlapBox.End.X, OverlapBox.End.Y);
			if (OverlapBox.IsEnable && Location - DGRelativePosition > OverlapBox.Start && Location - DGRelativePosition < OverlapBox.End) {//检测是否重叠
				if (!IsOverlap) {//如果上一次没有重叠，说明是进入
					IsOverlap = true;
					DGItemWidget->OverlapIndex = i;
					OnDGOverlapEntry(Type, i, DGItemWidget, OverlapBox.Panel.Get());
					UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapCheck8"));
				}
				return true;
			}
		}
	}
	if (IsOverlap) {//如果上一次重叠了，说明是离开
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapCheck9"));
		IsOverlap = false;
		OnDGOverlapLeave(Type, DGItemWidget->OverlapIndex, DGItemWidget);
		DGItemWidget->OverlapIndex = 0;
	}
	return false;
}

void UDraggableGrowableItemWidget::OverlapCompletion(EDGItemOverlapType Type, FVector2D & Location, UDraggableGrowableItemWidget * DGItemWidget)
{
	TArray<FOverlapBox>* OverlapBoxs = OverlapTypeToBox.Find(Type);
	if (OverlapBoxs != nullptr) {
		for (int i = 0; i < OverlapBoxs->Num(); i++)
		{
			FOverlapBox& OverlapBox = (*OverlapBoxs)[i];
			if (OverlapBox.IsEnable && Location - DGRelativePosition > OverlapBox.Start && Location - DGRelativePosition < OverlapBox.End) {
				//放下后，要重置重叠状态
				IsOverlap = false;
				OnDGOverlapLeave(Type, DGItemWidget->OverlapIndex, DGItemWidget);
				UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapCompletion X = %d, Y = %d"), i, OverlapBoxs->Num());

				ReplaceChildDGItem(Type, i, DGItemWidget);
				AddChildDGItem(Type, i, OverlapBox, DGItemWidget);

				return;
			}
		}
	}
}

void UDraggableGrowableItemWidget::AddChildDGItem(EDGItemOverlapType Type, int Idx, FOverlapBox& OverlapBox, UDraggableGrowableItemWidget * ChidDGItemWidget)
{
	UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(ChidDGItemWidget->Slot);
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::OverlapCompletion2 X = %p, Y = %s"), this, *OverlapBox.Panel->GetPathName());
	TSharedPtr<SWidget> Widget = ChidDGItemWidget->GetCachedWidget();//强引用，避免引用为0后重新Construct
	UCanvasPanelSlot* Slot = DGChidItemsCanvasPanel->AddChildToCanvas(ChidDGItemWidget);
	ChidDGItemWidget->OverlapIndex = Idx;
	ChidDGItemWidget->ParentItemWidgetPtr = this;
	AddChildPerformance(Type, Idx, ChidDGItemWidget, OverlapBox.Panel.Get());
	Slot->SetPosition(Cast<UCanvasPanelSlot>(OverlapBox.Panel->Slot)->GetPosition());
	ChidDGItemWidget->ResetDGItemSize();
}


void UDraggableGrowableItemWidget::AddChildDGItem(EDGItemOverlapType Type, int Idx, UDraggableGrowableItemWidget * ChidDGItemWidget)
{
	ChidDGItemWidget->ParentItemWidgetPtr->MoveOutChildWidget(ChidDGItemWidget);
	TArray<FOverlapBox>* OverlapBoxs = OverlapTypeToBox.Find(Type);
	if (OverlapBoxs != nullptr) {
		FOverlapBox& OverlapBox = (*OverlapBoxs)[Idx];
		AddChildDGItem(Type, Idx, OverlapBox, ChidDGItemWidget);
	}
}

void UDraggableGrowableItemWidget::ReplaceChildDGItem(EDGItemOverlapType Type, int Idx, UDraggableGrowableItemWidget * DGItemWidget)
{
	int NewIdx = GetReplaceChildIdx(Type, Idx);
	if (NewIdx >= 0) {
		UDraggableGrowableItemWidget* ChidItem = GetOverlapBoxDGItem(Type, Idx);
		if (ChidItem) {
			DGItemWidget->AddChildDGItem(Type, NewIdx, ChidItem);
			return;
		}
	}
}

FOverlapBox* UDraggableGrowableItemWidget::GetOverlapBox(EDGItemOverlapType Type, int Idx)
{
	TArray<FOverlapBox>* OverlapBoxs = OverlapTypeToBox.Find(Type);
	if (OverlapBoxs != nullptr) {
		return &(*OverlapBoxs)[Idx];
	}
	return nullptr;
}

UCanvasPanelSlot* UDraggableGrowableItemWidget::DraggedToCanvas(FVector2D& OutItemLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::DraggedToCanvas"));
	//不是拖动画布子ui的需要解除父类到拖动画布中
	if (DGCanvasPanelWidgetPtr->DraggableGrowableCanvasPanel.Get() != GetParent()) {
		UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::DraggedToCanvas2 %d"), ParentItemWidgetPtr.IsValid());
		if (ParentItemWidgetPtr.IsValid()) {
			OutItemLocation = ParentItemWidgetPtr->MoveOutChildWidget(this);
			//这边暂时不需要强引用，鼠标监听会强引用ItemUI
			UCanvasPanelSlot* Slot = DGCanvasPanelWidgetPtr->DraggableGrowableCanvasPanel->AddChildToCanvas(this);
			ParentItemWidgetPtr = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("UDraggableGrowableItemWidget::DraggedToCanvas3 X = %f, Y = %f"), OutItemLocation.X, OutItemLocation.Y);
			Slot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
			ResetDGItemSize();
			return Slot;
		}

	}
	return nullptr;
}

FVector2D UDraggableGrowableItemWidget::MoveOutChildWidget(UDraggableGrowableItemWidget * ChildWidget)
{
	TArray<FOverlapBox>* OverlapBoxs = OverlapTypeToBox.Find(ChildWidget->OverlapType);
	FOverlapBox& OverlapBox = (*OverlapBoxs)[ChildWidget->OverlapIndex];
	//OverlapBox.Panel->GetChildAt(0)
	DelChildPerformance(ChildWidget->OverlapType, ChildWidget->OverlapIndex, ChildWidget);
	//返回ChildWidget移除后的位置，暂时不需要手动移出
	return DGRelativePosition + OverlapBox.Start;
}

void UDraggableGrowableItemWidget::ResetDGItemSize()
{
	UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(Slot);
	PanelSlot->SetSize(VisualSize);
}

void UDraggableGrowableItemWidget::SetVisualSize(FVector2D InVisualSize)
{
	VisualSize = InVisualSize;
	ResetDGItemSize();
}
