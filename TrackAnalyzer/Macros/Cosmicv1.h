//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Aug 20 15:01:06 2015 by ROOT version 5.34/18
// from TTree cosmics/
// found on file: Outfile.root
//////////////////////////////////////////////////////////

#ifndef Cosmicv1_h
#define Cosmicv1_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <vector>
#include <vector>

// Fixed size dimensions of array or collections stored in the TTree if any.

class Cosmicv1 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   vector<double>  *pt;
   vector<double>  *charge;
   vector<double>  *chi2;
   vector<double>  *chi2_ndof;
   vector<double>  *eta;
   vector<double>  *theta;
   vector<double>  *phi;
   vector<double>  *p;
   vector<double>  *d0;
   vector<double>  *dz;
   vector<double>  *nvh;
   vector<double>  *run_time;
   vector<unsigned int> *runnum;
   vector<int>     *number_of_events;
   vector<int>     *number_of_tracks;
   vector<int>     *number_of_tracks_PIX;
   vector<int>     *number_of_tracks_FPIX;
   vector<int>     *number_of_tracks_BPIX;
   vector<int>     *number_of_tracks_TID;
   vector<int>     *number_of_tracks_TIDM;
   vector<int>     *number_of_tracks_TIDP;
   vector<int>     *number_of_tracks_TIB;
   vector<int>     *number_of_tracks_TEC;
   vector<int>     *number_of_tracks_TECP;
   vector<int>     *number_of_tracks_TECM;
   vector<int>     *number_of_tracks_TOB;

   // List of branches
   TBranch        *b_pt;   //!
   TBranch        *b_charge;   //!
   TBranch        *b_chi2;   //!
   TBranch        *b_chi2_ndof;   //!
   TBranch        *b_eta;   //!
   TBranch        *b_theta;   //!
   TBranch        *b_phi;   //!
   TBranch        *b_p;   //!
   TBranch        *b_d0;   //!
   TBranch        *b_dz;   //!
   TBranch        *b_nvh;   //!
   TBranch        *b_run_time;   //!
   TBranch        *b_runnum;   //!
   TBranch        *b_number_of_events;   //!
   TBranch        *b_number_of_tracks;   //!
   TBranch        *b_number_of_tracks_PIX;   //!
   TBranch        *b_number_of_tracks_FPIX;   //!
   TBranch        *b_number_of_tracks_BPIX;   //!
   TBranch        *b_number_of_tracks_TID;   //!
   TBranch        *b_number_of_tracks_TIDM;   //!
   TBranch        *b_number_of_tracks_TIDP;   //!
   TBranch        *b_number_of_tracks_TIB;   //!
   TBranch        *b_number_of_tracks_TEC;   //!
   TBranch        *b_number_of_tracks_TECP;   //!
   TBranch        *b_number_of_tracks_TECM;   //!
   TBranch        *b_number_of_tracks_TOB;   //!

   Cosmicv1(TTree *tree=0);
   virtual ~Cosmicv1();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Cosmicv1_cxx
Cosmicv1::Cosmicv1(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Outfile.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("Outfile.root");
      }
      f->GetObject("cosmics",tree);

   }
   Init(tree);
}

Cosmicv1::~Cosmicv1()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Cosmicv1::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Cosmicv1::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Cosmicv1::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   pt = 0;
   charge = 0;
   chi2 = 0;
   chi2_ndof = 0;
   eta = 0;
   theta = 0;
   phi = 0;
   p = 0;
   d0 = 0;
   dz = 0;
   nvh = 0;
   run_time = 0;
   runnum = 0;
   number_of_events = 0;
   number_of_tracks = 0;
   number_of_tracks_PIX = 0;
   number_of_tracks_FPIX = 0;
   number_of_tracks_BPIX = 0;
   number_of_tracks_TID = 0;
   number_of_tracks_TIDM = 0;
   number_of_tracks_TIDP = 0;
   number_of_tracks_TIB = 0;
   number_of_tracks_TEC = 0;
   number_of_tracks_TECP = 0;
   number_of_tracks_TECM = 0;
   number_of_tracks_TOB = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("pt", &pt, &b_pt);
   fChain->SetBranchAddress("charge", &charge, &b_charge);
   fChain->SetBranchAddress("chi2", &chi2, &b_chi2);
   fChain->SetBranchAddress("chi2_ndof", &chi2_ndof, &b_chi2_ndof);
   fChain->SetBranchAddress("eta", &eta, &b_eta);
   fChain->SetBranchAddress("theta", &theta, &b_theta);
   fChain->SetBranchAddress("phi", &phi, &b_phi);
   fChain->SetBranchAddress("p", &p, &b_p);
   fChain->SetBranchAddress("d0", &d0, &b_d0);
   fChain->SetBranchAddress("dz", &dz, &b_dz);
   fChain->SetBranchAddress("nvh", &nvh, &b_nvh);
   fChain->SetBranchAddress("run_time", &run_time, &b_run_time);
   fChain->SetBranchAddress("runnum", &runnum, &b_runnum);
   fChain->SetBranchAddress("number_of_events", &number_of_events, &b_number_of_events);
   fChain->SetBranchAddress("number_of_tracks", &number_of_tracks, &b_number_of_tracks);
   fChain->SetBranchAddress("number_of_tracks_PIX", &number_of_tracks_PIX, &b_number_of_tracks_PIX);
   fChain->SetBranchAddress("number_of_tracks_FPIX", &number_of_tracks_FPIX, &b_number_of_tracks_FPIX);
   fChain->SetBranchAddress("number_of_tracks_BPIX", &number_of_tracks_BPIX, &b_number_of_tracks_BPIX);
   fChain->SetBranchAddress("number_of_tracks_TID", &number_of_tracks_TID, &b_number_of_tracks_TID);
   fChain->SetBranchAddress("number_of_tracks_TIDM", &number_of_tracks_TIDM, &b_number_of_tracks_TIDM);
   fChain->SetBranchAddress("number_of_tracks_TIDP", &number_of_tracks_TIDP, &b_number_of_tracks_TIDP);
   fChain->SetBranchAddress("number_of_tracks_TIB", &number_of_tracks_TIB, &b_number_of_tracks_TIB);
   fChain->SetBranchAddress("number_of_tracks_TEC", &number_of_tracks_TEC, &b_number_of_tracks_TEC);
   fChain->SetBranchAddress("number_of_tracks_TECP", &number_of_tracks_TECP, &b_number_of_tracks_TECP);
   fChain->SetBranchAddress("number_of_tracks_TECM", &number_of_tracks_TECM, &b_number_of_tracks_TECM);
   fChain->SetBranchAddress("number_of_tracks_TOB", &number_of_tracks_TOB, &b_number_of_tracks_TOB);
   Notify();
}

Bool_t Cosmicv1::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Cosmicv1::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Cosmicv1::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Cosmicv1_cxx
