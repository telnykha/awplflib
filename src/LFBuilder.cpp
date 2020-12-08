#include "_LF.h"
/**
	TCSBuildDetector default constructor 
*/
TCSBuildDetector::TCSBuildDetector()
{
	m_strDetectorName = "detector_name";	
	m_strBKG          = "negative_examples";	
	m_strOBJ		  = "positive_examples";			 
	m_strPathToBase   = "bkground_base";   
	m_strConfigName   = "csbuild.xml";     
	m_strLogName	  = "csbuild.log";

	m_nBgrdCount			= -1;         
	m_nMaxSamplesPerImage	= 100;
	m_nMinBgrdCount			= 100;
	m_overlapThr			= 0.5;
	m_NS					= 1000;			/*num stages*/

	// setup adaboost with default parameters 
	m_AdaBoost.SetWidthBase(24);
	m_AdaBoost.SetHeightBase(24);
	m_AdaBoost.SetArtefactsBase(m_strBKG);
	m_AdaBoost.SetObjectsBase(m_strOBJ);
	m_AdaBoost.SetFinishFar(0.1);
	m_AdaBoost.SetFeaturesCount(100);
}

/**
	Load xml configuration file 
*/

bool		TCSBuildDetector::LoadConfig(std::string const& filename)
{
	m_AdaBoost.DbgMsg("CSBuild Detector loading config ..... ");

	/*попытка открыть файл конфигурации*/
	TiXmlDocument doc(filename.c_str());
	if (!doc.LoadFile())
	{
		m_AdaBoost.DbgMsg("fail.\n");
		return false;
	}
	m_strConfigName = filename;
	TiXmlHandle hDoc(&doc);

	/*разделы файла конфигурации*/
	TiXmlElement* pElem = NULL;

	pElem = hDoc.FirstChildElement().Element();
	if (!pElem)
	{
		m_AdaBoost.DbgMsg("fail.\n");
		m_AdaBoost.DbgMsg("Invalid XML file.\n");
		return false;
	}
	const char* name = pElem->Value();
	if (strcmp(name, "BuildDetector") != 0)
	{
		m_AdaBoost.DbgMsg("fail.\n");
		m_AdaBoost.DbgMsg("Invalid XML file.\n");
		return false;
	}

	m_strConfigName = filename;
	m_strPathToBase = pElem->Attribute("bkground_base");
	m_strBKG = pElem->Attribute("negative_examples");
	m_strOBJ = pElem->Attribute("positive_examples");
	m_strDetectorName = pElem->Attribute("detector_name");
	m_strLogName = pElem->Attribute("log_name");
	//pElem->Attribute("overlap_thr", &m_overlapThr);

	int num_positive = this->GetNumObjects();

	pElem->Attribute("num_samples_per_image", &m_nMaxSamplesPerImage);
	pElem->Attribute("min_negative", &m_nMinBgrdCount);
	if (m_nMinBgrdCount < 0)
		m_nMinBgrdCount = num_positive;

	pElem->Attribute("num_negative", &m_nBgrdCount);
	if (m_nBgrdCount < 0)
		m_nBgrdCount = num_positive;
	pElem->Attribute("max_stages", &m_NS);
	int baseWidth = 24;
	int baseHeight = 24;

	pElem->Attribute("base_width", &baseWidth);
	pElem->Attribute("base_height", &baseHeight);

	double ff;
	pElem->Attribute("finish_far", &ff);

	pElem = pElem->FirstChildElement("Features");
	if (pElem == NULL)
		return false;
	int value = 0;
	pElem->QueryIntAttribute("CSFeature", &value);
	m_AdaBoost.m_CSFeature = value != 0;
	pElem->QueryIntAttribute("TLFVFeature", &value);
	m_AdaBoost.m_VFeature = value != 0;
	pElem->QueryIntAttribute("TLFHFeature", &value);
	m_AdaBoost.m_HFeature = value != 0;
	pElem->QueryIntAttribute("TLFDFeature", &value);
	m_AdaBoost.m_DFeature = value != 0;
	pElem->QueryIntAttribute("TLFCFeature", &value);
	m_AdaBoost.m_CFeature = value != 0;
	pElem->QueryIntAttribute("TLFVAFeature", &value);
	m_AdaBoost.m_VAFeature = value != 0;
	pElem->QueryIntAttribute("TLFHAFeature", &value);
	m_AdaBoost.m_HAFeature = value != 0;
	pElem->QueryIntAttribute("TLFDAFeature", &value);
	m_AdaBoost.m_DAFeature = value != 0;
	pElem->QueryIntAttribute("TLFCAFeature", &value);
	m_AdaBoost.m_CAFeature = value != 0;

	pElem->QueryIntAttribute("TLFLBPFeature", &value);
	m_AdaBoost.m_LBPFeature = value != 0;
	// конфигурирование бустинга
	m_AdaBoost.SetArtefactsBase(m_strBKG);
	m_AdaBoost.SetObjectsBase(m_strOBJ);
	m_AdaBoost.SetWidthBase(baseWidth);
	m_AdaBoost.SetHeightBase(baseHeight);
	m_AdaBoost.SetFeaturesCount(m_NS);
	m_AdaBoost.SetFinishFar(ff);

	m_AdaBoost.DbgMsg(" done.\n");


	m_AdaBoost.InitFeatures();

	return true;

}

