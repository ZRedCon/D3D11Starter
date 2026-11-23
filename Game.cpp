#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "Mesh.h"
#include "BufferStructs.h"

#include <DirectXMath.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Material.h"
#include "Lights.h"
#include "Texture2D.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include <d3d11shadertracing.h>
#include <wrl/client.h>
#include <WICTextureLoader.h>

#include <unordered_map>
#include <memory>
#include <array>
#include <cmath>
#include <vector>
#include <string>
#include "Sky.h"

// The Color mode for the project: (DARKMODE / LIGHTMODE)
#define DARKMODE

// Shoud ImGui display, will also display if DEBUG is defined
#define ENABLE_IMGUI

#define ImGui_Window(windowName, block) ImGui::Begin(windowName); { block } ImGui::End();
#define MakeSkyPaths(path) { FixPath(ASSETS_PATH + path + L"right.png"), FixPath(ASSETS_PATH + path + L"left.png"), FixPath(ASSETS_PATH + path + L"up.png"), FixPath(ASSETS_PATH + path + L"down.png"), FixPath(ASSETS_PATH + path + L"front.png"), FixPath(ASSETS_PATH + path + L"back.png") }
#define RoundToMultiple(dataSize, intervalSize) (((dataSize) + (intervalSize) - 1) / (intervalSize) * (intervalSize))
#define GetAssetPath(subpath) FixPath(ASSETS_PATH + subpath).c_str()
#define DeviceAndContextGet Graphics::Device.Get(), Graphics::Context.Get()

// For the DirectX Math library
using namespace DirectX;
using namespace Microsoft::WRL;

static std::wstring ASSETS_PATH = L"../../Assets/";

bool displayDemoPanel = true;
std::unique_ptr<std::array<float, 4>> backgroundColor = std::make_unique<std::array<float, 4>>();

// Constant Buffer
BufPtr constantBuffer;
unsigned int chHeapSize = 256000;
unsigned int chHeapOffset = 0;

VertexShaderData vsData;
PixelColorBuffer psData;

// -- Temp vars for current assignment(s) --
std::vector<GameEntity> entities;
ComPtr<ID3D11SamplerState> samplerState;

static GameEntity& AddEntity(std::string nameOfMesh, std::string nameOfMaterial)
{
	entities.push_back(GameEntity(nameOfMesh + " : " + nameOfMaterial, Mesh::Get(nameOfMesh) , Material::Get(nameOfMaterial)));
	return entities.back();
}

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	backgroundColor->at(0) = 0.01f;
	backgroundColor->at(1) = 0.02f;
	backgroundColor->at(2) = 0.03f;
	backgroundColor->at(3) = 0.0f;

	psData.ambiantColor = XMFLOAT3(0.15f, 0.15f, 0.3f);

	LoadShaders();
	CreateGeometry();

	Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::Context->IASetInputLayout(inputLayout.Get());

	// ImGui init
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(Window::Handle());
		ImGui_ImplDX11_Init(DeviceAndContextGet);

		// Styles
#if defined(DARKMODE)
		ImGui::StyleColorsDark();
#elif defined(LIGHTMODE)
		ImGui::StyleColorsLight();
#endif
	}

	// Creation of constant buffer
	{
		// The doohickey
		XMStoreFloat4x4(&vsData.world, XMMatrixIdentity());

		// Actual creation and linking of constant bugger
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_DYNAMIC;
		ibd.ByteWidth = 256000;
		ibd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		Graphics::Device->CreateBuffer(&ibd, 0, 
			constantBuffer.GetAddressOf());

		Graphics::Context->VSSetConstantBuffers(
			0, 1, constantBuffer.GetAddressOf());

		Graphics::Context->PSSetConstantBuffers(
			0, 1, constantBuffer.GetAddressOf());
		
	}

	// Make the entities
	{
		std::string meshesOrder[] = { "cube", "cylinder", "helix", "sphere", "torus", "quad", "quad_double_sided" };
		std::string matOrder[] = { "cobblestone", "bronze", "floor", "paint", "rough", "scratched", "wood" };
		static const int NUM_Y = 7;
		static const int HALF_NUM = NUM_Y / 2;
		static const int GRID_SIZE = 4;
		for (int y = 0; y < NUM_Y; y++)
		{
			std::string mat = matOrder[y];
			for (int x = 0; x < NUM_Y; x++)
				AddEntity(meshesOrder[x], mat).GetTransform()->MoveAbsolute(float(x * GRID_SIZE), float(NUM_Y - y - HALF_NUM) * GRID_SIZE, 10.0f);
		}
	}
	
	// Make the cameras
	{
		Camera::Create("1", Window::AspectRatio(), XMFLOAT3(0, 0, -1));
		Camera::Create("2", Window::AspectRatio(), XMFLOAT3(1, 1, -4))->
			SetFOV( 0.7f );
	}

	// Make the light(s)
	{
		LightObject l0 = LightObject::Create("directional #1", LIGHT_TYPE_DIRECTIONAL)->
			SetDirection(1.0f, -2.0f, 0.2f)
			.SetColor(0.4f, 0.0f, 0.4f)
			.SetIntensity(2.0f);

		LightObject::Create("directional #2", l0)->
			SetDirection(4.0f, 0.0f, 2.0f)
			.SetColor(1.0f, 0.7f, 0.0f);

		LightObject::Create("directional #3", l0)->
			SetDirection(-3.0f, 0.5f, 6.0f)
			.SetColor(0.2f, 0.85f, 0.3f);

		LightObject::Create("point #1", LIGHT_TYPE_POINT)->
			SetPosition(0.0f, 2.0f, 8.0f)
			.SetColor(0.1f, 0.1f, 1.0f)
			.SetIntensity(4.0f)
			.SetRange(5.0f);

		LightObject::Create("spot #1", LIGHT_TYPE_SPOT)->
			SetDirection(0.0f, 0.0f, 1.0f)
			.SetPosition(10.0f, 2.0f, 3.0f)
			.SetColor(1.0f, 1.0f, 1.0f)
			.SetIntensity(10.0f)
			.SetRange(20.0f)
			.SetSpotAnglesDegrees(15.0f, 25.0f);

		LightObject::PlaceLightsInPS(psData);
	}
}

Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


