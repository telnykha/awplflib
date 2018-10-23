#ifndef _LF_AVG_FEATURE_
#define _LF_AVG_FEATURE_

#define FEATURE_AVG_SIZE 5 

class TLFAvgFeature : public ILFFeature
{
protected:
	int    m_CalcCount;
	double m_buffer[FEATURE_AVG_SIZE];
public:

	TLFAvgFeature();
	TLFAvgFeature(int sxbase, int sybase, int wbase, int hbase);
	TLFAvgFeature(TLFAvgFeature* sensor);
	virtual unsigned int     uCalcValue(TLFImage* pImage);
	virtual double           fCalcValue(TLFImage* pImage);


	/*
	XML io operations
	*/
	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool SaveXML(TiXmlElement* parent);

	virtual awpRect GetRect();
	void Clear();

	virtual const char* GetName()
	{
		return "TLFAvgFeature";
	}
};

class TLFSigmaFeature : public TLFAvgFeature
{
public:
	TLFSigmaFeature();
	TLFSigmaFeature(int sxbase, int sybase, int wbase, int hbase);
	TLFSigmaFeature(TLFSigmaFeature* feature);
	virtual double           fCalcValue(TLFImage* pImage);
	virtual const char* GetName()
	{
		return "TLFSigmaFeature";
	}
};

#endif //_LF_AVG_FEATURE_