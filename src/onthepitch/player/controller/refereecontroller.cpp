// written by bastiaan konings schuiling 2008 - 2015
// this work is public domain. the code is undocumented, scruffy, untested, and
// should generally not be used for anything important. i do not offer support,
// so don't ask. to be used for inspiration :)

#include "refereecontroller.hpp"

#include "../../AIsupport/AIfunctions.hpp"

#include "../../match.hpp"
#include "../playerofficial.hpp"

#include "../../../main.hpp"

RefereeController::RefereeController(Match *match) : IController(match) {}

RefereeController::~RefereeController() {
  if (Verbose())
    printf("exiting refereecontroller.. ");
  if (Verbose())
    printf("done\n");
}

PlayerOfficial *RefereeController::CastPlayer() {
  return static_cast<PlayerOfficial *>(player);
}

void RefereeController::GetForceField(std::vector<ForceSpot> &forceField) {
  {
    ForceSpot forceSpot;
    forceSpot.origin =
        match->GetBall()->GetAveragePosition(2000).Get2D() * 0.6f;
    forceSpot.magnetType = e_MagnetType_Attract;
    forceSpot.decayType = e_DecayType_Constant;
    forceSpot.power = 0.5f;
    forceField.push_back(forceSpot);
  }

  {
    ForceSpot forceSpot;
    forceSpot.origin = match->GetBall()->Predict(200).Get2D();
    forceSpot.magnetType = e_MagnetType_Repel;
    forceSpot.decayType = e_DecayType_Variable;
    forceSpot.power = 0.5f;
    forceSpot.scale = 10.0f;
    forceField.push_back(forceSpot);
  }

  std::vector<Player *> players;
  match->GetActiveTeamPlayers(0, players); // todo: only closest players will do
  match->GetActiveTeamPlayers(1, players);
  for (unsigned int i = 0; i < players.size(); i++) {
    ForceSpot forceSpot;
    forceSpot.origin =
        players.at(i)->GetPosition() + players.at(i)->GetMovement() * 0.4f;
    forceSpot.magnetType = e_MagnetType_Repel;
    forceSpot.decayType = e_DecayType_Variable;
    forceSpot.power = 0.5f;
    forceSpot.scale = 10.0f;
    forceField.push_back(forceSpot);
  }
}

void RefereeController::RequestCommand(PlayerCommandQueue &commandQueue) {

  switch (CastPlayer()->GetOfficialType()) {

  case e_OfficialType_Referee:
    if (match->GetReferee()->GetBuffer().active == true &&
        (match->GetReferee()->GetCurrentFoulType() == 2 ||
         match->GetReferee()->GetCurrentFoulType() == 3) &&
        match->GetReferee()->GetBuffer().prepareTime >
            match->GetActualTime_ms() + 5000) { // FOUL, walk towards offender

      Vector3 desiredPosition =
          match->GetReferee()->GetCurrentFoulPlayer()->GetPosition() +
          (CastPlayer()->GetPosition() -
           match->GetReferee()->GetCurrentFoulPlayer()->GetPosition())
                  .GetNormalized(0) *
              2.0;

      if ((CastPlayer()->GetPosition() - desiredPosition).GetLength() > 2.0) {
        PlayerCommand command;
        command.desiredFunctionType = e_FunctionType_Movement;
        command.useDesiredMovement = true;
        command.useDesiredLookAt = true;
        command.desiredDirection =
            (desiredPosition - CastPlayer()->GetPosition())
                .GetNormalized(CastPlayer()->GetDirectionVec());
        command.desiredVelocityFloat = RangeVelocity(
            (desiredPosition - CastPlayer()->GetPosition()).GetLength() * 1.0f);
        command.desiredLookAt =
            match->GetReferee()->GetCurrentFoulPlayer()->GetPosition();
        commandQueue.push_back(command);
      } else {
        {
          PlayerCommand command;
          command.desiredFunctionType = e_FunctionType_Special;
          command.useDesiredMovement = false;
          command.useDesiredLookAt = false;
          command.useSpecialVar1 = true;
          command.specialVar1 = 3;
          commandQueue.push_back(command);
        }

        {
          PlayerCommand command;
          command.desiredFunctionType = e_FunctionType_Movement;
          command.useDesiredMovement = true;
          command.useDesiredLookAt = true;
          command.desiredDirection =
              (desiredPosition - CastPlayer()->GetPosition())
                  .GetNormalized(CastPlayer()->GetDirectionVec());
          command.desiredVelocityFloat = idleVelocity;
          command.desiredLookAt =
              match->GetReferee()->GetCurrentFoulPlayer()->GetPosition();
          commandQueue.push_back(command);
        }
      }

    } else { // NORMAL

      PlayerCommand command;
      command.desiredFunctionType = e_FunctionType_Movement;
      command.useDesiredMovement = true;
      command.useDesiredLookAt = true;

      std::vector<ForceSpot> forceField;
      GetForceField(forceField);
      Vector3 desiredPosition =
          CastPlayer()->GetPosition() +
          AI_GetForceFieldMovement(forceField, CastPlayer()->GetPosition());

      command.desiredDirection =
          (desiredPosition - CastPlayer()->GetPosition())
              .GetNormalized(CastPlayer()->GetDirectionVec());
      command.desiredVelocityFloat =
          clamp((desiredPosition - CastPlayer()->GetPosition()).GetLength() *
                    distanceToVelocityMultiplier * 0.5f,
                idleVelocity, sprintVelocity); // take it easy, we are the ref
      command.desiredLookAt = match->GetBall()->Predict(60).Get2D();

      commandQueue.push_back(command);
    }
    break;

  case e_OfficialType_Linesman: {
    PlayerCommand command;
    command.desiredFunctionType = e_FunctionType_Movement;
    command.useDesiredMovement = true;
    command.useDesiredLookAt = true;

    float offside;
    Vector3 desiredPosition;
    if (player->GetPosition().coords[1] < 0) {
      offside = AI_GetOffsideLine(match, match->GetMentalImage(0), 1);
      desiredPosition = Vector3(offside, -(pitchHalfH + 0.8f), 0);
    } else {
      offside = AI_GetOffsideLine(match, match->GetMentalImage(0), 0);
      desiredPosition = Vector3(offside, pitchHalfH + 0.8f, 0);
    }

    command.desiredDirection =
        (desiredPosition - CastPlayer()->GetPosition())
            .GetNormalized(CastPlayer()->GetDirectionVec());
    command.desiredVelocityFloat = RangeVelocity(
        (desiredPosition - CastPlayer()->GetPosition()).GetLength() *
        distanceToVelocityMultiplier);
    command.desiredLookAt = Vector3(desiredPosition.coords[0], 0, 0);

    commandQueue.push_back(command);
  } break;
  }
}

void RefereeController::Process() {}

Vector3 RefereeController::GetDirection() { return player->GetDirectionVec(); }

float RefereeController::GetFloatVelocity() {
  return player->GetFloatVelocity();
}

int RefereeController::GetReactionTime_ms() { return 60; }

void RefereeController::Reset() {}
