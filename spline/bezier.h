#ifndef __H_VANYKA_BEZIER
#define __H_VANYKA_BEZIER

#include "spline.h"

#define mSupportPoints this->mSupportPoints

namespace vanyka::spline
{

enum class BezierType : char
{
	TWO_CONTROL_POINT,
	ONE_CONTROL_POINT
};

template <class V>
class BezierSpline : public Spline<V>
{
	BezierType mBezierType;

	static V Interpolate3(const float &t, const V &p1, const V &p2, const V &p3);
	static V Interpolate4(const float &t, const V &p1, const V &p2, const V &p3, const V &p4);

	void GeneratePoints3(std::vector<V> &points, const int &res) const;
	void GeneratePoints4(std::vector<V> &points, const int &res) const;
public:
	BezierSpline(BezierType bezierType = BezierType::TWO_CONTROL_POINT) : mBezierType(bezierType) {}
	~BezierSpline() = default;

	BezierType      GetType() const { return mBezierType; }
	std::vector<V>  GeneratePoints(int res = 10) const override;
	V               operator()(const float& t) const override;
};

template <class V>
V BezierSpline<V>::Interpolate3(const float &t, const V &p1, const V &p2, const V &p3)
{
	const float nt = 1.f - t;
	return p1 * (nt * nt) + p2 * (nt * nt * t) + p3 * t;
}

template <class V>
V BezierSpline<V>::Interpolate4(const float &t, const V &p1, const V &p2, const V &p3, const V &p4)
{
	const float nt = 1.f - t;
	return p1 * (nt * nt * nt) + p2 * (3 * nt * nt * t) + p3 * (3 * nt * t * t) + p4 * (t * t * t);
}

template <class V>
V BezierSpline<V>::operator()(const float& t) const {
	size_t stepSize;
	switch (mBezierType) {
	case BezierType::TWO_CONTROL_POINT: stepSize = 2; break;
	case BezierType::ONE_CONTROL_POINT: stepSize = 3; break;
	}

	if (mSupportPoints.size() < stepSize + 1)
		throw "Not enough points";

	const size_t segmentCount = (mSupportPoints.size() - 1) / stepSize;
	float localt; size_t index;
	std::tie(localt, index) = this->CalculateSegmentInfo(t, segmentCount);

	index *= stepSize;

	switch (mBezierType) {
	case BezierType::TWO_CONTROL_POINT: return Interpolate4(localt, mSupportPoints[index], mSupportPoints[index + 1], mSupportPoints[index + 2], mSupportPoints[index + 3]);
	case BezierType::ONE_CONTROL_POINT: return Interpolate3(localt, mSupportPoints[index], mSupportPoints[index + 1], mSupportPoints[index + 2]);
	}
}

template <class V>
inline void BezierSpline<V>::GeneratePoints3(std::vector<V> &points, const int &res) const
{
	points.reserve((mSupportPoints.size() - 1) / 2 * res);
	points.push_back(mSupportPoints[0]);
	for (size_t i = 2; i < mSupportPoints.size(); i += 2)
	{
		const V &v1 = mSupportPoints[i - 2];
		const V &v2 = mSupportPoints[i - 1];
		const V &v3 = mSupportPoints[i];
		for (int j = 1; j < res; ++j)
		{
			const float t = (float)j / (float)res;
			points.push_back(Interpolate3(t, v1, v2, v3));
		}
		points.push_back(v3);
	}
}

template <class V>
inline void BezierSpline<V>::GeneratePoints4(std::vector<V> &points, const int &res) const
{
	points.reserve((mSupportPoints.size() - 1) / 3 * res);
	points.push_back(mSupportPoints[0]);
	for (size_t i = 3; i < mSupportPoints.size(); i += 3)
	{
		const V &v1 = mSupportPoints[i - 3];
		const V &v2 = mSupportPoints[i - 2];
		const V &v3 = mSupportPoints[i - 1];
		const V &v4 = mSupportPoints[i];
		for (int j = 1; j < res; ++j)
		{
			const float t = (float)j / (float)res;
			points.push_back(Interpolate4(t, v1, v2, v3, v4));
		}
		points.push_back(v4);
	}
}

template <class V>
std::vector<V> BezierSpline<V>::GeneratePoints(int res) const
{
	std::vector<V> points;
	if (mSupportPoints.size() < 4)
		// Here Should be a warning that too few support points available
		return points;

	switch (mBezierType)
	{
	case BezierType::ONE_CONTROL_POINT:
		GeneratePoints3(points, res);
		break;
	case BezierType::TWO_CONTROL_POINT:
		GeneratePoints4(points, res);
		break;
	}

	return points;
}

} // vanyka::spline

#undef mSupportPoints

#endif