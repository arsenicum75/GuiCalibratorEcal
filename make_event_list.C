

void make_event_list(const char* cut="")
{
    TFile* file = new TFile("/media/Software/HADES/data/ag123ag_leptonttree_gen3_ver2.root","READ");
    TTree* nt = (TTree*)file->Get("nt");
    
    Float_t sector, cell;
//     Float_t TOT     ;
//     Float_t mom     ;
//     Float_t energy  ;
//     Float_t beta    ;
//     Float_t richQa  ;
//     Float_t emc_mq  ;
//     Float_t emc_path;
//     Float_t mm_qa   ;
//     Float_t chi2    ;
    nt->GetBranch("pid")->GetLeaf("sector"  )->SetAddress(&sector);
    nt->GetBranch("pid")->GetLeaf("cell"    )->SetAddress(&cell);
//     nt->GetBranch("pid")->GetLeaf("TOT"     )->SetAddress(&TOT);
//     nt->GetBranch("pid")->GetLeaf("mom"     )->SetAddress(&mom);
//     nt->GetBranch("pid")->GetLeaf("energy"  )->SetAddress(&energy);
//     nt->GetBranch("pid")->GetLeaf("beta"    )->SetAddress(&beta);
//     nt->GetBranch("pid")->GetLeaf("richQa"  )->SetAddress(&richQa);
//     nt->GetBranch("pid")->GetLeaf("emc_mq"  )->SetAddress(&emc_mq);
//     nt->GetBranch("pid")->GetLeaf("emc_path")->SetAddress(&emc_path);
//     nt->GetBranch("pid")->GetLeaf("mm_qa"   )->SetAddress(&mm_qa);
//     nt->GetBranch("pid")->GetLeaf("chi2"    )->SetAddress(&chi2);
    
    TEventList* list0 = new TEventList("list0");
    nt->Draw(">>list0",cut);
    
    TEventList* list[6][163];
    
    for(Int_t iSector=0; iSector<6;++iSector)
    {
        
        for(Int_t iCell=0;iCell<163;++iCell)
        {
            
            list[iSector][iCell] = new TEventList(Form("list_%i_%i",iSector,iCell));
        }
    }
    
    Int_t iEntry=0;
    for(Int_t iList=0; iList<list0->GetN();++iList)
    {
        iEntry=list0->GetEntry(iList);
        nt->GetEntry(iEntry);
        if(iList%1000==0)cout << iList << "\t" << iEntry << endl;
        if(sector>6 || sector<0 || cell>162 || cell<0){cout << "ERROR"; continue;}
        list[(Int_t)sector][(Int_t)cell]->Enter(iEntry);
    }
    
    TFile* outfile = new TFile("EventLists.root","RECREATE");
    
    for(Int_t iSector=0; iSector<6;++iSector)
    {
        
        for(Int_t iCell=0;iCell<163;++iCell)
        {
            
            outfile->cd();
            list[iSector][iCell]->Write();
            delete list[iSector][iCell];
        }
        
    }
    outfile->Close();
    delete list0;
    delete nt;
    
    file->Close();
    delete file;
    return;
}
