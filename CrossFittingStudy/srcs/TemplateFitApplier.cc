// template fit applier implementation
#include "TemplateFitApplier.hh"
#include <cmath>
#include <iostream>

namespace gm2calo {
// determine how to fit this island and then fit it, returning result
FitResultArtRecordCollection TemplateFitApplier::operator()(
    TemplateFitter& tf, const IslandArtRecord& islandRecord) const {
  // prepare output data
  FitResultArtRecord fitResult;
  fitResult.fillNum = islandRecord.fillNum;
  fitResult.caloNum = islandRecord.caloNum;
  fitResult.islandNum = islandRecord.islandNum;
  fitResult.xtalNum = islandRecord.xtalNum;
  fitResult.pulsewidth = 0;  // this fitter doesn't find a pulse width

  const auto& trace = islandRecord.trace;
  std::size_t peakIndex;
  bool aboveThreshold = false;
  if (negPolarity_) {
    peakIndex = std::distance(trace.begin(),
                              std::min_element(trace.begin(), trace.end()));
    aboveThreshold = trace[peakIndex] < peakCutoff_;
  } else {
    peakIndex = std::distance(trace.begin(),
                              std::max_element(trace.begin(), trace.end()));
    aboveThreshold = trace[peakIndex] > peakCutoff_;
  }

  if ((!aboveThreshold) || (peakIndex <= wiggleRoom_) ||
      (peakIndex + wiggleRoom_ >= trace.size())) {
    // don't try to fit if amplitude is too small or
    // peak is too close to the edge of the island
    fitResult.chi2 = 0;
    fitResult.energy = 0;
    fitResult.time = 0;
    fitResult.pedestal = 0;

    return {fitResult};
  } else {
    // over threshold, so do the fit.
    // we will try three times at three different times
    // if it doesn't succeed by then, we call it a failed fit.
    TemplateFitter::Output singlePulseOut;

    std::vector<int> timeOffsets = {0, 1, -1};
    bool successfulFit = false;
    for (std::size_t i = 0; (!successfulFit) && (i < timeOffsets.size()); ++i) {
      // for now noise is set to one here, doesn't matter as long as it's flat
      singlePulseOut = tf.fit(trace, peakIndex + timeOffsets[i]);
      if ((std::abs(singlePulseOut.times[0] - peakIndex) < wiggleRoom_) &&
          (singlePulseOut.converged) &&
          (negPolarity_ ? (singlePulseOut.scales[0] < -1 * scaleCutoff_)
                        : (singlePulseOut.scales[0] > scaleCutoff_))) {
        successfulFit = true;
      }
    }

    // prepare to fit additional pulses
    auto multiPulseOut = singlePulseOut;
    // if single pulse time has been pulled too far from peak index,
    // use peak index as initial guess for multi pulse fitting instead
    if (std::abs(singlePulseOut.times[0] - peakIndex) > 1) {
      int originalMaxIterations = tf.getMaxIterations();
      tf.setMaxIterations(0);
      multiPulseOut = tf.fit(trace, peakIndex);
      tf.setMaxIterations(originalMaxIterations);
    }

    // make sure peak index is within proper limits
    if (peakIndex - artificialDeadTime_ < 0) {
      peakIndex = artificialDeadTime_;
    } else if (peakIndex + artificialDeadTime_ >= trace.size()) {
      peakIndex = trace.size() - 1 - artificialDeadTime_;
    }

    // try to fit additional pulses until a fit fails
    // or no residuals are over the cutoff
    bool successfulMultiFit = false;
    bool lastPulseSuccess = true;
    while (lastPulseSuccess) {
      auto residuals = tf.getResiduals();
      std::size_t maxResidualIndex;
      // find peak in residuals not including those within ADT of peak index
      auto peakIter = residuals.begin() + peakIndex;
      if (negPolarity_) {
        auto maxLower =
            std::min_element(residuals.begin(), peakIter - artificialDeadTime_);
        auto maxUpper =
            std::min_element(peakIter + artificialDeadTime_, residuals.end());
        auto maxIter = *maxLower < *maxUpper ? maxLower : maxUpper;
        maxResidualIndex = std::distance(residuals.begin(), maxIter);
      } else {
        auto maxLower =
            std::max_element(residuals.begin(), peakIter - artificialDeadTime_);
        auto maxUpper =
            std::max_element(peakIter + artificialDeadTime_, residuals.end());
        auto maxIter = *maxLower > *maxUpper ? maxLower : maxUpper;
        maxResidualIndex = std::distance(residuals.begin(), maxIter);
      }
      double maxResidual = residuals[maxResidualIndex];

      // try another pulse fit?
      if (negPolarity_ ? (maxResidual < -1 * residualCutoff_)
                       : (maxResidual > residualCutoff_)) {
        TemplateFitter::Output anotherPulseOut;

        // try at three times again to help with convergence
        std::vector<int> timeOffsets = {0, 1, -1};
        bool successfulNextFit = false;
        for (std::size_t i = 0;
             (!successfulNextFit) && (i < timeOffsets.size()); ++i) {
          auto timeGuesses = multiPulseOut.times;
          timeGuesses.push_back(maxResidualIndex + timeOffsets[i]);

          anotherPulseOut = tf.fit(trace, timeGuesses);

          // check for convergence
          if (anotherPulseOut.converged) {
            successfulNextFit = true;
          }

          // check whether each pulse is in the approximately right place
          for (std::size_t i = 0; i < anotherPulseOut.times.size(); ++i) {
            if (std::abs(anotherPulseOut.times[i] - timeGuesses[i]) >=
                2 * wiggleRoom_) {
              successfulNextFit = false;
            }
          }

          // check whether any times are out of range
          for (const auto& time : anotherPulseOut.times) {
            if ((time < 0) || (time >= trace.size())) {
              successfulNextFit = false;
            }
          }

          // check whether any delta_ts too small
          auto sortedTimes = anotherPulseOut.times;
          std::sort(sortedTimes.begin(), sortedTimes.end());
          for (std::size_t i = 1; i < sortedTimes.size(); ++i) {
            if (std::abs(sortedTimes[i] - sortedTimes[i - 1]) <
                artificialDeadTime_) {
              successfulNextFit = false;
            }
          }
        }

        // test if fit we just did was successful
        if (!successfulNextFit) {
          lastPulseSuccess = false;
        }

        // any negative energies
        for (const auto& scale : anotherPulseOut.scales) {
          lastPulseSuccess &= negPolarity_ ? (scale < -1 * scaleCutoff_)
                                           : (scale > scaleCutoff_);
        }

        // ratio to main pulse too small
        for (const auto& scale : anotherPulseOut.scales) {
          lastPulseSuccess &= scale / anotherPulseOut.scales[0] > ratioCutoff_;
        }

        if (lastPulseSuccess) {
          multiPulseOut = anotherPulseOut;
          successfulMultiFit = true;
        }
      } else {
        lastPulseSuccess = false;
      }
    }

    // record results
    TemplateFitter::Output out;
    if (successfulMultiFit) {
      out = multiPulseOut;
      successfulFit = successfulMultiFit;
    } else {
      out = singlePulseOut;
    }

    FitResultArtRecordCollection foundPulsesCollection(out.times.size(),
                                                       fitResult);
    for (std::size_t i = 0; i < out.times.size(); ++i) {
      foundPulsesCollection[i].energy =
          negPolarity_ ? -1 * out.scales[i] : out.scales[i];

      foundPulsesCollection[i].time =
          islandRecord.firstSampleNum + out.times[i];

      foundPulsesCollection[i].pedestal = out.pedestal;
      if (successfulFit) {
        foundPulsesCollection[i].chi2 = out.chi2;
      } else {
        // failed fit
        foundPulsesCollection[i].chi2 = -1;
      }
    }

    return foundPulsesCollection;
  }
}
}
