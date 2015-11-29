// PX2GraphicsRoot.cpp

#include "PX2GraphicsRoot.hpp"
#include "PX2Environment.hpp"
#include "PX2StringHelp.hpp"
#include "PX2FString.hpp"
#include "PX2Renderer.hpp"
#include "PX2VertexColor4Material.hpp"
#include "PX2LightAmbientConstant.hpp"
#include "PX2LightDiffuseConstant.hpp"
#include "PX2LightSpecularConstant.hpp"
#include "PX2LightAttenuationConstant.hpp"
#include "PX2LightModelDVectorConstant.hpp"
#include "PX2LightWorldDVectorConstant.hpp"
#include "PX2MaterialManager.hpp"
#include "PX2TriMesh.hpp"
#include "PX2VertexFormat.hpp"
#include "PX2StandardMesh.hpp"
#include "PX2Time.hpp"
using namespace PX2;

const std::string GraphicsRoot::sEmptyResPath = "EmptyResPath";
const std::string GraphicsRoot::sTerResPath = "TerResPath";
//----------------------------------------------------------------------------
GraphicsRoot::GraphicsRoot ()
{
	mIsInEditor = false;
	mPlayType = PT_NONE;

	MaterialManager *mi = new0 MaterialManager();
	PX2_UNUSED(mi);

	mCurEnvirParam = new0 EnvirParam();
}
//----------------------------------------------------------------------------
GraphicsRoot::~GraphicsRoot ()
{
	MaterialManager *mi = MaterialManager::GetSingletonPtr();
	if (mi)
	{
		mi->Terminate();

		delete0(mi);
		MaterialManager::Set(0);
	}
}
//----------------------------------------------------------------------------
bool GraphicsRoot::Initlize ()
{
	InitTerm::ExecuteInitializers();

	// Camera
	Camera::DepthType dt = Camera::PM_DEPTH_ZERO_TO_ONE;
#if defined (PX2_USE_DX9)
	// DirectXʹ�����������[0,1]�ڵľ���
	dt = Camera::PM_DEPTH_ZERO_TO_ONE;
#endif

#if defined(PX2_USE_OPENGL) || defined(PX2_USE_OPENGLES)
	// OpenGLʹ�����������[-1,1]�ڵľ���
	dt = Camera::PM_DEPTH_MINUS_ONE_TO_ONE;
#endif
	Camera::SetDefaultDepthType(dt);

	PX2_MATERIALMAN.Initlize();

	// create help meshs
	VertexFormat *vf = GetVertexFormat(GraphicsRoot::VFT_PC);
	StandardMesh stdMesh(vf);

	mTriMeshXY = stdMesh.Rectangle(4, 4, 20000.0f, 20000.0f);
	mTriMeshXY->WorldBoundIsCurrent = true;
	mTriMeshXY->UpdateModelSpace(Renderable::GU_MODEL_BOUND_ONLY);
	mTriMeshXY->SetMaterialInstance(VertexColor4Material
		::CreateUniqueInstance());
	mTriMeshXY->GetMaterialInstance()->GetMaterial()->GetWireProperty(0, 0)
		->Enabled = true;
	mTriMeshXY->Update(Time::GetTimeInSeconds(), false);

	mTriMeshXZ = stdMesh.Rectangle(4, 4, 20000.0f, 20000.0f);
	mTriMeshXZ->WorldBoundIsCurrent = true;
	mTriMeshXZ->LocalTransform.SetRotate(Matrix3f().MakeEulerXYZ(
		Mathf::HALF_PI, 0.0f, 0.0f));
	mTriMeshXZ->UpdateModelSpace(Renderable::GU_MODEL_BOUND_ONLY);
	mTriMeshXZ->SetMaterialInstance(VertexColor4Material
		::CreateUniqueInstance());
	mTriMeshXZ->Update(Time::GetTimeInSeconds(), false);

	mTriMeshYZ = stdMesh.Rectangle(4, 4, 20000.0f, 20000.0f);
	mTriMeshYZ->WorldBoundIsCurrent = true;
	mTriMeshYZ->LocalTransform.SetRotate(Matrix3f().MakeEulerXYZ(0.0f,
		Mathf::HALF_PI, 0.0f));
	mTriMeshYZ->UpdateModelSpace(Renderable::GU_MODEL_BOUND_ONLY);
	mTriMeshYZ->SetMaterialInstance(VertexColor4Material
		::CreateUniqueInstance());
	mTriMeshYZ->Update(Time::GetTimeInSeconds(), false);

	return true;
}
//-----------------------------------------------------------------------------
bool GraphicsRoot::Terminate ()
{
	mCurEnvirParam = 0;

	mTriMeshXY = 0;
	mTriMeshXZ = 0;
	mTriMeshYZ = 0;

	PX2_MATERIALMAN.Terminate();

	mCanvasMap.clear();
	mCanvasVec.clear();

	mCreatedVFs.clear();

	Environment::RemoveAllDirectories();

	InitTerm::ExecuteTerminators();

	return true;
}
//----------------------------------------------------------------------------
void GraphicsRoot::SetScreenSize(const Sizef &size)
{
	mScreenSize = size;

	std::map<FString, CanvasPtr>::iterator it = mCanvasMap.begin();
	for (; it != mCanvasMap.end(); it++)
	{
		Canvas *renderStep = it->second;
		renderStep->SetScreenSize(size);
	}
}
//----------------------------------------------------------------------------
bool GraphicsRoot::AddCanvas(const char *name, Canvas *step)
{
	if (IsHasCanvas(name))
		return false;

	if ("Scene" == std::string(name)) mCanvasScene = step;
	if ("UI" == std::string(name)) mCanvasUI = step;

	mCanvasMap[name] = step;
	mCanvasVec.push_back(step);

	SortCanvass();

	return true;
}
//----------------------------------------------------------------------------
bool GraphicsRoot::IsHasCanvas(const char *name) const
{
	return mCanvasMap.find(name) != mCanvasMap.end();
}
//----------------------------------------------------------------------------
bool GraphicsRoot::RemoveCanvas(const char *name)
{
	std::map<FString, CanvasPtr>::iterator it = mCanvasMap.find(name);
	if (it != mCanvasMap.end())
	{
		std::vector<Canvas*>::iterator itVec = mCanvasVec.begin();
		for (; itVec != mCanvasVec.end();)
		{
			if (*itVec == it->second)
			{
				itVec = mCanvasVec.erase(itVec);
			}
			else
			{
				itVec++;
			}
		}

		mCanvasMap.erase(it);
		
		return true;
	}

	return false;
}
//----------------------------------------------------------------------------
void GraphicsRoot::RemoveCanvass(Canvas *step)
{
	std::vector<Canvas*>::iterator itVec = mCanvasVec.begin();
	for (; itVec != mCanvasVec.end();)
	{
		if (*itVec == step)
		{
			itVec = mCanvasVec.erase(itVec);
		}
		else
		{
			itVec++;
		}
	}

	std::map<FString, CanvasPtr>::iterator it = mCanvasMap.begin();
	for (; it != mCanvasMap.end(); it++)
	{
		if (it->second == step)
		{
			mCanvasMap.erase(it);
			return;
		}
	}
}
//----------------------------------------------------------------------------
void GraphicsRoot::SortCanvass()
{
	std::sort(mCanvasVec.begin(), mCanvasVec.end(),
		Canvas::LessThan);
}
//----------------------------------------------------------------------------
Canvas *GraphicsRoot::GetCanvas(const char *name)
{
	std::map<FString, CanvasPtr>::iterator it = mCanvasMap.find(name);

	if (it != mCanvasMap.end())
		return it->second;

	return 0;
}
//----------------------------------------------------------------------------
void GraphicsRoot::Update(double appSeconds, double elapsedSeconds)
{
	for (int i = 0; i < (int)mCanvasVec.size(); i++)
	{
		if (!mCanvasVec[i]->GetParent())
		{
			mCanvasVec[i]->Update(appSeconds, elapsedSeconds);
		}
	}
}
//----------------------------------------------------------------------------
void GraphicsRoot::ComputeVisibleSetAndEnv()
{
	for (int i = 0; i < (int)mCanvasVec.size(); i++)
	{
		mCanvasVec[i]->ClearVisibleSet();
		mCanvasVec[i]->ComputeVisibleSetAndEnv();
	}
}
//----------------------------------------------------------------------------
void GraphicsRoot::Draw()
{
	for (int i = 0; i < (int)mCanvasVec.size(); i++)
	{
		if (!mCanvasVec[i]->GetParent())
		{
			std::vector<Canvas*> childCanvas =
				mCanvasVec[i]->GetCuller().GetVisibleCanvas();
			mCanvasVec[i]->Draw();

			for (int i = 0; i < (int)childCanvas.size(); i++)
			{
				childCanvas[i]->Draw();

				if (childCanvas[i]->GetCuller().GetVisibleSet().GetNumVisible() > 0)
				{
					int aa = 0;
				}
			}
		}
	}
}
//----------------------------------------------------------------------------
VertexFormat *GraphicsRoot::GetVertexFormat(VertexFormatType type)
{
	std::map<VertexFormatType, VertexFormatPtr >::iterator it =
		mCreatedVFs.find(type);

	if (it != mCreatedVFs.end())
	{
		return it->second;
	}
	else
	{
		VertexFormat *vf = 0;
		if (VFT_PT1 == type)
		{
			vf = VertexFormat::Create(2,
				VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
				VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
		}
		else if (VFT_PT2 == type)
		{
			vf = VertexFormat::Create(3,
				VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
				VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0,
				VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 1);
		}
		else if (VFT_PC == type)
		{
			vf = VertexFormat::Create(2,
				VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
				VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT4, 0);
		}
		else if (VFT_PCT1 == type)
		{
			vf = VertexFormat::Create(3,
				VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
				VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT4, 0,
				VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
		}
		else if (VFT_PCT2 == type)
		{
			vf = VertexFormat::Create(4,
				VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
				VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT4, 0,
				VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0,
				VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 1);
		}
		else if (VFT_PNT1 == type)
		{
			vf = VertexFormat::Create(3,
				VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
				VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
				VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
		}
		else if (VFT_PCNT1 == type)
		{
			vf = VertexFormat::Create(4,
				VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
				VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT4, 0,
				VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
				VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
		}

		mCreatedVFs[type] = vf;

		return vf;
	}
}
//----------------------------------------------------------------------------
ObjectLoadFun GraphicsRoot::msUserLoadFun = 0;
//----------------------------------------------------------------------------
void GraphicsRoot::SetUserLoadFun(ObjectLoadFun userLoadFun)
{
	msUserLoadFun = userLoadFun;
}
//----------------------------------------------------------------------------
BufferLoadFun GraphicsRoot::msBufferLoadFun = 0;
//----------------------------------------------------------------------------
void GraphicsRoot::SetBufferLoadFun(BufferLoadFun bufferLoadFun)
{
	msBufferLoadFun = bufferLoadFun;
}
//----------------------------------------------------------------------------
const std::string *GraphicsRoot::GetShaderStr(const char *filename)
{
	std::map<FString, std::string>::iterator it = mShadersMap.find(filename);
	if (it != mShadersMap.end())
	{
		return &(it->second);
	}
	else
	{
		int bufferSize = 0;
		char *buffer = 0;
		PX2_GR.msBufferLoadFun(filename, bufferSize, buffer);
		if (0 != bufferSize && buffer)
		{
			std::string str(buffer, bufferSize);
			mShadersMap[filename] = str;
			return &(mShadersMap[filename]);
		}
	}

	return 0;
}
//----------------------------------------------------------------------------