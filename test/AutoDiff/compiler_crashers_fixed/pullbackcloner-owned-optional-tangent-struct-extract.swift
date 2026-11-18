// RUN: %target-build-swift %s
// RUN: %target-build-swift -O %s

// Regression test: `PullbackCloner::Implementation::visitEnumInst` (the
// implicit `Optional.some` promotion at a differentiable call site) emitted
// `struct_extract` directly on the materialized owned
// `Optional<T>.TangentVector` value. `struct_extract` is a static
// guaranteed-forwarding use (OperandOwnership.cpp) and can never legally
// take an owned operand, so the generated pullback failed verification.
//
// 6.5-dev (+assertions), Differentiation pass:
//   Found an operand with a value that is not compatible with the
//   operand's operand ownership kind map.
//   Value: %2 = apply %1(%0) ... @owned Optional<Array<Double>>.TangentVector
//   ...
//   %4 = struct_extract %2, #Optional.TangentVector.value
//   Constraint: <guaranteed NonLifetimeEnding>
//
// 6.4.x, -O (the malformed pullback survives to MandatoryDestroyHoisting):
//   Fatal error: ownership incompatible with an owned value
//
// Only fires when the optional wraps a non-trivial Differentiable type
// (e.g. Array): for trivial wrapped types (e.g. Double) the tangent struct
// has no ownership, so the illegal `struct_extract` was never diagnosed.
// The argument must be a non-constant expression derived from the wrt
// value (a constant folds away before the promotion is classified).

import _Differentiation

@differentiable(reverse)
func step(bcOverride: [Double]?) -> [Double] {
    return [0]
}

func runPB(state: [Double]) {
    _ = valueWithPullback(at: state) { a in
        step(bcOverride: a)
    }
}
