#pragma once

#include <UEFIpp/Architecture/X64/Assembly/Label.hpp>
#include <UEFIpp/Architecture/X64/Instruction/Instruction.hpp>
#include <UEFIpp/Library/Containers/Span.hpp>
#include <UEFIpp/Library/Containers/Vector.hpp>
#include <UEFIpp/Memory/AllocatorStub.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
enum class NodeType : Foundation::Uint8 { Instruction, Label, Data, Align };

class Node {
public:
  constexpr explicit Node(Memory::AllocatorStub Allocator = {}) noexcept
      : Data_(Allocator) {}

  [[nodiscard]] static auto
  InstructionNode(const InstructionSet::Instruction &Value,
                  Memory::AllocatorStub Allocator = {}) -> Node;

  [[nodiscard]] static auto LabelNode(Label Value,
                                      Memory::AllocatorStub Allocator = {})
      -> Node;

  [[nodiscard]] static auto
  DataNode(Library::Span<const Foundation::Byte> Bytes,
           Memory::AllocatorStub Allocator = {}) -> Node;

  [[nodiscard]] static auto AlignNode(Foundation::Size Alignment,
                                      Foundation::Byte Fill,
                                      Memory::AllocatorStub Allocator = {})
      -> Node;

  [[nodiscard]] constexpr auto Type() const noexcept -> NodeType {
    return Type_;
  }

  [[nodiscard]] constexpr auto InstructionValue() noexcept
      -> InstructionSet::Instruction & {
    return Instruction_;
  }

  [[nodiscard]] constexpr auto InstructionValue() const noexcept
      -> const InstructionSet::Instruction & {
    return Instruction_;
  }

  [[nodiscard]] constexpr auto LabelValue() const noexcept -> Label {
    return Label_;
  }

  [[nodiscard]] auto Data() const noexcept
      -> Library::Span<const Foundation::Byte>;

  [[nodiscard]] constexpr auto Alignment() const noexcept -> Foundation::Size {
    return Alignment_;
  }

  [[nodiscard]] constexpr auto Fill() const noexcept -> Foundation::Byte {
    return Fill_;
  }

  constexpr auto SetLabelTarget(Label Target,
                                Foundation::Uint8 OperandIndex) noexcept
      -> Foundation::Void {
    HasLabelTarget_ = true;
    TargetLabel_ = Target;
    TargetOperandIndex_ = OperandIndex;
  }

  [[nodiscard]] constexpr auto HasLabelTarget() const noexcept
      -> Foundation::Bool {
    return HasLabelTarget_;
  }

  [[nodiscard]] constexpr auto TargetLabel() const noexcept -> Label {
    return TargetLabel_;
  }

  [[nodiscard]] constexpr auto TargetOperandIndex() const noexcept
      -> Foundation::Uint8 {
    return TargetOperandIndex_;
  }

private:
  NodeType Type_{NodeType::Instruction};
  InstructionSet::Instruction Instruction_{};
  Label Label_{};
  Library::Vector<Foundation::Byte> Data_;
  Foundation::Size Alignment_{1};
  Foundation::Byte Fill_{};

  Foundation::Bool HasLabelTarget_{};
  Label TargetLabel_{};
  Foundation::Uint8 TargetOperandIndex_{};
};
} // namespace UEFIpp::Architecture::X64::Assembly
