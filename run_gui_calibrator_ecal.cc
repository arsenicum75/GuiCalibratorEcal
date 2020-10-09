#include "gui_calibrator_ecal.hh"

//========================================================
//-----------   M A I N   F U N C T I O N   ---------------||
//*********************************************************||
void run_gui_calibrator_ecal(Int_t sector, Int_t cell)  //*||
{                                                       //*||
    TStyle* style = new TStyle("style","style");        //*||
    style->Reset("Bold");                               //*||
    style->SetOptFit(111);                              //*||
    style->SetPalette(1);                               //*||
    style->cd();                                        //*||
    cout << endl;                                       //*||
    new GuiCalibratorEcal(gClient->GetRoot(),1000,900,sector,cell);//*||
}                                                       //*||
//*********************************************************||
//========================================================//

