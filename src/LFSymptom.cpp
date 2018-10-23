#include "_LF.h"
const int   c_nMaxNumTrainig = 100;
const float c_flProcessingArea = 76800.f; // число обрабатываемых пикселей. 
const float c_flMinT = 2.5 ;  // объект может пересечь всю область наблюдения за c_flMinT секунд
/*
	aspect w/h для человека:
	0.25 - 0.75
	для сумки:
	0.5  - 2
*/
// static helpers 
// заполнение изображения нулями
// предполагается, что изображение имеет 1 канал и тип AWPBYTE
void _awpZeroImage(awpImage* src)
{
	AWPBYTE* s1 = (AWPBYTE*)src->pPixels;
	memset(s1, 0, src->sSizeX*src->sSizeY);
}
// пороговая обработка
 void _awpAdaptiveThreshold(awpImage* src, awpImage* dst)
{
	//todo: вычисление порога бинаризации, на основе гистограммы изображения
	AWPBYTE thr = 30; // порог бинаризации по умолчанию. 
	AWPBYTE* s = (AWPBYTE*)src->pPixels;
	AWPBYTE* d = (AWPBYTE*)dst->pPixels;
	for (int i = 0; i < src->sSizeX*src->sSizeY; i++)
	{
		if (s[i] > thr) 
			d[i] = 255;
		else
			d[i] = 0;
	}
}
// удалят шум типа "соль" 
 void _awpNoiseRemove(awpImage* src)
 {
	 AWPBYTE* s = (AWPBYTE*)src->pPixels;
	 for (int i = 1; i < src->sSizeY - 1; i++)
	 {
		 for (int j = 1; j < src->sSizeX - 1; j++)
		 {
			 if ((s[i*src->sSizeX + j] == 0 && s[i*src->sSizeX + j + 1] > 0 && 
				 s[i*src->sSizeX + j -1] > 0) || 
				 (s[(i-1)*src->sSizeX + j] > 0 && 
				 s[(i+1)*src->sSizeX + j] > 0))
			 {
				 s[i*src->sSizeX + j]= 255;
				 continue;
			 }

			 if ((s[i*src->sSizeX + j] > 0 && s[i*src->sSizeX + j + 1] == 0 && 
				 s[i*src->sSizeX + j -1] == 0) || 
				 (s[(i-1)*src->sSizeX + j] == 0 && 
				 s[(i+1)*src->sSizeX + j] == 0))
			 {
				 s[i*src->sSizeX + j]= 0;
				 continue;
			 }


		 }
	 }
 }

static inline double CalcSum00(double* pix, int x, int y, int w, int h, int ww )
{
    double* p = pix + x - 1 + (y - 1)*ww;

    h = h*ww;

    return (p[0] + p[w + h] - p[h] - p[w]);
}


static void CalcFeatures(awpImage* pImage, awpRect r, float* features)
{
	memset(features, 0, NUM_FEATURES*sizeof(float));
	int ww = pImage->sSizeX;
	int w = (r.right - r.left) / NUM_LINES;
	int h = (r.bottom - r.top) / NUM_LINES;
	int x = r.left;
	int y = r.top;
	double* dd = (double*)pImage->pPixels;
	double area = w*h;
	int c = 0;
	for (int i = 0; i < NUM_LINES; i++)
	{
		y = r.top + i*h;
		for (int j = 0; j < NUM_LINES; j++)
		{
			x = r.left + j*w;
			features[c] = CalcSum00(dd,x,y,w,h,ww); 
			features[c] /= area;
			c++;
		}
	}
}
static double CalcFeature(awpImage* pImage, awpRect r)
{
	double res = 0;
	int ww = pImage->sSizeX;
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	double* dd = (double*)pImage->pPixels;
	double  area = w*h;
	res  = CalcSum00(dd, r.left, r.top, w, h, pImage->sSizeX);
	res /= area;
	return res;
}
static float CompareFeatures(float* f1, float* f2)
{
	float res = 0;
	for (int i = 0; i < NUM_FEATURES; i++)
	{
		res += (f1[i] - f2[i])*(f1[i] - f2[i]);
	}
	return sqrt(res) / NUM_FEATURES;
}


