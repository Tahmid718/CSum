#include "calculate.h"
#include "../gui/csum.h"
#include "../gui/hfs.h"
#include "../gui/ase.h"

#include <wx/msgdlg.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>

std::vector<unsigned char> aesEncrypt(std::vector<unsigned char> Key, std::vector<unsigned char> IV, const EVP_CIPHER* CipherName);
std::vector<unsigned char> aesDecrypt(std::vector<unsigned char> Key, std::vector<unsigned char> IV, const EVP_CIPHER* CipherName);

void Hash::CalculateHash()
{
    int AlgorithmIndex = CSumFrame::AlgorithmEntry->GetSelection();

    if(AlgorithmIndex >= 0 && AlgorithmIndex <= 2)
    {
        CSumFrame::OpenFileButton->Disable();
        CSumFrame::CalculateHashButton->Disable();

        const std::string FilePath = CSumFrame::PathEntry->GetValue().ToStdString();

        if(FilePath.length() == 0)
        {
            wxMessageBox("Choose a file", "CSum Error", wxOK|wxICON_ERROR);
            return;
        }

        double start = time(0);

        std::ifstream File(FilePath, std::ios::binary);
        CSumFrame::Status->SetLabel("Processing the file...");

        if(!File)
        {
            wxMessageBox("Couldn't open the given file", "CSum Error", wxOK|wxICON_ERROR);
            return;
        }

        File.seekg(0, std::ios::end);
        std::streamsize Size = File.tellg();
        File.seekg(0, std::ios::beg);

        std::vector<char> BufferValues(Size);
        if(!File.read(BufferValues.data(), Size))
        {
            wxMessageBox("Couldn't read the given file.", "CSum Error", wxOK|wxICON_ERROR);
            return;
        }

        File.close();
        CSumFrame::Status->SetLabel("Calculating the hash...");

        unsigned char Digest[64];
        unsigned int DigestLen = 0;

        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        switch(AlgorithmIndex)
        {
            case 0:
                EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
                break;
            case 1:
                EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr);
                break;
            case 2:
                EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
                break;
        }

        EVP_DigestUpdate(
            ctx,
            reinterpret_cast<const unsigned char*>(BufferValues.data()),
            BufferValues.size()
        );
        EVP_DigestFinal_ex(ctx, Digest, &DigestLen);
        EVP_MD_CTX_free(ctx);

        std::ostringstream hash;
        for(int i=0; i<DigestLen; ++i)
        {
            hash << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(Digest[i]);
        }

        CSumFrame::StrHash = hash.str();

        CSumFrame::HashEntry->SetValue(hash.str());
        CSumFrame::OpenFileButton->Enable();
        CSumFrame::CalculateHashButton->Enable();
        CSumFrame::CopyButton->Enable();
        CSumFrame::RecalculateButton->Enable();
        CSumFrame::MatchButton->Enable();
        CSumFrame::CheckHashEntry->SetEditable(true);

        int difference = time(0)-start;
        std::string StatusText = "Done!";

        if(difference > 5)
        {
            StatusText += (" Time took ~" + std::to_string(difference) + " seconds.");
        }

        CSumFrame::Status->SetLabel(StatusText);
    }
    else
    {
        wxMessageBox("Choose an algorithm.", "CSum Error", wxOK|wxICON_ERROR);
    }

    return;
}

void Hash::hfsCalculation(std::vector<std::string> PlainStrings)
{
    std::string OutptHash = "";
    
    for(const std::string strs: PlainStrings)
    {
        int algorithmIndex = hfsFrame::AlgorithmChoice->GetSelection();
        unsigned char Digest[64];
        unsigned int Len = 0;

        EVP_MD_CTX* ctx = EVP_MD_CTX_new();

        switch(algorithmIndex)
        {
            case 0:
                EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
                break;
            case 1:
                EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr);
                break;
            case 2:
                EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
                break;
        }

        EVP_DigestUpdate(ctx, strs.data(), strs.size());
        EVP_DigestFinal_ex(ctx, Digest, &Len);
        EVP_MD_CTX_free(ctx);

        std::ostringstream Hash_;
        for(int i=0; i<Len; ++i)
        {
            Hash_ << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(Digest[i]);
        }

        OutptHash += (Hash_.str() + "\n");
    }

    hfsFrame::InputHash->SetValue(OutptHash);
}

