#ifndef _LF_ATTRBUILDERIMPL_H_
#define _LF_ATTRBUILDERIMPL_H_
/*
        �����, ����������� ��������� IAttrBuilder
        ��������������� ��� ���������� ��������������� ��������� �����������
        ����
*/
class TLFAttrBuilder : public IAttrBuilder
{
protected:

public:
        TLFAttrBuilder();
        virtual ~TLFAttrBuilder();
        virtual bool Init(const char* lpFileName);
};

#endif //_LF_ATTRBUILDERIMPL_H_
