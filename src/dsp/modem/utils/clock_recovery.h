/*
Clock recovery from goestools / GNU Radio (https://github.com/pietern/goestools, https://github.com/gnuradio/gnuradio)
Made standalone
*/

#pragma once

#include <complex>
#include <vector>
#include <memory>

class ClockRecovery
{
public:
  explicit ClockRecovery(uint32_t sampleRate, float symbolRate);

  //void setSamplePublisher(std::unique_ptr<SamplePublisher> samplePublisher)
  //{
  //  samplePublisher_ = std::move(samplePublisher);
  //}

  // See http://www.trondeau.com/blog/2011/8/13/control-loop-gain-values.html
  void setLoopBandwidth(float bw);

  // Returns number of samples per symbol.
  float getOmega() const
  {
    return omega_;
  }

  std::vector<std::complex<float>> work(std::vector<std::complex<float>>, size_t size);

protected:
  float omega_;
  float omegaMin_;
  float omegaMax_;
  float omegaGain_;
  float mu_;
  float muGain_;

  // Past samples
  std::complex<float> p0t_;
  std::complex<float> p1t_;
  std::complex<float> p2t_;

  // Past associated quadrants
  std::complex<float> c0t_;
  std::complex<float> c1t_;
  std::complex<float> c2t_;

  std::vector<std::complex<float>> tmp_;

  //std::unique_ptr<SamplePublisher> samplePublisher_;
};
