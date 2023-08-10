// Usage:
// root -b
// [0].L readZtoMMNtuple.C++
// [1]readZtoMMNtuple("<inputFileName.root>")

#include <TH1.h>
#include <TH2F.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <TLorentzVector.h>

// number of bins for binning the eta/phi map of sagitta
unsigned int k_NBINS = 24;

// ansatz from eq. 5 of https://arxiv.org/pdf/2212.07338.pdf
double pTcorrector(const double& unCorrPt, const double& delta, const float& charge) {
  return unCorrPt / (1 - charge * delta * unCorrPt);
}

std::pair<int, int> findEtaPhiBin(const TH2* h2, const float& eta, const float& phi) {
  const auto& etaBin = h2->GetXaxis()->FindBin(eta) - 1;
  const auto& phiBin = h2->GetYaxis()->FindBin(phi) - 1;
  return std::make_pair(etaBin, phiBin);
}

void readTree(const char* filename) {
  TFile* file = TFile::Open(filename, "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return;
  }

  TTree* tree = dynamic_cast<TTree*>(file->Get("ZtoMMNtuple/tree"));
  if (!tree) {
    std::cerr << "Error: TTree 'myTree' not found in file" << std::endl;
    file->Close();
    return;
  }

  // Branch variables
  Float_t mass;
  Float_t posTrackDz, negTrackDz;
  Float_t posTrackD0, negTrackD0;
  Float_t posTrackEta, negTrackEta;
  Float_t posTrackPhi, negTrackPhi;
  Float_t posTrackPt, negTrackPt;
  Float_t genPosMuonEta, genNegMuonEta;
  Float_t genPosMuonPhi, genNegMuonPhi;
  Float_t genPosMuonPt, genNegMuonPt;

  // Set branch addresses
  tree->SetBranchAddress("mass", &mass);
  tree->SetBranchAddress("posTrackDz", &posTrackDz);
  tree->SetBranchAddress("negTrackDz", &negTrackDz);
  tree->SetBranchAddress("posTrackD0", &posTrackD0);
  tree->SetBranchAddress("negTrackD0", &negTrackD0);
  tree->SetBranchAddress("posTrackEta", &posTrackEta);
  tree->SetBranchAddress("negTrackEta", &negTrackEta);
  tree->SetBranchAddress("posTrackPhi", &posTrackPhi);
  tree->SetBranchAddress("negTrackPhi", &negTrackPhi);
  tree->SetBranchAddress("posTrackPt", &posTrackPt);
  tree->SetBranchAddress("negTrackPt", &negTrackPt);
  tree->SetBranchAddress("genPosMuonEta", &genPosMuonEta);
  tree->SetBranchAddress("genNegMuonEta", &genNegMuonEta);
  tree->SetBranchAddress("genPosMuonPhi", &genPosMuonPhi);
  tree->SetBranchAddress("genNegMuonPhi", &genNegMuonPhi);
  tree->SetBranchAddress("genPosMuonPt", &genPosMuonPt);
  tree->SetBranchAddress("genNegMuonPt", &genNegMuonPt);

  std::string outputString(filename);
  std::string oldSubstring = "ZmmNtuple_";
  std::string newSubstring = "histos_";
  outputString.replace(outputString.find(oldSubstring), oldSubstring.length(), newSubstring);

  TFile* outputFile = TFile::Open(outputString.c_str(), "RECREATE");

  TH1F* hposPt = new TH1F("posPt", "p_{T} for positive track;#mu^{+} p_{T} [GeV];n. events", 100, 0., 100.);
  TH1F* hnegPt = new TH1F("negPt", "p_{T} for negative track;#mu^{-} p_{T} [GeV];n. events", 100, 0., 100.);

  TH1F* hposCorrPt =
      new TH1F("posCorrPt", "corrected p_{T} for positive track;#mu^{+} #hat{p}_{T} [GeV];n. events", 100, 0., 100.);
  TH1F* hnegCorrPt =
      new TH1F("negCorrPt", "corrected p_{T} for negative track;#mu^{-} #hat{p}_{T} [GeV];n. events", 100, 0., 100.);

  TH1F* hposAvgCorrPt = new TH1F(
      "posAvgCorrPt", "corrected (average) p_{T} for positive track;#mu^{+} #hat{p}_{T} [GeV];n. events", 100, 0., 100.);
  TH1F* hnegAvgCorrPt = new TH1F(
      "negAvgCorrPt", "corrected (average) p_{T} for negative track;#mu^{-} #hat{p}_{T} [GeV];n. events", 100, 0., 100.);

  TH1F* h1 = new TH1F("deltaPosPt",
                      "#Delta p_{T} for positive track;#mu^{+} p^{reco}_{T} - p^{gen}_{T} [GeV];n. events",
                      100,
                      -10.,
                      10.);
  TH1F* h2 = new TH1F("deltaNegPt",
                      "#Delta p_{T} for negative track;#mu^{-} p^{reco}_{T} - p^{gen}_{T} [GeV];n. events",
                      100,
                      -10.,
                      10.);

  TH1F* h1Corr =
      new TH1F("deltaPosCorrPt",
               "#Delta p_{T} (corrected) for positive track;#mu^{+} #hat{p}^{reco}_{T} - p^{gen}_{T} [GeV];n. events",
               100,
               -0.1,
               0.1);
  TH1F* h2Corr =
      new TH1F("deltaNegCorrPt",
               "#Delta p_{T} (corrected) for negative track;#mu^{-} #hat{p}^{reco}_{T} - p^{gen}_{T} [GeV];n. events",
               100,
               -0.1,
               0.1);

  TH1F* h1AvgCorr = new TH1F(
      "deltaPosAvgCorrPt",
      "#Delta p_{T} (corrected on average) for positive track;#mu^{+} #hat{p}^{reco}_{T} - p^{gen}_{T} [GeV];n. events",
      100,
      -10.,
      10.);
  TH1F* h2AvgCorr = new TH1F(
      "deltaNegAvgCorrPt",
      "#Delta p_{T} (corrected on average) for negative track;#mu^{-} #hat{p}^{reco}_{T} - p^{gen}_{T} [GeV];n. events",
      100,
      -10.,
      10.);

  TH1F* hDeltaPtOverPt =
      new TH1F("deltaPtOverPt", "#Delta p_{T}/p_{T}; (p^{reco}_{T} - p^{gen}_{T})/p^{gen}_{T};n. muons", 100, -1., 1.);
  TH1F* hDeltaCorrPtOverCorrPt =
      new TH1F("deltaCorrPtOverCorrPt",
               "#Delta p^{corr}_{T}/p^{corr}_{T}; (#hat{p}^{reco}_{T} - #hat{p}^{gen}_{T})/p^{gen}_{T};n. muons",
               100,
               -1.,
               1.);

  TH1F* hDeltaEta = new TH1F("deltaEta", "#Delta #eta; #eta^{reco}-#eta^{gen}; n. muons", 100, -0.01, 0.01);
  TH1F* hDeltaPhi = new TH1F("deltaPhi", "#Delta #phi; #phi^{reco}-#phi^{gen}; n. muons", 100, -0.01, 0.01);

  TH1F* h3 =
      new TH1F("deltaMass", "#Delta M(#mu#mu); M^{reco}_{#mu#mu} -M^{gen}_{#mu#mu} [GeV]; n.events", 100, -10., 10.);

  TH1F* hDeltaSagitta =
      new TH1F("deltaSagitta", "#delta_{sagitta};per muon #delta_{s} [GeV^{-1}];n. muons", 100, -10e-3, 10e-3);

  TH2F* hSagittaMap =
      new TH2F("hSagittaMap",
               "#LT#delta_{sagitta}#GT vs muon kinematics;muon #eta;muon #phi;#LT#delta_{sagitta}#GT [TeV^{-1}]",
               k_NBINS,
               -2.47,
               2.47,
               k_NBINS,
               -TMath::Pi(),
               TMath::Pi());

  TH2F* hCountsPerBin = new TH2F("hCountsPerBin",
                                 "# muons vs muon kinematics;muon #eta;muon #phi;# muons",
                                 k_NBINS,
                                 -2.47,
                                 2.47,
                                 k_NBINS,
                                 -TMath::Pi(),
                                 TMath::Pi());

  // initialize the maps
  std::map<std::pair<int, int>, float> countsPerBin;
  std::map<std::pair<int, int>, float> sagittaCorrections;
  for (unsigned int i = 0; i < k_NBINS; i++) {
    for (unsigned int j = 0; j < k_NBINS; j++) {
      const auto& index = std::make_pair(i, j);
      sagittaCorrections[index] = 0.f;
      countsPerBin[index] = 0.f;
    }
  }

  // Loop over entries
  Long64_t nEntries = tree->GetEntries();
  for (Long64_t entry = 0; entry < nEntries; ++entry) {
    tree->GetEntry(entry);

    TLorentzVector posTrack, negTrack, mother;
    posTrack.SetPtEtaPhiM(posTrackPt, posTrackEta, posTrackPhi, 0.105658);  // assume muon mass for tracks
    negTrack.SetPtEtaPhiM(negTrackPt, negTrackEta, negTrackPhi, 0.105658);
    mother = posTrack + negTrack;

    TLorentzVector genPosMuon, genNegMuon, genMother;
    genPosMuon.SetPtEtaPhiM(genPosMuonPt, genPosMuonEta, genPosMuonPhi, 0.105658);  // assume muon mass for tracks
    genNegMuon.SetPtEtaPhiM(genNegMuonPt, genNegMuonEta, genNegMuonPhi, 0.105658);
    genMother = genPosMuon + genNegMuon;

    // Print the values for each entry
    h1->Fill(posTrackPt - genPosMuonPt);
    h2->Fill(negTrackPt - genNegMuonPt);

    hDeltaPtOverPt->Fill((posTrackPt - genPosMuonPt) / genPosMuonPt);
    hDeltaPtOverPt->Fill((negTrackPt - genNegMuonPt) / genNegMuonPt);

    h3->Fill(mother.M() - genMother.M());

    hDeltaEta->Fill(posTrackEta - genPosMuonEta);
    hDeltaEta->Fill(negTrackEta - genNegMuonEta);

    hDeltaPhi->Fill(posTrackPhi - genPosMuonPhi);
    hDeltaPhi->Fill(negTrackPhi - genNegMuonPhi);

    // inverting equation 5 of https://arxiv.org/pdf/2212.07338.pdf
    double deltaSagittaPlus = (genPosMuonPt - posTrackPt) / (genPosMuonPt * posTrackPt);
    double deltaSagittaMinus = (negTrackPt - genNegMuonPt) / (genNegMuonPt * negTrackPt);

    //std::cout << "deltaSagittaPlus" <<  deltaSagittaPlus << " deltaSagittaMinus" <<  deltaSagittaMinus << std::endl;

    hposPt->Fill(posTrackPt);
    hnegPt->Fill(negTrackPt);

    hposCorrPt->Fill(pTcorrector(posTrackPt, deltaSagittaPlus, +1.f));
    hnegCorrPt->Fill(pTcorrector(negTrackPt, deltaSagittaMinus, -1.f));

    h1Corr->Fill(pTcorrector(posTrackPt, deltaSagittaPlus, +1.f) - genPosMuonPt);
    h2Corr->Fill(pTcorrector(negTrackPt, deltaSagittaMinus, -1.f) - genNegMuonPt);

    hDeltaSagitta->Fill(deltaSagittaPlus);
    hDeltaSagitta->Fill(deltaSagittaMinus);

    // find the bins
    const auto& indexPlus = findEtaPhiBin(hSagittaMap, posTrackEta, posTrackPhi);
    const auto& indexMinus = findEtaPhiBin(hSagittaMap, negTrackEta, negTrackPhi);

    //std::cout << "before updating: " <<  sagittaCorrections[indexPlus] << " " << sagittaCorrections[indexMinus] << std::endl;

    // update the correction in the bin
    sagittaCorrections[indexPlus] += deltaSagittaPlus;
    sagittaCorrections[indexMinus] += deltaSagittaMinus;

    //std::cout << "after updating: " <<  sagittaCorrections[indexPlus] << " " << sagittaCorrections[indexMinus] << std::endl;

    // update the count of muons in that bin
    countsPerBin[indexPlus] += 1.;
    countsPerBin[indexMinus] += 1.;
  }

  // take the average
  for (unsigned int i = 0; i < k_NBINS; i++) {
    for (unsigned int j = 0; j < k_NBINS; j++) {
      const auto& index = std::make_pair(i, j);
      //std::cout << index.first << ", " << index.second << " value: " << sagittaCorrections[index] << " / " << countsPerBin[index] << std::endl;
      sagittaCorrections[index] /= countsPerBin[index];
    }
  }

  // fill the histogram
  for (unsigned int i = 0; i < k_NBINS; i++) {
    for (unsigned int j = 0; j < k_NBINS; j++) {
      const auto& index = std::make_pair(i, j);
      hCountsPerBin->SetBinContent(i + 1, j + 1, countsPerBin[index]);
      hSagittaMap->SetBinContent(i + 1, j + 1, sagittaCorrections[index] * 10e3);  // 1/GeV = 1000/TeV
    }
  }

  // re-loop to apply the correction
  for (Long64_t entry = 0; entry < nEntries; ++entry) {
    tree->GetEntry(entry);

    // find the bins
    const auto& indexPlus = findEtaPhiBin(hSagittaMap, posTrackEta, posTrackPhi);
    const auto& indexMinus = findEtaPhiBin(hSagittaMap, negTrackEta, negTrackPhi);

    hposAvgCorrPt->Fill(pTcorrector(posTrackPt, sagittaCorrections[indexPlus], +1.f));
    hnegAvgCorrPt->Fill(pTcorrector(negTrackPt, sagittaCorrections[indexMinus], -1.f));

    h1AvgCorr->Fill(pTcorrector(posTrackPt, sagittaCorrections[indexPlus], +1.f) - genPosMuonPt);
    h2AvgCorr->Fill(pTcorrector(negTrackPt, sagittaCorrections[indexMinus], -1.f) - genNegMuonPt);

    hDeltaCorrPtOverCorrPt->Fill((pTcorrector(posTrackPt, sagittaCorrections[indexPlus], +1.f) - genPosMuonPt) /
                                 genPosMuonPt);
    hDeltaCorrPtOverCorrPt->Fill((pTcorrector(negTrackPt, sagittaCorrections[indexMinus], -1.f) - genNegMuonPt) /
                                 genNegMuonPt);
  }

  h1->Write();
  h2->Write();
  h3->Write();

  hposPt->Write();
  hnegPt->Write();

  hposCorrPt->Write();
  hnegCorrPt->Write();

  hposAvgCorrPt->Write();
  hnegAvgCorrPt->Write();

  h1Corr->Write();
  h2Corr->Write();

  h1AvgCorr->Write();
  h2AvgCorr->Write();

  hDeltaPtOverPt->Write();
  hDeltaCorrPtOverCorrPt->Write();

  hDeltaEta->Write();
  hDeltaPhi->Write();
  hDeltaSagitta->Write();
  hSagittaMap->Write();
  hCountsPerBin->Write();

  outputFile->Close();
  file->Close();
}

void readZtoMMNtuple(const char* filename) { readTree(filename); }
