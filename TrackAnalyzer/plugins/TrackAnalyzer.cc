// -*- C++ -*-
//
// Package:    CosmicTrackTool/TrackAnalyzer
// Class:      TrackAnalyzer
// 
/**\class TrackAnalyzer TrackAnalyzer.cc CosmicTrackTool/TrackAnalyzer/plugins/TrackAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Sumit Keshri
//         Created:  Sat, 30 May 2015 20:14:35 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"


#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DPGAnalysis/SiStripTools/interface/APVCyclePhaseCollection.h"
#include "CondFormats/SiStripObjects/interface/SiStripLatency.h"                
#include "CondFormats/DataRecord/interface/SiStripCondDataRecords.h"


#include "DataFormats/Provenance/interface/Timestamp.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/SiStripDetId/interface/SiStripDetId.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"

#include "DataFormats/SiStripDetId/interface/TECDetId.h"
#include "DataFormats/SiStripDetId/interface/TIBDetId.h"
#include "DataFormats/SiStripDetId/interface/TOBDetId.h"
#include "DataFormats/SiStripDetId/interface/TIDDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
#include "DataFormats/SiPixelDetId/interface/PixelBarrelName.h"
#include "DataFormats/SiPixelDetId/interface/PixelEndcapName.h"


#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"



#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <TTree.h>


//
// class declaration
//

// http://cms-service-sdtweb.web.cern.ch/cms-service-sdtweb/doxygen/new1/CMSSW_3_6_0/doc/html/d7/d7d/DQMEventInfo_8cc-source.html
 static inline double stampToReal(edm::Timestamp time)
 {
         return (time.value() >> 32) + 1e-6 * (time.value() & 0xffffffff);
         }





class TrackAnalyzer : public edm::EDAnalyzer {
   public:
      explicit TrackAnalyzer(const edm::ParameterSet&);
      ~TrackAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      void ClearInEventLoop();
      void ClearInEndRun();
      // ----------member data ---------------------------
      

      edm::InputTag trackTags_; 
      edm::RunNumber_t lastrunnum;
      edm::TimeValue_t lastruntime;
      int runno;
      TFile* file;
      TTree* tree;
//      TFile* file1;
//      TTree* tree1;
//      TH1F*  Number_of_Tracks;
//      TH1F*  number_of_events;

      float	TimeDuration;
      int	mode;
      int	events;
      int       track_BPIX  ; 	
      int       track_FPIX  ; 	
      int       track_PIXEL ; 	
      int       track_TEC   ; 	
      int       track_TECM  ; 	
      int       track_TECP  ; 	
      int       track_TOB   ; 	
      int       track_TIB   ;     
      int       track_TID   ;     
      int       track_TIDM  ;     
      int       track_TIDP  ; 	
/*      int       events_BPIX  ;
      int       events_FPIX  ;
      int       events_PIXEL ;
      int       events_TEC   ;
      int       events_TECM  ;
      int       events_TECP  ;
      int       events_TOB   ;
      int       events_TIB   ;
      int       events_TID   ;
      int       events_TIDM  ;
      int       events_TIDP  ;
*/
      int	ntrk	     ;
   
      std::vector<int>		number_of_tracks;
      std::vector<int>		number_of_tracks_PIX;
      std::vector<int>		number_of_tracks_FPIX;
      std::vector<int>		number_of_tracks_BPIX;
      std::vector<int>		number_of_tracks_TEC;
      std::vector<int>		number_of_tracks_TECP;
      std::vector<int>		number_of_tracks_TECM;
      std::vector<int>		number_of_tracks_TOB;
      std::vector<int>		number_of_tracks_TIB;
      std::vector<int>		number_of_tracks_TID;
      std::vector<int>		number_of_tracks_TIDP;
      std::vector<int>		number_of_tracks_TIDM;
