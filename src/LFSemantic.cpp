/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                  Locate Framework  Computer Vision Library
//
// Copyright (C) 2004-2017, Alt-Soft.net, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Alt-Soft.net or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//      Locate Framework  (LF) Computer Vision Library.
//		File: LFSemantic.cpp
//		Purpose: implements TLFSemanticDictinaryItem, TLFSemanticDictinary
//
//      CopyRight 2004-1017 (c) ALT-Soft.net
//M*/
#include "_LF.h"

//--------------------------element dictionary ----------------------------------
TLFSemanticDictinaryItem::TLFSemanticDictinaryItem()
{
	this->m_strLabel = "Unknown";
	this->m_scanner = new TLFScanner();
}

TLFSemanticDictinaryItem::TLFSemanticDictinaryItem(const char* lpWord)
{
	this->m_strLabel = lpWord;
    this->m_scanner = new TLFScanner();
}
TLFSemanticDictinaryItem::~TLFSemanticDictinaryItem()
{
	delete m_scanner;
}

const char* TLFSemanticDictinaryItem::GetItemLabel()
{
	return m_strLabel.c_str();
}
ILFScanner* TLFSemanticDictinaryItem::GetScanner()
{
	return m_scanner;
}

bool TLFSemanticDictinaryItem::SaveXML(const char* lpFileName)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* dscr = SaveXML();
	return doc.SaveFile(lpFileName);
}
////////////////////////////////////////////////////////////////////////////////
bool TLFSemanticDictinaryItem::LoadXML(const char* lpFileName)
{
	return false;
}
////////////////////////////////////////////////////////////////////////////////
TiXmlElement* TLFSemanticDictinaryItem::SaveXML()
{
	TiXmlElement* dscr = new TiXmlElement(this->GetName());
	dscr->SetAttribute("noun", this->m_strLabel.c_str());
	if (m_scanner != NULL)
	{
		TiXmlElement* scanner_dscr = m_scanner->SaveXML();
		dscr->LinkEndChild(scanner_dscr);
	}
	return dscr;
}
bool TLFSemanticDictinaryItem::LoadXML(TiXmlElement* parent)
{
	if (parent == NULL)
		return false;
	if (strcmp(parent->Value(), GetName()) != 0)
		return false;
    this->m_strLabel = parent->Attribute("noun");
    if (!this->m_scanner->LoadXML(parent->FirstChildElement()))
    	return false;
	return true;
}
//---------------------------семантической словарь-----------------------------
TLFSemanticDictinary::TLFSemanticDictinary()
{
	this->m_Description = "defult dictinary";
}
TLFSemanticDictinary::~TLFSemanticDictinary()
{
}

void TLFSemanticDictinary::AddWordToDictinary(TLFSemanticDictinaryItem* item)
{
	Add(item);
}
void TLFSemanticDictinary::DelWordForomDictinary(int index)
{
	if (index < 0 || index > GetCount())
		return;
	Delete(index);
}
void TLFSemanticDictinary::DelWordForomDictinary(const char* lpWord)
{
	for (int i = 0; i < this->GetDictinaryItemsCount(); i++)
	{
		TLFSemanticDictinaryItem* di = this->GetWordFromDictinary(i);
		if (strcmp(lpWord, di->GetItemLabel()) == 0)
		{
			Delete(i);
			break;
		};
	}
}
TLFSemanticDictinaryItem* TLFSemanticDictinary::GetWordFromDictinary(int index)
{
	if (index < 0 || index > GetCount())
		return NULL;
	return (TLFSemanticDictinaryItem*)Get(index);
}
TLFSemanticDictinaryItem* TLFSemanticDictinary::GetWordFromDictinary(const char* lpWord)
{
	for (int i = 0; i < this->GetDictinaryItemsCount(); i++)
	{
		TLFSemanticDictinaryItem* di = this->GetWordFromDictinary(i);
		if (strcmp(lpWord, di->GetItemLabel()) == 0)
			return di;
	}
	return NULL;
}

bool TLFSemanticDictinary::SaveXML(const char* lpFileName)
{

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);
	TiXmlElement* dscr = new TiXmlElement(this->GetName());
	doc.LinkEndChild(dscr);
	dscr->SetAttribute("Description", this->m_Description.c_str());
	for (int i = 0; i < this->GetCount(); i++)
	{
		TLFSemanticDictinaryItem* di = this->GetWordFromDictinary(i);
		TiXmlElement* e = di->SaveXML();
		if (e != NULL)
			dscr->LinkEndChild(e);
	}
	return doc.SaveFile(lpFileName);

}
bool TLFSemanticDictinary::LoadXML(const char* lpFileName)
{
	this->Clear();

	TiXmlDocument doc(lpFileName);
	if (!doc.LoadFile())
		return false;
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem = NULL;

	pElem = hDoc.FirstChildElement().Element();
	if (pElem == NULL)
		return false;

	if (strcmp(pElem->Value(), GetName()) != 0)
		return false;

	//int w, h;
	this->m_Description = pElem->Attribute("Description");


	for (TiXmlNode* child = pElem->FirstChild(); child; child = child->NextSibling())
	{
		TLFSemanticDictinaryItem* di = new TLFSemanticDictinaryItem();
		if (strcmp(child->Value(), di->GetName()) != 0)
		{
			Clear();
			return false;
		}
		if (!di->LoadXML(child->ToElement()))
		{
			Clear();
			return false;
		}
		this->Add(di);
	}

	return true;
}
int  TLFSemanticDictinary::GetDictinaryItemsCount()
{
	return this->GetCount();
}

void TLFSemanticDictinary::Clear()
{
	this->m_Description = "";
    inherited::Clear();
}