TLFBLOBObject::TLFBLOBObject (TSymptom* symptom)
{
//	TLFBLOBObject();

	m_area = 0;
	m_bounds.left = 0;
	m_bounds.right = 0;
	m_bounds.bottom = 0;
	m_bounds.top = 0;
	m_center.X = 0;
	m_center.Y = 0;
	m_age = 0;
	m_Leave = false;
	m_StayCount = 0;
	m_id = 0;
	memset(m_data, 0, sizeof(m_data));

	m_start_time = clock() / CLOCKS_PER_SEC; 
	m_current_time = m_start_time;
	m_current_dt = 0;
	m_on_border = false;
	m_dL = 0; 
	m_pSymptom = symptom;
	m_new_blob = true;
}
TLFBLOBObject::~TLFBLOBObject () {}
void TLFBLOBObject::Setup(int id, int area, awpRect r, awpPoint p, int age, int stay, bool Leave, bool is_new)
{
	m_id = id;
	m_area = area;
	m_bounds = r;
	m_center = p;
	m_center.X = (r.left + r.right)/2;
	m_center.Y = (r.top + r.bottom) /2;
	m_age = age;
	m_Leave = Leave;
	m_StayCount = stay;
	m_new_blob = is_new;
}
int TLFBLOBObject::GetArea ()
{
	return m_area;
}
awpRect TLFBLOBObject::GetBounds()
{
	return m_bounds;
}

awpRect TLFBLOBObject::GetSearchArea()
{
	return m_search_area;
}
void TLFBLOBObject::SetSearchArea(float dL)
{
	if (m_pSymptom == NULL)
		return;
	m_dL = dL;
	awpRect r; 
	r.left = m_bounds.left - m_dL;
	r.top = m_bounds.top - m_dL;
	r.right = m_bounds.right + m_dL;
	r.bottom = m_bounds.bottom + m_dL;
	awpRect r1 = m_pSymptom->GetInsideRoiRect();
	bool on_border = false;
	if (r.left <= r1.left) 
	{
		r.left = r1.left;
		on_border = true;
	}
	if (r.top <= r1.top) 
	{
		r.top = r1.top;
		on_border = true;
	}
	if (r.right > r1.right) 
	{
		r.right = r1.right;
		on_border = true;
	}
	if (r.bottom > r1.bottom) 
	{
		r.bottom = r1.bottom;
		on_border = true;
	}
	m_on_border = on_border;
	m_search_area = r;
}
float TLFBLOBObject::GetDL()
{
	return m_dL;
}

awpPoint TLFBLOBObject::GetCenter()
{
	return m_center;
}

bool TLFBLOBObject::Getleave()
{
	return m_Leave;
}
float TLFBLOBObject::GetAge()
{
	return m_age; 
}
bool TLFBLOBObject::Alarm()
{
	return m_StayCount > 240;
}
int TLFBLOBObject::GetId()
{
	return m_id;
}
float TLFBLOBObject::GetStayCount()
{
	return m_StayCount;
}
// вычисление прямоугольника ограничивающего массив строковых объектов. 
void awpCalcObjectsRect(awpStrokeObj* str, int num, awpRect* r)
{
	int minLeft = 1000;
	int minTop  = 1000;
	int maxRight = 0;
	int maxBottom = 0;
	
	for (int i = 0; i < num; i++)
	{
		awpStrokeObj* so = &str[i];
		awpRect rr;
		awpCalcObjRect(so, &rr);
		if (rr.left < minLeft)
			minLeft = rr.left;
		if (rr.right > maxRight)
			maxRight = rr.right;
		if (rr.top < minTop)
			minTop = rr.top;
		if (rr.bottom > maxBottom)
			maxBottom = rr.bottom;
	}
	r->bottom = maxBottom;
	r->left = minLeft;
	r->top = minTop;
	r->right = maxRight;
}