/*      std::vector<int>		number_of_events_PIX;
      std::vector<int>		number_of_events_BPIX;
      std::vector<int>		number_of_events_FPIX;
      std::vector<int>		number_of_events_TEC;
      std::vector<int>		number_of_events_TECM;
      std::vector<int>		number_of_events_TECP;
      std::vector<int>		number_of_events_TIB;
      std::vector<int>		number_of_events_TOB;
      std::vector<int>		number_of_events_TID;
      std::vector<int>		number_of_events_TIDM;
      std::vector<int>		number_of_events_TIDP;
*/
      std::vector<int>		number_of_events;
      std::vector<edm::RunNumber_t>		runnum;
      std::vector<double>	pt;
      std::vector<double>	charge;
      std::vector<double>	chi2;
      std::vector<double>	chi2_ndof;
      std::vector<double>	eta;
      std::vector<double>	theta;
      std::vector<double>	phi;
      std::vector<double>	p;
      std::vector<double>	d0;
      std::vector<double>	dz;
      std::vector<double>	nvh;
      std::vector<double>	run_time;
/*      std::vector<double>
      std::vector<double>
      std::vector<double>
      std::vector<double>
 */

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
TrackAnalyzer::TrackAnalyzer(const edm::ParameterSet& iConfig) : 
        trackTags_(iConfig.getUntrackedParameter<edm::InputTag>("tracks"))

{
   //now do what ever initialization is needed

}


TrackAnalyzer::~TrackAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//
/*void TrackAnalyzer::ClearInEventLoop() {

	pt			.clear();
	charge			.clear(); 
	chi2			.clear();
	chi2_ndof		.clear();
	eta			.clear();
	theta			.clear();
	phi			.clear();
	p			.clear();
	d0			.clear();
	dz			.clear();
	nvh			.clear();

}
*/

void TrackAnalyzer::ClearInEndRun() {

	pt			.clear();
	charge			.clear(); 
	chi2			.clear();
	chi2_ndof		.clear();
	eta			.clear();
	theta			.clear();
	phi			.clear();
	p			.clear();
	d0			.clear();
	dz			.clear();
	nvh			.clear();
	number_of_tracks	.clear();
        number_of_events	.clear();
/*        number_of_events_PIX	.clear();	
        number_of_events_BPIX	.clear();	
        number_of_events_FPIX	.clear();	
        number_of_events_TEC	.clear();	
        number_of_events_TECM	.clear();	
        number_of_events_TECP	.clear();	
        number_of_events_TIB	.clear();	
        number_of_events_TOB	.clear();	
        number_of_events_TID	.clear();	
        number_of_events_TIDM	.clear();	
        number_of_events_TIDP	.clear();
*/	
        runnum			.clear();
	run_time		.clear();
        number_of_tracks_PIX	.clear();
        number_of_tracks_FPIX	.clear();
        number_of_tracks_BPIX	.clear();
        number_of_tracks_TEC	.clear();
        number_of_tracks_TECM	.clear();
        number_of_tracks_TECP	.clear();
        number_of_tracks_TOB	.clear();
        number_of_tracks_TIB	.clear();
        number_of_tracks_TID	.clear();
        number_of_tracks_TIDM	.clear();
        number_of_tracks_TIDP	.clear();
}

// ------------ method called for each event  ------------
void
TrackAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

