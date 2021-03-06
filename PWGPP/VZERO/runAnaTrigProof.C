// 2010
// .x runAnaTrigProof.C(0,"/alice/data","LHC10h_000138396_hlt_clustering",28,kFALSE,"CMBACS2")
// 2011
// .x runAnaTrigProof.C(0,"/alice/data","LHC11h_000166532_p1_HLT",28,kTRUE,"CPBI")

void runAnaTrigProof(Int_t mode = 0, const char *folder = "/alice/data",
		     const char *dataset = "LHC10h_000138396_hlt_clustering",
		     Int_t workers=28,
		     Bool_t usePS = kFALSE,
		     const char *minBias = "CPBI",
		     Int_t firstFile = 0, Int_t lastFile = -1)
{
  gSystem->Load("libANALYSIS");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libOADB");
  gSystem->AddIncludePath("-I$ALICE_PHYSICS/include ");

  if (mode==0) {
    // Connect to Proof
    gEnv->SetValue("XSec.GSI.DelegProxy","2");
    Char_t *alienuser = gSystem->Getenv("alien_API_USER");
    cout<<"==> Your AliEn username is: "<<alienuser<<endl;

    TProof *p = TProof::Open(alienuser!=0 ? Form("%s@alice-caf.cern.ch",
						 alienuser) : "alice-caf.cern.ch",
			     workers>0 ? Form("workers=%d",workers) : "");

    //  gProof->GetManager()->SetROOTVersion("VO_ALICE@ROOT::v5-28-00f");
    gProof->EnablePackage("VO_ALICE@AliRoot::v5-02-11-AN");

    //    gProof->Exec("TGrid::Connect(\"alien://\")",kTRUE);
    //    gSystem->Setenv("OADB_PATH","alien:///alice/cern.ch/user/c/cheshkov/OADB");
    //    gProof->Exec("gSystem->Setenv(\"OADB_PATH\",\"alien:///alice/cern.ch/user/c/cheshkov/OADB\")",kTRUE);
    //    gSystem->Setenv("OADB_PATH","alien:///alice/cern.ch/user/a/atoia/OADB");
    //    gProof->Exec("gSystem->Setenv(\"OADB_PATH\",\"alien:///alice/cern.ch/user/a/atoia/OADB\")",kTRUE);
  }

  // Create the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("AliAnaFwdDet");

  AliESDInputHandler* esdH = new AliESDInputHandler();
  esdH->SetInactiveBranches("FMD AliRawDataErrorLogs CaloClusters Cascades EMCALCells EMCALTrigger ESDfriend.fTracks Kinks MuonTracks TrdTracks");
  //  esdH->SetReadFriends(kTRUE);
  esdH->SetReadFriends(kFALSE);
  mgr->SetInputEventHandler(esdH);

  // physics and centrality selection
  if (usePS) {
    gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
    AliPhysicsSelectionTask *physicsSelectionTask = AddTaskPhysicsSelection(kFALSE);
    // Trigger analysis defaults
    AliOADBTriggerAnalysis * oadbTrigAnalysis = new AliOADBTriggerAnalysis("CustomTA");
    oadbTrigAnalysis->SetZDCCorrParameters(0, 0, 4*0.7, 4*0.7);
    physicsSelectionTask->GetPhysicsSelection()->SetCustomOADBObjects(0,0,oadbTrigAnalysis);
    // DefaultPbPb
    AliOADBPhysicsSelection * oadbDefaultPbPb = new AliOADBPhysicsSelection("oadbCustomPbPb");
  
    Int_t triggerCount = 0;
    oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kMB,"+CPBI1-B-NOPF-ALLNOTRD,CPBI2_B1-B-[NOPF|PF]-ALLNOTRD","B",triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kMB,"+CPBI[1|2_B1]-AC-NOPF-ALLNOTRD","AC",triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kMB,"+CPBI[1|2_B1]-E-NOPF-ALLNOTRD","E",triggerCount);
    oadbDefaultPbPb->SetHardwareTrigger         ( triggerCount,"V0A && V0C");
    oadbDefaultPbPb->SetOfflineTrigger          ( triggerCount,"V0A && V0C && !V0ABG && !V0CBG && !TPCLaserWarmUp && ZDCTime");

    triggerCount++;
    // Bug in early running, CVHN_R2-B-NOPF-ALLNOTRD needed explicitly up to run 167814 (to be moved to custom object as soon as all other things are fine)
    //   oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kCentral,"+CVHN-B-NOPF-ALLNOTRD,CVHN_R2-B-NOPF-ALLNOTRD,CVHN-B-NOPF-CENTNOTRD,CVHN-B-NOPF-CENTNOTRD,CVHN-B-PF-ALLNOTRD,CVHN-B-PF-CENTNOTRD","B",triggerCount);
    // oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kCentral,"+CVHN[|_R2]-B-[NOPF|PF]-[ALL|CENT]NOTRD","B",triggerCount);
    // oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kCentral,"+CVHN-AC-NOPF-[ALL|CENT]NOTRD","AC",triggerCount);
    // oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kCentral,"+CVHN-E-NOPF-[ALL|CENT]NOTRD","E",triggerCount);
    // Also include the semicentral class in the online classes selection (in order not to loose event due to the different time sharing of central and semi-central)
    oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kCentral,"+CVHN[|_R2]-B-[NOPF|PF]-[ALL|CENT]NOTRD,CVLN[|_B2|_R1]-B-[NOPF|PF]-ALLNOTRD","B",triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kCentral,"+CVHN-AC-NOPF-[ALL|CENT]NOTRD,CVLN-AC-NOPF-ALLNOTRD","AC",triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kCentral,"+CVHN-E-NOPF-[ALL|CENT]NOTRD,CVLN-E-NOPF-ALLNOTRD","E",triggerCount);
    oadbDefaultPbPb->SetHardwareTrigger         ( triggerCount,"V0A && V0C && Central");
    oadbDefaultPbPb->SetOfflineTrigger          ( triggerCount,"V0A && V0C && !V0ABG && !V0CBG && !TPCLaserWarmUp && ZDCTime");

    triggerCount++;
    // semicentral includes central ones (and might have different downscaling)
    // Bug in early running, CVHN_R2-B-NOPF-ALLNOTRD,CVLN_B2-B-NOPF-ALLNOTRD needed explicitly up to run 167814 (to be moved to custom object as soon as all other things are fine)
    //   oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kSemiCentral,"+CVHN-B-NOPF-ALLNOTRD,CVHN_R2-B-NOPF-ALLNOTRD,CVHN-B-NOPF-CENTNOTRD,CVHN-B-PF-ALLNOTRD,CVHN-B-PF-CENTNOTRD,CVLN-B-NOPF-ALLNOTRD,CVLN_B2-B-NOPF-ALLNOTRD,CVLN-B-PF-ALLNOTRD","B",triggerCount);
    //  oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kSemiCentral,"+CVHN[|_R2]-B-[NOPF|PF]-[ALL|CENT]NOTRD,CVLN[|_B2]-B-[NOPF|PF]-ALLNOTRD","B",triggerCount);
    oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kSemiCentral,"+CVHN[|_R2]-B-[NOPF|PF]-[ALL|CENT]NOTRD,CVLN[|_B2|_R1]-B-[NOPF|PF]-ALLNOTRD","B",triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kSemiCentral,"+CVHN-AC-NOPF-[ALL|CENT]NOTRD,CVLN-AC-NOPF-ALLNOTRD","AC",triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kSemiCentral,"+CVHN-E-NOPF-[ALL|CENT]NOTRD,CVLN-E-NOPF-ALLNOTRD","E",triggerCount);
    oadbDefaultPbPb->SetHardwareTrigger         ( triggerCount,"V0A && V0C && SemiCentral && !Central");
    oadbDefaultPbPb->SetOfflineTrigger          ( triggerCount,"V0A && V0C && !V0ABG && !V0CBG && !TPCLaserWarmUp && ZDCTime");

    triggerCount++;
    oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kEMCEJE,"+CPBI2EJE-B-NOPF-CENTNOTRD","B",triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kEMCEJE,"+CPBI2EJE-ACE-NOPF-CENTNOTRD","ACE",triggerCount);
    oadbDefaultPbPb->SetHardwareTrigger         ( triggerCount,"V0A && V0C");
    // TODO EMC offline check missing, https://savannah.cern.ch/bugs/index.php?87104
    oadbDefaultPbPb->SetOfflineTrigger          ( triggerCount,"V0A && V0C && !V0ABG && !V0CBG && !TPCLaserWarmUp && ZDCTime");
    
    triggerCount++;
    oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kEMCEGA,"+CPBI2EGA-B-NOPF-CENTNOTRD","B",triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kEMCEGA,"+CPBI2EGA-ACE-NOPF-CENTNOTRD","ACE",triggerCount);
    oadbDefaultPbPb->SetHardwareTrigger         ( triggerCount,"V0A && V0C");
    // TODO EMC offline check missing, https://savannah.cern.ch/bugs/index.php?87104
    oadbDefaultPbPb->SetOfflineTrigger          ( triggerCount,"V0A && V0C && !V0ABG && !V0CBG && !TPCLaserWarmUp && ZDCTime");
  
    triggerCount++;
    oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kMUSPB,"+CPBI1MSL-B-NOPF-MUON","B",  triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kMUSPB,"+CPBI1MSL-ACE-NOPF-MUON","ACE",triggerCount);
    oadbDefaultPbPb->SetHardwareTrigger         ( triggerCount,"V0A && V0C");                                         
    oadbDefaultPbPb->SetOfflineTrigger          ( triggerCount,"V0A && V0C && !V0ABG && !V0CBG && !TPCLaserWarmUp && ZDCTime");

    triggerCount++;
    oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kMUSHPB,"+CPBI1MSH-B-NOPF-MUON","B",  triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kMUSHPB,"+CPBI1MSH-ACE-NOPF-MUON","ACE",triggerCount);
    oadbDefaultPbPb->SetHardwareTrigger         ( triggerCount,"V0A && V0C");                                         
    oadbDefaultPbPb->SetOfflineTrigger          ( triggerCount,"V0A && V0C && !V0ABG && !V0CBG && !TPCLaserWarmUp && ZDCTime");
    
    triggerCount++;
    oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kMuonUnlikePB,"+CPBI1MUL-B-NOPF-MUON","B",  triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kMuonUnlikePB,"+CPBI1MUL-ACE-NOPF-MUON","ACE",triggerCount);
    oadbDefaultPbPb->SetHardwareTrigger         ( triggerCount,"V0A && V0C");                                         
    oadbDefaultPbPb->SetOfflineTrigger          ( triggerCount,"V0A && V0C && !V0ABG && !V0CBG && !TPCLaserWarmUp && ZDCTime");

    triggerCount++;
    oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kMuonLikePB,"+CPBI1MLL-B-NOPF-MUON","B",  triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kMuonLikePB,"+CPBI1MLL-ACE-NOPF-MUON","ACE",triggerCount);
    oadbDefaultPbPb->SetHardwareTrigger         ( triggerCount,"V0A && V0C");                                         
    oadbDefaultPbPb->SetOfflineTrigger          ( triggerCount,"V0A && V0C && !V0ABG && !V0CBG && !TPCLaserWarmUp && ZDCTime");

    triggerCount++;
    oadbDefaultPbPb->AddCollisionTriggerClass   ( AliVEvent::kPHOSPb,"+CPBI2PHS-B-NOPF-CENTNOTRD","B",  triggerCount);
    oadbDefaultPbPb->AddBGTriggerClass          ( AliVEvent::kPHOSPb,"+CPBI2PHS-ACE-NOPF-CENTNOTRD","ACE",triggerCount);
    oadbDefaultPbPb->SetHardwareTrigger         ( triggerCount,"V0A && V0C");
    oadbDefaultPbPb->SetOfflineTrigger          ( triggerCount,"V0A && V0C && !V0ABG && !V0CBG && !TPCLaserWarmUp && ZDCTime");
  
    physicsSelectionTask->GetPhysicsSelection()->SetCustomOADBObjects(oadbDefaultPbPb,0,0); 
  }

  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskCentrality.C");
  AliCentralitySelectionTask *taskCentrality = AddTaskCentrality();

  // Create task
  if (mode==0) {
    gProof->Load(Form("%s/AliAnaVZEROTrigger.cxx++g",
		      gSystem->pwd()));
  }
  else {
    gROOT->LoadMacro(Form("%s/AliAnaVZEROTrigger.cxx++g",
			  gSystem->pwd()));
  }
  AliAnaVZEROTrigger *task = new AliAnaVZEROTrigger("AliAnaVZEROTrigger");
  task->SetMBTrigName(minBias);
  task->Setup("trigger.txt");
  if (usePS) task->SetUsePhysSel(kTRUE);
  
  // Add task
  mgr->AddTask(task);

  // Create containers for input/output
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  AliAnalysisDataContainer *coutput = 
    mgr->CreateContainer("coutput", TList::Class(), 
			 AliAnalysisManager::kOutputContainer, (!usePS) ? Form("VZERO.Trigger.%s.root",dataset) : Form("VZERO.Trigger.PS.%s.root",dataset));

  // Connect input/output
  mgr->ConnectInput(task, 0, cinput);
  mgr->ConnectOutput(task, 1, coutput);


  // Enable debug printouts
  mgr->SetDebugLevel(3);

  if (!mgr->InitAnalysis())
    return;

  mgr->PrintStatus();

  if (mode==0)
    mgr->StartAnalysis("proof", Form("%s/%s",folder,dataset));
  else {
    TGrid::Connect("alien://");
    TChain *chain = new TChain("esdTree");
    TGridResult *res = gGrid->Query(folder,"AliESDs.root");
    Int_t nFiles = res->GetEntries();
    if (lastFile < 0) lastFile = nFiles - 1;
    for(Int_t iFile = firstFile; iFile <= lastFile; ++iFile) {
      TString filename = res->GetKey(iFile, "turl");
      if(filename == "") continue;
      chain->AddFile(filename.Data());
    }
    mgr->StartAnalysis("local", chain);
  }
}

