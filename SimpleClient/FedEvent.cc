
#include "unpacker/FedEvent.hh"
#include <stdlib.h>		// for NULL
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <vector>
#include <inttypes.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <iterator>
    

namespace FedAMC13{

  FedEvent::FedEvent(){
    dataPointer = NULL;
    dataSize = 0;
    dataVector.clear();
    block.clear();
    for(int iAMC=0;iAMC<12;iAMC++){
      AMC[iAMC].Clear();
    }
    eventSize = 0;
  }




  void FedEvent::Clear(){
    dataSize = 0;
    dataVector.clear();
    block.clear();
    for(int iAMC=0;iAMC<12;iAMC++){
      AMC[iAMC].Clear();
    }
    eventSize = 0;
  }





  uint64_t FedEvent::GetMaskedField(uint64_t word, uint32_t bit, uint32_t mask){
    if(dataPointer != NULL || dataSize != 0){
      if(word < dataSize){
	return ((dataPointer[word]>>bit) & mask);
      }
      FedException::OutOfRange e;
      e.Append("word input out of range in GetMaskedField");
      throw e;
    }
    FedException::NULLPointer e;
    e.Append("dataPointer is NULL calling GetMaskedField");
    throw e;
  }

  void FedEvent::FedKit2Binary(){
    printf("Not Implemented Yet\n");
    exit(1);
  }

  
  bool FedEvent::PreParse(std::ifstream & fileStream){
    FedException::DataFormatError e1;
    FedException::FileReadError e3;
    uint64_t tmpEventSize=0;
    uint64_t FedKitHeader;
    
    //Read first two words from file.
    dataVector.resize(HEADER_WORDS);
    fileStream.read( (char*)dataVector.data(), HEADER_WORDS*sizeof(uint64_t));
    if(fileStream.eof()){
      printf("End of file\n");
      return true;
    }
    if(fileStream.fail()){
      e3.Append("{PreParse} Error reading first two words from event.");
      throw e3;
    }

    //Check the file format
    if(dataVector[0] == 0xbadc0ffeebadcafe){
      dataFileType = "BadCoffee"; //Bad coffee format
      tmpEventSize = dataVector[1];
      fileStream.read( (char*)dataVector.data(), HEADER_WORDS*sizeof(uint64_t));
      if(fileStream.fail()){
	e3.Append("{PreParse} Error reading header words from event.");
	throw e3;
      }
    }else if( ((dataVector[BOE_WORD] >> BOE_BIT) & BOE_MASK) == MAGIC_WORD){
      dataFileType = "Binary"; //Binary format
    }else{
      //Might be a FedKit which will have the raw format starting at the 4th word
      //Read 1 more lines and overwrite dataVector.
      fileStream.read( (char*)dataVector.data(), sizeof(uint64_t));
      if(fileStream.fail()){
        e3.Append("{PreParse} Error reading first two words from event.");
        throw e3;
      }
      //Save the header value
      FedKitHeader = dataVector[0];
      //Now the next reads should be binary
      fileStream.read( (char*)dataVector.data(), HEADER_WORDS*sizeof(uint64_t));
      if(fileStream.fail()){
        e3.Append("{PreParse} Error reading first two words from event.");
        throw e3;
      }
      if( ((dataVector[BOE_WORD] >> BOE_BIT) & BOE_MASK) == MAGIC_WORD) {
        dataFileType = "FedKit";
      }else{
	char tmp[80];
	e1.Append("File format not recognized\n");
	sprintf( tmp, "First words: %016lx %016lx\n", dataVector[0], dataVector[1]);
	e1.Append( tmp);
	throw e1;    
      }
    }

    //Check header values
    size_t NAMC = (dataVector[NAMC_WORD]>>NAMC_BIT) & NAMC_MASK;
    if(((dataVector[BOE_WORD]>>BOE_BIT) & BOE_MASK) == MAGIC_WORD){
      if(NAMC > 0 && NAMC < 13){
	dataVector.resize(HEADER_WORDS + NAMC);
	fileStream.read( (char*)(dataVector.data() + HEADER_WORDS), NAMC*sizeof(uint64_t));
	if(fileStream.fail()){
	  e3.Append("{PreParse} Error reading header words from event.");
	  throw e3;
	}
      }else{
	e1.Append("Bad AMC count in header\n");
	throw e1;
      }
    }else{
      e1.Append("Magic word in header not equal to 5\n");
      throw e1;
    }
  
    //Calculate the size of the event
    size_t AMCSize = 0;
    int nblock[N_AMC];
    int totalSize = 0;
    int tblk = 0;
    int nblock_max = 0;

    for( size_t iAMC=0; iAMC<NAMC; iAMC++) {
      AMCSize = (dataVector[iAMC+2]>>AMCSIZE_BIT) & AMCSIZE_MASK;
      totalSize += AMCSize;
      // calculate block size
      if( AMCSize <= 0x13fe)
	nblock[iAMC] = 1;
      else
	nblock[iAMC] = (AMCSize-1023)/4096+1; // Wu's formula, fixed
      tblk += nblock[iAMC];
      if( nblock[iAMC] > nblock_max)	// keep track of AMC with the most blocks
	nblock_max = nblock[iAMC];
    }
    //Calculate number of words to read for full event.
    uint64_t lastReadSize = totalSize + tblk + nblock_max*2 - NAMC;
    uint64_t tmpCalcEventSize = lastReadSize + NAMC + HEADER_WORDS;

    //Check size calculation if BADCOFFEE format
    if(dataFileType == "BadCoffee"){
      if(tmpEventSize != tmpCalcEventSize){
	printf("BADCOFFEE = 0x%lx  Formula = 0x%lx\n", tmpEventSize, tmpCalcEventSize);
	e1.Append("BADCOFFEE size does not match Mr. Wu's formula size\n");
	throw e1;
      }
    }

    dataVector.resize( tmpCalcEventSize );
    fileStream.read( (char*)(dataVector.data() + HEADER_WORDS + NAMC), lastReadSize*sizeof(uint64_t));
    if(fileStream.fail()){
      e3.Append("{PreParse} Error reading full event.");
      throw e3;
    }

    //Check the event trailer values for format errors (magic word, event length)
    if( ((dataVector[tmpCalcEventSize-1] >> BOE_BIT) & BOE_MASK) != MAGIC_WORD_TRAILER){
      char tmp[80];
      e1.Append("Magic word in trailer is not 0xA\n");
      int posn = fileStream.tellg();
      sprintf( tmp, "trailer word: %016lx at offset 0x%x\n", dataVector[tmpCalcEventSize-1], posn );
      e1.Append( tmp);
      throw e1;
    }
    if( ((dataVector[tmpCalcEventSize-1] >> TRAILERSIZE_BIT) & TRAILERSIZE_MASK) != (tmpCalcEventSize & 0xffffff) ){
      e1.Append("Event size in trailer does not match calculated/badcoffee event length\n");
      throw e1;
    }

    
    //Set internal pointer and size to be used by ParseEvent().
    dataPointer = dataVector.data();
    dataSize = dataVector.size();
    return false;
  }






