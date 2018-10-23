#ifndef LFSabotageH
#define LFSabotageH
// forwrd 
#define SABOTAGE_BUF_SIZE 12 
class TLFSabotageWeak : public ILFWeak
{
protected:
	int		m_numFramesToAvg;
	int		m_bufSize;
	double* m_story_buffer;
	double  m_sens; 
	double  m_minValue;
	double  m_maxValue;
	double  m_diffValue;
	int     m_classifyCount;
	int		m_state;
    AWPDWORD   m_dt[SABOTAGE_BUF_SIZE];
    AWPDWORD   m_ct;
	int	    m_bf; // number frame in the buffer 
public:
	TLFSabotageWeak(ILFFeature* feature, double sens);
	virtual ~TLFSabotageWeak();
	/*
		Классифицирует участок изображения, связанный с TLFAvgFeature
		0 - все ок
		1 - слишком темно
		2 - слишком светло 
		3 - резкие изменения 
	*/
	virtual int Classify(TLFImage* pImage,  double* value = NULL);

	virtual void SaveXML(TiXmlElement* parent);
	virtual bool LoadXML(TiXmlElement* parent);

	void Clear();
	int  GetState();

	virtual const char* GetName()
	{
		return "TLFSabotageWeak";
	}
};
class TLFSabotageStrong : public ILFStrong
{
protected:
	
public: 
	/*
	XML io operations
	*/
	virtual void SaveXML(TiXmlElement* parent);
	virtual bool LoadXML(TiXmlElement* parent);
	/*
	Classification
	0 - object present
	1 - object absence
	*/
	virtual int Classify(TLFImage* pImage, double& err, double* vct = NULL);
	void Clear();
};

//
class TLFSabotage : public TLFObject
{
protected:
	TLFSabotageStrong m_classifyer;
	TLFImage*		  m_CurrentImage;
	int				  m_state;
	AWPWORD			  m_srcWidth;
	AWPWORD			  m_srcHeight;
	AWPBYTE			  m_srcBPP;

    AWPDWORD			  m_event_duration;
    AWPDWORD			  m_event_start;

	double			  m_time_thr;

	void Clear();
	void Init(TLFImage* img);

	bool Preprocess(awpImage* img);

	
public:
	TLFSabotage();
	TLFSabotage(double s, double t);
	virtual ~TLFSabotage();

	bool PorocessImage(awpImage* img, bool* result);
	int GetState();
	virtual const char* GetName()
	{
		return "TLFSabotage";
	}
};

#endif 
