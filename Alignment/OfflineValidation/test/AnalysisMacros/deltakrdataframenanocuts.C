#include <fstream>             // For file input/output operations
#include <string>              // For std::string operations
#include <vector>              // For std::vector container
#include <cmath>               // For mathematical functions like fabs()
#include <iostream>            // For standard input/output streams
#include <ROOT/RDataFrame.hxx> // For ROOT DataFrame operations
#include <ROOT/RVec.hxx>       // For ROOT::VecOps::RVec
#include <TH1D.h>              // For 1D histogram
#include <TH2D.h>              // For 2D histogram
#include <TH3D.h>              // For 3D histogram
#include <TF1.h>               // For TF1 function fitting
#include <TFile.h>             // For file I/O with ROOT files
#include <TCanvas.h>           // For canvas to draw plots
#include <TStyle.h>            // For style settings

bool exclude = false;
int pt = 20;

double fitmodel(double *x, double *par)
{
    if ((exclude) && (x[0] > -1.*pt) && (x[0] < (double)pt)) {
      TF1::RejectPoint();
      return 0;
   }
   return par[0] + par[1]*x[0] - par[2]/fabs(x[0]);
}

ROOT::VecOps::RVec<float> makegencharge(ROOT::VecOps::RVec<int> &GenPart_pdgId) {
  ROOT::VecOps::RVec<float> v;
  for (auto i=0U; i!=GenPart_pdgId.size(); i++) {
    v.push_back(-GenPart_pdgId[i]/abs(GenPart_pdgId[i]));
  }
  return v;
}

ROOT::VecOps::RVec<float> getgenvariable(ROOT::VecOps::RVec<int> &Muon_genPartIdx, ROOT::VecOps::RVec<float> &genvar) {
  ROOT::VecOps::RVec<float> v;
  for (auto i=0U; i!=Muon_genPartIdx.size(); i++) {
    if (Muon_genPartIdx[i]>-1) v.push_back(genvar[Muon_genPartIdx[i]]);
  }
  return v;
}

ROOT::VecOps::RVec<float> getratio(ROOT::VecOps::RVec<int> &Muon_genPartIdx, ROOT::VecOps::RVec<float> &Muon_pt, ROOT::VecOps::RVec<int> &Muon_charge, ROOT::VecOps::RVec<float> &GenPart_pt, ROOT::VecOps::RVec<float> &GenPart_charge) {
  ROOT::VecOps::RVec<float> v;
  for (auto i=0U; i!=Muon_genPartIdx.size(); i++) {
    int idx=Muon_genPartIdx[i];
    if (idx>-1) v.push_back((Muon_charge[i]/Muon_pt[i]-GenPart_charge[idx]/GenPart_pt[idx])/(GenPart_charge[idx]/GenPart_pt[idx]));
  }
  return v;
}

ROOT::VecOps::RVec<float> getproduct(ROOT::VecOps::RVec<float> &charge, ROOT::VecOps::RVec<float> &pt) {
  ROOT::VecOps::RVec<float> v;
  for (auto i=0U; i!=charge.size(); i++) {
    v.push_back(charge[i]*pt[i]);
  }
  return v;
}