static void FillAndBindNextConstantBuffer(const void* dataAddr, const int dataSize, D3D11_SHADER_TYPE shaderType, unsigned int registerSlot)
{
	unsigned int reservationSize = RoundToMultiple(dataSize, 256);
	
	if (chHeapOffset + reservationSize > chHeapSize) chHeapOffset = 0;

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedBuffer);

	void* uploadAddr = reinterpret_cast<void*>((UINT64)mappedBuffer.pData + chHeapOffset);
	memcpy(uploadAddr, dataAddr, dataSize);

	Graphics::Context->Unmap(constantBuffer.Get(), 0);
	
	// Calculate the binding offset and size as measured in 16-byte constants
	unsigned int firstConstant = chHeapOffset / 16;
	unsigned int numConstants = reservationSize / 16;

	switch (shaderType)
	{
	case D3D11_VERTEX_SHADER:
		Graphics::Context->VSSetConstantBuffers1(
			registerSlot,
			1,
			constantBuffer.GetAddressOf(),
			&firstConstant,
			&numConstants);
		break;
	case D3D11_PIXEL_SHADER:
		Graphics::Context->PSSetConstantBuffers1(
			registerSlot,
			1,
			constantBuffer.GetAddressOf(),
			&firstConstant,
			&numConstants);
		break;
	}

	chHeapOffset += reservationSize;
}

static ComPtr<ID3D11VertexShader> LoadVertexShader(const wchar_t* path, ComPtr<ID3DBlob>& blob)
{
	ComPtr<ID3D11VertexShader> vertexShader;
	D3DReadFileToBlob(FixPath(path).c_str(), blob.GetAddressOf());
	Graphics::Device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		0,
		vertexShader.GetAddressOf()
	);
	
	return vertexShader;
}

static ComPtr<ID3D11PixelShader> LoadPixelShader(const wchar_t* path, ComPtr<ID3DBlob>& blob)
{
	ComPtr<ID3D11PixelShader> pixelShader;
	D3DReadFileToBlob(FixPath(path).c_str(), &blob);
	Graphics::Device->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		0,
		pixelShader.GetAddressOf()
	);

	return pixelShader;
}

