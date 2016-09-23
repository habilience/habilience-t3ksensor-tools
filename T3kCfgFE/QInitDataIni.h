#ifndef QINITDATAINI_H
#define QINITDATAINI_H

#include "QSingletone.h"

class QInitDataIni : public QSingleton<QInitDataIni>
{
public:
    QInitDataIni();

protected:
    int		m_nBentMargin[4];		// left, top, right, bottom (% of screen)
    int		m_nBentDirection;
    int		m_nBentAlgorithm2;		// 2-camera algorithm (0: old 13 points, 1: new 13 points, 2: new 9 points)
    int		m_nBentAlgorithm4;		// 4-camera algorithm (0: old 13 points, 1: new 13 points, 2: new 9 points)
    int     m_nCalibrationCamera;

    bool    m_bBentWithDummyLoad;
    bool    m_bBentWithDummy;
    bool    m_bCalibrationCameraLoad;
    bool    m_bCalibrationCamera;
    bool    m_bIgnoreCameraPairForBentLoad;
    bool    m_bIgnoreCameraPairForBent;

    int     m_nMiniSideView;
    bool    m_bMirrorSideView;

    int		m_nDetectionGraphSharpWidth;
    float	m_fDetectionGraphCrackThresholdError;
    float	m_fDetectionGraphCrackThresholdWarning;
    int		m_nDetectionGraphLightThresholdError;
    int		m_nDetectionGraphLightThresholdWarning;

    int		m_nActiveLanguageIndex;
public:
    void setBentMargin( int nLeft, int nTop, int nRight, int nBottom, int nDir );
    void getBentMargin( int& nLeft, int& nTop, int& nRight, int& nBottom, int& nDir );
    void setBentAlgorithm2( int nType );
    int getBentAlgorithm2() const;
    void setBentAlgorithm4( int nType );
    int getBentAlgorithm4() const;
    int getActiveLanguageIndex() const;

    bool getBentWithDummy() const;
    bool getCalibrationWarning() const;
    bool getIgnoreCameraPair() const;

    int getMiniSideView() const;
    bool getMirrorSideView() const;

    int getDTCGraphSharpWidth() const;
    float getDTCGraphCrackThresholdError() const;
    float getDTCGraphCrackThresholdWarning() const;
    int getDTCGraphLightThresholdError() const;
    int getDTCGraphLightThresholdWarning() const;

    bool load();
    bool save();
};

#endif // QINITDATAINI_H
