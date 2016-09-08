#ifndef _BTNST_H
#define _BTNST_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Return values
#ifndef	BTNST_OK
#define	BTNST_OK						0
#endif
#ifndef	BTNST_INVALIDRESOURCE
#define	BTNST_INVALIDRESOURCE			1
#endif
#ifndef	BTNST_FAILEDMASK
#define	BTNST_FAILEDMASK				2
#endif
#ifndef	BTNST_INVALIDINDEX
#define	BTNST_INVALIDINDEX				3
#endif
#ifndef	BTNST_INVALIDALIGN
#define	BTNST_INVALIDALIGN				4
#endif
#ifndef	BTNST_BADPARAM
#define	BTNST_BADPARAM					5
#endif

// Dummy identifier for grayscale icon
#ifndef	BTNST_AUTO_GRAY
#define	BTNST_AUTO_GRAY					(HICON)(0xffffffff - 1L)
#endif



enum Buildings
{
	CANYIN1 = 0, CANYIN2, CANYIN3, CANYIN4, CANYIN5, CANYIN6,
	YULE1, YULE2, YULE3, YULE4, YULE5, YULE6,
	FUZHUANG1, FUZHUANG2, FUZHUANG3, FUZHUANG4, FUZHUANG5, FUZHUANG6,
	SHECHI1, SHECHI2, SHECHI3, SHECHI4, SHECHI5, SHECHI6,
	LINGSHOU1, LINGSHOU2, LINGSHOU3, LINGSHOU4, LINGSHOU5, LINGSHOU6,
	SHUMA1, SHUMA2, SHUMA3, SHUMA4, SHUMA5, SHUMA6,
	EMPTY, YESUS, EMPIRE, EIFFEL
};


const TCHAR SECTION_INFO[10]  = L"Info";
const DWORD BASEFLOW = 80;
const UINT  COMMANDSTART = 32774;

class CButtonST : public CButton
{
public:
    CButtonST();
	~CButtonST();

    enum	{	ST_ALIGN_HORIZ	= 0,			// Icon/bitmap on the left, text on the right
				ST_ALIGN_VERT,					// Icon/bitmap on the top, text on the bottom
				ST_ALIGN_HORIZ_RIGHT			// Icon/bitmap on the right, text on the left
			};

	enum	{	BTNST_COLOR_BK_IN	= 0,		// Background color when mouse is INside
				BTNST_COLOR_FG_IN,				// Text color when mouse is INside
				BTNST_COLOR_BK_OUT,				// Background color when mouse is OUTside
				BTNST_COLOR_FG_OUT,				// Text color when mouse is OUTside
				BTNST_COLOR_BK_FOCUS,			// Background color when the button is focused
				BTNST_COLOR_FG_FOCUS,			// Text color when the button is focused

				BTNST_MAX_COLORS
			};

	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CButtonST)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	//void SetBaseFlow(DWORD dword){ m_dwBaseFlow = dword; }
	const DWORD GetBaseFlow(){ return m_dwBaseFlow; }
	void AddBaseFlow(const DWORD number){ m_dwBaseFlow += number; }
	const DWORD GetRadiationFlow(){ return m_dwRadiationFlow; }
	void SetRadiationFlow(const DWORD number){ m_dwRadiationFlow = number; }
	void SetID(LPCTSTR);
	BOOL IsBuilding(){ return m_bIsBuilding; }
	void SetBuilding(DWORD number){ m_dwBuilding = number; }
	const DWORD GetWhichBuilding(){ return m_dwBuilding; }
	void InitData();
	void CalcIncome(){ m_dwIncome = DWORD(m_dwRadiationFlow*m_dwPrice*m_dwRate); }
	const DWORD GetIncome(){ return m_dwIncome; }
	void GenerateJoint();
	void CalcJoint(DWORD bd1, DWORD bd2, DWORD bd3, DWORD bd4);

	void SetImmutable(BOOL b){ m_bIsImmutable = b;}
	void SetCompete(DWORD number){ m_bIsCompete[number] = TRUE; }
private:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnSelectMenu(UINT nID);
	DWORD m_dwBaseFlow;
	DWORD m_dwRadiationFlow;
	float m_dwRate;
	DWORD m_dwPrice;
	DWORD m_dwIncome;
	CString m_sID;
	CString m_sBuilding;
	BOOL m_bIsJoint1;
	DWORD m_dwJoint1;
	CString m_sJoint1;
	BOOL m_bIsJoint2;
	DWORD m_dwJoint2;
	CString m_sJoint2;

	DWORD m_dwBuilding;
	BOOL m_bIsBuilding;

	BOOL m_bIsImmutable;
	BOOL m_bIsCompete[36];
