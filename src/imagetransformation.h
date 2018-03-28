#pragma once


class Image;

class ImageTransformation
{
public:
    virtual ~ImageTransformation() = default;

    virtual void Apply(Image & img) = 0;
    virtual bool IsIdentity() { return true; }
};