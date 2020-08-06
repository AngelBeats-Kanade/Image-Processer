/////////////////////////////////////////////////////////////////////////////
// Name:        MyApp.cpp
// Purpose:     wxWidgets plus OpenCV to proccess image
// Author:      Xiao Taojun
// Created:     2020.06.15
// Copyright:   (c) 2020 AngelBeats  <2018021401023@std.edu.uestc.cn>
// Licence:     GPL v3.0 <http://www.gnu.org/licenses/gpl-3.0.html>
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers
// ============================================================================

// for compilers that support precompilation, includes "wx/wx.h"
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers explicitly
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/artprov.h>
#include <wx/bookctrl.h>
#include <wx/sysopt.h>
#include <wx/slider.h>
#include <wx/display.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/graphics.h>
#include <wx/image.h>
#include <wx/mstream.h>
#include <wx/quantize.h>
#include <wx/scopedptr.h>
#include <wx/stopwatch.h>
#include <wx/versioninfo.h>
#include <wx/wfstream.h>

#if defined(__WXGTK__)
#ifdef wxHAVE_RAW_BITMAP
#include <wx/rawbmp.h>
#endif
#endif

#include "./resources/meaqua36.xpm"
#include <opencv2/opencv.hpp>

// ============================================================================
// declarations
// ============================================================================

using namespace cv;
// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
  virtual bool OnInit() wxOVERRIDE;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
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
  ID_RESIZE,
  ID_PAINT_BG,
  ID_NEWWINDOW,
  ID_SLIDER
};

// ----------------------------------------------------------------------------
// MyImageFrame
// ----------------------------------------------------------------------------

class MyImageFrame : public wxFrame
{
public:
  MyImageFrame(wxFrame *parent, const wxString &desc, const wxImage &image,
               double scale = 1.0)
  {
    Create(parent, desc, wxBitmap(image, wxBITMAP_SCREEN_DEPTH, scale),
           image.GetImageCount(desc));
  }

  MyImageFrame(wxFrame *parent, const wxString &desc, const wxBitmap &bitmap)
  {
    Create(parent, desc, bitmap);
  }

private:
  bool Create(wxFrame *parent, const wxString &desc, const wxBitmap &bitmap,
              int numImages = 1)
  {
    if (!wxFrame::Create(parent, wxID_ANY, wxString::Format("Image Processer"),
                         wxDefaultPosition, wxDefaultSize,
                         wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE))
      return false;

    SetIcon(wxIcon(meaqua_xpm));

    m_bitmap = bitmap;
    m_filename = desc;
    m_zoom = 1.;

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_NEW, "&Show any image...\tCtrl+O");
    menuFile->Append(wxID_SAVEAS, "&Save image...\tCtrl+S");
    menuFile->Append(ID_INFO, "Show image &information...\tCtrl+I");
    menuFile->AppendSeparator();
    menuFile->Append(ID_QUIT, "&Exit\tCtrl+Q");