void TLFBLOBObject::TrackOnBorder()
{
	//
	awpImage* pImage = m_pSymptom->GetBinary();
	int num = 0;
	awpStrokeObj* str = NULL;
	awpGetStrokes(pImage, &num, &str, 10, &m_search_area);
	if (num == 0)
	{
		m_Leave = true;
		goto cleanup;
	}
	// найдем объект с максимальной площадью. 
	int max_square = 0, index = -1;
	int sum_square = 0;
	awpRect r;
	for (int i = 0; i < num; i++)
	{
		awpStrokeObj* so = &str[i];
		int s = 0;
		awpStrObjSquare(so, &s);
		sum_square += s;
		if (s > max_square)
		{
			max_square = s;
			index = i;
		}
	}
	if (index == -1)
	{
		m_Leave = true;
		goto cleanup;
	}
	// проверим на допустимость параметров площадь объекта 
	awpStrokeObj* TheObject = &str[index];
	if (max_square >= m_pSymptom->GetMinAreaToLeave())
	{
		awpRect r;
		awpPoint p;
		awpCalcObjRect(TheObject, &r);
		//awpCalcObjectsRect(str, num, &m_bounds);
		//awpGetObjCentroid(pImage, TheObject, &p);

		float lx = this->m_bounds.right - this->m_bounds.left;
		float ly = this->m_bounds.bottom - this->m_bounds.top;
		float w = r.right - r.left;
		float h = r.bottom - r.top;
		float dx = 100.f*fabs(lx - w)/ lx;
		float dy = 100.f*fabs(ly - h) /ly;
		p.X = (r.left + r.right)/2;
		p.Y = (r.bottom + r.top) /2;
		
		if (dx < m_pSymptom->GetDxMaxChange() && dy < m_pSymptom->GetDyMaxChange())
		{
			this->m_bounds = r;
			this->m_center = p;
		}
		else
		{
			//this->m_center = p;
			//this->m_bounds.left = p.X - lx / 2;
			//this->m_bounds.right = p.X + lx /2;
			//this->m_bounds.top = p.Y - ly / 2;
			//this->m_bounds.bottom = p.Y + ly / 2;
		}

		//float d = sqrt(lx*lx+ly*ly);
		//lx = this->m_center.X - p.X;
		//ly = this->m_center.Y - p.Y;
		//float dc = sqrt(lx*lx+ly*ly);
		//if (dc < d / 5) 
		//{
		//	this->m_bounds = r;
		//	this->m_center = p;
		//}
	}
	else
		m_Leave = true;
	// удалим объект с бинарного изображения
	awpDrawStrokeObj(pImage, TheObject, 0);
	// обновим объект
	UpdateBLOBState();
cleanup:
	if (str != NULL)
		awpFreeStrokes(num, &str);
}


// находит новоое местоположение объекта на изображении
// pImage
// формат изображения pImage - AWP_DOUBLE, nChannels = 1
// содержание - интегральное изображение, полученное из исходного
void TLFBLOBObject::TrackBLOB(awpImage* pImage)
{
	if (m_pSymptom == NULL)
		return;
	m_new_blob = false;
	// обработка времени. 
	float t = clock() / (float)CLOCKS_PER_SEC;
	m_current_dt = t - m_current_time;
	m_current_time = t;

	float f[NUM_FEATURES];
	memset(f, 0, sizeof(f));
	awpRect r; r = m_bounds;
	int w = m_bounds.right - m_bounds.left; 
	int h = m_bounds.bottom - m_bounds.top;
	int mx;
	int my;
	double mf =10000000000000;
	double d = 10000000000000;
	//m_dL = m_pSymptom->GetBinary()->sSizeX*m_current_dt / c_flMinT;
	m_dL = (float)m_pSymptom->GetSearchDelta()*w/100.f;
	//if (m_current_dt > c_flMinT*w/m_pSymptom->GetBinary()->sSizeX)
	//{
	//	// установить максимальную область наблюдения
	//	m_dL = w / 2;		
	//}
	SetSearchArea(m_dL);
	if (true)
	{
		// объект соприкасается с границей roi	
		TrackOnBorder();
		return;
	}
	else if (false)
	{
		for (int y = m_search_area.top; y < m_search_area.top + 2*m_dL ; y++)
		{
			// проверка принадлежности объекта кадру
			if (y < 0 || y + h >= pImage->sSizeY)
			{
					// объект вышел за пределы кадра по вертикали 
					m_Leave = true;
					return;
			}
			for (int x = m_search_area.left; x < m_search_area.left + 2*m_dL; x++)
			{
				// проверка принадлежности объекта кадру
				if (x < 0 || x + w >= pImage->sSizeX )
				{
					// объект вышел за пределы кадра по горизонтали
					m_Leave = true;
					return;
				}
				r.left = x; 
				r.right = x+w;
				r.top = y;
				r.bottom = y+h;
				CalcFeatures(pImage, r, f);
				d = CompareFeatures(f, m_data);
				if (mf > d)
				{
					mf = d;
					mx = x;
					my = y;
				}
			}
		}
	}

	// анализ наилучшего положения объекта
#ifdef _DEBUG
	printf("obj dist = %f id = %d\n",mf, m_id );
#endif

	if (mf > 50)
	{
		// объект потерян. 
		m_Leave = true;
		return;
	}

	// объект присутствует на сцене
	m_bounds.left = mx;
	if (m_bounds.left < 0)
		m_bounds.left = 0;
	m_bounds.right = mx + w;
	if (m_bounds.right >= pImage->sSizeX)
		m_bounds.right = pImage->sSizeX -1;
	m_bounds.top = my;
	if (m_bounds.top < 0)
		m_bounds.top = 0;
	m_bounds.bottom = my + h;
	if (m_bounds.bottom > pImage->sSizeY -1)
		m_bounds.bottom =  pImage->sSizeY - 1;

	// установка параметров объекта
	UpdateBLOBState();
}

