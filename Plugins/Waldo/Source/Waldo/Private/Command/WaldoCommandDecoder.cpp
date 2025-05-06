#include "WaldoCommandDecoder.h"


FWaldoCommandDecoder::FWaldoCommandDecoder(const FWaldoCommand& inCommand) : Command(inCommand)
{

}

bool FWaldoCommandDecoder::DecodeRegisterInput(FWaldoInput& outInput) const
{
    if (Command.GetType() != EWaldoCommandType::RegisterInput) {
        return false;
    }

    const TArray<uint8>& data = Command.GetData();

    outInput.Id = data[0];
    outInput.Pin = data[1];
    outInput.Type = static_cast<EWaldoInputType>(data[2]);

    const int LabelLength = data.Num() - 3;
    outInput.Label = TEXT("");
    for (int i = 0; i < LabelLength; ++i)
    {
        outInput.Label += static_cast<TCHAR>(data[i + 3]);
    }

    return true;
}

bool FWaldoCommandDecoder::DecodeInputValue(FWaldoInputValue& outInputValue) const
{
    if (Command.GetType() != EWaldoCommandType::InputValue) {
        return false;
    }

    const TArray<uint8>& data = Command.GetData();
    outInputValue.Id = data[0];
    outInputValue.Value = data[1];

    return true;
}

bool FWaldoCommandDecoder::DecodeMessage(FString& outMessage) const
{
    if (Command.GetType() != EWaldoCommandType::Message) {
        return false;
    }

    const TArray<uint8>& data = Command.GetData();
    outMessage = TEXT("");
    const int MessageLength = data.Num();
    for (int i = 0; i < MessageLength; ++i)
    {
        outMessage += static_cast<TCHAR>(data[i]);   
    }
    
    return true;
}
