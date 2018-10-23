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
   // выходим из процесса, если не существует классификатора для обучения
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
// Реализация класса, создающего классификатор атрибутов.
TLFAttrBuilder::TLFAttrBuilder()
{
}
//------------------------------------------------------------------------------
TLFAttrBuilder::~TLFAttrBuilder()
{
}
//------------------------------------------------------------------------------
// Иницилизируется класс сборки детектора атрибутов.
// 1. файл параметров фильтра
// 2. база данных для обучения
// 3. критерий остановки.
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