void Game::LoadShaders()
{
	ComPtr<ID3DBlob> vertexShaderBlob;
	ComPtr<ID3DBlob> pixelShaderBlob;
	auto vertexShader = LoadVertexShader(L"VertexShader.cso", vertexShaderBlob);
	auto pixelShader = LoadPixelShader(L"PixelShader.cso", pixelShaderBlob);

	// Input layout stuff
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[ 4 ] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;			
		inputElements[1].SemanticName = "TEXCOORD";
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[2].SemanticName = "NORMAL";
		inputElements[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		// Set up the fourth element - a tangent, which is 3 more float values
		inputElements[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[3].SemanticName = "TANGENT";
		inputElements[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			4,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());
	
	// Materials
	{
		Material::Get("cobblestone")->
			SetShaders(pixelShader, vertexShader)
			.AddSampler(0, samplerState)
			.StorePBRTextures(L"PBR/cobblestone", L".png");

		Material::Get("bronze")->
			SetShaders(pixelShader, vertexShader)
			.AddSampler(0, samplerState)
			.StorePBRTextures(L"PBR/bronze", L".png");
		
		Material::Get("floor")->
			SetShaders(pixelShader, vertexShader)
			.AddSampler(0, samplerState)
			.StorePBRTextures(L"PBR/floor", L".png");

		Material::Get("paint")->
			SetShaders(pixelShader, vertexShader)
			.AddSampler(0, samplerState)
			.StorePBRTextures(L"PBR/paint", L".png");

		Material::Get("rough")->
			SetShaders(pixelShader, vertexShader)
			.AddSampler(0, samplerState)
			.StorePBRTextures(L"PBR/rough", L".png");

		Material::Get("scratched")->
			SetShaders(pixelShader, vertexShader)
			.AddSampler(0, samplerState)
			.StorePBRTextures(L"PBR/scratched", L".png");

		Material::Get("wood")->
			SetShaders(pixelShader, vertexShader)
			.AddSampler(0, samplerState)
			.StorePBRTextures(L"PBR/wood", L".png");
	}
}

void Game::CreateGeometry()
{
	Mesh::Create("cube", GetAssetPath(L"Meshes/cube.bobj"));
	Mesh::Create("cylinder", GetAssetPath(L"Meshes/cylinder.bobj"));
	Mesh::Create("helix", GetAssetPath(L"Meshes/helix.bobj"));
	Mesh::Create("sphere", GetAssetPath(L"Meshes/sphere.bobj"));
	Mesh::Create("torus", GetAssetPath(L"Meshes/torus.bobj"));
	Mesh::Create("quad", GetAssetPath(L"Meshes/quad.bobj"));
	Mesh::Create("quad_double_sided", GetAssetPath(L"Meshes/quad_double_sided.bobj"));

	ComPtr<ID3DBlob> skyVSBlob;
	ComPtr<ID3DBlob> skyPSBlob;
	Sky::MakeSky(Mesh::Get("cube"), samplerState,
		LoadVertexShader(L"SkyVertexShader.cso", skyVSBlob),
		LoadPixelShader(L"SkyPixelShader.cso", skyPSBlob),
		MakeSkyPaths(L"Textures/Skies/Planet/"));
}

void Game::OnResize()
{
	Camera::UpdateAllCamerasProjMatrix();
}

static void ImGuiUpdate(float deltaTime)
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();

	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

	// Show the demo window
	if (displayDemoPanel)
		ImGui::ShowDemoWindow();
}

static void BuildUI(float deltaTime)
{
	ImGui::Begin("Inspector"); {
		if (ImGui::CollapsingHeader("GameEntities")) {
			ImGui::Indent();
			for (int i = 0; i < entities.size(); i++)
				entities[i].Inspector();
			ImGui::Unindent();
		}
		if (ImGui::CollapsingHeader("Cameras")) {
			ImGui::Indent();
			Camera::InspectAll();
			ImGui::Unindent();
		}
		if (ImGui::CollapsingHeader("Lighting")) {
			ImGui::Indent();
			if (ImGui::CollapsingHeader("Global Lighting Features")) {
				ImGui::Indent();
				ImGui::ColorEdit3("Background", &backgroundColor->at(0));
				ImGui::ColorEdit3("Ambiant", &psData.ambiantColor.x);
				ImGui::Unindent();
			}
			LightObject::InspectAll();
			ImGui::Unindent();
		}
	} ImGui::End();;
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	ImGuiUpdate(deltaTime);
#if defined(ENABLE_IMGUI) || defined(DEBUG)
	BuildUI( deltaTime );
#endif

	Camera::GetCurrentCamera()->Update(deltaTime);

	psData.elaspedTime = totalTime;

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), &backgroundColor->at(0));
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	vsData.view = Camera::GetCurrentCamera()->GetViewMatrix();
	vsData.projection = Camera::GetCurrentCamera()->GetProjectionMatrix();
	psData.cameraPosition = Camera::GetCurrentCamera()->GetTransform().GetPosition();

	LightObject::PlaceLightsInPS(psData);

	for (int i = 0; i < entities.size(); i++)
	{
		GameEntity& currentEntity = entities[i];

		Graphics::Context->VSSetShader(currentEntity.GetMaterial()->GetVertexShader().Get(), 0, 0);
		Graphics::Context->PSSetShader(currentEntity.GetMaterial()->GetPixelShader().Get(), 0, 0);

		vsData.world = currentEntity.GetTransform()->GetWorldMatrix();
		vsData.worldInvTranspose = currentEntity.GetTransform()->GetWorldInverseTransposeMatrix();
		FillAndBindNextConstantBuffer(&vsData, sizeof(vsData), D3D11_VERTEX_SHADER, 0);

		psData.color = currentEntity.GetMaterial()->GetTint();
		psData.uvScale = currentEntity.GetMaterial()->GetUVScale();
		psData.uvOffset = currentEntity.GetMaterial()->GetUVOffset();
		FillAndBindNextConstantBuffer(&psData, sizeof(psData), D3D11_PIXEL_SHADER, 0);

		currentEntity.GetMaterial()->BindTexturesAndSamplers();
		currentEntity.Draw();
	}

	// End of Frame
	{
		// Sky
		SkyVSData skyData = {};
		skyData.view = Camera::GetCurrentCamera()->GetViewMatrix();
		skyData.projection = Camera::GetCurrentCamera()->GetProjectionMatrix();
		FillAndBindNextConstantBuffer(&skyData, sizeof(skyData), D3D11_VERTEX_SHADER, 0);
		Sky::GetInstance()->Draw();

		// ImGui Drawing
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}