void TLFBLOBObject::UpdateBLOBState()
{
	if (!m_Leave)
	{
		m_age += m_current_dt;
		awpPoint p;
		p.X = (m_bounds.left + m_bounds.right)/2;
		p.Y = (m_bounds.bottom + m_bounds.top) /2;
		if (abs(p.X - m_center.X) < 4 && abs(p.Y - m_center.Y) < 4)
		{
			m_StayCount += m_current_dt;
		}
		else
			m_StayCount = 0;
		m_center = p;
	}
}


void TLFBLOBObject::GetFeatures(awpImage* pImage)
{
	CalcFeatures(pImage, m_bounds, m_data);
}

bool  TLFBLOBObject::IsNewBLOB()
{
	return this->m_new_blob;
}



TSymptom::TSymptom()
{
	m_first_time = true;
	m_max_num_obj = 30;
	m_NumFrames =0;
	m_id_count = 0;
	m_min_blob_immoveable_age = 45; // todo: перейти от кадров ко времени
	m_min_blob_age = 45;
	m_max_aspect_ratio = 1.f;
	m_min_aspect_ratio = 0.2f;
	m_ResizeFactor = 1.f;

	SetMinBlobSquare(1.f);
	SetMaxBlobSquare(8.f);
	awpRect roiRect;
	roiRect.left = 10;
	roiRect.right = 310;
	roiRect.top = 10;
	roiRect.bottom = 230;
	SetRoiRect(roiRect);

	m_source_width = 320;
	m_source_height = 240;

	m_pUserData = NULL;

	m_start_time = -1;
	m_current_time = -1;
	m_current_dt = -1;
	m_bgAvg = 0;
	m_fgbg_algorytm = fgbgPCA;
	m_pPCATemplate = NULL;
	m_NumFramesToTraining = 100;

	m_ppSamples = (awpImage**)malloc(m_NumFramesToTraining*sizeof(awpImage*));
	memset(m_ppSamples, 0, m_NumFramesToTraining*sizeof(awpImage*));

	m_FGBGThreshold = 0.075f;


	m_DxMaxChange = 50;
	m_DyMaxChange = 50;
	m_SearchDelta = 50;
}
TSymptom::~TSymptom()
{
// dummy
	if (m_pPCATemplate != NULL)
		fvcFreeTemplate(&m_pPCATemplate);
	for (int i = 0; i < c_nMaxNumTrainig; i++)
		if (m_ppSamples != NULL)
			awpReleaseImage(&m_ppSamples[i]);
	free (m_ppSamples);
}


