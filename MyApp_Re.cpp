/////////////////////////////////////////////////////////////////////////////
// Name:         MyApp_Re.cpp
// Purpose:      wxWidgets plus OpenCV to proccess image
// Author:        Xiao Taojun
// Created:      2020.09.15
// Copyright:   (c) 2020 AngelBeats  <2018021401023@std.edu.uestc.cn>
// Licence:       GPL v3.0 <http://www.gnu.org/licenses/gpl-3.0.html>
/////////////////////////////////////////////////////////////////////////////
#include "MyApp_Re.hpp"

MyImageFrame::MyImageFrame(wxFrame* parent, const wxString& desc, const wxImage& image, double scale)
{
    Create(parent, desc, wxBitmap(image, wxBITMAP_SCREEN_DEPTH, scale), image.GetImageCount(desc));
    Bind(wxEVT_PAINT, &MyImageFrame::OnPaint, this);
    Bind(wxEVT_MENU, &MyImageFrame::OnNew, this, ID_NEW);
    Bind(wxEVT_MENU, &MyImageFrame::OnSave, this, wxID_SAVEAS);
    Bind(wxEVT_MENU, &MyImageFrame::OnInfo, this, ID_INFO);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MyImageFrame::OnQuit, this, ID_QUIT);
    Bind(wxEVT_MENU, &MyImageFrame::OnCvtColor, this, ID_CVT);
    Bind(wxEVT_MENU, &MyImageFrame::OnThreshold, this, ID_THR);
    Bind(wxEVT_MENU, &MyImageFrame::OnBeautify, this, ID_BEAUTIFY);
    Bind(wxEVT_MENU, &MyImageFrame::OnMedian, this, ID_MEDIAN);
    Bind(wxEVT_MENU, &MyImageFrame::OnGaussian, this, ID_GAUSSIAN);
    Bind(wxEVT_MENU, &MyImageFrame::OnCanny, this, ID_CANNY);
    Bind(wxEVT_MENU, &MyImageFrame::OnResize, this, ID_RESIZE);
    Bind(wxEVT_MENU, &MyImageFrame::OnZoom, this, wxID_ZOOM_IN);
    Bind(wxEVT_MENU, &MyImageFrame::OnZoom, this, wxID_ZOOM_OUT);
    Bind(wxEVT_MENU, &MyImageFrame::OnZoom, this, wxID_ZOOM_100);
    Bind(wxEVT_MENU, &MyImageFrame::OnRotate, this, ID_ROTATE_LEFT);
    Bind(wxEVT_MENU, &MyImageFrame::OnRotate, this, ID_ROTATE_RIGHT);
    Bind(wxEVT_MENU, &MyImageFrame::OnGuide, this, ID_GUIDE);
    Bind(wxEVT_MENU, &MyImageFrame::OnAbout, this, ID_ABOUT);
}

MyImageFrame::MyImageFrame(wxFrame* parent, const wxString& desc, const wxBitmap& bitmap)
{
    Create(parent, desc, bitmap);
    Bind(wxEVT_PAINT, &MyImageFrame::OnPaint, this);
    Bind(wxEVT_MENU, &MyImageFrame::OnNew, this, ID_NEW);
    Bind(wxEVT_MENU, &MyImageFrame::OnSave, this, wxID_SAVEAS);
    Bind(wxEVT_MENU, &MyImageFrame::OnInfo, this, ID_INFO);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MyImageFrame::OnQuit, this, ID_QUIT);
    Bind(wxEVT_MENU, &MyImageFrame::OnCvtColor, this, ID_CVT);
    Bind(wxEVT_MENU, &MyImageFrame::OnThreshold, this, ID_THR);
    Bind(wxEVT_MENU, &MyImageFrame::OnBeautify, this, ID_BEAUTIFY);
    Bind(wxEVT_MENU, &MyImageFrame::OnMedian, this, ID_MEDIAN);
    Bind(wxEVT_MENU, &MyImageFrame::OnGaussian, this, ID_GAUSSIAN);
    Bind(wxEVT_MENU, &MyImageFrame::OnCanny, this, ID_CANNY);
    Bind(wxEVT_MENU, &MyImageFrame::OnResize, this, ID_RESIZE);
    Bind(wxEVT_MENU, &MyImageFrame::OnZoom, this, wxID_ZOOM_IN);
    Bind(wxEVT_MENU, &MyImageFrame::OnZoom, this, wxID_ZOOM_OUT);
    Bind(wxEVT_MENU, &MyImageFrame::OnZoom, this, wxID_ZOOM_100);
    Bind(wxEVT_MENU, &MyImageFrame::OnRotate, this, ID_ROTATE_LEFT);
    Bind(wxEVT_MENU, &MyImageFrame::OnRotate, this, ID_ROTATE_RIGHT);
    Bind(wxEVT_MENU, &MyImageFrame::OnGuide, this, ID_GUIDE);
    Bind(wxEVT_MENU, &MyImageFrame::OnAbout, this, ID_ABOUT);
}

