#include <stdio.h>
#include <stddef.h>

int isExe(FILE* f, int* not_exe)
{
    unsigned char format_magic[2];
    int result;

    if (f == NULL)
    {
        printf("Error: NULL file pointer passed to isExe.\n");
        return 1;
    }

    if (not_exe == NULL)
    {
        printf("NULL pointer Error.\n");
        return 2;
    }


    if (fread(format_magic, sizeof(format_magic[0]), sizeof(format_magic)/sizeof(format_magic[0]), f) != sizeof(format_magic)/sizeof(format_magic[0]))
    {
        printf("Error: Failed to read file header.\n");
        return 3;
    }

    if (format_magic[0] != 'M' || format_magic[1] != 'Z')
    {
        *not_exe = 1;
        if (printf("This file is not an EXE.\n") < 0)
        {
            printf("Output Error.\n");
            return 4;
        }
    }
    return 0;
}

int isSizeEnough(FILE* f, long offset, size_t signature_size, int* not_enough)
{
    long size;
    int result;

    if (f == NULL)
    {
        printf("Error: NULL file pointer passed to isSizeEnough.\n");
        return 1;
    }
    if (offset < 0)
    {
        printf("Error: Negative offset passed to isSizeEnough.\n");
        return 2;
    }
    if (signature_size == 0)
    {
        printf("Error: Zero-length signature passed to isSizeEnough.\n");
        return 3;
    }
    if (not_enough == NULL)
    {
        printf("NULL pointer Error\n");
        return 4;
    }
    if (fseek(f, 0, SEEK_END) != 0)
    {
        printf("Error: Failed to seek to end of file.\n");
        return 5;
    }

    size = ftell(f);
    if (size == -1)
    {
        printf("Error: Failed to get file size.\n");
        return 6;
    }

    if (size < (offset + (long)signature_size))
    {
        *not_enough = 1;
        if (printf("File is too small: signature would go out of bounds.\n") < 0)
        {
            printf("Output Error.\n");
            return 7;
        }
    }

    return 0;
}

int main()
{
    char pathchecked[100];
    char infopath[100];
    FILE* checkedFile;
    FILE* infofile;
    unsigned char sign[8];
    unsigned char buffer[8];
    long int offset;
    int result;
    size_t i;
    char signature_name[100];
    int isExeResult;
    int isSizeEnoughResult;
    int not_exe = 0;
    int not_enough = 0;

    if (printf("Enter path to file to check:\n") < 0)
    {
        printf("Output error.\n");
        return 1;
    }
    if (scanf("%s", pathchecked) != 1)
    {
        printf("Error: Failed to read file path.\n");
        return 2;
    }

    if (printf("Enter path to info file:\n") < 0)
    {
        printf("Output error.\n");
        return 1;
    }
    if (scanf("%s", infopath) != 1)
    {
        printf("Error: Failed to read info file path.\n");
        return 2;
    }

    checkedFile = fopen(pathchecked, "rb");
    if (checkedFile == NULL)
    {
        printf("Error opening checked file.\n");
        return 3;
    }

    infofile = fopen(infopath, "r");
    if (infofile == NULL)
    {
        printf("Error opening info file.\n");
        fclose(checkedFile);
        return 3;
    }

    isExeResult = isExe(checkedFile, &not_exe);
    if (isExeResult != 0)
    {
        fclose(checkedFile);
        fclose(infofile);
        switch (isExeResult)
        {
            case 1:
                return 4;
                break;
            case 2:
                return 5;
                break;
            case 3:
                return 6;
                break;
            case 4:
                return 1;
                break;
        }
    }
    if (not_exe)
    {
        result = fclose(infofile);
        if (result != 0)
        {
            printf("Error closing file.\n");
            fclose(checkedFile);
            return 7;
        }
        result = fclose(checkedFile);
        if (result != 0)
        {
            printf("Error closing file.\n");
            return 7;
        }
        return 8;
    }
    result = fscanf(infofile, "%s", signature_name);
    if (result != 1)
    {
        printf("Error: Failed to read signature name from file.\n");
        fclose(checkedFile);
        fclose(infofile);
        return 9;
    }

    result = fscanf(infofile, "%lx", &offset);
    if (result != 1)
    {
        printf("Error: Failed to read offset from file.\n");
        fclose(checkedFile);
        fclose(infofile);
        return 9;
    }
    for(i = 0; i < sizeof(sign); i++)
    {
        result = fscanf(infofile, "%hhx", &sign[i]);
        if (result != 1)
        {
            printf("Error: Failed to read 8-byte signature from file.\n");
            fclose(checkedFile);
            fclose(infofile);
            return 9;
        }
    }

    isSizeEnoughResult = isSizeEnough(checkedFile, offset, sizeof(sign), &not_enough);
    if (isSizeEnoughResult != 0)
    {
        fclose(checkedFile);
        fclose(infofile);
        switch(isSizeEnoughResult)
        {
            case 1:
                return 10;
                break;
            case 2:
                return 11;
                break;
            case 3:
                return 12;
                break;
            case 4:
                return 13;
                break;
            case 5:
                return 14;
                break;
            case 6:
                return 15;
                break;
            case 7:
                return 1;
                break;
        }
    }
    if (not_enough)
    {
        result = fclose(infofile);
        if (result != 0)
        {
            printf("Error closing file.\n");
            fclose(checkedFile);
            return 7;
        }
        result = fclose(checkedFile);
        if (result != 0)
        {
            printf("Error closing file.\n");
            return 7;
        }
        return 8;
    }
    if (fseek(checkedFile, offset, SEEK_SET) != 0)
    {
        printf("Error: Failed to seek to offset.\n");
        fclose(checkedFile);
        fclose(infofile);
        return 14;
    }
    if (fread(buffer, sizeof(buffer[0]), sizeof(buffer)/sizeof(buffer[0]), checkedFile) != sizeof(buffer)/sizeof(buffer[0]))
    {
        printf("Error: Could not read from checked file.\n");
        fclose(checkedFile);
        fclose(infofile);
        return 6;
    }

    for (i = 0; i < (sizeof(buffer)/sizeof(buffer[0])); i++)
    {
        if (buffer[i] != sign[i])
        {
            if (printf("No signature match. You can open your file.\n") < 0)
            {
                printf("Output Error.\n");
                fclose(checkedFile);
                fclose(infofile);
                return 1;
            }
            result = fclose(infofile);
            if (result != 0)
            {
                printf("Error closing file.\n");
                fclose(checkedFile);
                return 7;
            }
            result = fclose(checkedFile);
            if (result != 0)
            {
                printf("Error closing file.\n");
                return 7;
            }
            return 8;
        }
    }

    if (printf("Don't open your file. Signature matched: %s\n", signature_name) < 0)
    {
        printf("Output Error.\n");
        fclose(checkedFile);
        fclose(infofile);
        return 1;
    }
    result = fclose(infofile);
    if (result != 0)
    {
        printf("Error closing file.\n");
        fclose(checkedFile);
        return 7;
    }
    result = fclose(checkedFile);
    if (result != 0)
    {
        printf("Error closing file.\n");
        return 7;
    }
    return 0;
}

