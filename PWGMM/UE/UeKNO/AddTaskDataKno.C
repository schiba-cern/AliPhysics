///////////////////////////////////////////////////////////////////
//                                                               //
//            AddTaskDataKno Macro to run on grids               //
//                                                               //
//                                                               //
///////////////////////////////////////////////////////////////////
class AliAnalysisDataContainer;

AliAnalysisTaskDataKno* AddTaskDataKno(const Char_t* taskname="DataKno", Bool_t  useMC  = kTRUE, Bool_t performMCclosuretest = kFALSE, Bool_t IspPb = kFALSE, Double_t minpT=0.5, Double_t PtLmin = 5.0, Double_t PtLmax = 40.0, Double_t V0Mmin = 0.0, Double_t V0Mmax = 100.0, Bool_t IsTPConly = kTRUE, Bool_t TPCclustersVar1 = kFALSE, Bool_t TPCclustersVar2 = kFALSE, Bool_t NcrVar1 = kFALSE, Bool_t NcrVar2 = kFALSE, Bool_t ChisqTPCVar1 = kFALSE, Bool_t ChisqTPCVar2 = kFALSE, Bool_t ChisqITSVar1 = kFALSE, Bool_t ChisqITSVar2 = kFALSE, Bool_t ChisqITSmTPCVar1 = kFALSE, Bool_t ChisqITSmTPCVar2 = kFALSE, Bool_t DcazVar1 = kFALSE, Bool_t DcazVar2 = kFALSE, Bool_t GeoTPCVar1 = kFALSE, Bool_t GeoTPCVar2 = kFALSE, Bool_t GeoTPCVar3 = kFALSE, Bool_t GeoTPCVar4 = kFALSE, Bool_t SPDreqVar1 = kFALSE)
{
    // get the manager via the static access member. since it's static, you don't need
    // an instance of the class to call the function


    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) {
        return 0x0;
    }
    // get the input event handler this handler is part of the managing system and feeds events to your task
    if (!mgr->GetInputEventHandler()) {
        return 0x0;
    }

    // now you create an instance of your task
    AliAnalysisTaskDataKno* taskKno = new AliAnalysisTaskDataKno("taskKno");
    if(!taskKno) return 0x0;
    taskKno->SetUseMC(useMC);
    taskKno->SetMCclosureTest(performMCclosuretest);
    // add your task to the manager
    taskKno->SetPtMin(minpT);
    taskKno->SetIspPb(IspPb);
    taskKno->SetLeadingPtMin(PtLmin);
    taskKno->SetLeadingPtMax(PtLmax);
    taskKno->SetV0Mmin(V0Mmin);
    taskKno->SetV0Mmax(V0Mmax);
    // Systematic -------------------------------
    taskKno->SetNchTScut(IsTPConly);
    taskKno->SetTPCclustersVar1(TPCclustersVar1);
    taskKno->SetTPCclustersVar2(TPCclustersVar2);
    taskKno->SetNcrVar1(NcrVar1);
    taskKno->SetNcrVar2(NcrVar2);
    taskKno->SetChisqTPCVar1(ChisqTPCVar1);
    taskKno->SetChisqTPCVar2(ChisqTPCVar2);
    taskKno->SetChisqITSVar1(ChisqITSVar1);
    taskKno->SetChisqITSVar2(ChisqITSVar2);
    taskKno->SetChisqITSmTPCVar1(ChisqITSmTPCVar1);
    taskKno->SetChisqITSmTPCVar2(ChisqITSmTPCVar2);
    taskKno->SetDcazVar1(DcazVar1);
    taskKno->SetDcazVar2(DcazVar2);
    taskKno->SetGeoTPCVar1(GeoTPCVar1);
    taskKno->SetGeoTPCVar2(GeoTPCVar2);
    taskKno->SetGeoTPCVar3(GeoTPCVar3);
    taskKno->SetGeoTPCVar4(GeoTPCVar4);
    taskKno->SetSPDreqVar1(SPDreqVar1);
    // Systematic -------------------------------
    Int_t Sys = 0;
    if (IsTPConly) Sys = 1;
    if (TPCclustersVar1) Sys = 2;
    if (TPCclustersVar2) Sys = 3;
    if (NcrVar1) Sys = 4;
    if (NcrVar2) Sys = 5;
    if (ChisqTPCVar1) Sys = 6;
    if (ChisqTPCVar2) Sys = 7;
    if (ChisqITSVar1) Sys = 8;
    if (ChisqITSVar2) Sys = 9;
    if (ChisqITSmTPCVar1) Sys = 10;
    if (ChisqITSmTPCVar2) Sys = 11;
    if (DcazVar1) Sys = 12;
    if (DcazVar2) Sys = 13;
    if (GeoTPCVar1) Sys = 14;
    if (GeoTPCVar2) Sys = 15;
    if (GeoTPCVar3) Sys = 16;
    if (GeoTPCVar4) Sys = 17;
    if (SPDreqVar1) Sys = 18;
    
    mgr->AddTask(taskKno);

    mgr->ConnectInput(taskKno,0,mgr->GetCommonInputContainer());
    mgr->ConnectOutput(taskKno,1,mgr->CreateContainer(Form("cList%s_%1.2f_%1.1f_%1.1f_%1.1f_%1.1f_%i",taskname,minpT,PtLmin,PtLmax,V0Mmin,V0Mmax,Sys), TList::Class(), AliAnalysisManager::kOutputContainer, Form("%s:%s", AliAnalysisManager::GetCommonFileName(),taskname)));
    //mgr->ConnectOutput(taskKno,1,mgr->CreateContainer(kName.Data(), TList::Class(), AliAnalysisManager::kOutputContainer, Form("%s:%s", AliAnalysisManager::GetCommonFileName(),taskname)));

    return taskKno;
}
