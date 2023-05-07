/*********************************************\
 * 火山视窗PIV模块 - inja                  *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_INJA_HPP
#define _PIV_INJA_HPP

#include "piv_string.hpp"
#include "inja.hpp"

class PivInja : public CVolObject
{
private:
    inja::Environment env;

public:
    PivInja() {}
    ~PivInja() {}

}; // PivInja

#endif // _PIV_INJA_HPP
