// written by bastiaan konings schuiling 2008 - 2015
// this work is public domain. the code is undocumented, scruffy, untested, and
// should generally not be used for anything important. i do not offer support,
// so don't ask. to be used for inspiration :)

#ifndef _HPP_HUMANOID
#define _HPP_HUMANOID

#include "../../../gamedefines.hpp"

#include "humanoidbase.hpp"

using namespace blunted;

class Player;
class Team;

class Humanoid : public HumanoidBase {

public:
  Humanoid(Player *player, boost::intrusive_ptr<Node> humanoidSourceNode,
           boost::intrusive_ptr<Node> fullbodySourceNode,
           std::map<Vector3, Vector3> &colorCoords,
           boost::shared_ptr<AnimCollection> animCollection,
           boost::intrusive_ptr<Node> fullbodyTargetNode,
           boost::intrusive_ptr<Resource<Surface>> kit,
           int bodyUpdatePhaseOffset);
  virtual ~Humanoid();

  Player *CastPlayer() const;

  virtual void Process();

  virtual void CalculateGeomOffsets();

  bool TouchPending() {
    return (currentAnim->frameNum < currentAnim->touchFrame) ? true : false;
  }
  bool TouchAnim() { return (currentAnim->touchFrame != -1) ? true : false; }
  Vector3 GetTouchPos() { return currentAnim->touchPos; }
  int GetTouchFrame() { return currentAnim->touchFrame; }
  int GetCurrentFrame() { return currentAnim->frameNum; }

  void SelectRetainAnim();

  virtual void ResetSituation(const Vector3 &focusPos);

protected:
  float GetHasteFactor(bool considerOpponentProximity = true) const;
  virtual bool
  SelectAnim(const PlayerCommand &command, e_InterruptAnim localInterruptAnim,
             bool preferPassAndShot =
                 false); // returns false on no applicable anim found
  bool NeedTouch(int animID, const PlayerCommand &command);
  float GetBodyBallDistanceAdvantage_deprecated(
      const Animation *anim, const Vector3 &animTouchMovement,
      const Vector3 &touchMovement, const Vector3 &incomingMovement,
      const Vector3 &outgoingMovement, radian outgoingAngle,
      /*const Vector3 &animBallToBall2D, */ const Vector3 &bodyPos,
      const Vector3 &FFO, const Vector3 &animBallPos2D,
      const Vector3 &actualBallPos2D, const Vector3 &ballMovement2D,
      float radiusFactor, float radiusCheatDistance, float decayPow,
      bool debug = false) const;
  float GetBodyBallDistanceAdvantage(
      const Animation *anim, e_FunctionType functionType,
      const Vector3 &animTouchMovement, const Vector3 &touchMovement,
      const Vector3 &incomingMovement, const Vector3 &outgoingMovement,
      radian outgoingAngle,
      /*const Vector3 &animBallToBall2D, */ const Vector3 &bodyPos,
      const Vector3 &FFO, const Vector3 &animBallPos2D,
      const Vector3 &actualBallPos2D, const Vector3 &ballMovement2D,
      float radiusFactor, float radiusCheatDistance, float decayPow,
      bool debug = false) const;
  signed int GetBestCheatableAnimID(
      const DataSet &sortedDataSet, bool useDesiredMovement,
      const Vector3 &desiredDirection, float desiredVelocityFloat,
      bool useDesiredBodyDirection, const Vector3 &desiredBodyDirectionRel,
      std::vector<Vector3> &positions_ret, int &animTouchFrame_ret,
      float &radiusOffset_ret, Vector3 &touchPos_ret,
      Vector3 &fullActionSmuggle_ret, Vector3 &actionSmuggle_ret,
      radian &rotationSmuggle_ret, float hasteFactor,
      e_InterruptAnim localInterruptAnim, bool preferPassAndShot = false) const;
  Vector3 CalculateMovementSmuggle(const Vector3 &desiredDirection,
                                   float desiredVelocityFloat);
  Vector3 GetBestPossibleTouch(const Vector3 &desiredTouch,
                               e_FunctionType functionType);

  Team *team;

  mutable int stat_GetBodyBallDistanceAdvantage_RadiusDeny;
  mutable int stat_GetBodyBallDistanceAdvantage_DistanceDeny;
};

#endif