// выполняет построение детектора
bool		TCSBuildDetector::Build()
{
	// начальная инициализация детектора 
	if (!InitDetector())
	{
		printf("Cannot init detector.\n");
		return false;
	}

	int cascade = 1;
	// главный цикл процесса
	while (BuildBkground())
	{
		//загрузка
		if (!m_AdaBoost.LoadSamples())
			return false;
		//обучение
		if (!Boost())
			return false;
		//обновление
		if (!UpdateDetector())
			break;
		m_AdaBoost.InitFeatures();
		cascade++;
	};
	return true;
}
// формирует образцы фона для "обучения" 
// из базы данных содержащей изображения, на которых
// не присутствуют обучаемые объекты
// см. файл конфигурации m_strPathToBase
bool		TCSBuildDetector::BuildBkground()
{
#ifdef WIN32
	if (!CheckDetector())
	{
		printf("Invalid detector.\n");
		return false;
	}

	RemoveBkground();

	ILFObjectDetector* cs = m_Engine.GetDetector();


	m_AdaBoost.DbgMsg("Building new bkground...\n");
	int nCount = 0;

	_finddata_t filesInfo;
	intptr_t handle = 0;
	std::string strPath = m_strPathToBase;
	// todo: сделать параметром тип данных в базе образцов. 
	strPath += "*.awp";
	int count = 0;
	double rect_ovr;
	std::string strPathToArt = m_AdaBoost.GetArtefactsBase();
	if ((handle = _findfirst((char*)strPath.c_str(), &filesInfo)) != -1)
	{
		do
		{
			count++;
			//Загрузка
			std::string strImageName = m_strPathToBase + filesInfo.name;

			TLFImage Image;
			TLFImage Image1;
			if (!Image.LoadFromFile((char*)strImageName.c_str()))
			{

				continue;
			}
			awpConvert(Image.GetImage(), AWP_CONVERT_3TO1_BYTE);
			Image1.SetImage(Image.GetImage());

			m_AdaBoost.DbgMsg("Num = " + TypeToStr(count) + " " + filesInfo.name + " ");
			m_AdaBoost.DbgMsg(TypeToStr(Image.GetImage()->sSizeX) + "x" + TypeToStr(Image.GetImage()->sSizeY) + " ");
			DWORD tc = GetTickCount();
			//поиск образцов
			int itemsFound = 0;
			if (cs->GetStagesCount() != 0)
			{
				if (m_Engine.SetSourceImage(&Image, true))
				{
					tc = GetTickCount() - tc;

					m_AdaBoost.DbgMsg("Total items = " + TypeToStr(m_Engine.GetScanner()->GetFragmentsCount()) + "\n");
					int nItemsCount = m_Engine.GetItemsCount() > m_nMaxSamplesPerImage ? m_nMaxSamplesPerImage : m_Engine.GetItemsCount();
					//сохранение образцов на диск
					rect_ovr = 0;
					for (int i = 0; i < nItemsCount; i++)
					{
						TLFDetectedItem* item = m_Engine.GetItem(i);

						awpImage* Fragment = NULL;
						awpRect r = item->GetBounds()->GetRect();
						bool canSave = true;
						/*
						попытаемся загрузить xml описание изображеия.
						*/
						std::string strXmlFileName = LFChangeFileExt(strImageName, ".xml");
						if (LFFileExists(strXmlFileName))
						{
							TLFSemanticImageDescriptor sd;
							if (sd.LoadXML(strXmlFileName.c_str()))
							{
								for (int ii = 0; ii < sd.GetItemsCount(); ii++)
								{
									TLFDetectedItem*  di = sd.GetDetectedItem(ii);
									TLFRect* bounds = di->GetBounds();
									rect_ovr = bounds->RectOverlap(r);
									if (rect_ovr > 0.5)
									{
										canSave = false;
										break;
									}
								}
							}
							if (canSave)
							{
								itemsFound++;
								if (awpCopyRect(Image1.GetImage(), &Fragment, &r) == AWP_OK)
								{
									nCount++;
									std::string strFragmentName = strPathToArt + TypeToStr(nCount) + ".awp";
									awpSaveImage(strFragmentName.c_str(), Fragment);
									//m_AdaBoost.DbgMsg("Overlap = " + TypeToStr(rect_ovr) + "\n");
									awpReleaseImage(&Fragment);
									if (nCount >= m_nBgrdCount)
									{

										m_AdaBoost.DbgMsg("\nDone.\n");
										return true;
									}
								}
							}
						}
						else
						{
							itemsFound++;
							if (awpCopyRect(Image1.GetImage(), &Fragment, &r) == AWP_OK)
							{
								nCount++;
								std::string strFragmentName = strPathToArt + TypeToStr(nCount) + ".awp";
								awpSaveImage(strFragmentName.c_str(), Fragment);
								//m_AdaBoost.DbgMsg("Overlap = " + TypeToStr(rect_ovr) + "\n");
								awpReleaseImage(&Fragment);
								if (nCount >= m_nBgrdCount)
								{

									m_AdaBoost.DbgMsg("\nDone.\n");
									return true;
								}
							}
						}
					}
					m_AdaBoost.DbgMsg("Items found = " + TypeToStr(itemsFound) + " " + TypeToStr(tc) + " ms\n");
					//if (nItemsCount == 0)
					//	DeleteFile(strImageName.c_str());
				}
			}
			else
			{
				m_AdaBoost.DbgMsg("Total items = " + TypeToStr(m_Engine.GetScanner()->GetFragmentsCount()) + "\n");
				ILFScanner* s = m_Engine.GetScanner();
				s->Scan(Image.GetImage()->sSizeX, Image.GetImage()->sSizeY);
				s->GetFragmentsCount();
				int nItemsCount = s->GetFragmentsCount() > m_nMaxSamplesPerImage ? m_nMaxSamplesPerImage : s->GetFragmentsCount();

				//сохранение образцов на диск
				for (int i = 0; i < nItemsCount; i++)
				{
					awpImage* Fragment = NULL;
					awpRect r = s->GetFragmentRect(i);
					bool canSave = true;
					/*
					попытаемся загрузить xml описание изображеия.
					*/
					std::string strXmlFileName = LFChangeFileExt(strImageName, ".xml");
					if (LFFileExists(strXmlFileName))
					{
						TLFSemanticImageDescriptor sd;
						if (sd.LoadXML(strXmlFileName.c_str()))
						{
							for (int ii = 0; ii < sd.GetItemsCount(); ii++)
							{
								TLFDetectedItem*  di = sd.GetDetectedItem(ii);
								TLFRect* bounds = di->GetBounds();
								if (bounds->RectOverlap(r) > 0.75)
								{
									canSave = false;
									break;
								}
							}
						}
					}
					if (canSave)
					{
						if (awpCopyRect(Image1.GetImage(), &Fragment, &r) == AWP_OK)
						{
							nCount++;
							std::string strFragmentName = strPathToArt + TypeToStr(nCount) + ".awp";
							awpSaveImage(strFragmentName.c_str(), Fragment);
							awpReleaseImage(&Fragment);
							if (nCount >= m_nBgrdCount)
							{
								m_AdaBoost.DbgMsg("\nDone.\n");
								return true;
							}
						}
					}
				}


			}
			//if (nCount % 10 == 0)
			m_AdaBoost.DbgMsg(">");
		} while (!_findnext(handle, &filesInfo));
	}
	_findclose(handle);
	if (nCount < m_nMinBgrdCount)
	{
		m_AdaBoost.DbgMsg("\n Bkground not found.\n");
		return false;
	}
	else
	{
		m_AdaBoost.DbgMsg("Done.\n");
		return true;
	}
#else
	return false;
#endif
}

