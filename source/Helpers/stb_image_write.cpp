/*-----------------------------------------------------------------------*
Project: Recruitment
File: Helpers/stb_image_write.cpp

Copyright (C) 2018 Nintendo, All rights reserved.

These coded instructions, statements, and computer programs contain proprietary
information of Nintendo and/or its licensed developers and are protected by
national and international copyright laws. They may not be disclosed to third
parties or copied or duplicated in any form, in whole or in part, without the
prior written consent of Nintendo.

The content herein is highly confidential and should be handled accordingly.
*-----------------------------------------------------------------------*/

#include <stdio.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#ifdef __GNUC__
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#include "stb_image_write.h"
