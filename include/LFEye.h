//---------------------------------------------------------------------------

#ifndef LFEyeH
#define LFEyeH
//---------------------------------------------------------------------------
// ��������� ������, ������� ��������� ��������� �������������� ����
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
   // ��������� ��������� ������������� ���� [roi] �� ����������� [image]
   // � ������ ������ ���������� NULL
   virtual TLFRoi* Precise(TLFImage& image, TLFRoi& roi, LFPreciseMethod Method = LFAverage);
};
//----------------------------------------------------------------------------
//���������� ILFRoi, ������� ���������� ������������� �������� ���������
class TLFEyeCorrelation : public ILFEye
{
private:

   awpImage*  m_pLeftEye;        // ��������� ����������� ������ �����
   awpImage*  m_pRightEye;       // ��������� ����������� ������� �����

   double      m_fMathLeft;       // �������������� �������� ��� ������� ������ �����
   double      m_fDispLeft;       // ������ �� ��������� ��� ������ �����

   double      m_fMathRight;      // �������������� �������� ��� ������� ������� �����
   double      m_fDispRight;      // ������ �� ��������� ��� ������� �����


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
