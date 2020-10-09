#include <TApplication.h>
#include <TGFrame.h>
// #include <TGClient.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
// #include <TF1.h>
// #include "TCut.h"
// #include <TRandom.h>
// #include <TGButton.h>
#include <TGNumberEntry.h>
// #include <TGTextEntry.h>
// #include <TGSlider.h>
#include "gui_calibrator_ecal.cc"

using namespace std;

class GuiCalibratorEcal : public TGMainFrame
{
private:
    TGMainFrame*         fMain;
    TCanvas*             fCanvas;
    
    // Screen sizes
    Float_t screenHeight()    {return 1080;}
    Float_t rightFrameHeight(){return screenHeight()-100.;}
    Float_t leftFrameHeight() {return screenHeight()-100.;}
    Float_t screenWidth()     {return 1920;}
    Float_t rightFrameWidth() {return 400;}
    Float_t leftFrameWidth()  {return screenWidth()-rightFrameWidth();}
    
    // GUI elements
    void AddCanvasFrame();
    void AddControlPanelFrame();
    
    TNtuple*             fNtHigh;
    TNtuple*             fNtLow;
    TGraph*              fPointsHigh;
    TGraph*              fPointsLow;
    
    // Data
    TGNumberEntry*       fSectorField;
    TGNumberEntry*       fCellField;
    Int_t                fSector;
    Int_t                fCell;
    
    // Histogram
    TGNumberEntry* fNbinsXfield;
    TGNumberEntry* fNbinsYfield;
    Int_t          fNbinsX;
    Int_t          fNbinsY;
    TGTextEntry*   fCut1Field;
    TGTextEntry*   fCut2Field;
    TCut           fCut1;
    TCut           fCut2;
    TCut           fCut;
    TH2F*          fHistLow;
    TH2F*          fHistHigh;
    
    // Graph
    Bool_t         fIsCheckEachSlice;
    TGraphErrors*  fGrHigh;
    TGraphErrors*  fGrLow;
    TGraphErrors*  fGr;
    TGNumberEntry* fNsliceCutField;
    Int_t          fNsliceCut;
    //TGHSlider*     fHslider;
    TGNumberEntry* fLowBorderField;
    Float_t        fGraphLowBorder;
    TGNumberEntry* fHighBorderField;
    Float_t        fGraphHighBorder;
    TGraph*        fGrCheckHigh;
    TGraph*        fGrCheckLow;
    
    // Fit
    TGNumberEntry* fP0Field;
    TGNumberEntry* fP1Field;
    TGNumberEntry* fP2Field;
    TGNumberEntry* fP3Field;
    Double_t       fPar0;
    Double_t       fPar1;
    Double_t       fPar2;
    Double_t       fPar3;
    TF1*           fFitFunction;
    
    // Output
    FILE*          fFileCalibParams;
    FILE*          fFileCalibQuality;
    TFile*         fFileOut;
    
public:
    // 
    void PrintInfo();
    void ClearObjects();
    void SetDefaults();
    
    // General
    void SetSector();
    void SetCell();
    void OpenData();
    void Next();
    
    // Histograms
    void SetNbinsX();
    void SetNbinsY();
    void SetCut1();
    void SetCut2();
    void DrawHistograms();
    void MakeHistograms();
    
    // Graphs
    void DoCheckEachSlice(Bool_t on){fIsCheckEachSlice=on; cout << "State changed to " << (on?"ON":"OFF")<<endl;}
    void MakeGraphs();
    void DrawGraphs();
    void SetNsliceCut();
    void SetLowBorder();
    void SetHighBorder();
    
    // Fit
    void DrawFit();    //draw fitfuncion onto graphs
    void FitGraph();   //fit graph
    void FitPanel();   //fit panel
    
    // Quality
    void DrawCheck();
    void Check();
    void WriteGoodQuality();
    void WriteUncalibratedQuality();
    void WriteBadQuality();
    
    // Write
    void SaveCanvas();
    void WriteGraphs();
    void WriteParameters();
    void Save();
    
    
    // Exit
    void DoExit();
    
