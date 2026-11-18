//===------- DifferentiationMangler.h --------- differentiation -*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2017 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_SIL_UTILS_DIFFERENTIATIONMANGLER_H
#define SWIFT_SIL_UTILS_DIFFERENTIATIONMANGLER_H

#include "swift/AST/ASTContext.h"
#include "swift/AST/ASTMangler.h"
#include "swift/AST/AutoDiff.h"
#include "swift/Basic/NullablePtr.h"
#include "swift/Demangling/Demangler.h"
#include "swift/SIL/SILFunction.h"

namespace swift {
namespace Mangle {

/// A mangler for generated differentiation functions.
class DifferentiationMangler : public ASTMangler {
public:
  DifferentiationMangler(ASTContext &Ctx) : ASTMangler(Ctx) {}
  /// Returns the mangled name for a differentiation function of the given kind.
  std::string mangleAutoDiffFunction(StringRef originalName,
                                     Demangle::AutoDiffFunctionKind kind,
                                     const AutoDiffConfig &config);
  /// Returns the mangled name for a derivative function of the given kind.
  std::string mangleDerivativeFunction(StringRef originalName,
                                       AutoDiffDerivativeFunctionKind kind,
                                       const AutoDiffConfig &config);
  /// Returns the mangled name for a linear map of the given kind.
  std::string mangleLinearMap(StringRef originalName,
                              AutoDiffLinearMapKind kind,
                              const AutoDiffConfig &config);
  /// Returns the mangled name for a derivative function subset parameters
  /// thunk.
  ///
  /// The name must be unique for every distinct (from, to) config pair: the
  /// "to" result indices in particular must be part of the name, because two
  /// subsets selecting different components of a tuple result can have the
  /// same linear map type (e.g. pulling result 0 vs result 1 of a
  /// `(Double, Double)` result) and would otherwise share a name.
  std::string mangleDerivativeFunctionSubsetParametersThunk(
      StringRef originalName, CanType toType,
      AutoDiffDerivativeFunctionKind linearMapKind,
      IndexSubset *fromParamIndices, IndexSubset *fromResultIndices,
      IndexSubset *toParamIndices, IndexSubset *toResultIndices);
  /// Returns the mangled name for a linear map subset parameters thunk.
  ///
  /// The name must be unique for every distinct thunk type. The thunk type is
  /// determined by both the source and target linear map types, so both types
  /// are included in the name; the "to" result indices are included as well,
  /// because the target type alone does not identify which components of a
  /// tuple result the (subset) result cotangents refer to (e.g. two pullback
  /// sites observing different components of a `(Double, Double)` result).
  std::string mangleLinearMapSubsetParametersThunk(
      CanType fromType, CanType toType, AutoDiffLinearMapKind linearMapKind,
      IndexSubset *fromParamIndices, IndexSubset *fromResultIndices,
      IndexSubset *toParamIndices, IndexSubset *toResultIndices);
};

} // end namespace Mangle
} // end namespace swift

#endif /* SWIFT_SIL_UTILS_DIFFERENTIATIONMANGLER_H */
