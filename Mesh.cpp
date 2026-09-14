#include "Mesh.h"

Mesh::Mesh(Vertex vertices[], unsigned int numVertices,
	unsigned int indicies[], unsigned int numIndicies)
{
	// Assign sizes
	vertexBufferSize = numVertices;
	indexBufferSize = numIndicies;

	// Vertex buffer
	{
		D3D11_BUFFER_DESC vbd = {};								// First, we need to describe the buffer we want Direct3D to make on the GPU
		vbd.Usage = D3D11_USAGE_IMMUTABLE;						//  - Note that this variable is created on the stack since we only need it once
		vbd.ByteWidth = sizeof(Vertex) * numVertices;			//  - After the buffer is created, this description variable is unnecessary
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initialVertexData = {};			// Create the proper struct to hold the initial vertex data
		initialVertexData.pSysMem = vertices;					// - This is how we initially fill the buffer with data
		Graphics::Device->CreateBuffer(&vbd,					// - Essentially, we're specifying a pointer to the data to copy
			&initialVertexData, vertexBuffer.GetAddressOf());
	}

	// Index buffer
	{
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(unsigned int) * numIndicies;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initialIndexData = {};
		initialIndexData.pSysMem = indicies;
		Graphics::Device->CreateBuffer(&ibd,
			&initialIndexData, indexBuffer.GetAddressOf());
	}


}

Mesh::~Mesh()
{
	
}

BufPtr Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

int Mesh::GetVertexCount()
{
	return vertexBufferSize;
}

BufPtr Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return indexBufferSize;
}

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	Graphics::Context->DrawIndexed(
		indexBufferSize,		// The number of indices to use (we could draw a subset if we wanted)
		0,						// Offset to the first index we want to use
		0);						// Offset to add to each index when looking up vertices
}

const void Mesh::GetGUI()
{
	ImGui::Text("Triangles: %i", vertexBufferSize / 3);
	ImGui::Text("Verticies: %i", vertexBufferSize);
	ImGui::Text("Indices: %i", indexBufferSize);
}