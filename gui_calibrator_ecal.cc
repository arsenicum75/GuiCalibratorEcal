#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "TFile.h"
#include "TList.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TChain.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TEventList.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TCutG.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TCut.h"
#include "TMath.h"
#include "TStyle.h"

using namespace std;
static const char* tree_high = "/home/arseniy/Documents/HADES/Analysis/gui_calibrator/raw_tree/Ag123Ag_High_gen3_v2.root";
static const char* tree_low  = "/home/arseniy/Documents/HADES/Analysis/gui_calibrator/raw_tree/Ag123Ag_Low_gen3_v2.root";
static const char* list_high = "/home/arseniy/Documents/HADES/Analysis/gui_calibrator/raw_tree/EventListsHigh.root";
static const char* list_low  = "/home/arseniy/Documents/HADES/Analysis/gui_calibrator/raw_tree/EventListsLow.root";
//==================================================================================================================
TNtuple* treeToNt(TFile* fileOut,
                  Int_t sector=0, 
                  Int_t cell=0, 
                  const char* cut="size==1 && emc_path>0 && chi2<250 && emc_mq<2.5"
                 )
{
    fileOut->cd();
    TNtuple* nt_out = new TNtuple("nt","nt","sector:cell:TOT:mom:E_calc:energy:beta:richQa:emc_mq:emc_path:mm_qa:chi2:size:low");
    
    TFile* file = new TFile(tree_high,"READ");
    
    TTree* nt_in = (TTree*)file->Get("nt");
    
    Float_t TOT     ;       nt_in->GetBranch("pid")->GetLeaf("TOT"     )->SetAddress(&TOT);
    Float_t mom     ;       nt_in->GetBranch("pid")->GetLeaf("mom"     )->SetAddress(&mom);
    Float_t energy  ;       nt_in->GetBranch("pid")->GetLeaf("energy"  )->SetAddress(&energy);
    Float_t beta    ;       nt_in->GetBranch("pid")->GetLeaf("beta"    )->SetAddress(&beta);
    Float_t richQa  ;       nt_in->GetBranch("pid")->GetLeaf("richQa"  )->SetAddress(&richQa);
    Float_t emc_mq  ;       nt_in->GetBranch("pid")->GetLeaf("emc_mq"  )->SetAddress(&emc_mq);
    Float_t emc_path;       nt_in->GetBranch("pid")->GetLeaf("emc_path")->SetAddress(&emc_path);
    Float_t mm_qa   ;       nt_in->GetBranch("pid")->GetLeaf("mm_qa"   )->SetAddress(&mm_qa);
    Float_t chi2    ;       nt_in->GetBranch("pid")->GetLeaf("chi2"    )->SetAddress(&chi2);
    Float_t size    ;       nt_in->GetBranch("pid")->GetLeaf("size"    )->SetAddress(&size);
    
    TFile* file_eventlist = new TFile(list_high,"READ");
    TEventList* list0 = (TEventList*)file_eventlist->Get(Form("list_%i_%i",sector,cell));
    nt_in->SetEventList(list0);
    
    TEventList* list = new TEventList("list");
    nt_in->Draw(">>list",cut);
    nt_in->SetEventList(list);
    
    cout << "====================\n";
    cout << " Sector\t" << sector << "\n";
    cout << " Module\t" << cell << "\n";
    Int_t nList = list->GetN();
    Float_t counter=0;
    for(Int_t iList=0; iList<nList; ++iList)
    {
        ++counter;
        if(counter/(Float_t)nList>0.05){cout << "-";counter=0;}
        nt_in->GetEntry(list->GetEntry(iList));
        //WARNING correct on energy loss in RPC
        mom=mom-3.47-0.104855*mom*(4./9.5);
        nt_out->Fill(sector,cell,TOT,mom,mom,energy,beta,richQa,emc_mq,emc_path,mm_qa,chi2,size,0);
        
    }
    cout << "-\n====================\n";
    
    
    delete list;
    delete list0;
    file_eventlist->Close();
    delete file_eventlist;
    delete nt_in;
    file->Close();
    delete file;
    
    return nt_out;
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
TNtuple* ntToNt(TNtuple* nt_in, TF1* func)
{
    TNtuple* nt_out = new TNtuple("nt","nt","sector:cell:TOT:mom:E_calc:energy:beta:richQa:emc_mq:emc_path:mm_qa:chi2:size:low");
    
    Float_t E_calc  ;
    Float_t sector  ;       nt_in->SetBranchAddress("sector"  ,&sector);
    Float_t cell    ;       nt_in->SetBranchAddress("cell"    ,&cell);
    Float_t TOT     ;       nt_in->SetBranchAddress("TOT"     ,&TOT);
    Float_t mom     ;       nt_in->SetBranchAddress("mom"     ,&mom);
    Float_t energy  ;       nt_in->SetBranchAddress("energy"  ,&energy);
    Float_t beta    ;       nt_in->SetBranchAddress("beta"    ,&beta);
    Float_t richQa  ;       nt_in->SetBranchAddress("richQa"  ,&richQa);
    Float_t emc_mq  ;       nt_in->SetBranchAddress("emc_mq"  ,&emc_mq);
    Float_t emc_path;       nt_in->SetBranchAddress("emc_path",&emc_path);
    Float_t mm_qa   ;       nt_in->SetBranchAddress("mm_qa"   ,&mm_qa);
    Float_t chi2    ;       nt_in->SetBranchAddress("chi2"    ,&chi2);
    Float_t size    ;       nt_in->SetBranchAddress("size"    ,&size);
    Float_t low     ;       nt_in->SetBranchAddress("low"     ,&low);
    
    
    for(Int_t iEntry=0; iEntry<nt_in->GetEntries();iEntry++)
    {
        nt_in->GetEntry(iEntry);
        E_calc = func->Eval(TOT);
        nt_out->Fill(sector,cell,TOT,mom,E_calc,energy,beta,richQa,emc_mq,emc_path,mm_qa,chi2,size,low);
        
    }
    
    
    delete nt_in;
    return nt_out;
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
TNtuple* treeToNtLow(TFile* fileOut,
                     Int_t sector=0, 
                     Int_t cell=0, 
                     const char* cut="size==1 && emc_path>0 && chi2<250 && emc_mq<2.5"
                    )
{
    fileOut->cd();
    TNtuple* nt_out = new TNtuple("nt","nt","sector:cell:TOT:mom:E_calc:energy:beta:richQa:emc_mq:emc_path:mm_qa:chi2:size:low");
    
    TFile* file = new TFile(tree_low,"READ");
    
    TTree* nt_in = (TTree*)file->Get("nt");
    
    Float_t TOT     ;       nt_in->GetBranch("pid")->GetLeaf("TOT"     )->SetAddress(&TOT);
    Float_t mom     ;       nt_in->GetBranch("pid")->GetLeaf("mom"     )->SetAddress(&mom);
    Float_t energy  ;       nt_in->GetBranch("pid")->GetLeaf("energy"  )->SetAddress(&energy);
    Float_t beta    ;       nt_in->GetBranch("pid")->GetLeaf("beta"    )->SetAddress(&beta);
    Float_t richQa  ;       nt_in->GetBranch("pid")->GetLeaf("richQa"  )->SetAddress(&richQa);
    Float_t emc_mq  ;       nt_in->GetBranch("pid")->GetLeaf("emc_mq"  )->SetAddress(&emc_mq);
    Float_t emc_path;       nt_in->GetBranch("pid")->GetLeaf("emc_path")->SetAddress(&emc_path);
    Float_t mm_qa   ;       nt_in->GetBranch("pid")->GetLeaf("mm_qa"   )->SetAddress(&mm_qa);
    Float_t chi2    ;       nt_in->GetBranch("pid")->GetLeaf("chi2"    )->SetAddress(&chi2);
    Float_t size    ;       nt_in->GetBranch("pid")->GetLeaf("size"    )->SetAddress(&size);
    
    TFile* file_eventlist = new TFile(list_low,"READ");
    TEventList* list0 = (TEventList*)file_eventlist->Get(Form("list_%i_%i",sector,cell));
    nt_in->SetEventList(list0);
    
    TEventList* list = new TEventList("list");
    nt_in->Draw(">>list",cut);
    nt_in->SetEventList(list);
    
    cout << "====================\n";
    cout << " Sector\t" << sector << "\n";
    cout << " Module\t" << cell << "\n";
    Int_t nList = list->GetN();
    Float_t counter=0;
    for(Int_t iList=0; iList<nList; ++iList)
    {
        ++counter;
        if(counter/(Float_t)nList>0.05){cout << "-";counter=0;}
        nt_in->GetEntry(list->GetEntry(iList));
        //WARNING correct on energy loss in RPC
        mom=mom-3.47-0.104855*mom*(4./9.5);
        nt_out->Fill(sector,cell,TOT,mom,mom,energy,beta,richQa,emc_mq,emc_path,mm_qa,chi2,size,1);
        
    }
    cout << "-\n====================\n";
    
    
    delete list;
    delete list0;
    file_eventlist->Close();
    delete file_eventlist;
    delete nt_in;
    file->Close();
    delete file;
    
    return nt_out;
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
TH2F* ntToHist(TNtuple* nt, 
               //TF1* f,
               //Double_t deviation,
               TCut cut="",
               const Int_t nX=60,
               const Int_t nY=50,
               Float_t minX=0, 
               Float_t maxX=1600, 
               Float_t minY=0, 
               Float_t maxY=1200)
{
    TEventList* list = new TEventList("list");
    nt->Draw(">>list",cut);
    nt->SetEventList(list);
    
    Float_t TOT, mom;
    nt->SetBranchAddress("TOT",&TOT);
    nt->SetBranchAddress("mom",&mom);
    
    TH2F* hist = new TH2F("hist","Raw data;TOT;mom",nX,minX,maxX,nY,minY,maxY);
    //hist->SetDirectory(0);
    Int_t nList = list->GetN();
    //Float_t counter=0;
    for(Int_t iList=0; iList<nList; ++iList)
    {
        nt->GetEntry(list->GetEntry(iList));
        //if(TMath::Abs(f->Eval(TOT)-mom)<deviation)
            hist->Fill(TOT,mom);
    }
    nt->SetEventList(0);
    delete list;
    return hist;
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
TGraphErrors* histToGraph(TH2F* hist, TCanvas* c, Int_t n=150, Bool_t isCheckEachSlice=kFALSE)
{
    c->Clear();
    c->Divide(2,1);
    c->cd(2)->Divide(1,2);
    c->cd(1);
    hist->Draw("colz");
    c->Update();
    
    TF1* f_gaus = new TF1("f_gaus","[0]*TMath::Gaus(x,[1],[2])+[3]*TMath::Landau(x,[4],[5],0)",0,1500);
    f_gaus->SetParLimits(0,0,1000);
    f_gaus->SetParLimits(1,0,1600);
    f_gaus->SetParLimits(2,0,700);
    f_gaus->SetParLimits(3,0,1000);
    f_gaus->SetParLimits(4,0,1500);
    f_gaus->SetParLimits(5,0,1500);
    f_gaus->SetLineColor(kRed);
    //TGraph* grOut = new TGraph();
    //grOut->SetMarkerStyle(20);
    TGraphErrors* gr = new TGraphErrors();
    gr->SetTitle("grerr");
    gr->SetMarkerColor(kBlack);
    gr->SetLineWidth(2);
    Float_t x,y,xerr,yerr,height,mean,dev;
    Int_t nX=hist->GetXaxis()->GetNbins();
    TH1D* projectY;
    //projectY->SetDirectory(0);
    Int_t iGr=0;
    Int_t nProjections = hist->GetNbinsX();
    for(Int_t iProjection=0; iProjection<nProjections;++iProjection)
    {
        Int_t iProjection1=iProjection;
        Int_t iProjection2=iProjection;
        projectY=hist->ProjectionY(Form("hist_%i",iProjection),iProjection,iProjection);
        while(projectY->Integral()<1 && iProjection2<nProjections)
        {
            ++iProjection;
            delete projectY;
            projectY=hist->ProjectionY(Form("hist_%i",iProjection),iProjection,iProjection);
            iProjection1=iProjection;
            iProjection2=iProjection;
        }
        while(projectY->Integral()<n && iProjection2<nProjections)
        {
            ++iProjection2;
            delete projectY;
            projectY=hist->ProjectionY(Form("hist_%i",iProjection),iProjection1,iProjection2);
        }
        iProjection=iProjection2;
        projectY->SetLineColor(kBlue);
        if(projectY->Integral()>=n)
        {
            // Rebinning
            while((projectY->GetNbinsX()<1+TMath::Log2(projectY->Integral()) && projectY->GetNbinsX()>4))
            {projectY->Rebin(2);cout << projectY->GetNbinsX() << " bins in Y projection\n";}
            
            c->cd(2)->cd(1);
            projectY->Draw();
            
            // Fit Y
            height = projectY->GetMaximum();
            mean   = projectY->GetBinCenter(projectY->GetMaximumBin() );//Mean();
            dev    = (projectY->GetStdDev()>0?projectY->GetStdDev():0.5*projectY->GetBinWidth(1));
            
            f_gaus->SetParameter(0,height); f_gaus->SetParLimits(0,height*(1.-1./TMath::Sqrt(height)),height*(1.+1./TMath::Sqrt(height))); 
            f_gaus->SetParameter(1,mean);   f_gaus->SetParLimits(1,(mean-dev>0?mean-dev:0),mean+dev);     
            f_gaus->SetParameter(2,dev);    f_gaus->SetParLimits(2,dev*0.1,2.*dev);           
            
            f_gaus->SetParameter(4,200);//mean-dev>0?mean-dev:0);
            f_gaus->SetParameter(3,1);      f_gaus->SetParLimits(3,0,height*(1.-1./TMath::Sqrt(height))/0.180656);
            f_gaus->SetParameter(5,3.*dev);
            
            
            projectY->Fit(f_gaus,"LQM","",mean-2*dev>0?mean-2*dev:0,mean+5*dev>1200?1200:mean+5*dev);//mean+dev);
            //projectY->Fit(f_gaus,"LQM","",0,1200);
            //c->Update();
            //if(isCheckEachSlice)c->WaitPrimitive();
            
            y = f_gaus->GetParameter(1);
            yerr = f_gaus->GetParameter(2);
            
            x = (hist->GetXaxis()->GetBinUpEdge(iProjection2)+hist->GetXaxis()->GetBinLowEdge(iProjection1))*0.5;
            xerr = (hist->GetXaxis()->GetBinUpEdge(iProjection2)-hist->GetXaxis()->GetBinLowEdge(iProjection1))*0.5;
            
            // *******************************************************
            
            TCutG* cutg = new TCutG(Form("cut_%i",iProjection),4);
            cutg->SetPoint(0, x-xerr,y-yerr);
            cutg->SetPoint(1, x-xerr,y+yerr);
            cutg->SetPoint(2, x+xerr,y+yerr);
            cutg->SetPoint(3, x+xerr,y-yerr);
            cutg->SetLineColor(kRed);
            c->cd(1);
            cutg->Draw("same");
            
            TH1D* projectX = hist->ProjectionX(Form("histX_%i",iProjection),hist->GetYaxis()->FindBin(y-yerr),hist->GetYaxis()->FindBin(y+yerr),Form("[cut_%i]",iProjection));
            c->cd(2)->cd(2);
            projectX->Draw();
            
            height = projectX->GetMaximum();
            mean   = projectX->GetBinCenter(projectX->GetMaximumBin() );//Mean();projectX->GetMean();
            dev    = xerr;
            //(projectX->GetStdDev()!=0?projectX->GetStdDev():0.5*projectX->GetBinWidth(0));
            /*
            f_gaus->SetParameter(0,height); f_gaus->SetParLimits(0,height*(1.-1./TMath::Sqrt(height)),height*(1.+1./TMath::Sqrt(height)));
            f_gaus->SetParameter(1,mean);   f_gaus->SetParLimits(1,mean-2*xerr,mean+2*xerr);     
            f_gaus->SetParameter(2,dev);    f_gaus->SetParLimits(2,0.5*xerr,2.*xerr);           
            
            projectX->Fit(f_gaus,"LQM","",mean-dev,mean+dev);
            c->Update();
            */
            if(height>0)x = mean;//f_gaus->GetParameter(1);
            xerr = dev;//f_gaus->GetParameter(2);
            //Double_t max = projectY->GetBinCenter(projectY->GetMaximumBin());
            //if(TMath::Abs(max-y)<2*yerr && yerr<cut)
            //if(projectY->Integral()>=n)
            //if(x>=hist->GetXaxis()->GetBinLowEdge(iProjection1) && x <=hist->GetXaxis()->GetBinUpEdge(iProjection2))
            yerr/=TMath::Sqrt(y*0.001);
            {
                gr->SetPoint(iGr,x,y);
                gr->SetPointError(iGr,xerr,yerr);
                ++iGr;
            }
            c->cd(1);
            gr->Draw();
            c->Update();
            if(isCheckEachSlice)c->WaitPrimitive();
            delete cutg;
            delete projectX;
        }
        delete projectY;
        
    }
    if(isCheckEachSlice)
    {
        cout<< "===================================\n";
        cout<< "=========   CLOSE CANVAS ? ========\n";
        cout<< "===================================\n";
        //cin.get();
        
        //c->WaitPrimitive();
    }
    delete f_gaus;
    //delete projectY;
    //c->Close();
    //delete c;
    return gr;
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
TGraphErrors* joinGraphs(TGraphErrors* gr1, TGraphErrors* gr2, Float_t grLowBorder=400, Float_t grHighBorder=700)
{
    TGraphErrors* gr = new TGraphErrors();
    Int_t n1 = gr1->GetN();
    Int_t n2 = gr2->GetN();
    Int_t i2=0;
    Int_t i1=0;
    Int_t i=0;
    Double_t x=0;
    Double_t y=0;
    Double_t xerr=0;
    Double_t yerr=0;
    
    if(grLowBorder>grHighBorder)
    {
        cout << "WARNING: interchange borders!\n";
        Float_t value = grLowBorder;
        grLowBorder=grHighBorder;
        grHighBorder=value;
    }
    i=0;
    i1=0;
    while(i1 < n1)
    {
        x = gr1->GetX()[i1];
        if(x>grLowBorder)break;
        y = gr1->GetY()[i1];
        gr->SetPoint(i,x,y);
        xerr = gr1->GetErrorX(i1);
        yerr = gr1->GetErrorY(i1);
        gr->SetPointError(i,xerr,yerr);
        ++i;
        ++i1;
    }
    
    i2=0;
    while(i2<n2)
    {
        x = gr2->GetX()[i2];
        if(x>=grHighBorder)break;
        ++i2;
    }
    
    while(i2<n2)
    {
        x = gr2->GetX()[i2];
        y = gr2->GetY()[i2];
        gr->SetPoint(i,x,y);
        xerr = gr2->GetErrorX(i2);
        yerr = gr2->GetErrorY(i2);
        gr->SetPointError(i,xerr,yerr);
        ++i;
        ++i2;
    }
    
    return gr;
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
TF1* graphToFunc(TGraphErrors* gr)
{
    TF1* f0 = new TF1("f0","TMath::Exp([0]*x+[1])+[2]",0,1600);
    f0->SetParameter(0,1E-4);
    f0->FixParameter(2,0.);
    f0->SetParameter(3,50.);
    gr->Fit("f0","NRB","");
    f0->SetLineColor(kRed);
    f0->SetLineWidth(2);
    return f0;
}
//------------------------------------------------------------------------------------------------------------------

//==================================================================================================================
void printHeader(FILE* parameters)
{
    fprintf(parameters,"\n");
//     fprintf(parameters,"################################################################################################\n");
//     fprintf(parameters,"################################################################################################\n");
//     fprintf(parameters,"################################################################################################\n");
//     fprintf(parameters,"# Calibration parameters for the Emc\n"                                                            );
//     fprintf(parameters,"# Format: energy = TMath::Exp([Adc_0]+HEmcCal::width*[Adc_1])+[Adc_2]+[Adc_3]*HEmcCal::width\n"    );
//     fprintf(parameters,"# sector(0...5)   position(0...162)  Tdc_0  Tdc_1 Tdc_2 Adc_0  Adc_1 Adc_2  Twc_0  Twc_1  Twc_2\n" );
//     fprintf(parameters,"################################################################################################\n");
//     fprintf(parameters,"[EmcCalPar]\n"                                                                                     );
//     fprintf(parameters,"// Parameter Context: EmcCalParProduction\n"                                                       );
    fprintf(parameters,"//----------------------------------------------------------------------------------------------\n");
    
    return;
}
//------------------------------------------------------------------------------------------------------------------
