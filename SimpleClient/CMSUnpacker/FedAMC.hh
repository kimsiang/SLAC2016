#ifndef __FEDAMC_HH__
#define __FEDAMC_HH__

#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "FedException.hh"

//Bit locations and masks for AMCPayload format:
#define AMCNo_WORD 0
#define AMCNo_BIT 56
#define AMCNo_MASK 0xf
#define EvN_WORD 0
#define EvN_BIT 32
#define EvN_MASK 0xffffff
#define BcN_WORD 0
#define BcN_BIT 20
#define BcN_MASK 0xfff
#define OrN_WORD 1
#define OrN_BIT 16
#define OrN_MASK 0xffff
#define DataLength_WORD 0
#define DataLength_BIT 0
#define DataLength_MASK 0xfffff
#define BoardID_WORD 1
#define BoardID_BIT 0
#define BoardID_MASK 0xffff
#define CRC_BIT 32
#define CRC_MASK 0xffffffff
#define EvN_TR_BIT 24
#define EvN_TR_MASK 0xff
#define DataLength_TR_BIT 0
#define DataLength_TR_MASK 0xfffff

namespace FedAMC13{

  /*! \brief Class to access AMC payloads within an AMC13 event
   */
  class FedAMC{
  
  public:
    FedAMC();
    //! Clears AMC pointer and size vectors
    void Clear();

    //! Operator to get word from AMC.
    uint64_t operator [] (size_t iWord){
      return GetWord(iWord);
    }

    //! Pushes back pointer and size onto AMCData and AMCSize vectors, respectively.
    void AddPayload(uint64_t* ptr,uint64_t size);

    //! Returns the 64-bit word of the AMC payload.
    uint64_t GetWord(size_t iWord);

    //! Returns the size of the AMC payload
    uint64_t GetSize();

    //! Returns reference to AMCPayload vector. This will make a copy of the AMC payloads.
    const std::vector<uint64_t> & GetPayloadCopy();

    //Functions to access values in AMC payload header and trailer:
    uint32_t AMCNo(){        return (GetWord(AMCNo_WORD) >> AMCNo_BIT) & AMCNo_MASK;}
    uint32_t EvN(){          return (GetWord(EvN_WORD) >> EvN_BIT) & EvN_MASK; }
    uint32_t BcN(){          return (GetWord(BcN_WORD) >> BcN_BIT) & BcN_MASK;}
    uint32_t OrN(){          return (GetWord(OrN_WORD) >> OrN_BIT) & OrN_MASK;}
    uint32_t DataLength(){   return (GetWord(DataLength_WORD) >> DataLength_BIT) & DataLength_MASK;}
    uint32_t BoardID(){      return (GetWord(BoardID_WORD) >> BoardID_BIT) & BoardID_MASK;}
    uint32_t CRC(){          return (GetWord(GetSize()-1) >> CRC_BIT) & CRC_MASK;}
    uint32_t EvN_TR(){       return (GetWord(GetSize()-1) >> EvN_TR_BIT) & EvN_TR_MASK;}
    uint32_t DataLength_TR(){return (GetWord(GetSize()-1) >> DataLength_TR_BIT) & DataLength_TR_MASK;}

  private:
    std::vector <uint64_t*> AMCData;
    std::vector <uint64_t> AMCSize;
    std::vector <uint64_t> AMCPayload;
  };
}

#endif

  