public:
	DWORD SetDefaultColors(BOOL bRepaint = TRUE);
	DWORD SetColor(BYTE byColorIndex, COLORREF crColor, BOOL bRepaint = TRUE);
	DWORD GetColor(BYTE byColorIndex, COLORREF* crpColor);
	DWORD OffsetColor(BYTE byColorIndex, short shOffset, BOOL bRepaint = TRUE);

	DWORD SetCheck(int nCheck, BOOL bRepaint = TRUE);
	int GetCheck();

	DWORD SetURL(LPCTSTR lpszURL = NULL);
	void DrawTransparent(BOOL bRepaint = FALSE);

	BOOL GetDefault();
	DWORD SetAlwaysTrack(BOOL bAlwaysTrack = TRUE);

	void SetTooltipText(int nText, BOOL bActivate = TRUE);
	void SetTooltipText(LPCTSTR lpszText, BOOL bActivate = TRUE);
	void ActivateTooltip(BOOL bEnable = TRUE);

	DWORD SetBtnCursor(int nCursorId = NULL, BOOL bRepaint = TRUE);

	DWORD SetFlat(BOOL bFlat = TRUE, BOOL bRepaint = TRUE);
	DWORD SetAlign(BYTE byAlign, BOOL bRepaint = TRUE);

	DWORD DrawBorder(BOOL bDrawBorder = TRUE, BOOL bRepaint = TRUE);
	DWORD DrawFlatFocus(BOOL bDrawFlatFocus, BOOL bRepaint = TRUE);

	DWORD SetIcon(int nIconIn, int nIconOut = NULL);
	DWORD SetIcon(HICON hIconIn, HICON hIconOut = NULL);

	DWORD SetBitmaps(int nBitmapIn, COLORREF crTransColorIn, int nBitmapOut = NULL, COLORREF crTransColorOut = 0);
	DWORD SetBitmaps(HBITMAP hBitmapIn, COLORREF crTransColorIn, HBITMAP hBitmapOut = NULL, COLORREF crTransColorOut = 0);

#ifdef	BTNST_USE_BCMENU
	DWORD SetMenu(UINT nMenu, HWND hParentWnd, BOOL bWinXPStyle = TRUE, UINT nToolbarID = NULL, CSize sizeToolbarIcon = CSize(16, 16), COLORREF crToolbarBk = RGB(255, 0, 255), BOOL bRepaint = TRUE);
#else
	DWORD SetMenu(UINT nMenu, HWND hParentWnd, BOOL bRepaint = TRUE);
#endif

	static short GetVersionI()		{return 35;}
	static LPCTSTR GetVersionC()	{return (LPCTSTR)_T("3.5");}

	BOOL	m_bShowDisabledBitmap;

protected:
    //{{AFX_MSG(CButtonST)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSysColorChange();
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

#ifdef	BTNST_USE_BCMENU
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
#endif

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	HICON CreateGrayscaleIcon(HICON hIcon);
	virtual DWORD OnDrawBackground(CDC* pDC, LPCRECT pRect);
	virtual DWORD OnDrawBorder(CDC* pDC, LPCRECT pRect);

	BOOL		m_bIsFlat;			// Is a flat button?

	BOOL		m_bDrawTransparent;	// Draw transparent?

	BOOL		m_bIsDisabled;		// Is button disabled?
	BOOL		m_bIsDefault;		// Is default button?
	BOOL		m_bIsCheckBox;		// Is the button a checkbox?
	BYTE		m_byAlign;			// Align mode
	BOOL		m_bDrawBorder;		// Draw border?
	BOOL		m_bDrawFlatFocus;	// Draw focus rectangle for flat button?
	COLORREF	m_crColors[BTNST_MAX_COLORS];	// Colors to be used
	HWND		m_hParentWndMenu;	// Handle to window for menu selection
	BOOL		m_bMenuDisplayed;	// Is menu displayed ?

#ifdef	BTNST_USE_BCMENU
	BCMenu		m_menuPopup;		// BCMenu class instance
#else
	HMENU		m_hMenu;			// Handle to associated menu
#endif

private:
	LRESULT OnSetCheck(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetCheck(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetStyle(WPARAM wParam, LPARAM lParam);

	void FreeResources(BOOL bCheckForNULL = TRUE);
	void PrepareImageRect(BOOL bHasTitle, RECT* rpItem, CRect* rpTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, CRect* rpImage);
	HBITMAP CreateBitmapMask(HBITMAP hSourceBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTransColor);
	void DrawTheIcon(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpTitle, BOOL bIsPressed, BOOL bIsDisabled);
	void DrawTheBitmap(CDC* pDC, BOOL bHasTitle, RECT *rItem, CRect *rCaption, BOOL bIsPressed, BOOL bIsDisabled);
	void PaintBk(CDC* pDC);

	void InitToolTip();

	HCURSOR		m_hCursor;			// Handle to cursor
	CToolTipCtrl m_ToolTip;			// Tooltip

	CDC			m_dcBk;
	CBitmap		m_bmpBk;
	CBitmap*	m_pbmpOldBk;

	BOOL		m_bAlwaysTrack;		// Always hilight button?
	int			m_nCheck;			// Current value for checkbox
	UINT		m_nTypeStyle;		// Button style

	TCHAR		m_szURL[_MAX_PATH];	// URL to open when clicked

#pragma pack(1)
	typedef struct _STRUCT_ICONS
	{
		HICON		hIcon;			// Handle to icon
		DWORD		dwWidth;		// Width of icon
		DWORD		dwHeight;		// Height of icon
	} STRUCT_ICONS;
#pragma pack()

#pragma pack(1)
	typedef struct _STRUCT_BITMAPS
	{
		HBITMAP		hBitmap;		// Handle to bitmap
		DWORD		dwWidth;		// Width of bitmap
		DWORD		dwHeight;		// Height of bitmap
		HBITMAP		hMask;			// Handle to mask bitmap
		COLORREF	crTransparent;	// Transparent color
	} STRUCT_BITMAPS;
#pragma pack()

	STRUCT_ICONS	m_csIcons[2];
	STRUCT_BITMAPS	m_csBitmaps[2];

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
