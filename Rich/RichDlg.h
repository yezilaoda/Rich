
// RichDlg.h : ͷ�ļ�
//

#pragma once
#include "BtnST.h"
#include <map>
// CRichDlg �Ի���
typedef std::map<CString, int> AssistCalc;

class CRichDlg : public CDialogEx
{
	// ����
public:
	CRichDlg(CWnd* pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_RICH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


	// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
