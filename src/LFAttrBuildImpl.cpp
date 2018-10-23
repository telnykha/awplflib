#include "_LF.h"
#pragma hdrstop
//------------------------------------------------------------------------------
IAttrBuilder::IAttrBuilder()
{
   m_pClassifier = NULL;
}
//------------------------------------------------------------------------------
IAttrBuilder::~IAttrBuilder()
{
    if (m_pClassifier != NULL)
        delete m_pClassifier;
}
//------------------------------------------------------------------------------
bool IAttrBuilder::Build()
{
   // ������� �� ��������, ���� �� ���������� �������������� ��� ��������
   if (m_pClassifier == NULL)
        return false;
   IAttrTrainer* trainer = m_pClassifier->GetTrainer();
   if (trainer == NULL)
        return false;
   LFTermCriteria c;
   c = trainer->EstimateErrors();
   int status = 0;
   while (status = CompareTermCriteria(m_TermCriteria, c) == LF_TERM_CONTINUE)
   {
        trainer->UpdateClassifier();
        c = trainer->EstimateErrors();
   }
   if (status == LF_TERM_BREAK)
           return true;
   else
        return false;
}

//------------------------------------------------------------------------------
// ���������� ������, ���������� ������������� ���������.
TLFAttrBuilder::TLFAttrBuilder()
{
}
//------------------------------------------------------------------------------
TLFAttrBuilder::~TLFAttrBuilder()
{
}
//------------------------------------------------------------------------------
// ��������������� ����� ������ ��������� ���������.
// 1. ���� ���������� �������
// 2. ���� ������ ��� ��������
// 3. �������� ���������.
bool TLFAttrBuilder::Init(const char* lpFileName)
{
   TiXmlDocument doc(lpFileName);
   if (!doc.LoadFile())
   	return false;
   TiXmlHandle hDoc(&doc);
   TiXmlElement* pElem = NULL;
   TiXmlHandle hRoot(0);
   pElem = hDoc.FirstChildElement().Element();
   if (!pElem)
       return false;

   

   return true;
}

