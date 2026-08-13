#include <SampleApplication/LibrarySamples.hpp>

namespace SampleApplication {
auto RunIoSamples(TestSuite &Tests) -> Foundation::Void {
  Tests.Begin("Modern I/O");

  Foundation::Byte SetBuffer[300]{};
  auto *SetResult = Memory::Set(SetBuffer, 0xA5, sizeof(SetBuffer));
  Foundation::Bool SetContentsValid{true};
  for (const auto Byte : SetBuffer) {
    if (Byte != 0xA5) {
      SetContentsValid = false;
      break;
    }
  }
  Tests.Check("bulk memory set preserves its ABI return value",
              SetResult == SetBuffer && SetContentsValid);

  Foundation::Byte CompareLeft[31]{};
  Foundation::Byte CompareRight[31]{};
  CompareLeft[30] = 1;
  CompareRight[30] = 2;
  Tests.Check("bulk memory compare checks a trailing partial block",
              Memory::Compare(CompareLeft, CompareRight,
                              sizeof(CompareLeft)) < 0);

  Foundation::Byte Overlap[48]{};
  for (Foundation::Size Index{}; Index < 32; ++Index) {
    Overlap[Index] = Foundation::Cast::Auto<Foundation::Byte>(Index + 1);
  }
  auto *MoveResult = Memory::Move(Overlap + 8, Overlap, 32);
  Foundation::Bool MoveContentsValid{true};
  for (Foundation::Size Index{}; Index < 32; ++Index) {
    if (Overlap[Index + 8] != Index + 1) {
      MoveContentsValid = false;
      break;
    }
  }
  Tests.Check("bulk memory move handles backward overlap",
              MoveResult == Overlap + 8 && MoveContentsValid);

  IO::MemorySink Output{};
  const auto Printed = IO::Print(
      Output, "value={:#06X}, text={:?}", Foundation::Uint32{0x2A},
      U8StringView{u8"UEFI\n"});
  const U8StringView Formatted{
      Foundation::Cast::Auto<const Foundation::Char8 *>(
          Output.Buffer().Data()),
      Output.Buffer().Size()};
  Tests.Check("format strings are checked and write UTF-8",
              Printed && Formatted == u8"value=0X002A, text=\"UEFI\\n\"");

  IO::MemorySink ChainedOutput{};
  auto Chained = IO::Out(ChainedOutput)
                 << "value="
                 << IO::Formatted("{:#06X}", Foundation::Uint32{0x2A})
                 << ", text=" << IO::Quoted(U8StringView{u8"UEFI\n"})
                 << IO::Newline;
  const U8StringView ChainedText{
      Foundation::Cast::Auto<const Foundation::Char8 *>(
          ChainedOutput.Buffer().Data()),
      ChainedOutput.Buffer().Size()};
  Tests.Check("operator output is stateless, formatted, and result-aware",
              Chained &&
                  ChainedText == u8"value=0X002A, text=\"UEFI\\n\"\n");
  auto ChainedResult = Foundation::Utility::Move(Chained).TakeResult();
  Tests.Check("operator output exposes an ordinary Result",
              static_cast<Foundation::Bool>(ChainedResult));

  Foundation::Byte BufferedStorage[32]{};
  IO::BufferedSink<IO::FixedBufferSink, 16> BufferedOutput{
      IO::FixedBufferSink{BufferedStorage}};
  auto FlushedOutput = IO::Out(BufferedOutput) << "visible" << IO::Flush;
  Tests.Check("operator flush publishes buffered output and reports failure",
              FlushedOutput && BufferedOutput.BufferedSize() == 0 &&
                  BufferedOutput.Sink().Size() == 7);

  IO::MemorySink ReferencedOutput{};
  IO::WriterRef OriginalWriter{ReferencedOutput};
  IO::WriterRef CopiedWriter{OriginalWriter};
  auto ReferencedWrite = IO::WriteText(CopiedWriter, u8"writer copy");
  const U8StringView ReferencedText{
      Foundation::Cast::Auto<const Foundation::Char8 *>(
          ReferencedOutput.Buffer().Data()),
      ReferencedOutput.Buffer().Size()};
  Tests.Check("borrowed writer references copy without self-wrapping",
              ReferencedWrite && ReferencedText == u8"writer copy");

  constexpr U8StringView ReferencedInputText{u8"reader copy"};
  IO::MemorySource ReferencedInput{IO::AsBytes(ReferencedInputText)};
  IO::ReaderRef OriginalReader{ReferencedInput};
  IO::ReaderRef CopiedReader{OriginalReader};
  Foundation::Byte ReferencedReadStorage[11]{};
  auto ReferencedRead = CopiedReader.ReadSome(ReferencedReadStorage);
  Tests.Check("borrowed reader references copy without self-wrapping",
              ReferencedRead && ReferencedRead.Value().Count == 11 &&
                  Memory::Compare(ReferencedReadStorage,
                                  ReferencedInputText.Data(), 11) == 0);

  constexpr U8StringView ExactInputText{u8"exact"};
  IO::MemorySource ExactInput{IO::AsBytes(ExactInputText)};
  Foundation::Byte ExactReadStorage[5]{};
  auto ExactRead = IO::ReadExact(ExactInput, ExactReadStorage);
  Tests.Check("exact reads accept end-of-input on the requested boundary",
              ExactRead &&
                  Memory::Compare(ExactReadStorage, ExactInputText.Data(), 5) ==
                      0);

  Foundation::Byte SmallStorage[4]{};
  IO::FixedBufferSink SmallOutput{SmallStorage};
  auto FailedChain =
      IO::Out(SmallOutput) << "UEFI" << "does not fit" << "not attempted";
  Tests.Check("operator output retains its first error and stops writing",
              !FailedChain &&
                  FailedChain.Error().Code == IO::ErrorCode::Overflow &&
                  SmallOutput.Size() == 4);

  constexpr U8StringView InputText{u8"-42 true"};
  IO::MemorySource Input{IO::AsBytes(InputText)};
  IO::TextReader<IO::MemorySource> Reader{
      Foundation::Utility::Move(Input)};
  auto Values = Reader >> IO::Read<Foundation::Int32, Foundation::Bool>;
  Tests.Check("operator extraction returns typed values",
              Values && UEFIpp::Library::Get<0>(Values.Value()) == -42 &&
                  UEFIpp::Library::Get<1>(Values.Value()));

  constexpr U8StringView HexText{u8"FF"};
  IO::MemorySource HexInput{IO::AsBytes(HexText)};
  IO::TextReader<IO::MemorySource> HexReader{
      Foundation::Utility::Move(HexInput)};
  IO::ParseOptions HexOptions{};
  HexOptions.Base = 16;
  auto HexValue =
      HexReader >> IO::Read<Foundation::Uint32>.With(HexOptions);
  Tests.Check("operator extraction accepts explicit parse options",
              HexValue && HexValue.Value() == 255);

  constexpr U8StringView InvalidSingleText{u8"invalid"};
  IO::MemorySource InvalidSingleInput{IO::AsBytes(InvalidSingleText)};
  IO::TextReader<IO::MemorySource> InvalidSingleReader{
      Foundation::Utility::Move(InvalidSingleInput)};
  auto InvalidSingle =
      InvalidSingleReader >> IO::Read<Foundation::Uint32>;
  auto RestoredSingleToken = InvalidSingleReader.ReadToken();
  Tests.Check("failed single extraction restores seekable input",
              !InvalidSingle && RestoredSingleToken &&
                  RestoredSingleToken.Value().View() == u8"invalid");

  constexpr U8StringView AssignmentText{u8"FF false"};
  IO::MemorySource AssignmentInput{IO::AsBytes(AssignmentText)};
  IO::TextReader<IO::MemorySource> AssignmentReader{
      Foundation::Utility::Move(AssignmentInput)};
  Foundation::Uint32 AssignedNumber{};
  Foundation::Bool AssignedFlag{true};
  auto Assigned = AssignmentReader >>
                  IO::Into(AssignedNumber, AssignedFlag).With(HexOptions);
  Tests.Check("Into commits destinations after complete parsing",
              Assigned && AssignedNumber == 255 && !AssignedFlag);

  constexpr U8StringView InvalidAssignmentText{u8"7 invalid"};
  IO::MemorySource InvalidAssignmentInput{
      IO::AsBytes(InvalidAssignmentText)};
  IO::TextReader<IO::MemorySource> InvalidAssignmentReader{
      Foundation::Utility::Move(InvalidAssignmentInput)};
  Foundation::Int32 PreservedNumber{91};
  Foundation::Bool PreservedFlag{true};
  auto RejectedAssignment = InvalidAssignmentReader >>
                            IO::Into(PreservedNumber, PreservedFlag);
  auto RestoredToken = InvalidAssignmentReader.ReadToken();
  Tests.Check("failed Into preserves destinations and seekable input",
              !RejectedAssignment && PreservedNumber == 91 && PreservedFlag &&
                  RestoredToken && RestoredToken.Value().View() == u8"7");

  constexpr U16StringView Utf16{u"Gr\u00fc\u00dfe \U0001F680"};
  constexpr U8StringView ExpectedUtf8{u8"Gr\u00fc\u00dfe \U0001F680"};
  auto Utf8 = IO::Utf16ToUtf8(Utf16);
  auto RoundTrip = Utf8 ? IO::Utf8ToUtf16(Utf8.Value().View())
                        : IO::Result<U16String>{IO::Failure(Utf8.Error())};
  Tests.Check("Unicode conversion validates and round-trips text",
              Utf8 && Utf8.Value().View() == ExpectedUtf8 && RoundTrip &&
                  RoundTrip.Value().View() == Utf16);

  const Foundation::Char8 MalformedBytes[]{
      Foundation::Cast::Auto<Foundation::Char8>(0xC2), u8'A'};
  auto Replaced = IO::Utf8ToUtf16(
      {MalformedBytes, 2}, {}, IO::InvalidEncodingPolicy::Replace);
  Tests.Check("replacement decoding preserves the following valid byte",
              Replaced && Replaced.Value().Size() == 2 &&
                  Replaced.Value()[0] == u'\uFFFD' &&
                  Replaced.Value()[1] == u'A');

  Foundation::Byte Storage[8]{};
  IO::AnyWriter<> Writer{IO::FixedBufferSink{Storage}};
  const auto Written = IO::WriteAll(Writer, IO::AsBytes(u8"firmware"));
  Tests.Check("owning type erasure stays allocation-free for small sinks",
              static_cast<Foundation::Bool>(Written));
}
} // namespace SampleApplication
