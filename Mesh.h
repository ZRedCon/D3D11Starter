#pragma once
#include <array>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <cstddef>
#include <memory>
#include <iostream>

#include <fstream>
#include <vector>
#include <stdexcept>
#include <unordered_map>

#include "Graphics.h"
#include "Vertex.h"

#include "Inspectable.h"

using namespace DirectX;
using namespace Microsoft::WRL;

typedef ComPtr<ID3D11Buffer> BufPtr;


class Mesh : public Registered<Mesh>
{
private:
	// Vertex buffer
	BufPtr vertexBuffer;
	int vertexBufferSize;
	// Index Buffer
	BufPtr indexBuffer;
	int indexBufferSize;

	void CreateBuffers(Vertex vertices[], unsigned int numVertices,
		unsigned int indicies[], unsigned int numIndicies);

public:
	/// <summary>
	/// Mesh constructor that automatically creates the indicies array 
	/// for you assuming indicies to be 0->size 
	/// </summary>
	template <std::size_t N>
	Mesh(std::string _name, std::array<Vertex, N> vertices)			// Note for future Zach, as it turns out Lambdas are turing complete
		: Mesh(_name, vertices, []() {						// 1. Evil Lambda
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
	Mesh(std::string _name, std::array<Vertex, N> vertices, std::array<unsigned int, M> indicies)	// 1. Wrapper
		: Mesh(_name, vertices.data(), int(N), indicies.data(), int(M)) { }								// 2. Candy

	/// <summary>
	/// Default constructor
	/// </summary>
	Mesh(std::string _name, Vertex vertices[], unsigned int numVertices,
		unsigned int indicies[], unsigned int numIndicies);
	Mesh(std::string _name, const char* fileToLoad);
	Mesh(std::string _name, const wchar_t* fileToLoad);

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	~Mesh();
	BufPtr GetVertexBuffer();
	int GetVertexCount();
	BufPtr GetIndexBuffer();
	int GetIndexCount();
	void Draw();

	void Inspector() override;
};