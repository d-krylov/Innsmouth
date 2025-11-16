#ifndef INNSMOUTH_MASK_H
#define INNSMOUTH_MASK_H

#include <concepts>
#include <utility>

namespace Innsmouth {

template <typename T>
concept ScopedEnum = std::is_scoped_enum_v<T>;

template <ScopedEnum MaskBit> class Mask {
public:
  using MaskType = typename std::underlying_type_t<MaskBit>;

  constexpr Mask() : mask_(0) {
  }

  constexpr Mask(MaskBit bit) : mask_(std::to_underlying(bit)) {
  }

  constexpr explicit Mask(MaskType mask) : mask_(mask) {
  }

  constexpr bool operator==(const Mask &other) const = default;

  constexpr Mask<MaskBit> &operator|=(const Mask<MaskBit> &rhs) {
    mask_ |= rhs.mask_;
    return *this;
  }

  constexpr Mask<MaskBit> &operator&=(const Mask<MaskBit> &rhs) {
    mask_ &= rhs.mask_;
    return *this;
  }

  constexpr Mask<MaskBit> &operator^=(const Mask<MaskBit> &rhs) {
    mask_ ^= rhs.mask_;
    return *this;
  }

  constexpr Mask<MaskBit> operator~() const {
    return ~mask_;
  }

  bool HasBits(const Mask<MaskBit> &requiered_mask) const {
    return (*this & requiered_mask) == requiered_mask;
  }

  constexpr Mask<MaskBit> operator&(const Mask<MaskBit> &rhs) const {
    return Mask<MaskBit>(mask_ & rhs.mask_);
  }

  constexpr Mask<MaskBit> operator|(const Mask<MaskBit> &rhs) const {
    return Mask<MaskBit>(mask_ | rhs.mask_);
  }

  constexpr Mask<MaskBit> operator^(const Mask<MaskBit> &rhs) const {
    return Mask<MaskBit>(mask_ ^ rhs.mask_);
  }

  constexpr bool operator!() const {
    return mask_ == 0;
  }

  explicit constexpr operator bool() const {
    return !!mask_;
  }

  explicit constexpr operator MaskType() const {
    return mask_;
  }

  auto GetValue() const {
    return mask_;
  }

private:
  MaskType mask_;
};

template <typename MaskBit> inline constexpr Mask<MaskBit> operator&(MaskBit bit, const Mask<MaskBit> &flags) {
  return flags.operator&(bit);
}

template <typename MaskBit> inline constexpr Mask<MaskBit> operator|(MaskBit bit, const Mask<MaskBit> &flags) {
  return flags.operator|(bit);
}

template <ScopedEnum MaskBit> inline constexpr Mask<MaskBit> operator&(MaskBit lhs, MaskBit rhs) {
  return Mask<MaskBit>(lhs) & rhs;
}

template <ScopedEnum MaskBit> inline constexpr Mask<MaskBit> operator|(MaskBit lhs, MaskBit rhs) {
  return Mask<MaskBit>(lhs) | rhs;
}

template <ScopedEnum MaskBit> inline constexpr Mask<MaskBit> operator^(MaskBit lhs, MaskBit rhs) {
  return Mask<MaskBit>(lhs) ^ rhs;
}

} // namespace Innsmouth

#endif // INNSMOUTH_MASK_H