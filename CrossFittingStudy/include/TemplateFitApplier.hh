#ifndef TemplateFitApplier_hh
#define TemplateFitApplier_hh
// simple functor for applying template fit to island art records and returning
// fit results
// Aaron Fienberg

#include "IslandArtRecord.hh"
#include "FitResultArtRecord.hh"

#include "TemplateFitter.hh"

namespace gm2calo {
class TemplateFitApplier {
 public:
  TemplateFitApplier(bool negPolarity, short peakCutoff,
                     unsigned int wiggleRoom, double residualCutoff,
                     double ratioCutoff, double scaleCutoff,
                     double artificialDeadTime)
      : negPolarity_{negPolarity},
        peakCutoff_{peakCutoff},
        wiggleRoom_{wiggleRoom},
        residualCutoff_{residualCutoff},
        ratioCutoff_{ratioCutoff},
        scaleCutoff_{scaleCutoff},
        artificialDeadTime_{artificialDeadTime} {}

  FitResultArtRecordCollection operator()(
      TemplateFitter& tf, const IslandArtRecord& islandRecord) const;

 private:
  const bool negPolarity_;
  const short peakCutoff_;
  // how far from the peak you're allowed to go without calling it a failed fit
  const unsigned int wiggleRoom_;

  // pileup fitting parameters
  const double residualCutoff_;
  const double ratioCutoff_;
  const double scaleCutoff_;
  const double artificialDeadTime_;
};
}

#endif
