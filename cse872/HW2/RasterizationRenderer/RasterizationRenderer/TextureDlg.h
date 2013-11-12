#pragma once


// TextureDlg dialog

class TextureDlg : public CDialog
{
	DECLARE_DYNAMIC(TextureDlg)

public:
	TextureDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TextureDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_TEXTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nTexture;
	int m_nShaders;
};
