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
//		File: LFSemantic.h
//		Purpose: contains semantic dictonary description 
//
//      CopyRight 2004-1017 (c) ALT-Soft.net
//M*/
#ifndef _lf_semantic_h_
#define _lf_semantic_h_

/**
\brief Elemnet of semantic dictionary, contains: class label, 
\ aperture and scanner. 
*/
class TLFSemanticDictinaryItem : public TLFObject
{
protected:
	ILFScanner*		   m_scanner;
	std::string        m_strLabel;
public:
	TLFSemanticDictinaryItem();
	TLFSemanticDictinaryItem(const char* lpWord);
	virtual ~TLFSemanticDictinaryItem();

	const char* GetItemLabel();
	ILFScanner* GetScanner();
	virtual bool SaveXML(const char* lpFileName);
	virtual bool LoadXML(const char* lpFileName);

	virtual TiXmlElement* SaveXML();
	virtual bool LoadXML(TiXmlElement* parent);


	virtual const char* GetName()
	{
		return "TLFSemanticDictinaryItem";
	}
};
/**
\brief Semantic dictionary, contains list of TLFSemanticDictinaryItem
\ 
*/
class TLFSemanticDictinary : public TLFObjectList
{
	typedef TLFObjectList inherited;
protected:
	std::string        m_Description;
public:
	TLFSemanticDictinary();
	virtual ~TLFSemanticDictinary();

	void AddWordToDictinary(TLFSemanticDictinaryItem* item);
	void DelWordForomDictinary(int index);
	void DelWordForomDictinary(const char* lpWord);
	TLFSemanticDictinaryItem* GetWordFromDictinary(int index);
	TLFSemanticDictinaryItem* GetWordFromDictinary(const char* lpWord);
	int  GetDictinaryItemsCount();

	virtual bool SaveXML(const char* lpFileName);
	virtual bool LoadXML(const char* lpFileName);

	virtual void Clear();

	virtual const char* GetName()
	{
		return "TLFSemanticDictinary";
	}
};

#endif //_lf_semantic_h_