//   runno   = int (iEvent.getRun().run());     
   edm::ESHandle<MagneticField> magfield;
   iSetup.get<IdealMagneticFieldRecord>().get(magfield);
   float B_=magfield.product()->inTesla(GlobalPoint(0,0,0)).mag();
   std::cout<< " magfield " << B_ << std::endl;



   edm::Timestamp ts_begin = iEvent.getRun().beginTime();
   double t_begin = stampToReal(ts_begin);
   edm::Timestamp ts_end = iEvent.getRun().endTime();
   double t_end = stampToReal(ts_end);

   lastruntime = t_end - t_begin;
   lastrunnum = iEvent.getRun().run();
   TimeDuration+=lastruntime;

   std::cout<<"lastruntime : "<<lastruntime<<"       lastrunnum : "<<lastrunnum<<std::endl;


   edm::ESHandle<SiStripLatency> apvlat;
   iSetup.get<SiStripLatencyRcd>().get(apvlat);
   if(apvlat->singleReadOutMode()==1) mode = 47; // peak mode
   if(apvlat->singleReadOutMode()==0) mode = 37; // deco mode

   std::cout<< "Mode : "<<mode<<std::endl;

   using reco::TrackCollection;
   edm::Handle<TrackCollection> tracks;
   iEvent.getByLabel(trackTags_, tracks);

   for(TrackCollection::const_iterator itTrack1 = tracks->begin(); itTrack1 != tracks->end(); ++itTrack1)
        {
   pt			.push_back(itTrack1->pt());
   charge		.push_back(itTrack1->charge());
   chi2			.push_back(itTrack1->chi2());
   chi2_ndof		.push_back(itTrack1->normalizedChi2());
   eta			.push_back(itTrack1->eta());
   theta		.push_back(itTrack1->theta());
   phi			.push_back(itTrack1->phi());
   p			.push_back(itTrack1->p());
   d0			.push_back(itTrack1->d0());
   dz			.push_back(itTrack1->dz());
   nvh			.push_back(itTrack1->numberOfValidHits());

std::cout<<"@@@@@@@@@@@@@@@@@@ pt: "<<pt.at(0)<<std::endl;
                int nhitinBPIX  	= 0;
                int nhitinFPIX  	= 0;
                int nhitinPIXEL 	= 0;
                int nhitinTEC 		= 0;
                int nhitinTOB 		= 0;
                int nhitinTIB 		= 0;
                int nhitinTID 		= 0;
                int nhitinTECminus 	= 0;
                int nhitinTECplus 	= 0;
                int nhitinTIDminus 	= 0;
                int nhitinTIDplus 	= 0;

         for(trackingRecHit_iterator iHit1 = itTrack1->recHitsBegin(); iHit1 != itTrack1->recHitsEnd(); ++iHit1)
                {



                   const DetId detId1((*iHit1)->geographicalId());
                   const int subdetId1 = detId1.subdetId();
                   if (!(*iHit1)->isValid()) continue; // only real hits count as in trackp->numberOfValidHits()



//************************************************************************************************/
// 					 Hit information in PixelBarrel                          //
//***********************************************************************************************/

                     if (PixelSubdetector::PixelBarrel == subdetId1) 
			   { 
			       ++nhitinBPIX; ++nhitinPIXEL;
                               PixelBarrelName pxbId1(detId1);
                    /*      if(pxbId1.isHalfModule())
			     {
                               ++nhitinPXBminus;
                             }
                          else
			     {
                               ++nhitinPXBplus;
			     }
		    */
                	   }



//***********************************************************************************************/
//				Hit information in PixelEndcap                                  //
//**********************************************************************************************/


                      else if (PixelSubdetector::PixelEndcap == subdetId1) 
			   {
			      ++nhitinFPIX; ++nhitinPIXEL;
                           }



//***********************************************************************************************/
//				Hit information in TEC						//
//**********************************************************************************************/

                      else if (SiStripDetId::TEC == subdetId1)
                           {
                              ++nhitinTEC;
                              TECDetId tecId1(detId1);
                          if (tecId1.isZMinusSide()) 
			     {
                              ++nhitinTECminus;
                             }
                          else 
			     {
                              ++nhitinTECplus;
                             }

                           }



//***********************************************************************************************/
//				Hit information in TOB		                                //
//**********************************************************************************************/

                       else if (SiStripDetId::TOB == subdetId1)
                           {
                              ++nhitinTOB;
                              TOBDetId tobId1(detId1);
                     /*     if (tobId1.isZMinusSide())
			     {
                              ++nhitinTOBminus;
                             }

                          else 
			     {
                              ++nhitinTOBplus;
                             }
		     */
                           }



//***********************************************************************************************/
//				Hit information in TIB		                                //
//**********************************************************************************************/

                        else if (SiStripDetId::TIB == subdetId1)
                           {
                              ++nhitinTIB;
                              TIBDetId tibId1(detId1);
                    /*      if (tibId1.isZMinusSide()) 
			   {
                              ++nhitinTIBminus;
                           }

                            else 
			     {
                              ++nhitinTIBplus;
                             }
                    */
                           }



//***********************************************************************************************/
//				Hit information in TID		                                //
//**********************************************************************************************/


                        else if (SiStripDetId::TID == subdetId1)
                           {
                              ++nhitinTID;
                              TIDDetId tidId1(detId1);
                          if (tidId1.isZMinusSide())
                             {
                              ++nhitinTIDminus;
                             }

                          else 
			     {
                              ++nhitinTIDplus;
                             }
                           }




                    } // for Loop over Hits

     
                if (nhitinBPIX		 > 0 )    	{track_BPIX++	;}     
                if (nhitinFPIX  	 > 0 )    	{track_FPIX++	;} 
                if (nhitinPIXEL 	 > 0 )   	{track_PIXEL++	;} 
                if (nhitinTEC 		 > 0 )    	{track_TEC++	;}
                if (nhitinTECminus 	 > 0 ) 		{track_TECM++	;} 
                if (nhitinTECplus	 > 0 )  	{track_TECP++	;}
                if (nhitinTOB		 > 0 )    	{track_TOB++	;}
                if (nhitinTIB		 > 0 )    	{track_TIB++	;}
                if (nhitinTID		 > 0 )    	{track_TID++	;}
                if (nhitinTIDminus	 > 0 ) 		{track_TIDM++	;}
                if (nhitinTIDplus	 > 0 )  	{track_TIDP++	;}


		ntrk++;
//		ntrk_runnum++;
		}  // for Loop over TrackCollection

		events++;		

