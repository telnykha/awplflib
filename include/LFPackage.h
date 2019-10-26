#ifndef LFPackageH
#define LFPackageH

class TLFPackageEngine : public ILFDetectEngine
{
protected: 
#ifdef _DEBUG
	FILE* m_logFile;
	std::string m_strFileName;
#endif 
protected:

	float m_minWidth;
	float m_maxWidth;
	float m_minHeight;
	float m_maxHeight;

	TLFObjectList 		m_tmpList;
    TLFImage 			m_foreground;

	virtual void OverlapsFilter(TLFSemanticImageDescriptor* descriptor);
	virtual void InitDetectors();
	void    BuildForeground();

	TLFClusterRecursive* m_cluster_maker;
public:
	TLFPackageEngine();
	virtual ~TLFPackageEngine();
	virtual void Clear();
	virtual bool LoadXML(TiXmlElement* parent);
	virtual bool FindObjects();
	virtual TiXmlElement*  SaveXML();
	virtual bool DetectInRect(awpRect* rect);

	int GetDelay();
	void SetDelay(int value);

	awpImage* GetForeground();
	TLFImage* GetForegroundImage();

	float GetMinWidth();
	void SetMinWidth(float value);

	float GetMinHeight();
	void SetMinHeight(float value);

	float GetMaxWidth();
	void SetMaxWidth(float value);

	void SetAverageBufferSize(int value);
	int GetAverageBufferSize();

	void SetBGStability(int value);
	int GetBGStability();
	
	float GetMaxHeight();
	void  SetMaxHeight(float value);
	virtual const char* GetName()
	{
		return "TLFPackageEngine";
	}
};

#endif //LFPackageH