#pragma once

#include "dx12_labs.h"

#include "win32_window.h"

class Renderer
{
public:
	Renderer(UINT width, UINT height) : width(width), height(height), title(L"DX12 renderer"), frame_index(0), rtv_descriptor_size(0)
	{
		view_port = CD3DX12_VIEWPORT(0.f, 0.f, static_cast<float>(width), static_cast<float>(height));
		scissor_rect = CD3DX12_RECT(0, 0, static_cast<UINT>(width), static_cast<UINT>(height));

		world = XMMatrixIdentity();
		eye_position = XMVECTOR({ 0.0f, 1.0f, -5.0f });
		XMVECTOR focus_position = eye_position + XMVECTOR({ sin(angle), 0.f, cos(angle) });
		XMVECTOR up_direction = XMVECTOR({ 0.0f, 1.0f, 0.0f });

		view = XMMatrixLookAtLH(eye_position, focus_position, up_direction);
		aspect_ratio = static_cast<float>(width) / static_cast<float>(height);

		projection = XMMatrixPerspectiveFovLH(60.f*XM_PI/180.f, aspect_ratio, 0.001f, 100.f);

		world_view_projection = world * view * projection;
	};
	virtual ~Renderer() {};

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();

	virtual void OnKeyDown(UINT8 key);
	virtual void OnKeyUp(UINT8 key);

	UINT GetWidth() const { return width; }
	UINT GetHeight() const { return height; }
	const WCHAR* GetTitle() const { return title.c_str(); }

protected:
	UINT width;
	UINT height;
	std::wstring title;

	static const UINT frame_number = 2;

	// Pipeline objects.
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> command_queue;
	ComPtr<IDXGISwapChain3> swap_chain;
	ComPtr<ID3D12DescriptorHeap> rtv_heap;
	ComPtr<ID3D12DescriptorHeap> cbv_heap;
	UINT rtv_descriptor_size;
	ComPtr<ID3D12Resource> render_targets[frame_number];
	ComPtr<ID3D12CommandAllocator> command_allocators[frame_number];
	ComPtr<ID3D12PipelineState> pipeline_state;
	ComPtr<ID3D12GraphicsCommandList> command_list;

	ComPtr<ID3D12RootSignature> root_signature;
	CD3DX12_VIEWPORT view_port;
	CD3DX12_RECT scissor_rect;

	// Resources
	std::vector<ColorVertex> verteces;
	ComPtr<ID3D12Resource> vertex_buffer;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;

	XMMATRIX world_view_projection;
	ComPtr<ID3D12Resource> constant_buffer;
	UINT8* constant_buffer_data_begin;

	// Synchronization objects.
	UINT frame_index;
	HANDLE fence_event;
	ComPtr<ID3D12Fence> fence;
	UINT64 fence_values[frame_number] = { 1, 1 };

	float aspect_ratio;

	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForGPU();
	void MoveToNextFrame();


	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;

	XMVECTOR eye_position;
	float angle = 0.f;

	float delta_forward = 0.f;
	float delta_rotation = 0.f;
};