#include "CommandDecoder.h"


FCommandDecoder::FCommandDecoder(const FCommand& inCommand) : Command(inCommand)
{

}

bool FCommandDecoder::RegisterInput(FInput& outInput) const
{
    if (Command.GetType() != ECommandType::RegisterInput) {
        return false;
    }

    const TArray<uint8>& data = Command.GetData();

    outInput.Id = data[0];
    outInput.Pin = data[1];
    outInput.Type = static_cast<EInputType>(data[2]);

    const int LabelLength = data.Num() - 3;
    outInput.Label = TEXT("");
    for (int i = 0; i < LabelLength; ++i)
    {
        outInput.Label += static_cast<TCHAR>(data[i + 3]);
    }

    return true;
}

bool FCommandDecoder::InputValue(FInputValue& outInputValue) const
{
    if (Command.GetType() != ECommandType::InputValue) {
        return false;
    }

    const TArray<uint8>& data = Command.GetData();
    outInputValue.Id = data[0];
    outInputValue.Value = data[1];

    return true;
}

bool FCommandDecoder::Message(FString& outMessage) const
{
    if (Command.GetType() != ECommandType::Message) {
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
