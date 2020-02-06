#ifndef _LF_IMAGE_PROCESSOR_H_
#define _LF_IMAGE_PROCESSOR_H_

/** \defgroup LFImagePorcessor
*	Interface of an abstract image processor 
*   @{
*/

/* ���������� ��������� ����������� ��������� ����������� 
*/
class ILFImageProcessor : public TLFObject
{
protected:
	/*���������� ��� ����������� 
	*/
	TLFImage m_SourceImage;
	TLFImage m_ResultImage;
	/*������� ��������� ����������� 
	*/
	TLFRect  m_ImageRoi;
	/* ������ ����������
	*/
	TLFObjectList m_Params;
	virtual bool Process() = 0;
public:
	ILFImageProcessor();
	virtual ~ILFImageProcessor();
	/* ����������� 
	*/
	virtual bool SetSourceImage(TLFImage* pImage);
	virtual TLFImage* GetSourceImage();
	virtual TLFImage* GetResultImage();
	/* ���������
	*/
	int GetNumParameters();
	TLFParameter* GetParameter(int index);
	bool SetParameterValue(double Value, int index);
	/* ������� ��������� 
	*/
	TLFRect GetRoi();
	void SetRoi(TLFRect& Rect);
};
/** @} */ /*  end of LFHistogramm group */
#endif //_LF_IMAGE_PROCESSOR_H_