    wxMenu *menuEdit = new wxMenu;
    menuEdit->AppendCheckItem(ID_PAINT_BG, "&Paint background",
                              "Uncheck this for transparent images");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_CVT, "&Gray scale\tCtrl+Shift+1");
    menuEdit->Append(ID_THR, "&Binarization processing\tCtrl+Shift+2");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_BEAUTIFY, "&Beautify image\tCtrl+Shift+3");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_MEDIAN, "&Median Blur\tCtrl+Shift+4");
    menuEdit->Append(ID_GAUSSIAN, "&Gaussian Blur\tCtrl+Shift+5");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_CANNY, "&Edge recognize\tCtrl+Shift+6");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_RESIZE, "&Fit to window\tCtrl+F");
    menuEdit->Append(wxID_ZOOM_IN, "Zoom &in\tCtrl++");
    menuEdit->Append(wxID_ZOOM_OUT, "Zoom &out\tCtrl+-");
    menuEdit->Append(wxID_ZOOM_100, "Reset zoom to &100%\tCtrl+1");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_ROTATE_LEFT, "Rotate &left\tCtrl+L");
    menuEdit->Append(ID_ROTATE_RIGHT, "Rotate &right\tCtrl+R");
    menuEdit->AppendSeparator();

    wxMenu *menuWindow = new wxMenu;
    menuWindow->Append(ID_NEWWINDOW, "&New window\tCtrl+N");

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(ID_GUIDE, "&Guide\tCtrl+F1");
    menuHelp->AppendSeparator();
    menuHelp->Append(ID_ABOUT, "&About\tCtrl+F2");

    wxMenuBar *mbar = new wxMenuBar;
    mbar->Append(menuFile, "&File");
    mbar->Append(menuEdit, "&Edit");
    mbar->Append(menuWindow, "&Window");
    mbar->Append(menuHelp, "&Help");
    SetMenuBar(mbar);

    mbar->Check(ID_PAINT_BG, true);

    //TODO: Set a slider to control the level of image process. Such as, the level of edge detection.
    //wxSlider *slider = new wxSlider(this, ID_SLIDER, 20, 0, 40, wxPoint(50,30), wxSize(10,10), wxSL_HORIZONTAL);

    CreateStatusBar(2);
    if (numImages != 1)
      SetStatusText(wxString::Format("%d images", numImages), 1);

    SetClientSize(bitmap.GetWidth(), bitmap.GetHeight());

    wxLogStatus("Welcome to use Image Processer v0.0.1!");

    return true;
  }

  void OnEraseBackground(wxEraseEvent &WXUNUSED(event))
  {
    // do nothing here to be able to see how transparent images are shown
  }

  void OnPaint(wxPaintEvent &WXUNUSED(event))
  {
    wxPaintDC dc(this);

    if (GetMenuBar()->IsChecked(ID_PAINT_BG))
      dc.Clear();

    dc.SetUserScale(m_zoom, m_zoom);

    const wxSize size = GetClientSize();
    dc.DrawBitmap(
        m_bitmap,
        dc.DeviceToLogicalX((size.x - m_zoom * m_bitmap.GetWidth()) / 2),
        dc.DeviceToLogicalY((size.y - m_zoom * m_bitmap.GetHeight()) / 2),
        true /* use mask */
    );
  }

  void OnQuit(wxCommandEvent &WXUNUSED(event)) { Close(true); }

  void OnNew(wxCommandEvent &WXUNUSED(event))
  {
    wxImage image;
    wxString filename = LoadUserImage(image);
    if (!filename.empty())
    {
      m_filename = filename;
      m_bitmap = wxBitmap(image);
    }
    m_zoom = 1;
    Refresh();
    SetClientSize(m_bitmap.GetWidth(), m_bitmap.GetHeight());
  }

  void OnInfo(wxCommandEvent &WXUNUSED(event))
  {
    wxImage image = m_bitmap.ConvertToImage();

    if (image.IsOk())
    {
      // TODO: show more information about the file
      wxString info = wxString::Format("Image size: %dx%d", image.GetWidth(),
                                       image.GetHeight());

      int xres = image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONX),
          yres = image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONY);
      if (xres || yres)
      {
        info += wxString::Format("\nResolution: %dx%d", xres, yres);
        switch (image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONUNIT))
        {
        default:
          wxFAIL_MSG("unknown image resolution units");
          wxFALLTHROUGH;

        case wxIMAGE_RESOLUTION_NONE:
          info += " in default units";
          break;

        case wxIMAGE_RESOLUTION_INCHES:
          info += " in";
          break;

        case wxIMAGE_RESOLUTION_CM:
          info += " cm";
          break;
        }
      }
      info += wxString::Format("\nImage from: %s", m_filename);
      wxLogMessage("%s", info);
    }
  }

  void OnSave(wxCommandEvent &WXUNUSED(event))
  {
#if wxUSE_FILEDLG
    wxImage image = m_bitmap.ConvertToImage();

    wxString savefilename = wxFileSelector(
        "Save Image", wxEmptyString, wxEmptyString, wxEmptyString,
        "BMP files (*.bmp)|*.bmp|"
#if wxUSE_LIBPNG
        "PNG files (*.png)|*.png|"
#endif
#if wxUSE_LIBJPEG
        "JPEG files (*.jpg)|*.jpg|"
#endif
#if wxUSE_GIF
        "GIF files (*.gif)|*.gif|"
#endif
#if wxUSE_LIBTIFF
        "TIFF files (*.tif)|*.tif|"
#endif
#if wxUSE_PCX
        "PCX files (*.pcx)|*.pcx|"
#endif
#if wxUSE_XPM
        "X PixMap files (*.xpm)|*.xpm|"
#endif
        "ICO files (*.ico)|*.ico|"
        "CUR files (*.cur)|*.cur",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);

    if (savefilename.empty())
      return;

    wxString extension;
    wxFileName::SplitPath(savefilename, NULL, NULL, &extension);

    bool saved = false;
    if (extension == "bmp")
    {
      static const int bppvalues[] = {
          wxBMP_1BPP, wxBMP_1BPP_BW, wxBMP_4BPP, wxBMP_8BPP,
          wxBMP_8BPP_GREY, wxBMP_8BPP_RED, wxBMP_8BPP_PALETTE, wxBMP_24BPP};

      const wxString bppchoices[] = {
          "1 bpp color", "1 bpp B&W", "4 bpp color", "8 bpp color",
          "8 bpp greyscale", "8 bpp red", "8 bpp own palette", "24 bpp"};

      int bppselection =
          wxGetSingleChoiceIndex("Set BMP BPP", "Image sample: save file",
                                 WXSIZEOF(bppchoices), bppchoices, this);
      if (bppselection != -1)
      {
        int format = bppvalues[bppselection];
        image.SetOption(wxIMAGE_OPTION_BMP_FORMAT, format);

        if (format == wxBMP_8BPP_PALETTE)
        {
          unsigned char *cmap = new unsigned char[256];
          for (int i = 0; i < 256; i++)
            cmap[i] = (unsigned char)i;
          image.SetPalette(wxPalette(256, cmap, cmap, cmap));

          delete[] cmap;
        }
      }
    }
#if wxUSE_LIBPNG
    else if (extension == "png")
    {
      static const int pngvalues[] = {
          wxPNG_TYPE_COLOUR,
          wxPNG_TYPE_COLOUR,
          wxPNG_TYPE_GREY,
          wxPNG_TYPE_GREY,
          wxPNG_TYPE_GREY_RED,
          wxPNG_TYPE_GREY_RED,
      };

      const wxString pngchoices[] = {
          "Colour 8bpp",
          "Colour 16bpp",
          "Grey 8bpp",
          "Grey 16bpp",
          "Grey red 8bpp",
          "Grey red 16bpp",
      };

      int sel =
          wxGetSingleChoiceIndex("Set PNG format", "Image sample: save file",
                                 WXSIZEOF(pngchoices), pngchoices, this);
      if (sel != -1)
      {
        image.SetOption(wxIMAGE_OPTION_PNG_FORMAT, pngvalues[sel]);
        image.SetOption(wxIMAGE_OPTION_PNG_BITDEPTH, sel % 2 ? 16 : 8);

        // these values are taken from OptiPNG with -o3 switch
        const wxString compressionChoices[] = {
            "compression = 9, memory = 8, strategy = 0, filter = 0",
            "compression = 9, memory = 9, strategy = 0, filter = 0",
            "compression = 9, memory = 8, strategy = 1, filter = 0",
            "compression = 9, memory = 9, strategy = 1, filter = 0",
            "compression = 1, memory = 8, strategy = 2, filter = 0",
            "compression = 1, memory = 9, strategy = 2, filter = 0",
            "compression = 9, memory = 8, strategy = 0, filter = 5",
            "compression = 9, memory = 9, strategy = 0, filter = 5",
            "compression = 9, memory = 8, strategy = 1, filter = 5",
            "compression = 9, memory = 9, strategy = 1, filter = 5",
            "compression = 1, memory = 8, strategy = 2, filter = 5",
            "compression = 1, memory = 9, strategy = 2, filter = 5",
        };

        sel = wxGetSingleChoiceIndex(
            "Select compression option (Cancel to use default)\n",
            "PNG Compression Options", WXSIZEOF(compressionChoices),
            compressionChoices, this);
        if (sel != -1)
        {
          const int zc[] = {9, 9, 9, 9, 1, 1, 9, 9, 9, 9, 1, 1};
          const int zm[] = {8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9};
          const int zs[] = {0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2};
          const int f[] = {0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                           0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8};

          image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL, zc[sel]);
          image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL, zm[sel]);
          image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_STRATEGY, zs[sel]);
          image.SetOption(wxIMAGE_OPTION_PNG_FILTER, f[sel]);
          image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_BUFFER_SIZE,
                          1048576); // 1 MB
        }
      }
    }
