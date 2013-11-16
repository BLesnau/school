// TextureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RasterizationRenderer.h"
#include "TextureDlg.h"
#include "afxdialogex.h"


// TextureDlg dialog

IMPLEMENT_DYNAMIC(TextureDlg, CDialog)

   BEGIN_MESSAGE_MAP(TextureDlg, CDialog)
      ON_BN_CLICKED(IDC_NO_TEXTURE, RadioClick)
      ON_BN_CLICKED(IDC_DEFAULT_TEXTURE, RadioClick)
      ON_BN_CLICKED(IDC_SPECIFIED_TEXTURE, RadioClick)
      ON_BN_CLICKED(IDC_SPECIFIED_TEXTURE2, RadioClick)
      ON_BN_CLICKED(IDC_SPECIFIED_TEXTURE3, RadioClick)
   END_MESSAGE_MAP()

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

void TextureDlg::RadioClick()
{
   if( ((CButton*)GetDlgItem(IDC_NO_TEXTURE))->GetCheck() == BST_CHECKED )
   {
      m_nTexture = 0;
   }

   if( ((CButton*)GetDlgItem(IDC_DEFAULT_TEXTURE))->GetCheck() == BST_CHECKED )
   {
      m_nTexture = 1;
   }

   if( ((CButton*)GetDlgItem(IDC_SPECIFIED_TEXTURE))->GetCheck() == BST_CHECKED )
   {
      m_nTexture = 2;
   }

   if( ((CButton*)GetDlgItem(IDC_SPECIFIED_TEXTURE2))->GetCheck() == BST_CHECKED )
   {
      m_nTexture = 3;
   }

   if( ((CButton*)GetDlgItem(IDC_SPECIFIED_TEXTURE3))->GetCheck() == BST_CHECKED )
   {
      m_nTexture = 4;
   }
}