  bool FedEvent::PreParse(FILE * fp){
    FedException::DataFormatError e1;
    FedException::FileReadError e3;
    uint64_t tmpEventSize=0;
    uint64_t FedKitHeader;

    //Read the first two words.
    dataVector.resize(HEADER_WORDS);
    if(fread( dataVector.data(), sizeof(uint64_t), HEADER_WORDS, fp) != HEADER_WORDS){
      printf("End of File\n");
      return true;
    }

    //Check the format of the first two words.
    if(dataVector[0] == 0xbadc0ffeebadcafe){
      dataFileType = "BadCoffee"; //bad coffee format
      tmpEventSize = dataVector[1];  //Save the size for later
      //File format known, overwrite first two words and read the Header
      if(fread( dataVector.data(), sizeof(uint64_t), HEADER_WORDS, fp) != HEADER_WORDS){
	e3.Append("{PreParse} fread failed reading header of badcoffee format file");
	throw e3;
      }
    }else if(((dataVector[BOE_WORD] >> BOE_BIT) & BOE_MASK) == MAGIC_WORD){
      dataFileType = "Binary"; //Binary file. Words read are the Header
    }else{
      //Might be a FedKit which will have the raw format starting at the 4th word
      //Read 1 more lines and overwrite dataVector.
      if(fread( dataVector.data(), sizeof(uint64_t), 1, fp) != 1){
        e3.Append("{PreParse} fread failed reading header of possible fedkit format");
        throw e3;
      }
      FedKitHeader = dataVector[0];
      //Now the next reads should be binary
      if(fread( dataVector.data(), sizeof(uint64_t), HEADER_WORDS, fp) != HEADER_WORDS){
        e3.Append("{PreParse} fread failed reading header of possible fedkit format");
        throw e3;
      }
      if( ((dataVector[BOE_WORD] >> BOE_BIT) & BOE_MASK) == MAGIC_WORD) {
        dataFileType = "FedKit";
      }else{
	char tmp[80];
	e1.Append("File format not recognized\n");
	sprintf( tmp, "First words: %016lx %016lx\n", dataVector[0], dataVector[1]);
	e1.Append( tmp);
	throw e1;    
      }
    }

    //Check header values
    size_t NAMC = (dataVector[NAMC_WORD]>>NAMC_BIT) & NAMC_MASK;
    if(((dataVector[BOE_WORD]>>BOE_BIT) & BOE_MASK) == MAGIC_WORD){
      //check number of amcs
      if(NAMC > 0 && NAMC < 13){
	dataVector.resize(HEADER_WORDS + NAMC);
	if(fread(dataVector.data()+HEADER_WORDS, sizeof(uint64_t), NAMC, fp) != NAMC){
	  e3.Append("{PreParse} fread failed reading AMC headers");
	  throw e3;
	}
      }else{
	e1.Append("Bad AMC count in header\n");
	throw e1;
      }
    }else{
      e1.Append("Magic word in header not equal to 5\n");
      throw e1;
    }

    // calculate the size of the event and read the entire event from the file.
    // read AMC headers
    int AMCSize = 0;
    int nblock[N_AMC];
    int totalSize = 0;
    int tblk = 0;
    int nblock_max = 0;
    
    for( size_t iAMC=0; iAMC<NAMC; iAMC++) {
      AMCSize = (dataVector[iAMC+2]>>AMCSIZE_BIT) & AMCSIZE_MASK;
      totalSize += AMCSize;
      // calculate block size
      if( AMCSize <= 0x13fe)
	nblock[iAMC] = 1;
      else
	nblock[iAMC] = (AMCSize-1023)/4096+1; // Wu's formula, fixed
      tblk += nblock[iAMC];
      if( nblock[iAMC] > nblock_max)	// keep track of AMC with the most blocks
	nblock_max = nblock[iAMC];
    }
    // calculate total event size Wu's way
    size_t lastReadSize = totalSize + tblk + nblock_max*2 - NAMC;
    size_t tmpCalcEventSize = lastReadSize + NAMC + HEADER_WORDS;    

    //Check size calculation if BADCOFFEE format
    if(dataFileType == "BadCoffee"){
      if(tmpEventSize != tmpCalcEventSize){
	printf("BADCOFFEE = %lx  Formula = %lx\n", tmpEventSize, tmpCalcEventSize);
	e1.Append("BADCOFFEE size does not match Mr. Wu's formula size\n");
	throw e1;
      }
    }

    //read the rest of the event
    dataVector.resize(tmpCalcEventSize);
    if(fread(dataVector.data() + GetNAMC() + HEADER_WORDS, sizeof(uint64_t), lastReadSize, fp) != lastReadSize){
      e3.Append("{PreParse} fread failed reading entire event");
      throw e3;
    }

    //Check the event trailer values for format errors (magic word, event length)
    if( ((dataVector[tmpCalcEventSize-1] >> BOE_BIT) & BOE_MASK) != MAGIC_WORD_TRAILER){
      char tmp[80];
      e1.Append("Magic word in trailer is not 0xA\n");
      sprintf( tmp, "trailer word: %016lx\n", dataVector[tmpCalcEventSize-1]);
      throw e1;
    }
    if( ((dataVector[tmpCalcEventSize-1] >> TRAILERSIZE_BIT) & TRAILERSIZE_MASK) != (tmpCalcEventSize & 0xffffff) ){
      e1.Append("Event size in trailer does not match calculated/badcoffee event length\n");
      throw e1;
    }

    //Set internal pointer and size to be used by ParseEvent().
    dataPointer = dataVector.data();
    dataSize = dataVector.size();
    return false;
  }






