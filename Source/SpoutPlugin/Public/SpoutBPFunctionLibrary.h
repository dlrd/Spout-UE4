#pragma once

#if 0 // SMODE TECH
#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h" // SMODE TECH, Fix UE 4.25
#endif
#include "SpoutDirectX.h" 
#include "SpoutFrameCount.h" // Smode Tech
#include <d3d11.h>
#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h" // SMODE TECH, Fix UE 4.25
#endif
#endif // 0

#include "SpoutBPFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class ESpoutType : uint8
{
	Sender,
	Receiver
};

UENUM(BlueprintType)
enum class ESpoutState : uint8
{
	ER,
	EnoR,
	noER,
	noEnoR
};

UENUM(BlueprintType)
enum class ESpoutSendTextureFrom : uint8
{
	GameViewport,
	TextureRenderTarget2D
};

struct ID3D11Texture2D;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
class spoutFrameCount;
typedef void* HANDLE;

enum DXGI_FORMAT;
typedef enum DXGI_FORMAT DXGI_FORMAT;

USTRUCT(BlueprintType)
struct FSenderStruct
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spout Struct")
		FName sName;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spout Struct")
		bool bIsAlive;

		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spout Struct")
		ESpoutType spoutType;

		//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spout Struct")
		HANDLE sHandle;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spout Struct")
		int32 w;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spout Struct")
		int32 h;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spout Struct")
		int32 SenderID;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spout Struct")
		UTexture2D* TextureColor;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spout Struct")
		UMaterialInstanceDynamic* MaterialInstanceColor;

		// Pointer to our Texture's resource
		/*FTexture2DResource Smode Tech for Unreal 4.26*/FTextureResource* Texture2DResource;

		// Regions we need to update
		FUpdateTextureRegion2D* UpdateRegions;

		//Sender
		ID3D11Texture2D *activeTextures;

		ID3D11Resource * sharedResource;
		ID3D11ShaderResourceView * rView;
		ID3D11Texture2D* texTemp;
		spoutFrameCount* frame; // Smode Tech frame counter

	void SetName(FName NewsName)
	{
		sName = NewsName;
	}
	void SetSenderID(int32 NewSenderID)
	{
		SenderID = NewSenderID;
	}
	void SetHandle(HANDLE NewsHandle)
	{
		sHandle = NewsHandle;
	}
	void SetW(int32 NewW)
	{
		w = NewW;
	}
	void SetH(int32 NewH)
	{
		h = NewH;
	}

	FSenderStruct(){

	}

};


UCLASS(ClassGroup = Spout, Blueprintable)
class SPOUTPLUGIN_API USpoutBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static bool CreateRegisterSender(FName spoutName, unsigned int width, unsigned int height, DXGI_FORMAT format);

	UFUNCTION(BlueprintCallable, Category = "Spout", meta = (AdvancedDisplay = "2"))
		static bool SpoutSender(FName spoutName, ESpoutSendTextureFrom sendTextureFrom, UTextureRenderTarget2D* textureRenderTarget2D, float targetGamma = 2.2);

	UFUNCTION(BlueprintCallable, Category = "Spout")
		static void CloseSender(FName spoutName);

	UFUNCTION(BlueprintCallable, Category = "Spout")
		static bool SpoutReceiver(const FName spoutName, UMaterialInstanceDynamic*& mat, UTexture2D*& texture);

	UFUNCTION(BlueprintCallable, Category = "Spout")
   	static bool SpoutReceiverWithFallback(const FName spoutName, UMaterialInstanceDynamic* fallbackMat, UTexture2D* fallbackTexture, UMaterialInstanceDynamic*& mat, UTexture2D*& texture);
	
	UFUNCTION(BlueprintCallable, Category = "Spout")
		static bool SpoutInfo(TArray<FSenderStruct>& Senders);
	
	UFUNCTION(BlueprintCallable, Category = "Spout")
		static bool SpoutInfoFrom(FName spoutName, FSenderStruct& SenderStruct);

	UFUNCTION(BlueprintCallable, Category = "Spout")
		static int32 SetMaxSenders(int32 max);
	
	UFUNCTION(BlueprintCallable, Category = "Spout")
		static void GetMaxSenders(int32& max);

	UFUNCTION(BlueprintCallable, Category = "Spout")
		static UTextureRenderTarget2D* CreateTextureRenderTarget2D(int32 w=1024, int32 h=768, EPixelFormat pixelFormat= EPixelFormat::PF_B8G8R8A8, bool forceLinearGamma = true );

	static bool UpdateRegisteredSpout(FName spoutName, unsigned int width, unsigned int height, DXGI_FORMAT format);
};