//---------------------------------------------------------------------------
#ifndef RoiUtilsH
#define RoiUtilsH
//---------------------------------------------------------------------------



awp2DPoint  awpGetPointsCentroid(awp2DPoint* p, int count);
awpPoint    awpGetPointsCentroid(awpPoint* p, int count);
awpRect     awpResizeRect(awpRect r, double scale);
awpPoint    awpGetLeftTopRectCorner(awpRect r);
awpPoint    awpGetRightBottomRectCorner(awpRect r);
awpRect     awpTranslateRect(awpRect r, awpPoint p);
awpRect     awpTranslateRect(awpRect r, awp2DPoint p);
awpPoint    awpTranslatePoint (awpPoint p, int dx, int dy);
awpPoint    awpTranslatePoint (awp2DPoint p, int dx, int dy);
int awpRectWidth(awpRect r);
int awpRectHeight(awpRect r);
awpRect  awpCreateRect(awpPoint lt, awpPoint rb);
// яЁютхЁ хЄ яЁшэрфыхцэюёЄ№ "Ўхыющ" Єюўъш яЁ ьюєуюы№эшъє
AWPBOOL IsPointInRect(awpPoint p, awpRect r);
//эрїюфшЄ ЎхэЄЁ яЁ ьюєуюы№эшър
awpPoint RectCenter (awpRect r);

//"собирает" точки изображения pImage, принадлежащие pRoi в массив ppPoints
void ImageROIToPoints(awpImage* pImage, TROI* pRoi, awp2DPoint** ppPoints, int* count, int thr);
bool GetROIMoments(awpImage* img, TROI* roi, double& m00, double& m01, double& m10,
double& m20, double& m02, double& m11);
// перемещает ROI на dx dy
TROI TranslateROI(TROI* roi, int dx, int dy);
//изменяет размеры ROI на scale
TROI ResizeROI(TROI* roi, double scale);
// поворачивает roi на заданный угол относительно некоторой точки
TROI RotateROI(TROI* roi, double angle, awpPoint center);
//находит ц.м. прямоугольника
bool GetCenterMass(awpImage* pImage, TROI* r, awpPoint* p);
//////
bool IsIntersect(TROI* r1, TROI* r2);



/*
            RoiDistanceAvg
    вычисляет расстояние между двумя roi
*/
double RoiDistanceAvg(TROI& roi1, TROI& roi2);
/*
            RoiDistanceMin
    вычисляет расстояние между двумя roi
*/
double RoiDistanceMin(TROI& roi1, TROI& roi2);
/*
            RoiDistanceMax
    вычисляет расстояние между двумя roi
*/
double RoiDistanceMax(TROI& roi1, TROI& roi2);
/*
            RoiDistance1
    вычисляет расстояние между двумя roi
*/
double RoiDistance1(TROI& roi1, TROI& roi2);
/*
            RoiDistance2
    вычисляет расстояние между двумя roi
*/
double RoiDistance2(TROI& roi1, TROI& roi2);

/*
        Points2Rois
    преобразует массив точек в массив roi
*/
int Points2Rois(awpPoint* pPoints, int Num, TROI** ppRois, AWPWORD MinDistance, AWPWORD MaxDistance, double Angle);
/*
        ClusterRois
    собрает близкие roi в кластеры
*/
int ClusterRois(TROI* pRois, int Num, TROI** ppClusters);

/*
     AoiRect2Roi
     преобразует прямоугольник, у которого w = h  в ROI
*/
bool AoiRect2Roi(awpRect r, TROI& roi, double c1 = 0.5, double c2 = 0.25);

// получение исходных данных AOI
awpImage* GetAOISource(awpImage* pImage, awpPoint p1, awpPoint p2);
awpImage* GetAOIImage(awpImage* pImage, awpPoint p1, awpPoint p2);

#endif
