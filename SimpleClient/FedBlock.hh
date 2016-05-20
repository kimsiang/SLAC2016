#ifndef __FEDLOCK_HH__
#define __FEDBLOCK_HH__

// Class to parse a specific Block within the Event
 
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include "FedException.hh"
#define N_AMC 12

//Bit Field Values:
#define BLOCK_NAMC_WORD 0
#define BLOCK_NAMC_BIT 52
#define BLOCK_NAMC_MASK 0xf
#define MS_BIT 60
#define MS_MASK 0x3
#define AMCSIZE_BIT 32
#define AMCSIZE_MASK 0xffffff
#define LMSEPVC_BIT 56
#define LMSEPVC_MASK 0x7f
#define AMCNO_BIT 16
#define AMCNO_MASK 0xf
#define ORN_BLOCK_WORD 0
#define ORN_BLOCK_BIT 4
#define ORN_BLOCK_MASK 0xffffffff
#define BLOCK_NO_BIT 20
#define BLOCK_NO_MASK 0xff
#define ID_BIT 0
#define ID_MASK 0xffff
#define UFOV_WORD 0
#define UFOV_BIT 60
#define UFOV_MASK 0xf
#define BLOCK_NO_TR_WORD -1
#define BLOCK_NO_TR_BIT 20
#define BLOCK_NO_TR_MASK 0xff
#define EVN_TR_WORD -1
#define EVN_TR_BIT 12
#define EVN_TR_MASK 0xff
#define BCN_TR_WORD -1
#define BCN_TR_BIT 0
#define BCN_TR_MASK 0xfff




namespace FedAMC13{
  /*! \brief Class to access blocks within an AMC13 event
   */
  class FedBlock{ 

  public: 

    FedBlock();

    void Clear();

    //! PreParse does minimal checks on data format and sets private variables blockPointer and sizeLeft. No return.
    /// \param blockPointer pointer to the beginning of a block
    /// \param size number of 64-bit words left in the total event to prevent reading off the end of an event
    void PreParse(uint64_t* blockPointerInput, uint64_t sizeInput);

    //! Parse returns a pointer to the next block using the internal pointer/size set by PreParse.
    uint64_t* Parse();

    //! Returns a pointer to an AMC header in a block (will return null if the AMC is not active).
    /// \param iAMC AMC number (1-based)
    uint64_t* GetAMCInfo(int iAMC);

    //! Returns value in the block header by AMC number. Will throw if trying to access inactive AMC.
    /// \param iAMCword 0 for first word in block header. 1-12 to access AMC headers.
    /// \param bit Bit location of value in 64-bit word.
    /// \param mask Mask of value.
    uint32_t GetMaskedHeader(int64_t iAMCword, int bit, int mask);

    //Functions used to compute the number of 64-bit words to the next block or error checking.
    uint32_t GetNAMC();
    uint32_t GetOrN(){         return GetMaskedHeader(ORN_BLOCK_WORD, ORN_BLOCK_BIT, ORN_BLOCK_MASK);}
    uint32_t GetuFOV(){        return GetMaskedHeader(UFOV_WORD, UFOV_BIT, UFOV_MASK);}
    uint32_t GetBlockNoTR(){   return GetMaskedHeader(BLOCK_NO_TR_WORD, BLOCK_NO_TR_BIT, BLOCK_NO_TR_MASK);}
    uint32_t GetEvNTR(){       return GetMaskedHeader(EVN_TR_WORD, EVN_TR_BIT, EVN_TR_MASK);}
    uint32_t GetBcNTR(){       return GetMaskedHeader(BCN_TR_WORD, BCN_TR_BIT, BCN_TR_MASK);}

    //When calling these function must check if AMC is active or GetMaskedHeader will throw.
    uint32_t GetMS(int iAMC){      return GetMaskedHeader(iAMC, MS_BIT, MS_MASK);}        
    uint32_t GetLMSEPVC(int iAMC){ return GetMaskedHeader(iAMC, LMSEPVC_BIT, LMSEPVC_MASK);}
    uint32_t GetAMCSize(int iAMC){ return GetMaskedHeader(iAMC, AMCSIZE_BIT, AMCSIZE_MASK);}
    uint32_t GetID(int iAMC) {     return GetMaskedHeader(iAMC, ID_BIT, ID_MASK);}
    uint64_t GetBlockNo(int iAMC){ return GetMaskedHeader(iAMC, BLOCK_NO_BIT, BLOCK_NO_MASK);}

    //! Used in Parse to compute the number of words to the next block.
    uint64_t GetSize();

    //! Returns a pointer to the beginning of an AMC payload. Must be called with {GetAMCInfo()}.
    /// \param iAMC AMC number (1-based)
    uint64_t* GetAMCPtr(int iAMC);

    //! Outputs the actual AMCSize within the block.
    uint64_t BlockAMCSize(int iAMC);

    //! Returns the size of the event calculated by Mr. Wu's formula.
    uint64_t GetEventSize();


  private:
    uint64_t* blockPointer;
    uint64_t sizeLeft;
    uint64_t* AMCInfo[N_AMC];
  };  
}

#endif
