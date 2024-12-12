#include <openssl/evp.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* Version = "1.1";

struct {
    const char* AName;
    int BlockSize;
    int Id;

} Field[] = {
    {"md5", 16, 1}, {"MD5", 16, 1},
    {"sha1", 20, 2}, {"SHA1", 20, 2},
    {"sha224", 28, 3}, {"SHA224", 28, 3},
    {"sha256", 32, 4}, {"SHA256", 32, 4},
    {"sha384", 48, 5}, {"SHA384", 48, 5},
    {"sha512", 64, 6}, {"SHA512", 64, 6},
};

void HelpText()
{
    printf("CSum v%s. CIL Version.\nUsage: .\\CSum <algorithm> <input>\n\nAlgorithms:\nMD5\nSHA1\nSHA224\nSHA256\nSHA384\nSHA512\n\nSwitches:\n-f / --file <path> = Takes contents inside of the file as input.\n-o / --output <path> = Outputs the hash string into a file.\n-m / --match <hash> = Matches calculated hash with the hash given in this switch.\n", Version);
}

char *Calculate(int* BlockSize, const unsigned char* Buffer, size_t BufferLength, char* HashString, int* id_)
{
    unsigned char HashOutput[*BlockSize];

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();

    switch(*id_)
    {
        case 1:
            EVP_DigestInit_ex(ctx, EVP_md5(), NULL);
            break;
        case 2:
            EVP_DigestInit_ex(ctx, EVP_sha1(), NULL);
            break;
        case 3:
            EVP_DigestInit_ex(ctx, EVP_sha224(), NULL);
            break;
        case 4:
            EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
            break;
        case 5:
            EVP_DigestInit_ex(ctx, EVP_sha384(), NULL);
            break;
        case 6:
            EVP_DigestInit_ex(ctx, EVP_sha512(), NULL);
            break;
        default:
            printf("Error: an unknown error. (And a super rare one.)");
    }

    EVP_DigestUpdate(ctx, Buffer, BufferLength);
    EVP_DigestFinal_ex(ctx, HashOutput, NULL);
    EVP_MD_CTX_free(ctx);

    for(int i=0; i < *BlockSize; ++i)
    {
        sprintf(&HashString[i * 2], "%02x", HashOutput[i]);
    }

    HashString[*BlockSize * 2] = '\0';
    
    return HashString;
}

int main(int argc, char* argv[])
{
    if(argc == 1 || argc == 2)
    {
        HelpText();
        return 0;
    }

    char* Algorithm = argv[1];
    int Block = 0, id = 0;
    for(int i=0; i<14; i++)
    {
        if(strcmp(Algorithm, Field[i].AName) == 0)
        {
            Block = Field[i].BlockSize;
            id = Field[i].Id;

            break;
        }
    }

    if (Block == 0)
    {
        HelpText();
        return 0;
    }

    char* InputFile = NULL; 
    char* OutputFile = NULL;
    char* Match = NULL;
    int RawStringMode = 0;

    for(int i=2; i<argc; ++i)
    {
        if(argv[i][0] != '-' || (argv[i][0] != '-' && argv[i][1] != '-'))
        {
            InputFile = argv[i];
            RawStringMode = 1;
        }
        else
        {
            if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0){InputFile = argv[++i];}
            else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--match") == 0){Match = argv[++i];}
            else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0){OutputFile = argv[++i];}
        }
    }

    char* HashString = NULL;

    if (RawStringMode == 0)
    {
        FILE* File_ = fopen(InputFile, "rb");
        if(!File_)
        {
            printf("Error: The file %s doesn't exist.\n", InputFile);
            return 0;
        }

        fseek(File_, 0, SEEK_END);
        long Size_ = ftell(File_);
        rewind(File_);

        unsigned char* BufferBinary = (unsigned char*)malloc(Size_);

        fread(BufferBinary, 1, Size_, File_);
        char AllocatedBlock[Block * 2 + 1];

        HashString = strdup(Calculate(&Block, BufferBinary, Size_, AllocatedBlock, &id));
        
        free(BufferBinary);
        fclose(File_);
    }
    else
    {
        size_t Size_ = strlen(InputFile);
        unsigned char* BufferBinary = (unsigned char*)malloc(Size_+1);

        memcpy(BufferBinary, InputFile, Size_);

        char AllocatedBlock[Block * 2 + 1];
        HashString = strdup(Calculate(&Block, BufferBinary, Size_, AllocatedBlock, &id));

        free(BufferBinary);
    }

    if (Match == NULL)
    {
        printf("%s\n", HashString);
    }
    else
    {
        if(strcmp(HashString, Match) == 0)
        {
            printf("The hash matches.\n");
        }
        else
        {
            printf("The hash doesn't match.\n");
        }
    }

    if (OutputFile != NULL)
    {
        FILE* SaveFile;
        SaveFile = fopen(OutputFile, "w");
        if(!SaveFile){printf("Error: Couldn't Open the file\n");}
        else
        {
            fprintf(SaveFile, HashString);
            fclose(SaveFile);
        }
    }

    free(HashString);
    return 0;
}
