
#include "FedAMC.hh"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

namespace FedAMC13{

  FedAMC::FedAMC(){
  }

  void FedAMC::Clear(){
    AMCData.clear();
    AMCSize.clear();
    AMCPayload.clear();
  }

  void FedAMC::AddPayload(uint64_t* ptr,uint64_t size){
    AMCData.push_back(ptr);
    AMCSize.push_back(size);  
  } 

  uint64_t FedAMC::GetWord(size_t iWord){
    if(AMCData.size()==0 && AMCSize.size()==0){
      FedException::NULLPointer e;
      e.Append("That AMC is not active calling {GetWord(iWord)} \n");
      throw e;
    } 
 
    if(iWord > GetSize()-1){
      FedException::NULLPointer e;
      e.Append("That AMC data is not %ld words long {GetWord(iWord)}");
      throw e;
    }
 
    for(size_t iBlock=0;iBlock<AMCSize.size();iBlock++){ 
      if(AMCData[iBlock]!=NULL && AMCSize[iBlock]!=0){
	if(iWord < AMCSize[iBlock]){
	  return AMCData[iBlock][iWord];
	}
	iWord -= AMCSize[iBlock];
      }
    }
    FedException::NULLPointer e;
    e.Append("{GetWord(iWord)} call failed");
    throw e;
    return 0;
  }
  
  uint64_t FedAMC::GetSize(){
    if(AMCData.size()==0 && AMCSize.size()==0){
      FedException::NULLPointer e;
      e.Append("That AMC is not active calling {GetSize()} \n");
      throw e;
    } 
    uint64_t totalSize = 0;
    for(size_t iBlock=0;iBlock<AMCSize.size();iBlock++){
      totalSize += AMCSize[iBlock];
    }
    return totalSize;
  }

  const std::vector<uint64_t> & FedAMC::GetPayloadCopy(){
    //Note on push_back: This effectively increases the container size by one, which causes an automatic reallocation of the allocated storage space if -and only if- the new vector size surpasses the current vector capacity.
    
    //First check if vector is empty. If it is just build it.
    if(AMCPayload.empty()){
      //printf("Vector empty, building...\n");
      for(size_t iWord=0;iWord<GetSize();iWord++){
	AMCPayload.push_back(GetWord(iWord));
      }
      return AMCPayload;
    }    
    //Already built for this event. Note: FedEvent::Clear() (calls FedAMC::Clear()) will clear this payload vector.
    return AMCPayload;
  } 
}



