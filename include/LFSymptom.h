#ifndef __LF_SYMPTOM_H__
#define __LF_SYMPTOM_H__
#include "LF.h"
#include "fvlcore.h"
#define NUM_FEATURES 9
const int  NUM_LINES     = NUM_FEATURES / 3;
class TSymptom;
class TLFBLOBObject : public TLFObject
{
private:
	// обработка времени для объекта
	float m_start_time;
	float m_current_time;
	float m_current_dt;
	float m_dL;
	// symptom class 
	TSymptom*   m_pSymptom;
	bool  m_on_border;
protected:
	int			m_area;
	awpRect		m_bounds;
	awpPoint	m_center;
	awpRect		m_search_area;
	float			m_age;
	float		m_data[NUM_FEATURES];
	bool		m_Leave;
	float         m_StayCount;
	int			m_id;
	bool		m_new_blob;
	void	    TrackOnBorder();
	void		UpdateBLOBState();
public:
	TLFBLOBObject (TSymptom* symptom);
	virtual ~TLFBLOBObject ();
	void Setup(int id, int area, awpRect r, awpPoint p, int age, int stay, bool Leave, bool is_new);
	int GetArea ();
	awpRect GetBounds();
	awpRect GetSearchArea();
	void SetSearchArea(float dL);
	awpPoint GetCenter();
	void TrackBLOB(awpImage* pImage);
	void GetFeatures(awpImage* pImage);
	bool Getleave();
	float GetAge();
	bool Alarm();
	int GetId();
	float GetStayCount();
	float GetDL();
	bool  IsNewBLOB();
};
enum TFGBGAlgorythm {
   fgbgSG = 0, /*Single Gauss*/ 
   fgbgPCA /*Principal Components Analysis*/
};
class TSymptom : public TLFObject
{
private:
	/*events
	*/
	void* m_pUserData;
	struct VariableParameres
	{
		awpRect roiRect;
		float min_blob_age;
		float min_blob_age_immov;
		float min_blob_aspect_ratio;
		float max_blob_aspect_ratio;
		float max_blob_square;
		float min_blob_square;
	}m_VariableParameters;
private:

	TLFImage m_background;
	TLFImage m_background2;
	TLFImage m_current;
	TLFImage m_diff;
	TLFImage m_binary;
	TLFImage m_binary_source;
	TLFImage m_diff1;
	TLFImage m_prev;
	TLFImage m_source_image;
	//обработка времени
	float    m_start_time;
	float    m_current_time;
	float    m_current_dt;
	bool	 m_first_time;

	//Задание максимального количества объектов для отслеживания
	int     m_max_num_obj;
	// Минимальная площадь BLOB
	int		m_MinArea;
	// Минимальная площадь, менее которой BLOB не более не сопровождается 
	int     m_MinAreaToLeave;
	// Максимальная площадь BLOB
	int		m_MaxArea;
	//счетчик кадров
	int m_NumFrames;

	// список найденных объектов 
	TLFObjectList m_BLOBs;

	// счетчик объектов
	int m_id_count;
	float m_min_blob_immoveable_age;
	float m_min_blob_age;
	float m_max_aspect_ratio;
	float m_min_aspect_ratio;
	float m_max_blob_square;
	float m_min_blob_square;
	
	awpRect		m_roiRect;
	awpRect		m_insideRoi;
	
	int			m_source_width;
	int			m_source_height;

	float       m_ResizeFactor;
	double      m_bgAvg;
    // параметры 
	TFGBGAlgorythm m_fgbg_algorytm;
	float       m_FGBGThreshold;
	int         m_NumFramesToTraining;

	int		m_DxMaxChange;
	int     m_DyMaxChange;
	int		m_SearchDelta;
protected:
	// fvl support 
	awpImage** m_ppSamples;
	FvcTemplate* m_pPCATemplate;
protected:
	virtual void Process();
	virtual void Analysis(awpImage* pImage);
	void ApplyMask(awpImage* pImage);
	double GetAverageRoi(awpImage* pImage);
	void AbsDiff();
public:
	TSymptom ();
	virtual ~TSymptom ();

	// функции возвращают результат работы алгоритма поиска объектов 
	int GetNumBlobs();
	TLFBLOBObject* GetBlob(int index);
	// функция обработки нового изображения 
	void SetImage(awpImage* pImage);
	void Reset();
	void ClearObjects();

	// 
	void BeginChangeParams();
	void RollBackChangeParams();
	void CommitChangeParams();
	// свойства объекта. функции установки свойств работают 
	// при значении переменной m_first_time = true;
	int GetmaxNumObj();
	void SetMaxNumObj(int NumObj);
	// промежуточные изображения 
	awpImage* GetBinary();
	awpImage* GetDiff();
	awpImage* GetBackground();
	awpImage* GetBinarySource();
	awpImage* GetSource();

	/* parameters
	*/
	int  GetSourceWidth();
	int  GetSourceHeight();
	
	awpRect GetRoiRect();
	awpRect GetInsideRoiRect();

	float GetMinBlobSquare();
	float GetMaxBlobSquare();

	float GetMinAspectRatio();
	float GetMaxAspectRatio();

	float GetMinBlobAge();
	float GetMinBlobImmoveableAge();

	void SetRoiRect(awpRect roiRect);
	
	void SetMinBlobSquare(float Value);
	void SetMaxBlobSquare(float Value);

	void SetMinAspectRatio(float Value);
	void SetMaxAspectRatio(float Value);

	void SetMinBlobAge(float Value);
	void SetMinBlobImmoveableAge(float Value);

	void* GetUserData();
	void  SetUserData(void* Value);

	int GetMinArea();
	int GetMaxArea();

	int GetMinAreaToLeave();

	TFGBGAlgorythm GetFGBGAlgorythm();
	void SetFGBGAlgorythm(TFGBGAlgorythm Value);

	float GetFGBGThreshold();
	void SetFGBGThreshold(float value);

	int GetNuFramesToTrainig();
	void SetNumFramesToTrainig(int value);

	int GetDxMaxChange();
	void SetDxMaxChange(int value);

	int GetDyMaxChange();
	void SetDyMaxChange(int value);
	
	int GetSearchDelta();
	void SetSearchDelta(int value);
};

#endif //__LF_SYMPTOM_H__