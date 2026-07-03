// SendEdit.cpp : implementation file
//

#include "stdafx.h"
#include "officechat.h"
#include "SendEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendEdit
extern BOOL g_bSendMsgEnter;

CSendEdit::CSendEdit()
{
}

CSendEdit::~CSendEdit()
{
}


BEGIN_MESSAGE_MAP(CSendEdit, CEdit)
	//{{AFX_MSG_MAP(CSendEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendEdit message handlers

void CSendEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == 13 || nChar == 10)//如果按了回车键
	{
		if(g_bSendMsgEnter == FALSE)
		{
			int vkctrl = GetKeyState(VK_CONTROL);//检测是否CTRL是否按下
			if(HIBYTE(vkctrl) > 0)//如果按下了
			{
				GetParent()->SendMessage(WM_COMMAND, IDC_BUTTON1, NULL);
				return ;
			}
		}
		else
		{
			int vkctrl = GetKeyState(VK_CONTROL);//检测是否CTRL是否按下
			if(HIBYTE(vkctrl) <= 0)//如果没按下
			{
				GetParent()->SendMessage(WM_COMMAND, IDC_BUTTON1, NULL);
				return ;
			}
		}
	
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
