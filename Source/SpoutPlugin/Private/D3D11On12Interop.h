// D3D 11 on 12 interop (Copyright SmodeTech)
#pragma once


#pragma warning(push)
// macro redefinition in DirectX headers from ThirdParty folder while they are already defined by <winerror.h> included 
// from "Windows/AllowWindowsPlatformTypes.h"
#pragma warning(disable: 4005)
#include <d3d11on12.h>
#pragma warning(pop)

class FD3D11On12Interop
{
public:
  // to be called in render thread
  bool createInterop(FDynamicRHI* interopDynamicRHI) // = GDynamicRHI 
  {
    this->dynamicRHI = interopDynamicRHI;
    if (!dynamicRHI)
    {
      UE_LOG(SpoutUELog, Error, TEXT("No existing RHI :-( "));
      return false;
    }

    FString RHIName = dynamicRHI->GetName();

    if (RHIName == TEXT("D3D11"))
    {
      device11 = (ID3D11Device*)dynamicRHI->RHIGetNativeDevice();
      device11->GetImmediateContext(&deviceContext11);
      UE_LOG(SpoutUELog, Display, TEXT("Graphics Device D3D11"));
      return true;
    }

    else if (RHIName == TEXT("D3D12"))
    {
      if (!createD3D11On12Device())
      {
        UE_LOG(SpoutUELog, Error, TEXT("We are on dx12 but cannot create D3D11On12"));
        return false;
      }
      UE_LOG(SpoutUELog, Display, TEXT("Graphics Device D3D11On12"));
      return true;
    }
    UE_LOG(SpoutUELog, Error, TEXT("Spout requires D3D11 or D3D12"));
    return false;
  }

  void releaseInterop()
  {
    if (dynamicRHI && dynamicRHI->GetName() == TEXT("D3D12"))
    {
      if (device11on12 != nullptr)
      {
        device11on12->Release();
        device11on12 = nullptr;
      }
      if (device11 != nullptr)
      {
        device11->Release();
        device11 = nullptr;
      }
      if (deviceContext11 != nullptr)
      {
        deviceContext11->Release();
        deviceContext11 = nullptr;
      }
    }
  }

  DXGI_FORMAT getTextureFormat(const FTexture2DRHIRef SrcTexture, unsigned int& width, unsigned int& height)
  {
    DXGI_FORMAT sourceFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    FString RHIName = dynamicRHI->GetName();
    if (RHIName == TEXT("D3D12"))
    {
      D3D12_RESOURCE_DESC desc;
      //UINT64 Width;
      //UINT Height;
      ID3D12Resource* NativeTex = (ID3D12Resource*)SrcTexture->GetNativeResource();
      desc = NativeTex->GetDesc();
      width = desc.Width;
      height = desc.Height;
      sourceFormat = desc.Format;
    }
    else // (RHIName == TEXT("D3D11"))
    {
      D3D11_TEXTURE2D_DESC desc;
      ID3D11Texture2D* NativeTex = (ID3D11Texture2D*)SrcTexture->GetNativeResource();
      NativeTex->GetDesc(&desc);
      width = desc.Width;
      height = desc.Height;
      sourceFormat = desc.Format;
    }

    if (sourceFormat == DXGI_FORMAT_B8G8R8A8_TYPELESS)
      sourceFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    return sourceFormat;
  }