// функции возвращают результат работы алгоритма поиска объектов 
int TSymptom::GetNumBlobs()
{
	return m_BLOBs.GetCount();
}
TLFBLOBObject* TSymptom::GetBlob(int index)
{
	if (index < 0 || index >= m_BLOBs.GetCount())
		return NULL;
	TLFBLOBObject* b = (TLFBLOBObject*)m_BLOBs.Get(index);
	TLFBLOBObject* bo = new TLFBLOBObject(this);
	awpRect r;
	awpPoint p;
	p.X = (AWPSHORT)(b->GetCenter().X / m_ResizeFactor);
	p.Y = (AWPSHORT)(b->GetCenter().Y / m_ResizeFactor);
	r.left = (AWPSHORT)(b->GetBounds().left /m_ResizeFactor);
	r.top = (AWPSHORT)(b->GetBounds().top / m_ResizeFactor);
	r.right = (AWPSHORT)(b->GetBounds().right / m_ResizeFactor);
	r.bottom = (AWPSHORT)(b->GetBounds().bottom / m_ResizeFactor);
	int area = (int)(b->GetArea() / (m_ResizeFactor*m_ResizeFactor));
	bo->Setup(b->GetId(), area, r, p, b->GetAge(), b->GetStayCount(), b->Getleave(), b->IsNewBLOB());
	bo->SetSearchArea((float)GetSearchDelta()*(r.right - r.left)/100.f);

	return bo;
}
/*вычисление разницы между моделью фона и вновь пришедщим изображением*/
void TSymptom::AbsDiff()
{
	awpImage* pbg = m_background.GetImage();
	awpImage* pbg2 = m_background2.GetImage();
	awpImage* pcr = m_current.GetImage();
	awpImage* pdf = m_diff.GetImage();
	awpImage* pdiff1 = m_diff1.GetImage();
	awpImage* pprev  = m_prev.GetImage();
	
	float* bg = (float*)pbg->pPixels;
	float* bg2 = (float*)pbg2->pPixels;

	AWPBYTE*  c  = (AWPBYTE*)pcr->pPixels;
	AWPBYTE*  d  = (AWPBYTE*)pdf->pPixels;
	AWPBYTE*  d1 = (AWPBYTE*)pdiff1->pPixels;
	AWPBYTE*  pp = (AWPBYTE*)pprev->pPixels;

	int thr = m_FGBGThreshold*255;
	if (m_fgbg_algorytm == fgbgPCA)
	{
		if (this->m_NumFrames < m_NumFramesToTraining)
		{
			awpCopyImage(pcr, &m_ppSamples[m_NumFrames-1]);
		}
		else if (this->m_NumFrames == m_NumFramesToTraining)
		{
			fvcBuildTemplate(this->m_ppSamples, c_nMaxNumTrainig, m_pPCATemplate);
#ifdef _DEBUG 
			fvcSaveTemplateAsImage("template.jpg", m_pPCATemplate);
#endif
		}
		else
		{
			awpImage* pReconstr = NULL;
			fvcGetReconstruction(m_pPCATemplate, pcr, &pReconstr); 
			AWPBYTE* rr = (AWPBYTE*)pReconstr->pPixels;
#ifdef _DEBUG
			awpSaveImage("tmpl_reconst.jpg", pReconstr);
#endif
		int i = 0;
		for (int y= 0; y < pcr->sSizeY; y++)
		{
			for (int x = 0; x < pcr->sSizeX; x++)
			{
				if (x > m_insideRoi.left && x < m_insideRoi.right &&
					y > m_insideRoi.top && y < m_insideRoi.bottom)
				{
					d[i] = abs(c[i] - rr[i]) > thr ? 255 : 0; 
				}
				else
					d[i] = 0;
				i++;
			}
		}
			awpReleaseImage(&pReconstr);
		}
	}
	else if (m_fgbg_algorytm == fgbgSG)
	{
		int i = 0;
		for (int y= 0; y < pbg->sSizeY; y++)
		{
			for (int x = 0; x < pbg->sSizeX; x++)
			{
				if (x > m_insideRoi.left && x < m_insideRoi.right &&
					y > m_insideRoi.top && y < m_insideRoi.bottom)
				{
					bg[i] += c[i];
					bg2[i] += c[i]*c[i];
					float m = bg[i] / (float)(m_NumFrames + 1);
					float s2 = bg2[i] / (float)(m_NumFrames + 1);
					s2 -= m*m;
					s2 += 0.00001;
					float s = sqrt(s2) + 0.00001;
					//float pr = (1 / (s*sqrt(2*3.14)))*exp(-(c[i] - m)*(c[i] - m) / (2*s2));
					//if ((c[i] - m) == 0)
					//	d[i] = 0;
					//else
					//	d[i] = pr > 0.004 ? 0 : 255;
					//d[i] = abs(c[i] - m) > 4*s ? 255 : 0; 
					d[i] = abs(c[i] - m) > thr ? 255 : 0; 
				}
				else
					d[i] = 0;
				i++;
			}
		}
	}
}

