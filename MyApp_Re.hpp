/////////////////////////////////////////////////////////////////////////////
// Name:         MyApp_Re.hpp
// Purpose:      wxWidgets plus OpenCV to proccess image
// Author:        Xiao Taojun
// Created:      2020.09.15
// Copyright:   (c) 2020 AngelBeats  <2018021401023@std.edu.uestc.cn>
// Licence:       GPL v3.0 <http://www.gnu.org/licenses/gpl-3.0.html>
/////////////////////////////////////////////////////////////////////////////
#include "./resources/meaqua36.xpm"
#include <opencv2/opencv.hpp>
#include <wx/filename.h>
#include <wx/wx.h>

using namespace cv;

#define OPENCV_VERSION "OpenCV-" CV_VERSION
#define WX_VERSION "wxWidgets-3.1.3"

enum {
    ID_QUIT = wxID_EXIT,
    ID_ABOUT = wxID_ABOUT,
    ID_GUIDE,
    ID_NEW = 100,
    ID_INFO,
    ID_CVT,
    ID_THR,
    ID_BEAUTIFY,
    ID_MEDIAN,
    ID_GAUSSIAN,
    ID_CANNY,
    ID_ROTATE_LEFT = wxID_HIGHEST + 1,
    ID_ROTATE_RIGHT,
    ID_RESIZE
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() wxOVERRIDE;
};

class MyImageFrame : public wxFrame {
private:
    bool Create(wxFrame* parent, const wxString& desc, const wxBitmap& bitmap, int numImages = 1);
    void OnEraseBackground(wxEraseEvent& WXUNUSED(event));
    void OnPaint(wxPaintEvent& WXUNUSED(event));
    void OnQuit(wxCommandEvent& WXUNUSED(event));
    void OnNew(wxCommandEvent& WXUNUSED(event));
    void OnInfo(wxCommandEvent& WXUNUSED(event));
    void OnSave(wxCommandEvent& WXUNUSED(event));
    void OnCvtColor(wxCommandEvent& WXUNUSED(event));
    void OnThreshold(wxCommandEvent& WXUNUSED(event));
    void OnBeautify(wxCommandEvent& WXUNUSED(event));
    void OnMedian(wxCommandEvent& WXUNUSED(event));
    void OnGaussian(wxCommandEvent& WXUNUSED(event));
    void OnCanny(wxCommandEvent& WXUNUSED(event));
    void OnResize(wxCommandEvent& WXUNUSED(event));
    void OnZoom(wxCommandEvent& event);
    void OnRotate(wxCommandEvent& event);
    void OnGuide(wxCommandEvent& WXUNUSED(event));
    void OnAbout(wxCommandEvent& WXUNUSED(event));
    void UpdateStatusBar();
    wxString LoadUserImage(wxImage& image);
    wxBitmap m_bitmap;
    wxString m_filename;
    double m_zoom;

public:
    MyImageFrame(wxFrame* parent, const wxString& desc, const wxImage& image, double scale = 1.0);
    MyImageFrame(wxFrame* parent, const wxString& desc, const wxBitmap& bitmap);
};