/*                if ( track_BPIX  > 0 )    	{events_BPIX++	;}     
                if ( track_FPIX  > 0 )    	{events_FPIX++	;} 
                if ( track_PIXEL > 0 )   	{events_PIXEL++	;} 
                if ( track_TEC   > 0 )    	{events_TEC++	;}
                if ( track_TECM  > 0 ) 		{events_TECM++	;} 
                if ( track_TECP  > 0 )  	{events_TECP++	;}
                if ( track_TOB   > 0 )    	{events_TOB++	;}
                if ( track_TIB   > 0 )    	{events_TIB++	;}
                if ( track_TID   > 0 )    	{events_TID++	;}
                if ( track_TIDM  > 0 ) 		{events_TIDM++	;}
                if ( track_TIDP  > 0 )  	{events_TIDP++	;}
*/


//   tree1 ->Fill();
//   ClearInEventLoop();
#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}   //Event Loop


// ------------ method called once each job just before starting event loop  ------------
void 
TrackAnalyzer::beginJob()
{
   file 		= new TFile("Outfile.root","RECREATE");
   tree			= new TTree("cosmics","");
//   file1		= new TFile("Kinematic.root","RECREATE");
//   tree1		= new TTree("CosmicKinematics","");
/*   const bool oldAddDir = TH1::AddDirectoryStatus();
   TH1::AddDirectory(true);
   Number_of_Tracks 	= new TH1F("Number_of_Tracks","Track_Distribution",7, 0., 7.);  
 
*/
//  number_of_events 	= new TH1F("number_of_events","Events_Distribution",7, 0., 7.);  
   
//   ntrk 	= 0;
   tree 	->Branch("pt",&pt);
   tree 	->Branch("charge",&charge);
   tree 	->Branch("chi2",&chi2);
   tree 	->Branch("chi2_ndof",&chi2_ndof);
   tree 	->Branch("eta",&eta);
   tree 	->Branch("theta",&theta);
   tree 	->Branch("phi",&phi);
   tree 	->Branch("p",&p);
   tree 	->Branch("d0",&d0);
   tree 	->Branch("dz",&dz);
   tree 	->Branch("nvh",&nvh);
   tree		->Branch("run_time",&run_time);   
   tree		->Branch("runnum",&runnum);   
   tree		->Branch("number_of_events",&number_of_events);
   tree		->Branch("number_of_tracks",&number_of_tracks);
   tree		->Branch("number_of_tracks_PIX",&number_of_tracks_PIX);
   tree		->Branch("number_of_tracks_FPIX",&number_of_tracks_FPIX);
   tree		->Branch("number_of_tracks_BPIX",&number_of_tracks_BPIX);
   tree		->Branch("number_of_tracks_TID",&number_of_tracks_TID);
   tree		->Branch("number_of_tracks_TIDM",&number_of_tracks_TIDM);
   tree		->Branch("number_of_tracks_TIDP",&number_of_tracks_TIDP);
   tree		->Branch("number_of_tracks_TIB",&number_of_tracks_TIB);
   tree		->Branch("number_of_tracks_TEC",&number_of_tracks_TEC);
   tree		->Branch("number_of_tracks_TECP",&number_of_tracks_TECP);
   tree		->Branch("number_of_tracks_TECM",&number_of_tracks_TECM);
   tree		->Branch("number_of_tracks_TOB",&number_of_tracks_TOB);
/*   tree		->Branch("number_of_events_PIX",&number_of_events_PIX);
   tree		->Branch("number_of_events_FPIX",&number_of_events_FPIX);
   tree		->Branch("number_of_events_BPIX",&number_of_events_BPIX);
   tree		->Branch("number_of_events_TOB",&number_of_events_TOB);
   tree		->Branch("number_of_events_TIB",&number_of_events_TIB);
   tree		->Branch("number_of_events_TEC",&number_of_events_TEC);
   tree		->Branch("number_of_events_TECP",&number_of_events_TECP);
   tree		->Branch("number_of_events_TECM",&number_of_events_TECM);
   tree		->Branch("number_of_events_TID",&number_of_events_TID);
   tree		->Branch("number_of_events_TIDP",&number_of_events_TIDP);
   tree		->Branch("number_of_events_TIDM",&number_of_events_TIDM);
  */ 



//   TH1::AddDirectory(oldAddDir);

}

