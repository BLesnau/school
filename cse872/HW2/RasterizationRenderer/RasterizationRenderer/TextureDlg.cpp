// TextureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RasterizationRenderer.h"
#include "TextureDlg.h"
#include "afxdialogex.h"


// TextureDlg dialog

IMPLEMENT_DYNAMIC(TextureDlg, CDialog)

TextureDlg::TextureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TextureDlg::IDD, pParent)
	, m_nTexture(0)
	, m_nShaders(0)
{

}

TextureDlg::~TextureDlg()
{
}

void TextureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_NO_TEXTURE, m_nTexture);
	DDX_Radio(pDX, IDC_GOURAUD, m_nShaders);
}


BEGIN_MESSAGE_MAP(TextureDlg, CDialog)
END_MESSAGE_MAP()


// TextureDlg message handlers
