#ifndef RecoTauTag_RecoTau_RecoTauCommonUtilities_h
#define RecoTauTag_RecoTau_RecoTauCommonUtilities_h

/*
 * RecoTauCommonUtilities - utilities for extracting PFCandidates from
 * PFJets and summing over collections of PFCandidates.
 *
 * Author: Evan K. Friis (UC Davis)
 *
 * $Id $
 */

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include <vector>
#include <algorithm>
#include <numeric>

// Boost helpers
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/mem_fn.hpp>

#include <boost/type_traits/is_base_of.hpp>
#include <boost/static_assert.hpp>

namespace reco { namespace tau {

/// Extract pfCandidates of a given particle Id from a PFJet.  If sort is true,
/// candidates will be sorted by descending PT
std::vector<PFCandidatePtr> pfCandidates(const PFJet& jet,
                                         int particleId, bool sort=true);

/// Extract pfCandidates of a that match a list of particle Ids from a PFJet
std::vector<PFCandidatePtr> pfCandidates(const PFJet& jet,
                                         const std::vector<int>& particleIds,
                                         bool sort=true);

/// Extract all non-neutral candidates from a PFJet
std::vector<PFCandidatePtr> pfChargedCands(const PFJet& jet, bool sort=true);

/// Extract all pfGammas from a PFJet
std::vector<PFCandidatePtr> pfGammas(const PFJet& jet, bool sort=true);

/// Flatten a list of pi zeros into a list of there constituent PFCandidates
std::vector<PFCandidatePtr> flattenPiZeros(const std::vector<RecoTauPiZero>&);

/// Convert a BaseView (View<T>) to a TRefVector
template<typename RefVectorType, typename BaseView>
RefVectorType castView(const edm::Handle<BaseView>& view) {
  typedef typename RefVectorType::value_type OutputRef;
  // Double check at compile time that the inheritance is okay.  It can still
  // fail at runtime if you pass it the wrong collection.
  BOOST_STATIC_ASSERT(
      (boost::is_base_of<typename BaseView::value_type,
                         typename RefVectorType::member_type>::value));
  RefVectorType output;
  size_t nElements = view->size();
  output.reserve(nElements);
  // Cast each of our Refs
  for (size_t i = 0; i < nElements; ++i) {
    output.push_back(view->refAt(i).template castTo<OutputRef>());
  }
  return output;
}

/*
 *Given a range over a container of type C, return a new 'end' iterator such
 *that at max <N> elements are taken.  If there are less than N elements in the
 *array, leave the <end>  as it is
 */
template<typename InputIterator> InputIterator takeNElements(
    const InputIterator& begin, const InputIterator& end, size_t N) {
  size_t input_size = end - begin;
  return (N > input_size) ? end : begin + N;
}

/// Sum the four vectors in a collection of PFCandidates
template<typename InputIterator, typename FunctionPtr, typename ReturnType>
  ReturnType sumPFVector(InputIterator begin, InputIterator end,
      FunctionPtr func, ReturnType init) {
    ReturnType output = init;
    for(InputIterator cand = begin; cand != end; ++cand) {
      //#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
      output += ((**cand).*(func))();
    }
    return output;
  }

template<typename InputIterator> reco::Candidate::LorentzVector sumPFCandP4(
    InputIterator begin, InputIterator end) {
  return sumPFVector(begin, end, &PFCandidate::p4,
      reco::Candidate::LorentzVector());
}

/// Sum the PT of a collection of PFCandidates
template<typename InputIterator> double sumPFCandPt(InputIterator begin,
    InputIterator end) {
    return sumPFVector(begin, end, &PFCandidate::pt, 0.0);
  }

/// Sum the PT of a collection of PFCandidates
template<typename InputIterator> int sumPFCandCharge(InputIterator begin,
    InputIterator end) {
    return sumPFVector(begin, end, &PFCandidate::charge, 0);
  }

template<typename InputIterator> InputIterator leadPFCand(InputIterator begin,
    InputIterator end) {
    double max_pt = 0;
    InputIterator max_cand = begin;
    for(InputIterator cand = begin; cand != end; ++cand) {
      if( (*cand)->pt() > max_pt ) {
        max_pt = (*cand)->pt();
        max_cand = cand;
      }
    }
    return max_cand;
  }
}}  // end namespace reco::tau
#endif
