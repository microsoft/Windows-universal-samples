#pragma once
#include "CameraResources.h"
#include "DeviceResources.h"
#include "BaseElement.h"
#include "MeshGeometry.h"
#include "StepTimer.h"
#include "BaseElement.h"

#define DEFAULT_TEXTURE_SIZE 128

namespace DX
{
    class BaseControl
    {
    public:
        BaseControl(
            const std::shared_ptr<DX::MeshGeometry> geometry = MeshGeometry::MakeTexturedCube());
        // Whether to use the child element to determine the initial texture dimensions and geometry scale.
        void SnapInitialSizeToRootElement();
        void Initialize();
        void Update(const DX::StepTimer& timer, Windows::UI::Input::Spatial::SpatialPointerPose^ cameraPose);
        void Render();
        void SetScale(Windows::Foundation::Numerics::float3 value);
        void SetPosition(Windows::Foundation::Numerics::float3 value);
        void SetParentTransform(Windows::Foundation::Numerics::float4x4 transform);
        void ApplyShaders();
        // Render stage overrides                
        bool IsFocused() { return m_isFocused; };
        void SetIsVisible(bool value);
        void PerformPressedAction();
        void SetRenderElement(std::shared_ptr<DX::FocusableElement> renderItem);
        void SetTextureWidth(UINT width) { m_textureWidth = width; m_calculateTextureDimensionsFromScale = false; }
        void SetTextureHeight(UINT height) { m_textureHeight = height; m_calculateTextureDimensionsFromScale = false; }

    private:
        // Calculate the initial texture dimensions and geometry scale.
        virtual void CalculateInitialSize();
        void AllocateTexture();
        void UploadGeometryAndShaders();
        void InitializeFocusPoint();
        void ApplyPixelShader();

        bool m_isInitialized;
        bool m_isFocused;
        bool m_isVisible;
        bool m_isUITextureRenderNeeded = true;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_focusPointConstantBuffer;
        _360VideoPlayback::FocusPointConstantBuffer m_focusPointConstantBufferData;
        float m_calculateTextureDimensionsFromScale = true;
        UINT m_textureWidth;
        UINT m_textureHeight;
        // Resources
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::shared_ptr<FocusableElement> m_renderItem;
        bool m_snapSizeToRootElement = false;
        std::shared_ptr<MeshGeometry> m_geometry;
        _360VideoPlayback::ModelConstantBuffer m_modelConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_modelConstantBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
        Windows::Foundation::Numerics::float3 m_scale;
        Windows::Foundation::Numerics::float4 m_rotation;
        Windows::Foundation::Numerics::float3 m_position;
        Windows::Foundation::Numerics::float4x4 m_transform;
        // Parent Transform for objects within a parent
        Windows::Foundation::Numerics::float4x4 m_parentTransform;
        // Status
        Windows::Foundation::Numerics::float2 m_relativeIntersectionPoint;
        bool IsFocusOnControl(const DirectX::XMMATRIX& transform, Windows::UI::Input::Spatial::SpatialPointerPose^ cameraPose);
        Microsoft::WRL::ComPtr<ID2D1Image> m_d2dContextTargetToRestore;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;
        Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_stateBlock;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
        Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_textureTarget;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureShaderResourceView;
    };
}
