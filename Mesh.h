#pragma once
#include <array>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <cstddef>
#include <memory>
#include <iostream>

#include "Graphics.h"
#include "Vertex.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

using namespace Microsoft::WRL;

typedef ComPtr<ID3D11Buffer> BufPtr;


class Mesh
{
private:
	// Vertex buffer
	BufPtr vertexBuffer;
	int vertexBufferSize;
	// Index Buffer
	BufPtr indexBuffer;
	int indexBufferSize;

public:
	/// <summary>
	/// Mesh constructor that automatically creates the indicies array 
	/// for you assuming indicies to be 0->size 
	/// </summary>
	template <std::size_t N>
	Mesh(std::array<Vertex, N> vertices)			// Note for future Zach, as it turns out Lambdas are turing complete
		: Mesh(vertices, []() {						// 1. Evil Lambda
			std::array<unsigned int, N> indicies;	// 2. Create second array now
			for (int i = 0; i < N; i++)				// 3. increment since for some reason c++ with all it's bloat has 
			{										//    no iterate() function
				indicies[i] = i;					//
			}										//
			return indicies;						// 4. Why the hell do lambdas work like this
		}()) { }									// 5. Hiroglyphs of "google help where error"

	/// <summary>
	/// Mesh constructor that automatically figures out the sizes of the two arrays
	/// </summary>
	template <std::size_t N, std::size_t M>
	Mesh(std::array<Vertex, N> vertices, std::array<unsigned int, M> indicies)	// 1. Wrapper
		: Mesh(vertices.data(), int(N), indicies.data(), int(M)) { }			// 2. Candy

	/// <summary>
	/// Default constructor
	/// </summary>
	Mesh(Vertex vertices[], unsigned int numVertices,		
		unsigned int indicies[], unsigned int numIndicies);

	~Mesh();
	BufPtr GetVertexBuffer();
	int GetVertexCount();
	BufPtr GetIndexBuffer();
	int GetIndexCount();
	void Draw();
	const void GetGUI();
};