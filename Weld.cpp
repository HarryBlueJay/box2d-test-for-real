#include "Weld.h"
#include "BasicIncludes.h"
#include "Casts.h"
#include "Level.h"
extern b2WorldId worldId;
extern std::vector<b2BodyId> objectIds;
extern std::vector<Object*> objectList;
void Weld::start() {
	b2WeldJointDef jointDef = b2DefaultWeldJointDef();
	jointDef.bodyIdA = objectA->bodyId;
	jointDef.bodyIdB = objectIds[objectB];
	jointDef.localAnchorA = b2Body_GetPosition(jointDef.bodyIdA) - b2Body_GetPosition(jointDef.bodyIdB);
	jointDef.collideConnected = false;

	b2Body_SetType(jointDef.bodyIdA, b2_dynamicBody);
	b2JointId joint = b2CreateWeldJoint(worldId, &jointDef);
}
void Weld::update(float deltaTime) {
	Casts::get().move(*objectA->transform, objectA->bodyId);
}