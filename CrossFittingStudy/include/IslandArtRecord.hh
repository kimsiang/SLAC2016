#ifndef ISLANDARTRECORD_HH
#define ISLANDARTRECORD_HH

/** @file IslandArtRecord.hh

  @Structure for 24 calos per fill art record
  @Author Kim Siang Khaw, khaw84@uw.edu
  @2016/01/21

 **/

#include <vector>

namespace gm2calo {

    struct IslandArtRecord {

        // Fill number
        int fillNum;

        // Calorimeter number
        int caloNum;

        // Island number
        int islandNum;

        // Crystal number
        int xtalNum;

        // Rider number (only for DAQ)
        int riderNum;

        // Rider Channel number (only for DAQ)
        int riderChannelNum;

        // uTCA slot number (only for DAQ)
        int utcaSlotNum;

        // Sample number of the first sample in the island
        int firstSampleNum;

        // Number of samples in the island
        int length;

        // ADC count of each sample
        std::vector<short> trace;

        IslandArtRecord() :
            fillNum(0), caloNum(0), islandNum(0), xtalNum(0), riderNum(0), 
            riderChannelNum(0), utcaSlotNum(0), firstSampleNum(0), length(0), trace()
        {}

        virtual ~IslandArtRecord(){};

        // ROOT doesn't need to know the rest
#ifndef __GCCXML__
        IslandArtRecord(int f_n, int c_n, int i_n, int x_n, int r_n,  int rc_n, 
                int u_n, int s_n, int len, std::vector<short> tr) :
            fillNum(f_n), caloNum(c_n), islandNum(i_n), xtalNum(x_n), 
            riderNum(r_n), riderChannelNum(rc_n), utcaSlotNum(u_n), firstSampleNum(s_n), length(len), trace(tr)
        {}
#endif // __GCCXML__

    }; //end of IslandArtRecord struct

    typedef std::vector<IslandArtRecord> IslandArtRecordCollection;

    struct MatrixIslandArtRecord {

        // Fill number
        int fillNum;

        // Calorimeter number
        int caloNum;

        // Island number in a single fill
        int islandNum;

        // Sample number of the first sample in each island
        int firstSampleNum;

        // Number of samples in the islands
        int length;

        // The island matrix
        IslandArtRecordCollection islands;

        MatrixIslandArtRecord() :
            fillNum(0), caloNum(0), islandNum(0), firstSampleNum(0), length(0), islands()
        {}

        virtual ~MatrixIslandArtRecord(){};

        // ROOT doesn't need to know the rest
#ifndef __GCCXML__
        MatrixIslandArtRecord(int f_n, int c_n, int i_n, int s_n ,int len, 
                IslandArtRecordCollection is) :
            fillNum(f_n), caloNum(c_n), islandNum(i_n), firstSampleNum(s_n), length(len), islands(is)
        {}
#endif // __GCCXML__

    }; //end of MatrixIslandArtRecord struct

    typedef std::vector<MatrixIslandArtRecord> MatrixIslandArtRecordCollection;

    struct CaloIslandArtRecord {

        // Fill number
        int fillNum;

        // Calorimeter number
        int caloNum;

        // t0 determined from laser calibration
        double tZero;

        // Vector of IslandMatrices, one for each island this fill
        MatrixIslandArtRecordCollection matrixIslands;

        CaloIslandArtRecord():
            fillNum(0), caloNum(0), tZero(0.0), matrixIslands()
        {}

        virtual ~CaloIslandArtRecord(){};

        // ROOT doesn't need to know the rest
#ifndef __GCCXML__
        CaloIslandArtRecord(int f_n, int c_n, double tZ, 
                MatrixIslandArtRecordCollection iM) :
            fillNum(f_n), caloNum(c_n), tZero(tZ), matrixIslands(iM)
        {}
#endif // __GCCXML__

    }; // end of CaloIslandArtRecord struct

    typedef std::vector<CaloIslandArtRecord> CaloIslandArtRecordCollection;
 
} // end namespace gm2calo

#endif //  ISLANDARTRECORD_HH
