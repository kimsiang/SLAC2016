#ifndef __FEDEVENT_HH__
#define __FEDEVENT_HH__

#include <stdint.h>
#include <vector>
#include <stdlib.h>
#include "FedBlock.hh"
#include "FedAMC.hh"
#include "FedException.hh"
#include "Fedcrc16d64.hh"


//Version Number
#define VERSION 0x2
//Djarcaro - 4/12/16




#define N_AMC 12
#define HEADER_WORDS 2
#define MAGIC_WORD 5
#define MAGIC_WORD_TRAILER 0xA

//List of values to be used by GetMaskedField to read values in header/trailer
#define  BOE_WORD 0
#define  BOE_BIT 60
#define  BOE_MASK 0xf
#define  EVNTYPE_WORD 0
#define  EVNTYPE_BIT 56
#define  EVNTYPE_MASK 0xf
#define  EVN_WORD 0
#define  EVN_BIT 32
#define  EVN_MASK 0xffffff
#define  BCN_WORD 0
#define  BCN_BIT 20
#define  BCN_MASK 0xfff
#define  BXID_WORD 0
#define  BXID_BIT 8
#define  BXID_MASK 0xfff
#define  ORN_WORD 1
#define  ORN_BIT 4
#define  ORN_MASK 0xffffffff
#define  NAMC_WORD 1
#define  NAMC_BIT 52
#define  NAMC_MASK 0xf
#define  AMCSIZE_BIT 32
#define  AMCSIZE_MASK 0xffffff
#define  TRAILERSIZE_BIT 32
#define  TRAILERSIZE_MASK 0xffffff

namespace FedAMC13{

  /*! \brief Class to access items within AMC13 Data with minimal unpacking
   *
   * First PreParse must be called with a pointer to the raw event and the number
   * of 64-bit words safe to read(two in the first call) two times to read an event. 
   * Parse can then be called with a pointer to the raw event and its size in 
   * 64-bit words. 
   * AMC objects are created to easily access any of the AMC payloads.
   * Accessor functions are provided to the various parts of the data payload.
   *
   * See documentation for data format:
   * http://bucms.bu.edu/twiki/bin/view/BUCMSPublic/AMC13CommonFirmwareProposal
   *
   * Error-checking functions is provided.
   *
   */
  class FedEvent {
  public:
    FedEvent();
    //! Clear all values from FedEvent object.
    void Clear();

    //! Operator to get an AMC object.
    FedAMC13::FedAMC & operator [] (int iAMC){
      return GetAMC(iAMC);
    }

    //! Calculate CMS CRC.
    uint16_t CalcCRC();

    //!Get CRC from CDF trailer.
    uint16_t CRC() { return (dataPointer[dataSize-1] >> 16) & 0xffff; }

    void FedKit2Binary();

    //! Allocates an internal vector of the data from the file. Checks the output along the way to confirm AMC13 format
    /// \param fileStream File Stream of data file.  
    bool PreParse(std::ifstream & fileStream);
    
    //! Allocates an internal vector of the data from the file. Checks the output along the way to confirm AMC13 format
    /// \param fp File pointer to data file. 
    bool PreParse(FILE * fp);
    
    //! Sets up pointer to each AMC Payload for fast access. Uses internal pointer/size set by PreParse().
    void ParseEvent();

    //! Sets up pointer to each AMC payload for fast access. Uses internal pointer/size set by arguments. 
    /// \param inputPointer pointer to the header of a raw AMC event
    /// \param inputSize size of event to be parsed
    void ParseEvent(uint64_t * inputPointer, uint64_t inputSize);
  
    //! Outputs summary of events onto the screen with various levels of detail.
    /// \param dumpLevel input to determine how much to print, ranging from 0-4 (default 0, or least amount)
    /// \param CRCLevel input to turn CRC calculation on/off, false = off, true = on
    /// \param errorLevel input to turn error checking on/off, off = default, on = 1
    void Dump(int dumpLevel, bool CRCLevel, int errorLevel);
 
    //Functions to access FedBlock and FedAMC objects
    
    //! Function to access FedBlock objects
    FedAMC13::FedBlock & GetBlock(uint32_t iBlock);
    
    //! Returns the number of blocks in a FedEvent
    size_t GetnBlocks();
    
    //! Function to access FedAMC objects
    FedAMC13::FedAMC & GetAMC(int iAMC);

    //Function to get values in AMC Data
    uint64_t GetMaskedField(uint64_t word, uint32_t bit, uint32_t mask);
    //Functions that use GetMaskedField and #define values to access various header/trailer values
    size_t GetBOE(){ return GetMaskedField(BOE_WORD, BOE_BIT, BOE_MASK);}
    size_t GetEventType(){ return GetMaskedField(EVNTYPE_WORD, EVNTYPE_BIT, EVNTYPE_MASK);}
    size_t GetNAMC(){ return GetMaskedField(NAMC_WORD, NAMC_BIT, NAMC_MASK);}
    size_t GetEvN(){ return GetMaskedField(EVN_WORD, EVN_BIT, EVN_MASK);}
    size_t GetBcN(){ return GetMaskedField(BCN_WORD,BCN_BIT,BCN_MASK);}
    size_t GetOrN(){ return GetMaskedField(ORN_WORD,ORN_BIT,ORN_MASK);}
    size_t GetBXID(){ return GetMaskedField(BXID_WORD, BXID_BIT, BXID_MASK);}
    size_t GetAMCSize(int n){ return GetMaskedField(n+1, AMCSIZE_BIT, AMCSIZE_MASK);}

    //! Returns calculated event size by Mr. Wu's formula.
    size_t GetEventSize(){return eventSize;}
    
    //! Function to create error array
    void ErrorChecking();
    //! Function to get elements from the error array. 
    uint32_t GetAMC13Errors(uint32_t AMC, uint32_t error, uint32_t value);

  private:
    std::string dataFileType; //String to keep track of the data type (Binary, BadCoffee, FedKit).
    std::vector<uint64_t> dataVector;
    uint64_t * dataPointer;
    uint64_t dataSize;
    std::vector<FedBlock> block;    //Vector of FedBlock objects that carry a pointer and size to each block within an event
    FedAMC13::FedAMC AMC[12];       //Array of FedAMC objects that carry a vector of pointers to the AMC payloads
    size_t eventSize;               //Calculated size of event
    uint32_t AMC13Errors[13][20][3]; //Error Checking
  };
}

#endif
