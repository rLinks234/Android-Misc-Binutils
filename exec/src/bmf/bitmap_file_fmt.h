struct BmpFileHeader {

    uint16_t bfType;  //specifies the file type
    uint32_t bfSize;  //specifies the size in bytes of the bitmap file
    uint16_t bfReserved1;  //reserved; must be 0
    uint16_t bfReserved2;  //reserved; must be 0
    uint32_t bOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits

} __attribute__((packed));

struct BmpInfoHeader {

    uint32_t biSize;  //specifies the number of bytes required by the struct
    int32_t biWidth;  //specifies width in pixels
    int32_t biHeight;  //species height in pixels
    uint16_t biPlanes; //specifies the number of color planes, must be 1
    uint16_t biBitCount; //specifies the number of bit per pixel
    uint32_t biCompression;//spcifies the type of compression
    uint32_t biSizeImage;  //size of image in bytes
    int32_t biXPelsPerMeter;  //number of pixels per meter in x axis
    int32_t biYPelsPerMeter;  //number of pixels per meter in y axis
    uint32_t biClrUsed;  //number of colors used by th ebitmap
    uint32_t biClrImportant;  //number of colors that are important

} __attribute__((packed));

static uint8_t* loadBitmapFile(char* filename, BmpInfoHeader* bitmapInfoHeader) {

    FILE* filePtr; //our file pointer
    BmpFileHeader bitmapFileHeader; //our bitmap file header

    uint8_t* bitmapImage;  //store image data
    int imageIdx = 0;  //image index counter
    uint8_t tempRGB;  //our swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");

    if (filePtr == nullptr) {
        return nullptr;
    }

    //read the bitmap file header
    fread(&bitmapFileHeader, sizeof(BmpFileHeader), 1, filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType != 0x4D42) {

        fclose(filePtr);
        return nullptr;

    }

    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BmpInfoHeader), 1, filePtr);

    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader.bOffBits, SEEK_SET);

    //allocate enough memory for the bitmap image data
    bitmapImage = (uint8_t*)malloc(bitmapInfoHeader->biSizeImage);

    //verify memory allocation
    if (!bitmapImage) {

        free(bitmapImage);
        fclose(filePtr);

        return nullptr;

    }

    //read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader->biSizeImage, 1, filePtr);

    //make sure bitmap image data was read
    if (bitmapImage == nullptr) {

        fclose(filePtr);
        return nullptr;

    }

    //swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3 ) {

        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;

    }

    //close file and return bitmap image data
    fclose(filePtr);

    return bitmapImage;

}

