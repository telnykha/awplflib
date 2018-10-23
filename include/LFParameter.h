#ifndef _LF_PARAMETER_H_
#define _LF_PARAMETER_H_

#define LF_MAX_NAME_LEN		128
#define LF_MAX_DESCR_LEN	256
class TLFParameter : public TLFObject
{
protected:
	char m_Name[LF_MAX_NAME_LEN];
	char m_Descr[LF_MAX_DESCR_LEN];
	double m_Value;
	double m_MaxValue;
	double m_MinValue;
public:
	TLFParameter();
	virtual ~TLFParameter();

	char* GetPName();
	char* GetDescription();
	double GetValue();
	double GetMaxValue();
	double GetMinValue();
	void  SetPName(const char* Value);
	void  SetDescription(const char* Value);
	bool  SetValue(double Value);
	bool  SetMaxValue(double Value);
	bool  SetMinValue(double Value);

	bool LoadXML(TiXmlElement* parent);
	TiXmlElement* SaveXML();

    virtual const char* GetName()
    {
        return "TLFParameter";
    }
};
#endif //_LF_PARAMETER_H_