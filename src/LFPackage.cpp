#include "_LF.h"

TLFPackageEngine::TLFPackageEngine() : ILFDetectEngine()
{
	m_minWidth = 1;
	m_maxWidth = 30;
	m_minHeight = 1;
	m_maxHeight = 30;

	TLFFGBGDetector* d = new TLFFGBGDetector();
    d->SetDelay(15);

	this->m_detectors.Add(d);
 	m_cluster_maker = new TLFClusterRecursive(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	m_strPredictorName = "TLFIntegralImagePredictor";
}

void TLFPackageEngine::InitDetectors()
{
   ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
   if (d != NULL)
   {
	   awpImage* img = m_SourceImage.GetImage();
	   d->Init(img);
   }
}

void TLFPackageEngine::Clear()
{
	this->m_tmpList.Clear();
	this->m_result.Clear();
	this->m_tmp_result.Clear();
	for (int i = 0; i < this->m_detectors.GetCount(); i++)
	{
		ILFObjectDetector* d = dynamic_cast<ILFObjectDetector*>(m_detectors.Get(i));
		if (d)
		{
			d->Clear();
		}
	}
}
bool TLFPackageEngine::LoadXML(TiXmlElement* parent)
{
	return false;
}
TiXmlElement*  TLFPackageEngine::SaveXML()
{
	return NULL;
}
bool TLFPackageEngine::FindObjects()
{
	if (this->m_SourceImage.GetImage() == NULL)
		return false;
	if (this->m_SourceImage.GetImage()->dwType != AWP_BYTE)
		return false;

	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	d->Detect();
   	m_cluster_maker->MakeClusters();
	BuildForeground();
	this->OverlapsFilter(&m_tmp_result);
	return true;

}

bool TLFPackageEngine::DetectInRect(awpRect* rect)
{
	return false;
}

int TLFPackageEngine::GetDelay()
{
	TLFFGBGDetector* d = (TLFFGBGDetector*)GetDetector();
	if (d != NULL)
	{
		return d->GetDelay();
	}
	else
		return 0;
}

void TLFPackageEngine::SetDelay(int value)
{
	TLFFGBGDetector* d = (TLFFGBGDetector*)GetDetector();
	if (d != NULL)
	{
		d->SetDelay(value);
	}
}

void    TLFPackageEngine::BuildForeground()
{
	ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
	if (!d)
		return;

	awpImage* fg = NULL;
	TLFImage* img = d->GetImage();

	awpCreateImage(&fg, img->GetImage()->sSizeX, img->GetImage()->sSizeY, 1, AWP_BYTE);
	m_foreground.SetImage(fg);
	awpReleaseImage(&fg);
	int count = 0;

	if (d->GetNumItems() > 0)
	{
		for (int i = 0; i < d->GetNumItems(); i++)
		{
			TLFDetectedItem* item = d->GetItem(i);
			if (item != NULL && item->HasObject())
			{
				awpRect    item_rect = item->GetBounds()->GetRect();
				awpImage*  fg = m_foreground.GetImage();

				awpFillRect(fg, &item_rect, 0,item->GetColor() + 1);

				TLFDetectedItem* di = new TLFDetectedItem(item);
				di->Resize(m_resizeCoef);
				m_tmpList.Add(di);
				count++;
			}
		}
	}

}

awpImage* TLFPackageEngine::GetForeground()
{
	return m_foreground.GetImage();
}

TLFImage* TLFPackageEngine::GetForegroundImage()
{
	return &m_foreground;
}

void TLFPackageEngine::OverlapsFilter(TLFSemanticImageDescriptor* descriptor)
{
	SLFBinaryBlob* blob = this->m_cluster_maker->GetBlobs();
	this->m_result.Clear();
	SLFBinaryBlob* blobs = this->m_cluster_maker->GetBlobs();
	for (int i = 0; i < LF_NUM_CLUSTERS; i++)
	{
		if (blobs[i].state > 0)
		{
			awpRect rect;
			rect.left = (AWPSHORT)blobs[i].sx;
			rect.right = rect.left + (AWPSHORT)blobs[i].w;
			rect.top = (AWPSHORT)blobs[i].sy;
			rect.bottom = rect.top + (AWPSHORT)blobs[i].h;

			TLFDetectedItem* item = new TLFDetectedItem(&rect, 0, "Package part", 0, 0,8,8,"TLFFGBGDetector",blobs[i].id,NULL);
			item->Resize(m_resizeCoef);
			item->SetState(blobs[i].state);
			m_result.Add(item);
		}
	}
}

float TLFPackageEngine::GetMinWidth()
{
	return m_minWidth;
}
void TLFPackageEngine::SetMinWidth(float value)
{
	if (value > 0 && value < 100)
	{
		m_minWidth = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterRecursive(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);

	}
}
float TLFPackageEngine::GetMinHeight()
{
	return m_minHeight;
}

void TLFPackageEngine::SetMinHeight(float value)
{
	if (value > 0 && value < 100)
	{
		m_minHeight = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterRecursive(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	}
}

float TLFPackageEngine::GetMaxWidth()
{
	return this->m_maxWidth;
}

void TLFPackageEngine::SetMaxWidth(float value)
{
	if (value > 0 && value < 100)
	{
		m_maxWidth = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterRecursive(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	}
}

float TLFPackageEngine::GetMaxHeight()
{
	return this->m_maxHeight;
}

void  TLFPackageEngine::SetMaxHeight(float value)
{
	if (value > 0 && value < 100)
	{
		m_maxHeight = value;
		delete m_cluster_maker;
		ILFObjectDetector* d = (ILFObjectDetector*)m_detectors.Get(0);
		m_cluster_maker = new TLFClusterRecursive(d, m_minWidth, m_minHeight, m_maxWidth, m_maxHeight);
	}
}

void TLFPackageEngine::SetAverageBufferSize(int value)
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		d->SetAverageBufferSize(value);
	}
}

int TLFPackageEngine::GetAverageBufferSize()
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		return d->GetAverageBufferSize();
	}
	else
		return 0;

}

void TLFPackageEngine::SetBGStability(int value)
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		d->SetBgStability(value);
	}
}
int TLFPackageEngine::GetBGStability()
{
	if (this->GetDetector() != NULL)
	{
		TLFFGBGDetector* d = (TLFFGBGDetector*)this->GetDetector();
		return d->GetBgStability();
	}
	else
		return 0;
}