  void FedEvent::ParseEvent(uint64_t * inputPointer, uint64_t inputSize){
    dataPointer = inputPointer;
    dataSize = inputSize;
    ParseEvent();
  }

  void FedEvent::ParseEvent(){
    if(dataPointer == NULL || dataSize == 0){
      FedException::NULLPointer e;
      e.Append("dataPointer is NULL or dataSize is 0 calling ParseEvent");
      throw e;
    }

    uint64_t * curPtr = dataPointer;
    uint64_t size = dataSize;

    //Check magic word in event header
    if(GetBOE() != MAGIC_WORD){ 
      FedException::BadHeaderMagicWord e;
      throw e;
    }

    //Move through the Event header:
    curPtr+=1;

    //Create FedBlock objects
    try{
      FedAMC13::FedBlock newBlock;
      newBlock.Clear();
      uint64_t sizeLeft = size - 2;
      newBlock.PreParse(curPtr,size);
      curPtr=newBlock.Parse();
      block.push_back(newBlock);
    
      //Check the size of the event:
      eventSize = GetBlock(0).GetEventSize();
      if(eventSize <= size){
	//Set the size left for the blocks to be the full event size minus event header and trailer:
	sizeLeft = eventSize-GetBlock(0).GetSize();  //Full size minus 2 for Event header and trailer
      }else{
	FedException::DataFormatError e;
	e.Append("Event size calculation does not agree with input size");
	throw e;
      }


      //Create FedBlock objects until ParseBlock gets to the end of the event (or size left goes to 0):
      while (curPtr !=NULL){
	newBlock.PreParse(curPtr,size);
	curPtr = newBlock.Parse(); //ParseBlock reads the block and returns pointer to the next block 
	sizeLeft -= newBlock.GetSize();
	block.push_back(newBlock);
      }
      
    }catch(FedException::exBase & e){
      e.Append("Block parsing went badly \n");
      throw;
    }
    
    
    try{
      //Create AMC array full of Vectors that point to AMC data in each block along with the AMC data size
      for(int iAMC=1;iAMC<=N_AMC;iAMC++){
	AMC[iAMC-1].Clear();   //AMC is array so 0-based, minus 1 because iAMC is 1-based
	for(uint32_t iBlock=0;iBlock<GetnBlocks();iBlock++){
	  if(GetBlock(iBlock).GetAMCInfo(iAMC) != NULL){
	    AMC[iAMC-1].AddPayload(GetBlock(iBlock).GetAMCPtr(iAMC),GetBlock(iBlock).BlockAMCSize(iAMC));
	  }
	}
      }
    }catch(FedException::exBase & e){
      e.Append("Something went wrong creating FedAMC objects");
      throw;
    }
  }
  
