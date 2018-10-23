#ifndef _LF_THRESHOLD_PROC_
#define _LF_THRESHOLD_PROC_

typedef struct tagSLFBinaryMoments
{
	double area;
	double cx;
	double cy;
	double mxx;
	double mxy;
	double myy;
	double major;
	double minor;
	double angle;
	awp2DPoint major_p1;
	awp2DPoint major_p2;
	awp2DPoint minor_p1;
	awp2DPoint minor_p2;
}SLFBinaryMoments;

double GetMinMomnetsX(SLFBinaryMoments& m);
double GetMaxMomnetsX(SLFBinaryMoments& m);
double GetMinMomnetsY(SLFBinaryMoments& m);
double GetMaxMomnetsY(SLFBinaryMoments& m);


/* ����������� ����������� �� ������������
   � ��������.
   ������� ����������� ������ ���� ���� AWP_BYTE
   � ����� ���� �����.
   ��������� ��������������:
   �������� ������ [0..255], �� ��������� ��������������� - 128
*/
class TLFThresholdProc : public ILFImageProcessor
{
protected:
	virtual bool Process();
public:
	TLFThresholdProc ();
	virtual ~TLFThresholdProc ();

	bool ProcessMoments(TLFRect& rect, SLFBinaryMoments& moments);
};
#endif //_LF_THRESHOLD_PROC_