// функция обработки нового изображения 
void TSymptom::SetImage(awpImage* pImage)
{
	if (pImage == NULL)
		return;
	awpImage* src = NULL;
	awpCopyImage(pImage, &src);
	m_source_image.SetImage(pImage);
	awpConvert(src, AWP_CONVERT_3TO1_AWPBYTE);
	
	AWPWORD NewWidth  = (AWPWORD)(m_ResizeFactor*pImage->sSizeX);
	AWPWORD NewHeight = (AWPWORD)(m_ResizeFactor*pImage->sSizeY);

	if (m_first_time)
	{
		// установим новые параметры площади
		m_source_width = pImage->sSizeX;
		m_source_height = pImage->sSizeY;	

		SetMinBlobSquare(m_min_blob_square);
		SetMaxBlobSquare(m_max_blob_square);

		
		m_ResizeFactor = sqrt(c_flProcessingArea/(m_source_width*m_source_height));

		//awpRect roiRect;
		//roiRect.left = 10;
		//roiRect.right = this->m_source_width - 10;
		//roiRect.top = 10;
		//roiRect.bottom = this->m_source_height -10;
		//SetRoiRect(roiRect);

		NewWidth  = (AWPWORD)(m_ResizeFactor*pImage->sSizeX);
		NewHeight = (AWPWORD)(m_ResizeFactor*pImage->sSizeY);

#ifdef _DEBUG
		awpSaveImage("bg.jpg", src);
#endif

		awpResize(src, (AWPWORD)NewWidth, (AWPWORD)NewHeight);
		awpConvert(src, AWP_CONVERT_TO_FLOAT);
		
		m_background.SetImage(src);
		awpImage* src1 = m_background.GetImage();
		awpGaussianBlur(src, src1,2);
		m_background2.SetImage(src);
		awpImage* pb2 = m_background2.GetImage();
		float* b2 = (float*)pb2->pPixels;
		for (int i = 0; i < pb2->sSizeX*pb2->sSizeY; i++)
			b2[i] *=b2[i];
		awpImage* tmp = NULL;
		awpCreateImage(&tmp, src->sSizeX, src->sSizeY, 1, AWPBYTE);	
		
		// SetImage делает копию изображения tmp внутри объекта
		m_diff.SetImage(tmp);
		m_binary.SetImage(tmp);
		m_binary_source.SetImage(tmp);
		m_diff1.SetImage(tmp);
		m_prev.SetImage(tmp);
		awpReleaseImage(&tmp);
		this->SetRoiRect(m_roiRect);

			
		fvcCreateTemplate(&m_pPCATemplate, src->sSizeX, src->sSizeY, 10);


		m_first_time = false;

		m_start_time = clock() / CLOCKS_PER_SEC ;
		m_current_time = m_start_time;
	}
	else if (NewWidth != m_background.GetImage()->sSizeX || 
			 NewHeight != m_background.GetImage()->sSizeY) 
	{
		Reset();
		goto cleanup;
	}
	else
	{
		m_NumFrames++;
		awpResize(src, (AWPWORD)NewWidth, (AWPWORD)NewHeight);
		//обработка времени 
		float t = clock() / CLOCKS_PER_SEC;
		m_current_dt = t - m_current_time;
		m_current_time = t;
		// todo: если промежуток времени между кадрами превысил порог
		// сбрасываем фон и все объекта и начинаем наблюдения снова. 
		if (/*m_current_dt > c_flMinT*/false)
		{
			Reset();
			goto cleanup;
		}
		else
		{
					
			m_current.SetImage(src);
			awpImage* src1 = m_current.GetImage();
			awpGaussianBlur(src, src1, 2);
#ifdef _DEBUG
//			m_current.SaveImage("Gauss.jpg");
#endif
		}
		// обработка изображения. 
		Process();
		m_prev.SetImage(src);
	}
cleanup:
	awpReleaseImage(&src);
}

void TSymptom::Reset()
{
	m_BLOBs.Clear();
	m_NumFrames = 0;
	m_first_time = true;
	m_start_time = -1;
	m_current_time = -1;
	m_current_dt = 0;
}

double TSymptom::GetAverageRoi(awpImage* pImage)
{
	return CalcFeature(pImage, m_insideRoi);
}

void TSymptom::Process()
{
	awpImage* pbg = m_background.GetImage();
	awpImage* pcr = m_current.GetImage();

	awpImage* pdf = m_diff.GetImage();
	awpImage* pbn = m_binary.GetImage();
	awpImage* pbns = m_binary_source.GetImage();


	_awpZeroImage(pbn);
	_awpZeroImage(pbns);
	_awpZeroImage(pdf);
	if (pbg == NULL || pcr == NULL || pdf == NULL || pbn == NULL)
		return;
	if (pbg->sSizeX != pcr->sSizeX || pbg->sSizeY != pcr->sSizeY)
		return;
	//вычисление разницы между полученным изображением и эталонным. 
	AbsDiff();
	if (m_NumFrames < m_NumFramesToTraining)
		return;
	//нахождение бинарного изображения 
	_awpAdaptiveThreshold(pdf, pbns);
	::awpCopyImage(pbns, &pbn);
	_awpNoiseRemove(pbns);
	//awpGaussianBlur(pbns, pbn,2);
#ifdef _DEBUG
	m_background.SaveImage("background.jpg");
	m_current.SaveImage("Current.jpg");
	m_diff.SaveImage("diff.jpg");
	m_binary.SaveImage("binary.jpg");
	m_binary_source.SaveImage("binary_source.jpg");
#endif 
	//	 сопровождение найденных объектов
	for (int i = 0; i < m_BLOBs.GetCount(); i++)
	{
		TLFBLOBObject* bo = (TLFBLOBObject*)m_BLOBs.Get(i);
		bo->TrackBLOB(NULL);
	}
	//	 вычисление интегрального изображения. 
	// todo: удаление областей на бинарном изображении, занятых объектами. 
	ApplyMask(pbn);
	// анализ полученного бинарного изображения 
	// нахождение новых объектов
	Analysis(NULL);
}

