
// RichDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Rich.h"
#include "RichDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRichDlg �Ի���
TCHAR PROFILE[MAX_PATH];


CRichDlg::CRichDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRichDlg::IDD, pParent)
	, m_sTotalFlow(_T(""))
	, m_sTotalIncome(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


}

void CRichDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TOTALFLOW, m_sTotalFlow);
	DDX_Text(pDX, IDC_TOTALINCOME, m_sTotalIncome);
}

BEGIN_MESSAGE_MAP(CRichDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_REFRESH, OnMessageREFRESH)
END_MESSAGE_MAP()

const UINT captionHeight = 30;
const UINT side = 90;
#define  SIDELENGTH  13
// CRichDlg ��Ϣ�������

BOOL CRichDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	MoveWindow(0, 0, SIDELENGTH*side, (SIDELENGTH*side + captionHeight));
	CenterWindow();

	TCHAR szAppDataDirectory[MAX_PATH];
	SHGetFolderPath(0, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, 0, 0, szAppDataDirectory);
	wsprintf(PROFILE, _T("%s\\Rich\\config.ini"), szAppDataDirectory);


	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	CxLog::Instance().SetLogFileName(L"rich.log");

	for (int j = 0; j < SIDELENGTH; j++)
	{
		for (int i = 0; i < SIDELENGTH; i++)
		{
			m_bt[j * SIDELENGTH+ i].Create(_T(""), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				CRect(i * side, captionHeight + j * side, i * side + side + 3, captionHeight + j * side + side + 3),
				this, IDC_BUTTON + j * SIDELENGTH+ i);
			CString sID;
			sID.Format(L"%d,%d", j, i);

			WCHAR temp[5] = { 0 };
			if (sID == "1,7")
			{
				m_bt[j * SIDELENGTH+ i].SetBuilding(Buildings::SHECHI3);
				m_bt[j * SIDELENGTH+ i].SetImmutable(TRUE);
				WritePrivateProfileString(SECTION_INFO, sID, L"21", PROFILE);
			}
			else if (sID == "6,8")
			{
				m_bt[j * SIDELENGTH+ i].SetBuilding(Buildings::SHUMA4);
				m_bt[j * SIDELENGTH+ i].SetImmutable(TRUE);
				WritePrivateProfileString(SECTION_INFO, sID, L"34", PROFILE);
			}
			else if (sID == "7,1")
			{
				m_bt[j * SIDELENGTH+ i].SetBuilding(Buildings::CANYIN6);
				m_bt[j * SIDELENGTH+ i].SetImmutable(TRUE);
				WritePrivateProfileString(SECTION_INFO, sID, L"6", PROFILE);
			}

			if (GetPrivateProfileInt(SECTION_INFO, sID, NULL, PROFILE) == 0)
			{
				_itow_s(Buildings::SHUMA6, temp, 10);
				WritePrivateProfileString(SECTION_INFO, sID, temp, PROFILE);
			}

			m_bt[j * SIDELENGTH+ i].SetID(sID);
		}
	}

	CalcEvery();
	CalcTotal();
	CalcCompete();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRichDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRichDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

const DWORD JESUSFLOW = 928;
const DWORD EMPIREFLOW = 387;

