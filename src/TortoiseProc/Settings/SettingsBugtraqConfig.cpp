// settings\SettingsBugtraqConfig.cpp : implementation file
//

#include "stdafx.h"
#include "TortoiseProc.h"
#include "settings\SettingsBugtraqConfig.h"
#include "ProjectProperties.h"
#include "git.h"
#include "messagebox.h"
// CSettingsBugtraqConfig dialog

IMPLEMENT_DYNAMIC(CSettingsBugtraqConfig, ISettingsPropPage)

CSettingsBugtraqConfig::CSettingsBugtraqConfig(CString cmdPath)
	: ISettingsPropPage(CSettingsBugtraqConfig::IDD)	
	, m_URL(_T(""))
	, m_bNWarningifnoissue(FALSE)
	, m_Message(_T(""))
	, m_bNAppend(FALSE)
	, m_Label(_T(""))
	, m_bNNumber(FALSE)
	, m_Logregex(_T(""))
{
	m_ChangeMask=0;
}

CSettingsBugtraqConfig::~CSettingsBugtraqConfig()
{
}

void CSettingsBugtraqConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BUGTRAQ_URL, m_URL);
	DDX_Radio(pDX, IDC_BUGTRAQ_WARNINGIFNOISSUE_TRUE, m_bNWarningifnoissue);
	DDX_Text(pDX, IDC_BUGTRAQ_MESSAGE, m_Message);
	DDX_Radio(pDX, IDC_BUGTRAQ_APPEND_TRUE, m_bNAppend);
	DDX_Text(pDX, IDC_BUGTRAQ_LABEL, m_Label);
	DDX_Radio(pDX, IDC_BUGTRAQ_NUMBER_TRUE, m_bNNumber);
	DDX_Text(pDX, IDC_BUGTRAQ_LOGREGEX, m_Logregex);
}


BEGIN_MESSAGE_MAP(CSettingsBugtraqConfig, ISettingsPropPage)

	ON_EN_CHANGE(IDC_BUGTRAQ_URL, &CSettingsBugtraqConfig::OnEnChangeBugtraqUrl)
	ON_BN_CLICKED(IDC_BUGTRAQ_WARNINGIFNOISSUE_TRUE, &CSettingsBugtraqConfig::OnBnClickedBugtraqWarningifnoissueTrue)
	ON_BN_CLICKED(IDC_BUGTRAQ_WARNINGIFNOISSUE_FALSE, &CSettingsBugtraqConfig::OnBnClickedBugtraqWarningifnoissueFalse)
	ON_EN_CHANGE(IDC_BUGTRAQ_MESSAGE, &CSettingsBugtraqConfig::OnEnChangeBugtraqMessage)
	ON_BN_CLICKED(IDC_BUGTRAQ_APPEND_TRUE, &CSettingsBugtraqConfig::OnBnClickedBugtraqAppendTrue)
	ON_BN_CLICKED(IDC_BUGTRAQ_APPEND_FALSE, &CSettingsBugtraqConfig::OnBnClickedBugtraqAppendFalse)
	ON_EN_CHANGE(IDC_BUGTRAQ_LABEL, &CSettingsBugtraqConfig::OnEnChangeBugtraqLabel)
	ON_BN_CLICKED(IDC_BUGTRAQ_NUMBER_TRUE, &CSettingsBugtraqConfig::OnBnClickedBugtraqNumberTrue)
	ON_BN_CLICKED(IDC_BUGTRAQ_NUMBER_FALSE, &CSettingsBugtraqConfig::OnBnClickedBugtraqNumberFalse)
	ON_EN_CHANGE(IDC_BUGTRAQ_LOGREGEX, &CSettingsBugtraqConfig::OnEnChangeBugtraqLogregex)
END_MESSAGE_MAP()

BOOL CSettingsBugtraqConfig::OnInitDialog()
{
	ISettingsPropPage::OnInitDialog();
	ProjectProperties::GetStringProps(this->m_URL,_T("bugtraq.url"));
	ProjectProperties::GetStringProps(this->m_Logregex,_T("bugtraq.logregex"));
	ProjectProperties::GetStringProps(this->m_Label,_T("bugtraq.label"));
	ProjectProperties::GetStringProps(this->m_Message,_T("bugtraq.message"));

	ProjectProperties::GetBOOLProps(this->m_bNAppend,_T("bugtraq.append"));
	ProjectProperties::GetBOOLProps(this->m_bNNumber,_T("bugtraq.number"));
	ProjectProperties::GetBOOLProps(this->m_bNWarningifnoissue,_T("bugtraq.warnifnoissue"));
	
	m_bNAppend = !m_bNAppend;
	m_bNNumber = !m_bNNumber;
	m_bNWarningifnoissue = !m_bNWarningifnoissue;

	this->UpdateData(FALSE);
	return TRUE;
}