void deltakrdataframenanocuts() {
  gStyle->SetOptStat(0);
  ROOT::EnableImplicitMT();
  std::ifstream infile;
  infile.open("filelist.txt");
  std::vector<std::string> filenames;
  std::string line, root(".root");
  while (std::getline(infile,line)) {
    if (line.find(root)==std::string::npos) continue;
    std::cout<<line<<"\n";
    /*TFile file(line.c_str());
      if (!file.IsOpen()) continue;
      if (file.IsZombie()) continue;
      if (file.TestBit(TFile::kRecovered)) continue;
      file.Close();*/
    filenames.push_back(line);
  }
  /*std::string troot("_0.root");
    for (unsigned int i=1; i!=26; i++) {
    std::string plus("outputplus_idx");
    std::string minus("outputminus_idx");
    plus+=std::to_string(i)+troot;
    minus+=std::to_string(i)+troot;
    filenames.push_back(plus);
    filenames.push_back(minus);
    }*/

  
  ROOT::RDataFrame d("ZtoMMNtuple/tree", "ZmmNtuple_MC_GEN-SIM_ideal.root");
 
  // Define vectors for gen and track quantities
  auto d1 = d.Define("genCharge", [](float posPt, float negPt){ return std::vector<float>{posPt, -negPt}; }, {"genPosMuonPt", "genNegMuonPt"})
    .Define("genPt", [](float posPt, float negPt){ return std::vector<float>{posPt, negPt}; }, {"genPosMuonPt", "genNegMuonPt"})
    .Define("genEta", [](float posEta, float negEta){ return std::vector<float>{posEta, negEta}; }, {"genPosMuonEta", "genNegMuonEta"})
    .Define("trackPt", [](float posPt, float negPt){ return std::vector<float>{posPt, negPt}; }, {"posTrackPt", "negTrackPt"})
    .Define("trackCharge", [](float posCharge, float negCharge){ return std::vector<float>{posCharge, negCharge}; }, {"posTrackD0", "negTrackD0"})
    .Define("ratio", [](const std::vector<float>& trackPt, const std::vector<float>& genPt){
		       std::vector<float> result;
		       for (size_t i = 0; i < trackPt.size(); ++i) {
			 result.push_back(trackPt[i] / genPt[i]);
		       }
		       return result;
		     }, {"trackPt", "genPt"})
    .Define("product", [](const std::vector<float>& genCharge, const std::vector<float>& genPt){
			 std::vector<float> result;
			 for (size_t i = 0; i < genCharge.size(); ++i) {
			   result.push_back(genCharge[i] * genPt[i]);
			 }
			 return result;
		       }, {"genCharge", "genPt"});
  
  // Histogramming
  auto qoverp = d1.Histo3D({"qoverp", "qoverp", 48, -2.4, 2.4, 300, -150., 150., 100, -1., 1.}, "genEta", "product", "ratio");
  
  // Cloning and further processing
  TH2D* QoverP = new TH2D("QoverP", "", 48, -2.4, 2.4, 300, -150., 150.);
  TH3D* qoverp3D = (TH3D*)qoverp->Clone();
  std::string outputname("output.root");
  outputname+=std::to_string(pt)+std::string(".root");
  TFile *output=new TFile(outputname.c_str(),"RECREATE");
  exclude = true;
  for (unsigned int i=0; i!=48; i++) {
    qoverp3D->GetXaxis()->SetRange(i+1,i+1);
    double min=-2.4+i*0.1, max=-2.4+(i+1)*0.1;
    std::string title;
    title=std::to_string(min)+std::string("<#eta<")+std::to_string(max);
    for (unsigned int j=0; j!=300; j++) {
      qoverp3D->GetYaxis()->SetRange(j+1,j+1);
      TH1D* meanvariance=(TH1D*)qoverp3D->Project3D("z");
      double minpt=-150.+j*1., maxpt=-150.+(j+1)*1.;
      std::string Title;
      Title=title+std::string(" ")+std::to_string(minpt)+std::string("<q*p_{T}<")+std::to_string(maxpt);
      std::string name("meanvariance_");
      name+=std::to_string(i)+std::string("_")+std::to_string(j);
      meanvariance->SetName(name.c_str());
      meanvariance->SetTitle(Title.c_str());
      std::string fitname("fit");
      fitname+=std::to_string(i)+std::string("_")+std::to_string(j);
      int binmax = meanvariance->GetMaximumBin(); double x = meanvariance->GetXaxis()->GetBinCenter(binmax);
      TF1 *fit=new TF1(fitname.c_str(),"[0]*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))+[3]",x-0.1,x+0.1);
      fit->SetParameter(0,meanvariance->GetBinContent(binmax));
      fit->SetParameter(1,x);
      fit->SetParameter(2,0.1);
      fit->SetParameter(3,0.);
      int Status1 = meanvariance->Fit(fit,"R","same",x-0.1,x+0.1);
      std::string fitname2;
      fitname2=fitname+std::string("_2");
      TF1 *fit2=new TF1(fitname2.c_str(),"[0]*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))+[3]",x-fit->GetParameter(2)*1.5,x+fit->GetParameter(2)*1.5);
      fit2->SetParameter(0,meanvariance->GetBinContent(binmax));
      fit2->SetParameter(1,x);
      fit2->SetParameter(2,fit->GetParameter(2));
      fit2->SetParameter(3,fit->GetParameter(3));
      int Status2 = meanvariance->Fit(fit2,"S","same",x-fit->GetParameter(2)*1.5,x+fit->GetParameter(2)*1.5);
      if ((Status2==0)&&(abs(fit2->GetParameter(2))<0.4)&&(fit2->GetParError(1)<1)&&(abs(fit2->GetParameter(1)-x)<0.1)) {QoverP->SetBinContent(QoverP->GetBin(i+1,j+1),fit2->GetParameter(1));
	QoverP->SetBinError(QoverP->GetBin(i+1,j+1),fit2->GetParError(1));}
      //else if ((Status1==0)&&(abs(fit->GetParameter(2))<0.4)&&(fit->GetParError(1)<1)&&(abs(fit->GetParameter(1)-x)<0.1)) {QoverP->SetBinContent(QoverP->GetBin(i+1,j+1),fit->GetParameter(1));
      //QoverP->SetBinError(QoverP->GetBin(i+1,j+1),fit->GetParError(1));}
      //else {QoverP->SetBinContent(QoverP->GetBin(i+1,j+1),meanvariance->GetMean(1));
      //QoverP->SetBinError(QoverP->GetBin(i+1,j+1),meanvariance->GetMeanError(1));}
      /*if (status2==0) {QoverP->SetBinContent(QoverP->GetBin(i+1,j+1),fit2->GetParameter(1));
	QoverP->SetBinError(QoverP->GetBin(i+1,j+1),fit2->GetParError(1));}
	else if (status1==0) {QoverP->SetBinContent(QoverP->GetBin(i+1,j+1),fit->GetParameter(1));
	QoverP->SetBinError(QoverP->GetBin(i+1,j+1),fit->GetParError(1));}
	else {QoverP->SetBinContent(QoverP->GetBin(i+1,j+1),meanvariance->GetMean(1));
	QoverP->SetBinError(QoverP->GetBin(i+1,j+1),meanvariance->GetMeanError(1));}*/
      meanvariance->Write();
    }
  }
  TH1D *histoA=new TH1D("histoA","A",48,-2.4,2.4);
  TH1D *histoM=new TH1D("histoM","M",48,-2.4,2.4);
  TH1D *histoepsilon=new TH1D("histoepsilon","#epsilon",48,-2.4,2.4);
  TH1D *status=new TH1D("status","status",48,-2.4,2.4);
  TH1D *nchi2=new TH1D("nchi2","#chi^{2}/nDOF",48,-2.4,2.4);
  for (unsigned int i=0; i!=48; i++) {
    std::string fitname("fit");
    fitname+=std::to_string(i);
    TF1 *fit=new TF1(fitname.c_str(),fitmodel,-150.,150.,3);
    fit->SetParameter(0,0.0);
    fit->SetParameter(1,0.0);
    fit->SetParameter(2,0.0);
    QoverP->GetXaxis()->SetRange(i+1,i+1);
    TH1D* histo=(TH1D*)QoverP->ProjectionY();
    int Status1 = histo->Fit(fitname.c_str(),"N");
    std::string fitname2;
    fitname2=fitname+std::string("_2");
    std::string fitname3;
    fitname3=fitname+std::string("_3");
    TF1 *fit2=new TF1(fitname2.c_str(),fitmodel,-150.,150.,3);
    fit2->SetParameter(0,fit->GetParameter(0));
    fit2->SetParameter(1,fit->GetParameter(1));
    fit2->SetParameter(2,fit->GetParameter(2));
    if (Status1!=0) {
      fit2->SetParameter(0,histoA->GetBinContent(i));
      fit2->SetParameter(1,histoM->GetBinContent(i));
      fit2->SetParameter(2,histoepsilon->GetBinContent(i));
    }
    int Status = histo->Fit(fitname2.c_str(),"N");
    std::string Fitmodel("([0]+[1]*x-[2]/fabs(x))*(fabs(x)>");
    Fitmodel+=std::to_string(pt)+std::string(")+-100.*(fabs(x)<=")+std::to_string(pt)+std::string(")");
    TF1 *fit3=new TF1(fitname3.c_str(),Fitmodel.c_str(),-150.,150.);
    fit3->SetParameter(0,fit2->GetParameter(0));
    fit3->SetParameter(1,fit2->GetParameter(1));
    fit3->SetParameter(2,fit2->GetParameter(2));
    histo->SetMinimum(-0.01);
    histo->SetMaximum(0.01);
    histo->GetXaxis()->SetTitle("genq*genp_{T}");
    histo->GetYaxis()->SetTitle("#deltak/k");
    histoA->SetBinContent(i+1,fit2->GetParameter(0));
    histoA->SetBinError(i+1,fit2->GetParError(0));
    histoM->SetBinContent(i+1,fit2->GetParameter(1));
    histoM->SetBinError(i+1,fit2->GetParError(1));
    histoepsilon->SetBinContent(i+1,fit2->GetParameter(2));
    histoepsilon->SetBinError(i+1,fit2->GetParError(2));
    status->SetBinContent(i+1,Status);
    nchi2->SetBinContent(i+1,fit2->GetChisquare()/fit2->GetNDF());
    double min=-2.4+i*0.1, max=-2.4+(i+1)*0.1;
    std::string title;
    title=std::to_string(min)+std::string("<#eta<")+std::to_string(max);
    histo->SetTitle(title.c_str());
    std::string canvname("canv");
    canvname+=std::to_string(i+1);
    TCanvas *c1=new TCanvas(canvname.c_str(), canvname.c_str());
    c1->cd();
    c1->Draw();
    histo->Draw();
    fit3->Draw("same");
    c1->SaveAs((canvname+std::string(".pdf")).c_str());
  }
  histoA->Write();
  histoM->Write();
  histoepsilon->Write();
  status->Write();
  nchi2->Write();
  output->Close();
}