// ------------ method called once each job just after ending the event loop  ------------
void 
TrackAnalyzer::endJob() 
{

//   Number_of_Tracks->Fill(ntrk);
//   Number_of_events->Fill(events);
//   tree ->Fill();
   file->Write();
//   file1->Write();
//   ClearInEndRun();
}

// ------------ method called when starting to processes a run  ------------

void 
TrackAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
   TimeDuration 	= 0.0;
   lastruntime		= 0.0;
   lastrunnum		=0;
   ntrk 		= 0;
   events		= 0;
   track_BPIX           =0;  
   track_FPIX           =0;
   track_PIXEL          =0;
   track_TEC            =0;
   track_TECM           =0;
   track_TECP           =0;
   track_TOB            =0;
   track_TIB            =0;
   track_TID            =0;
   track_TIDM           =0;
   track_TIDP           =0;
/*   events_BPIX          =0;
   events_FPIX          =0;
   events_PIXEL         =0;
   events_TEC           =0;
   events_TECM          =0;
   events_TECP          =0;
   events_TOB           =0;
   events_TIB           =0;
   events_TID           =0;
   events_TIDM          =0;
   events_TIDP          =0;
  */

}


// ------------ method called when ending the processing of a run  ------------

void 
TrackAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
//std::cout<<"  ntrk  : "<<ntrk<<std::endl;
//   runno = int(lastrunnum);
    if (lastrunnum !=0 ){
std::cout<<"  events  : "<<events<<std::endl;
   number_of_tracks	.push_back(ntrk);
   run_time		.push_back(lastruntime);
   //runnum		.push_back(runno);
   runnum		.push_back(lastrunnum);
   number_of_tracks_PIX	.push_back(track_PIXEL ); 
   number_of_tracks_FPIX.push_back(track_FPIX ); 
   number_of_tracks_BPIX.push_back(track_BPIX); 
   number_of_tracks_TEC	.push_back(track_TEC  ); 
   number_of_tracks_TECM.push_back(track_TECM ); 
   number_of_tracks_TECP.push_back(track_TECP ); 
   number_of_tracks_TOB	.push_back(track_TOB  ); 
   number_of_tracks_TIB	.push_back(track_TIB  ); 
   number_of_tracks_TID	.push_back(track_TID  ); 
   number_of_tracks_TIDM.push_back(track_TIDM ); 
   number_of_tracks_TIDP.push_back(track_TIDP ); 
   number_of_events	.push_back(events);
/*   number_of_events_PIX	.push_back(events_PIXEL);
   number_of_events_BPIX.push_back(events_BPIX);
   number_of_events_FPIX.push_back(events_FPIX);
   number_of_events_TEC	.push_back(events_TEC);
   number_of_events_TECM.push_back(events_TECM);
   number_of_events_TECP.push_back(events_TECP);
   number_of_events_TIB	.push_back(events_TIB);
   number_of_events_TOB	.push_back(events_TOB);
   number_of_events_TID	.push_back(events_TID);
   number_of_events_TIDM.push_back(events_TIDM);
   number_of_events_TIDP.push_back(events_TIDP);
//   number_of_events ->Fill(events);
  */ 
   tree  ->Fill();

   ClearInEndRun();
}
//number_of_events.clear();
//   Number_of_Tracks->Fill(ntrk);
//   Number_of_events->Fill(events);
}


// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
TrackAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
TrackAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TrackAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TrackAnalyzer);
