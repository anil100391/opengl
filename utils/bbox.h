#ifndef _bbox_h_
#define _bbox_h_

#include <cfloat>
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class box3
{
public:

    box3() = default;

    box3(const float *min, const float *max)
    {
        _min[0] = min[0]; _min[1] = min[1]; _min[2] = min[2];
        _max[0] = max[0]; _max[1] = max[1]; _max[2] = max[2];
    }

    [[nodiscard]] const float* min() const noexcept
    {
        return _min;
    }

    [[nodiscard]] const float* max() const noexcept
    {
        return _max;
    }

    [[nodiscard]] const float* center() const noexcept
    {
        static float center[3] = {0, 0, 0};
        center[0] = (_min[0] + _max[0]) / 2;
        center[1] = (_min[1] + _max[1]) / 2;
        center[2] = (_min[2] + _max[2]) / 2;
        return center;
    }

    [[nodiscard]] bool contains(float *v) const noexcept
    {
        return (v[0] >= _min[0]) &&
               (v[1] >= _min[1]) &&
               (v[2] >= _min[2]) &&
               (v[0] <  _max[0]) &&
               (v[1] <  _max[1]) &&
               (v[2] <  _max[2]);
    }

    void expand(const float *v) noexcept
    {
        if ( v[0] > _max[0] ) _max[0] = v[0];
        if ( v[1] > _max[1] ) _max[1] = v[1];
        if ( v[2] > _max[2] ) _max[2] = v[2];
        if ( v[0] < _min[0] ) _min[0] = v[0];
        if ( v[1] < _min[1] ) _min[1] = v[1];
        if ( v[2] < _min[2] ) _min[2] = v[2];
    }

private:

    float _min[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
    float _max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
};

#endif // _bbox_h_