// выполняет "накачку сильного классификатора"
bool		TCSBuildDetector::Boost()
{
	int stages = 0;
	if (this->m_Engine.GetDetector() == NULL)
		return false;
	ILFObjectDetector* od = this->m_Engine.GetDetector();
	stages = od->GetStagesCount();
	return m_AdaBoost.Boost(stages);
}
//обновление детектора. Добавление нового каскада
//к существующему детектору.
bool		TCSBuildDetector::UpdateDetector()
{
#ifdef WIN32
	m_AdaBoost.DbgMsg("Update detector...\n");

	//анализ сильного классификатора.
	TErrTrainData td = m_AdaBoost.GetTrainData();
	double min_v = 2;
	int    min_idx = -1;

	for (unsigned int i = 0; i < td.size(); i++)
	{
		if (td[i].m_frr > 0.009)
			continue;

		if (min_v > td[i].m_far)
		{
			min_v = td[i].m_far;
			min_idx = i;
		}
	}

	m_AdaBoost.DbgMsg("Min FAR = " + TypeToStr(min_v) + " Min FRR = " + TypeToStr(td[min_idx].m_frr) + ";\n");
	m_AdaBoost.DbgMsg("Stages count = " + TypeToStr(min_idx) + ";\n");
	if (min_v > 0.9)
	{
		m_AdaBoost.DbgMsg("Cannot update detector. \n");
		return false;
	}
	m_AdaBoost.DbgMsg("Updating detector.... \n");

	TCSStrong cls;
	for (int i = 0; i < m_AdaBoost.GetClass()->GetCount(); i++)
	{
		TCSWeak* wcl1 = (TCSWeak *)m_AdaBoost.GetClass()->GetWeak(i);
		cls.AddWeak(new TCSWeak(wcl1));
	}
	cls.SetThreshold(td[min_idx].m_thr);
	TSCObjectDetector* d = (TSCObjectDetector*)this->m_Engine.GetDetector();
	d->AddStrong(&cls);

	string strNegativePath = m_AdaBoost.GetArtefactsBase();
	strNegativePath += "\\stage";
	strNegativePath += TypeToStr(d->GetStagesCount());
	strNegativePath += "\\";
	//m_AdaBoost.SaveNegativeSamples(strNegativePath.c_str());

	m_AdaBoost.DbgMsg("Save engine. \n");
	m_Engine.Save(this->m_strDetectorName.c_str());

	return InitDetector();
}

