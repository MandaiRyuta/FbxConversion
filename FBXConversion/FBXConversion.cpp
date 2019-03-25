// FBXConversion.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"
#include "FileManager.h"

#pragma warning(disable : 4996)
using namespace std;

#include <iostream>
#include <fbxsdk.h>
#include <algorithm>
#include <vector>
InputModelDataFile* ifsFile;
OutPutModelDataFile* opmFile;
StaticMeshModelInfomation setmodel;
const char* filePath = "stage2_1.fbx";

struct COLOR_RGBA
{
	float r;
	float g;
	float b;
	float a;
};

COLOR_RGBA* vertexColor = nullptr;
VERTEX* vertexBuffer = nullptr;
int* indexBuffer = nullptr;

void LoadUVInformation(FbxMesh* pMesh)
{
	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);

	//iterating over all uv sets
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
	{
		//get lUVSetIndex-th uv set
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

		if (!lUVElement)
			continue;

		// only support mapping mode eByPolygonVertex and eByControlPoint
		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			return;

		//index array, where holds the index referenced to the uv data
		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

		//iterating through the data by polygon
		const int lPolyCount = pMesh->GetPolygonCount();

		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					lUVValue.Normalize();
					UV uvs;
					uvs.U = (float)lUVValue.mData[0];
					uvs.V = (float)lUVValue.mData[1];
					setmodel.uv.push_back(uvs);
					//std::cout << "U  ::" << lUVValue.mData[0] << "," << "V ::" << lUVValue.mData[1] << std::endl;
					//User TODO:
					//Print out the value of UV(lUVValue) or log it to a file

				}
			}
		}
		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
						lUVValue.Normalize();
						UV uvs;
						uvs.U = (float)lUVValue.mData[0];
						uvs.V = (float)lUVValue.mData[1];
						setmodel.uv.push_back(uvs);
						//std::cout << "U  ::" << lUVValue.mData[0] << "," << "V ::" << lUVValue.mData[1] << std::endl;
						//User TODO:
						//Print out the value of UV(lUVValue) or log it to a file
						//std::cout << lUVValue.mData[0] << lUVValue.mData[1] << std::endl;
						lPolyIndexCounter++;
					}
				}
			}
		}
	}
}

void getNormals(FbxMesh* mesh)
{
	int vColorLayerCount = mesh->GetElementVertexColorCount();

	for (int i = 0; vColorLayerCount > i; i++)
	{
		//レイヤーの数分だけ色情報を取得
		FbxGeometryElementVertexColor* color = mesh->GetElementVertexColor(i);

		//マッピングモードの取得
		FbxGeometryElement::EMappingMode mappingMode = color->GetMappingMode();

		//リファレンスモードの取得
		FbxGeometryElement::EReferenceMode referenceMode = color->GetReferenceMode();
	
		switch (mappingMode) 
		{
		case FbxGeometryElement::eByControlPoint:
			break;
		case FbxGeometryElement::eByPolygon:
			break;
		case FbxGeometryElement::eByPolygonVertex:
			switch (referenceMode)
			{
			case FbxGeometryElement::eIndexToDirect:
			{
				FbxLayerElementArrayTemplate<int>* index = &color->GetIndexArray();

				int indexCount = index->GetCount();

				vertexColor = new COLOR_RGBA[indexCount];

				for (int j = 0; indexCount > j; j++)
				{
					vertexColor[j].r = (float)color->GetDirectArray().GetAt(index->GetAt(j))[0];
					vertexColor[j].g = (float)color->GetDirectArray().GetAt(index->GetAt(j))[1];
					vertexColor[j].b = (float)color->GetDirectArray().GetAt(index->GetAt(j))[2];
					vertexColor[j].a = (float)color->GetDirectArray().GetAt(index->GetAt(j))[3];

					COLOR col;

					col.r = vertexColor[j].r;
					col.g = vertexColor[j].g;
					col.b = vertexColor[j].b;
					col.a = vertexColor[j].a;

					setmodel.color.push_back(col);
					//std::cout << "COLOR_RGBA" << vertexColor[j].r << "," << vertexColor[j].g << "," << vertexColor[j].b << "," << vertexColor[j].a <<std::endl;
				}
			}
			break;
			case FbxGeometryElement::eDirect:
				break;
			case FbxGeometryElement::eIndex:
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}	

}

void getPosition(FbxMesh* mesh)
{
	int polygonCount = mesh->GetPolygonCount();
	int vertexCount = mesh->GetControlPointsCount();
	int indexCount = mesh->GetPolygonVertexCount();
	FbxVector4* vertex = mesh->GetControlPoints();
	int* index = mesh->GetPolygonVertices();
	vertexBuffer = new VERTEX[vertexCount];

	for (int i = 0; vertexCount > i; i++)
	{
		vertexBuffer[i].x = (float)vertex[i][0];
		vertexBuffer[i].y = (float)vertex[i][1];
		vertexBuffer[i].z = (float)vertex[i][2];

		//std::cout << "Position_X" << vertexBuffer[i].x << std::endl;
		//std::cout << "Position_Y" << vertexBuffer[i].y << std::endl;
		//std::cout << "Position_Z" << vertexBuffer[i].z << std::endl;

		VERTEX vtxs;
		vtxs.x = vertexBuffer[i].x;
		vtxs.y = vertexBuffer[i].y;
		vtxs.z = vertexBuffer[i].z;

		setmodel.vtx.push_back(vtxs);
	}

	indexBuffer = new int[indexCount];

	for (int i = 0; indexCount > i; i++)
	{
		indexBuffer[i] = index[i];
		setmodel.idx.push_back(index[i]);
	}
}
void getMesh(FbxNode* node)
{
	FbxNodeAttribute* attr = node->GetNodeAttribute();

	if (nullptr != attr)
	{
		switch (attr->GetAttributeType()) 
		{
		case FbxNodeAttribute::eMesh:
		{
			FbxMesh* mesh = node->GetMesh();
			getPosition(mesh);
			getNormals(mesh);
			LoadUVInformation(mesh);
		}
		break;
		}
	}
	
	int childCount = node->GetChildCount();
	for (int i = 0; childCount > i; i++)
	{
		getMesh(node->GetChild(i));
	}
}

int main()
{
	FbxManager* manager = FbxManager::Create();
	FbxImporter* importer = FbxImporter::Create(manager,"stage2_1.fbx");
	FbxScene* scene = FbxScene::Create(manager, "");
	
	if (nullptr != manager)
	{
		importer->Initialize(filePath);

		if (nullptr != importer)
		{
			importer->Import(scene);

			if (nullptr != scene)
			{
				bool result = importer->Initialize(filePath);

				if (result) 
				{
					result = importer->Import(scene);
					std::cout << "用意できました" << std::endl;
				}
			}
		}
		importer->Destroy();

		FbxNode* rootNode = scene->GetRootNode();

		if (nullptr != rootNode)
		{
			int childCount = rootNode->GetChildCount();

			for (int i = 0; childCount > i; i++)
			{
				getMesh(rootNode->GetChild(i));
			}
		}

		manager->Destroy();
	}

	opmFile = new OutPutModelDataFile("ModelData.txt", setmodel);
	opmFile->CreateTextData();
	
	ifsFile = new InputModelDataFile("ModelData.txt");
	ifsFile->CreateModelData();

	delete[] vertexBuffer;
	delete[] indexBuffer;
	delete opmFile;
	delete ifsFile;

	return 0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