#endif // wxUSE_LIBPNG
    else if (extension == "cur")
    {
      image.Rescale(32, 32);
      image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
      image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0);
      // This shows how you can save an image with explicitly
      // specified image format:
      saved = image.SaveFile(savefilename, wxBITMAP_TYPE_CUR);
    }

    if (!saved)
    {
      // This one guesses image format from filename extension
      // (it may fail if the extension is not recognized):
      image.SaveFile(savefilename);
    }
#endif // wxUSE_FILEDLG
  }

  void OnCvtColor(wxCommandEvent &WXUNUSED(event))
  {
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_GRAYSCALE);
    std::cout << m_filename.ToStdString() << std::endl;
    if (!image_cv.empty())
    {
      image_cv.convertTo(image_cv, CV_8U);
      cvtColor(image_cv, image_cv, COLOR_GRAY2RGB);
      const long loadTime = sw.Time();
      wxImage image(image_cv.cols, image_cv.rows, image_cv.data, true);
      m_bitmap = wxBitmap(image);
      Refresh();
      wxLogStatus("Gray scale: Process completed in %ldms...", loadTime);
    }
    else
      wxLogError("Couldn`t load image from '%s'.", m_filename);
  }

  void OnThreshold(wxCommandEvent &WXUNUSED(event))
  {
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_GRAYSCALE);
    GaussianBlur(image_cv, image_cv, Size(5, 5), 0);
    threshold(image_cv, image_cv, 0, 255, THRESH_BINARY | THRESH_OTSU);
    image_cv.convertTo(image_cv, CV_8U);
    cvtColor(image_cv, image_cv, COLOR_GRAY2RGB);
    const long loadTime = sw.Time();
    wxImage image(image_cv.cols, image_cv.rows, image_cv.data, true);
    m_bitmap = wxBitmap(image);
    if (!m_bitmap.IsOk())
    {
      wxLogError("I`m sorry that an error occurs, please retry!");
      return;
    }
    Refresh();
    wxLogStatus("Binarization processing: Process completed in %ldms...",
                loadTime);
  }

  void OnBeautify(wxCommandEvent &WXUNUSED(event))
  {
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_COLOR), image_temp;
    bilateralFilter(image_cv, image_temp, 0, 100, 5);
    Mat kernel = (Mat_<int>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
    filter2D(image_temp, image_temp, -1, kernel, Point(-1, -1), 0); //use filter2D to improve the beautify
    cvtColor(image_temp, image_temp, COLOR_BGR2RGB);
    const long loadTime = sw.Time();
    wxImage image(image_temp.cols, image_temp.rows, image_temp.data, true);
    m_bitmap = wxBitmap(image);
    Refresh();
    wxLogStatus("Beautify image: Process completed in %ldms...", loadTime);
  }

  void OnMedian(wxCommandEvent &WXUNUSED(event))
  {
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_COLOR);
    medianBlur(image_cv, image_cv, 5);
    cvtColor(image_cv, image_cv, COLOR_BGR2RGB);
    const long loadTime = sw.Time();
    wxImage image(image_cv.cols, image_cv.rows, image_cv.data, true);
    m_bitmap = wxBitmap(image);
    Refresh();
    wxLogStatus("Median Blur: Process completed in %ldms...", loadTime);
  }

  void OnGaussian(wxCommandEvent &WXUNUSED(event))
  {
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_COLOR);
    GaussianBlur(image_cv, image_cv, Size(5, 5), 0);
    cvtColor(image_cv, image_cv, COLOR_BGR2RGB);
    const long loadTime = sw.Time();
    wxImage image(image_cv.cols, image_cv.rows, image_cv.data, true);
    m_bitmap = wxBitmap(image);
    Refresh();
    wxLogStatus("Gaussian Blur: Process completed in %ldms...", loadTime);
  }

  void OnCanny(wxCommandEvent &WXUNUSED(event))
  {
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_COLOR), image_temp,
        image_gray;
    cvtColor(image_cv, image_gray, COLOR_BGR2GRAY);
    GaussianBlur(image_gray, image_gray, Size(5, 5), 0);
    Canny(image_gray, image_temp, 3, 9, 3);
    Mat copy;
    copy.create(image_cv.size(), image_cv.type());
    copy = Scalar::all(0);
    image_cv.copyTo(copy, image_temp);
    cvtColor(copy, copy, COLOR_BGR2RGB);
    const long loadTime = sw.Time();
    wxImage image(copy.cols, copy.rows, copy.data, true);
    m_bitmap = wxBitmap(image);
    Refresh();
    wxLogStatus("Edge recognize: Process completed in %ldms...", loadTime);
  }

  void OnResize(wxCommandEvent &WXUNUSED(event))
  {
    wxImage img(m_bitmap.ConvertToImage());

    const wxSize size = GetClientSize();
    img.Rescale(size.x, size.y, wxIMAGE_QUALITY_HIGH);
    m_bitmap = wxBitmap(img);

    UpdateStatusBar();
  }

  void OnZoom(wxCommandEvent &event)
  {
    if (event.GetId() == wxID_ZOOM_IN)
      m_zoom *= 1.2;
    else if (event.GetId() == wxID_ZOOM_OUT)
      m_zoom /= 1.2;
    else // wxID_ZOOM_100
      m_zoom = 1.;

    UpdateStatusBar();
  }

  void OnRotate(wxCommandEvent &event)
  {
    double angle = 5;
    if (event.GetId() == ID_ROTATE_LEFT)
      angle = -angle;

    wxImage img(m_bitmap.ConvertToImage());
    img = img.Rotate(angle, wxPoint(img.GetWidth() / 2, img.GetHeight() / 2));
    if (!img.IsOk())
    {
      wxLogWarning("Rotation failed");
      return;
    }

    m_bitmap = wxBitmap(img);

    UpdateStatusBar();
  }

  void OnNewWindow(wxCommandEvent &WXUNUSED(event))
  {
    wxImage image = m_bitmap.ConvertToImage();
    MyImageFrame *frame = new MyImageFrame(this, m_filename, image);
    frame->Show(true);
  }

  void OnGuide(wxCommandEvent &WXUNUSED(event))
  {
    wxArrayString array;
    array.Add(
        "Note: This Image Processer is my C++ class design demo. This demo "
        "can process images(e.g. binary images). If you want to konw all "
        "funcitons, please read 'readme.text'.");
    array.Add("Here are some steps to use it:");
    array.Add("1.Click the 'File' to open file menu.");
    array.Add("2.Choose 'Show any image' to open image.");
    array.Add("3.Click the 'Edit' menu.");
    array.Add(
        "Now you can choose the function you want to use, enjoy yourself :)!");
    (void)wxMessageBox(wxJoin(array, '\n'), "User guide",
                       wxICON_INFORMATION | wxOK);
  }

  void OnAbout(wxCommandEvent &WXUNUSED(event))
  {
    wxArrayString array;

    array.Add("Image Processer demo");
    array.Add("(c) AngelBeats 2020");
    array.Add("Email: angelbeats177@gmail.com");

    array.Add(wxEmptyString);
    array.Add("Version of the wxWidgets libraries used:");
    array.Add("wxWidgets 3.1.3");
    array.Add("Version of the OpenCV libraries used:");
    array.Add("OpenCV 4.3.0");

    (void)wxMessageBox(wxJoin(array, '\n'), "About Image Processer Demo",
                       wxICON_INFORMATION | wxOK);
  }

  void UpdateStatusBar()
  {
    wxLogStatus(this, "Image size: (%d, %d), zoom %.2f", m_bitmap.GetWidth(),
                m_bitmap.GetHeight(), m_zoom);
    Refresh();
  }

  wxString LoadUserImage(wxImage &image)
  {
    wxString filename;
    filename = wxLoadFileSelector("Image", wxEmptyString);
    wxStopWatch sw;
    if (!filename.empty())
      if (!image.LoadFile(filename))
      {
        wxLogError("Couldn`t load image from '%s'.", filename);
        return wxEmptyString;
      }
    const long loadTime = sw.Time();
    wxLogStatus("Image loaded from '%s' in %ldms...", filename, loadTime);
    return filename;
  }

  wxBitmap m_bitmap;
  wxString m_filename;
  double m_zoom;

  wxDECLARE_EVENT_TABLE();
};

