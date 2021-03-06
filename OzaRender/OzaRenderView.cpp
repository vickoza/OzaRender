// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// OzaRenderView.cpp : implementation of the COzaRenderView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "OzaRender.h"
#endif

#include "OzaRenderDoc.h"
#include "OzaRenderView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COzaRenderView

IMPLEMENT_DYNCREATE(COzaRenderView, CView)

BEGIN_MESSAGE_MAP(COzaRenderView, CView)
	// Standard printing commands
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// COzaRenderView construction/destruction

COzaRenderView::COzaRenderView() noexcept
{
	// TODO: add construction code here

}

COzaRenderView::~COzaRenderView()
{
}

BOOL COzaRenderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// COzaRenderView drawing

void COzaRenderView::OnDraw(CDC* /*pDC*/)
{
	COzaRenderDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// COzaRenderView printing


void COzaRenderView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL COzaRenderView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void COzaRenderView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void COzaRenderView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void COzaRenderView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void COzaRenderView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// COzaRenderView diagnostics

#ifdef _DEBUG
void COzaRenderView::AssertValid() const
{
	CView::AssertValid();
}

void COzaRenderView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COzaRenderDoc* COzaRenderView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COzaRenderDoc)));
	return (COzaRenderDoc*)m_pDocument;
}
#endif //_DEBUG


// COzaRenderView message handlers