std::vector<unsigned char> Hash::Symmetric()
{
    std::string Key = aseFrame::KeyEntry->GetValue().ToStdString();
    std::string CipherArray[3] = {"cbc", "ecb", "ctr"};

    int CipherIndex = aseFrame::CipherChoice->GetSelection();
    int KeySizes[3] = {16, 24, 32};
    int KeySelectionIndex = aseFrame::KeySizeChoice->GetSelection();

    std::string CipherName = "aes-" + std::to_string(KeySizes[KeySelectionIndex] * 8) + "-" + CipherArray[CipherIndex];
    const EVP_CIPHER* Cipher = EVP_CIPHER_fetch(nullptr, CipherName.c_str(), nullptr);
    if(!Cipher)
    {
        wxMessageBox("Invalid cipher mode " + CipherName + ".", "CSum | AES Symmetric Encryption", wxOK|wxICON_ERROR);
        return {};
    }

    if(Key.length() != KeySizes[aseFrame::KeySizeChoice->GetSelection()])
    {
        wxMessageBox("Not a valid AES " + std::to_string(KeySizes[KeySelectionIndex] * 8) + " bits key", "CSum Error", wxOK|wxICON_ERROR);
        return {};
    }

    std::vector<unsigned char> IV(EVP_CIPHER_get_iv_length(Cipher));
    std::string IVstr = aseFrame::IvEntry->GetValue().ToStdString();

    if (CipherArray[CipherIndex] == "ecb" && IVstr.length() > 0)
    {
        wxMessageBox("IV should be empty in ECB Cipher method.", "CSum | AES Symmetric Encryption", wxOK|wxICON_ERROR);
        return {};
    }

    if (IVstr.length() == 16)
    {
        std::transform(IVstr.begin(), IVstr.end(), IV.begin(), [](char c){return static_cast<unsigned char>(c);});
    }
    else if (IVstr.length() == 0)
    {
        RAND_bytes(IV.data(), IV.size());
    }
    else
    {
        wxMessageBox("IV length should be 128 bits (or 16 ASCII characters).", "CSum | AES Symmetric Encryption", wxOK|wxICON_ERROR);
        return {};
    }

    std::vector<unsigned char> AESKey(Key.begin(), Key.end());
    std::vector<unsigned char> result;
    std::ostringstream Hash_;

    if(!aseFrame::PerformWhat->GetSelection())
    {
        result = aesEncrypt(AESKey, IV, Cipher);
    }
    else
    {
        result = aesDecrypt(AESKey, IV, Cipher);
    }

    if (result.size() == 0)
    {
        return {};
    }

    return result;
}

std::vector<unsigned char> aesEncrypt(std::vector<unsigned char> Key, std::vector<unsigned char> IV, const EVP_CIPHER* CipherName)
{
    std::string PlainTextStr = aseFrame::TextEntry->GetValue().ToStdString();
    std::vector <unsigned char> PlainText(PlainTextStr.begin(), PlainTextStr.end());

    int PlainTextLength = 0;
    int CipherTextLength = 0;
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if(!EVP_EncryptInit_ex(ctx, CipherName, nullptr, Key.data(), IV.data()))
    {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    std::vector<unsigned char> CipherText(PlainText.size() + EVP_CIPHER_block_size(CipherName));

    if(!EVP_EncryptUpdate(ctx, CipherText.data(), &PlainTextLength, PlainText.data(), PlainText.size()))
    {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    
    CipherTextLength = PlainTextLength;

    if(!EVP_EncryptFinal_ex(ctx, CipherText.data() + PlainTextLength, &PlainTextLength))
    {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    CipherText.resize(PlainTextLength+CipherTextLength);
    EVP_CIPHER_CTX_free(ctx);

    return CipherText;
}

std::vector<unsigned char> aesDecrypt(std::vector<unsigned char> Key, std::vector<unsigned char> IV, const EVP_CIPHER* CipherName)
{
    std::string CipherTextStr = aseFrame::TextEntry->GetValue().ToStdString();
    std::vector<unsigned char> BinaryCipher;

    if(CipherTextStr.length() % 2 != 0)
    {
        wxMessageBox("Not a valid AES Hexadecimal Cipher Text.", "CSum | AES Symmetric Encryption", wxOK|wxICON_ERROR);
        return {};
    }

    BinaryCipher.reserve(CipherTextStr.length() / 2);
    for(size_t i=0;i<CipherTextStr.length();i+=2)
    {
        unsigned char BinaryByte = static_cast<unsigned char>(std::stoi(CipherTextStr.substr(i, 2), nullptr, 16));
        BinaryCipher.push_back(BinaryByte); 
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int PlainTextLength = 0, TotalPlainTextLength = 0;

    if(!EVP_DecryptInit_ex(ctx, CipherName, nullptr, Key.data(), IV.data()))
    {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    std::vector<unsigned char> PlainText(CipherTextStr.size() + EVP_CIPHER_block_size(CipherName));

    if(!EVP_DecryptUpdate(ctx, PlainText.data(), &PlainTextLength, BinaryCipher.data(), BinaryCipher.size()))
    {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    TotalPlainTextLength += PlainTextLength;

    if(!EVP_DecryptFinal_ex(ctx, PlainText.data() + TotalPlainTextLength, &PlainTextLength))
    {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    TotalPlainTextLength += PlainTextLength;

    PlainText.resize(TotalPlainTextLength);
    EVP_CIPHER_CTX_free(ctx);

    return PlainText;
}
