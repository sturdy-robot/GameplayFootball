// written by bastiaan konings schuiling 2008 - 2014
// this work is public domain. the code is undocumented, scruffy, untested, and
// should generally not be used for anything important. i do not offer support,
// so don't ask. to be used for inspiration :)

#ifndef _HPP_DEFINES
#define _HPP_DEFINES

#ifdef WIN32
#include <windows.h>
#endif

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <cmath>
#include <fstream>

#include <algorithm>
#include <deque>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/signals2/slot.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

namespace blunted {

using namespace boost;
typedef float real;

} // namespace blunted

#endif
