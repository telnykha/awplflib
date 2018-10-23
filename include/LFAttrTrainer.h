#ifndef _LF_ATTR_TRAINER_
#define _LF_ATTR_TRAINER_
#include "LF.h"
/*
	�����, ��������������� ��� ���������� TAttrClassifier
    ��������� �������� ������ �������� ���� TCSSeparate

*/
class TLFAttrTrainer : public IAttrTrainer
{
private:
class TImageSample :public TLFImage
{
protected:
	int m_Flag; // ��� �������. 1 - Positive 0 - negative
    double m_Weight; // ��� �������
    bool  m_IsProcessed; //
public:

};


protected:
  // ������� �������������� ���������� ��������� ������� �� ���� ������
  // ���� LFDatabase
  virtual bool InitDatabase();
public:
  TLFAttrTrainer();
  TLFAttrTrainer(ILFAttrClassifier* classifier):IAttrTrainer(classifier){}
  virtual ~TLFAttrTrainer();
  virtual void UpdateClassifier();
  virtual LFTermCriteria EstimateErrors();
};
#endif//_LF_ATTR_TRAINER_
