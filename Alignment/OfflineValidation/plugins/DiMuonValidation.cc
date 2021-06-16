// system included

#include <memory>
#include <cmath>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

// ROOT includes

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TLorentzVector.h"

//
// class declaration
//
class DiMuonValidation : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit DiMuonValidation(const edm::ParameterSet& pset)
      : TkTag_(pset.getParameter<std::string>("TkTag")),
        pair_mass_min(pset.getParameter<double>("Pair_mass_min")),
        pair_mass_max(pset.getParameter<double>("Pair_mass_max")),
        pair_mass_nbins(pset.getParameter<int>("Pair_mass_nbins")),
        pair_etaminpos(pset.getParameter<double>("Pair_etaminpos")),
        pair_etamaxpos(pset.getParameter<double>("Pair_etamaxpos")),
        pair_etaminneg(pset.getParameter<double>("Pair_etaminneg")),
        pair_etamaxneg(pset.getParameter<double>("Pair_etamaxneg")),
        variable_CosThetaCS_xmin(pset.getParameter<double>("Variable_CosThetaCS_xmin")),
        variable_CosThetaCS_xmax(pset.getParameter<double>("Variable_CosThetaCS_xmax")),
        variable_CosThetaCS_nbins(pset.getParameter<int>("Variable_CosThetaCS_nbins")),
        variable_DeltaEta_xmin(pset.getParameter<double>("Variable_DeltaEta_xmin")),
        variable_DeltaEta_xmax(pset.getParameter<double>("Variable_DeltaEta_xmax")),
        variable_DeltaEta_nbins(pset.getParameter<int>("Variable_DeltaEta_nbins")),
        variable_EtaMinus_xmin(pset.getParameter<double>("Variable_EtaMinus_xmin")),
        variable_EtaMinus_xmax(pset.getParameter<double>("Variable_EtaMinus_xmax")),
        variable_EtaMinus_nbins(pset.getParameter<int>("Variable_EtaMinus_nbins")),
        variable_EtaPlus_xmin(pset.getParameter<double>("Variable_EtaPlus_xmin")),
        variable_EtaPlus_xmax(pset.getParameter<double>("Variable_EtaPlus_xmax")),
        variable_EtaPlus_nbins(pset.getParameter<int>("Variable_EtaPlus_nbins")),
        variable_PhiCS_xmin(pset.getParameter<double>("Variable_PhiCS_xmin")),
        variable_PhiCS_xmax(pset.getParameter<double>("Variable_PhiCS_xmax")),
        variable_PhiCS_nbins(pset.getParameter<int>("Variable_PhiCS_nbins")),
        variable_PhiMinus_xmin(pset.getParameter<double>("Variable_PhiMinus_xmin")),
        variable_PhiMinus_xmax(pset.getParameter<double>("Variable_PhiMinus_xmax")),
        variable_PhiMinus_nbins(pset.getParameter<int>("Variable_PhiMinus_nbins")),
        variable_PhiPlus_xmin(pset.getParameter<double>("Variable_PhiPlus_xmin")),
        variable_PhiPlus_xmax(pset.getParameter<double>("Variable_PhiPlus_xmax")),
        variable_PhiPlus_nbins(pset.getParameter<int>("Variable_PhiPlus_nbins")),
        variable_PairPt_xmin(pset.getParameter<double>("Variable_PairPt_xmin")),
        variable_PairPt_xmax(pset.getParameter<double>("Variable_PairPt_xmax")),
        variable_PairPt_nbins(pset.getParameter<int>("Variable_PairPt_nbins")) {
    usesResource(TFileService::kSharedResource);
    theTrackCollectionToken = consumes<reco::TrackCollection>(TkTag_);

    variables_min[0] = variable_CosThetaCS_xmin;
    variables_min[1] = variable_DeltaEta_xmin;
    variables_min[2] = variable_EtaMinus_xmin;
    variables_min[3] = variable_EtaPlus_xmin;
    variables_min[4] = variable_PhiCS_xmin;
    variables_min[5] = variable_PhiMinus_xmin;
    variables_min[6] = variable_PhiPlus_xmin;
    variables_min[7] = variable_PairPt_xmin;

    variables_max[0] = variable_CosThetaCS_xmax;
    variables_max[1] = variable_DeltaEta_xmax;
    variables_max[2] = variable_EtaMinus_xmax;
    variables_max[3] = variable_EtaPlus_xmax;
    variables_max[4] = variable_PhiCS_xmax;
    variables_max[5] = variable_PhiMinus_xmax;
    variables_max[6] = variable_PhiPlus_xmax;
    variables_max[7] = variable_PairPt_xmax;

    variables_bins_number[0] = variable_CosThetaCS_nbins;
    variables_bins_number[1] = variable_DeltaEta_nbins;
    variables_bins_number[2] = variable_EtaMinus_nbins;
    variables_bins_number[3] = variable_EtaPlus_nbins;
    variables_bins_number[4] = variable_PhiCS_nbins;
    variables_bins_number[5] = variable_PhiMinus_nbins;
    variables_bins_number[6] = variable_PhiPlus_nbins;
    variables_bins_number[7] = variable_PairPt_nbins;
  }

  ~DiMuonValidation() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  const double mu_mass2 = 0.105658 * 0.105658;  //The invariant mass of muon 105.658MeV

  //===========Parameters==========

  std::string TkTag_;

  double pair_mass_min;
  double pair_mass_max;
  int pair_mass_nbins;
  double pair_etaminpos;
  double pair_etamaxpos;
  double pair_etaminneg;
  double pair_etamaxneg;

  double variable_CosThetaCS_xmin;
  double variable_CosThetaCS_xmax;
  int variable_CosThetaCS_nbins;

  double variable_DeltaEta_xmin;
  double variable_DeltaEta_xmax;
  int variable_DeltaEta_nbins;

  double variable_EtaMinus_xmin;
  double variable_EtaMinus_xmax;
  int variable_EtaMinus_nbins;

  double variable_EtaPlus_xmin;
  double variable_EtaPlus_xmax;
  int variable_EtaPlus_nbins;

  double variable_PhiCS_xmin;
  double variable_PhiCS_xmax;
  int variable_PhiCS_nbins;

  double variable_PhiMinus_xmin;
  double variable_PhiMinus_xmax;
  int variable_PhiMinus_nbins;

  double variable_PhiPlus_xmin;
  double variable_PhiPlus_xmax;
  int variable_PhiPlus_nbins;

  double variable_PairPt_xmin;
  double variable_PairPt_xmax;
  int variable_PairPt_nbins;

  //==================================================
