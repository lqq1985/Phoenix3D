// PX2EditRenderView_UI.cpp

#include "PX2EditRenderView_UI.hpp"
#include "PX2Edit.hpp"
#include "PX2GraphicsRoot.hpp"
#include "PX2StandardMesh.hpp"
#include "PX2VertexColor4Material.hpp"
#include "PX2Project.hpp"
#include "PX2EditEventType.hpp"
using namespace PX2;

//----------------------------------------------------------------------------
EditRenderView_UI::EditRenderView_UI():
EditRenderView(RVT_SCENEUI)
{
	_CreateGridGeometry();
	_CreateNodeCtrl();

	Edit::EditType editType = PX2_EDIT.GetEditType();
	Enable(editType == Edit::ET_UI);
}
//----------------------------------------------------------------------------
EditRenderView_UI::~EditRenderView_UI()
{
	if (mCanvas)
	{
		PX2_GR.RemoveCanvass(mCanvas);
		mCanvas = 0;
	}

	if (mCanvasCtrl)
	{
		PX2_GR.RemoveCanvass(mCanvasCtrl);
		mCanvasCtrl = 0;
	}

	if (mCanvasCtrl1)
	{
		PX2_GR.RemoveCanvass(mCanvasCtrl1);
		mCanvasCtrl1 = 0;
	}
}
//----------------------------------------------------------------------------
void EditRenderView_UI::_CreateGridGeometry()
{
	VertexFormat *vf = PX2_GR.GetVertexFormat(GraphicsRoot::VFT_PC);

	VertexBuffer *vertexBuffer = new0 VertexBuffer(5, vf->GetStride());
	mProjRangeSegment = new0 Polysegment(vf, vertexBuffer, true);
	mProjRangeSegment->SetNumSegments(4);
	mProjRangeSegment->SetMaterialInstance(VertexColor4Material::CreateUniqueInstance());
	mProjRangeSegment->Culling = Movable::CULL_NEVER;

	_UpdateProjectRange();

	mRangeNode = new0 Node();
	mRangeNode->AttachChild(mProjRangeSegment);

	mCanvasCtrl = new0 Canvas();
	mCanvasCtrl->SetPriority(-5);
	mCanvasCtrl->SetName("UIRangeSegmentCanvas");
	mCanvasCtrl->SetSizeChangeReAdjustCamera(false);
	mCanvasCtrl->AttachChild(mRangeNode);
	PX2_GR.AddCanvas(mCanvasCtrl->GetName().c_str(), mCanvasCtrl);
}
//----------------------------------------------------------------------------
void EditRenderView_UI::_CreateNodeCtrl()
{
	mUIObjectCtrl = new0 UIObjectCtrl();
	PX2_EW.ComeIn(mUIObjectCtrl);

	mUIObjectNode = new0 Node();
	mUIObjectNode->AttachChild(mUIObjectCtrl->GetCtrlsGroup());
	mUIObjectNode->Update(Time::GetTimeInSeconds(), 0.0f, true);

	mCanvasCtrl1 = new0 Canvas();
	mCanvasCtrl1->SetPriority(-10);
	mCanvasCtrl1->SetBeforeDrawClear(false,true, false);
	mCanvasCtrl1->SetName("UICtrlCanvas");
	mCanvasCtrl1->AttachChild(mUIObjectNode);
	PX2_GR.AddCanvas(mCanvasCtrl1->GetName().c_str(), mCanvasCtrl1);
}
//----------------------------------------------------------------------------
void EditRenderView_UI::_UpdateProjectRange()
{
	Project *proj = Project::GetSingletonPtr();
	if (!proj) return;

	float width = proj->GetWidth();
	float height = proj->GetHeight();

	VertexBufferAccessor vba(mProjRangeSegment);
	vba.Position<Float3>(0) = Float3(-1.0f, 0.0f, -1.0f);
	vba.Position<Float3>(1) = Float3(width, 0.0f, -1.0f);
	vba.Position<Float3>(2) = Float3(width, 0.0f, height);
	vba.Position<Float3>(3) = Float3(-1.0f, 0.0f, height);
	vba.Position<Float3>(4) = Float3(-1.0f, 0.0f, -1.0f);
	vba.Color<Float4>(0, 0) = Float4::BLACK;
	vba.Color<Float4>(0, 1) = Float4::BLACK;
	vba.Color<Float4>(0, 2) = Float4::BLACK;
	vba.Color<Float4>(0, 3) = Float4::BLACK;
	vba.Color<Float4>(0, 4) = Float4::BLACK;

	mProjRangeSegment->UpdateModelSpace(Renderable::GU_MODEL_BOUND_ONLY);
	Renderer::UpdateAll(mProjRangeSegment->GetVertexBuffer());
}
//----------------------------------------------------------------------------
void EditRenderView_UI::_AdjustCameraPercent()
{
	Project *proj = Project::GetSingletonPtr();
	if (!proj) return;

	float uiCameraPercent = proj->GetEdit_UICameraPercent();
	Sizef size = Sizef(mSize.Width*uiCameraPercent, mSize.Height*uiCameraPercent);
}
//----------------------------------------------------------------------------
void EditRenderView_UI::OnSize(const Sizef& size)
{
	mSize = size;

	Project *proj = Project::GetSingletonPtr();
	if (!proj) return;

	EditRenderView::OnSize(size);

	_AdjustCameraPercent();
}
//----------------------------------------------------------------------------
void EditRenderView_UI::OnLeftDown(const APoint &pos)
{
	EditRenderView::OnLeftDown(pos);

	_PickPos();

	if (mUIObjectCtrl)
		mUIObjectCtrl->OnLeftDown(mCanvasCtrl1, pos);
}
//----------------------------------------------------------------------------
void EditRenderView_UI::OnLeftUp(const APoint &pos)
{
	EditRenderView::OnLeftUp(pos);

	if (mUIObjectCtrl)
		mUIObjectCtrl->OnLeftUp(mCanvasCtrl1, pos);
}
//----------------------------------------------------------------------------
void EditRenderView_UI::OnMiddleDown(const APoint &pos)
{
	EditRenderView::OnMiddleDown(pos);

	_PickPos();
}
//----------------------------------------------------------------------------
void EditRenderView_UI::OnMiddleUp(const APoint &pos)
{
	EditRenderView::OnMiddleUp(pos);
}
//----------------------------------------------------------------------------
void EditRenderView_UI::OnMouseWheel(float delta)
{
	if (PX2_EDIT.IsCtrlDown)
	{
		float uiCameraPercent = PX2_PROJ.GetEdit_UICameraPercent();
		uiCameraPercent += Mathf::Sign(delta) * 0.02f;
		if (uiCameraPercent >= 0.1f && uiCameraPercent <= 10.0f)
		{
			PX2_PROJ.SetEdit_UICameraPercent(uiCameraPercent);
			_AdjustCameraPercent();
		}
	}
}
//----------------------------------------------------------------------------
void EditRenderView_UI::OnRightDown(const APoint &pos)
{
	EditRenderView::OnRightDown(pos);

	_PickPos();
}
//----------------------------------------------------------------------------
void EditRenderView_UI::OnRightUp(const APoint &pos)
{
	EditRenderView::OnRightUp(pos);
}
//----------------------------------------------------------------------------
void EditRenderView_UI::OnMotion(const APoint &pos)
{
	APoint curPos = pos;
	APoint lastPos = mLastMousePoint;
	AVector delta = curPos - lastPos;
	mLastMousePoint = curPos;

	if (delta == AVector::ZERO) return;

	if (PX2_EDIT.IsCtrlDown && (mIsMiddleDown || mIsRightDown))
	{
		AVector detalMove = delta;
		detalMove.X() *= mPixelToWorld.second;
		detalMove.Z() *= mPixelToWorld.second;

		UICanvas *uiCanvas = PX2_PROJ.GetUICanvas();

		CameraNode *cameraNode = uiCanvas->GetCameraNode();
		APoint trans = cameraNode->LocalTransform.GetTranslate();
		trans -= detalMove;

		cameraNode->LocalTransform.SetTranslate(trans);

		if (mUIObjectCtrl)
			mUIObjectCtrl->OnMotion(mIsLeftDown, mCanvasCtrl, curPos, lastPos);
	}
}
//----------------------------------------------------------------------------
void EditRenderView_UI::DoExecute(Event *event)
{
	EditRenderView::DoExecute(event);
	
	if (EditEventSpace::IsEqual(event, EditEventSpace::SetEditType))
	{
		Edit::EditType editType = PX2_EDIT.GetEditType();
		if (editType == Edit::ET_UI)
		{
			Enable(true);
			Show(true);
		}
		else
		{
			Enable(false);
			Show(false);
		}
	}
}
//----------------------------------------------------------------------------
void EditRenderView_UI::_PickPos()
{
	APoint origin;
	AVector direction;
	mCanvas->GetPickRay(mLastMousePoint.X(), mLastMousePoint.Z(), origin, direction);

	TriMesh *xzPlane = PX2_GR.GetXZPlane();

	Picker pick;
	pick.Execute(xzPlane, origin, direction, 0.0f, Mathf::MAX_REAL);

	const PickRecord &record = pick.GetClosestNonnegative();
	if (record.Intersected)
	{
		APoint pickPos = origin + direction * record.T;
		pickPos = APoint(pickPos[0], pickPos[1], pickPos[2]);

		PX2_EDIT.SetPickPos(pickPos);
	}
}
//----------------------------------------------------------------------------