bool TCSBuildDetector::SaveConfig(std::string const& filename)
{
	return false;
}
/**
*
*
*
*
*/
bool	TCSBuildDetector::InitDetector()
{
	m_AdaBoost.DbgMsg("CSBuild Detector init detector ..... ");

	if (!m_Engine.Load(m_strDetectorName.c_str()))
	{
		if (!this->CreateDetector(m_strDetectorName.c_str()))
			return false;
		if (!m_Engine.Load(m_strDetectorName.c_str()))
			return false;
	}

	m_Engine.SetNeedCluster(false);
	m_Engine.SetResize(false);
	ILFObjectDetector* cs = (ILFObjectDetector*)m_Engine.GetDetector();
	if (cs == NULL)
	{
		m_AdaBoost.DbgMsg("fail.\n");
		m_AdaBoost.DbgMsg("Detector was not created.\n");
		return false;
	}
	/*if CSEngine is empty return*/
	if (cs->GetStagesCount() == 0)
		return true;
	else
		return CheckDetector();
}
// проверяет детектор.
bool		    TCSBuildDetector::CheckDetector()
{
	/*check detector*/
	std::string strPath = m_AdaBoost.GetObjectsBase();
	_finddata_t filesInfo;
	intptr_t handle = 0;
	int num = 0;
	int count = 0;
	int NumFailed = 0;
	TSCObjectDetector* d = (TSCObjectDetector*)this->m_Engine.GetDetector();

	bool res = true;
	if ((handle = _findfirst((char*)((strPath + "\\*.awp").c_str()), &filesInfo)) != -1)
	{
		do
		{
			TLFImage img;
			std::string name = strPath + "\\" + filesInfo.name;
			img.LoadFromFile ((char*)name.c_str());
			if (img.GetImage() == NULL)
				continue;
			d->Init(img.GetImage(), false);
			awpRect r;
			r.left = 0;
			r.right = img.GetImage()->sSizeX;
			r.top = 0;
			r.bottom = img.GetImage()->sSizeY;
			if (img.GetImage()->sSizeX == 14)
				awpSaveImage("test.awp", img.GetImage());

			if (d->ClassifyRect(r, NULL, NULL) == 0)
			{
				//res = false;
				NumFailed++;
				m_AdaBoost.DbgMsg(name +" Failed.\n");
			}
//			else
//				m_AdaBoost.DbgMsg(name + " Success.\n");
			count++;
		} while (!_findnext(handle, &filesInfo));
	}
	_findclose(handle);
	//if (res) 
	//	m_AdaBoost.InitFeatures();
	return res;
#else
return false;
#endif
}
static int boost_random(int v)
{
	return (int)(((double)rand() / (double)RAND_MAX) * v);
}
//строит начальные изображения образцов фона.
bool		    TCSBuildDetector::BuildDefaultBkGround()
{
	m_AdaBoost.DbgMsg("Building new default bkground...\n");
	srand((unsigned)time(NULL));

	std::string strPathToArt = m_AdaBoost.GetArtefactsBase();


	for (int i = 0; i < m_nBgrdCount; i++)
	{
		awpImage* img = NULL;
		std::string strFragmentName = strPathToArt + TypeToStr(i) + ".awp";
		if (awpCreateImage(&img, m_AdaBoost.WidthBase(), m_AdaBoost.HeightBase(), 1, AWP_BYTE) == AWP_OK)
		{


			AWPBYTE* b = (AWPBYTE*)img->pPixels;
			int v = boost_random(230);

			for (int j = 0; j < img->sSizeX*img->sSizeY; j++)
			{
				b[j] = (AWPBYTE)(v + boost_random(20));
			}
			if (i % 100 == 0)
				m_AdaBoost.DbgMsg(".");
			awpSaveImage(strFragmentName.c_str(), img);
			awpReleaseImage(&img);
		}
	}

	m_AdaBoost.DbgMsg("\nDone.\n");
	return true;
}
// удаляет все изображения из базы образцов фона
void		    TCSBuildDetector::RemoveBkground()
{
#ifdef WIN32
	m_AdaBoost.DbgMsg("Removing old bkground...\n");
	std::string strPathToArt = m_AdaBoost.GetArtefactsBase();

	std::string strPath = strPathToArt;
	strPath += "*.awp";

	_finddata_t filesInfo;
	intptr_t handle = 0;

	if ((handle = _findfirst((char*)strPath.c_str(), &filesInfo)) != -1)
	{
		do
		{
			std::string strImageName = strPathToArt + filesInfo.name;
			DeleteFile(strImageName.c_str());
		} while (!_findnext(handle, &filesInfo));
	}
	_findclose(handle);
	m_AdaBoost.DbgMsg("Done removing old bkground.\n");
#else
	// todo: LINUX
#endif
}

