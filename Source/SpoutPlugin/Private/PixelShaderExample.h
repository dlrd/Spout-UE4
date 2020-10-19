// Copyright 2016-2020 Cadic AB. All Rights Reserved.
// @Author	Fredrik Lindh [Temaran] (temaran@gmail.com) {https://github.com/Temaran}
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"


// This struct contains all the data we need to pass from the game thread to draw our effect.
struct FShaderUsageExampleParameters
{
	FTexture2DRHIRef InputTexture;
	UTextureRenderTarget2D* RenderTarget;
	FColor StartColor;
	FColor EndColor;
	float SimulationState;
	float ComputeShaderBlend;
	int FrameNumber;
	FMatrix ProjectionMatrix;
	FMatrix ViewMatrix;

	FIntPoint GetRenderTargetSize() const
	{
		return CachedRenderTargetSize;
	}

	FShaderUsageExampleParameters() { }
	FShaderUsageExampleParameters(UTextureRenderTarget2D* InRenderTarget)
		: RenderTarget(InRenderTarget)
		, StartColor(FColor::White)
		, EndColor(FColor::White)
		, SimulationState(1.0f)
	{
		CachedRenderTargetSize = RenderTarget ? FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY) : FIntPoint::ZeroValue;
	}

private:
	FIntPoint CachedRenderTargetSize;
};

/**************************************************************************************/
/* This is just an interface we use to keep all the pixel shading code in one file.   */
/**************************************************************************************/
class FPixelShaderExample
{
public:
	static void DrawToRenderTarget_RenderThread(FRHICommandListImmediate& RHICmdList, const FShaderUsageExampleParameters& DrawParameters);// , FTextureRHIRef ComputeShaderOutput);
};