  // calculate CMS CRC
  uint16_t FedEvent::CalcCRC() {
    if(dataPointer !=NULL){
      uint32_t *m_raw32;
      m_raw32 = (uint32_t*)dataPointer;
      uint16_t crc = 0xFFFF;
      uint32_t my_size = dataSize * 2;
      
      for (unsigned int i=0; i < my_size; i+=2) {
	if(i == my_size -2)
	  crc = crc16d64_(m_raw32[i+1], m_raw32[i]&0x0FF8, crc); //Blank out bits 12-15(FEROL bits) and bits 1-3(S-Link)
	else
	  crc = crc16d64_(m_raw32[i+1], m_raw32[i], crc);
      }
      
      return crc;
    }
    FedException::NULLPointer e;
    e.Append("data64 is NULL calling CalcCRC");
    throw e;   
  }

  FedBlock & FedEvent::GetBlock(uint32_t iBlock){
    if(iBlock >= block.size()){
      fprintf(stderr,"Error, getBlock input out of range \n");
    }
    return block[iBlock];
  }

  size_t FedEvent::GetnBlocks(){
    return block.size();
  }

  //Gets the vector of pointers and sizes of the iAMC data
  FedAMC & FedEvent::GetAMC(int iAMC){
    if(iAMC < 13 && iAMC > 0)
      return AMC[iAMC-1];
    FedException::OutOfRange e;
    e.Append("AMC number of out range calling GetAMC");
    throw e;
  }