bool MyImageFrame::Create(wxFrame* parent, const wxString& desc, const wxBitmap& bitmap, int numImages)
{
    if (!wxFrame::Create(
            parent,
            wxID_ANY,
            wxString::Format("Image Processer"),
            wxDefaultPosition,
            wxDefaultSize,
            wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE))
        return false;
    SetIcon(wxIcon(meaqua_xpm));

    m_bitmap = bitmap;
    m_filename = desc;
    m_zoom = 1.0;

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_NEW, "&Show any image...\tCtrl+O");
    menuFile->Append(wxID_SAVEAS, "&Save image...\tCtrl+S");
    menuFile->Append(ID_INFO, "&Show image information...\tCtrl+I");
    menuFile->AppendSeparator();
    menuFile->Append(ID_QUIT, "&Exit\tCtrl+Q");

    wxMenu* menuEdit = new wxMenu;
    menuEdit->Append(ID_CVT, "&Gral scale\tCtrl+Shift+1");
    menuEdit->Append(ID_THR, "&Binarization process\tCtrl+Shift+2");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_BEAUTIFY, "&Beautify image\tCtrl+Shift+3");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_MEDIAN, "&Median Blur\tCtrl+Shift+4");
    menuEdit->Append(ID_GAUSSIAN, "&Gaussian Blur\tCtrl+Shift+5");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_CANNY, "&Edge recognize\tCtrl+Shift+6");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_RESIZE, "&Fit to window\tCtrl+F");
    menuEdit->Append(wxID_ZOOM_IN, "&Zoom in\tCtrl++");
    menuEdit->Append(wxID_ZOOM_OUT, "&Zoom out\tCtrl+-");
    menuEdit->Append(wxID_ZOOM_100, "&Reset zoom to 100%\tCtrl+1");
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_ROTATE_LEFT, "&Rotate left\tCtrl+L");
    menuEdit->Append(ID_ROTATE_RIGHT, "&Rotate right\tCtrl+R");

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(ID_GUIDE, "&Guide\tCtrl+G");
    menuHelp->AppendSeparator();
    menuHelp->Append(ID_ABOUT, "&About\tCtrl+A");

    wxMenuBar* mbar = new wxMenuBar;
    mbar->Append(menuFile, "&File");
    mbar->Append(menuEdit, "&Edit");
    mbar->Append(menuHelp, "&Help");
    SetMenuBar(mbar);

    CreateStatusBar(2);
    if (numImages != 1) {
        SetStatusText(wxString::Format("%d images", numImages), 1);
    }
    SetClientSize(bitmap.GetWidth(), bitmap.GetHeight());
    wxLogStatus("Welcome to use Image Processer v0.0.2!");

    return true;
}

void MyImageFrame::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {}

void MyImageFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.Clear();
    dc.SetUserScale(m_zoom, m_zoom);

    const wxSize size = GetClientSize();
    dc.DrawBitmap(
        m_bitmap,
        dc.DeviceToLogicalX((size.GetX() - m_zoom * m_bitmap.GetWidth()) / 2),
        dc.DeviceToLogicalY((size.GetY() - m_zoom * m_bitmap.GetHeight()) / 2),
        true);
}

void MyImageFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyImageFrame::OnNew(wxCommandEvent& WXUNUSED(event))
{
    wxImage image;
    wxString filename = LoadUserImage(image);
    if (!filename.empty()) {
        m_filename = filename;
        m_bitmap = wxBitmap(image);
    }
    m_zoom = 1;
    SetClientSize(m_bitmap.GetWidth(), m_bitmap.GetHeight());
    Refresh();
}

void MyImageFrame::OnInfo(wxCommandEvent& WXUNUSED(event))
{
    wxImage image = m_bitmap.ConvertToImage();
    if (image.IsOk()) {
        wxString info = wxString::Format("Image size:%dx%d", image.GetWidth(), image.GetHeight());
        int xres = image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONX);
        int yres = image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONY);
        if (xres || yres) {
            info += wxString::Format("\nResolution:%dx%d", xres, yres);
            switch (image.GetOptionInt(wxIMAGE_OPTION_RESOLUTIONUNIT)) {
            case wxIMAGE_RESOLUTION_NONE:
                info += "in default units";
                break;

            case wxIMAGE_RESOLUTION_INCHES:
                info += "in";
                break;

            case wxIMAGE_RESOLUTION_CM:
                info += "cm";
                break;

            default:
                wxFAIL_MSG("unkonwn image resolution units");
                wxFALLTHROUGH;
            }
        }
        info += wxString::Format("\nImage from:%s", m_filename);
        wxLogMessage("%s", info);
    }
}

void MyImageFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FILEDLG
    wxImage image = m_bitmap.ConvertToImage();

    //chose the file extension you want to use
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

    if (savefilename.empty()) {
        return;
    }

    wxString extension;
    wxFileName::SplitPath(savefilename, NULL, NULL, &extension);

    bool saved = false;
    if (extension == "bmp") {
        static const int bppvalues[] = {
            wxBMP_1BPP,
            wxBMP_1BPP_BW,
            wxBMP_4BPP,
            wxBMP_8BPP,
            wxBMP_8BPP_GREY,
            wxBMP_8BPP_RED,
            wxBMP_8BPP_PALETTE,
            wxBMP_24BPP
        };
        const wxString bppchoices[]{
            "1 bpp color",
            "1 bpp B&W",
            "4 bpp color",
            "8 bpp color",
            "8 bpp greyscale",
            "8 bpp red",
            "8 bpp own palette",
            "24 bpp"
        };
        int bppselection = wxGetSingleChoiceIndex(
            "Set BMP BPP",
            "Image sample: save file",
            WXSIZEOF(bppchoices),
            bppchoices,
            this);
        if (bppselection != -1) {
            int format = bppvalues[bppselection];
            image.SetOption(wxIMAGE_OPTION_BMP_FORMAT, format);

            if (format == wxBMP_8BPP_PALETTE) {
                u_char* cmap = new u_char[256];
                for (int i = 0; i < 256; i++) {
                    cmap[i] = (u_char)i;
                }
                image.SetPalette(wxPalette(256, cmap, cmap, cmap));
                delete[] cmap;
            }
        }
    }
#if wxUSE_LIBPNG
    else if (extension == "png") {
        static const int pngvalues[] = {
            wxPNG_TYPE_COLOUR,
            wxPNG_TYPE_COLOUR,
            wxPNG_TYPE_GREY,
            wxPNG_TYPE_GREY,
            wxPNG_TYPE_GREY_RED,
            wxPNG_TYPE_GREY_RED
        };
        const wxString pngchoices[] = {
            "Colour 8bpp",
            "Colour 16bpp",
            "Grey 8bpp",
            "Grey 16bpp",
            "Grey red 8bpp",
            "Grey red 16bpp"
        };
        int pngselection = wxGetSingleChoiceIndex(
            "Set PNG format",
            "Image sample: save file",
            WXSIZEOF(pngchoices),
            pngchoices,
            this);
        if (pngselection != -1) {
            image.SetOption(wxIMAGE_OPTION_PNG_FORMAT, pngvalues[pngselection]);
            image.SetOption(wxIMAGE_OPTION_PNG_BITDEPTH, pngselection % 2 ? 16 : 8);
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
                "compression = 1, memory = 9, strategy = 2, filter = 5"
            };
            pngselection = wxGetSingleChoiceIndex(
                "Select compression option (Cancel to use default)\n",
                "PNG Compression Options",
                WXSIZEOF(compressionChoices),
                compressionChoices,
                this);
            if (pngselection != -1) {
                const int zc[] = { 9, 9, 9, 9, 1, 1, 9, 9, 9, 9, 1, 1 };
                const int zm[] = { 8, 9, 8, 9, 8, 9, 8, 9, 8, 9, 8, 9 };
                const int zs[] = { 0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2 };
                const int f[] = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8 };

                image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL, zc[pngselection]);
                image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL, zm[pngselection]);
                image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_STRATEGY, zs[pngselection]);
                image.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_BUFFER_SIZE, 1048576);
            }
        }
    }
