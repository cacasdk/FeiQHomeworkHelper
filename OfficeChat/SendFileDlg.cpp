// SendFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OfficeChat.h"
#include "SendFileDlg.h"
#include "afxtempl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendFileDlg dialog


CSendFileDlg::CSendFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendFileDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSendFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendFileDlg)
	DDX_Control(pDX, IDC_LIST_FILE, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendFileDlg, CDialog)
	//{{AFX_MSG_MAP(CSendFileDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendFileDlg message handlers

void CSendFileDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_EXPLORER,"All Files (*.*)|*.*||", this);
	//--------只要加上下面这段代码，就可以显示2000下的打开文件对话框-------------
	//------用来检测目前的操作系统的版本信息
	int nStructSize = 0;
	DWORD dwVersion, dwWindowsMajorVersion, dwWindwosMinorVersion;
	dwVersion = GetVersion();
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwWindwosMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if(dwVersion < 0x80000000)
	{
		nStructSize = 88;   //2k,xp下显示新的版本
	}
	else
	{
		nStructSize = 76;  //98下显示老版本
	}
	filedlg.m_ofn.lStructSize = nStructSize;  //------用来检测目前的操作系统的版本信息
	filedlg.m_ofn.Flags|=OFN_ALLOWMULTISELECT;
	DWORD MAXFILE = 2562 * 50; //2562 is the max
	filedlg.m_ofn.nMaxFile = MAXFILE;//2562 is the max
	char* pc = new char[MAXFILE];
	filedlg.m_ofn.lpstrFile = pc;
	filedlg.m_ofn.lpstrFile[0] = NULL;
	if(filedlg.DoModal()==IDOK)
	{
		POSITION   pos=filedlg.GetStartPosition();   
		CStringArray   strFileNameArr;   
		strFileNameArr.RemoveAll();
		while(pos)   
		{   
			strFileNameArr.Add(filedlg.GetNextPathName(pos));        
		}   

		for(int i = 0; i < strFileNameArr.GetSize(); i++)
		{
			char  pFilename[MAX_PATH];  
			WIN32_FIND_DATA	fdat;
			
			strcpy(pFilename, strFileNameArr[i]);
			if (GetFileInfomation(pFilename, &fdat))
			{
				for(int k = 0; k < m_strSendFilePathArr.GetSize(); k++)
				{
					if(strcmp(m_strSendFilePathArr[k], pFilename) == 0)break;
				}
				if(k == m_strSendFilePathArr.GetSize())
				{
					m_strSendFilePathArr.Add(pFilename);
					m_strSendFileNameArr.Add(fdat.cFileName);
				}
			}
		}

		RefreshList();
	}
	delete []pc; 
}

void CSendFileDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	BROWSEINFO   bi;       
	char   dispname[MAX_PATH],   pFilename[MAX_PATH];       
	ITEMIDLIST   *   pidl;       
    
	bi.hwndOwner   =   m_hWnd;       
	bi.pidlRoot   =   0;       
	bi.pszDisplayName   =   dispname;       
	bi.lpszTitle   =   "增加文件夹:";       
	bi.ulFlags   =   BIF_RETURNONLYFSDIRS;       
	bi.lpfn   =   0;       
	bi.lParam   =   0;       
	bi.iImage   =   0;       
    
	if   (pidl   =   SHBrowseForFolder(&bi))   //弹出文件夹浏览窗口，并选取目录   
	{       
		SHGetPathFromIDList(pidl,   pFilename);       
		
		WIN32_FIND_DATA	fdat;
		
		if (GetFileInfomation(pFilename, &fdat))
		{
			for(int k = 0; k < m_strSendFilePathArr.GetSize(); k++)
			{
				if(strcmp(m_strSendFilePathArr[k], pFilename) == 0)break;
			}
			if(k == m_strSendFilePathArr.GetSize())
			{
				m_strSendFilePathArr.Add(pFilename);
				m_strSendFileNameArr.Add(fdat.cFileName);
			}
		}

		RefreshList();
	}
}

void CSendFileDlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	if(m_list.GetSelectedCount() == 0) return;
	CArray<int, int> delArr;
	for(int t = 0; t < m_strSendFilePathArr.GetSize(); t++)
	{
		delArr.Add(0);
	}
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nIndex = m_list.GetNextSelectedItem(pos);
		if(nIndex >= 0 && nIndex < delArr.GetSize())
		{
			delArr[nIndex] = 1;
		}
	}
	for(int i = m_strSendFilePathArr.GetSize() - 1; i >= 0; i--)
	{
		if(delArr[i])
		{
			m_strSendFilePathArr.RemoveAt(i);
			m_strSendFileNameArr.RemoveAt(i);
		}
	}
	RefreshList();
}

void CSendFileDlg::RefreshList()
{
	m_list.DeleteAllItems();
	for(int i = 0; i < m_strSendFileNameArr.GetSize(); i++)
	{
		m_list.InsertItem(i, m_strSendFileNameArr[i]);
		m_list.SetItemText(i, 0, m_strSendFileNameArr[i]);
		m_list.SetItemText(i, 1, m_strSendFilePathArr[i]);
	}
}

BOOL CSendFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LV_COLUMN lvcol;

	//向列表控制中添加表列
	lvcol.mask=LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvcol.fmt=LVCFMT_LEFT;
	int i=0;
	lvcol.pszText="文件名";
	lvcol.iSubItem=i;
	lvcol.cx=100;
	m_list.InsertColumn(i++,&lvcol);

	lvcol.pszText="位于";
	lvcol.iSubItem=i;
	lvcol.cx=100;
	m_list.InsertColumn(i++,&lvcol);

	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);//完全行选择

	RefreshList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSendFileDlg::GetFileInfomation(const char *path, WIN32_FIND_DATA *fdata)
{
	HANDLE	fh;

	if ((fh = ::FindFirstFile(path, fdata)) != INVALID_HANDLE_VALUE)
	{
		::FindClose(fh);
		return	TRUE;
	}

	memset(fdata, 0, sizeof(WIN32_FIND_DATA));

	if ((fh = ::CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0)) != INVALID_HANDLE_VALUE)
	{
		BY_HANDLE_FILE_INFORMATION	info;
		BOOL	info_ret = ::GetFileInformationByHandle(fh, &info);
		::CloseHandle(fh);
		if (info_ret)
			return	memcpy(fdata, &info, (char *)&info.dwVolumeSerialNumber - (char *)&info), TRUE;
	}

	return	(fdata->dwFileAttributes = ::GetFileAttributes(path)) == 0xffffffff ? FALSE : TRUE;
}

