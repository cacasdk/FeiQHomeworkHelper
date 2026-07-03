// SetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "officechat.h"
#include "SetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetDlg dialog


CSetDlg::CSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDlg::IDD, pParent)
{
	m_nShowNameMode = 1;
	//{{AFX_DATA_INIT(CSetDlg)
	m_nMsgMode = 2;
	m_bOfficeChat = FALSE;
	m_bSendMsgEnter = -1;
	m_bSaveWndSize = TRUE;
	m_nUserChangeTip = TRUE;
	//}}AFX_DATA_INIT
}


void CSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetDlg)
	DDX_Control(pDX, IDC_LIST2, m_nameShowList);
	DDX_CBIndex(pDX, IDC_COMBO1, m_nMsgMode);
	DDX_Check(pDX, IDC_CHECK_OFFICECHAT, m_bOfficeChat);
	DDX_Radio(pDX, IDC_RADIO1, m_bSendMsgEnter);
	DDX_Check(pDX, IDC_CHECK_SAVEWNDSIZE, m_bSaveWndSize);
	DDX_Check(pDX, IDC_CHECK_USERCHANGETIP, m_nUserChangeTip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetDlg, CDialog)
	//{{AFX_MSG_MAP(CSetDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetDlg message handlers

void CSetDlg::OnButton1() 
{
	CFontDialog fontDlg(&fontSend, CF_EFFECTS | CF_SCREENFONTS,NULL,this);
	fontDlg.m_cf.rgbColors = clrSend;         //默认字体对话框颜色
	if(fontDlg.DoModal() == IDOK)
	{
		memcpy(&fontSend, fontDlg.m_cf.lpLogFont, sizeof(LOGFONT));
		clrSend = fontDlg.GetColor();
	}
}

void CSetDlg::OnButton3() 
{
	CFontDialog fontDlg(&fontRecv, CF_EFFECTS | CF_SCREENFONTS,NULL,this);
	fontDlg.m_cf.rgbColors = clrRecv;         //默认字体对话框颜色
	if(fontDlg.DoModal() == IDOK)
	{
		memcpy(&fontRecv, fontDlg.m_cf.lpLogFont, sizeof(LOGFONT));
		clrRecv = fontDlg.GetColor();
	}
}

BOOL CSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nameShowList.AddString("用户名");
	m_nameShowList.AddString("主机名");
	m_nameShowList.AddString("组名");
	m_nameShowList.AddString("IP地址");
	m_nameShowList.AddString("MAC地址");
	
	if(m_nShowNameMode & 0x1)
	{
		m_nameShowList.SetCheck(0, TRUE);
	}
	if(m_nShowNameMode & 0x10)
	{
		m_nameShowList.SetCheck(1, TRUE);
	}
	if(m_nShowNameMode & 0x100)
	{
		m_nameShowList.SetCheck(2, TRUE);
	}
	if(m_nShowNameMode & 0x1000)
	{
		m_nameShowList.SetCheck(3, TRUE);
	}
	if(m_nShowNameMode & 0x10000)
	{
		m_nameShowList.SetCheck(4, TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetDlg::OnOK() 
{
	UpdateData();
	m_nShowNameMode = 0;
	if(m_nameShowList.GetCheck(0))
	{
		m_nShowNameMode = m_nShowNameMode | 0x1;
	}
	if(m_nameShowList.GetCheck(1))
	{
		m_nShowNameMode = m_nShowNameMode | 0x10;
	}
	if(m_nameShowList.GetCheck(2))
	{
		m_nShowNameMode = m_nShowNameMode | 0x100;
	}
	if(m_nameShowList.GetCheck(3))
	{
		m_nShowNameMode = m_nShowNameMode | 0x1000;
	}
	if(m_nameShowList.GetCheck(4))
	{
		m_nShowNameMode = m_nShowNameMode | 0x10000;
	}
	if(m_nShowNameMode == 0)
	{
		m_nShowNameMode = 1;
	}
	CDialog::OnOK();
}