// создание нового детектора 
bool TCSBuildDetector::CreateDetector(const char* lpDetectorName)
{
	if (m_Engine.GetDetector() == NULL)
	{
		TSCObjectDetector* d = new TSCObjectDetector();
		d->SetBaseHeight(m_AdaBoost.HeightBase());
		d->SetBaseWidht(m_AdaBoost.WidthBase());
		m_Engine.AddDetector(d);
	}
	return m_Engine.Save(lpDetectorName);
}

int	TCSBuildDetector::GetNumObjects()
{
#ifdef WIN32
	std::string strPath = m_strOBJ;
	strPath += "*.awp";
	int count = 0;
	_finddata_t filesInfo;
	intptr_t handle = 0;

	if ((handle = _findfirst((char*)strPath.c_str(), &filesInfo)) != -1)
	{
		do
		{
			std::string strImageName = m_strOBJ + filesInfo.name;
			count++;
		} while (!_findnext(handle, &filesInfo));
	}
	_findclose(handle);
	return count;
#else
	return 0;
#endif
}

void		TCSBuildDetector::PrintDetectorInfo()
{
	InitDetector();
	int num_stages = this->m_Engine.GetDetector()->GetStagesCount();
	this->m_AdaBoost.DbgMsg("Num stages: " + TypeToStr(num_stages) + "\n");
	for (int i = 0; i < num_stages; i++)
	{
		TSCObjectDetector* d = (TSCObjectDetector*)this->m_Engine.GetDetector();
		this->m_AdaBoost.DbgMsg("Stage " + TypeToStr(i) + " num weaks = " +
			TypeToStr(d->GetSensorsCount(i)) + " thr = " +
			TypeToStr(d->GetStageThreshold(i)) + "\n");
	}
}
void		TCSBuildDetector::AddNewClassifier()
{
	// начальная инициализация детектора 
	if (!InitDetector())
	{
		printf("Cannot init detector.\n");
		return;
	}
	// главный цикл процесса
	if (!BuildBkground())
		return;
	if (!m_AdaBoost.LoadSamples())
		return;
	if (!Boost())
		return;
	if (!UpdateDetector())
		return;
}