BOOL CSettingsBugtraqConfig::OnApply()
{
	this->UpdateData();

	CString cmd,out;
	if(m_ChangeMask & BUG_URL)
	{
		cmd.Format(_T("git.exe config bugtraq.url \"%s\""),m_URL);
		if(g_Git.Run(cmd,&out,CP_ACP))
		{
			CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
		}
	}
	out.Empty();
	if(m_ChangeMask & BUG_WARNING)
	{
		cmd.Format(_T("git.exe config bugtraq.warnifnoissue \"%s\""),(!this->m_bNWarningifnoissue)?_T("true"):_T("false"));
		if(g_Git.Run(cmd,&out,CP_ACP))
		{
			CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
		}

	}
	out.Empty();
	if(m_ChangeMask & BUG_MESSAGE)
	{
		cmd.Format(_T("git.exe config bugtraq.message \"%s\""),this->m_Message);
		if(g_Git.Run(cmd,&out,CP_ACP))
		{
			CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
		}

	}
	out.Empty();
	if(m_ChangeMask & BUG_APPEND )
	{
		cmd.Format(_T("git.exe config bugtraq.append \"%s\""),(!this->m_bNAppend)?_T("true"):_T("false"));
		if(g_Git.Run(cmd,&out,CP_ACP))
		{
			CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
		}

	}
	out.Empty();
	if(m_ChangeMask & BUG_LABEL )
	{
		cmd.Format(_T("git.exe config bugtraq.label \"%s\""),this->m_Label);
		if(g_Git.Run(cmd,&out,CP_ACP))
		{
			CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
		}

	}
	out.Empty();
	if(m_ChangeMask &BUG_NUMBER )
	{
		cmd.Format(_T("git.exe config bugtraq.number \"%s\""),(!this->m_bNNumber)?_T("true"):_T("false"));
		if(g_Git.Run(cmd,&out,CP_ACP))
		{
			CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
		}

	}
	out.Empty();
	if(m_ChangeMask & BUG_LOGREGEX)
	{
		cmd.Format(_T("git.exe config bugtraq.logregex \"%s\""),this->m_Logregex);
		if(g_Git.Run(cmd,&out,CP_ACP))
		{
			CMessageBox::Show(NULL,out,_T("TortoiseGit"),MB_OK);
		}

	}

	m_ChangeMask= 0;
	return TRUE;
}
// CSettingsBugtraqConfig message handlers

void CSettingsBugtraqConfig::OnEnChangeBugtraqUrl()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the ISettingsPropPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_ChangeMask |= BUG_URL;
	SetModified();
}

void CSettingsBugtraqConfig::OnBnClickedBugtraqWarningifnoissueTrue()
{
	// TODO: Add your control notification handler code here
	m_ChangeMask |= BUG_WARNING;
	SetModified();
}

void CSettingsBugtraqConfig::OnBnClickedBugtraqWarningifnoissueFalse()
{
	// TODO: Add your control notification handler code here
	m_ChangeMask |= BUG_WARNING;
	SetModified();
}

void CSettingsBugtraqConfig::OnEnChangeBugtraqMessage()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the ISettingsPropPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_ChangeMask |= BUG_MESSAGE;
	SetModified();
}

void CSettingsBugtraqConfig::OnBnClickedBugtraqAppendTrue()
{
	// TODO: Add your control notification handler code here
	m_ChangeMask |= BUG_APPEND;
	SetModified();
}

void CSettingsBugtraqConfig::OnBnClickedBugtraqAppendFalse()
{
	// TODO: Add your control notification handler code here
	m_ChangeMask |= BUG_APPEND;
	SetModified();
}

void CSettingsBugtraqConfig::OnEnChangeBugtraqLabel()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the ISettingsPropPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_ChangeMask |= BUG_LABEL;
	SetModified();
}

void CSettingsBugtraqConfig::OnBnClickedBugtraqNumberTrue()
{
	// TODO: Add your control notification handler code here
	m_ChangeMask |= BUG_NUMBER;
	SetModified();
}

void CSettingsBugtraqConfig::OnBnClickedBugtraqNumberFalse()
{
	// TODO: Add your control notification handler code here
	m_ChangeMask |= BUG_NUMBER;
	SetModified();
}

void CSettingsBugtraqConfig::OnEnChangeBugtraqLogregex()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the ISettingsPropPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_ChangeMask |= BUG_LOGREGEX;
	SetModified();
}