private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;

  // ----------member data ---------------------------
  edm::Service<TFileService> fs;
  edm::EDGetTokenT<reco::TrackCollection> theTrackCollectionToken;

  const static int variables_number = 8;
  TH2D* th2d_mass_variables[variables_number];
  TString tstring_variables_name[variables_number] = {
      "CosThetaCS", "DeltaEta", "EtaMinus", "EtaPlus", "PhiCS", "PhiMinus", "PhiPlus", "Pt"};

  int variables_bins_number[variables_number];  // = {20, 20, 12, 12, 20, 16, 16, 100};
  double variables_min[variables_number];       // = {-1, -4.8, -2.4, -2.4, -M_PI / 2, -M_PI, -M_PI, 0};
  double variables_max[variables_number];       // = {+1, +4.8, +2.4, +2.4, +M_PI / 2, +M_PI, +M_PI, 100};
};

DiMuonValidation::~DiMuonValidation() = default;

//
// member functions
//

// ------------ method called for each event  ------------
void DiMuonValidation::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  const reco::TrackCollection& tC = iEvent.get(theTrackCollectionToken);

  TLorentzVector TLVect_mother(0., 0., 0., 0.);
  for (reco::TrackCollection::const_iterator track1 = tC.begin(); track1 != tC.end(); track1++) {
    TLorentzVector TLVect_track1(track1->px(),
                                 track1->py(),
                                 track1->pz(),
                                 sqrt((track1->p() * track1->p()) + mu_mass2));  //old 106

    for (reco::TrackCollection::const_iterator track2 = track1 + 1; track2 != tC.end(); track2++) {
      if (track1->charge() == track2->charge()) {
        continue;
      }  // only reconstruct opposite charge pair

      TLorentzVector TLVect_track2(
          track2->px(), track2->py(), track2->pz(), sqrt((track2->p() * track2->p()) + mu_mass2));

      TLVect_mother = TLVect_track1 + TLVect_track2;
      double mother_mass = TLVect_mother.M();
      double mother_pt = TLVect_mother.Pt();

      int charge1 = track1->charge();
      double etaMu1 = track1->eta();
      double phiMu1 = track1->phi();
      double ptMu1 = track1->pt();

      int charge2 = track2->charge();
      double etaMu2 = track2->eta();
      double phiMu2 = track2->phi();
      double ptMu2 = track2->pt();

      if (charge1 < 0) {  // use Mu+ for charge1, Mu- for charge2
        std::swap(charge1, charge2);
        std::swap(etaMu1, etaMu2);
        std::swap(phiMu1, phiMu2);
        std::swap(ptMu1, ptMu2);
      }
      //eta cut
      if (etaMu1 < pair_etaminpos or etaMu1 > pair_etamaxpos or etaMu2 < pair_etaminneg or etaMu2 > pair_etamaxneg) {
        continue;
      }

      double delta_eta = etaMu1 - etaMu2;

      double muplus = 1.0 / sqrt(2.0) * (TLVect_track1.E() + TLVect_track1.Z());
      double muminus = 1.0 / sqrt(2.0) * (TLVect_track1.E() - TLVect_track1.Z());
      double mubarplus = 1.0 / sqrt(2.0) * (TLVect_track2.E() + TLVect_track2.Z());
      double mubarminus = 1.0 / sqrt(2.0) * (TLVect_track2.E() - TLVect_track2.Z());
      //double costheta = 2.0 / Q.Mag() / sqrt(pow(Q.Mag(), 2) + pow(Q.Pt(), 2)) * (muplus * mubarminus - muminus * mubarplus);
      double costhetaCS = 2.0 / TLVect_mother.Mag() / sqrt(pow(TLVect_mother.Mag(), 2) + pow(TLVect_mother.Pt(), 2)) *
                          (muplus * mubarminus - muminus * mubarplus);

      TLorentzVector Pbeam(0., 0., 3500., 3500.);
      TVector3 R = Pbeam.Vect().Cross(TLVect_mother.Vect());
      TVector3 Runit = R.Unit();
      TVector3 Qt = TLVect_mother.Vect();
      Qt.SetZ(0);
      TVector3 Qtunit = Qt.Unit();
      TLorentzVector D(TLVect_track1 - TLVect_track2);
      TVector3 Dt = D.Vect();
      Dt.SetZ(0);
      double tanphi = sqrt(pow(TLVect_mother.Mag(), 2) + pow(TLVect_mother.Pt(), 2)) / TLVect_mother.Mag() *
                      Dt.Dot(Runit) / Dt.Dot(Qtunit);
      double phiCS = atan(tanphi);

      if (mother_mass > pair_mass_min && mother_mass < pair_mass_max) {
        th2d_mass_variables[0]->Fill(mother_mass, costhetaCS, 1);
        th2d_mass_variables[1]->Fill(mother_mass, delta_eta, 1);
        th2d_mass_variables[2]->Fill(mother_mass, etaMu2, 1);
        th2d_mass_variables[3]->Fill(mother_mass, etaMu1, 1);
        th2d_mass_variables[4]->Fill(mother_mass, phiCS, 1);
        th2d_mass_variables[5]->Fill(mother_mass, phiMu2, 1);
        th2d_mass_variables[6]->Fill(mother_mass, phiMu1, 1);
        th2d_mass_variables[7]->Fill(mother_mass, mother_pt, 1);
      }
    }
  }
}