void CRichDlg::CalcEvery()
{
	// �����������
	for (int j = 0; j < SIDELENGTH; j++)
	{
		for (int i = 0; i < SIDELENGTH; i++)
		{
			BOOL isBuilding = m_bt[j * SIDELENGTH+ i].IsBuilding();
			if (isBuilding)
			{
				for (int k = j - 1; k < j + 2; k++)
				{
					for (int m = i - 1; m < i + 2; m++)
					{
						//LOGINFO("x,y:%d,%d", k, m);
						DWORD dwBuildingFlow = 0;
						if (m_bt[j * SIDELENGTH+ i].GetWhichBuilding() == Buildings::YESUS)
							dwBuildingFlow = JESUSFLOW;
						else if (m_bt[j * SIDELENGTH+ i].GetWhichBuilding() == Buildings::EMPIRE)
							dwBuildingFlow = EMPIREFLOW;

						if ((k < 0) | (m < 0) | (k > 8) | (m > 8))
						{
							DWORD dwFlow = dwBuildingFlow;
							CString temp;
							temp.Format(L"%d,%d", k, m);

							AssistCalc::iterator iter;
							iter = m_mapAssistCalc.find(temp);
							if (iter != m_mapAssistCalc.end())
							{
								dwFlow += iter->second;
								m_mapAssistCalc[temp] = dwFlow;

							}
							else
							{
								dwFlow += 80;
								m_mapAssistCalc.insert(std::pair<CString, int>(temp, dwFlow));
							}

							//LOGINFO("[%d,%d],dwBuildingFlow:%d", k, m, dwBuildingFlow);
						}
						else
						{
							m_bt[k * SIDELENGTH+ m].AddBaseFlow(dwBuildingFlow);
							//LOGINFO("k * sidelength+ m:%d,BaseFlow:%d", k * sidelength+ m, m_bt[k * sidelength+ m].GetBaseFlow());
						}

					}
				}
			}
		}
	}
	// �����������
	for (int j = 0; j < SIDELENGTH; j++)
	{
		for (int i = 0; i < SIDELENGTH; i++)
		{
			if (m_bt[j * SIDELENGTH+ i].IsBuilding())
			{
				continue;
			}

			DWORD dwRadiationFlow = 0;
			for (int k = j - 3; k < j + 4; k++)
			{
				for (int m = i - 3; m < i + 4; m++)
				{
					if ((k < 0) | (m < 0) | (k > 8) | (m > 8))
					{
						DWORD dwFlow = 0;

						CString temp;
						temp.Format(L"%d,%d", k, m);

						AssistCalc::iterator iter;
						iter = m_mapAssistCalc.find(temp);
						if (iter != m_mapAssistCalc.end())
						{
							//LOGINFO("[%d,%d],iter->second:%d", k, m, iter->second);
							dwFlow = iter->second;
						}
						else
						{

							dwFlow = BASEFLOW;
						}
						//LOGINFO("[%d,%d],dwFlow:%d", k, m, dwFlow);
						dwRadiationFlow += dwFlow;
					}
					else
					{
						dwRadiationFlow += m_bt[k * SIDELENGTH+ m].GetBaseFlow();
						//LOGINFO("k * sidelength+ m:%d,GetBaseFlow():%d", k * sidelength+ m, m_bt[k * sidelength+ m].GetBaseFlow());
					}
					//LOGINFO("x,y:%d,%d", k, m);
				}
			}
			m_bt[j * SIDELENGTH+ i].SetRadiationFlow(dwRadiationFlow);
			//LOGINFO("No:%d,dwRadiationFlow:%d", j * sidelength+ i, dwRadiationFlow);
		}
	}
	// ������Ӫ
	for (int j = 0; j < SIDELENGTH; j++)
	{
		for (int i = 0; i < SIDELENGTH; i++)
		{

			int nButtonIndex = j * SIDELENGTH+ i;

			int nIndex1 = nButtonIndex - 1;
			if (i - 1 < 0)
				nIndex1 = nButtonIndex;
			int nIndex2 = nButtonIndex + 1;
			if (i + 1 > 8)
				nIndex2 = nButtonIndex;
			int nIndex3 = nButtonIndex - 9;
			if (j - 1 < 0)
				nIndex3 = nButtonIndex;
			int nIndex4 = nButtonIndex + 9;
			if (j + 1 > 8)
				nIndex4 = nButtonIndex;

			m_bt[nButtonIndex].CalcJoint(m_bt[nIndex1].GetWhichBuilding(), m_bt[nIndex2].GetWhichBuilding(), m_bt[nIndex3].GetWhichBuilding(), m_bt[nIndex4].GetWhichBuilding());

			m_bt[nButtonIndex].CalcIncome();
		}
	}
}

void CRichDlg::CalcTotal()
{
	for (int j = 0; j < SIDELENGTH; j++)
	{
		for (int i = 0; i < SIDELENGTH; i++)
		{
			m_bt[j * SIDELENGTH+ i].Invalidate(TRUE);
			m_dwTotalFlow += m_bt[j * SIDELENGTH+ i].GetRadiationFlow();
			m_dwTotalIncome += m_bt[j * SIDELENGTH+ i].GetIncome();
			//LOGINFO("No.%d,GetIncome:%d", j * sidelength+ i, m_bt[j * sidelength+ i].GetIncome());
			//LOGINFO("m_dwTotalIncome:%I64d", m_dwTotalIncome);
		}
	}

	m_sTotalFlow.Format(L"%d", m_dwTotalFlow);
	m_sTotalIncome.Format(L"%I64d", m_dwTotalIncome);
	UpdateData(FALSE);
}

void CRichDlg::CalcCompete()
{
	for (int j = 0; j < SIDELENGTH; j++)
	{
		for (int i = 0; i < SIDELENGTH; i++)
		{
			for (int k = j - 6; k < j + 7; k++)
			{
				for (int m = i - 6; m < i + 7; m++)
				{
					if ((k < 0) | (m < 0) | (k > 8) | (m > 8))
					{
						continue;
					}
					DWORD dwBuiding = m_bt[j * SIDELENGTH+ i].GetWhichBuilding();
					if (dwBuiding < Buildings::YESUS)
					{
						m_bt[k * SIDELENGTH+ m].SetCompete(dwBuiding);
					}
				}
			}
		}
	}
}

LRESULT CRichDlg::OnMessageREFRESH(WPARAM wParam, LPARAM lParam)
{
	// init
	for (int j = 0; j < SIDELENGTH; j++)
	{
		for (int i = 0; i < SIDELENGTH; i++)
		{
			m_bt[j * SIDELENGTH+ i].InitData();
		}
	}
	m_dwTotalFlow = 0;
	m_dwTotalIncome = 0;
	m_mapAssistCalc.clear();
	CalcEvery();
	CalcTotal();
	CalcCompete();
	return S_OK;
}