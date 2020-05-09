/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Util.h
 @brief Utilities
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/17 Created by minseob
 ***********************************************************/

#ifndef _LE_UTIL_H_
#define _LE_UTIL_H_

#include <string.h>

#include <LEIntVector2.h>
#include <LEFloatVector2.h>

#include "Core/Common.h"

namespace LimitEngine {
    class cColorRGBA;
    /* @brief Get size aligned
     * @param size [In] input size
     * @param align [In] alignment boundary
     * @return size aligned
     */
    size_t GetSizeAlign(size_t size, size_t align);
    /* @brief Swap a between b
     * @param a [In/Out] target A
     * @param b [In/Out] target B
     */
    void Swap(float &a, float &b);
    /* @brief Convert text all lowercase
     * @param text [In/Out] Target text
     */
    void LowerCase(char *text);
    /* @brief Convert text all uppercase
     * @param text [In/Out] Target text
     */
    void UpperCase(char *text);
    /* @brief Get BaseFolder from path
     * @param path [In] source path
     * @param basefolderpath [Out] base folder path
     */
    void GetBaseFolderPath(const char *path, char *basefolderpath);
    /* @brief Get filename from path
     * @param path [In] Source path
     * @param containExtension [In] Filename has extension
     * @param filename [Out] File name
     */
    void GetFileName(const char *path, bool containExtension, char *filename);
    /* @brief Get extension word from path
     * @param path [In] Source path
     * @param ext [Out] Extension word
     */
    void GetExtension(const char *path, char *ext);
    /* @brief Get file type from filename
     * @param path [In] File name
     * @param ext [Out] File type
     */
    void GetTypeFromFileName(const char *path, char *type);
    /* @brief Get file format from filename
     * @param path [In] File name
     * @param ext [Out] File format
     */
    void GetFormatFromFileName(const char *path, char *format);
    /* @brief Convert real screen size into virtual size
     * @param p [In] real screen size
     * @return virtual screen size converted p
     */
    LEMath::IntSize ConvertReal2Virtual(const LEMath::IntSize &p);
    /* @brief Convert real screen point into virtual point
     * @param p [In] real screen point
     * @return virtual screen point converted p
     */
    LEMath::FloatPoint ConvertReal2Virtual(const LEMath::FloatPoint &p);
    /* @brief Convert virtual screen size into real size
    * @param p [In] real screen size
    * @return virtual screen size converted p
    */
    LEMath::IntSize ConvertVirtual2Real(const LEMath::IntSize &p);
    /* @brief Convert virtual screen point into real point
    * @param p [In] real screen point
    * @return virtual screen point converted p
    */
    LEMath::FloatPoint ConvertVirtual2Real(const LEMath::FloatPoint &p);
    /* @brief Check that value is nan
     * @param f [In] float value
     * @return true:value is nan, false:value is not nan
     */
    bool IsNan(float f);
    /* @brief Convert float to unsigned char(uint8)
     * @param f [In] float value
     * @return Converted value into uint8
     */
    uint8 FloatToUInt8(float f);

    // STD functions
    template <typename T> struct RemoveReference { typedef T Type; };
    template <typename T> struct RemoveReference<T&> { typedef T Type; };
    template <typename T> struct RemoveReference<T&&> { typedef T Type; };

    template <typename T>
    T&& Forward(typename RemoveReference<T>::Type &Obj)
    {
        return (T&&)Obj;
    }

    template <typename T>
    T&& Forward(typename RemoveReference<T>::Type &&Obj)
    {
        return (T&&)Obj;
    }
}

#endif // _LE_UTIL_H_