  void FedEvent::Dump(int dumpLevel,bool CRCLevel,int errorLevel){

    if(CRCLevel){
      // check CRC
      uint16_t evt_crc = CRC();
      uint16_t calc_crc = CalcCRC();
	
      if( evt_crc != calc_crc) { // CRC is bad
	char tmp[80];
	snprintf( tmp, 80, "BAD Event CRC: %04x  Calculated CRC: %04x\n", CRC(), CalcCRC());
	FedException::BadCRC e;
	e.Append( tmp);
	throw e;
      } else {		   // CRC is OK
	if( dumpLevel > 1)
	  printf("Event CRC: %04x  Calculated CRC: %04x  %s\n", CRC(), CalcCRC(),
		 (evt_crc != calc_crc) ? "ERROR" : "OK");
      }
    }


    //Check for the dump level input (dumpLevel)
    if(dumpLevel > 0){
      //first level here
      //EvN 000001 BcN 123 ID abc Evt_ty=3  [3 blocks, 0x3812 words]
    
      printf("EvN %06lx (%06ld) BcN %03lx ID %04lx Evt_ty=%ld",GetEvN(), GetEvN(), GetBcN(), GetBXID(), GetEventType()); //Done
      printf("%s","   ");
      printf("[%ld blocks, 0x%lx words] \n",GetnBlocks(), GetEventSize());  //Done
     
      if( dumpLevel > 3) {
	printf("\n");
	printf("First 20 words:\n");
	for(int i=0;i<=20;i++){
	  printf("%04d 0x%016lx \n",i,dataPointer[i]);
	}
	printf("\n");
      }

      if(dumpLevel > 1){
	//second level
	//  Blk uFOV=1 nAMC=3 OrN=12345678
	for(uint32_t iBlock=0;iBlock<GetnBlocks();iBlock++){
	  printf("   Blk %02d uFOV=%d nAMC=%02d OrN=%08x \n",
		 iBlock,GetBlock(iBlock).GetuFOV() ,
		 GetBlock(iBlock).GetNAMC() ,
		 GetBlock(iBlock).GetOrN());

	  if(dumpLevel > 2){
	    //third level
	    //      LMSEPVC  size  Blk  BoardID
	    //AMC1 1001110 001800 00   abcd
	    //AMC2 1001110 001800 00   dead
	    //AMC3 0001110 000800 00   beef

	    printf("               LMSEPVC         size   Blk  BoardID \n");
	    for(int iAMC=1;iAMC<=N_AMC;iAMC++){
	      if(GetBlock(iBlock).GetAMCInfo(iAMC)!=NULL){
		
		int v = GetBlock(iBlock).GetLMSEPVC(iAMC);
	      
		int siz = 7; //Size of LMSEPVC bits
		static char s[7];
		for( int i=0; i<siz; i++){
		  s[i] = (v >> (siz-i-1)) & 1 ? '1' : '0';
		}
		
		printf("       AMC%02d   ",AMC[iAMC-1].AMCNo());
		for(int i=0;i<siz;i++){
		  printf("%c",s[i]);
		}
	       
		printf("   0x%08x    %02d   %04x \n",
		       GetBlock(iBlock).GetAMCSize(iAMC),
		       iBlock,
		       GetBlock(iBlock).GetID(iAMC));
	      
		if(dumpLevel > 4){
		  //fourth and last level
		  //Dump AMC data
		  for(size_t i=0;i<10;i++){
		    if(AMC[iAMC-1].GetPayloadCopy().size() > i)
		      printf("           %02ld 0x%016lx\n",i,AMC[iAMC-1].GetPayloadCopy()[i]);
		  }
		}
	      }
	    }
	    printf("\n");
	  }
	}
      }
    }
    if(errorLevel){
      if(AMC13Errors[0][0][0] > 0){
	printf("Event: %06lx Errors: %d \n",GetEvN(), AMC13Errors[0][0][0]);
	//Check the Event and Block errors
	if( AMC13Errors[0][1][0] > 0)
	  printf("FedEvent::ErrorChecking(): EvN mismatch. Event: %08x  Block: %08x \n", AMC13Errors[0][1][1],AMC13Errors[0][1][2]);
	if( AMC13Errors[0][2][0] > 0)
	  printf("FedEvent::ErrorChecking(): EvN mismatch. Event: %08x  Block: %08x \n", AMC13Errors[0][2][1],AMC13Errors[0][2][2]);
	if( AMC13Errors[0][3][0] > 0)
	  printf("FedEvent::ErrorChecking(): EvN mismatch. Event: %08x  Block: %08x \n", AMC13Errors[0][3][1],AMC13Errors[0][3][2]);
	if( AMC13Errors[0][4][0] > 0)
	  printf("FedEvent::ErrorChecking(): EvN mismatch. Event: %08x  Block: %08x \n", AMC13Errors[0][4][1],AMC13Errors[0][4][2]);
	//Check the AMC errors. This includes the errors in the block header corresponding to an AMC. 
	for(uint32_t iAMC=1;iAMC<=N_AMC;iAMC++){
	  if(AMC13Errors[iAMC][1][0] > 0){
	    printf("FedEvent::ErrorChecking(): BlkN mismatch in AMC %d. AMC13: %08x  AMC: %08x \n", iAMC, AMC13Errors[iAMC][1][1],AMC13Errors[iAMC][1][2]);
	  }
	  if(AMC13Errors[iAMC][3][0] > 0){
	    printf("FedEvent::ErrorChecking(): EvN mismatch in AMC %d. AMC13: %08x  AMC: %08x \n", iAMC,AMC13Errors[iAMC][3][1],AMC13Errors[iAMC][3][2]);
	  }
	  if(AMC13Errors[iAMC][4][0] > 0){
	    printf("FedEvent::ErrorChecking(): BcN mismatch in AMC %d. AMC13: %08x  AMC: %08x \n", iAMC,AMC13Errors[iAMC][4][1],AMC13Errors[iAMC][4][2]);
	  }
	  if(AMC13Errors[iAMC][5][0] > 0){
	    printf("FedEvent::ErrorChecking(): OrN mismatch in AMC %d. AMC13: %08x  AMC: %08x \n", iAMC,AMC13Errors[iAMC][5][1],AMC13Errors[iAMC][5][2]);
	  }
	  if(AMC13Errors[iAMC][7][0] > 0){
	    printf("FedEvent::ErrorChecking(): EvN_TR mismatch in AMC %d. AMC13: %08x  AMC: %08x \n", iAMC,AMC13Errors[iAMC][7][1],AMC13Errors[iAMC][7][2]);
	  }
	}
      }
    }
  }

