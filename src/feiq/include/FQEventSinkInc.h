#pragma once
#include "FQEventSink.h"


BEGIN_FQ_EVENT_SINK(ModuleSite)
	FQ_EVENT(OnDataReceived, (LPCTSTR key), (key));
	FQ_EVENT(OnViewData, (LPCTSTR key, LPCTSTR Result), (key, Result));
	FQ_EVENT(OnSendDataResult, (LPCTSTR Sender, LPCTSTR key, enum FQ_SENDDATA_RESULT nResult), (Sender, key, nResult));
END_FQ_EVENT_SINK()

BEGIN_FQ_EVENT_SINK(Root)
	FQ_EVENT(OnUserEnter, (LPCTSTR Memo, LPCTSTR Name, LPCTSTR Host, LPCTSTR Login, LPCTSTR MemoGroup, LPCTSTR Group, LPCTSTR IP, LPCTSTR MAC), (Memo, Name, Host, Login, MemoGroup, Group, IP, MAC));
	FQ_EVENT(OnUserExit, (LPCTSTR Memo, LPCTSTR Name, LPCTSTR Host, LPCTSTR Login, LPCTSTR MemoGroup, LPCTSTR Group, LPCTSTR IP, LPCTSTR MAC), (Memo, Name, Host, Login, MemoGroup, Group, IP, MAC));
	FQ_EVENT(OnSkinChange, (COLORREF clrFrame, COLORREF clrFont, COLORREF clrLeftTop, COLORREF clrRightBtm), (clrFrame, clrFont, clrLeftTop, clrRightBtm));
	FQ_EVENT(OnFolderBarChange, (LONG nBeforeBarIndex, LONG nNowBarIndex), (nBeforeBarIndex, nNowBarIndex));
	FQ_EVENT(OnBeforeRecvMsg, (LPCTSTR Memo, LPCTSTR Host, LPCTSTR Group, LPCTSTR IP, LPCTSTR MAC, LPCTSTR sendMsg, LPCTSTR font, enum FQ_BEFORERECVMSG_RESULT *pResult), (Memo, Host, Group, IP, MAC, sendMsg, font, pResult));
	FQ_EVENT(OnLoadModule, (), ());
	FQ_EVENT(OnUnLoadModule, (), ());
	FQ_EVENT(OnSkinChange2, (COLORREF clrRgb), (clrRgb));
	FQ_EVENT(OnMainPanelChange, (LPCTSTR HidePanelName, HWND hHideWnd, LPCTSTR ShowPanelName, HWND hShowWnd), (HidePanelName, hHideWnd, ShowPanelName, hShowWnd));
	FQ_EVENT(OnCustomButtonClick, (LPCTSTR ButtonName), (ButtonName));
	FQ_EVENT(OnRootEvent, (LPCTSTR eventType, LPCTSTR Name, LPCTSTR sParam, IFQData* pData), (eventType, Name, sParam, pData));
END_FQ_EVENT_SINK()