void TSymptom::ClearObjects()
{
	int i = m_BLOBs.GetCount();
	while( i > 0 )
	{
		TLFBLOBObject* o = (TLFBLOBObject*)m_BLOBs.Get(i-1);
		if (o->Getleave())
			m_BLOBs .Delete(i-1);
		i--;	
	}
}


void TSymptom::Analysis(awpImage* pImage)
{
	awpImage* pbn = m_binary.GetImage();
	awpImage* pbns = m_binary_source.GetImage();

	if (pbn == NULL)
		return;
	int num = 0;
	awpStrokeObj* str = NULL;
	awpGetStrokes(pbn, &num, &str, 128, NULL);
#ifdef _DEBUG
	printf("num objects = %d\n", num);
	//m_binary_source.SaveImage("c:\\test.jpg");
#endif 
	if (num == 0)
		return;
	for (int i = 0; i < num; i++)
	{
		awpStrokeObj* so = &str[i];
		int s = 0;
		awpStrObjSquare(so, &s);
		if (s >= m_MinArea && s <= m_MaxArea)
		{
			// найден объект, удовлетворяющий условиям поиска
			awpRect r;
			awpCalcObjRect(so, &r);
			float ar = (float)(r.right - r.left) / (float) (r.bottom - r.top);
			int min_dx = 10;
			int min_dy = 12;
			if ((r.right - r.left) > min_dx && (r.bottom - r.top) > min_dx &&
				(r.right < m_insideRoi.right - min_dx && r.left > m_insideRoi.left &&
				 r.bottom <= m_insideRoi.bottom && r.top > m_insideRoi.top) && 
				ar > m_min_aspect_ratio && ar < m_max_aspect_ratio)
			{
				awpPoint p;
				awpGetObjCentroid(pbn, so, &p);
				TLFBLOBObject* blob = new TLFBLOBObject(this);
				blob->Setup(m_id_count, s, r, p, 0,0,false, true);
				
				int index = 0;
   				m_BLOBs.Add(blob);
				m_id_count++;
			}
			else
			{
				awpDrawStrokeObj(pbns, so, 0);
			}
	
		}
		else 
		{
			awpDrawStrokeObj(pbns, so, 0);
		}

	}
	awpFreeStrokes(num, &str);
}



void TSymptom::ApplyMask(awpImage* pImage)
{
	AWPBYTE* b = (AWPBYTE*)pImage->pPixels;
	for (int i = 0; i < m_BLOBs.GetCount(); i++)
	{
		TLFBLOBObject* o = (TLFBLOBObject*)m_BLOBs.Get(i);
		for (int y = o->GetSearchArea().top; y < o->GetSearchArea().bottom; y++)
		{
			for (int x = o->GetSearchArea().left; x < o->GetSearchArea().right; x++)
			{
				if (y> 0 && y < pImage->sSizeY && x > 0 && x < pImage->sSizeX)
					b[y*pImage->sSizeX + x] = 0;
			}
		}
	}
}

int  TSymptom::GetSourceWidth()
{
	return m_source_width;
}
int  TSymptom::GetSourceHeight()
{
	return m_source_height;
}
awpRect TSymptom::GetRoiRect()
{
	return m_roiRect;
}

float TSymptom::GetMinBlobSquare()
{
	return m_min_blob_square;
}
float TSymptom::GetMaxBlobSquare()
{
	return m_max_blob_square;
}

int TSymptom::GetMinArea()
{
	return m_MinArea;
}

int TSymptom::GetMinAreaToLeave()
{
	return m_MinAreaToLeave;
}


int TSymptom::GetMaxArea()
{
	return m_MaxArea;
}


float TSymptom::GetMinAspectRatio()
{
	return m_min_aspect_ratio;
}
float TSymptom::GetMaxAspectRatio()
{
	return m_max_aspect_ratio;
}

float TSymptom::GetMinBlobAge()
{
	return m_min_blob_age;
}
float TSymptom::GetMinBlobImmoveableAge()
{
	return m_min_blob_immoveable_age;
}

void TSymptom::SetRoiRect(awpRect roiRect)
{
	m_roiRect = roiRect;
	this->m_insideRoi.left = m_roiRect.left * m_ResizeFactor;
	this->m_insideRoi.top = m_roiRect.top * m_ResizeFactor;
	this->m_insideRoi.right = m_roiRect.right * m_ResizeFactor;
	this->m_insideRoi.bottom = m_roiRect.bottom * m_ResizeFactor;
}

awpRect TSymptom::GetInsideRoiRect()
{
	return m_insideRoi;
}