// ------------ method called once each job just before starting event loop  ------------
void DiMuonValidation::beginJob() {
  for (int i = 0; i < variables_number; i++) {
    TString th2d_name = Form("th2d_mass_%s", tstring_variables_name[i].Data());
    th2d_mass_variables[i] = fs->make<TH2D>(th2d_name,
                                            th2d_name,
                                            pair_mass_nbins,
                                            pair_mass_min,
                                            pair_mass_max,
                                            variables_bins_number[i],
                                            variables_min[i],
                                            variables_max[i]);
  }
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void DiMuonValidation::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setComment("Validates alignment payloads by evaluating bias in Z->mm mass distributions");
  desc.addUntracked<int>("compressionSettings", -1);

  desc.add<std::string>("TkTag", "ALCARECOTkAlZMuMu");

  desc.add<double>("Pair_mass_min", 60);
  desc.add<double>("Pair_mass_max", 120);
  desc.add<int>("Pair_mass_nbins", 120);
  desc.add<double>("Pair_etaminpos", 60);
  desc.add<double>("Pair_etamaxpos", 60);
  desc.add<double>("Pair_etaminneg", 60);
  desc.add<double>("Pair_etamaxneg", 60);

  desc.add<double>("Variable_CosThetaCS_xmin", -1.);
  desc.add<double>("Variable_CosThetaCS_xmax", 1.);
  desc.add<int>("Variable_CosThetaCS_nbins", 20);

  desc.add<double>("Variable_DeltaEta_xmin", -4.8);
  desc.add<double>("Variable_DeltaEta_xmax", 4.8);
  desc.add<int>("Variable_DeltaEta_nbins", 20);

  desc.add<double>("Variable_EtaMinus_xmin", -2.4);
  desc.add<double>("Variable_EtaMinus_xmax", 2.4);
  desc.add<int>("Variable_EtaMinus_nbins", 12);

  desc.add<double>("Variable_EtaPlus_xmin", -2.4);
  desc.add<double>("Variable_EtaPlus_xmax", 2.4);
  desc.add<int>("Variable_EtaPlus_nbins", 12);

  desc.add<double>("Variable_PhiCS_xmin", -M_PI / 2);
  desc.add<double>("Variable_PhiCS_xmax", M_PI / 2);
  desc.add<int>("Variable_PhiCS_nbins", 20);

  desc.add<double>("Variable_PhiMinus_xmin", -M_PI);
  desc.add<double>("Variable_PhiMinus_xmax", M_PI);
  desc.add<int>("Variable_PhiMinus_nbins", 16);

  desc.add<double>("Variable_PhiPlus_xmin", -M_PI);
  desc.add<double>("Variable_PhiPlus_xmax", M_PI);
  desc.add<int>("Variable_PhiPlus_nbins", 16);

  desc.add<double>("Variable_PairPt_xmin", 0.);
  desc.add<double>("Variable_PairPt_xmax", 100.);
  desc.add<int>("Variable_PairPt_nbins", 100);

  descriptions.add("DiMuonValidation", desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DiMuonValidation);
