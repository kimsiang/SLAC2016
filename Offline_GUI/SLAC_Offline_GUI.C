// Mainframe macro generated from application: /Applications/root_v6.06.02/bin/root.exe
// By ROOT version 6.06/02 on 2016-05-12 14:08:31

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGColorDialog
#include "TGColorDialog.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGHtml
#include "TGHtml.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGView
#include "TGView.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGTextView
#include "TGTextView.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGuiBldGeometryFrame
#include "TGuiBldGeometryFrame.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include "Riostream.h"

void SLAC_Offline_GUI()
{
    // Initialize values
//  Char_t  *fHistoName = "Island";
   Int_t   fRunNumber = 100;
   Int_t   fEventNumber = 0;
   Int_t   fCrystalNumber = 30;

   // main frame
   TGMainFrame *fMainFrame893 = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMainFrame893->SetName("fMainFrame893");
   fMainFrame893->SetLayoutBroken(kTRUE);
   fMainFrame893->SetWindowName("Muon g-2 SLAC Offline GUI");

   TGFont *ufont;         // will reflect user font changes
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-24-*-*-*-*-*-*-*");

   TGGC   *uGC;           // will reflect user GC changes
   // graphics context changes
   GCValues_t vall4499;
   vall4499.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall4499.fForeground);
   gClient->GetColorByName("#c6c3c6",vall4499.fBackground);
   vall4499.fFillStyle = kFillSolid;
   vall4499.fFont = ufont->GetFontHandle();
   vall4499.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall4499, kTRUE);

   ULong_t ucolor;        // will reflect user color changes
   gClient->GetColorByName("#ffffff",ucolor);
   TGLabel *fLabel4499 = new TGLabel(fMainFrame893,"SLAC 2016 Analysis GUI",uGC->GetGC(),ufont->GetFontStruct(),kRaisedFrame,ucolor);
   fLabel4499->SetTextJustify(36);
   fLabel4499->SetMargins(0,0,0,0);
   fLabel4499->SetWrapLength(-1);
   fMainFrame893->AddFrame(fLabel4499, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel4499->MoveResize(160,8,360,40);

   gClient->GetColorByName("#ffffce",ucolor);

   //-------------------------------------------------------
   // 1st vertical frame
   //
   TGVerticalFrame *RootFile = new TGVerticalFrame(fMainFrame893,208,528,kVerticalFrame | kRaisedFrame,ucolor);
   RootFile->SetName("RootFile");
   RootFile->SetLayoutBroken(kTRUE);

   //ufont = gClient->GetFont("-*-arial black-medium-r-*-*-0-*-*-*-*-*-*-*");
   ufont = gClient->GetFont("-*-helvetica-bold-r-*-*-20-*-*-*-*-*-*-*");

   // graphics context changes
   GCValues_t vall1822;
   vall1822.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall1822.fForeground);
   gClient->GetColorByName("#c6c3c6",vall1822.fBackground);
   vall1822.fFillStyle = kFillSolid;
   vall1822.fFont = ufont->GetFontHandle();
   vall1822.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall1822, kTRUE);

   gClient->GetColorByName("#ffffff",ucolor);
   TGLabel *fLabel1822 = new TGLabel(RootFile,"ROOT File",uGC->GetGC(),ufont->GetFontStruct(),kChildFrame,ucolor);
   fLabel1822->SetTextJustify(36);
   fLabel1822->SetMargins(0,0,0,0);
   fLabel1822->SetWrapLength(-1);
   RootFile->AddFrame(fLabel1822, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel1822->MoveResize(24,8,160,32);

   fNumberEntry100 = new TGNumberEntry(RootFile, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
   fNumberEntry100->SetName("fNumberEntry100");
   RootFile->AddFrame(fNumberEntry100, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry100->MoveResize(16,96,59,22);
   fNumberEntry100->SetNumber(fRunNumber);

   fNumberEntry101 = new TGNumberEntry(RootFile, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
   fNumberEntry101->SetName("fNumberEntry101");
   RootFile->AddFrame(fNumberEntry101, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry101->MoveResize(16,128,59,22);
   fNumberEntry101->SetNumber(fEventNumber);

   fNumberEntry102 = new TGNumberEntry(RootFile, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
   fNumberEntry102->SetName("fNumberEntry102");
   RootFile->AddFrame(fNumberEntry102, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fNumberEntry102->MoveResize(16,160,59,22);
   fNumberEntry102->SetNumber(fCrystalNumber);

   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-*-*");

/*
   GCValues_t valEntry102;
   valEntry102.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry102.fForeground);
   gClient->GetColorByName("#c6c3c6",valEntry102.fBackground);
   valEntry102.fFillStyle = kFillSolid;
   valEntry102.fFont = ufont->GetFontHandle();
   valEntry102.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry102, kTRUE);
   fTextEntry102 = new TGTextEntry(RootFile, new   TGTextBuffer(15),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kDoubleBorder | kOwnBackground);
   fTextEntry102->SetMaxLength(4096);
   fTextEntry102->SetAlignment(kTextLeft);
   fTextEntry102->SetText("hTOF00");
   fTextEntry102->Resize(93,fTextEntry102->GetDefaultHeight());
   RootFile->AddFrame(fTextEntry102, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextEntry102->MoveResize(16,160,60,22);
 */
   // graphics context changes
   GCValues_t vall100;
   vall100.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",vall100.fForeground);
   gClient->GetColorByName("#c6c3c6",vall100.fBackground);
   vall100.fFillStyle = kFillSolid;
   vall100.fFont = ufont->GetFontHandle();
   vall100.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall100, kTRUE);
   
   TGLabel *fLabel100 = new TGLabel(RootFile," Run Number",uGC->GetGC(),ufont->GetFontStruct());
   fLabel100->SetTextJustify(33);
   fLabel100->SetMargins(0,0,0,0);
   fLabel100->SetWrapLength(-1);
   RootFile->AddFrame(fLabel100, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel100->MoveResize(88,96,96,18);

   TGLabel *fLabel101 = new TGLabel(RootFile," Event Number",uGC->GetGC(),ufont->GetFontStruct());
   fLabel101->SetTextJustify(33);
   fLabel101->SetMargins(0,0,0,0);
   fLabel101->SetWrapLength(-1);
   RootFile->AddFrame(fLabel100, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel101->MoveResize(88,128,96,18);
   
   TGLabel *fLabel102 = new TGLabel(RootFile," Crystal Number",uGC->GetGC(),ufont->GetFontStruct());
   fLabel102->SetTextJustify(33);
   fLabel102->SetMargins(0,0,0,0);
   fLabel102->SetWrapLength(-1);
   RootFile->AddFrame(fLabel102, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel102->MoveResize(88,160,96,18);

   TGTextButton *fTextButton103 = new TGTextButton(RootFile,"Plot Island");
//  fTextButton103->Connect("Clicked()", 0, 0, "PlotTitle()");
   fTextButton103->SetTextJustify(33);
   fTextButton103->SetMargins(0,0,0,0);
   fTextButton103->SetWrapLength(-1);
   fTextButton103->Resize(99,24);
   RootFile->AddFrame(fTextButton103, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton103->MoveResize(88,192,96,18);

   TGTextButton *fTextButton104 = new TGTextButton(RootFile,"Plot Fit Result");
//  fTextButton104->Connect("Clicked()", 0, 0, "PlotBeamSpot()");
   fTextButton104->SetTextJustify(33);
   fTextButton104->SetMargins(0,0,0,0);
   fTextButton104->SetWrapLength(-1);
   fTextButton104->Resize(99,24);
   RootFile->AddFrame(fTextButton104, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton104->MoveResize(88,224,96,18);

/*  TGTextButton *fTextButton105 = new TGTextButton(RootFile,"Plot ");
  fTextButton105->Connect("Clicked()", 0, 0, "PlotTimeOfFlight()");
   fTextButton105->SetTextJustify(33);
   fTextButton105->SetMargins(0,0,0,0);
   fTextButton105->SetWrapLength(-1);
   fTextButton105->Resize(119,24);
   RootFile->AddFrame(fTextButton105, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton105->MoveResize(68,256,116,18);

*/
   fMainFrame893->AddFrame(RootFile, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   RootFile->MoveResize(8,56,208,528);

   gClient->GetColorByName("#ffffce",ucolor);
   //
   // 1st vertical frame end
   // ------------------------------------------------------------------
 


   fMainFrame893->SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputModeless);
   fMainFrame893->MapSubwindows();

   fMainFrame893->Resize(fMainFrame893->GetDefaultSize());
   fMainFrame893->MapWindow();
   fMainFrame893->Resize(703,512);
}  
