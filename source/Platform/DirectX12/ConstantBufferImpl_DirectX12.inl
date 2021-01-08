/*********************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file  LE_ConstantBufferImpl_DirectX12.inl
@brief ConstantBuffer Implement (DX12)
@author minseob (leeminseob@outlook.com)
**********************************************************************/
namespace LimitEngine {
class ConstantBufferImpl_DirectX12 : public ConstantBufferImpl
{
public:
    ConstantBufferImpl_DirectX12() {}
    virtual ~ConstantBufferImpl_DirectX12() {}

    virtual void PrepareForDrawing() override
    {
    }

    virtual void Set(uint32 Offset, const void* Data, size_t Size) override
    {
    }

    template<typename T>
    void Set(uint32 Offset, T* Data)
    {
        this->Set(Offset, Data, sizeof(T));
    }
};
} // namespace LimitEngine