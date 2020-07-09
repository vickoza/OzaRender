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

// OzaRender.h : main header file for the OzaRender application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// COzaRenderApp:
// See OzaRender.cpp for the implementation of this class
//

class MyRibbonCheckBox : public CMFCRibbonCheckBox
{
public:
	   BOOL SetCheck(BOOL checked)
	   {
		   // access protected member of baseclass CMFCRibbonBaseElement 
		   BOOL prevchecked = m_bIsChecked;
		   m_bIsChecked = checked;
		   return prevchecked;
	}
};

class COzaRenderApp : public CWinAppEx
{
public:
	COzaRenderApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnVulkan();
	afx_msg void OnDirectX();
	afx_msg void OnOpenGL();

private:
	enum class RenderPackage { Vulkan, DirectX, OpenGLPlus };
	RenderPackage currentRenderPackage;
	HINSTANCE hinstRenderLib;
	void loadVulkan();
	void loadDirectX();
	void loadOpenGLPlus(); //refer to OpenGL 2.0 or later should use openGL 4.0
};

extern COzaRenderApp theApp;