void TSymptom::SetMinBlobSquare(float Value)
{
	// проверка допустимости параметра
	if (Value > 0 && Value < 100)
	{
		m_min_blob_square = Value;
		float image_area = m_source_width*m_source_height;
		m_MinArea = m_min_blob_square*image_area / 100.f;
		m_MinAreaToLeave = m_MinArea / 20;
	}
}
void TSymptom::SetMaxBlobSquare(float Value)
{
	//проверка допустимости параметра
	if (Value > 0 && Value < 100)
	{
		m_max_blob_square = Value;
		float image_area = m_source_width*m_source_height;
		m_MaxArea = m_max_blob_square*image_area / 100.f;
	}
}

void TSymptom::SetMinAspectRatio(float Value)
{
	if (Value > 0)
		m_min_aspect_ratio = Value;
}

void TSymptom::SetMaxAspectRatio(float Value)
{
	if (Value > 0)
		m_max_aspect_ratio = Value;
}

void TSymptom::SetMinBlobAge(float Value)
{
	if (Value > 0)
		m_min_blob_age = Value;
}
void TSymptom::SetMinBlobImmoveableAge(float Value)
{
	if (Value > 0)
		m_min_blob_immoveable_age = Value;
}

void* TSymptom::GetUserData()
{
	return m_pUserData;
}
void  TSymptom::SetUserData(void* Value)
{
	m_pUserData = Value;
}

void TSymptom::BeginChangeParams()
{
	memset(&m_VariableParameters, 0, sizeof(m_VariableParameters));
	m_VariableParameters.roiRect = m_roiRect;
	m_VariableParameters.max_blob_aspect_ratio = m_max_aspect_ratio;
	m_VariableParameters.max_blob_square = m_max_blob_square;
	m_VariableParameters.min_blob_age = m_min_blob_age;
	m_VariableParameters.min_blob_age_immov = m_min_blob_immoveable_age;
	m_VariableParameters.min_blob_aspect_ratio = m_min_aspect_ratio;
	m_VariableParameters.min_blob_square = m_min_blob_square;
}
void TSymptom::RollBackChangeParams()
{
	this->SetRoiRect(m_VariableParameters.roiRect);
	m_max_aspect_ratio = m_VariableParameters.max_blob_aspect_ratio;
	SetMaxBlobSquare(m_VariableParameters.max_blob_square);
	m_min_blob_age = m_VariableParameters.min_blob_age;
	m_min_blob_immoveable_age = m_VariableParameters.min_blob_age_immov;
	m_min_aspect_ratio = m_VariableParameters.min_blob_aspect_ratio;
	SetMinBlobSquare(m_VariableParameters.min_blob_square);
}
void TSymptom::CommitChangeParams()
{
	memset(&m_VariableParameters, 0, sizeof(m_VariableParameters));
}

int TSymptom::GetmaxNumObj()
{
	return m_max_num_obj;
}
void TSymptom::SetMaxNumObj(int NumObj)
{
	if (!m_first_time)
		return;
	if (NumObj > 30)
		return;
	m_max_num_obj = NumObj;
}
awpImage* TSymptom::GetBinary()
{
	return m_binary.GetImage();
}
awpImage* TSymptom::GetDiff()
{
	return m_diff.GetImage();
}
awpImage* TSymptom::GetBackground()
{
	return m_background.GetImage();
}
awpImage* TSymptom::GetBinarySource()
{
	return m_binary_source.GetImage();
}
awpImage* TSymptom::GetSource()
{
	return m_source_image.GetImage();
}


TFGBGAlgorythm TSymptom::GetFGBGAlgorythm()
{
	return m_fgbg_algorytm;
}
void TSymptom::SetFGBGAlgorythm(TFGBGAlgorythm Value)
{
	m_fgbg_algorytm = Value;
	this->Reset();
}
float TSymptom::GetFGBGThreshold()
{
	return m_FGBGThreshold;
}

void TSymptom::SetFGBGThreshold(float value)
{
	m_FGBGThreshold = value; 
}

int TSymptom::GetNuFramesToTrainig()
{
	return m_NumFramesToTraining;
}
void TSymptom::SetNumFramesToTrainig(int value)
{
	m_NumFramesToTraining = value;
}

int TSymptom::GetDxMaxChange()
{
	return m_DxMaxChange;
}
void TSymptom::SetDxMaxChange(int value)
{
	m_DxMaxChange = value;
}
int TSymptom::GetDyMaxChange()
{
	return m_DyMaxChange;
}

void TSymptom::SetDyMaxChange(int value)
{
	m_DyMaxChange = value;
}

int TSymptom::GetSearchDelta()
{
	return m_SearchDelta;
}

void TSymptom::SetSearchDelta(int value)
{
	m_SearchDelta = value;
}
