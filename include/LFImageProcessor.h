#ifndef _LF_IMAGE_PROCESSOR_H_
#define _LF_IMAGE_PROCESSOR_H_

/** \defgroup LFImagePorcessor
*	Interface of an abstract image processor 
*   @{
*/

/* Обобщенный интерфейс выполняющий обработку изображения 
*/
class ILFImageProcessor : public TLFObject
{
protected:
	/*контейнеры для изображений 
	*/
	TLFImage m_SourceImage;
	TLFImage m_ResultImage;
	/*область обработки изображения 
	*/
	TLFRect  m_ImageRoi;
	/* вектор параметров
	*/
	TLFObjectList m_Params;
	virtual bool Process() = 0;
public:
	ILFImageProcessor();
	virtual ~ILFImageProcessor();
	/* изображения 
	*/
	virtual bool SetSourceImage(TLFImage* pImage);
	virtual TLFImage* GetSourceImage();
	virtual TLFImage* GetResultImage();
	/* параметры
	*/
	int GetNumParameters();
	TLFParameter* GetParameter(int index);
	bool SetParameterValue(double Value, int index);
	/* область обработки 
	*/
	TLFRect GetRoi();
	void SetRoi(TLFRect& Rect);
};
/** @} */ /*  end of LFHistogramm group */
#endif //_LF_IMAGE_PROCESSOR_H_