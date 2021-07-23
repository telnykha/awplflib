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
// Copyright (C) 2004-2018, NN-Videolab.net, all rights reserved.
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
// In no event shall the NN-Videolab.net or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//      Locate Framework  (LF) Computer Vision Library.
//		File: LFLandmarks.h
//		Purpose: Contains landmarks classes
//
//      CopyRight 2004-2021 (c) NN-Videolab.net
//M*/
#ifndef _lf_landmarks_h_
#define _lf_landmarks_h_

/** \defgroup LFLandmarks
*	Implementation LFLandmark
*   @{
*/

class TLFDBLandmarks;
class TLFLandmark : public TLFObject
{
protected:
	awp2DPoint m_landmark;
	UUID			m_id;
public:
	TLFLandmark(UUID id, awp2DPoint point);
	virtual ~TLFLandmark();
	virtual const char* GetName(){ return "TLFLandmark"; }

	TiXmlElement* SaveXML();
	bool LoadXML(TiXmlElement* parent);
	
	void		GetId(UUID& id);
	awp2DPoint _landmark();
	AWPDOUBLE  _x();
	AWPDOUBLE  _y();
};
class TLFLandmarkAttr : public TLFObject
{
protected:
	TLFString m_ClassName;
	UUID			m_id;
public:
	TLFLandmarkAttr();
	virtual ~TLFLandmarkAttr();
	virtual const char* GetName(){ return "TLFLandmarkAttr"; }
};
class TLFLandmarkAttributes : public TLFObject
{
protected:
	TLFDBLandmarks* m_database;
	TLFObjectList	m_attributes;
public: 
	TLFLandmarkAttributes();
	virtual ~TLFLandmarkAttributes();
	virtual const char* GetName(){ return "TLFLandmarkAttributes"; }

	void Clear();
	void Append(TLFLandmarkAttr* attr);
	TLFLandmarkAttr* Attribute(int index);
	void Delete(int index);
	int Count();

	TLFObjectList* GetList();
};
class TLFLandmarksFile : public TLFObjectList
{
protected:
public:
};

class TLFDBLandmarks : public TLFObjectList
{
private: 
	TLFString m_fileName;
	TLFLandmarkAttributes m_attributes;
protected:
	void SetAttributes(TLFLandmarkAttributes& attributes);
	void SetFileName(const char* fileName);
	bool Save();
	bool Load(const char* fileName);
public:
	TLFDBLandmarks();
	virtual ~TLFDBLandmarks();
	virtual const char* GetName(){ return "TLFDBLandmarks"; }

	bool Connect(const char* fileName);
	void Close();
	TLFLandmarkAttributes* 


	static TLFDBLandmarks* CreateDatabase(TLFLandmarkAttributes& attributes, const char* fileName);
};
/** @} */ /*  end of LFBuffers group */
#endif //_lf_landmarks_h_