  void FedEvent::ErrorChecking(){

    //Set everything to zero for each event. Cummulate errors in parse.cc or dump.cc to then be analyzed.     
    for(uint32_t iAMC = 0;iAMC < 13;iAMC++){
      for(uint32_t ierror = 0;ierror < 10;ierror++){
	for(uint32_t ivalue = 0;ivalue < 3;ivalue++){
	  AMC13Errors[iAMC][ierror][ivalue]=0;
	}
      }
    }

    //The following builds and array of errors. The dimensions are [iAMC][error type][value]. iAMC has values 0 (Event vs Block) and 1-12 for each AMC. error type 
    //describes the type of error (a table will be available). value=0 indicates if an error occured or not, value=1,2 are the values being checked. 
    for(uint32_t iBlock = 0;iBlock<GetnBlocks();iBlock++){
      if(GetBlock(0).GetuFOV() != GetBlock(iBlock).GetuFOV()){
	AMC13Errors[0][1][0]+=1;
	AMC13Errors[0][1][1]=GetBlock(0).GetuFOV();
	AMC13Errors[0][1][2]=GetBlock(iBlock).GetuFOV();
      }
      if(GetBlock(0).GetOrN() != GetBlock(iBlock).GetOrN()){
	AMC13Errors[0][2][0]+=1;
	AMC13Errors[0][2][1]=GetBlock(0).GetOrN();
	AMC13Errors[0][2][2]=GetBlock(iBlock).GetOrN();
      }
      if((GetEvN() & 0xff) != GetBlock(iBlock).GetEvNTR()){
	AMC13Errors[0][3][0]+=1;
	AMC13Errors[0][3][1]=(GetEvN() & 0xff);
	AMC13Errors[0][3][2]=GetBlock(iBlock).GetEvNTR();
      }
      if(GetBcN() != GetBlock(iBlock).GetBcNTR()){
	AMC13Errors[0][4][0]+=1;
	AMC13Errors[0][4][1]=GetBcN();
	AMC13Errors[0][4][2]=GetBlock(iBlock).GetBcNTR();	
      }
      
      for(int iAMC=1;iAMC<=N_AMC;iAMC++){
	if(GetBlock(iBlock).GetAMCInfo(iAMC) != NULL){
	  if(GetBlock(iBlock).GetBlockNoTR() != GetBlock(iBlock).GetBlockNo(iAMC)){
	    AMC13Errors[iAMC][1][0]+=1;
	    AMC13Errors[iAMC][1][1]=GetBlock(iBlock).GetBlockNoTR();
	    AMC13Errors[iAMC][1][2]=GetBlock(iBlock).GetBlockNo(iAMC);
	  }
	  //Need a function call for the BoardIDs
	  //AMC13Errors[iAMC][2][0]+=1;
	  if(GetEvN() != AMC[iAMC-1].EvN()){
	    AMC13Errors[iAMC][3][0]+=1;
	    AMC13Errors[iAMC][3][1]=GetEvN();
	    AMC13Errors[iAMC][3][2]=AMC[iAMC-1].EvN(); 
	  }
	  if(GetBcN() != AMC[iAMC-1].BcN()){
	    AMC13Errors[iAMC][4][0]+=1; 
	    AMC13Errors[iAMC][4][1]=GetBcN();
	    AMC13Errors[iAMC][4][2]=AMC[iAMC-1].BcN();
	  }
	  if((GetBlock(0).GetOrN()&0xffff) != AMC[iAMC-1].OrN()){
	    AMC13Errors[iAMC][5][0]+=1;
	    AMC13Errors[iAMC][5][1]=(GetBlock(0).GetOrN()&0xffff);
	    AMC13Errors[iAMC][5][2]= AMC[iAMC-1].OrN();
	  }
	  //if(GetBlock(iBlock).GetID(1) != GetBlock(iBlock).GetID(iAMC))   //Need to fix this so AMC 1 is not called, but instead the first amc active
	  //   AMC13Errors[iAMC][6][0]+=1;
	  if((GetEvN()&0xff) != AMC[iAMC-1].EvN_TR()){
	    AMC13Errors[iAMC][7][0]+=1;
	    AMC13Errors[iAMC][7][1]=(GetEvN()&0xff);
	    AMC13Errors[iAMC][7][2]=AMC[iAMC-1].EvN_TR();
	  }
	  //if((GetEvN()&0xff) != AMC[iAMC-1].DataLength_TR())
	  //   AMC13Errors[iAMC][8][0]+=1;
	}
      }
    }
    
    for(uint32_t iAMC = 0;iAMC < 13;iAMC++){
      for(uint32_t ierror = 1;ierror < 10;ierror++){
	AMC13Errors[0][0][0] += AMC13Errors[iAMC][ierror][0];
	// printf("%d",AMC13Errors[iAMC][ierror][0]);
      }
      //printf("\n");
    }
  }
  uint32_t FedEvent::GetAMC13Errors(uint32_t AMC, uint32_t error, uint32_t value){
    return AMC13Errors[AMC][error][value];
  }

}
