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

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include <wrl/client.h>

#include <unordered_map>
#include <memory>
#include <array>
#include <cmath>
#include <vector>
#include <string>

/*
I just learned about preproccesors, 
so this is going to flood the codebase a bit
*/ 

// The Color mode for the project: (DARKMODE / LIGHTMODE)
#define DARKMODE

/*Break stupid auto comment runoff*/
// Shoud ImGui display, will also display if DEBUG is defined
#define ENABLE_IMGUI

/*Break stupid auto comment runoff*/
// Wrapper that blocks out a ImGui window creation
// Avoid using commas please I swear to god I don't want to have to fix that :sob:
#define ImGui_Window(windowName, block) ImGui::Begin(windowName); { block } ImGui::End();

#define MakeMesh(name, path) meshes[name] = std::make_shared<Mesh>(FixPath( path ).c_str());

// For the DirectX Math library
using namespace DirectX;
using namespace Microsoft::WRL;


bool displayDemoPanel = true;
std::unique_ptr<std::array<float, 4>> backgroundColor = std::make_unique<std::array<float, 4>>();
char textValue[2048] = "Hello Cruel World!";
int number = 0;

// Constant Buffer
BufPtr constantBuffer;

// -- Temp vars for current assignment(s) --
std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;

VertexShaderData vsData;

std::vector<GameEntity> entities;

std::vector<std::shared_ptr<Camera>> cameras;
int currCam = -1;

std::shared_ptr<Material> mat1;
std::shared_ptr<Material> mat2;
std::shared_ptr<Material> mat3;

// --------------------------------------------------------
// The constructor is called after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
Game::Game()
{
	// Setup cornflower blue bg color
	backgroundColor->at(0) = 0.4f;
	backgroundColor->at(1) = 0.6f;
	backgroundColor->at(2) = 0.75f;
	backgroundColor->at(3) = 0.0f;

	LoadShaders();

	CreateGeometry();

	Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Graphics::Context->IASetInputLayout(inputLayout.Get());

	//// Set initial graphics API state
	////  - These settings persist until we change them
	////  - Some of these, like the primitive topology & input layout, probably won't change
	////  - Others, like setting shaders, will need to be moved elsewhere later
	//{
	//	Graphics::Context->VSSetShader(mat1->GetVertexShader().Get(), 0, 0);
	//	Graphics::Context->PSSetShader(mat1->GetPixelShader().Get(), 0, 0);
	//}

	// ImGui init
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(Window::Handle());
		ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());

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
		vsData.colorTint = XMFLOAT4();
		XMStoreFloat4x4(&vsData.world, XMMatrixIdentity());

		// Actual creation and linking of constant bugger
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_DYNAMIC;
		ibd.ByteWidth = (sizeof(VertexShaderData) + 15) / 16 * 16;
		ibd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		Graphics::Device->CreateBuffer(&ibd, 0, 
			constantBuffer.GetAddressOf());

		Graphics::Context->VSSetConstantBuffers(
			0,
			1,
			constantBuffer.GetAddressOf());
	}

	// Make the entities
	{
		entities.push_back( GameEntity(meshes["helix"], mat1) );
		entities.push_back( GameEntity(meshes["quad_double_sided"], mat2) );
		entities.push_back( GameEntity(meshes["cube"], mat1) );
		entities.push_back( GameEntity(meshes["cube"], mat2) );
		entities.push_back( GameEntity(meshes["cube"], mat3) );

		entities[ 0 ].GetTransform()->MoveAbsolute(0, 0, 0.1f);
	}
	
	// Make the cameras
	{
		cameras.push_back( std::make_shared<Camera>(Window::AspectRatio(), XMFLOAT3(0,0,-1)) );
		cameras.push_back(std::make_shared<Camera>(Window::AspectRatio(), XMFLOAT3(1, 1, -4)));
		cameras[ 1 ]->SetFOV( 0.7f );
		currCam = 0;
	}
}

Game::~Game()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
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
// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	ComPtr<ID3DBlob> vertexShaderBlob;
	ComPtr<ID3DBlob> pixelShaderBlob;
	auto vertexShader = LoadVertexShader(L"VertexShader.cso", vertexShaderBlob);
	auto pixelShader = LoadPixelShader(L"PixelShader.cso", pixelShaderBlob);

	// Input layout stuff
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[ 3 ] = {};

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

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			3,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}

	// Material stuff
	mat1 = std::make_shared<Material>(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), vertexShader, pixelShader);
	mat2 = std::make_shared<Material>(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),vertexShader, pixelShader);
	mat3 = std::make_shared<Material>(XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), vertexShader, pixelShader);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	MakeMesh("cube", "../../Assets/Meshes/cube.bobj");
	MakeMesh("cylinder", "../../Assets/Meshes/cylinder.bobj");
	MakeMesh("helix", "../../Assets/Meshes/helix.bobj");
	MakeMesh("quad", "../../Assets/Meshes/quad.bobj");
	MakeMesh("quad_double_sided", "../../Assets/Meshes/quad_double_sided.bobj");
	MakeMesh("sphere", "../../Assets/Meshes/sphere.bobj");
	MakeMesh("torus", "../../Assets/Meshes/torus.bobj");
}

// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	if (currCam > -1)
		for (auto cam : cameras)
			cam->UpdateProjectionMatrix(Window::AspectRatio());
}

void ImGuiUpdate(float deltaTime)
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
	{
		ImGui::ShowDemoWindow();
	}
}

void GameEntityInspector(GameEntity& entity, std::string name)
{
	auto currTransform = entity.GetTransform();

	XMFLOAT3 newPos = currTransform->GetPosition();
	ImGui::DragFloat3((name + std::string(" Position")).c_str(), &newPos.x, 0.01f, -1.0f, 1.0f);
	currTransform->SetPosition(newPos);

	XMFLOAT3 newRot = currTransform->GetPitchYawRoll();
	ImGui::DragFloat3((name + std::string(" Rotation")).c_str(), &newRot.x, 0.01f, -1.0f, 1.0f);
	currTransform->SetRotation(newRot);

	XMFLOAT3 newScale = currTransform->GetScale();
	ImGui::DragFloat3((name + std::string(" Scale")).c_str(), &newScale.x, 0.01f, 0.01f, 10.0f);
	currTransform->SetScale(newScale);
}

void CameraInspector(Camera& camera, std::string name, int index)
{
	if (ImGui::CollapsingHeader((name + " Transform").c_str()))
	{
		Transform& currTransform = camera.GetTransform();

		XMFLOAT3 newPos = currTransform.GetPosition();
		ImGui::DragFloat3((name + std::string(" Position")).c_str(), &newPos.x, 0.01f, -1.0f, 1.0f);
		currTransform.SetPosition(newPos);

		XMFLOAT3 newRot = currTransform.GetPitchYawRoll();
		ImGui::DragFloat3((name + std::string(" Rotation")).c_str(), &newRot.x, 0.01f, -1.0f, 1.0f);
		currTransform.SetRotation(newRot);
	}
	
	float newFov = camera.GetFov();
	ImGui::DragFloat((name + " Fov").c_str(), &newFov, 0.01f, 0.1f, XM_2PI);
	camera.SetFOV( newFov );

	XMFLOAT2 newPlanes = camera.GetClipPlanes();
	ImGui::DragFloat2((name + " Clipping planes (near|far)").c_str(), &newPlanes.x, 0.001f, 0.01f, 10000.0f);
	if (newPlanes.x >= newPlanes.y)
		newPlanes.y = newPlanes.x + 0.001f;
	camera.SetClipPlanes( newPlanes );

	if (ImGui::Button((name + " Set as Main Camera").c_str()))
	{
		currCam = index;
	}
}

void BuildUI(float deltaTime)
{
	ImGui::Begin("Inspector"); {
		{
			if (ImGui::CollapsingHeader("Meshes")) {
				//if (ImGui::CollapsingHeader("Triangle")) {
				//	triangle->GetGUI();
				//} if (ImGui::CollapsingHeader("Diamond")) {
				//	diamond->GetGUI();
				//} if (ImGui::CollapsingHeader("Spikey")) {
				//	cube->GetGUI();
				//}
			} if (ImGui::CollapsingHeader("GameEntities")) {
				for (int i = 0; i < entities.size(); i++) {
					if (ImGui::CollapsingHeader((std::string("Entity ") + std::to_string(i) + std::string(":")).c_str())) {
						GameEntityInspector(entities[i], std::to_string(i));
					}
				}
			} if (ImGui::CollapsingHeader("Cameras")) {
				for (int i = 0; i < cameras.size(); i++) {
					if (ImGui::CollapsingHeader((std::string("Camera ") + std::to_string(i) + std::string(":")).c_str())) {
						CameraInspector(*cameras[i], std::to_string(i), i);
					}
				}
			}
		}
	} ImGui::End();;
	ImGui_Window("Global Inspector",
	{
		ImGui::ColorEdit4("Tint", &vsData.colorTint.x);
	});
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	ImGuiUpdate( deltaTime );
#if defined(ENABLE_IMGUI) || defined(DEBUG)
	BuildUI( deltaTime );
#endif

	cameras[ currCam ]->Update( deltaTime );


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


	vsData.view = cameras[currCam]->GetViewMatrix();
	vsData.projection = cameras[currCam]->GetProjectionMatrix();
	for (int i = 0; i < entities.size(); i++)
	{
		GameEntity& currentEntity = entities[i];

		vsData.colorTint = currentEntity.GetMaterial()->GetTint();
		Graphics::Context->VSSetShader(currentEntity.GetMaterial()->GetVertexShader().Get(), 0, 0);
		Graphics::Context->PSSetShader(currentEntity.GetMaterial()->GetPixelShader().Get(), 0, 0);

		vsData.world = currentEntity.GetTransform()->GetWorldMatrix();
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
		Graphics::Context->Unmap(constantBuffer.Get(), 0);
		currentEntity.Draw();
	}

	// ImGui Drawing
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
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

