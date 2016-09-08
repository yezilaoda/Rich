
// RichDlg.h : 头文件
//

#pragma once
#include "BtnST.h"
#include <map>
// CRichDlg 对话框
typedef std::map<CString, int> AssistCalc;

class CRichDlg : public CDialogEx
{
	// 构造
public:
	CRichDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_RICH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CButtonST m_bt[169];
	afx_msg LRESULT OnMessageREFRESH(WPARAM wParam, LPARAM lParam);
	void CalcEvery();
	void CalcTotal();
	void CalcCompete();
private:
	DWORD	m_dwTotalFlow;
	CString m_sTotalFlow;
	__int64	m_dwTotalIncome;
	CString m_sTotalIncome;

	AssistCalc m_mapAssistCalc;
};
