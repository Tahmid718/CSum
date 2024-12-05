#include <openssl/evp.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* Version = "1.0";

void HelpText()
{
    printf("CSum v%s. CIL Version.\nUsage: .\\CSum <algorithm> <file path>\nAlgorithms: MD5, SHA1, SHA256/SHA2.\nSwitches (Can be inputted at the end.)\n-o / --output <file path> = Outputs the hash string into a file.\n-m / --match <hash> = Matches calculated hash with the hash given in this switch.\n", Version);
}

const char* Calculate(int* BlockSize, const unsigned char* Buffer, size_t BufferLength, char* HashString)
{
    unsigned char HashOutput[*BlockSize];

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();

    switch(*BlockSize)
    {
        case 16:
            EVP_DigestInit_ex(ctx, EVP_md5(), NULL);
            break;
        case 20:
            EVP_DigestInit_ex(ctx, EVP_sha1(), NULL);
            break;
        case 32:
            EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
            break;
    }

    EVP_DigestUpdate(ctx, Buffer, BufferLength);
    EVP_DigestFinal_ex(ctx, HashOutput, NULL);
    EVP_MD_CTX_free(ctx);

    for(int i=0; i < *BlockSize; ++i)
    {
        sprintf(&HashString[i * 2], "%02x", HashOutput[i]);
    }

    HashString[*BlockSize*2] = '\0';
    
    return HashString;
}

int AParser(const char* Algorithm)
{
    struct {
        const char* AName;
        int Block;

    } Field[] = {
        {"md5", 16}, {"MD5", 16},
        {"sha1", 20}, {"SHA1", 20},
        {"sha2", 32}, {"SHA2", 32},
        {"sha256", 32}, {"SHA256", 32} 
    };

    for(int i=0; i<8; i++)
    {
        if(strcmp(Algorithm, Field[i].AName) == 0)
        {
            return Field[i].Block;
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int Match = 0, opt = 0;
    
    if (argc == 1 || argc == 2)
    {
        HelpText();

        return 0;
    }

    const char* Algorithm = argv[1];
    int BlockSize = AParser(argv[1]);

    if(BlockSize == 0)
    {
        printf("Error: Invalid algorithms.\nMD5, SHA1, SHA2/SHA256. Only these algorithms exist.\n");
        return 0;
    }

    const char* FilePath = argv[2];

    FILE* File_ = fopen(FilePath, "rb");
    if(!File_)
    {
        printf("Error: The file %s doesn't exist.\n", FilePath);
        return 0;
    }

    fseek(File_, 0, SEEK_END);
    long Size_ = ftell(File_);
    rewind(File_);

    unsigned char* BufferBinary = (unsigned char*)malloc(Size_);

    fread(BufferBinary, 1, Size_, File_);
    char AllocatedBlock[BlockSize * 2 + 1];

    const char* HashString = Calculate(&BlockSize, BufferBinary, Size_, AllocatedBlock);

    fclose(File_);
    free(BufferBinary);

    if(argc == 3)
    {
        printf("%s\n", HashString);
        return 0;
    }

    if(strcmp(argv[3], "-m") == 0 || strcmp(argv[3], "--match") == 0)
    {
        if(strcmp(argv[4], HashString) == 0)
        {
            printf("The hash matches.\n");
            return 0;
        }

        printf("The hash doesn't match.\n");
    }

    if (strcmp(argv[3], "-o") == 0 || strcmp(argv[3], "--output") == 0)
    {
        FILE* SaveFile = fopen(argv[4], "w");
        if(!SaveFile)
        {
            printf("Error: Couldn't save into a file.\n");
            return 0;
        }

        fprintf(SaveFile, HashString);
        fclose(SaveFile);
    }

    return 0;
}