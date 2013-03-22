//#include "BillboardData.h"
//
//#include "Vertex.h"
//
//BillboardData::BillboardData() : zVertex(), zSRV(NULL) 
//{
//
//}
///*BillboardData::BillboardData(const VertexBillboard1& vertex, ID3D11ShaderResourceView* srv) : zVertex(new VertexBillboard1(vertex)), zSRV(srv)
//{
//
//}*/
//BillboardData::BillboardData(const VertexBillboard1* vertex, ID3D11ShaderResourceView* srv) : zVertex(new VertexBillboard1(vertex)), zSRV(srv)
//{
//
//}
//
//BillboardData::~BillboardData()
//{
//	if(this->zVertex)
//	{
//		delete this->zVertex;
//		this->zVertex = NULL;
//	}
//}