    GuiCalibratorEcal(const TGWindow *p,UInt_t w,UInt_t h, Int_t sector, Int_t cell);
    virtual ~GuiCalibratorEcal();
    ClassDef(GuiCalibratorEcal,0)
};
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::PrintInfo()
{
    cout << endl;
    cout << "fSector = " << fSector << endl;
    cout << "fCell   = " << fCell << endl;
    cout << "fNbinsX = " << fNbinsX << endl;
    cout << "fNbinsY = " << fNbinsY << endl;
    cout << "fCut    = " << (fCut.GetTitle()) << endl;
    cout << endl;
    cout << "fNsliceCut   = " << fNsliceCut << endl;
    cout << "graph borders: " << fGraphLowBorder << " - " << fGraphHighBorder << endl;
    cout << endl;
    cout << "fFileOut:    " << (fFileOut==NULL?"NULL":"Object")<<endl;
    cout << "fHistLow :   " << (fHistLow ==NULL?"NULL":"Object")<<endl;
    cout << "fHistHigh:   " << (fHistHigh==NULL?"NULL":"Object")<<endl;
    cout << "fPointsHigh: " << (fPointsHigh==NULL?"NULL":"Object")<<endl;
    cout << "fPointsLow:  " << (fPointsLow==NULL?"NULL":"Object")<<endl;
    cout << endl;
    cout << "fGrHigh:     " << (fGrHigh==NULL?"NULL":"Object")<<endl;
    cout << "fGrLow:      " << (fGrLow==NULL?"NULL":"Object")<<endl;
    cout << "fGr:         " << (fGr==NULL?"NULL":"Object")<<endl;
    cout << "fGrCheckHigh:" << (fGrCheckHigh==NULL?"NULL":"Object")<<endl;
    cout << "fGrCheckLow: " << (fGrCheckLow==NULL?"NULL":"Object")<<endl;
    
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::ClearObjects()
{
    if(fCanvas     !=NULL)fCanvas->Clear();
    //if(fFitFunction!=NULL){delete fFitFunction;fFitFunction = NULL;}
    if(fGrHigh     !=NULL){delete fGrHigh     ;fGrHigh      = NULL;}
    if(fGrLow      !=NULL){delete fGrLow      ;fGrLow       = NULL;}
    if(fGr         !=NULL){delete fGr         ;fGr          = NULL;}
    if(fHistLow    !=NULL){delete fHistLow    ;fHistLow     = NULL;}
    if(fHistHigh   !=NULL){delete fHistHigh   ;fHistHigh    = NULL;}
    if(fPointsHigh !=NULL){delete fPointsHigh ;fPointsHigh  = NULL;}
    if(fPointsLow  !=NULL){delete fPointsLow  ;fPointsLow   = NULL;}
    if(fNtHigh     !=NULL){delete fNtHigh     ;fNtHigh      = NULL;}
    if(fNtLow      !=NULL){delete fNtLow      ;fNtLow       = NULL;}
    if(fGrCheckHigh!=NULL){delete fGrCheckHigh;fGrCheckHigh = NULL;}
    if(fGrCheckLow !=NULL){delete fGrCheckLow ;fGrCheckLow  = NULL;}
    PrintInfo();
    cout << "Objects cleared\n";
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
GuiCalibratorEcal::GuiCalibratorEcal(const TGWindow *p,UInt_t w,UInt_t h,Int_t sector=1,Int_t cell=0) : TGMainFrame(p,w,h)
{
    const char* outfilename="out.root";
    
    //fFitFunction = new TF1("f0","TMath::Exp([0]+x*[1])+[2]",0,1600);//+[3]*x
    fFitFunction = new TF1("f0","[0]+TMath::Exp([1]+x*[2])",0,1600);//+[3]*x
    
    // Histograms
    fNbinsX = 64;
    fNbinsY = 64;
    fCut1 = "emc_mq<2.0 && size==1 && emc_path>91 && chi2<250";
    fCut2 = "";
    fCut  = fCut1 && fCut2;
    
    // Graphs
    fNsliceCut = 100;
    fGraphLowBorder = 600;
    fGraphHighBorder = 600;
    fIsCheckEachSlice=kFALSE;
    
    SetDefaults();
    //ClearObjects();
    fSector = sector;
    fCell   = cell;
    
    fFileCalibParams = fopen("ECal_parameters.txt","a");
    printHeader(fFileCalibParams);
    fFileCalibQuality = fopen("Calibrated_modules.txt","a");
    fprintf(fFileCalibQuality,"\n-------------------------------------\n");
    
    fFileOut    = new TFile(outfilename,"RECREATE");
    fNtHigh     = NULL;
    fNtLow      = NULL;
    fHistHigh   = NULL;
    fHistLow    = NULL;
    fPointsHigh = NULL;
    fPointsLow  = NULL;
    fCanvas     = NULL;
    fGrHigh     = NULL;
    fGrLow      = NULL;
    fGr         = NULL;
    fGrCheckHigh= NULL;
    fGrCheckLow = NULL;
    
    fIsCheckEachSlice=kFALSE;
    
    fFitFunction->SetParameters(fPar0,fPar1,fPar2);//,fPar3);
    //fFitFunction->SetParLimits(0,1E-5,1E-3);
    fFitFunction->SetLineColor(kRed);
    fFitFunction->SetLineWidth(2);
    
    fMain = new TGMainFrame(p,w,h,kMainFrame | kHorizontalFrame);
    
    //**********************
    AddCanvasFrame();
    AddControlPanelFrame();
    //**********************
    
    fMain->SetWindowName("ECal calibrator");// Set a name to the main frame
    fMain->MapSubwindows();// Map all subwindows of main frame
    fMain->Resize(1920,980);// Initialize the layout algorithm
    fMain->MapWindow();// Map main frame
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::AddCanvasFrame()
{
    TGVerticalFrame *vframeLeft = new TGVerticalFrame(fMain,leftFrameWidth(),leftFrameHeight());
    {
        TRootEmbeddedCanvas* eCanvas = new TRootEmbeddedCanvas("Ecanvas",vframeLeft,leftFrameWidth()-4,leftFrameHeight()-4);
        fCanvas = eCanvas->GetCanvas();
        vframeLeft->AddFrame(eCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 1,1,1,1));
    }
    fMain->AddFrame(vframeLeft, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2,2,2,2));
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::AddControlPanelFrame()
{
    {
        TGVerticalFrame *vframeRight = new TGVerticalFrame(fMain,rightFrameWidth(),rightFrameHeight());
        {
            //___________________________________________________________________________________________________________
            // Main parameters ------------------------------------------------------------------------------------------
            TGGroupFrame* frameGeneral = new TGGroupFrame(vframeRight,"General parameters",kVerticalFrame);
            {
                // Sector
                fSectorField = new TGNumberEntry(frameGeneral,fSector,5,-1,
                                                 TGNumberFormat::kNESInteger,
                                                 TGNumberFormat::kNEANonNegative,
                                                 TGNumberFormat::kNELLimitMinMax,
                                                 0,5);
                fSectorField->Connect("ValueSet(Long_t)", "GuiCalibratorEcal", this, "SetSector()");
                fSectorField->GetNumberEntry()->Connect("ReturnPressed()", "GuiCalibratorEcal", this, "SetSector()");
                frameGeneral->AddFrame(fSectorField, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                
                // Cell
                fCellField = new TGNumberEntry(frameGeneral,fCell,5,-1,
                                               TGNumberFormat::kNESInteger,
                                               TGNumberFormat::kNEANonNegative,
                                               TGNumberFormat::kNELLimitMinMax,
                                               0,163);
                fCellField->Connect("ValueSet(Long_t)", "GuiCalibratorEcal", this, "SetCell()");
                fCellField->GetNumberEntry()->Connect("ReturnPressed()", "GuiCalibratorEcal", this, "SetCell()");
                frameGeneral->AddFrame(fCellField, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                
                // Print info
                TGTextButton *infoButton = new TGTextButton(frameGeneral,"&Print info");
                infoButton->Connect("Clicked()","GuiCalibratorEcal",this,"PrintInfo()");
                frameGeneral->AddFrame(infoButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                
                // Open data
                TGTextButton *openButton = new TGTextButton(frameGeneral,"&Open data");
                openButton->Connect("Clicked()","GuiCalibratorEcal",this,"OpenData()");
                frameGeneral->AddFrame(openButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                
                TGTextButton *nextButton = new TGTextButton(frameGeneral,"&Next");
                nextButton->Connect("Clicked()","GuiCalibratorEcal",this,"Next()");
                frameGeneral->AddFrame(nextButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                
                // Clear data
                TGTextButton *clearButton = new TGTextButton(frameGeneral,"Clear data");
                clearButton->Connect("Clicked()","GuiCalibratorEcal",this,"ClearObjects()");
                frameGeneral->AddFrame(clearButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
            }
            vframeRight->AddFrame(frameGeneral, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,20));
            
            //___________________________________________________________________________________________________________
            // Histogram ------------------------------------------------------------------------------------------------
            TGGroupFrame* frameHistogram = new TGGroupFrame(vframeRight,"Histogram panel",kVerticalFrame);
            {
                // nBinsX
                {
                    fNbinsXfield = new TGNumberEntry(frameHistogram,fNbinsX,5,-1,
                                                     TGNumberFormat::kNESInteger,
                                                     TGNumberFormat::kNEAPositive,
                                                     TGNumberFormat::kNELLimitMin,
                                                     0);
                    fNbinsXfield->Connect("ValueSet(Long_t)", "GuiCalibratorEcal", this, "SetNbinsX()");
                    fNbinsXfield->GetNumberEntry()->Connect("ReturnPressed()", "GuiCalibratorEcal", this, "SetNbinsX()");
                    frameHistogram->AddFrame(fNbinsXfield, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                }
                // nBinsY
                {
                    fNbinsYfield = new TGNumberEntry(frameHistogram,fNbinsY,5,-1,
                                                     TGNumberFormat::kNESInteger,
                                                     TGNumberFormat::kNEAPositive,
                                                     TGNumberFormat::kNELLimitMin,
                                                     0);
                    fNbinsYfield->Connect("ValueSet(Long_t)", "GuiCalibratorEcal", this, "SetNbinsY()");
                    fNbinsYfield->GetNumberEntry()->Connect("ReturnPressed()", "GuiCalibratorEcal", this, "SetNbinsY()");
                    frameHistogram->AddFrame(fNbinsYfield, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                }
                // Cut1
                {
                    fCut1Field = new TGTextEntry(frameHistogram,fCut1.GetTitle());
                    fCut1Field->Connect("ReturnPressed()", "GuiCalibratorEcal", this, "SetCut1()");
                    frameHistogram->AddFrame(fCut1Field, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                }
                // Cut2
                {
                    fCut2Field = new TGTextEntry(frameHistogram,fCut2.GetTitle());
                    fCut2Field->Connect("ReturnPressed()", "GuiCalibratorEcal", this, "SetCut2()");
                    frameHistogram->AddFrame(fCut2Field, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                }
                // Histograms
                {
                    TGTextButton *histButton = new TGTextButton(frameHistogram,"Make &histograms");
                    histButton->Connect("Clicked()","GuiCalibratorEcal",this,"MakeHistograms()");
                    frameHistogram->AddFrame(histButton, new TGLayoutHints(kLHintsExpandY | kLHintsExpandX, 2,2,2,2));
                }
            }
            vframeRight->AddFrame(frameHistogram, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
            
            //___________________________________________________________________________________________________________
            // Graph ----------------------------------------------------------------------------------------------------
            TGGroupFrame* frameGraph = new TGGroupFrame(vframeRight,"Graph panel",kVerticalFrame);
            {
                // Flag check each slice
                {
                    TGCheckButton *checkButton = new TGCheckButton(frameGraph,"Check each slice");
                    //checkButton->SetTextJustify(36);
                    checkButton->SetMargins(10,10,10,10);
                    checkButton->SetWrapLength(-1);
                    checkButton->SetState(kButtonUp);
                    checkButton->Connect("Toggled(Bool_t)","GuiCalibratorEcal",this,"DoCheckEachSlice(Bool_t)");
                    frameGraph->AddFrame(checkButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
                    checkButton->Resize(400,50);
                }
                // N hits in slice
                {
                    fNsliceCutField = new TGNumberEntry(frameGraph,fNsliceCut,5,-1,
                                                        TGNumberFormat::kNESInteger,
                                                        TGNumberFormat::kNEAPositive,
                                                        TGNumberFormat::kNELLimitMin,
                                                        0);
                    fNsliceCutField->Connect("ValueSet(Long_t)", "GuiCalibratorEcal", this, "SetNsliceCut()");
                    //fNsliceCutField->GetNumberEntry()->Connect("ReturnPressed()", "GuiCalibratorEcal", this, "SetNsliceCut()");
                    frameGraph->AddFrame(fNsliceCutField, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                }
                // Join graphs
//                 if(0)
//                 {
//                     TGHSlider* fHslider = new TGHSlider(frameGraph,150,kSlider1|kScaleDownRight,1);
//                     fHslider->SetRange(0,1600);
//                     fHslider->SetPosition(700);
//                     fHslider->Connect("PositionChanged(Int_t)", "GuiCalibratorEcal",this,"ChangeBorder()");
//                     frameGraph->AddFrame(fHslider, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
//                 }
                {
                    fLowBorderField = new TGNumberEntry(frameGraph,fGraphLowBorder,5,-1,
                                                        TGNumberFormat::kNESReal,
                                                        TGNumberFormat::kNEAPositive,
                                                        TGNumberFormat::kNELLimitMin,
                                                        0);
                    fLowBorderField->Connect("ValueSet(Long_t)", "GuiCalibratorEcal", this, "SetLowBorder()");
                    //fBorderField->GetNumberEntry()->Connect("ReturnPressed()", "GuiCalibratorEcal", this, "SetDeviation()");
                    frameGraph->AddFrame(fLowBorderField, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                }
                {
                    fHighBorderField = new TGNumberEntry(frameGraph,fGraphHighBorder,5,-1,
                                                     TGNumberFormat::kNESReal,
                                                     TGNumberFormat::kNEAPositive,
                                                     TGNumberFormat::kNELLimitMin,
                                                     0);
                    fHighBorderField->Connect("ValueSet(Long_t)", "GuiCalibratorEcal", this, "SetHighBorder()");
                    //fBorderField->GetNumberEntry()->Connect("ReturnPressed()", "GuiCalibratorEcal", this, "SetDeviation()");
                    frameGraph->AddFrame(fHighBorderField, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5,5,2,2));
                }
                // Graphs
                {
                    TGTextButton *graphButton = new TGTextButton(frameGraph,"Make &graphs");
                    graphButton->Connect("Clicked()","GuiCalibratorEcal",this,"MakeGraphs()");
                    frameGraph->AddFrame(graphButton, new TGLayoutHints(kLHintsExpandY | kLHintsExpandX, 2,2,2,2));
                }
            }
            vframeRight->AddFrame(frameGraph, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
            
            //___________________________________________________________________________________________________________
            // Fit ------------------------------------------------------------------------------------------------------
            TGGroupFrame* frameFit = new TGGroupFrame(vframeRight,"Fit panel",kHorizontalFrame);
            {
                // Set defaults
                {
                    TGTextButton *defaultsButton = new TGTextButton(frameFit,"Set &defaults");
                    defaultsButton->Connect("Clicked()","GuiCalibratorEcal",this,"SetDefaults()");
                    frameFit->AddFrame(defaultsButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                }
                // Fit graph
                {
                    TGTextButton *fitGraphButton = new TGTextButton(frameFit,"&Fit Graph");
                    fitGraphButton->Connect("Clicked()","GuiCalibratorEcal",this,"FitGraph()");
                    frameFit->AddFrame(fitGraphButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                }
                // Fit graph
                {
                    TGTextButton *fitPanelButton = new TGTextButton(frameFit,"&Fit Panel");
                    fitPanelButton->Connect("Clicked()","GuiCalibratorEcal",this,"FitPanel()");
                    frameFit->AddFrame(fitPanelButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                }
            }
            vframeRight->AddFrame(frameFit, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,20));
            
            //___________________________________________________________________________________________________________
            // Mark calibration quality ---------------------------------------------------------------------------------
            TGGroupFrame* frameQuality = new TGGroupFrame(vframeRight,"Mark calibration quality",kVerticalFrame);
            {
                TGTextButton *checkButton = new TGTextButton(frameQuality,"Check calibration");
                checkButton->Connect("Clicked()","GuiCalibratorEcal",this,"Check()");
                frameQuality->AddFrame(checkButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,20));
                
                TGHorizontalFrame *hQualityFrame = new TGHorizontalFrame(frameQuality,rightFrameWidth(),rightFrameHeight());
                {
                    // Save canvas
                    TGTextButton *goodButton = new TGTextButton(hQualityFrame,"Good");
                    goodButton->Connect("Clicked()","GuiCalibratorEcal",this,"WriteGoodQuality()");
                    hQualityFrame->AddFrame(goodButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                    // Write parameters
                    TGTextButton *uncalibratedButton = new TGTextButton(hQualityFrame,"Uncalibrated");
                    uncalibratedButton->Connect("Clicked()","GuiCalibratorEcal",this,"WriteUncalibratedQuality()");
                    hQualityFrame->AddFrame(uncalibratedButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                    // Write trees
                    TGTextButton *badButton = new TGTextButton(hQualityFrame,"Bad");
                    badButton->Connect("Clicked()","GuiCalibratorEcal",this,"WriteBadQuality()");
                    hQualityFrame->AddFrame(badButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                }
                frameQuality->AddFrame(hQualityFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
            }
            vframeRight->AddFrame(frameQuality, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,20));
            
            //___________________________________________________________________________________________________________
            // Save results ---------------------------------------------------------------------------------------------
            TGGroupFrame* frameSave = new TGGroupFrame(vframeRight,"Save results",kVerticalFrame);
            {
                TGHorizontalFrame *hSaveFrame = new TGHorizontalFrame(frameSave,rightFrameWidth(),rightFrameHeight());
                {
                    // Save canvas
                    TGTextButton *saveButton = new TGTextButton(hSaveFrame,"Canvas");
                    saveButton->Connect("Clicked()","GuiCalibratorEcal",this,"SaveCanvas()");
                    hSaveFrame->AddFrame(saveButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                    // Write parameters
                    TGTextButton *writeParametersButton = new TGTextButton(hSaveFrame,"Parameters");
                    writeParametersButton->Connect("Clicked()","GuiCalibratorEcal",this,"WriteParameters()");
                    hSaveFrame->AddFrame(writeParametersButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                    // Write trees
                    TGTextButton *writeGraphsButton = new TGTextButton(hSaveFrame,"Trees");
                    writeGraphsButton->Connect("Clicked()","GuiCalibratorEcal",this,"WriteGraphs()");
                    hSaveFrame->AddFrame(writeGraphsButton, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                }
                frameSave->AddFrame(hSaveFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,2));
                // Save all
                TGTextButton *saveAllButon = new TGTextButton(frameSave,"&Save all");
                saveAllButon->Connect("Clicked()","GuiCalibratorEcal",this,"Save()");
                frameSave->AddFrame(saveAllButon, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,20));
            }
            vframeRight->AddFrame(frameSave, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,20));
            
            //___________________________________________________________________________________________________________
            // Check ----------------------------------------------------------------------------------------------------
            TGGroupFrame* frameCheck = new TGGroupFrame(vframeRight,"Check panel",kVerticalFrame);
            {
                
                // Draw histograms TODO
                
                // Draw graphs TODO
                
                // Draw resolution TODO
            }
            vframeRight->AddFrame(frameCheck, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 2,2,2,20));
            //___________________________________________________________________________________________________________
            // Exit -----------------------------------------------------------------------------------------------------
            TGGroupFrame* frameExit = new TGGroupFrame(vframeRight,"Exit panel",kVerticalFrame);
            {
                TGTextButton *exitButton = new TGTextButton(frameExit,"&Exit");
                exitButton->Connect("Clicked()","GuiCalibratorEcal",this,"DoExit()");
                frameExit->AddFrame(exitButton, new TGLayoutHints(kLHintsExpandY | kLHintsExpandX, 2,2,2,2));
            }
            vframeRight->AddFrame(frameExit, new TGLayoutHints(kLHintsExpandX | kLHintsBottom, 2,2,2,2));
            
        }
        fMain->AddFrame(vframeRight, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 2,2,2,2));
    }
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::SetDefaults()
{
    // Fit
    fPar1 = 3.664;
    fPar2 = 2.082E-3;
    fPar0 = -200.5;
    
    if(fFitFunction!=NULL)fFitFunction->SetParameters(fPar0,fPar1,fPar2);
    //PrintInfo();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::SetSector()
{
    fSector = fSectorField->GetNumberEntry()->GetIntNumber();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::SetCell()
{
    fCell = fCellField->GetNumberEntry()->GetIntNumber();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::OpenData()
{
    if(fSector==0 || fSector==3 || fSector<0 || fSector>5)return;
    if(fCell<0 || fCell>162)return;
    if(fSector==1 && fCell>4 && fCell<131)return;
    
    Int_t colorLow = kGreen+1;
    Int_t colorHigh = kAzure;
    
    ClearObjects();
    
    fNtHigh = treeToNt(fFileOut, fSector,fCell, ""); fNtHigh->Show();
    fNtLow = treeToNtLow(fFileOut, fSector,fCell,"");fNtLow->Show();
    
    fHistHigh = ntToHist(fNtHigh/*,fFitFunction,fDeviation*/, ""); 
    fHistHigh->SetNameTitle(Form("histHigh_%i_%i",fSector,fCell), Form("High field %i sector %i cell;TOT[ns];p[MeV]",fSector,fCell));
    fHistLow  = ntToHist(fNtLow/*, fFitFunction,fDeviation*/, ""); 
    fHistLow ->SetNameTitle(Form("histLow_%i_%i",fSector,fCell),  Form("Low field %i sector %i cell;TOT[ns];p[MeV]",fSector,fCell));
    
    fCanvas->Clear();
    fCanvas->cd();
    fNtHigh->Draw("mom:TOT");
    fPointsHigh = new TGraph(fNtHigh->GetSelectedRows(),fNtHigh->GetV2(),fNtHigh->GetV1());
    fPointsHigh->SetMarkerColor(colorHigh);
    fPointsHigh->SetMarkerStyle(6);
    fNtLow->Draw("mom:TOT");
    fPointsLow = new TGraph(fNtLow->GetSelectedRows(),fNtLow->GetV2(),fNtLow->GetV1());
    fPointsLow->SetMarkerColor(colorLow);
    fPointsLow->SetMarkerStyle(6);
    DrawHistograms();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::Next()
{
    if(fSector==5 && fCell==162)return;
    if(fSector==0){fSector=1;fCell=0;OpenData();return;}
    if(fSector==3 || (fSector==2 && fCell==162)){fSector=4;fCell=0;OpenData();return;}
    if(fSector==1 && fCell>=4 && fCell<131){fCell=131;OpenData();return;}
    if(fCell==162){++fSector;fCell=0;OpenData();return;}
    ++fCell;OpenData();return;
}
//------------------------------------------------------------------------------------------------------------------

//##################################################################################################################

//==================================================================================================================
void GuiCalibratorEcal::SetNbinsX()
{
    fNbinsX = fNbinsXfield->GetNumberEntry()->GetIntNumber();
    //MakeHistograms();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::SetNbinsY()
{
    fNbinsY = fNbinsYfield->GetNumberEntry()->GetIntNumber();
    //MakeHistograms();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::SetCut1()
{
    fCut1 = fCut1Field->GetBuffer()->GetString();
    fCut=fCut1 && fCut2;
    cout << "Cut: " << (fCut.GetTitle()) << endl;
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::SetCut2()
{
    fCut2 = fCut2Field->GetBuffer()->GetString();
    fCut=fCut1 && fCut2;
    cout << "Cut: " << (fCut.GetTitle()) << endl;
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::DrawHistograms()
{
    if(fPointsHigh==NULL || fPointsLow==NULL || fHistHigh==NULL || fHistLow==NULL)return;
    fCanvas->Clear();
    fCanvas->Divide(2,2);
    fCanvas->cd(1);
    fPointsHigh->Draw("AP");
    fCanvas->cd(2);
    fPointsLow->Draw("AP");
    fCanvas->cd(3)->SetLogz(1);
    fHistHigh->Draw("colz");
    fCanvas->cd(4)->SetLogz(1);
    fHistLow->Draw("colz");
    fCanvas->Update();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::MakeHistograms()
{
    if(fNtHigh==NULL || fNtLow==NULL)OpenData();
    fHistHigh->Delete(); fHistLow->Delete();
    fHistHigh = ntToHist(fNtHigh/*,fFitFunction,fDeviation*/,fCut,fNbinsX,fNbinsY); 
    fHistHigh->SetNameTitle(Form("histHigh_%i_%i",fSector,fCell), Form("High field %i sector %i cell;TOT[ns];p[MeV]",fSector,fCell));
    fHistLow  = ntToHist(fNtLow/*, fFitFunction,fDeviation*/,fCut,fNbinsX,fNbinsY); 
    fHistLow ->SetNameTitle(Form("histLow_%i_%i",fSector,fCell),  Form("Low field %i sector %i cell;TOT[ns];p[MeV]",fSector,fCell));
    DrawHistograms();
}
//------------------------------------------------------------------------------------------------------------------

//##################################################################################################################

//==================================================================================================================
void GuiCalibratorEcal::DrawGraphs()
{
    if(fGrHigh==NULL || fGrLow==NULL || fGr==NULL)return;
    fCanvas->Clear();
    fCanvas->Divide(2,1);
    fCanvas->cd(1)->Divide(1,2);
    fCanvas->cd(1)->cd(1)->SetLogz(1);
    fHistHigh->Draw("colz");
    fGrHigh->Draw("same P");
    fCanvas->cd(1)->cd(2)->SetLogz(1);
    fHistLow->Draw("colz");
    fGrLow->Draw("same P");
    fCanvas->cd(2);
    //fGr->Draw("AP");
    fPointsHigh->Draw("same AP");
    fPointsHigh->GetXaxis()->SetRangeUser(0,1600);
    fPointsHigh->GetYaxis()->SetRangeUser(0,1200);
    fPointsLow->Draw("same P");
    fGr->Draw("same P");
    fCanvas->Update();
    
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::SetNsliceCut()
{
    fNsliceCut = fNsliceCutField->GetNumberEntry()->GetIntNumber();
    //MakeGraphs();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::MakeGraphs()
{
    if(fHistHigh==NULL || fHistLow==NULL)MakeHistograms();
    if(fGrHigh!=NULL)delete fGrHigh;
    if(fGrLow !=NULL)delete fGrLow;
    if(fGr    !=NULL)delete fGr;
    fGrHigh = histToGraph(fHistHigh, fCanvas, fNsliceCut, fIsCheckEachSlice);
    fGrLow  = histToGraph(fHistLow,  fCanvas, fNsliceCut, fIsCheckEachSlice);
    fGr = joinGraphs(fGrLow,fGrHigh, fGraphLowBorder,fGraphHighBorder);
    fGr->SetNameTitle("fGr","Joint low & high field graphs;TOT[ns];p[MeV/c]");
    
    DrawGraphs();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::SetLowBorder()
{
    if(fGrHigh==NULL || fGrLow==NULL)return;
    fGraphLowBorder = fLowBorderField->GetNumber();
    if(fGraphLowBorder>fGraphHighBorder)fGraphHighBorder=fGraphLowBorder;
    if(fGr!=NULL)delete fGr;
    fGr = joinGraphs(fGrLow,fGrHigh, fGraphLowBorder,fGraphHighBorder);
    DrawGraphs();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::SetHighBorder()
{
    if(fGrHigh==NULL || fGrLow==NULL)return;
    fGraphHighBorder = fHighBorderField->GetNumber();
    if(fGraphHighBorder<fGraphLowBorder)fGraphLowBorder=fGraphHighBorder;
    if(fGr!=NULL)delete fGr;
    fGr = joinGraphs(fGrLow,fGrHigh, fGraphLowBorder,fGraphHighBorder);
    DrawGraphs();
}
//------------------------------------------------------------------------------------------------------------------

//##################################################################################################################

//==================================================================================================================
void GuiCalibratorEcal::DrawCheck()
{
    if(fGrCheckHigh==NULL || fGrCheckLow==NULL)return;
    fCanvas->Clear();
    fCanvas->cd();
    fGrCheckHigh->Draw("AP");
    fGrCheckHigh->GetXaxis()->SetRangeUser(0,1500);
    fGrCheckHigh->GetYaxis()->SetRangeUser(-500,500);
    fGrCheckLow->Draw("P same");
    
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::Check()
{
    Int_t colorLow = kGreen+1;
    Int_t colorHigh = kAzure;
    
    fNtLow = ntToNt(fNtLow,fFitFunction);
    fNtHigh = ntToNt(fNtHigh,fFitFunction);
    
    fCanvas->Clear();
    fCanvas->cd();
    fNtHigh->Draw("E_calc-mom:mom", fCut1);
    fGrCheckHigh = new TGraph(fNtHigh->GetSelectedRows(),fNtHigh->GetV2(),fNtHigh->GetV1());
    fGrCheckHigh->SetMarkerColor(colorHigh);
    fGrCheckHigh->SetMarkerStyle(6);
    fGrCheckHigh->SetTitle(Form("Check %i_%i;p[MeV/c];E_{CALC}[MeV]",fSector,fCell));
    
    fNtLow->Draw("E_calc-mom:mom", fCut1);
    fGrCheckLow  = new TGraph(fNtLow->GetSelectedRows(),fNtLow->GetV2(),fNtLow->GetV1());
    fGrCheckLow->SetMarkerColor(colorLow);
    fGrCheckLow->SetMarkerStyle(6);
    
    TF1* zero = new TF1("zero","pol0",0,1500);
    zero->SetParameter(0,0);
    zero->SetLineColor(kRed);
    zero->SetLineWidth(2);
    
    fCanvas->Clear();
    fCanvas->Divide(2,2);
    fCanvas->cd(2);
    fGrCheckHigh->Draw("AP");
    fGrCheckHigh->GetXaxis()->SetRangeUser(0,1500);
    fGrCheckHigh->GetYaxis()->SetRangeUser(-1000,1000);
    fGrCheckLow->Draw("P same");
    //fGrCheckLow->GetXaxis()->SetRangeUser(0,1500);
    //fGrCheckLow->GetYaxis()->SetRangeUser(-500,500);
    zero->Draw("same");
    
    fCanvas->cd(1);
    TH2F* h = new TH2F(Form("hCheck_%i_%i",fSector,fCell),Form("Sector %i Cell %i;p[MeV/c];E_{Calc}-pc[MeV]",fSector,fCell),100,0,1500,100,-500,500);
    fNtHigh->Draw(Form("E_calc-mom:mom>>hCheck_%i_%i",fSector,fCell),fCut1,"colz");
    fNtLow->Draw(Form("E_calc-mom:mom>>+hCheck_%i_%i",fSector,fCell),fCut1,"colz");
    zero->Draw("same");
    
    TMultiGraph* mg = new TMultiGraph();
    fCanvas->cd(4);
    fNtHigh->Draw(Form("log(mom-%f):TOT",fFitFunction->GetParameter(0)), Form("log(mom-%f)>0",fFitFunction->GetParameter(0)) && fCut1);
    TGraph* fGrCheckLinearHigh  = new TGraph(fNtHigh->GetSelectedRows(),fNtHigh->GetV2(),fNtHigh->GetV1());
    fGrCheckLinearHigh->SetMarkerColor(colorHigh);
    fGrCheckLinearHigh->SetMarkerStyle(6);
    mg->Add(fGrCheckLinearHigh,"P");
    
    fCanvas->cd(4);
    fNtLow->Draw(Form("log(mom-%f):TOT",fFitFunction->GetParameter(0)), Form("log(mom-%f)>0",fFitFunction->GetParameter(0)) && fCut1);
    TGraph* fGrCheckLinearLow  = new TGraph(fNtLow->GetSelectedRows(),fNtLow->GetV2(),fNtLow->GetV1());
    fGrCheckLinearLow->SetMarkerColor(colorLow);
    fGrCheckLinearLow->SetMarkerStyle(6);
    mg->Add(fGrCheckLinearLow,"P");
    
    TF1* line = new TF1("zero","pol1",0,1500);
    line->SetParameter(0,fFitFunction->GetParameter(1));
    line->SetParameter(1,fFitFunction->GetParameter(2));
    line->SetLineColor(kRed);
    line->SetLineWidth(2);
    
    fCanvas->cd(4);
//     fGrCheckLinearHigh->Draw("AP");
//     fGrCheckLinearHigh->GetXaxis()->SetRangeUser(0,1500);
//     fGrCheckLinearHigh->GetYaxis()->SetRangeUser(-1,15);
//     fGrCheckLinearLow->Draw("P same");
    mg->Draw("APL");
    line->Draw("same");
    //mg->Fit("pol1");
    
    
    //fCanvas->cd(4);
    
    fCanvas->Update();
    h->SetShowProjectionY(5);
    
    fCanvas->WaitPrimitive();
    
    mg->RecursiveRemove(fGrCheckLinearHigh);
    mg->RecursiveRemove(fGrCheckLinearLow);
    delete mg;
    delete line;
    
    //DrawCheck();
}
//------------------------------------------------------------------------------------------------------------------

//##################################################################################################################

//==================================================================================================================
void GuiCalibratorEcal::DrawFit()
{
    
    fCanvas->Clear();
    fCanvas->Divide(2,1);
    fCanvas->cd(1)->Divide(1,2);
    fCanvas->cd(1)->cd(1)->SetLogz(1);
    fHistHigh->Draw("colz");
    fGrHigh->Draw("same P");
    fFitFunction->Draw("same");
    fCanvas->cd(1)->cd(2)->SetLogz(1);
    fHistLow->Draw("colz");
    fGrLow->Draw("same P");
    fFitFunction->Draw("same");
    fCanvas->cd(2);
    fPointsHigh->Draw("AP");
    fPointsHigh->GetXaxis()->SetRangeUser(0,1600);
    fPointsHigh->GetYaxis()->SetRangeUser(0,1200);
    fPointsLow->Draw("same P");
    fGr->Draw("same P");
    fFitFunction->Draw("same");
    fCanvas->Update();
    
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::FitGraph()
{
    if(fGr==NULL)return;
    //TFitResultPtr r = fGr->Fit("expo","S");
    //fFitFunction->SetParameter(0,r->Parameter(0));
    //fFitFunction->SetParameter(1,r->Parameter(1));
    //fCanvas->WaitPrimitive();
    fGr->Fit(fFitFunction,"BM","",fGr->GetX()[0],fGr->GetX()[fGr->GetN()-1]);
    //fNtLow = ntToNt(fNtLow,fFitFunction);
    //fNtHigh = ntToNt(fNtHigh,fFitFunction);
    DrawFit();
    //fP0Field->SetNumber(fPar0);fP0Field->SetState();
    //fP1Field->SetNumber(fPar1);fP1Field->SetState();
    //fP2Field->SetNumber(fPar2);fP2Field->SetState();
}

void GuiCalibratorEcal::FitPanel()
{
    fGr->FitPanel();
}
//------------------------------------------------------------------------------------------------------------------

//##################################################################################################################

//==================================================================================================================
void GuiCalibratorEcal::SaveCanvas()
{
    fCanvas->SaveAs(Form("pic/ECal_%i_%i.gif",fSector,fCell));
    cout << "Canvas " << fSector << " - " << fCell << " is saved\n";
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::WriteGraphs()
{
    fNtLow = ntToNt(fNtLow,fFitFunction);
    fNtHigh = ntToNt(fNtHigh,fFitFunction);
    
    TFile* out = new TFile(Form("tree/%i_%i.root",fSector,fCell),"RECREATE");
    fNtHigh->SetName("fNtHigh");
    out->cd();
    fNtHigh->Write();
    fNtLow->SetName("fNtLow");
    out->cd();
    fNtLow->Write();
    cout << "Trees are saved\n";
    
    fGrHigh->SetName("fGrHigh");
    out->cd();
    fGrHigh->Write();
    fGrLow->SetName("fGrLow");
    out->cd();
    fGrLow->Write();
    out->cd();
    fGr->Write();
    cout << "Graphs are saved\n";
    
    fGrCheckHigh->SetName("fGrCheckHigh");
    out->cd();
    fGrCheckHigh->Write();
    fGrCheckLow->SetName("fGrCheckLow");
    out->cd();
    fGrCheckLow->Write();
    
    out->cd();
    fHistHigh->Write();
    out->cd();
    fHistLow->Write();
    cout << "Histograms are saved\n";
    
    fPointsHigh->SetName("fPointsHigh");
    out->cd();
    fPointsHigh->Write();
    fPointsHigh->SetName("fPointsLow");
    out->cd();
    fPointsLow->Write();
    cout << "Points are saved\n";
    
    out->cd();
    fFitFunction->Write();
    cout << "Fit function is saved\n";
    
    out->Close();
    delete out;
    
    fFileOut->cd();
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::WriteParameters()
{
    Float_t tdc_0=1.;
    Float_t tdc_1=0.;
    Float_t tdc_2=0.;
    Float_t adc_0 = fFitFunction->GetParameter(0);
    Float_t adc_1 = fFitFunction->GetParameter(1);
    Float_t adc_2 = fFitFunction->GetParameter(2);
    Float_t adc_3 = 0;//fFitFunction->GetParameter(3);
    Float_t twc_0=0.;
    Float_t twc_1=0.;
    Float_t twc_2=0.;
    
    fprintf(fFileCalibParams,
            "%i\t%i\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
            fSector,
            fCell,
            tdc_0,
            tdc_1,
            tdc_2,
            adc_0,
            adc_1,
            adc_2,
            adc_3,
            twc_0,
            twc_1,
            twc_2
    );
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::WriteBadQuality()
{
    fprintf(fFileCalibQuality, 
            "%i\t%i\t%s\n",
            fSector,
            fCell,
            "-1"
           );
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::WriteUncalibratedQuality()
{
    fprintf(fFileCalibQuality, 
            "%i\t%i\t%s\n",
            fSector,
            fCell,
            "0"
    );
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::WriteGoodQuality()
{
    fprintf(fFileCalibQuality, 
            "%i\t%i\t%s\n",
            fSector,
            fCell,
            "1"
    );
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::Save()
{
    WriteGoodQuality();
    WriteParameters();
    WriteGraphs();
    SaveCanvas();
    Next();
}
//------------------------------------------------------------------------------------------------------------------

//##################################################################################################################


//==================================================================================================================
GuiCalibratorEcal::~GuiCalibratorEcal()
{
    ;
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void GuiCalibratorEcal::DoExit()
{
    gApplication->Terminate();
}
//------------------------------------------------------------------------------------------------------------------