uint32_t color_map[] = {  0x00000000, 0x00010101, 0x00020202, 0x00030303, 0x00040404, 0x00050505, 0x00060606, 0x00070707, 0x00080808, 0x00090909, 0x000A0A0A, 0x000B0B0B, 0x000C0C0C, 0x000D0D0D, 0x000E0E0E, 0x000F0F0F, 0x00101010, 0x00111111, 0x00121212, 0x00131313, 0x00141414, 0x00151515, 0x00161616, 0x00171717, 0x00181818, 0x00191919, 0x001A1A1A, 0x001B1B1B, 0x001C1C1C, 0x001D1D1D, 0x001E1E1E, 0x001F1F1F, 
0x00202020, 0x00212121, 0x00222222, 0x00232323, 0x00242424, 0x00252525, 0x00262626, 0x00272727, 0x00282828, 0x00292929, 0x002A2A2A, 0x002B2B2B, 0x002C2C2C, 0x002D2D2D, 0x002E2E2E, 0x002F2F2F, 0x00303030, 0x00313131, 0x00323232, 0x00333333, 0x00343434, 0x00353535, 0x00363636, 0x00373737, 0x00383838, 0x00393939, 0x003A3A3A, 0x003B3B3B, 0x003C3C3C, 0x003D3D3D, 0x003E3E3E, 0x003F3F3F, 
0x00404040, 0x00414141, 0x00424242, 0x00434343, 0x00444444, 0x00454545, 0x00464646, 0x00474747, 0x00484848, 0x00494949, 0x004A4A4A, 0x004B4B4B, 0x004C4C4C, 0x004D4D4D, 0x004E4E4E, 0x004F4F4F, 0x00505050, 0x00515151, 0x00525252, 0x00535353, 0x00545454, 0x00555555, 0x00565656, 0x00575757, 0x00585858, 0x00595959, 0x005A5A5A, 0x005B5B5B, 0x005C5C5C, 0x005D5D5D, 0x005E5E5E, 0x005F5F5F, 
0x00606060, 0x00616161, 0x00626262, 0x00636363, 0x00646464, 0x00656565, 0x00666666, 0x00676767, 0x00686868, 0x00696969, 0x006A6A6A, 0x006B6B6B, 0x006C6C6C, 0x006D6D6D, 0x006E6E6E, 0x006F6F6F, 0x00707070, 0x00717171, 0x00727272, 0x00737373, 0x00747474, 0x00757575, 0x00767676, 0x00777777, 0x00787878, 0x00797979, 0x007A7A7A, 0x007B7B7B, 0x007C7C7C, 0x007D7D7D, 0x007E7E7E, 0x007F7F7F, 
0x00808080, 0x00818181, 0x00828282, 0x00838383, 0x00848484, 0x00858585, 0x00868686, 0x00878787, 0x00888888, 0x00898989, 0x008A8A8A, 0x008B8B8B, 0x008C8C8C, 0x008D8D8D, 0x008E8E8E, 0x008F8F8F, 0x00909090, 0x00919191, 0x00929292, 0x00939393, 0x00949494, 0x00959595, 0x00969696, 0x00979797, 0x00989898, 0x00999999, 0x009A9A9A, 0x009B9B9B, 0x009C9C9C, 0x009D9D9D, 0x009E9E9E, 0x009F9F9F, 
0x00A0A0A0, 0x00A1A1A1, 0x00A2A2A2, 0x00A3A3A3, 0x00A4A4A4, 0x00A5A5A5, 0x00A6A6A6, 0x00A7A7A7, 0x00A8A8A8, 0x00A9A9A9, 0x00AAAAAA, 0x00ABABAB, 0x00ACACAC, 0x00ADADAD, 0x00AEAEAE, 0x00AFAFAF, 0x00B0B0B0, 0x00B1B1B1, 0x00B2B2B2, 0x00B3B3B3, 0x00B4B4B4, 0x00B5B5B5, 0x00B6B6B6, 0x00B7B7B7, 0x00B8B8B8, 0x00B9B9B9, 0x00BABABA, 0x00BBBBBB, 0x00BCBCBC, 0x00BDBDBD, 0x00BEBEBE, 0x00BFBFBF, 
0x00C0C0C0, 0x00C1C1C1, 0x00C2C2C2, 0x00C3C3C3, 0x00C4C4C4, 0x00C5C5C5, 0x00C6C6C6, 0x00C7C7C7, 0x00C8C8C8, 0x00C9C9C9, 0x00CACACA, 0x00CBCBCB, 0x00CCCCCC, 0x00CDCDCD, 0x00CECECE, 0x00CFCFCF, 0x00D0D0D0, 0x00D1D1D1, 0x00D2D2D2, 0x00D3D3D3, 0x00D4D4D4, 0x00D5D5D5, 0x00D6D6D6, 0x00D7D7D7, 0x00D8D8D8, 0x00D9D9D9, 0x00DADADA, 0x00DBDBDB, 0x00DCDCDC, 0x00DDDDDD, 0x00DEDEDE, 0x00DFDFDF, 
0x00E0E0E0, 0x00E1E1E1, 0x00E2E2E2, 0x00E3E3E3, 0x00E4E4E4, 0x00E5E5E5, 0x00E6E6E6, 0x00E7E7E7, 0x00E8E8E8, 0x00E9E9E9, 0x00EAEAEA, 0x00EBEBEB, 0x00ECECEC, 0x00EDEDED, 0x00EEEEEE, 0x00EFEFEF, 0x00F0F0F0, 0x00F1F1F1, 0x00F2F2F2, 0x00F3F3F3, 0x00F4F4F4, 0x00F5F5F5, 0x00F6F6F6, 0x00F7F7F7, 0x00F8F8F8, 0x00F9F9F9, 0x00FAFAFA, 0x00FBFBFB, 0x00FCFCFC, 0x00FDFDFD, 0x00FEFEFE, 0x00FFFFFF 
};

static void writeToBitmap(uint8_t* pData, uint32_t pWidth, uint32_t pHeight, const char* pFileName) {

	FILE* filePtr;

    filePtr = fopen(pFileName, "wb+");

    if (filePtr == nullptr) {
    
    	printf("Could not create file %s\n", pFileName);
        return;
    
    }

    const uint32_t pixelCount = pWidth * pHeight;
    const uint32_t bareFileSz = pixelCount + sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) + sizeof(color_map);
    const uint32_t pad4 = bareFileSz % 4;

    BmpFileHeader fHeader;
    BmpInfoHeader fInfo;

    fHeader.bfType = 0x4D42;
    fHeader.bfSize = bareFileSz + pad4;
    fHeader.bfReserved1 = 0;
    fHeader.bfReserved2 = 0;
    fHeader.bOffBits = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) + sizeof(color_map);

    fInfo.biSize = sizeof(BmpInfoHeader);
    fInfo.biPlanes = 1;
    fInfo.biWidth = pWidth;
    fInfo.biHeight = pHeight;
    fInfo.biBitCount = 8;
    fInfo.biCompression = 0;
    fInfo.biSizeImage = pixelCount + pad4;
    fInfo.biXPelsPerMeter = 2834;
    fInfo.biYPelsPerMeter = 2834;
    fInfo.biClrUsed = 0;
    fInfo.biClrImportant = 0;

    fwrite(&fHeader, sizeof(BmpFileHeader), 1, filePtr);
    fwrite(&fInfo, sizeof(BmpInfoHeader), 1, filePtr);
    fwrite(color_map, sizeof(color_map), 1, filePtr);
    fwrite(pData, pixelCount, 1, filePtr);

    uint8_t pad_data[] = { 0, 0, 0, 0 };
    
    if (pad4) {
    	fwrite(pad_data, pad4, 1, filePtr);
    }

    fclose(filePtr);

}