//============================================================================
// event tables
//============================================================================

wxBEGIN_EVENT_TABLE(MyImageFrame, wxFrame)
EVT_ERASE_BACKGROUND(MyImageFrame::OnEraseBackground)
EVT_PAINT(MyImageFrame::OnPaint)

EVT_MENU(ID_NEW, MyImageFrame::OnNew)
EVT_MENU(ID_INFO, MyImageFrame::OnInfo)
EVT_MENU(wxID_SAVEAS, MyImageFrame::OnSave)

EVT_MENU(ID_CVT, MyImageFrame::OnCvtColor)
EVT_MENU(ID_THR, MyImageFrame::OnThreshold)
EVT_MENU(ID_BEAUTIFY, MyImageFrame::OnBeautify)
EVT_MENU(ID_MEDIAN, MyImageFrame::OnMedian)
EVT_MENU(ID_GAUSSIAN, MyImageFrame::OnGaussian)
EVT_MENU(ID_CANNY, MyImageFrame::OnCanny)
EVT_MENU_RANGE(ID_ROTATE_LEFT, ID_ROTATE_RIGHT, MyImageFrame::OnRotate)
EVT_MENU(ID_RESIZE, MyImageFrame::OnResize)
EVT_MENU(wxID_ZOOM_IN, MyImageFrame::OnZoom)
EVT_MENU(wxID_ZOOM_OUT, MyImageFrame::OnZoom)
EVT_MENU(wxID_ZOOM_100, MyImageFrame::OnZoom)
EVT_MENU(ID_QUIT, MyImageFrame::OnQuit)

EVT_MENU(ID_NEWWINDOW, MyImageFrame::OnNewWindow)

EVT_MENU(ID_GUIDE, MyImageFrame::OnGuide)
EVT_MENU(ID_ABOUT, MyImageFrame::OnAbout)
wxEND_EVENT_TABLE()

// ============================================================================
// event handlers
// ============================================================================

bool MyApp::OnInit()
{
  if (!wxApp::OnInit())
    return false;

  wxInitAllImageHandlers();

  wxImage image;
  wxString filename;
  filename = "./resources/meaqua.jpg";
  if (!filename.empty())
    if (!image.LoadFile(filename))
    {
      wxLogError("Couldn`t load image from '%s'.", filename);
      return false;
    }

  wxFrame *frame = new MyImageFrame((wxFrame *)NULL, filename, image);
  frame->Center();
  frame->Show(true);

  return true;
}

// ============================================================================
// implementations
// ============================================================================

wxIMPLEMENT_APP(MyApp);
