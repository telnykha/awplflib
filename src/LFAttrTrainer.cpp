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
// �������� �������� ����������� �� ���� ������
bool TLFAttrTrainer::InitDatabase()
{
  return false;
}