#endif //wxUSE_LIBPNG
    else if (extension == "cur") {
        image.Rescale(32, 32);
        image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
        image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0);
        saved = image.SaveFile(savefilename, wxBITMAP_TYPE_CUR);
    }
    if (!saved) {
        //this one guesses image format from filename extension
        //it may fail if the extension is not recognized :)
        image.SaveFile(savefilename);
    }

#endif //wxUSE_FILEDLG
}

void MyImageFrame::OnCvtColor(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_GRAYSCALE);
    if (!image_cv.empty()) {
        image_cv.convertTo(image_cv, CV_8U);
        cvtColor(image_cv, image_cv, COLOR_GRAY2RGB);
        const long loadTime = sw.Time();
        wxImage image(image_cv.cols, image_cv.rows, image_cv.data, true);
        m_bitmap = wxBitmap(image);
        Refresh();
        wxLogStatus("Gray scale: Process completed in %ldms...", loadTime);
    } else {
        wxLogError("Couldn`t load image from '%s'.", m_filename);
    }
}

void MyImageFrame::OnThreshold(wxCommandEvent& WXUNUSED(event))
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
    if (!m_bitmap.IsOk()) {
        wxLogError("I`m sorry that an error occurs, please retry!");
        return;
    }
    Refresh();
    wxLogStatus("Binarization processing: Process completed in %ldms...", loadTime);
}

void MyImageFrame::OnBeautify(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_COLOR);
    Mat image_temp; //Intermediate variable
    bilateralFilter(image_cv, image_temp, 0, 100, 5);
    Mat kernel = (Mat_<int>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
    filter2D(image_temp, image_temp, -1, kernel, Point(-1, -1), 0);
    cvtColor(image_temp, image_temp, COLOR_BGR2RGB); //Linux is RGB, windows is BGR
    const long loadTime = sw.Time();
    wxImage image(image_temp.cols, image_temp.rows, image_temp.data, true);
    m_bitmap = wxBitmap(image);
    Refresh();
    wxLogStatus("Beautify image: Process completed in %ldms...", loadTime);
}

void MyImageFrame::OnMedian(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_COLOR);
    medianBlur(image_cv, image_cv, 5);
    cvtColor(image_cv, image_cv, COLOR_BGR2RGB);
    const long loadTime = sw.Time();
    wxImage image(image_cv.cols, image_cv.rows, image_cv.data, true);
    m_bitmap = wxBitmap(image);
    Refresh();
    wxLogStatus("Median blur: Process completed in %ldms...", loadTime);
}

void MyImageFrame::OnGaussian(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_COLOR);
    GaussianBlur(image_cv, image_cv, Size(5, 5), 0);
    cvtColor(image_cv, image_cv, COLOR_BGR2RGB);
    const long loadTime = sw.Time();
    wxImage image(image_cv.cols, image_cv.rows, image_cv.data, true);
    m_bitmap = wxBitmap(image);
    Refresh();
    wxLogStatus("Guassian Blur: Process completed in %ldms...", loadTime);
}

