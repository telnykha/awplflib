//---------------------------------------------------------------------------

#ifndef LFEyeH
#define LFEyeH
//---------------------------------------------------------------------------
// интерфейс класса, который выполняет уточнение местоположения глаз
enum LFEyes {LFLeftEye, LFRightEye};
enum LFPreciseMethod {LFAverage, LFExtremum, LFMean};
class ILFEye : public TLFObject
{
protected:
   bool m_IsInitialized;
   virtual bool CheckEye(awpRect& r, awpImage* pImage, double& err, LFEyes eye) = 0;
public:
   ILFEye(){m_IsInitialized = false;};
   virtual ~ILFEye(){};

   virtual bool Init(const char* lpInitString)= 0;
   virtual bool SetImage(awpImage* pImage) = 0;
   // выполняет уточнение метоположения глаз [roi] на изображении [image]
   // в случае ошибки возвращает NULL
   virtual TLFRoi* Precise(TLFImage& image, TLFRoi& roi, LFPreciseMethod Method = LFAverage);
};
//----------------------------------------------------------------------------
//реализация ILFRoi, который использует корреляцинный алгоритм сравнения
class TLFEyeCorrelation : public ILFEye
{
private:

   awpImage*  m_pLeftEye;        // эталонное изображение левого глаза
   awpImage*  m_pRightEye;       // эталонное изображение правого глаза

   double      m_fMathLeft;       // математическое ожидание для эталона левого глаза
   double      m_fDispLeft;       // корень из дисперсии для левого глаза

   double      m_fMathRight;      // математическое ожидание для эталона правого глаза
   double      m_fDispRight;      // корень из дисперсии для правого глаза


   double Correlation(double* TestVct1, LFEyes eye);

protected:
   virtual bool CheckEye(awpRect& r, awpImage* pImage, double& err, LFEyes eye);
public:
   TLFEyeCorrelation();
   virtual ~TLFEyeCorrelation();
   virtual bool Init(const char* lpInitString);
   virtual bool SetImage(awpImage* pImage){return true;};
};

#endif
