// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "DraggableGrowableItemWidget.generated.h"

UENUM(BlueprintType)
enum class  EDGItemOverlapType : uint8
{
	LOGIC,
	LVALUE,
	RVALUE
};

USTRUCT()
struct FOverlapBox
{
	GENERATED_USTRUCT_BODY();
	FVector2D Start;
	FVector2D End;
	TWeakObjectPtr<UCanvasPanel> Panel;
	bool IsEnable;
};

/**
 * 基础实现，还有不足。后续可以把所有子item存起来，设定好重置时直接赋值位置
 */
UCLASS(Blueprintable)
class AAAAA_API UDraggableGrowableItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;
	//virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//初始化重叠数据，除被拖动外的所有item进行
	void OverlapInit();
	//重叠检测，除被拖动外的所有item进行，DGItemWidget:被拖动的item
	bool OverlapCheck(EDGItemOverlapType Type, FVector2D& Location, UDraggableGrowableItemWidget * DGItemWidget);
	//重叠完成，除被拖动外的所有item进行
	void OverlapCompletion(EDGItemOverlapType Type, FVector2D& Location, UDraggableGrowableItemWidget* DGItemWidget);
	//被拖动到无限画布时调用，用来做从父ui移出逻辑
	UCanvasPanelSlot* DraggedToCanvas(FVector2D& OutItemLocation);
	//移除子ui
	FVector2D MoveOutChildWidget(UDraggableGrowableItemWidget* ChildWidget);
	void ResetDGItemSize();
	void AddChildDGItem(EDGItemOverlapType Type, int Idx, FOverlapBox& OverlapBox, UDraggableGrowableItemWidget * ChidDGItemWidget);
	void AddChildDGItem(EDGItemOverlapType Type, int Idx, UDraggableGrowableItemWidget * ChidDGItemWidget);
	void ReplaceChildDGItem(EDGItemOverlapType Type, int Idx, UDraggableGrowableItemWidget * DGItemWidget);

	FOverlapBox* GetOverlapBox(EDGItemOverlapType Type, int Idx);
private:
	FVector2D DGRelativePosition;//距离无限面板的相对位置

	friend class UDraggableGrowableCanvasPanelWidget;

	TWeakObjectPtr<UDraggableGrowableCanvasPanelWidget> DGCanvasPanelWidgetPtr;
	TMap< EDGItemOverlapType, TArray<FOverlapBox>> OverlapTypeToBox;
	bool IsOverlap;
	TWeakObjectPtr<UCanvasPanel> DGChidItemsCanvasPanel;
public:
	UPROPERTY(EditAnywhere, Category = "Overlap")
	EDGItemOverlapType OverlapType;//能够被什么重叠框重叠
	UPROPERTY(BlueprintReadOnly, Category = "Overlap")
	int OverlapIndex;//下标，在哪个碰撞器下面

	UPROPERTY(BlueprintReadOnly, Category = "Overlap")
	TWeakObjectPtr<UDraggableGrowableItemWidget> ParentItemWidgetPtr;

	UPROPERTY(BlueprintReadWrite, Category = "Overlap")
		FVector2D VisualSize;//可视化大小
public:
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void AddOverlapBoxByLocation(EDGItemOverlapType Type, FVector2D Start, FVector2D End, UCanvasPanel* Panel);

	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void AddOverlapBox(EDGItemOverlapType Type, UCanvasPanel* Panel);

	UFUNCTION(BlueprintCallable, Category = "Overlap")
		void UpdateOverlapBox(EDGItemOverlapType Type, int Idx);

	UFUNCTION(BlueprintCallable, Category = "Overlap")
		void EnableOverlapBox(EDGItemOverlapType Type, int Idx, bool IsEnable);

	UFUNCTION(BlueprintCallable, Category = "Overlap")
		UDraggableGrowableItemWidget* GetOverlapBoxDGItem(EDGItemOverlapType Type, int Idx);

	UFUNCTION(BlueprintCallable, Category = "Overlap")
	void SetVisualSize(FVector2D InVisualSize);

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlap")
	void OnDGOverlapEntry(EDGItemOverlapType Type, int Idx, UDraggableGrowableItemWidget * DGItemWidget, UCanvasPanel* Panel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlap")
	void OnDGOverlapLeave(EDGItemOverlapType Type, int Idx, UDraggableGrowableItemWidget * DGItemWidget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlap")
	void AddChildPerformance(EDGItemOverlapType Type, int Idx, UDraggableGrowableItemWidget * DGItemWidget, UCanvasPanel* Panel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlap")
	void DelChildPerformance(EDGItemOverlapType Type, int Idx, UDraggableGrowableItemWidget * DGItemWidget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Overlap")
		int GetReplaceChildIdx(EDGItemOverlapType Type, int Idx);
};
