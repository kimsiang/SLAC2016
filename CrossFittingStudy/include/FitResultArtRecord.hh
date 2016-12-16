#ifndef FITRESULTARTRECORD_HH
#define FITRESULTARTRECORD_HH

/** @file FitResultArtRecord.hh

  @Structure for 24 calos per fill art record
  @Author Kim Siang Khaw, khaw84@uw.edu
  @2016/01/21

 **/

#include <vector>

namespace gm2calo {

    struct FitResultArtRecord {

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

        // time from the island fit
        double time;

        // energy from the island fit
        double energy;

        //fit pedestal
        double pedestal;

        // pulse width from the island fit
        double pulsewidth;

        // chi2/NDF from the island fit
        double chi2;

        FitResultArtRecord() :
            fillNum(0), caloNum(0), islandNum(0), xtalNum(0), riderNum(0), 
            riderChannelNum(0), utcaSlotNum(0), time(0.0), energy(0.0), pedestal(0.0), pulsewidth(0.0), chi2(0.0)
        {}

        virtual ~FitResultArtRecord(){};

        // ROOT doesn't need to know the rest
#ifndef __GCCXML__
        FitResultArtRecord(int f_n, int c_n, int i_n, int x_n, int r_n,  int rc_n, 
                int u_n, double t, double ene, double ped, double pw, double cnd) :
            fillNum(f_n), caloNum(c_n), islandNum(i_n), xtalNum(x_n), 
            riderNum(r_n), riderChannelNum(rc_n), utcaSlotNum(u_n), time(t), energy(ene), pedestal(ped), pulsewidth(pw), chi2(cnd)
        {}
#endif // __GCCXML__

    }; //end of FitResultArtRecord struct

    typedef std::vector<FitResultArtRecord> FitResultArtRecordCollection;

    struct MatrixFitResultArtRecord {

        // Fill number
        int fillNum;

        // Calorimeter number
        int caloNum;

        // island number in a calo
        int islandNum;

        // The island matrix
        FitResultArtRecordCollection fitResults;

        MatrixFitResultArtRecord() :
            fillNum(0), caloNum(0), islandNum(0), fitResults()
        {}

        virtual ~MatrixFitResultArtRecord(){};

        // ROOT doesn't need to know the rest
#ifndef __GCCXML__
        MatrixFitResultArtRecord(int f_n, int c_n, int i_n,  
                FitResultArtRecordCollection frs) :
            fillNum(f_n), caloNum(c_n), islandNum(i_n), fitResults(frs)
        {}
#endif // __GCCXML__

    }; //end of MatrixFitResultArtRecord struct

    typedef std::vector<MatrixFitResultArtRecord> MatrixFitResultArtRecordCollection;

    struct CaloFitResultArtRecord {

        // Fill number
        int fillNum;

        // Calorimeter number
        int caloNum;

        // t0 determined from laser calibration
        double tZero;

        // Vector of FitResultMatrices, one for each island this fill
        MatrixFitResultArtRecordCollection matrixFitResults;

        CaloFitResultArtRecord():
            fillNum(0), caloNum(0), tZero(0.0), matrixFitResults()
        {}

        virtual ~CaloFitResultArtRecord(){};

        // ROOT doesn't need to know the rest
#ifndef __GCCXML__
        CaloFitResultArtRecord(int f_n, int c_n, double tZ, 
                MatrixFitResultArtRecordCollection mfr) :
            fillNum(f_n), caloNum(c_n), tZero(tZ), matrixFitResults(mfr)
        {}
#endif // __GCCXML__

    }; // end of CaloFitResultArtRecord struct

    typedef std::vector<CaloFitResultArtRecord> CaloFitResultArtRecordCollection;

} // end namespace gm2calo

#endif //  FITRESULTARTRECORD_HH
