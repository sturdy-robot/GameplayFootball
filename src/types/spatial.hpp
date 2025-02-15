// written by bastiaan konings schuiling 2008 - 2014
// this work is public domain. the code is undocumented, scruffy, untested, and
// should generally not be used for anything important. i do not offer support,
// so don't ask. to be used for inspiration :)

#ifndef _HPP_SPATIAL
#define _HPP_SPATIAL

#include "defines.hpp"

#include "types/refcounted.hpp"

#include "base/math/quaternion.hpp"
#include "base/math/vector3.hpp"

#include "types/lockable.hpp"

#include "base/geometry/aabb.hpp"

#include "systems/isystem.hpp"

namespace blunted {

enum e_LocalMode { e_LocalMode_Relative, e_LocalMode_Absolute };

enum e_SpatialDataType {
  e_SpatialDataType_Position,
  e_SpatialDataType_Rotation,
  e_SpatialDataType_Both
};

enum e_Streaming_DataType {
  e_Streaming_DataType_File,
  e_Streaming_DataType_String
};

/// spatial object
/** an object in a scene. responsibilities:
      - stream loading/saving
      - guarantee atomicity
*/

class Spatial : public RefCounted {

public:
  Spatial(const std::string &name);
  virtual ~Spatial();

  Spatial(const Spatial &src);

  virtual void Exit() = 0;

  void SetLocalMode(e_LocalMode localMode);
  bool GetLocalMode();

  void SetName(const std::string &name);
  virtual const std::string GetName() const;

  void SetParent(Spatial *parent);
  Spatial *GetParent() const;

  virtual void SetPosition(const Vector3 &newPosition,
                           bool updateSpatialData = true);
  virtual Vector3 GetPosition() const;

  virtual void SetRotation(const Quaternion &newRotation,
                           bool updateSpatialData = true);
  virtual Quaternion GetRotation() const;

  virtual void SetScale(const Vector3 &newScale);
  virtual Vector3 GetScale() const;

  virtual Vector3 GetDerivedPosition() const;
  virtual Quaternion GetDerivedRotation() const;
  virtual Vector3 GetDerivedScale() const;

  virtual void RecursiveUpdateSpatialData(
      e_SpatialDataType spatialDataType,
      e_SystemType excludeSystem = e_SystemType_None) = 0;

  virtual void InvalidateBoundingVolume();
  virtual void InvalidateSpatialData();

  virtual AABB GetAABB() const;

protected:
  std::string name;

  Spatial *parent;

  mutable boost::mutex spatialMutex;

  Vector3 position;
  Quaternion rotation;
  Vector3 scale;

  // cache
  mutable boost::mutex cacheMutex;
  mutable bool _dirty_DerivedPosition;
  mutable bool _dirty_DerivedRotation;
  mutable bool _dirty_DerivedScale;
  mutable Vector3 _cache_DerivedPosition;
  mutable Quaternion _cache_DerivedRotation;
  mutable Vector3 _cache_DerivedScale;

  e_LocalMode localMode;

  mutable Lockable<AABBCache> aabb;
};

} // namespace blunted

#endif
