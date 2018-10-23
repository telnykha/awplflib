#ifndef _LF_ATTR_TRAINER_
#define _LF_ATTR_TRAINER_
#include "LF.h"
/*
	класс, предназначенный для тренировки TAttrClassifier
    реализует обучения одного эксперта типа TCSSeparate

*/
class TLFAttrTrainer : public IAttrTrainer
{
private:
class TImageSample :public TLFImage
{
protected:
	int m_Flag; // Тип образца. 1 - Positive 0 - negative
    double m_Weight; // вес образца
    bool  m_IsProcessed; //
public:

};


protected:
  // функция инициализирует внутренние структуры тренера из базы данных
  // типа LFDatabase
  virtual bool InitDatabase();
public:
  TLFAttrTrainer();
  TLFAttrTrainer(ILFAttrClassifier* classifier):IAttrTrainer(classifier){}
  virtual ~TLFAttrTrainer();
  virtual void UpdateClassifier();
  virtual LFTermCriteria EstimateErrors();
};
#endif//_LF_ATTR_TRAINER_
