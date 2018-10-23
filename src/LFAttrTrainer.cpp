#include "_LF.h"
#pragma hdrstop
TLFAttrTrainer::TLFAttrTrainer():IAttrTrainer()
{

}

TLFAttrTrainer::~TLFAttrTrainer()
{

}
void TLFAttrTrainer::UpdateClassifier()
{

}

LFTermCriteria TLFAttrTrainer::EstimateErrors()
{
  LFTermCriteria tc;
  memset(&tc, 0, sizeof(tc));
  return tc;
}
// загрузка образцов изображений из базы данных
bool TLFAttrTrainer::InitDatabase()
{
  return false;
}



