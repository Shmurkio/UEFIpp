#include <UEFIpp/Architecture/X64/Assembly/Node.hpp>

namespace UEFIpp::Architecture::X64::Assembly {
auto Node::InstructionNode(const InstructionSet::Instruction &Value,
                           Memory::AllocatorStub Allocator) -> Node {
  Node Result{Allocator};
  Result.Type_ = NodeType::Instruction;
  Result.Instruction_ = Value;
  return Result;
}

auto Node::LabelNode(Label Value, Memory::AllocatorStub Allocator) -> Node {
  Node Result{Allocator};
  Result.Type_ = NodeType::Label;
  Result.Label_ = Value;
  return Result;
}

auto Node::DataNode(Library::Span<const Foundation::Byte> Bytes,
                    Memory::AllocatorStub Allocator) -> Node {
  Node Result{Allocator};
  Result.Type_ = NodeType::Data;
  (void)Result.Data_.Assign(Bytes);
  return Result;
}

auto Node::AlignNode(Foundation::Size Alignment, Foundation::Byte Fill,
                     Memory::AllocatorStub Allocator) -> Node {
  Node Result{Allocator};
  Result.Type_ = NodeType::Align;
  Result.Alignment_ = Alignment;
  Result.Fill_ = Fill;
  return Result;
}

auto Node::Data() const noexcept -> Library::Span<const Foundation::Byte> {
  return Data_.View();
}
} // namespace UEFIpp::Architecture::X64::Assembly
