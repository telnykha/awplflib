#include "_LF.h"


TLFLandmarkAttributes::TLFLandmarkAttributes()
{

}
TLFLandmarkAttributes::~TLFLandmarkAttributes()
{

}

void TLFLandmarkAttributes::Clear()
{
	this->m_attributes.Clear();
}
void TLFLandmarkAttributes::Append(TLFLandmarkAttr* attr)
{
	this->m_attributes.Add(attr);
}
void TLFLandmarkAttributes::Delete(int index)
{
	
}

TLFLandmarkAttr* TLFLandmarkAttributes::Attribute(int index)
{
	return dynamic_cast<TLFLandmarkAttr*>(m_attributes.Get(index));
}
int TLFLandmarkAttributes::Count()
{
	return this->m_attributes.GetCount();
}
TLFObjectList* TLFLandmarkAttributes::GetList()
{
	return &m_attributes;
}


TLFDBLandmarks::TLFDBLandmarks()
{
	m_fileName = "";
}
TLFDBLandmarks::~TLFDBLandmarks()
{

}
void TLFDBLandmarks::SetAttributes(TLFLandmarkAttributes& attributes)
{
	
}

TLFDBLandmarks* TLFDBLandmarks::CreateDatabase(TLFLandmarkAttributes& attributes, const char* fileName)
{
	TLFDBLandmarks* landmarks = new TLFDBLandmarks();
	landmarks->SetAttributes(attributes);
	landmarks->SetFileName(fileName);
	if (landmarks->Save() && landmarks->Connect(fileName))
		return landmarks;
	else
	{
		delete landmarks;
		return NULL;
	}
}

bool TLFDBLandmarks::Connect(const char* fileName)
{
	bool res = false;
	if (m_fileName != "")
		Close();
	if (res = Load(fileName))
		m_fileName = fileName;
	return res;
}
void TLFDBLandmarks::Close()
{
	m_attributes.Clear();
	m_fileName = "";
}

void TLFDBLandmarks::SetFileName(const char* fileName)
{
	m_fileName = fileName;
}
bool TLFDBLandmarks::Save()
{
	return false;
}
bool TLFDBLandmarks::Load(const char* fileName)
{
	return false;
}

