#include "Weld.h"
#include "BasicIncludes.h"
extern b2WorldId worldId;
extern std::vector<b2BodyId> objectIds;
void Weld::start() {
	b2WeldJointDef jointDef = {};
	jointDef.bodyIdA = objectIds[objectA];
	jointDef.bodyIdB = objectIds[objectB];
	b2JointId joint = b2CreateWeldJoint(worldId, &jointDef);
}