  void copyToDx11Texture(const FTexture2DRHIRef Src, ID3D11Texture2D* targetTex)
  {
    FString RHIName = dynamicRHI->GetName();
    if (RHIName == TEXT("D3D12"))
    {
      ID3D11Resource* wrappedDX11SrcResource = nullptr;
      if (!wrapDX12SourceResource(Src, &wrappedDX11SrcResource))
      {
        UE_LOG(SpoutUELog, Error, TEXT("Couldn't wrap dx12 resource"));
        return;
      }
      // Get our Source Resource from d3d12 to be available for use with d3d11
      device11on12->AcquireWrappedResources(&wrappedDX11SrcResource, 1);
      // copy it using the d3d11 context into a d3d11 shared texture
      deviceContext11->CopyResource(targetTex, wrappedDX11SrcResource);
      // Release the source Resource so it can be used again with d3d12
      device11on12->ReleaseWrappedResources(&wrappedDX11SrcResource, 1);
      // submit d3d11 commands to the GPU
      deviceContext11->Flush();
    }
    else // (RHIName == TEXT("D3D11"))
    {
      ID3D11Texture2D* Source = (ID3D11Texture2D*)Src->GetNativeResource();
      deviceContext11->CopyResource(targetTex, Source);
      deviceContext11->Flush();
    }
    // SenderResource->Width, SenderResource->Height, SenderResource->Handle);
  }


  // DirectX Interface
  ID3D11Device* getDevice11() const
    {return device11;}

  ID3D11DeviceContext* getDeviceContext11() const
    {return deviceContext11;}

private:
  FDynamicRHI* dynamicRHI = nullptr;

  // DirectX Interface
  ID3D11Device* device11 = nullptr;
  ID3D11DeviceContext* deviceContext11 = nullptr;
  ID3D11On12Device* device11on12 = nullptr;

  bool createD3D11On12Device()
  {
    // Grab native d3d12 device that is used by ue4
    ID3D12Device* device12 = static_cast<ID3D12Device*>(dynamicRHI->RHIGetNativeDevice());
    UINT deviceFlags11 = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    HRESULT res = S_OK;

    // Create a d3d11 device and context using the native d3d12 device
    // note: we're not passing an existing d3d12 command queue but perhaps we should?
    // seems to work fine without it, but we might want to revisit when we know the definitive approach
    res = D3D11On12CreateDevice(
      device12,
      deviceFlags11,
      nullptr,
      0,
      nullptr,
      0,
      0,
      &device11,
      &deviceContext11,
      nullptr
    );

    if (FAILED(res))
    {
      UE_LOG(SpoutUELog, Error, TEXT("DX12: D3D11On12CreateDevice FAIL"));
      return false;
    }
    // Grab interface to the d3d11on12 device from the newly created d3d11 device
    res = device11->QueryInterface(__uuidof(ID3D11On12Device), (void**)&device11on12);

    if (FAILED(res))
    {
      UE_LOG(SpoutUELog, Error, TEXT("Init11on12: failed to query 11on12 device"));
      return false;
    }
    return true;
  }

  bool wrapDX12SourceResource(const FTexture2DRHIRef& Src, ID3D11Resource** Wrapped11Resource)
  {
    HRESULT hr = S_OK;
    // Grab reference to the d3d12 native implementation of the texture
    ID3D12Resource* SrcDX12Resource = (ID3D12Resource*)Src->GetTexture2D()->GetNativeResource();
    D3D11_RESOURCE_FLAGS rf11 = {};

    // Create a wrapped resource - or the way to access our d3d12 resource from the d3d11 device
    //note: D3D12_RESOURCE_STATE variables are: (1) the state of the d3d12 resource when we acquire it
    // (when the d3d12) pipeline is finished with it and we are ready to use it in d3d11 and (2) when
    // we are done using it in d3d11 (we release it back to d3d12) these are the states our resource 
    // will be transitioned into
    hr = device11on12->CreateWrappedResource(
      SrcDX12Resource, &rf11,
      D3D12_RESOURCE_STATE_RENDER_TARGET,
      D3D12_RESOURCE_STATE_PRESENT, __uuidof(ID3D11Resource),
      (void**)Wrapped11Resource);

    if (FAILED(hr))
    {
      UE_LOG(SpoutUELog, Error, TEXT("create_d3d12_tex: failed to create "));
      return false;
    }

    device11on12->ReleaseWrappedResources(Wrapped11Resource, 1);
    return true;
  }
};