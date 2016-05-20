
//Definitions of class functions for FedBlock
#include "unpacker/FedBlock.hh"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <vector>
#define N_AMC 12

namespace FedAMC13{

  FedBlock::FedBlock(){
  }

  void FedBlock::Clear(){
    blockPointer = NULL;
    sizeLeft=0;
  }

  void FedBlock::PreParse(uint64_t* blockPointerInput, uint64_t sizeInput){
    if((blockPointerInput == NULL) || (sizeInput == 0)){
      FedException::NULLPointer e;
      e.Append("Input is NULL to function FedBlock::Parse \n");
      throw e;
    }

    //Check if the uFOV bits are good:
    if( ((blockPointerInput[UFOV_WORD]>>UFOV_BIT) & UFOV_MASK) != 1 ){
      FedException::DataFormatError e;
      e.Append("uFOV is not equal to 1 {FedBlock::PreParse} \n");
      throw e;
    }
    
    //Set the internal pointer/size once everything looks good.
    blockPointer = blockPointerInput;
    sizeLeft = sizeInput;
  }
  
  
  uint64_t* FedBlock::Parse(){

    for(int i=0;i<N_AMC;i++){
      AMCInfo[i] = NULL;
    }
    
    //Set up AMCInfo to be an array of pointers to the active AMC numbers.
    //Example: AMC 1, 3, 5 are active, the rest are not. AMCInfo is NULL for 
    //AMCInfo[i] where i=2,4,6...12. For i=1,3,5 AMCInfo is a pointer to the 
    //Block header info on those AMC numbers. AMCInfo is 0 based. The call to 
    //AMCInfo {GetAMCInfo()} is 1 based. 

    for(uint32_t i=1;i<=GetNAMC();i++){
      int iAMC = (blockPointer[i] >> AMCNO_BIT) & AMCNO_MASK;
      AMCInfo[iAMC-1] = &blockPointer[i];
    }

    //Check to see if all AMCs show they are the last block in the event
    uint32_t segmentedEndCheck=0;
    uint32_t singleBlockEndCheck=0;
    for(int iAMC=1;iAMC<=N_AMC;iAMC++){
      if(GetAMCInfo(iAMC) != NULL){
	if(GetMS(iAMC) == 1){
	  segmentedEndCheck ++;
	}
	if(GetMS(iAMC) == 0){
	  singleBlockEndCheck ++;
	}
      }
    }
  
    if( (segmentedEndCheck == GetNAMC()) || (singleBlockEndCheck == GetNAMC()) ){
      return NULL;
    }

    if(GetSize()==0){
      return NULL;
    }

    if(sizeLeft<=GetSize()){
      return NULL;
    }
    
    return (blockPointer + GetSize());
  
  }

  uint64_t* FedBlock::GetAMCInfo(int iAMC){
    if(iAMC>0 && iAMC<13){
      return AMCInfo[iAMC-1];
    }
    FedException::ReadBlockHeaderInputError e;
    e.Append("Out of range value {GetAMCInfo(iAMC)} \n");
    throw e;
  }

  //Function to get value in block header (Only header is needed to calculate the block size)
  uint32_t FedBlock::GetMaskedHeader(int64_t iAMCword, int bit, int mask){
    if(blockPointer == NULL){
      FedException::NULLPointer e;
      e.Append("blockPointer is NULL calling function GetMaskedHeader \n");
      throw e;
    }
    
    if(iAMCword == 0){ //For first word in block header
      return ((blockPointer[iAMCword]>>bit) & mask);
    }
    if(iAMCword > 0 && iAMCword < 13){ //For AMC words in block header
      if(GetAMCInfo(iAMCword) != NULL){
	return (GetAMCInfo(iAMCword)[0]>>bit) & mask; 
      }
      FedException::NULLPointer e;
      e.Append("Trying to access NULL AMC {GetMaskedHeader(word,bit,mask)} \n");
      throw e;
    }
    if(iAMCword == -1){
      return((blockPointer[GetSize()-1]>>bit) & mask);
    }
    FedException::NULLPointer e;
    e.Append("Word out of range {GetMaskedHeader(word,bit,mask)} \n");
    throw e;
  }

  uint32_t FedBlock::GetNAMC(){
    size_t nAMC = (blockPointer[BLOCK_NAMC_WORD]>>BLOCK_NAMC_BIT)&BLOCK_NAMC_MASK;
    if(nAMC > 0 && nAMC < 13){
      return nAMC;
    }
    FedException::OutOfRange e;
    e.Append("NAMC out of range \n");
    throw e;
  }

  //Calculates the size of the block. Can be both segmented and different sizes for each AMC.
  uint64_t FedBlock::GetSize(){
    uint64_t totalBlockSize = 0;

    //Add the nAMC headers to the total block size and the header and trailer for the block(+2):
    totalBlockSize += GetNAMC()+2;
    for(int iAMC=1;iAMC<=N_AMC;iAMC++){
      if(GetAMCInfo(iAMC) != NULL){
	//Add the AMC sizes to the total block size
	switch(GetMS(iAMC)){
	case 0:
	case 1:
	case 3:
	  totalBlockSize += GetAMCSize(iAMC);
	  break;
	case 2:
	  totalBlockSize += 0x1000;
	  break;
	default:
	  FedException::OutOfRange e;
	  e.Append("Bad MS value inside {GetSize()} \n");
	  throw e;
	} 
      }
    }
    return totalBlockSize;
  }

  uint64_t* FedBlock::GetAMCPtr(int iAMC){

    uint64_t AMCOffset= GetNAMC()+1;

    for(int i=1;i<iAMC;i++){
      if(GetAMCInfo(i) != NULL){
	switch(GetMS(i)){
	case 0:
	case 1:
	case 3:
	  AMCOffset += GetAMCSize(i);
	  break;
	case 2:
	  AMCOffset += 0x1000;
	  break;
	default:
	  FedException::OutOfRange e;
	  e.Append("Switch case failed {GetAMCPtr(iAMC)} \n");
	  throw e;
	}
      }
    }
    return (blockPointer + AMCOffset);
  }

  uint64_t FedBlock::BlockAMCSize(int iAMC){  
    if(GetAMCSize(iAMC)>0x1000){
      if(GetMS(iAMC)!=0 && GetMS(iAMC) !=1){
	return 0x1000;
      }
    }
    return GetAMCSize(iAMC);
  }

  uint64_t FedBlock::GetEventSize(){
    int nBlocks[N_AMC];
    int totalBlocks = 0;
    int nBlocksMax = 0;
    int totalSize = 0;
    int nWords = 0;

    memset(nBlocks,0,sizeof(nBlocks));

    for(int i=0; i<N_AMC;i++){
      if(GetAMCInfo(i+1) != NULL){
	totalSize += GetAMCSize(i+1);
      
	if(GetAMCSize(i+1) <= 0x13fe){
	  nBlocks[i] = 1;
	}else{
	  nBlocks[i] = (GetAMCSize(i+1) - 1023)/4096 +1;
	}
	totalBlocks += nBlocks[i];
	if(nBlocks[i] > nBlocksMax){
	  nBlocksMax = nBlocks[i];
	}   
	nWords = totalSize + totalBlocks + nBlocksMax*2 + 2;
      }  
    }
    return nWords;
  }
}