void MyImageFrame::OnCanny(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;
    Mat image_cv = imread(std::string(m_filename.ToUTF8().data()), IMREAD_COLOR);
    Mat image_temp, image_gray, image_copy;
    cvtColor(image_cv, image_gray, COLOR_BGR2GRAY);
    GaussianBlur(image_gray, image_gray, Size(5, 5), 0);
    Canny(image_gray, image_temp, 3, 9, 3);
    image_copy.create(image_cv.size(), image_cv.type());
    image_copy = Scalar::all(0);
    image_cv.copyTo(image_copy, image_temp);
    cvtColor(image_copy, image_copy, COLOR_BGR2RGB);
    const long loadTime = sw.Time();
    wxImage image(image_copy.cols, image_copy.rows, image_copy.data, true);
    m_bitmap = wxBitmap(image);
    Refresh();
    wxLogStatus("Edge recognize: Process completed in %ldms...", loadTime);
}

void MyImageFrame::OnResize(wxCommandEvent& WXUNUSED(event))
{
    wxImage image(m_bitmap.ConvertToImage());
    const wxSize size = GetClientSize();
    image.Rescale(size.GetWidth(), size.GetHeight(), wxIMAGE_QUALITY_HIGH);
    m_bitmap = wxBitmap(image);
    UpdateStatusBar();
}

void MyImageFrame::OnZoom(wxCommandEvent& event)
{
    if (event.GetId() == wxID_ZOOM_IN) {
        m_zoom *= 1.2;
    } else if (event.GetId() == wxID_ZOOM_OUT) {
        m_zoom /= 1.2;
    } else {
        m_zoom = 1;
    }
    UpdateStatusBar();
}

void MyImageFrame::OnRotate(wxCommandEvent& event)
{
    double angle = 5;
    if (event.GetId() == ID_ROTATE_LEFT) {
        angle = -angle;
    }
    wxImage image(m_bitmap.ConvertToImage());
    image = image.Rotate(angle, wxPoint(image.GetWidth() / 2, image.GetHeight() / 2));
    if (!image.IsOk()) {
        wxLogWarning("Rotation failed!");
        return;
    }
    m_bitmap = wxBitmap(image);
    UpdateStatusBar();
}

void MyImageFrame::OnGuide(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString array;
    array.Add("Note: this is my C++ class exam demo.");
    array.Add("I`m working on it.");
    wxMessageBox(wxJoin(array, '\n'), "User Guide", wxICON_INFORMATION | wxOK);
}

void MyImageFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString array;
    array.Add("Image Processer demo");
    array.Add("(c) AngelBeats 2020");
    array.Add("Email: angelbeats177@gamil.com");

    array.Add(wxEmptyString);
    array.Add("Version of the wxWidgets libraries used:");
    array.Add(WX_VERSION);
    array.Add("Version of the OpenCV libraries used:");
    array.Add(OPENCV_VERSION);

    (void)wxMessageBox(wxJoin(array, '\n'), "About Image Processer Demo", wxICON_INFORMATION | wxOK);
}

void MyImageFrame::UpdateStatusBar()
{
    wxLogStatus(this, "Image size: (%d, %d), zoom %.2f", m_bitmap.GetWidth(), m_bitmap.GetHeight(), m_zoom);
    Refresh();
}

wxString MyImageFrame::LoadUserImage(wxImage& image)
{
    wxString filename;
    filename = wxLoadFileSelector("Image", wxEmptyString);
    wxStopWatch sw;
    if (!filename.empty()) {
        if (!image.LoadFile(filename)) {
            wxLogError("Couldn`t load image from '%s'.", filename);
            return wxEmptyString;
        }
    }
    const long loadTime = sw.Time();
    wxLogStatus("Image loaded from '%s' in %ldms...", filename, loadTime);
    return filename;
}

bool MyApp::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }
    wxInitAllImageHandlers();

    wxString filename = "./resources/meaqua.jpg";
    wxImage image;
    if (!filename.empty()) {
        if (!image.LoadFile(filename)) {
            wxLogError("Couldn`t load image from '%s'.", filename);
            return false;
        }
    }

    wxFrame* frame = new MyImageFrame((wxFrame*)NULL, filename, image);
    frame->Center();
    frame->Show(true);

    return true;
}

wxIMPLEMENT_APP(MyApp);